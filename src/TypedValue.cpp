//
// Created by tobi on 30.05.24.
//

#include "TypedValue.h"
#include "ProtoUtils.h"

namespace Types {
    TypedValue::TypedValue(org::polypheny::prism::ProtoValue proto_value) : serialized(std::move(proto_value)),
                                                                            is_serialized(true),
                                                                            value_case(proto_value.value_case()) {
    }

    TypedValue::TypedValue(bool value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kBoolean) {
        this->value.boolean_value = value;
    }

    TypedValue::TypedValue(int32_t value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kInteger) {
        this->value.integer_value = value;
    }

    TypedValue::TypedValue(int64_t value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kLong) {
        this->value.bigint_value = value;
    }

    TypedValue::TypedValue(const Types::BigDecimal &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal) {
        new(&this->value.big_decimal_value) Types::BigDecimal(value);
    }

    TypedValue::TypedValue(float value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kFloat) {
        this->value.float_value = value;
    }

    TypedValue::TypedValue(double value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kDouble) {
        this->value.double_value = value;
    }

    TypedValue::TypedValue(const std::chrono::system_clock::time_point &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kDate) {
        this->value.date_value = value;
    }

    TypedValue::TypedValue(const std::chrono::milliseconds &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kTime) {
        this->value.time_value = value;
    }

    TypedValue::TypedValue(const std::chrono::system_clock::time_point &timestamp_value, bool is_timestamp)
            : value_case(is_timestamp ? org::polypheny::prism::ProtoValue::ValueCase::kTimestamp
                                      : org::polypheny::prism::ProtoValue::ValueCase::kDate) {
        if (is_timestamp) {
            this->value.timestamp_value = timestamp_value;
        } else {
            this->value.date_value = timestamp_value;
        }
    }

    TypedValue::TypedValue(const Types::Interval &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kInterval) {
        new(&this->value.interval_value) Types::Interval(value);
    }

    TypedValue::TypedValue(const Types::Document &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kDocument) {
        //TODO: copy documents propertly!!!
        // Assign value.document_value = value
        new(&this->value.document_value) Types::Document(value);
    }

    TypedValue::TypedValue()
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kNull) {}

    TypedValue::TypedValue(const std::list<Types::TypedValue> &values)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kList) {
        new(&this->value.list_value) std::list<Types::TypedValue>(values);
    }

    TypedValue::TypedValue(const std::string &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kString) {
        new(&this->value.varchar_value) std::string(value);
    }

    TypedValue::TypedValue(const std::vector<uint8_t> &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kBinary) {
        new(&this->value.binary_value) std::vector<uint8_t>(value);
    }

    TypedValue::TypedValue(const TypedValue &original)
            : value_case(original.value_case), is_serialized(original.is_serialized) {

        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                value.boolean_value = original.value.boolean_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                value.integer_value = original.value.integer_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                value.bigint_value = original.value.bigint_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                new(&value.big_decimal_value) BigDecimal(original.value.big_decimal_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                value.float_value = original.value.float_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                value.double_value = original.value.double_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                value.date_value = original.value.date_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                value.time_value = original.value.time_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                value.timestamp_value = original.value.timestamp_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                new(&value.interval_value) Interval(original.value.interval_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                new(&value.varchar_value) std::string(original.value.varchar_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                new(&value.binary_value) std::vector<uint8_t>(original.value.binary_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList:
                new(&value.list_value) std::list<TypedValue>(original.value.list_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument:
                new(&value.document_value) Document(original.value.document_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                break;
            default:
                throw std::runtime_error("Unsupported value case in TypedValue copy constructor");
        }

        if (original.is_serialized) {
            serialized = original.serialized;
        }
    }

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

    std::unique_ptr<org::polypheny::prism::ProtoValue> TypedValue::serialize() const {
        auto proto_value = std::make_unique<org::polypheny::prism::ProtoValue>();

        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                proto_value->mutable_boolean()->set_boolean(value.boolean_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                proto_value->mutable_integer()->set_integer(value.integer_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                proto_value->mutable_long_()->set_long_(value.bigint_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                *proto_value->mutable_big_decimal() = *value.big_decimal_value.serialize();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                proto_value->mutable_float_()->set_float_(value.float_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                proto_value->mutable_double_()->set_double_(value.double_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                *proto_value->mutable_date() = *Utils::ProtoUtils::date_to_proto(value.date_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                *proto_value->mutable_time() = *Utils::ProtoUtils::time_to_proto(value.time_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                *proto_value->mutable_timestamp() = *Utils::ProtoUtils::timestamp_to_proto(value.timestamp_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                *proto_value->mutable_interval() = *value.interval_value.serialize();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                proto_value->mutable_string()->set_string(value.varchar_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                proto_value->mutable_binary()->set_binary(Utils::ProtoUtils::vector_to_string(value.binary_value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList:
                *proto_value->mutable_list() = *Utils::ProtoUtils::list_to_proto(value.list_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument:
                *proto_value->mutable_document() = *value.document_value.serialize();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFile:
                proto_value->mutable_file()->set_binary(Utils::ProtoUtils::vector_to_string(value.binary_value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                proto_value->mutable_null();
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
                value.time_value = Utils::ProtoUtils::proto_to_time(serialized.time());
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
        return value_case == org::polypheny::prism::ProtoValue::ValueCase::kNull;
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

    Interval TypedValue::as_interval() {
        return value.interval_value;
    }

    Document TypedValue::as_document() {
        // TODO: implement returning of document
        // Placeholder implementation
        return {
        };
    }

    std::list<TypedValue> TypedValue::as_list() {
        // TODO: implement returnin of list
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
