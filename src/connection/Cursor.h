#ifndef POLYPHENY_CPP_DRIVER_CURSOR_H
#define POLYPHENY_CPP_DRIVER_CURSOR_H

#include <string>
#include <functional>
#include <memory>
#include <stdexcept>
#include "types/TypedValue.h"
#include "protointerface.pb.h"
#include "statement_responses.pb.h"
#include "types/Interval.h"
#include "results/Result.h"
#include "results/RelationalResult.h"
#include "results/DocumentResult.h"
#include "results/GraphResult.h"
#include "results/ScalarResult.h"

namespace Connection {
    // forward declaration, include in .cpp
    class Connection;

    class Cursor {
    private:
        uint32_t statement_id{};
        bool is_statement_id_set{};
        bool is_prepared{};
        Connection &connection;

        void reset_statement();

        std::unique_ptr<Results::Result>
        get_result_from_frame(const org::polypheny::prism::Frame &frame);

    protected:
        const uint32_t DEFAULT_FETCH_SIZE = 100;

        [[nodiscard]] uint32_t get_statement_id() const;

        Connection& get_connection();

        friend class Results::RelationalResult;
        friend class Results::DocumentResult;
        friend class Results::GraphResult;

    public:
        explicit Cursor(Connection &connection);

        Cursor(const Cursor &other);

        Cursor &operator=(const Cursor &other);

        ~Cursor();

        std::unique_ptr<Results::Result>
        execute(const std::string &language, const std::string &statement, const std::string &nspace);

        std::unique_ptr<Results::Result> execute(const std::string &language, const std::string &statement);

        std::vector<uint64_t> execute(const std::string &language, const std::vector<std::string> &statements, const std::string &nspace);

        std::vector<uint64_t> execute(const std::string &language, const std::vector<std::string> &statements);

        void prepare(const std::string &language, const std::string &statement);

        void prepare(const std::string &language, const std::string &statement, const std::string &nspace);

        std::unique_ptr<Results::Result> execute_prepared(const std::vector<Types::TypedValue> &params);

        std::unique_ptr<Results::Result> execute_prepared(const std::unordered_map<std::string, Types::TypedValue> &params);

        std::vector<uint64_t> execute_prepared(const std::vector<std::vector<Types::TypedValue>> &params_batch);
    };

} // namespace Connection

#endif // POLYPHENY_CPP_DRIVER_CURSOR_H
