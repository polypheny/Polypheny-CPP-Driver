//
// Created by tobi on 30.05.24.
//

#include "TypedValue.h"
#include "BigDecimal.h"
#include "Document.h"
#include "Interval.h"

namespace Types {

    TypedValue::TypedValue(org::polypheny::prism::ProtoValue proto_value)
            : serialized(std::make_unique<org::polypheny::prism::ProtoValue>(std::move(proto_value))),
              is_serialized(true),
              is_deserialized(false),
              value_case(proto_value.value_case()) {
    }

    TypedValue::TypedValue(bool value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kBoolean),
              is_serialized(false),
              is_deserialized(true) {
        this->value.
                boolean_value = value;
    }

    TypedValue::TypedValue(int32_t value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kInteger),
              is_serialized(false),
              is_deserialized(true) {
        this->value.integer_value = value;
    }

    TypedValue::TypedValue(int64_t value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kLong),
              is_serialized(false),
              is_deserialized(true) {
        this->value.bigint_value = value;
    }

    TypedValue::TypedValue(const Types::BigDecimal &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal),
              is_serialized(false),
              is_deserialized(true) {
        this->value.big_decimal_value = new Types::BigDecimal(value);
    }

    TypedValue::TypedValue(float value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kFloat),
              is_serialized(false),
              is_deserialized(true) {
        this->value.float_value = value;
    }

    TypedValue::TypedValue(double value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kDouble),
              is_serialized(false),
              is_deserialized(true) {
        this->value.double_value = value;
    }

    TypedValue::TypedValue(const std::chrono::system_clock::time_point &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kDate),
              is_serialized(false),
              is_deserialized(true) {
        this->value.date_value = value;
    }

    TypedValue::TypedValue(const std::chrono::milliseconds &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kTime),
              is_serialized(false),
              is_deserialized(true) {
        this->value.time_value = value;
    }

    TypedValue::TypedValue(const std::chrono::system_clock::time_point &timestamp_value, bool is_timestamp)
            : value_case(is_timestamp ? org::polypheny::prism::ProtoValue::ValueCase::kTimestamp
                                      : org::polypheny::prism::ProtoValue::ValueCase::kDate),
              is_serialized(false),
              is_deserialized(true) {
        if (is_timestamp) {
            this->value.timestamp_value = timestamp_value;
        } else {
            this->value.date_value = timestamp_value;
        }
    }

    TypedValue::TypedValue(const Types::Interval &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kInterval),
              is_serialized(false),
              is_deserialized(true) {
        this->value.interval_value = new Types::Interval(value);
    }

    TypedValue::TypedValue(const Types::Document &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kDocument),
              is_serialized(false),
              is_deserialized(true) {
        this->value.document_value = new Types::Document(value);
    }

    TypedValue::TypedValue()
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kNull) {}

    TypedValue::TypedValue(const std::list<Types::TypedValue> &values)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kList),
              is_serialized(false),
              is_deserialized(true) {
        new(&this->value.list_value) std::list<Types::TypedValue>(values);
    }

    TypedValue::TypedValue(const std::string &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kString),
              is_serialized(false),
              is_deserialized(true) {
        new(&this->value.varchar_value) std::string(value);
    }

    TypedValue::TypedValue(const std::vector<uint8_t> &value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kBinary),
              is_serialized(false),
              is_deserialized(true) {
        new(&this->value.binary_value) std::vector<uint8_t>(value);
    }

    TypedValue::TypedValue(const TypedValue &other)
            : value_case(other.value_case), is_serialized(other.is_serialized), is_deserialized(other.is_deserialized) {

        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                value.boolean_value = other.value.boolean_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                value.integer_value = other.value.integer_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                value.bigint_value = other.value.bigint_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                value.big_decimal_value = new BigDecimal(*other.value.big_decimal_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                value.float_value = other.value.float_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                value.double_value = other.value.double_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                value.date_value = other.value.date_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                value.time_value = other.value.time_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                value.timestamp_value = other.value.timestamp_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                value.interval_value = new Interval(*other.value.interval_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                new(&value.varchar_value) std::string(other.value.varchar_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                new(&value.binary_value) std::vector<uint8_t>(other.value.binary_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList:
                new(&value.list_value) std::list<TypedValue>(other.value.list_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument:
                value.document_value = new Document(*other.value.document_value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                break;
            default:
                throw std::runtime_error("Unsupported value case in TypedValue copy constructor");
        }

        if (other.is_serialized) {
            serialized = std::make_unique<org::polypheny::prism::ProtoValue>(*other.serialized);
        }
    }

    TypedValue::TypedValue(TypedValue &&other) noexcept
            : value_case(other.value_case), is_serialized(other.is_serialized), is_deserialized(other.is_deserialized),
              serialized(std::move(other.serialized)) {
        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                value.boolean_value = other.value.boolean_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                value.integer_value = other.value.integer_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                value.bigint_value = other.value.bigint_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                value.big_decimal_value = other.value.big_decimal_value;
                other.value.big_decimal_value = nullptr;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                value.float_value = other.value.float_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                value.double_value = other.value.double_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                value.date_value = other.value.date_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                value.time_value = other.value.time_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                value.timestamp_value = other.value.timestamp_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                value.interval_value = other.value.interval_value;
                other.value.interval_value = nullptr;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                new(&value.varchar_value) std::string(std::move(other.value.varchar_value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                new(&value.binary_value) std::vector<uint8_t>(std::move(other.value.binary_value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList:
                new(&value.list_value) std::list<TypedValue>(std::move(other.value.list_value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument:
                value.document_value = other.value.document_value;
                other.value.document_value = nullptr;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                break;
            default:
                throw std::runtime_error("Unsupported value case in TypedValue move constructor");
        }
        other.value_case = org::polypheny::prism::ProtoValue::ValueCase::VALUE_NOT_SET;
    }

    TypedValue &TypedValue::operator=(const TypedValue &other) {
        if (this != &other) {
            this->~TypedValue();
            new(this) TypedValue(other);
        }
        return *this;
    }

    TypedValue &TypedValue::operator=(TypedValue &&other) noexcept {
        if (this != &other) {
            this->~TypedValue();
            new(this) TypedValue(std::move(other));
        }
        return *this;
    }

    TypedValue::~TypedValue() {
        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                delete value.big_decimal_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                delete value.interval_value;
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                value.varchar_value.std::string::~string();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                value.binary_value.std::vector<uint8_t>::~vector();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument:
                delete value.document_value;
                break;
            default:
                break;
        }
    }

    std::shared_ptr<org::polypheny::prism::ProtoValue> TypedValue::serialize() {
        if (is_serialized) {
            return serialized;
        }
        auto proto_value = std::make_shared<org::polypheny::prism::ProtoValue>();

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
                *proto_value->mutable_big_decimal() = *value.big_decimal_value->serialize();
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
                *proto_value->mutable_interval() = *value.interval_value->serialize();
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
                *proto_value->mutable_document() = *value.document_value->serialize();
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
        serialized = proto_value;
        is_serialized = true;
        return proto_value;
    }

    void TypedValue::deserialize() {
        if (is_deserialized) {
            return;
        }
        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                value.boolean_value = serialized->boolean().boolean();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                value.integer_value = serialized->integer().integer();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                value.bigint_value = serialized->long_().long_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                value.big_decimal_value = new Types::BigDecimal(serialized->big_decimal());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                value.float_value = serialized->float_().float_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                value.double_value = serialized->double_().double_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                value.date_value = Utils::ProtoUtils::proto_to_date(serialized->date());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                value.time_value = Utils::ProtoUtils::proto_to_time(serialized->time());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                value.timestamp_value = Utils::ProtoUtils::proto_to_timestamp(serialized->timestamp());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                value.interval_value = new Types::Interval(serialized->interval());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                value.varchar_value = serialized->string().string();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                value.binary_value = Utils::ProtoUtils::string_to_vector(serialized->binary().binary());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList:
                value.list_value = Utils::ProtoUtils::proto_to_list(serialized->list());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument:
                value.document_value = new Types::Document(serialized->document());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFile:
                value.binary_value = Utils::ProtoUtils::string_to_vector(serialized->file().binary());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                break;
            default:
                throw std::runtime_error("Encountered unknown data type");
        }
        is_deserialized = true;
    }

    bool TypedValue::is_null() {
        if (!is_deserialized) {
            deserialize();
        }
        return value_case == org::polypheny::prism::ProtoValue::ValueCase::kNull;
    }

    bool TypedValue::as_boolean() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kBoolean) {
            throw std::runtime_error("This value can not be retrieved as a boolean.");
        }
        return value.boolean_value;
    }

    int32_t TypedValue::as_int32_t() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kInteger) {
            throw std::runtime_error("This value can not be retrieved as a signed 32 bit integer.");
        }
        return value.integer_value;
    }

    int64_t TypedValue::as_int64_t() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kLong) {
            throw std::runtime_error("This value can not be retrieved as a signed 64 bit integer.");
        }
        return value.bigint_value;
    }

    Types::BigDecimal TypedValue::as_big_decimal() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal) {
            throw std::runtime_error("This value can not be retrieved as a big decimal.");
        }
        return *value.big_decimal_value;
    }

    float TypedValue::as_float() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kFloat) {
            throw std::runtime_error("This value can not be retrieved as a float.");
        }
        return value.float_value;
    }

    double TypedValue::as_double() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kDouble) {
            throw std::runtime_error("This value can not be retrieved as a double.");
        }
        return value.double_value;
    }

    std::chrono::system_clock::time_point TypedValue::as_date() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kDate) {
            throw std::runtime_error("This value can not be retrieved as a date.");
        }
        return value.date_value;
    }

    std::chrono::milliseconds TypedValue::as_time() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kTime) {
            throw std::runtime_error("This value can not be retrieved as a time.");
        }
        return value.time_value;
    }

    std::chrono::system_clock::time_point TypedValue::as_timestamp() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kTimestamp) {
            throw std::runtime_error("This value can not be retrieved as a timestamp.");
        }
        return value.timestamp_value;
    }

    Interval TypedValue::as_interval() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kInterval) {
            throw std::runtime_error("This value can not be retrieved as an interval.");
        }
        return *value.interval_value;
    }

    Document TypedValue::as_document() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kDocument) {
            throw std::runtime_error("This value can not be retrieved as a document.");
        }
        return *value.document_value;
    }

    std::list<TypedValue> TypedValue::as_list() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kList) {
            throw std::runtime_error("This value can not be retrieved as a list.");
        }
        return value.list_value;
    }

    std::string TypedValue::as_string() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kString) {
            throw std::runtime_error("This value can not be retrieved as a string.");
        }
        return value.varchar_value;
    }

    std::vector<uint8_t> TypedValue::as_bytes() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kBinary) {
            throw std::runtime_error("This value can not be retrieved as a vector of bytes.");
        }
        return value.binary_value;
    }

} // namespace Types
