#include "UnixTransport.h"

namespace Transport {

    const std::string VERSION = "unix-v1@polypheny.com\n";

    UnixTransport::UnixTransport(const std::string &path) {
#ifdef _WIN32
        throw std::runtime_error("UnixTransport is not supported on windows.");
#else
        socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (socket_fd == -1) {
            throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
        //TODO: throw error if path does not fit

        if (connect(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
            close_socket();
            throw std::runtime_error("Failed to connect to server: " + std::string(strerror(errno)));
        }

        exchange_version();
#endif
    }

    UnixTransport::~UnixTransport() {
        close_socket();
    }

    std::string UnixTransport::get_version() const {
        return VERSION;
    }

} // namespace Transport
