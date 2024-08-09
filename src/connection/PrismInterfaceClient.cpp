#include "PrismInterfaceClient.h"
#include "protointerface.pb.h"
#include "connection_requests.pb.h"
#include "connection_responses.pb.h"
#include "statement_requests.pb.h"
#include "statement_responses.pb.h"
#include "transport/ConnectionClosedError.h"
#include <stdexcept>
#include <thread>
#include <chrono>

namespace Communication {

    const std::string PrismInterfaceClient::AUTOCOMMIT_PROPERTY_KEY = "autocommit";
    const std::string PrismInterfaceClient::NAMESPACE_PROPERTY_KEY = "namespace";

    PrismInterfaceClient::PrismInterfaceClient(const Connection::ConnectionProperties& connection_properties, std::unique_ptr<Transport::Transport> transport) : transport(std::move(transport)) {
        this->transport->connect();
        response_reader = std::thread(&PrismInterfaceClient::read_responses, this);
        connect(connection_properties);
    }

    PrismInterfaceClient::~PrismInterfaceClient() {
        if (has_sent_disconnect) {
            return;
        }
        disconnect_and_close();
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
        google::protobuf::Map<std::string, std::string>* properties = inner->mutable_properties();
        properties->emplace(AUTOCOMMIT_PROPERTY_KEY, connection_properties.get_is_auto_commit() ? "true" : "false");
        properties->emplace(NAMESPACE_PROPERTY_KEY, connection_properties.get_default_namespace());

        org::polypheny::prism::ConnectionResponse response = complete_synchronously(outer, timeout_millis).connection_response();
        if (!response.is_compatible()) {
            std::string server_api_version =
                    std::to_string(response.major_api_version()) + "." + std::to_string(response.minor_api_version());
            std::string client_api_version =
                    std::to_string(inner->major_api_version()) + "." + std::to_string(inner->minor_api_version());
            throw std::runtime_error(
                    "Incompatible API version. Server expected " + server_api_version + ", client has " + client_api_version);
        }
        return response;
    }


    void PrismInterfaceClient::disconnect_and_close(uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        outer.mutable_disconnect_request();
        has_sent_disconnect = true;
        complete_synchronously(outer, timeout_millis);
        is_closed = true;
        if (response_reader.joinable()) {
            response_reader.join();
        }
    }

    void PrismInterfaceClient::execute_unparameterized_statement(std::string namespace_name, std::string language_name,
                                                                 std::string statement,
                                                                 std::shared_ptr<CallbackQueue> &callback_queue) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteUnparameterizedStatementRequest *inner = outer.mutable_execute_unparameterized_statement_request();
        inner->set_namespace_name(namespace_name);
        inner->set_language_name(language_name);
        inner->set_statement(statement);
        callback_queues.emplace(outer.id(), callback_queue);
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
        callback_queues.emplace(outer.id(), callback_queue);
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
            while (!has_sent_disconnect) {
                org::polypheny::prism::Response response = receive_message();
                if (response.id() == 0) {
                    throw std::runtime_error("Invalid message id");
                }
                { // start locked region
                    std::lock_guard<std::mutex> lock(mutex);
                    auto it = callbacks.find(response.id());
                    if (it != callbacks.end()) {
                        complete_callback_unsafe(response, it->second);
                        continue;
                    }
                    handle_callback_queue_unsafe(response);
                } // end locked region
            }
        } catch (const Errors::ConnectionClosedError &e) {
            handle_connection_closure(std::current_exception());
            if (!has_sent_disconnect) { // this is not a logic error: has_sent_disconnect is modified by another thread!
                std::rethrow_exception(std::current_exception());
            }
        } catch (const std::runtime_error &e) {
            handle_connection_closure(std::current_exception());
            std::rethrow_exception(std::current_exception());
        }
    }

    void PrismInterfaceClient::complete_callback_unsafe(const org::polypheny::prism::Response &response,
                                                        std::promise<org::polypheny::prism::Response> &callback) {
        std::promise<org::polypheny::prism::Response> moved_callback = std::move(callback);
        if (response.last()) {
            callbacks.erase(response.id());
        }
        moved_callback.set_value(response);
    }

    void PrismInterfaceClient::handle_callback_queue_unsafe(const org::polypheny::prism::Response &response) {
        auto it = callback_queues.find(response.id());
        if (it == callback_queues.end()) {
            return;
        }

        auto &callback_queue = it->second;
        if (response.has_error_response()) {
            callback_queues.erase(response.id());
            callback_queue->on_error(
                    std::make_exception_ptr(std::runtime_error(response.error_response().message())));
            return;
        }

        callback_queue->on_next(response);
        if (response.last()) {
            callback_queues.erase(response.id());
            callback_queue->on_completed();
        }
    }

    void PrismInterfaceClient::handle_connection_closure(const std::exception_ptr &exception) {
        std::lock_guard<std::mutex> lock(mutex);
        is_closed = true;
        for (auto &[id, callback]: callbacks) {
            callback.set_exception(std::make_exception_ptr(exception));
        }
        for (auto &[id, callback_queue]: callback_queues) {
            callback_queue->on_error(std::make_exception_ptr(exception));
        }
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

        send_message(request);

        org::polypheny::prism::Response response = wait_for_completion(future, timeout_millis);
        if (response.has_error_response()) {
            throw Errors::ServerError(response.error_response().message());
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
                                                    uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteIndexedStatementRequest *inner = outer.mutable_execute_indexed_statement_request();
        inner->set_statement_id(statement_id);
        inner->set_fetch_size(fetch_size);
        org::polypheny::prism::IndexedParameters *inner_parameters = inner->mutable_parameters();
        google::protobuf::RepeatedPtrField<org::polypheny::prism::ProtoValue> *parameters = inner_parameters->mutable_parameters();
        for (auto &value: values) {
            parameters->AddAllocated(value.serialize());
        }

        //for (auto value : values) {
        //    parameters->AddAllocated(value.serialize().get());
        //}

        return complete_synchronously(outer, timeout_millis).statement_result();
    }

    org::polypheny::prism::StatementResult PrismInterfaceClient::execute_named_statement(const uint32_t &statement_id,
                                                                                         std::unordered_map<std::string, Types::TypedValue> &values,
                                                                                         const uint32_t &fetch_size,
                                                                                         uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteNamedStatementRequest *inner = outer.mutable_execute_named_statement_request();
        inner->set_statement_id(statement_id);
        inner->set_fetch_size(fetch_size);
        org::polypheny::prism::NamedParameters *inner_parameters = inner->mutable_parameters();
        google::protobuf::Map<std::basic_string<char>, org::polypheny::prism::ProtoValue> *parameters = inner_parameters->mutable_parameters();
        for (auto &entry: values) {
            (*parameters)[entry.first] = *entry.second.serialize();
        }
        return complete_synchronously(outer, timeout_millis).statement_result();
    }

    org::polypheny::prism::StatementBatchResponse
    PrismInterfaceClient::execute_indexed_statement_batch(uint32_t &statement_id,
                                                          const std::vector<std::vector<Types::TypedValue>> &params_batch,
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
                inner_parameters->AddAllocated(&(*value.serialize()));
            }
        }
        return complete_synchronously(outer, timeout_millis).statement_batch_response();
    }
} // namespace Communication
