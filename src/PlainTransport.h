#ifndef POLYPHENY_CPP_DRIVER_PLAINTRANSPORT_H
#define POLYPHENY_CPP_DRIVER_PLAINTRANSPORT_H

#include "Transport.h"
#include "TransportUtils.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <ConnectionClosedError.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/tcp.h>
#endif

namespace Communication {

    class PlainTransport : public Transport {
    public:
        PlainTransport(const std::string &host, uint16_t port);
        ~PlainTransport();
        void send_message(const std::string &message);
        std::string receive_message();

    private:
        void exchange_version();
        void close_socket();

        int socket_fd;
        std::mutex write_mutex;
        static const std::string VERSION;  // Declaration of VERSION
    };

}

#endif // POLYPHENY_CPP_DRIVER_PLAINTRANSPORT_H
