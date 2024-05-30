#include "Connection.h"

namespace Connection {

    Connection::Connection(const ConnectionProperties &connection_properties)
            : prism_interface_client(connection_properties) {
    }

    Connection::Connection(const std::string &host, const std::string &user, const std::string &password) {
        ConnectionProperties connection_properties = ConnectionProperties();
        connection_properties.set_host(host);
        connection_properties.set_username(user);
        connection_properties.set_password(password);
        *this = Connection(connection_properties);
    }

    Cursor Connection::get_cursor() {
        // Return a cursor object
        return Cursor();
    }

}