#ifndef POLYPHENY_CPP_DRIVER_TEST_SCRIPTEDTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_TEST_SCRIPTEDTRANSPORT_H

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "org/polypheny/prism/protointerface.pb.h"
#include "transport/Transport.h"

namespace TestSupport {

    class ScriptedTransport final : public Transport::Transport {
    public:
        using Request = org::polypheny::prism::Request;
        using Response = org::polypheny::prism::Response;
        using Handler = std::function<void(const Request &, Response &)>;
        using ReplySequence = std::vector<Response>;
        using SequenceHandler = std::function<ReplySequence(const Request &)>;

        explicit ScriptedTransport(Handler handler, Handler connection_handler = {})
                : handler(std::move(handler)), connection_handler(std::move(connection_handler)) {}

        explicit ScriptedTransport(SequenceHandler sequence_handler)
                : sequence_handler(std::move(sequence_handler)) {}

        void connect() override {}

        void send_message(const std::string &message) override {
            Request request;
            if (!request.ParseFromString(message)) {
                throw std::runtime_error("Invalid request in scripted transport");
            }

            Response response;
            response.set_id(request.id());
            response.set_last(true);
            if (request.has_connection_request()) {
                if (connection_handler) {
                    connection_handler(request, response);
                } else {
                    auto *reply = response.mutable_connection_response();
                    reply->set_major_api_version(request.connection_request().major_api_version());
                    reply->set_minor_api_version(request.connection_request().minor_api_version());
                    reply->set_is_compatible(true);
                }
            } else if (request.has_disconnect_request()) {
                response.mutable_disconnect_response();
            } else if (sequence_handler) {
                deliver_sequence(request.id(), sequence_handler(request));
                return;
            } else {
                handler(request, response);
            }
            {
                std::lock_guard<std::mutex> lock(mutex);
                responses.push(response.SerializeAsString());
            }
            ready.notify_one();
        }

        std::string receive_message() override {
            std::unique_lock<std::mutex> lock(mutex);
            // Re-entering receive_message means the caller has handled the previous response.
            handled_count = received_count;
            drained.notify_all();
            ready.wait(lock, [this] { return closed || !responses.empty(); });
            if (responses.empty()) {
                throw std::runtime_error("Scripted transport is closed");
            }
            auto response = std::move(responses.front());
            responses.pop();
            ++received_count;
            return response;
        }

        void shutdown() noexcept override {
            {
                std::lock_guard<std::mutex> lock(mutex);
                closed = true;
            }
            ready.notify_one();
            drained.notify_all();
        }

    private:
        void deliver_sequence(std::uint64_t request_id, ReplySequence replies) {
            std::unique_lock<std::mutex> lock(mutex);
            for (auto &response : replies) {
                response.set_id(request_id);
                responses.push(response.SerializeAsString());
            }
            const auto target = received_count + responses.size();
            ready.notify_one();
            if (!drained.wait_for(lock, std::chrono::seconds(5), [this, target] {
                return closed || handled_count >= target;
            }) || closed) {
                throw std::runtime_error("Scripted responses were not handled");
            }
        }

        Handler handler;
        Handler connection_handler;
        SequenceHandler sequence_handler;
        std::mutex mutex;
        std::condition_variable ready;
        std::condition_variable drained;
        std::queue<std::string> responses;
        std::size_t received_count = 0;
        std::size_t handled_count = 0;
        bool closed = false;
    };

}

#endif // POLYPHENY_CPP_DRIVER_TEST_SCRIPTEDTRANSPORT_H
