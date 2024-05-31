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

    org::polypheny::prism::ProtoValue TypedValue::serialize() const {
        org::polypheny::prism::ProtoValue proto_value;
        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                proto_value.mutable_boolean()->set_boolean(value.boolean_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                proto_value.mutable_integer()->set_integer(value.integer_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                proto_value.mutable_long_()->set_long_(value.bigint_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                *proto_value.mutable_big_decimal() = value.big_decimal_value.serialize();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                proto_value.mutable_float_()->set_float_(value.float_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                proto_value.mutable_double_()->set_double_(value.double_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                *proto_value.mutable_date() = Utils::ProtoUtils::date_to_proto(value.date_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                *proto_value.mutable_time() = Utils::ProtoUtils::time_to_proto(value.time_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                *proto_value.mutable_timestamp() = Utils::ProtoUtils::timestamp_to_proto(value.timestamp_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                *proto_value.mutable_interval() = value.interval_value.serialize();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                proto_value.mutable_string()->set_string(value.varchar_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                proto_value.mutable_binary()->set_binary(Utils::ProtoUtils::vector_to_string(value.binary_value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList:
                *proto_value.mutable_list() = Utils::ProtoUtils::list_to_proto(value.list_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument:
                *proto_value.mutable_document() = value.document_value.serialize();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFile:
                proto_value.mutable_file()->set_binary(Utils::ProtoUtils::vector_to_string(value.binary_value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                proto_value.mutable_null();
                break;
            default:
                throw std::runtime_error("Encountered unknown data type");
        }
        return proto_value;
    }

    void TypedValue::deserialize() {
        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                value.boolean_value = serialized.boolean().boolean();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                value.integer_value = serialized.integer().integer();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                value.bigint_value = serialized.long_().long_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                value.big_decimal_value = Types::BigDecimal(serialized.big_decimal());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                value.float_value = serialized.float_().float_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                value.double_value = serialized.double_().double_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                value.date_value = Utils::ProtoUtils::proto_to_date(serialized.date());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                value.time_value = Utils::ProtoUtils::proto_to_time(serialized.time())
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                value.timestamp_value = Utils::ProtoUtils::proto_to_timestamp(serialized.timestamp());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                value.interval_value = Types::Interval(serialized.interval());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                value.varchar_value = serialized.string().string();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                value.binary_value = Utils::ProtoUtils::string_to_vector(serialized.binary().binary());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList:
                value.list_value = Utils::ProtoUtils::proto_to_list(serialized.list());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument:
                value.document_value = Types::Document(serialized.document());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFile:
                // TODO: we currently represent files the same way as binaries. with the introduction of streaming we might implement a custom file type which automatically fetches more data if required.
                value.binary_value = Utils::ProtoUtils::string_to_vector(serialized.file().binary());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                break;
            default:
                throw std::runtime_error("Encountered unknown data type");
        }
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
        auto tv = std::make_unique<TypedValue>();
        tv->value.list_value = values;
        tv->value_case = org::polypheny::prism::ProtoValue::ValueCase::kList;
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
