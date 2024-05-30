#ifndef POLYPHENY_CPP_DRIVER_CONNECTION_H
#define POLYPHENY_CPP_DRIVER_CONNECTION_H

#include "ConnectionProperties.h"
#include "../src/PrismInterfaceClient.h"

namespace Connection {

    class Connection {
    private:
        Communication::PrismInterfaceClient prism_interface_client;
    public:
        Connection(const ConnectionProperties &connection_properties);

        Connection(const std::string &host, const std::string &user, const std::string &password);

        Cursor get_cursor();
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CONNECTION_H
