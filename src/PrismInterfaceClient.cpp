#include "PrismInterfaceClient.h"
#include "protointerface.pb.h"
#include "connection_requests.pb.h"
#include "connection_responses.pb.h"
#include "statement_requests.pb.h"
#include "statement_responses.pb.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include <chrono>

namespace Communication {

    const std::string PlainTransport::VERSION = "plain-v1@polypheny.com";

    PrismInterfaceClient::PrismInterfaceClient(const Connection::ConnectionProperties &connection_properties) {
        transport = std::make_unique<PlainTransport>(connection_properties.get_host(),
                                                     connection_properties.get_port());
        response_reader = std::thread(&PrismInterfaceClient::read_responses, this);
        connect(connection_properties, DEFAULT_TIMEOUT_MILLIS);
    }

    PrismInterfaceClient::~PrismInterfaceClient() {
        is_closed = true;
        if (response_reader.joinable()) {
            response_reader.join();
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
        org::polypheny::prism::ConnectionProperties *properties_message = inner->mutable_connection_properties();
        properties_message->set_namespace_name(connection_properties.get_default_namespace());
        properties_message->set_is_auto_commit(connection_properties.get_is_auto_commit());
        org::polypheny::prism::ConnectionResponse response = complete_synchronously(outer,
                                                                                    timeout_millis).connection_response();
        if (!response.is_compatible()) {
            std::string server_api_version =
                    std::to_string(response.major_api_version()) + "." + std::to_string(response.minor_api_version());
            std::string client_api_version =
                    std::to_string(inner->major_api_version()) + "." + std::to_string(inner->minor_api_version());
            throw std::runtime_error(
                    "Incompatible API version. Server expected " + server_api_version + ", client has " +
                    client_api_version);
        }
        return response;
    }

    void PrismInterfaceClient::disconnect(uint32_t timeout_millis) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        outer.mutable_disconnect_request();
        complete_synchronously(outer, timeout_millis);
        close();
    }

    void PrismInterfaceClient::execute_unparameterized_statement(std::string namespace_name, std::string language_name,
                                                                 std::string statement,
                                                                 CallbackQueue<org::polypheny::prism::StatementResponse> &callback_queue) {
        org::polypheny::prism::Request outer;
        outer.set_id(request_id.fetch_add(1));
        org::polypheny::prism::ExecuteUnparameterizedStatementRequest *inner = outer.mutable_execute_unparameterized_statement_request();
        inner->set_namespace_name(namespace_name);
        inner->set_language_name(language_name);
        inner->set_statement(statement);
        callback_queues.emplace(outer.id(), std::move(callback_queue));
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
    PrismInterfaceClient::fetch_result(uint32_t statement_id, uint32_t fetch_size, uint32_t timeout_millis) {
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
            while (true) {
                org::polypheny::prism::Response response = receive_message();
                if (response.id() == 0) {
                    throw std::runtime_error("Invalid message id");
                }

                auto it = callbacks.find(response.id());
                if (it != callbacks.end()) {
                    complete_callback(response, it->second);
                    continue;
                }

                handle_callback_queue(response);
            }
        } catch (const boost::system::system_error &e) {
            if (e.code() == boost::asio::error::eof || e.code() == boost::asio::error::connection_reset) {
                handle_connection_closure(e);
            } else {
                handle_unexpected_exception(std::make_exception_ptr(e));
            }
        } catch (...) {
            handle_unexpected_exception(std::current_exception());
        }
    }

    void PrismInterfaceClient::complete_callback(const org::polypheny::prism::Response &response,
                                                 std::promise<org::polypheny::prism::Response> &callback) {
        if (response.last()) {
            callbacks.erase(response.id());
        }
        callback.set_value(response);
    }

    void PrismInterfaceClient::handle_callback_queue(const org::polypheny::prism::Response &response) {
        auto it = callback_queues.find(response.id());
        if (it == callback_queues.end()) {
            return;
        }

        auto &callback_queue = it->second;
        if (response.has_error_response()) {
            callback_queues.erase(response.id());
            callback_queue.on_error(std::make_exception_ptr(std::runtime_error(response.error_response().message())));
            return;
        }

        callback_queue.on_next(response);
        if (response.last()) {
            callback_queues.erase(response.id());
            callback_queue.on_completed();
        }
    }

    void PrismInterfaceClient::handle_connection_closure(const std::exception &exception) {
        std::lock_guard<std::mutex> lock(mutex);
        is_closed = true;
        for (auto &[id, callback]: callbacks) {
            callback.set_exception(std::make_exception_ptr(exception));
        }
        for (auto &[id, callback_queue]: callback_queues) {
            callback_queue.on_error(std::make_exception_ptr(exception));
        }
    }

    void PrismInterfaceClient::handle_unexpected_exception(const std::exception_ptr &exception) {
        std::lock_guard<std::mutex> lock(mutex);
        is_closed = true;
        for (auto &[id, callback]: callbacks) {
            callback.set_exception(exception);
        }
        for (auto &[id, callback_queue]: callback_queues) {
            callback_queue.on_error(exception);
        }
        std::rethrow_exception(exception);
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

        if (request.type_case() == org::polypheny::prism::Request::TypeCase::kDisconnectRequest) {
            has_sent_disconnect = true;
        }

        send_message(request);

        org::polypheny::prism::Response response = wait_for_completion(future, timeout_millis);
        if (response.has_error_response()) {
            throw std::runtime_error(response.error_response().message());
        }
        return response;
    }

    void PrismInterfaceClient::close() {
        is_closed = true;
        //TODO: is joinable blocking? If yes, code gets stuck here!
        if (response_reader.joinable()) {
            response_reader.join();
        }
    }

} // namespace Communication
