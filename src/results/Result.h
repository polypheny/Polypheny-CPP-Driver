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

    class Result {
    private:
        ResultType resultType;

    protected:
        explicit Result(ResultType resultType) : resultType(resultType) {}

        const uint32_t DEFAULT_FETCH_SIZE = 100;
    public:
        virtual ~Result() = default

        [[nodiscard]] ResultType get_result_type() const {
            return resultType;
        }

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
