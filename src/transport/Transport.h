#ifndef POLYPHENY_CPP_DRIVER_TRANSPORT_H
#define POLYPHENY_CPP_DRIVER_TRANSPORT_H

#include <string>

namespace Transport {

    class Transport {
    public:
        virtual ~Transport() = default;

        virtual void send_message(const std::string& message) = 0;

        virtual std::string receive_message() = 0;

        virtual void connect() = 0;

        /**
         * Interrupt a pending receive when closing the connection.
         * Custom transports whose receive_message() can block must override this
         * method to unblock that call.
         */
        virtual void shutdown() noexcept {}
    };

}

#endif // POLYPHENY_CPP_DRIVER_TRANSPORT_H
