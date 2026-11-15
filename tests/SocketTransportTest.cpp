#include <catch2/catch_all.hpp>
#include "transport/SocketTransport.h"

#ifndef _WIN32
#include <future>

namespace {

    class SocketPairTransport final : public Transport::SocketTransport {
    public:
        explicit SocketPairTransport(int fd) { socket_fd = fd; }
        void connect() override {}

    private:
        [[nodiscard]] std::string get_version() const override { return {}; }
    };

}

TEST_CASE("Socket transport reads a complete frame across short reads") {
    int sockets[2];
    REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == 0);
    SocketPairTransport sender(sockets[0]);
    SocketPairTransport receiver(sockets[1]);
    const int buffer_size = 4096;
    REQUIRE(setsockopt(sockets[0], SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size)) == 0);

    std::string payload(512 * 1024, 'a');
    payload.back() = 'z';
    auto sending = std::async(std::launch::async, [&] {
        try {
            sender.send_message(payload);
        } catch (...) {
            sender.shutdown();
            throw;
        }
    });
    std::string received;
    try {
        received = receiver.receive_message();
    } catch (...) {
        receiver.shutdown();
        throw;
    }
    receiver.shutdown();
    REQUIRE_NOTHROW(sending.get());
    CHECK(received == payload);
}
#endif
