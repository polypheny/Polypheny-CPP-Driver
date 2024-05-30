#ifndef POLYPHENY_CPP_DRIVER_CONNECTION_H
#define POLYPHENY_CPP_DRIVER_CONNECTION_H

#include "ConnectionProperties.h"
#include "../src/PrismInterfaceClient.h"
#include "src/Cursor.h"

namespace Connection {

    class Connection {
    private:
        Communication::PrismInterfaceClient prism_interface_client;
        ConnectionProperties connection_properties;
    public:
        Connection(const ConnectionProperties &connection_properties);

        Connection(const std::string &host, const std::string &user, const std::string &password);

        Communication::PrismInterfaceClient get_prism_interface_client() const;

        ConnectionProperties get_connection_properties() const;

        Cursor get_cursor();

        static ConnectionProperties
        build_connection_properties(const std::string &host, const std::string &username, const std::string &password);
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CONNECTION_H
