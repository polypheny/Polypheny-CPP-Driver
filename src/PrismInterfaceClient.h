#ifndef POLYPHENY_CPP_DRIVER_PRISMINTERFACECLIENT_H
#define POLYPHENY_CPP_DRIVER_PRISMINTERFACECLIENT_H

#include <atomic>
#include <thread>
#include <map>
#include <cstdint>
#include <future>
#include <mutex>
#include <memory>
#include <iostream>
#include <stdexcept>

#include "CallbackQueue.h"
#include "Transport.h"
#include "PlainTransport.h"
#include "ConnectionProperties.h"
#include "statement_responses.pb.h"
#include "connection_responses.pb.h"
#include "protointerface.pb.h"

namespace Communication {

    class PrismInterfaceClient {
    private:
        static constexpr uint32_t DEFAULT_TIMEOUT_MILLIS = 10000;
        // TODO: send proper api version, polypheny version for testing uses 5.1
        static constexpr int MAJOR_API_VERSION = 5;
        static constexpr int MINOR_API_VERSION = 1;

        std::atomic<long> request_id{1};
        std::unique_ptr<Transport> transport;
        std::thread response_reader;
        bool is_closed = false;
        bool has_sent_disconnect = false;
        std::exception_ptr error = nullptr;
        std::map<uint64_t, std::promise<org::polypheny::prism::Response>> callbacks;
        std::map<uint64_t, std::shared_ptr<CallbackQueue>> callback_queues;
        std::mutex mutex;

        void read_responses();

        void complete_callback_unsafe(const org::polypheny::prism::Response &response,
                                      std::promise<org::polypheny::prism::Response> &callback);

        void handle_callback_queue_unsafe(const org::polypheny::prism::Response &response);

        void handle_connection_closure(const std::exception &exception);

        void handle_unexpected_exception(const std::exception_ptr &exception);

    public:
        explicit PrismInterfaceClient(const Connection::ConnectionProperties &connection_properties);

        ~PrismInterfaceClient();

        org::polypheny::prism::ConnectionResponse
        connect(const Connection::ConnectionProperties &connection_properties, uint32_t timeout_millis);

        void disconnect(uint32_t timeout_millis);

        void
        execute_unparameterized_statement(std::string namespace_name, std::string language_name, std::string statement,
                                          std::shared_ptr<CallbackQueue> callback_queue);

        void commit_transaction(uint32_t timeout_millis);

        void rollback_transaction(uint32_t timeout_millis);

        void close_statement(uint32_t statement_id, uint32_t timeout_millis);

        void close_result(uint32_t statement_id, uint32_t timeout_millis);

        org::polypheny::prism::Frame fetch_result(uint32_t statement_id, uint32_t fetch_size, uint32_t timeout_millis);

        void send_message(const org::polypheny::prism::Request &request);

        org::polypheny::prism::Response receive_message();

        org::polypheny::prism::Response
        wait_for_completion(std::future<org::polypheny::prism::Response> &future, uint32_t timeout_millis);

        org::polypheny::prism::Response
        complete_synchronously(const org::polypheny::prism::Request &request, uint32_t timeout_millis);

        void close();
    };

} // namespace Communication

#endif //POLYPHENY_CPP_DRIVER_PRISMINTERFACECLIENT_H
