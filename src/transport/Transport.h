#ifndef POLYPHENY_CPP_DRIVER_TRANSPORT_H
#define POLYPHENY_CPP_DRIVER_TRANSPORT_H

#include <string>

namespace Communication {

    class Transport {
    public:
        virtual ~Transport() = default;

        virtual void send_message(const std::string& message) = 0;

        virtual std::string receive_message() = 0;
    };

}

#endif // POLYPHENY_CPP_DRIVER_TRANSPORT_H
