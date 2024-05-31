#include "Cursor.h"
#include <iostream>

namespace Connection {

    // Private method to reset the statement ID status
    void Cursor::reset_statement() {
        if (is_statement_id_set) {
            is_statement_id_set = false;
        }
    }

    // Static method to retrieve the result from a frame
    Results::Result &Cursor::get_result_from_frame(org::polypheny::prism::Frame &frame) {
        // Implement the method to extract the result from the frame
        // This is a placeholder implementation
        static Results::Result result;
        return result;
    }

    // Public method to execute a statement with a namespace
    Results::Result &
    Cursor::execute(const std::string &language, const std::string &statement, const std::string &nspace) {
        reset_statement();
        Communication::CallbackQueue<org::polypheny::prism::StatementResponse> callback(response_extractor);
        connection->get_prism_interface_client().execute_unparameterized_statement(nspace, language, statement,
                                                                                   callback);

        while (true) {
            org::polypheny::prism::StatementResponse statement_response = callback.take_next();

            if (!is_statement_id_set) {
                statement_id = statement_response.statement_id();
                is_statement_id_set = true;
            }

            if (!statement_response.has_result()) {
                continue;
            }

            callback.await_completion();

            if (!statement_response.result().has_frame()) {
                // Assuming ScalarResult is a class that converts scalar response to a Result object
                return ScalarResult(statement_response.result().scalar());
            }

            return get_result_from_frame(statement_response.result().frame());
        }
    }

    Results::Result &Cursor::execute(const std::string &language, const std::string &statement) {
        const std::string &nspace = connection->get_connection_properties().get_default_namespace();
        return execute(language, statement, nspace);
    }

    Cursor::Cursor(Connection &connection) : statement_id(0), is_statement_id_set(false), connection(connection),
                                             response_extractor(
                                                     [](const org::polypheny::prism::Response &response) -> org::polypheny::prism::StatementResponse & {
                                                         return response.statement_response();
                                                     }) {}

} // namespace Connection
