//
// Created by tobi on 17.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_SOCKETTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_SOCKETTRANSPORT_H

#include "Transport.h"

#include <string>
#include <vector>
#include <mutex>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#endif

namespace Transport {

    class SocketTransport : public Transport {
    public:
        ~SocketTransport() override;
        void send_message(const std::string &message) override;
        std::string receive_message() override;
        void shutdown() noexcept override;

    protected:
        int socket_fd = -1;
        std::mutex write_mutex;
        void exchange_version();
        [[nodiscard]] virtual std::string get_version() const = 0;
        void close_socket();

    private:
        void send_all(const char *data, size_t size);
        void receive_all(char *data, size_t size);
    };

} // namespace Transport

#endif //POLYPHENY_CPP_DRIVER_SOCKETTRANSPORT_H
