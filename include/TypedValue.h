//
// Created by tobi on 30.05.24.
//

#ifndef POLYPHENY_CPP_DRIVER_TYPEDVALUE_H
#define POLYPHENY_CPP_DRIVER_TYPEDVALUE_H

#include <chrono>
#include <vector>
#include <list>
#include <memory>
#include <string>

#include "BigDecimal.h"
#include "Document.h"
#include "Interval.h"
#include "value.pb.h"

namespace Types {

    union Representation {
        bool boolean_value;
        int32_t integer_value;
        int64_t bigint_value;
        Types::BigDecimal big_decimal_value;
        float float_value;
        double double_value;
        std::chrono::system_clock::time_point date_value;
        std::chrono::milliseconds time_value;
        std::chrono::system_clock::time_point timestamp_value;
        Types::Interval interval_value;
        std::string varchar_value;
        std::vector<uint8_t> binary_value;
        std::list<TypedValue> list_value;
        Types::Document document_value;

        Representation() {}
        ~Representation() {}
    };

    class TypedValue {
    private:
        org::polypheny::prism::ProtoValue::ValueCase value_case;
        Representation value;
        org::polypheny::prism::ProtoValue serialized;
        bool is_serialized;
        void deserialize();

    public:
        TypedValue();
        ~TypedValue();
        TypedValue(const org::polypheny::prism::ProtoValue &proto_value);

        org::polypheny::prism::ProtoValue serialize() const;

        bool is_null() const;

        static std::unique_ptr<TypedValue> from_boolean(bool value);
        static std::unique_ptr<TypedValue> from_int32_t(int32_t value);
        static std::unique_ptr<TypedValue> from_int64_t(int64_t value);
        static std::unique_ptr<TypedValue> from_big_decimal(const Types::BigDecimal &value);
        static std::unique_ptr<TypedValue> from_float(float value);
        static std::unique_ptr<TypedValue> from_double(double value);
        static std::unique_ptr<TypedValue> from_date(const std::chrono::system_clock::time_point &value);
        static std::unique_ptr<TypedValue> from_time(const std::chrono::milliseconds &value);
        static std::unique_ptr<TypedValue> from_timestamp(const std::chrono::system_clock::time_point &value);
        static std::unique_ptr<TypedValue> from_interval(const Types::Interval &value);
        static std::unique_ptr<TypedValue> from_document(const Types::Document &value);
        static std::unique_ptr<TypedValue> from_null();
        static std::unique_ptr<TypedValue> from_list(const std::list<TypedValue> &values);
        static std::unique_ptr<TypedValue> from_string(const std::string &value);
        static std::unique_ptr<TypedValue> from_bytes(const std::vector<uint8_t> &value);

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
