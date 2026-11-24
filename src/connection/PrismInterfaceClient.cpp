#include "PrismInterfaceClient.h"

#include <stdexcept>
#include <thread>
#include <chrono>

#include "types/TypedValue.h"
#include "transport/ConnectionClosedError.h"
#include "types/Interval.h"

namespace {

    Errors::ServerError decode_server_error(const org::polypheny::prism::ErrorResponse &response) {
#if POLYPHENY_PRISM_STREAMING
        return Errors::ServerError(response.message());
#else
        const auto &details = response.details();
        return Errors::ServerError(response.message(),
                                   details.has_state() ? std::make_optional(details.state()) : std::nullopt,
                                   details.has_error_code() ? std::make_optional(details.error_code()) : std::nullopt);
#endif
    }

}

namespace Communication {

#if POLYPHENY_PRISM_STREAMING
    const std::string PrismInterfaceClient::AUTOCOMMIT_PROPERTY_KEY = "autocommit";
    const std::string PrismInterfaceClient::NAMESPACE_PROPERTY_KEY = "namespace";
    const std::string PrismInterfaceClient::SERVER_STREAMING_FEATURE = "client_streaming";
    const std::string PrismInterfaceClient::CLIENT_STREAMING_FEATURE = "server_streaming";
#endif


    PrismInterfaceClient::PrismInterfaceClient(const Connection::ConnectionProperties &connection_properties,
                                               std::unique_ptr<Transport::Transport> transport) : transport(
            std::move(transport)) {
        this->transport->connect();
        response_reader = std::thread(&PrismInterfaceClient::read_responses, this);
        try {
            connect(connection_properties);
        } catch (...) {
            close_transport();
            throw;
        }
    }

    PrismInterfaceClient::~PrismInterfaceClient() {
        try {
            disconnect_and_close();
        } catch (const std::exception &) {
            // Destruction must finish even if the final request failed.
        }
    }

    org::polypheny::prism::ConnectionResponse
    PrismInterfaceClient::connect(const Connection::ConnectionProperties &connection_properties,
                                  uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ConnectionRequest *inner = outer.mutable_connection_request();
        inner->set_major_api_version(MAJOR_API_VERSION);
        inner->set_minor_api_version(MINOR_API_VERSION);
        inner->set_username(connection_properties.get_username());
        if (connection_properties.get_is_password_required()) {
            inner->set_password(connection_properties.get_password());
        }

#if POLYPHENY_PRISM_STREAMING
        auto *features = inner->mutable_features();
        features->Add(std::string(CLIENT_STREAMING_FEATURE));
        features->Add(std::string(SERVER_STREAMING_FEATURE));

        google::protobuf::Map<std::string, std::string> *properties = inner->mutable_properties();
        properties->emplace(AUTOCOMMIT_PROPERTY_KEY, connection_properties.get_is_auto_commit() ? "true" : "false");
        properties->emplace(NAMESPACE_PROPERTY_KEY, connection_properties.get_default_namespace());
#else
        auto *properties = inner->mutable_connection_properties();
        properties->set_is_auto_commit(connection_properties.get_is_auto_commit());
        properties->set_namespace_name(connection_properties.get_default_namespace());
#endif

        auto reply = complete_synchronously(outer, timeout_millis);
        if (!reply.has_connection_response()) {
            throw std::runtime_error("Prism did not return a connection response");
        }
        auto response = reply.connection_response();
        if (!response.is_compatible()) {
            std::string server_api_version =
                    std::to_string(response.major_api_version()) + "." + std::to_string(response.minor_api_version());
            std::string client_api_version =
                    std::to_string(inner->major_api_version()) + "." + std::to_string(inner->minor_api_version());
            throw std::runtime_error(
                    "Incompatible API version. Server expected " + server_api_version + ", client has " +
                    client_api_version);
        }
        server_api_version = {response.major_api_version(), response.minor_api_version()};
        return response;
    }

    std::pair<int, int> PrismInterfaceClient::get_server_api_version() const noexcept {
        return server_api_version;
    }

    void PrismInterfaceClient::set_client_info_properties(const std::map<std::string, std::string> &properties,
                                                          uint32_t timeout_millis) {
        org::polypheny::prism::Request request;
        request.set_id(request_id.fetch_add(1));
        auto *values = request.mutable_set_client_info_properties_request()->mutable_properties();
        for (const auto &[name, value] : properties) {
            (*values)[name] = value;
        }
        auto response = complete_synchronously(request, timeout_millis);
        if (!response.has_set_client_info_properties_response()) {
            throw std::runtime_error("Prism did not confirm client information properties");
        }
    }

    org::polypheny::prism::EntitiesResponse
    PrismInterfaceClient::search_entities(const std::string &namespace_name,
                                           const std::optional<std::string> &pattern, uint32_t timeout_millis) {
        org::polypheny::prism::Request request;
        request.set_id(request_id.fetch_add(1));
        auto *entities = request.mutable_entities_request();
        entities->set_namespace_name(namespace_name);
        if (pattern) {
            entities->set_entity_pattern(*pattern);
        }
        auto response = complete_synchronously(request, timeout_millis);
        if (!response.has_entities_response()) {
            throw std::runtime_error("Prism did not return an entities response");
        }
        return response.entities_response();
    }

    void PrismInterfaceClient::disconnect_and_close(uint32_t timeout_millis) {
        if (has_sent_disconnect.exchange(true)) {
            return;
        }
        try {
            if (!is_closed) {
                org::polypheny::prism::Request outer;
                outer.set_id(request_id.fetch_add(1));
                outer.mutable_disconnect_request();
                complete_synchronously(outer, timeout_millis);
            }
        } catch (...) {
            close_transport();
            throw;
        }
        close_transport();
    }

    void PrismInterfaceClient::shutdown_transport() noexcept {
        if (!is_closed.exchange(true)) {
            transport->shutdown();
        }
    }

    void PrismInterfaceClient::close_transport() {
        shutdown_transport();
        if (response_reader.joinable()) {
            response_reader.join();
        }
    }

    void PrismInterfaceClient::execute_unparameterized_statement(std::string namespace_name, std::string language_name,
                                                                 std::string statement,
                                                                 std::shared_ptr<CallbackQueue> &callback_queue,
                                                                 uint32_t fetch_size) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteUnparameterizedStatementRequest *inner = outer.mutable_execute_unparameterized_statement_request();
        inner->set_namespace_name(namespace_name);
        inner->set_language_name(language_name);
        inner->set_statement(statement);
        inner->set_fetch_size(fetch_size);
        {
            std::lock_guard<std::mutex> lock(mutex);
            callback_queues.emplace(outer.id(), callback_queue);
        }
        send_message(outer);
    }

    void
    PrismInterfaceClient::execute_unparameterized_statement_batch(std::string namespace_name, std::string language_name,
                                                                  const std::vector<std::string> &statements,
                                                                  const std::shared_ptr<CallbackQueue> &callback_queue) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteUnparameterizedStatementBatchRequest *inner = outer.mutable_execute_unparameterized_statement_batch_request();
        google::protobuf::RepeatedPtrField<org::polypheny::prism::ExecuteUnparameterizedStatementRequest> *inner_statements = inner->mutable_statements();
        for (auto statement: statements) {
            org::polypheny::prism::ExecuteUnparameterizedStatementRequest *inner_statement = inner_statements->Add();
            inner_statement->set_namespace_name(namespace_name);
            inner_statement->set_language_name(language_name);
            inner_statement->set_statement(statement);
        }
        {
            std::lock_guard<std::mutex> lock(mutex);
            callback_queues.emplace(outer.id(), callback_queue);
        }
        send_message(outer);
    }


    void PrismInterfaceClient::commit_transaction(uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        outer.mutable_commit_request();
        complete_synchronously(outer, timeout_millis);
    }

    void PrismInterfaceClient::rollback_transaction(uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        outer.mutable_rollback_request();
        complete_synchronously(outer, timeout_millis);
    }

    void PrismInterfaceClient::close_statement(uint32_t statement_id, uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::CloseStatementRequest *inner = outer.mutable_close_statement_request();
        inner->set_statement_id(statement_id);
        complete_synchronously(outer, timeout_millis);
    }

    void PrismInterfaceClient::close_result(uint32_t statement_id, uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::CloseResultRequest *inner = outer.mutable_close_result_request();
        inner->set_statement_id(statement_id);
        complete_synchronously(outer, timeout_millis);
    }

    org::polypheny::prism::Frame
    PrismInterfaceClient::fetch_result(uint32_t &statement_id, const uint32_t &fetch_size, uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::FetchRequest *inner = outer.mutable_fetch_request();
        inner->set_statement_id(statement_id);
        inner->set_fetch_size(fetch_size);
        return complete_synchronously(outer, timeout_millis).frame();
    }

    void PrismInterfaceClient::send_message(const org::polypheny::prism::Request &request) {
        if (is_closed) {
            throw std::runtime_error("Connection is closed");
        }
        std::string serialized_message;
        if (!request.SerializeToString(&serialized_message)) {
            throw std::runtime_error("Failed to serialize message");
        }
        transport->send_message(serialized_message);
    }

    org::polypheny::prism::Response PrismInterfaceClient::receive_message() {
        org::polypheny::prism::Response response;
        if (!response.ParseFromString(transport->receive_message())) {
            throw std::runtime_error("Failed to parse message");
        }
        return response;
    }

    void PrismInterfaceClient::read_responses() {
        try {
            while (!is_closed) {
                org::polypheny::prism::Response response = receive_message();
                if (response.id() == 0) {
                    throw std::runtime_error("Invalid message id");
                }
                if (response.has_error_response() && !response.last()) {
                    throw std::runtime_error("Non-terminal Prism error response");
                }
                { // start locked region
                    std::lock_guard<std::mutex> lock(mutex);
                    auto it = callbacks.find(response.id());
                    if (it != callbacks.end()) {
                        complete_callback_unsafe(response, it->second);
                    } else {
                        handle_callback_queue_unsafe(response);
                    }
                } // end locked region
                if (response.has_disconnect_response()) {
                    return;
                }
            }
        } catch (...) {
            handle_connection_closure(std::current_exception());
        }
    }

    void PrismInterfaceClient::complete_callback_unsafe(const org::polypheny::prism::Response &response,
                                                        std::promise<org::polypheny::prism::Response> &callback) {
        std::promise<org::polypheny::prism::Response> moved_callback = std::move(callback);
        callbacks.erase(response.id());
        moved_callback.set_value(response);
    }

    void PrismInterfaceClient::handle_callback_queue_unsafe(const org::polypheny::prism::Response &response) {
        auto it = callback_queues.find(response.id());
        if (it == callback_queues.end()) {
            return;
        }

        auto callback_queue = it->second;
        if (response.has_error_response()) {
            callback_queue->on_error(std::make_exception_ptr(decode_server_error(response.error_response())));
            callback_queues.erase(it);
            return;
        }

        callback_queue->on_next(response);
        if (response.last()) {
            callback_queues.erase(response.id());
            callback_queue->on_completed();
        }
    }

    void PrismInterfaceClient::handle_connection_closure(const std::exception_ptr &exception) {
        shutdown_transport();
        std::lock_guard<std::mutex> lock(mutex);
        for (auto &[id, callback]: callbacks) {
            callback.set_exception(exception);
        }
        for (auto &[id, callback_queue]: callback_queues) {
            callback_queue->on_error(exception);
        }
        callbacks.clear();
        callback_queues.clear();
    }

    org::polypheny::prism::Response
    PrismInterfaceClient::wait_for_completion(std::future<org::polypheny::prism::Response> &future,
                                              uint32_t timeout_millis) {
        if (timeout_millis == 0) {
            return future.get();
        } else {
            if (future.wait_for(std::chrono::milliseconds(timeout_millis)) == std::future_status::timeout) {
                throw std::runtime_error("Timeout while waiting for response");
            }
            return future.get();
        }
    }

    org::polypheny::prism::Response
    PrismInterfaceClient::complete_synchronously(const org::polypheny::prism::Request &request,
                                                 uint32_t timeout_millis) {
        std::promise<org::polypheny::prism::Response> promise;
        std::future<org::polypheny::prism::Response> future = promise.get_future();
        {
            std::lock_guard<std::mutex> lock(mutex);
            callbacks[request.id()] = std::move(promise);
        }

        org::polypheny::prism::Response response;
        try {
            send_message(request);
            response = wait_for_completion(future, timeout_millis);
        } catch (...) {
            std::lock_guard<std::mutex> lock(mutex);
            callbacks.erase(request.id());
            throw;
        }
        if (response.has_error_response()) {
            throw decode_server_error(response.error_response());
        }
        return response;
    }

    org::polypheny::prism::PreparedStatementSignature
    PrismInterfaceClient::prepare_indexed_statement(const std::string &namespace_name, const std::string &language_name,
                                                    const std::string &statement, uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::PrepareStatementRequest *inner = outer.mutable_prepare_indexed_statement_request();
        inner->set_namespace_name(namespace_name);
        inner->set_language_name(language_name);
        inner->set_statement(statement);
        return complete_synchronously(outer, timeout_millis).prepared_statement_signature();
    }

    org::polypheny::prism::PreparedStatementSignature
    PrismInterfaceClient::prepare_named_statement(const std::string &namespace_name, const std::string &language_name,
                                                  const std::string &statement, uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::PrepareStatementRequest *inner = outer.mutable_prepare_named_statement_request();
        inner->set_namespace_name(namespace_name);
        inner->set_language_name(language_name);
        inner->set_statement(statement);
        return complete_synchronously(outer, timeout_millis).prepared_statement_signature();
    }

    org::polypheny::prism::StatementResult
    PrismInterfaceClient::execute_indexed_statement(const uint32_t &statement_id,
                                                    std::vector<Types::TypedValue> &values, const uint32_t &fetch_size,
                                                    std::shared_ptr<Streaming::StreamingIndex> streaming_index,
                                                    uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteIndexedStatementRequest *inner = outer.mutable_execute_indexed_statement_request();
        inner->set_statement_id(statement_id);
        inner->set_fetch_size(fetch_size);
        org::polypheny::prism::IndexedParameters *inner_parameters = inner->mutable_parameters();
        google::protobuf::RepeatedPtrField<org::polypheny::prism::ProtoValue> *parameters = inner_parameters->mutable_parameters();
        for (auto &value: values) {
            parameters->AddAllocated(value.serialize(streaming_index));
        }
        return complete_synchronously(outer, timeout_millis).statement_result();
    }

    org::polypheny::prism::StatementResult PrismInterfaceClient::execute_named_statement(const uint32_t &statement_id,
                                                                                         std::unordered_map<std::string, Types::TypedValue> &values,
                                                                                         const uint32_t &fetch_size,
                                                                                         std::shared_ptr<Streaming::StreamingIndex> streaming_index,
                                                                                         uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteNamedStatementRequest *inner = outer.mutable_execute_named_statement_request();
        inner->set_statement_id(statement_id);
        inner->set_fetch_size(fetch_size);
        org::polypheny::prism::NamedParameters *inner_parameters = inner->mutable_parameters();
        google::protobuf::Map<std::basic_string<char>, org::polypheny::prism::ProtoValue> *parameters = inner_parameters->mutable_parameters();
        for (auto &entry: values) {
            std::unique_ptr<org::polypheny::prism::ProtoValue> parameter(entry.second.serialize(streaming_index));
            (*parameters)[entry.first] = *parameter;
        }
        return complete_synchronously(outer, timeout_millis).statement_result();
    }

    org::polypheny::prism::StatementBatchResponse
    PrismInterfaceClient::execute_indexed_statement_batch(uint32_t &statement_id,
                                                          const std::vector<std::vector<Types::TypedValue>> &params_batch,
                                                          std::shared_ptr<Streaming::StreamingIndex> streaming_index,
                                                          uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteIndexedStatementBatchRequest *inner = outer.mutable_execute_indexed_statement_batch_request();
        inner->set_statement_id(statement_id);
        google::protobuf::RepeatedPtrField<org::polypheny::prism::IndexedParameters> *inner_parameter_batch = inner->mutable_parameters();
        for (const auto &param_batch: params_batch) {
            org::polypheny::prism::IndexedParameters *indexed_param = inner_parameter_batch->Add();
            google::protobuf::RepeatedPtrField<org::polypheny::prism::ProtoValue> *inner_parameters = indexed_param->mutable_parameters();
            for (auto value: param_batch) {
                inner_parameters->AddAllocated(&(*value.serialize(streaming_index)));
            }
        }
        return complete_synchronously(outer, timeout_millis).statement_batch_response();
    }

#if POLYPHENY_PRISM_STREAMING
    org::polypheny::prism::StreamFrame
    PrismInterfaceClient::fetch_stream(uint32_t statement_id, uint64_t stream_id, uint64_t position, int32_t length,
                                       uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::StreamFetchRequest *inner = outer.mutable_stream_fetch_request();
        inner->set_statement_id(statement_id);
        inner->set_stream_id(stream_id);
        inner->set_position(position);
        inner->set_length(length);
        return complete_synchronously(outer, timeout_millis).stream_frame();
    }

    org::polypheny::prism::StreamAcknowledgement
    PrismInterfaceClient::stream_binary(const std::vector<uint8_t> bytes, bool is_last, uint32_t statement_id,
                                        uint64_t stream_id, uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::StreamSendRequest *inner = outer.mutable_stream_send_request();
        inner->set_statement_id(statement_id);
        inner->set_stream_id(stream_id);
        org::polypheny::prism::StreamFrame *frame = inner->mutable_frame();
        frame->set_binary(Utils::ProtoUtils::vector_to_string(bytes));
        frame->set_is_last(is_last);
        return complete_synchronously(outer, timeout_millis).stream_acknowledgement();
    }

    org::polypheny::prism::StreamAcknowledgement
    PrismInterfaceClient::stream_string(const std::string substring, bool is_last, uint32_t statement_id,
                                        uint64_t stream_id, uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::StreamSendRequest *inner = outer.mutable_stream_send_request();
        inner->set_statement_id(statement_id);
        inner->set_stream_id(stream_id);
        org::polypheny::prism::StreamFrame *frame = inner->mutable_frame();
        frame->set_string(substring);
        frame->set_is_last(is_last);
        return complete_synchronously(outer, timeout_millis).stream_acknowledgement();
    }
#endif
} // namespace Communication
