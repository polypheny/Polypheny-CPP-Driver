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
#include "streaming/StreamingIndex.h"

namespace Connection {
    // forward declaration, include in .cpp
    class Connection;


    /**
     * @brief Represents a cursor for managing statements executed on a specific connection to a specific Polypheny instance.
     *
     * This class provides methods for executing statements using multiple datamodels and query languages, their preparation as well as the retrieval of their results.
     * Each cursor is permanently bound to a specific connection to a specific instance of Polypheny. Multiple cursors can be bound to a single connection and used concurrently.
     */
    class Cursor {
    private:
        uint32_t statement_id{};
        bool is_statement_id_set{};
        bool is_prepared{};
        Connection &connection;
        Streaming::StreamingIndex streaming_index;

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

        /**
         * @brief Constructs a Cursor object associated with a specific connection.
         *
         * @param connection The connection to be used by this cursor.
         */
        Cursor(const Cursor &other);

        Cursor &operator=(const Cursor &other);

        ~Cursor();

        /**
         * @brief Executes a statement in a specified language and returns the result.
         *
         * It's the programmers responsibility to dispose of the result object returned by this method after it's no longer in use,
         * Further this cursor should not be used to execute another statement until the result had been properly disposed of. For parallel statements multiple cursors should be used.
         *
         * @param language The language of the statement.
         * @param statement The statement to execute.
         * @param nspace The namespace in which to execute the statement.
         * @return A unique pointer to the result of the execution.
         */
        std::unique_ptr<Results::Result>
        execute(const std::string &language, const std::string &statement, const std::string &nspace);

        /**
        * @brief Executes a statement in a specified language in the default namespace of the underlying connection and returns the result.
        *
        * It's the programmers responsibility to dispose of the result object returned by this method after it's no longer in use,
        * Further this cursor should not be used to execute another statement until the result had been properly disposed of. For parallel statements multiple cursors should be used.
        *
        * @param language The language of the statement.
        * @param statement The statement to execute.
        * @return A unique pointer to the result of the execution.
        */
        std::unique_ptr<Results::Result> execute(const std::string &language, const std::string &statement);

        /**
         * @brief Executes multiple data manipulation statements in a specified language and returns the results.
         *
         * If a non data manipulation statement is amongst the statements an error will be thrown.
         * In contrast to most of the other execution methods, the cursor can directly execute another statement after this call even if the results ares still in use.
         *
         * @param language The language of the statements.
         * @param statements A vector of statements to execute.
         * @param nspace The namespace in which to execute the statements.
         * @return A vector of results from the execution of the statements.
         */
        std::vector<uint64_t> execute(const std::string &language, const std::vector<std::string> &statements, const std::string &nspace);

        /**
         * @brief Executes multiple data manipulation statements in a specified language in the default namespace of the underlying connection and returns the results.
         *
         * If a non data manipulation statement is amongst the statements an error will be thrown.
         * In contrast to most of the other execution methods, the cursor can directly execute another statement after this call even if the results ares still in use.
         *
         * @param language The language of the statements.
         * @param statements A vector of statements to execute.
         * @return A vector of results from the execution of the statements.
         */
        std::vector<uint64_t> execute(const std::string &language, const std::vector<std::string> &statements);

        /**
         * @brief Prepares a statement in a specific language for execution in the default namespace of the underlying connection.
         *
         * There are two supported kinds of parameterization for prepared statements. One is called indexed parametrization and the other is called names parameterization.
         * Indexed parameterization maps the parameters to placeholders inserted into the statement base don their order. Names parameterization uses named placeholders and parameters passed as a map to assign parameters.
         * Indexed parameterization uses "?" at arbitrary places in the statement to denote a placeholder (e.g. "INSERT INTO users (id, name, email) VALUES (?, ?, ?)").
         * Named parameterization would look like this: "INSERT INTO users (id, name, email) VALUES (:id, :name, :email)", where :id, :name and :email are the placeholders.
         * Placeholder names used when specifying the parameters are without hte leading ":".
         *
         * After preparation the statement can be executed using one of the execute_prepared(...) methods.
         *
         * @param language The language of the statement.
         * @param statement The statement to prepare.
         */
        void prepare(const std::string &language, const std::string &statement);

        /**
         * @brief Prepares a statement in a specific language for execution.
         *
         * There are two supported kinds of parameterization for prepared statements. One is called indexed parametrization and the other is called names parameterization.
         * Indexed parameterization maps the parameters to placeholders inserted into the statement base don their order. Names parameterization uses named placeholders and parameters passed as a map to assign parameters.
         * Indexed parameterization uses "?" at arbitrary places in the statement to denote a placeholder (e.g. "INSERT INTO users (id, name, email) VALUES (?, ?, ?)").
         * Named parameterization would look like this: "INSERT INTO users (id, name, email) VALUES (:id, :name, :email)", where :id, :name and :email are the placeholders.
         * Placeholder names used when specifying the parameters are without hte leading ":".
         *
         * After preparation the statement can be executed using one of the execute_prepared(...) methods.
         *
         * @param language The language of the statement.
         * @param statement The statement to prepare.
         * @param nspace The namespace in which to prepare and later execute the statement.
         */
        void prepare(const std::string &language, const std::string &statement, const std::string &nspace);

        /**
         * @brief Executes an indexed parameterized prepared statement with the given parameters and returns the result.
         *
         * Before this method is used, a indexed parameterized statement must first be prepared using the prepare method.
         *
         * It's the programmers responsibility to dispose of the result object returned by this method after it's no longer in use,
         * Further this cursor should not be used to execute another statement until the result had been properly disposed of. For parallel statements multiple cursors should be used.
         *
         * @param params A vector of parameters to pass to the prepared statement.
         * @return A unique pointer to the result of the execution.
         */
        std::unique_ptr<Results::Result> execute_prepared(std::vector<Types::TypedValue> &params);

        /**
         * @brief Executes a named parameterized prepared statement with the given named parameters and returns the result.
         *
         * Before this method is used, a named parameterized statement must first be prepared using the prepare method.
         *
         * It's the programmers responsibility to dispose of the result object returned by this method after it's no longer in use,
         * Further this cursor should not be used to execute another statement until the result had been properly disposed of. For parallel statements multiple cursors should be used.
         *
         * @param params An unordered map of named parameters to pass to the prepared statement.
         * @return A unique pointer to the result of the execution.
         */
        std::unique_ptr<Results::Result> execute_prepared(std::unordered_map<std::string, Types::TypedValue> &params);

        /**
         * @brief Executes an indexed parameterized prepared data manipulation statement with the given parameter batch and returns the result.
         *
         * Before this method is used, an indexed parameterized statement must first be prepared using the prepare method.
         *
         * If a non data manipulation statement is amongst the statements an error will be thrown.
         *
         * The statement will be executed once for each inner vector using it's values as the parameters.
         *
         * In contrast to most of the other execution methods, the cursor can directly execute another statement after this call even if the results ares still in use.
         *
         * @param params A vector of parameters to pass to the prepared statement.
         * @return A unique pointer to the result of the execution.
         */
        std::vector<uint64_t> execute_prepared(std::vector<std::vector<Types::TypedValue>> &params_batch);
    };

} // namespace Connection

#endif // POLYPHENY_CPP_DRIVER_CURSOR_H
