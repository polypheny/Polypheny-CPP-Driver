#include "Connection.h"
#include "Cursor.h"

namespace Connection {

    Connection::Connection(ConnectionProperties &connection_properties)
            : prism_interface_client(connection_properties, std::make_unique<Transport::PlainTCPTransport>()) {
    }

    Connection::Connection(ConnectionProperties &connection_properties, std::unique_ptr<Transport::Transport> &&transport)
            : prism_interface_client(connection_properties, std::move(transport)) {
    }

    Connection::Connection(const std::string &host, const std::string &user, const std::string &password)
            : prism_interface_client(build_connection_properties(user, password), std::make_unique<Transport::PlainTCPTransport>(host)) {
    }

    Connection::~Connection() {
        prism_interface_client.disconnect_and_close();
    }


    ConnectionProperties Connection::build_connection_properties(const std::string &username,
                                                                 const std::string &password) {
        ConnectionProperties connection_properties;
        connection_properties.set_username(username);
        connection_properties.set_password(password);
        return connection_properties;
    }

    Communication::PrismInterfaceClient &Connection::get_prism_interface_client() {
        return prism_interface_client;
    }

    const ConnectionProperties &Connection::get_connection_properties() const {
        return connection_properties;
    }
}