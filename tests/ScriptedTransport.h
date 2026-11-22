#ifndef POLYPHENY_CPP_DRIVER_TEST_SCRIPTEDTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_TEST_SCRIPTEDTRANSPORT_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <utility>

#include "org/polypheny/prism/protointerface.pb.h"
#include "transport/Transport.h"

namespace TestSupport {

    class ScriptedTransport final : public Transport::Transport {
    public:
        using Request = org::polypheny::prism::Request;
        using Response = org::polypheny::prism::Response;
        using Handler = std::function<void(const Request &, Response &)>;

        explicit ScriptedTransport(Handler handler, Handler connection_handler = {})
                : handler(std::move(handler)), connection_handler(std::move(connection_handler)) {}

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
            ready.wait(lock, [this] { return closed || !responses.empty(); });
            if (responses.empty()) {
                throw std::runtime_error("Scripted transport is closed");
            }
            auto response = std::move(responses.front());
            responses.pop();
            return response;
        }

        void shutdown() noexcept override {
            {
                std::lock_guard<std::mutex> lock(mutex);
                closed = true;
            }
            ready.notify_one();
        }

    private:
        Handler handler;
        Handler connection_handler;
        std::mutex mutex;
        std::condition_variable ready;
        std::queue<std::string> responses;
        bool closed = false;
    };

}

#endif // POLYPHENY_CPP_DRIVER_TEST_SCRIPTEDTRANSPORT_H
