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

#include "transport/Transport.h"
#include "connection/CallbackQueue.h"
#include "transport/PlainTcpTransport.h"
#include "connection/ServerError.h"
#include "connection/ConnectionProperties.h"
#include "statement_responses.pb.h"
#include "connection_responses.pb.h"
#include "protointerface.pb.h"
#include "types/TypedValue.h"

namespace Communication {

    class PrismInterfaceClient {
    private:
        static constexpr uint32_t DEFAULT_TIMEOUT_MILLIS = 10000;
        // TODO: send proper api version, polypheny version for testing uses 5.1
        static constexpr int MAJOR_API_VERSION = 5;
        static constexpr int MINOR_API_VERSION = 1;

        std::atomic<long> request_id{1};
        std::unique_ptr<Transport::Transport> transport;
        std::thread response_reader;
        volatile bool is_closed = false;
        volatile bool has_sent_disconnect = false;
        std::exception_ptr error = nullptr;
        std::map<uint64_t, std::promise<org::polypheny::prism::Response>> callbacks;
        std::map<uint64_t, std::shared_ptr<CallbackQueue>> callback_queues;
        std::mutex mutex;

        void read_responses();

        void complete_callback_unsafe(const org::polypheny::prism::Response &response,
                                      std::promise<org::polypheny::prism::Response> &callback);

        void handle_callback_queue_unsafe(const org::polypheny::prism::Response &response);

        void handle_connection_closure(const std::exception_ptr &exception);

    public:
        explicit PrismInterfaceClient(const Connection::ConnectionProperties& connection_properties, std::unique_ptr<Transport::Transport> transport);

        ~PrismInterfaceClient();

        org::polypheny::prism::ConnectionResponse
        connect(const Connection::ConnectionProperties &connection_properties,
                uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void disconnect_and_close(uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void
        execute_unparameterized_statement(std::string namespace_name, std::string language_name, std::string statement,
                                          std::shared_ptr<CallbackQueue> &callback_queue);

        void
        execute_unparameterized_statement_batch(std::string namespace_name, std::string language_name,
                                                const std::vector<std::string> &statements,
                                                const std::shared_ptr<CallbackQueue> &callback_queue);

        org::polypheny::prism::PreparedStatementSignature
        prepare_indexed_statement(const std::string &namespace_name, const std::string &language_name, const std::string &statement,
                                  uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::PreparedStatementSignature
        prepare_named_statement(const std::string &namespace_name, const std::string &language_name, const std::string &statement,
                                  uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::StatementResult
        execute_indexed_statement(const uint32_t &statement_id, std::vector<Types::TypedValue> &values, const uint32_t &fetch_size,
                                  uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::StatementBatchResponse
        execute_indexed_statement_batch(uint32_t &statement_id,
                                        const std::vector<std::vector<Types::TypedValue>> &params_batch,
                                        uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::StatementResult
        execute_named_statement(const uint32_t &statement_id, std::unordered_map<std::string, Types::TypedValue> &parameters, const uint32_t &fetch_size,
                                uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void commit_transaction(uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void rollback_transaction(uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void close_statement(uint32_t statement_id, uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void close_result(uint32_t statement_id, uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::Frame
        fetch_result(uint32_t &statement_id, const uint32_t &fetch_size, uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void send_message(const org::polypheny::prism::Request &request);

        org::polypheny::prism::Response receive_message();

        org::polypheny::prism::Response
        wait_for_completion(std::future<org::polypheny::prism::Response> &future,
                            uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::Response
        complete_synchronously(const org::polypheny::prism::Request &request,
                               uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void close();
    };

} // namespace Communication

#endif //POLYPHENY_CPP_DRIVER_PRISMINTERFACECLIENT_H
