//
// Created by tobi on 30.05.24.
//

#ifndef POLYPHENY_CPP_DRIVER_TYPEDVALUE_H
#define POLYPHENY_CPP_DRIVER_TYPEDVALUE_H

#include <chrono>
#include <vector>
#include <list>

#include "BigDecimal.h"
#include "Interval.h"
#include "value.pb.h"

namespace Types {

    union Representation {
        bool boolean_value;
        int32_t integer_value;
        int64_t bigint_value;
        Types::BigDecimal big_decimal_value;
        float float_value; // TODO: we could use fixe width floats from stdfloat here. This would require c++23
        double double_value; // TODO: we could use fixe width floats from stdfloat here. This would require c++23
        std::chrono::system_clock::time_point date_value;
        std::chrono::milliseconds time_value;
        std::chrono::system_clock::time_point timestamp_value;
        Types::Interval interval_value;
        std::string varchar_value;
        std::vector<uint8_t> binary_value;

        Representation() {}

        ~Representation() {}
    };

    class TypedValue {
    private:
        ValueCase value_case;
        Representation value;
        org::polypheny::prism::ProtoValue serialized;
        bool is_serialized;

        TypedValue();

        org::polypheny::prism::ProtoValue serialize();
        void deserialize();

    public:
        TypedValue(const ProtoValue &proto_value);

        bool is_null();

        static TypedValue &from_boolean(bool value);

        static TypedValue &from_int32_t(int32_t value);

        static TypedValue &from_int64_t(int64_t value);

        static TypedValue &from_big_decimal(Types::BigDecimal value);

        static TypedValue &from_float32_t(float32_t value);

        static TypedValue &from_float64_t(float64_t value);

        static TypedValue &from_date(std::chrono::system_clock::time_point value);

        static TypedValue &from_time(std::chrono::milliseconds value)

        static TypedValue &form_timestamp(std::chrono::system_clock::time_point value);

        static TypedValue &from_interval(Types::Interval value);

        static TypedValue &from_document(Types::Document value);

        static TypedValue &from_null();

        static TypedValue &from_list(std::list<TypedValue()> values);

        static TypedValue &from_string(std::string value);

        static TypedValue &from_bytes(std::vector<uint8_t> value);

        bool as_boolean();

        int32_t as_int32_t();
        int64_t as_int64_t();
        Types::BigDecimal as_big_decimal();
        float32_t as_float32_t();
        float64_t as_float64_t();
        std::chrono::system_clock::time_point as_date();
        std::chrono::milliseconds as_time();
        std::chrono::system_clock::time_point as_timestamp();
        Types::Interval as_interval();
        Types::Document as_document();
        std::list as_list();
        std::string as_string();
        std::vector<uint8_t> as_bytes();
    };

} // Types

#endif //POLYPHENY_CPP_DRIVER_TYPEDVALUE_H
