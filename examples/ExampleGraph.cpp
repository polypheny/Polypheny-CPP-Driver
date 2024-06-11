#include "PolyphenyCPPDriver.h"

int main() {
    // Valid connection parameters to connect to a polypheny instance
    std::string host = "localhost";
    std::string username = "pa";
    std::string password = "";

    // Open a connection to the polypheny instance
    Connection::Connection database_connection(host, username, password);

    // Get a cursor operating on the previously opened connection
    Connection::Cursor cursor(database_connection);

    // Execute a statement using this cursor
    cursor.execute("sql", "DROP TABLE IF EXISTS customers", "public");
    cursor.execute("sql",
                   "CREATE TABLE customers("
                   "id INTEGER PRIMARY KEY, "
                   "name TEXT NOT NULL, "
                   "year_joined INTEGER NOT NULL"
                   ")", "public");

    //TODO: replace with batch execute once available
    std::vector<std::string> insert_statements = {
            "INSERT INTO customers(id, name, year_joined) VALUES (1, 'Maria', 2012)",
            "INSERT INTO customers(id, name, year_joined) VALUES (2, 'Daniel', 2020)",
            "INSERT INTO customers(id, name, year_joined) VALUES (3, 'Peter', 2001)",
            "INSERT INTO customers(id, name, year_joined) VALUES (4, 'Anna', 2001)",
            "INSERT INTO customers(id, name, year_joined) VALUES (5, 'Thomas', 2004)",
            "INSERT INTO customers(id, name, year_joined) VALUES (6, 'Andreas', 2014)",
            "INSERT INTO customers(id, name, year_joined) VALUES (7, 'Michael', 2010)"
    };

    for (const auto& stmt : insert_statements) {
        cursor.execute("sql", stmt, "public");
    }

    std::unique_ptr<Results::Result> result = cursor.execute(
            "cypher",
            "MATCH (c:customers) RETURN c",
            "public"
    );

    // The result can now be processed
    if (result->get_result_type() != Results::ResultType::GRAPH) {
        throw std::runtime_error("Received result is of unexpected type");
    }

    auto* graph_result = result->unwrap<Results::GraphResult>();
    std::cout << "---------------------------" << std::endl;
    for (const auto &graph_element : *graph_result) {
        for (const auto &property : *graph_element) {
            std::cout << property.first << ": " << property.second << std::endl;
        }
        std::cout << "---------------------------" << std::endl;
    }

    return 0;
}