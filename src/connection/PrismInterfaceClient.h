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

#include "org/polypheny/prism/statement_responses.pb.h"
#include "org/polypheny/prism/connection_responses.pb.h"
#include "org/polypheny/prism/protointerface.pb.h"
#include "org/polypheny/prism/stream_requests.pb.h"
#include "org/polypheny/prism/stream_responses.pb.h"

namespace Types {
    class TypedValue;
}

namespace Streaming {
    class StreamingIndex;
}

namespace Communication {

    class PrismInterfaceClient {
    private:
        static constexpr uint32_t DEFAULT_TIMEOUT_MILLIS = 10000;
        static constexpr int MAJOR_API_VERSION = 1;
        static constexpr int MINOR_API_VERSION = 10;

        static const std::string AUTOCOMMIT_PROPERTY_KEY;
        static const std::string NAMESPACE_PROPERTY_KEY;
        static const std::string SERVER_STREAMING_FEATURE;
        static const std::string CLIENT_STREAMING_FEATURE;

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
        explicit PrismInterfaceClient(const Connection::ConnectionProperties &connection_properties,
                                      std::unique_ptr<Transport::Transport> transport);

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
        prepare_indexed_statement(const std::string &namespace_name, const std::string &language_name,
                                  const std::string &statement,
                                  uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::PreparedStatementSignature
        prepare_named_statement(const std::string &namespace_name, const std::string &language_name,
                                const std::string &statement,
                                uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::StatementResult
        execute_indexed_statement(const uint32_t &statement_id, std::vector<Types::TypedValue> &values,
                                  const uint32_t &fetch_size, std::shared_ptr<Streaming::StreamingIndex> streaming_index,
                                  uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::StatementBatchResponse
        execute_indexed_statement_batch(uint32_t &statement_id,
                                        const std::vector<std::vector<Types::TypedValue>> &params_batch,
                                        std::shared_ptr<Streaming::StreamingIndex> streaming_index,
                                        uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::StatementResult
        execute_named_statement(const uint32_t &statement_id,
                                std::unordered_map<std::string, Types::TypedValue> &parameters,
                                const uint32_t &fetch_size, std::shared_ptr<Streaming::StreamingIndex> streaming_index,
                                uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void commit_transaction(uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void rollback_transaction(uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void close_statement(uint32_t statement_id, uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        void close_result(uint32_t statement_id, uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::Frame
        fetch_result(uint32_t &statement_id, const uint32_t &fetch_size,
                     uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::StreamFrame fetch_stream(
                uint32_t statement_id, uint64_t stream_id,
                uint64_t position, int32_t length,
                uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS
        );

        org::polypheny::prism::StreamAcknowledgement stream_binary(
                std::vector<uint8_t> bytes, bool is_last, uint32_t statement_id,
                uint64_t stream_id, uint32_t timeout_millis = 0
        );

        org::polypheny::prism::StreamAcknowledgement stream_string(
                std::string substring, bool is_last, uint32_t statement_id,
                uint64_t stream_id, uint32_t timeout_millis = 0);

        void send_message(const org::polypheny::prism::Request &request);

        org::polypheny::prism::Response receive_message();

        org::polypheny::prism::Response
        wait_for_completion(std::future<org::polypheny::prism::Response> &future,
                            uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);

        org::polypheny::prism::Response
        complete_synchronously(const org::polypheny::prism::Request &request,
                               uint32_t timeout_millis = DEFAULT_TIMEOUT_MILLIS);
    };

} // namespace Communication

#endif //POLYPHENY_CPP_DRIVER_PRISMINTERFACECLIENT_H
