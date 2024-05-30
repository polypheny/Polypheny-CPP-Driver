#ifndef POLYPHENY_CPP_DRIVER_CURSOR_H
#define POLYPHENY_CPP_DRIVER_CURSOR_H

#include <string>
#include <vector>
#include <map>
#include "TypedValue.h"
#include "Connection.h"
#include "Result.h"
#include "protointerface.pb.h"
#include "statement_responses.pb.h"

namespace Connection {

    class Cursor {
    private:
        Connection *connection;
        int32_t statement_id;
        bool is_statement_id_set;

        void reset_statement();

        std::function<org::polypheny::prism::StatementResponse &(
                const org::polypheny::prism::Response &)> response_extractor =
                [](const org::polypheny::prism::Response &response) -> org::polypheny::prism::StatementResponse & {
                    return response.statement_response();
                };

    public:
        explicit Cursor(Connection* connection);

        Results::Result execute(const std::string &language, const std::string &statement, const std::string &nspace);

        Results::Result execute(const std::string &language, const std::string &statement);

        Results::Result execute(const std::string &language, const std::string &statement, const std::string &nspace,
                                const std::vector<TypedValues> &parameters);

        Results::Result
        execute(const std::string &language, const std::string &statement, const std::vector<TypedValues> &parameters);

        Results::Result execute(const std::string &language, const std::string &statement, const std::string &nspace,
                                const std::map<std::string, TypedValues> &parameters);

        Results::Result execute(const std::string &language, const std::string &statement,
                                const std::map<std::string, TypedValues> &parameters);

        Cursor(Connection *pConnection);

        Cursor(Connection *pConnection);

        Cursor(Connection *pConnection);

        Results::Result execute(const std::string &language, const std::string &statement, const std::string &nspace,
                                const std::vector<TypedValues> &params);
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_CURSOR_H
