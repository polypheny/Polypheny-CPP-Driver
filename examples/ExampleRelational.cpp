#include "PolyphenyCPPDriver.h"

int main() {
    // valid connection parameters to connect to a polypheny instance
    std::string host = "localhost";
    std::string username = "pa";
    std::string password = "";

    // open a connection to the polypheny instance
    Connection::Connection database_connection = Connection::Connection(host, username, password);

    // get a cursor operating on the previously opened connection
    Connection::Cursor cursor(database_connection);

    // execute a statement using this cursor
    std::unique_ptr<Results::Result> result = cursor.execute("sql", "SELECT * FROM emp;", "public");

    // the result can now be processed
    if (result->get_result_type() != Results::ResultType::RELATIONAL) {
        throw std::runtime_error("Received result is of unexpected type");
    }

    auto* relational_result = result->unwrap<Results::RelationalResult>();
    auto metadata = relational_result->get_metadata();
    uint32_t columns_count = metadata->get_column_count();
    for(uint32_t i = 0; i < columns_count; i++) {
        std::cout << metadata->get_column_meta(i).get_column_label() << ", ";
    }
    std::cout << std::endl;
    for (const auto& row : *relational_result) {
        for (const auto& value : row) {
            std::cout << value << ", ";
        }
        std::cout << std::endl;
    }

}