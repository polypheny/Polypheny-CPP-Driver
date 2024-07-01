//
// Created by tobi on 30.05.24.
//

#ifndef POLYPHENY_CPP_DRIVER_RESULT_H
#define POLYPHENY_CPP_DRIVER_RESULT_H

#include <stdexcept>
#include <string>
#include <typeinfo>

namespace Results {

    enum class ResultType {
        RELATIONAL,
        DOCUMENT,
        GRAPH,
        SCALAR
    };

    /**
     * @brief Base class for all result types from database queries.
     *
     * This class is the base for all results that can be returned by a statement execution. It offers a method to unwrap it into one of the specific result types RELATIONAL, DOCUMENT, GRAPH and SCALAR.
     */
    class Result {
    private:
        ResultType resultType;

    protected:
        explicit Result(ResultType resultType) : resultType(resultType) {}

        const uint32_t DEFAULT_FETCH_SIZE = 100;
    public:
        virtual ~Result() = default;

        /**
         * @brief Gets the type of the result.
         *
         * The output is on eof the values of the ResulType enum namely RELATIONAL, DOCUMENT, GRAPH or SCALAR.
         * This information is used to unwrap thi Result into one of the classes ScalarResult, RelationalResult, GraphResult or DocumentResult.
         *
         * @return The result type.
         */
        [[nodiscard]] ResultType get_result_type() const {
            return resultType;
        }

        /**
         * @brief Casts the result to a specific derived types ScalarResult, RelationalResult, GraphResult or DocumentResult.
         *
         * @tparam T The type to cast to.
         * @return A pointer to the casted type.
         * @throws std::runtime_error if the cast is not valid. This is the case if the results is not of the type it should be casted to.
         */
        template<typename T>
        T *unwrap() {
            T *derived = dynamic_cast<T *>(this);
            if (derived) {
                return derived;
            } else {
                throw std::runtime_error("Not a wrapper for " + std::string(typeid(T).name()));
            }
        }
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_RESULT_H
