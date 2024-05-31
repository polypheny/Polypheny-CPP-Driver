#ifndef POLYPHENY_CPP_DRIVER_CURSOR_H
#define POLYPHENY_CPP_DRIVER_CURSOR_H

#include <string>
#include <functional>
#include "TypedValue.h"
#include "Connection.h"
#include "Result.h"
#include "protointerface.pb.h"
#include "statement_responses.pb.h"

namespace Connection {

    class Cursor {
    private:
        int32_t statement_id;
        bool is_statement_id_set;
        Connection& connection;

        void reset_statement();

        static Results::Result &get_result_from_frame(org::polypheny::prism::Frame &frame);

        std::function<org::polypheny::prism::StatementResponse &(
                const org::polypheny::prism::Response &)> response_extractor;

    public:
        Cursor(Connection& connection);

        Results::Result &execute(const std::string &language, const std::string &statement, const std::string &nspace);

        Results::Result &execute(const std::string &language, const std::string &statement);
    };

} // namespace Connection

#endif // POLYPHENY_CPP_DRIVER_CURSOR_H
