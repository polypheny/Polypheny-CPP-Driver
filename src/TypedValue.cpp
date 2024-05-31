//
// Created by tobi on 30.05.24.
//

#include "TypedValue.h"

namespace Types {

    TypedValue::~TypedValue() {
        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                value.big_decimal_value.~BigDecimal();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                value.interval_value.~Interval();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                value.varchar_value.~basic_string();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                value.binary_value.~vector<uint8_t>();
                break;
            default:
                break;
        }
    }

    org::polypheny::prism::ProtoValue TypedValue::serialize() {
        // Placeholder implementation
        return {};
    }

    void TypedValue::deserialize() {
        // Placeholder implementation
    }

    bool TypedValue::is_null() const {
        return value_case == org::polypheny::prism::ProtoValue::ValueCase::VALUE_NOT_SET;
    }

    std::unique_ptr<TypedValue> TypedValue::from_boolean(bool value) {
        auto tv = std::make_unique<TypedValue>();
        tv->value.boolean_value = value;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kBoolean;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_int32_t(int32_t value) {
        auto tv = std::make_unique<TypedValue>();
        tv->value.integer_value = value;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kInteger;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_int64_t(int64_t value) {
        auto tv = std::make_unique<TypedValue>();
        tv->value.bigint_value = value;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kLong;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_big_decimal(const Types::BigDecimal &value) {
        auto tv = std::make_unique<TypedValue>();
        new(&tv->value.big_decimal_value) Types::BigDecimal(value);
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_float(float value) {
        auto tv = std::make_unique<TypedValue>();
        tv->value.float_value = value;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kFloat;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_double(double value) {
        auto tv = std::make_unique<TypedValue>();
        tv->value.double_value = value;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kDouble;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_date(const std::chrono::system_clock::time_point &value) {
        auto tv = std::make_unique<TypedValue>();
        tv->value.date_value = value;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kDate;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_time(const std::chrono::milliseconds &value) {
        auto tv = std::make_unique<TypedValue>();
        tv->value.time_value = value;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kTime;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_timestamp(const std::chrono::system_clock::time_point &value) {
        auto tv = std::make_unique<TypedValue>();
        tv->value.timestamp_value = value;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kTimestamp;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_interval(const Types::Interval &value) {
        auto tv = std::make_unique<TypedValue>();
        new(&tv->value.interval_value) Types::Interval(value);
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kInterval;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_document(const Types::Document &value) {
        // Placeholder implementation
        auto tv = std::make_unique<TypedValue>();
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kDocument;
        // Assign value.document_value = value (requires actual implementation)
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_null() {
        auto tv = std::make_unique<TypedValue>();
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kNull;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_list(const std::list<TypedValue> &values) {
        // Placeholder implementation
        auto tv = std::make_unique<TypedValue>();
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kList;
        // Assign value.list_value = values (requires actual implementation)
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_string(const std::string &value) {
        auto tv = std::make_unique<TypedValue>();
        new(&tv->value.varchar_value) std::string(value);
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kString;
        return tv;
    }

    std::unique_ptr<TypedValue> TypedValue::from_bytes(const std::vector<uint8_t> &value) {
        auto tv = std::make_unique<TypedValue>();
        new(&tv->value.binary_value) std::vector<uint8_t>(value);
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kBinary;
        return tv;
    }

    bool TypedValue::as_boolean() {
        return value.boolean_value;
    }

    int32_t TypedValue::as_int32_t() {
        return value.integer_value;
    }

    int64_t TypedValue::as_int64_t() {
        return value.bigint_value;
    }

    Types::BigDecimal TypedValue::as_big_decimal() {
        return value.big_decimal_value;
    }

    float TypedValue::as_float() {
        return value.float_value;
    }

    double TypedValue::as_double() {
        return value.double_value;
    }

    std::chrono::system_clock::time_point TypedValue::as_date() {
        return value.date_value;
    }

    std::chrono::milliseconds TypedValue::as_time() {
        return value.time_value;
    }

    std::chrono::system_clock::time_point TypedValue::as_timestamp() {
        return value.timestamp_value;
    }

    Types::Interval TypedValue::as_interval() {
        return value.interval_value;
    }

    Types::Document TypedValue::as_document() {
        // Placeholder implementation
        return {};
    }

    std::list<TypedValue> TypedValue::as_list() {
        // Placeholder implementation
        return {};
    }

    std::string TypedValue::as_string() {
        return value.varchar_value;
    }

    std::vector<uint8_t> TypedValue::as_bytes() {
        return value.binary_value;
    }

} // namespace Types
