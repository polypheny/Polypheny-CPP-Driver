#include "PolyphenyCPPDriver.h"
#include "transport/UnixTransport.h"
#include <memory>

int main() {
    // for more control over the configuration of the connection a connection properties object can be created
    // this object is initialized with the default values
    Connection::ConnectionProperties properties = Connection::ConnectionProperties();

    // we can now customize these values
    properties.set_username("pa");
    properties.set_password("");
    properties.set_is_auto_commit(true);
    properties.set_default_namespace("public");

    // further we must create a mode of transport to be used by the connection
    std::unique_ptr<Transport::Transport> transport = std::make_unique<Transport::UnixTransport>("/home/tobi/.polypheny/polypheny-prism.sock");

    // the connection properties and the mode of transport can now be used to open connections
    Connection::Connection database_connection(properties, std::move(transport));

    // get a cursor operating on the previously opened connection
    Connection::Cursor cursor(database_connection);

    // execute a statement using this cursor
    std::unique_ptr<Results::Result> result = cursor.execute("sql", "SELECT * FROM emps;", "public");

    // the result can now be processed
    // for further details please refer to the examples "ExampleRelational", "ExampleDocument" and "ExampleGraph"
    std::cout << "Example completed successfully";

    // connection is automatically closed in destructor of the connection
}