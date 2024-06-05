#include "PolyphenyCPPDriver.h"
#include <memory>

int main() {
    // valid connection parameters to connect to a polypheny instance
    std::string host = "localhost";
    std::string username = "pa";
    std::string password = "pa";

    // open a connection to the polypheny instance
    Connection::Connection database_connection = Connection::Connection(host, username, password);

    // get a cursor operating on the previously opened connection
    Connection::Cursor cursor(database_connection);

    // execute a statement using this cursor
    std::unique_ptr<Results::Result> result = cursor.execute("sql", "SELECT * FROM emps;", "public");

    // the result can now be processed
    // for further details please refer to the examples "ExampleRelational", "ExampleDocument" and "ExampleGraph"

    // TODO: close connection

}