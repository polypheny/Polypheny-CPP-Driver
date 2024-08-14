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

    // Insert some entries using an unparameterized statement batch
    std::vector<std::string> insert_statements = {
            "INSERT INTO customers(id, name, year_joined) VALUES (1, 'Maria', 2012)",
            "INSERT INTO customers(id, name, year_joined) VALUES (2, 'Daniel', 2020)",
            "INSERT INTO customers(id, name, year_joined) VALUES (3, 'Peter', 2001)",
    };

    cursor.execute("sql", insert_statements, "public");

    // Insert one more entry using an indexed parameterized statement
    cursor.prepare_indexed("sql", "INSERT INTO customers(id, name, year_joined) VALUES (?, ?, ?)", "public");
    std::vector<Types::TypedValue> parameters = { Types::TypedValue(4), Types::TypedValue("Anna"), Types::TypedValue(2004)};
    cursor.execute_prepared(parameters);

    // Insert some more entries using a batched indexed parameterized statement
    std::vector<std::vector<Types::TypedValue>> parameter_batch = {
            { Types::TypedValue(5), Types::TypedValue("Thomas"), Types::TypedValue(2004)},
            { Types::TypedValue(6), Types::TypedValue("Andreas"), Types::TypedValue(2014)}
    };
    cursor.execute_prepared(parameter_batch);

    // Insert the last entry using a named parameterized statement
    cursor.prepare_named("sql", "INSERT INTO customers(id, name, year_joined) VALUES (:id, :name, :year_joined)", "public");
    std::unordered_map<std::string, Types::TypedValue> named_parameters;
    named_parameters["id"] = Types::TypedValue(7);
    named_parameters["name"] = Types::TypedValue("Michael");
    named_parameters["year_joined"] = Types::TypedValue(2010);
    cursor.execute_prepared(named_parameters);

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