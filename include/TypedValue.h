#ifndef POLYPHENY_CPP_DRIVER_TYPEDVALUE_H
#define POLYPHENY_CPP_DRIVER_TYPEDVALUE_H

#include <chrono>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include "ProtoUtils.h"
#include "value.pb.h"
// Forward declarations to avoid cyclic dependencies
namespace Types {
    class BigDecimal;

    class Document;

    class Interval;

    class TypedValue;
}

namespace Types {

    union Representation {
        bool boolean_value;
        int32_t integer_value;
        int64_t bigint_value;
        BigDecimal *big_decimal_value;
        float float_value;
        double double_value;
        std::chrono::system_clock::time_point date_value;
        std::chrono::milliseconds time_value;
        std::chrono::system_clock::time_point timestamp_value;
        Interval *interval_value;
        std::string varchar_value;
        std::vector<uint8_t> binary_value;
        std::list<TypedValue> list_value;
        Document *document_value;

        Representation() : boolean_value(false) {}

        ~Representation() {}
    };

    class TypedValue {
    private:
        org::polypheny::prism::ProtoValue::ValueCase value_case;
        Types::Representation value;
        std::shared_ptr<org::polypheny::prism::ProtoValue> serialized;
        bool is_serialized{};
        bool is_deserialized{};
        void deserialize();

    public:
        explicit TypedValue(org::polypheny::prism::ProtoValue proto_value);

        explicit TypedValue(bool value);

        explicit TypedValue(int32_t value);

        explicit TypedValue(int64_t value);

        explicit TypedValue(const BigDecimal &value);

        explicit TypedValue(float value);

        explicit TypedValue(double value);

        explicit TypedValue(const std::chrono::system_clock::time_point &value);

        explicit TypedValue(const std::chrono::milliseconds &value);

        explicit TypedValue(const std::chrono::system_clock::time_point &timestamp_value, bool is_timestamp);

        explicit TypedValue(const Interval &value);

        explicit TypedValue(const Document &value);

        explicit TypedValue();

        explicit TypedValue(const std::list<Types::TypedValue> &values);

        explicit TypedValue(const std::string &value);

        explicit TypedValue(const std::vector<uint8_t> &value);

        TypedValue(const TypedValue &other);

        TypedValue(TypedValue &&other) noexcept;

        TypedValue &operator=(const TypedValue &other);

        TypedValue &operator=(TypedValue &&other) noexcept;

        ~TypedValue();

        std::shared_ptr<org::polypheny::prism::ProtoValue> serialize();

        bool is_null();

        bool is_moved_from();

        bool as_boolean();

        int32_t as_int32_t();

        int64_t as_int64_t();

        Types::BigDecimal as_big_decimal();

        float as_float();

        double as_double();

        std::chrono::system_clock::time_point as_date();

        std::chrono::milliseconds as_time();

        std::chrono::system_clock::time_point as_timestamp();

        Types::Interval as_interval();

        Types::Document as_document();

        std::list<TypedValue> as_list();

        std::string as_string();

        std::vector<uint8_t> as_bytes();
    };

} // namespace Types

#endif // POLYPHENY_CPP_DRIVER_TYPEDVALUE_H
