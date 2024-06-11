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
    class TypedValue {
    private:
        Representation value;

        org::polypheny::prism::ProtoValue::ValueCase value_case;

        std::shared_ptr<org::polypheny::prism::ProtoValue> serialized;
        bool is_serialized;
        bool is_deserialized;
        void deserialize();

    public:
        // Constructors for different types
        explicit TypedValue(const org::polypheny::prism::ProtoValue &proto_value);

        explicit TypedValue(bool value);

        explicit TypedValue(int32_t value);

        explicit TypedValue(int64_t value);

        explicit TypedValue(const mpf_class &value);

        explicit TypedValue(float value);

        explicit TypedValue(double value);

        explicit TypedValue(const std::chrono::system_clock::time_point &value);

        explicit TypedValue(const std::chrono::milliseconds &value);

        explicit TypedValue(const std::chrono::system_clock::time_point &timestamp_value, bool is_timestamp);

        explicit TypedValue(const Interval &value);

        explicit TypedValue(const std::unordered_map<std::string, TypedValue> &document_value);

        explicit TypedValue();

        explicit TypedValue(const std::list<TypedValue> &values);

        explicit TypedValue(const std::string &value);

        explicit TypedValue(const std::vector<uint8_t> &value);

        TypedValue(const TypedValue &other);

        //TypedValue(TypedValue &&other) noexcept;

        TypedValue &operator=(const TypedValue &other);

        ~TypedValue() = default;

        friend std::ostream &operator<<(std::ostream &os, const TypedValue &typed_value);

        std::shared_ptr<org::polypheny::prism::ProtoValue> serialize();

        org::polypheny::prism::ProtoValue::ValueCase get_value_case();

        bool is_null();

        bool as_boolean();

        int32_t as_int32_t();

        int64_t as_int64_t();

        mpf_class as_big_decimal();

        float as_float();

        double as_double();

        std::chrono::system_clock::time_point as_date();

        std::chrono::milliseconds as_time();

        std::chrono::system_clock::time_point as_timestamp();

        Interval as_interval();

        std::unordered_map<std::string, TypedValue> as_document();

        std::list<TypedValue> as_list();

        std::string as_string();

        std::vector<uint8_t> as_bytes();
    };

} // namespace Types

#endif // POLYPHENY_CPP_DRIVER_TYPEDVALUE_H
