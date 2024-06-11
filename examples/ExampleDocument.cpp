#include "PolyphenyCPPDriver.h"

void print_document(const std::unordered_map<std::string, Types::TypedValue> &document) {
    for (const auto &entry : document) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
}

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
    std::unique_ptr<Results::Result> result = cursor.execute("mongo", "db.emp.find()", "public");

    // The result can now be processed
    if (result->get_result_type() != Results::ResultType::DOCUMENT) {
        throw std::runtime_error("Received result is of unexpected type");
    }

    auto* document_result = result->unwrap<Results::DocumentResult>();
    std::cout << "---------------------------" << std::endl;
    for (const auto &document : *document_result) {
        print_document(document);
        std::cout << "---------------------------" << std::endl;
    }

    return 0;
}