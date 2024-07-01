#ifndef POLYPHENY_CPP_DRIVER_TYPEDVALUE_H
#define POLYPHENY_CPP_DRIVER_TYPEDVALUE_H

#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <list>
#include <unordered_map>
#include <gmpxx.h>
#include <chrono>
#include "utils/ProtoUtils.h"
#include "value.pb.h"

// Forward declarations to avoid cyclic dependencies
namespace Types {
    class Document;
    class Interval;
    class TypedValue;

    using Representation = std::variant<
            std::monostate,
            bool,
            int32_t,
            int64_t,
            mpf_class,
            float,
            double,
            std::chrono::system_clock::time_point,
            std::chrono::milliseconds,
            std::unique_ptr<Interval>,
            std::string,
            std::vector<uint8_t>,
            std::list<std::unique_ptr<TypedValue>>,
            std::unordered_map<std::string, std::unique_ptr<TypedValue>>
    >;
}

namespace Types {
    /**
     * @brief Represents a wrapper for all types used in the driver.
     *
     * This class represents a value of a specific type. It acts as the bridge between the polypheny internal type system and the one of C++.
     * Methods are provided to create a TypedValue from a variety of C++ types as well as retrieve polypheny internal types in a variety of C++ types.
     *
     * Instances of this class are returned whenever a value is retrieved from a result like the value in a column of a relational result.
     * The retrieval methods can then be used to retrieve the value in a C++ compliant type.
     *
     * Further, this class is used whenever a value is passed to the driver like when parameters are specified for a prepared statement.
     */
    class TypedValue {
    private:
        Representation value;

        org::polypheny::prism::ProtoValue::ValueCase value_case;
        std::shared_ptr<org::polypheny::prism::ProtoValue> serialized;
        bool is_deserialized;
        void deserialize();

    public:
        // Constructors for different types
        explicit TypedValue(const org::polypheny::prism::ProtoValue &proto_value);

        /**
         * @brief Constructs a TypedValue object from a boolean value.
         *
         * @param value The boolean value.
         */
        explicit TypedValue(bool value);

        /**
         * @brief Constructs a TypedValue object from a 32-bit signed integer value.
         *
         * @param value The 32-bit signed integer value.
         */
        explicit TypedValue(int32_t value);

        /**
         * @brief Constructs a TypedValue object from a 64-bit signed integer value.
         *
         * @param value The 64-bit signed integer value.
         */
        explicit TypedValue(int64_t value);

        /**
         * @brief Constructs a TypedValue object from a big decimal value.
         *
         * For big decimals the mpf type of the GNU Multiple Precision Arithmetic Library is used.
         * This not only provides the user with a representation of a big decimal value but also with useful operators to use them in calculations.
         *
         * @param value The big decimal value.
         */
        explicit TypedValue(const mpf_class &value);

        /**
         * @brief Constructs a TypedValue object from a float value.
         *
         * @param value The float value.
         */
        explicit TypedValue(float value);

        /**
         * @brief Constructs a TypedValue object from a double value.
         *
         * @param value The double value.
         */
        explicit TypedValue(double value);

        /**
         * @brief Constructs a TypedValue object from a time point representing a date.
         *
         * This is used to construct the polypheny internal "DATE" data type used to represent a specific day.
         *
         * @param value The time point value.
         */
        explicit TypedValue(const std::chrono::system_clock::time_point &value);

        /**
         * @brief Constructs a TypedValue object from milliseconds since midnight specifying a specific time of day.
         *
         * @param value The duration value.
         */
        explicit TypedValue(const std::chrono::milliseconds &value);

        /**
         * @brief Constructs a TypedValue object from a time point representing a timestamp.
         *
         * This method serves two purposes based on the value set for "is_timestamp". If set to true, this constructs a typed value representing a timestamp where the time_point value is interpreted as milliseconds since epoch.
         * If set to false, the time_point is parsed as a date.
         *
         * @param timestamp_value The time point value.
         * @param is_timestamp Indicates if the value is a timestamp.
         */
        explicit TypedValue(const std::chrono::system_clock::time_point &timestamp_value, bool is_timestamp);

        /**
         * @brief Constructs a TypedValue object from the Polypheny proprietary interval type.
         *
         * @param value The interval value.
         */
        explicit TypedValue(const Interval &value);

        /**
         * @brief Constructs a TypedValue object from a document value.
         *
         * Documents are represented by a unordered map of key, value pairs. The keys are the field names as strings while the values are further typed value instances.
         * Documents can be nested.
         *
         * @param document_value The document value.
         */
        explicit TypedValue(const std::unordered_map<std::string, TypedValue> &document_value);

        /**
         * @brief Constructs an empty TypedValue object representing a null value.
         */
        explicit TypedValue();

        /**
         * @brief Constructs a TypedValue object from a list of values representing a list.
         *
         * @param values The list of values.
         */
        explicit TypedValue(const std::list<TypedValue> &values);

        /**
         * @brief Constructs a TypedValue object from a string value.
         *
         * @param value The string value.
         */
        explicit TypedValue(char const* values);

/**
         * @brief Constructs a TypedValue object from a vector of bytes.
         *
         * @param value The vector of bytes.
         */
        explicit TypedValue(const std::string &value);

/**
         * @brief Constructs a TypedValue object from a vector of bytes.
         *
         * @param value The vector of bytes.
         */
        explicit TypedValue(const std::vector<uint8_t> &value);

        TypedValue(const TypedValue &other);

        TypedValue &operator=(const TypedValue &other);

        ~TypedValue() = default;

        friend std::ostream &operator<<(std::ostream &os, const TypedValue &typed_value);

        org::polypheny::prism::ProtoValue* serialize();

        org::polypheny::prism::ProtoValue::ValueCase get_value_case();

        /**
         * @brief Checks if the TypedValue is null.
         *
         * A typed value is null if it's internal type is the null type. Such a value is either returned by Polypheny or created using the no arg constructor of this class.
         *
         * @return True if the value is null, false otherwise.
         */
        bool is_null();

        /**
         * @brief Retrieves the value as a boolean if possible..
         *
         * @return The boolean value.
         * @throws std::runtime_error if the value is not a boolean.
         */
        bool as_boolean();

        /**
         * @brief Retrieves the value as a signed 32-bit integer if possible.
         *
         * @return The 32-bit integer value.
         * @throws std::runtime_error if the value is not a 32-bit integer.
         */
        int32_t as_int32_t();

        /**
         * @brief Retrieves the value as a 64-bit integer if possible.
         *
         * @return The 64-bit integer value.
         * @throws std::runtime_error if the value is not a 64-bit integer.
         */
        int64_t as_int64_t();

        /**
         * @brief Retrieves a big decimal value as a mpf object of the GNU Multiple Precision Arithmetic Library.
         *
         * @return The big decimal value.
         * @throws std::runtime_error if the value is not a big decimal.
         */
        mpf_class as_big_decimal();

        /**
         * @brief Retrieves the value as a float.
         *
         * @return The float value.
         * @throws std::runtime_error if the value is not a float.
         */
        float as_float();

        /**
         * @brief Retrieves the value as a double.
         *
         * @return The double value.
         * @throws std::runtime_error if the value is not a double.
         */
        double as_double();

        /**
         * @brief Retrieves the value as a date.
         *
         * @return The date value.
         * @throws std::runtime_error if the value is not a date.
         */
        std::chrono::system_clock::time_point as_date();

        /**
         * @brief Retrieves the value as a time.
         *
         * @return The time value.
         * @throws std::runtime_error if the value is not a time.
         */
        std::chrono::milliseconds as_time();

        /**
         * @brief Retrieves the value as a timestamp.
         *
         * @return The timestamp value.
         * @throws std::runtime_error if the value is not a timestamp.
         */
        std::chrono::system_clock::time_point as_timestamp();

        /**
         * @brief Retrieves the value as an interval.
         *
         * @return The interval value.
         * @throws std::runtime_error if the value is not an interval.
         */
        Interval as_interval();
/**
         * @brief Retrieves the value as a document.
         *
         * @return The document value.
         * @throws std::runtime_error if the value is not a document.
         */
        std::unordered_map<std::string, TypedValue> as_document();

        /**
         * @brief Retrieves the value as a list.
         *
         * @return The list value.
         * @throws std::runtime_error if the value is not a list.
         */
        std::list<TypedValue> as_list();

        /**
         * @brief Retrieves the value as a string.
         *
         * @return The string value.
         * @throws std::runtime_error if the value is not a string.
         */
        std::string as_string();

        /**
         * @brief Retrieves the value as a vector of bytes.
         *
         * @return The vector of bytes.
         * @throws std::runtime_error if the value is not a vector of bytes.
         */
        std::vector<uint8_t> as_bytes();
    };

} // namespace Types

#endif // POLYPHENY_CPP_DRIVER_TYPEDVALUE_H
