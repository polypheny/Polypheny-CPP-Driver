#include "Connection.h"

namespace Connection {

    Connection::Connection(const ConnectionProperties &connection_properties)
            : prism_interface_client(connection_properties) {
    }

    Connection::Connection(const std::string &host, const std::string &user, const std::string &password)
            : prism_interface_client(build_connection_properties(host, user, password)) {
    }


    ConnectionProperties Connection::build_connection_properties(const std::string &host, const std::string &username,
                                                                 const std::string &password) {
        ConnectionProperties connection_properties;
        connection_properties.set_host(host);
        connection_properties.set_username(username);
        connection_properties.set_password(password);
        return connection_properties;
    }


    Cursor Connection::get_cursor() {
        return Cursor(this);
    }

    Communication::PrismInterfaceClient Connection::get_prism_interface_client() const {
        return prism_interface_client;
    }

    ConnectionProperties Connection::get_connection_properties() const {
        return connection_properties;
    }
}