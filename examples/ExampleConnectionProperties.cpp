#include "PolyphenyCPPDriver.h"
#include <memory>

int main() {
    // for more control over the configuration of the connection a connection properties object can be created
    // this object is initialized with the default values
    Connection::ConnectionProperties properties = Connection::ConnectionProperties();

    // we can now customize these values
    properties.set_username("custom_user");
    properties.set_password("strong_password");
    properties.set_host("127.0.0.1");
    properties.set_port(7987);
    properties.set_is_auto_commit(false);
    properties.set_default_namespace("custom_namespace");

    // the connection properties can now be used to open arbitrarily many connections
    Connection::Connection database_connection(properties);

    // get a cursor operating on the previously opened connection
    Connection::Cursor cursor(database_connection);

    // execute a statement using this cursor
    std::unique_ptr<Results::Result> result = cursor.execute("sql", "SELECT * FROM emps;", "public");

    // the result can now be processed
    // for further details please refer to the examples "ExampleRelational", "ExampleDocument" and "ExampleGraph"

    // TODO: close connection

}