//
// Created by tobi on 30.05.24.
//

#include "types/TypedValue.h"
#include "types/Interval.h"
#include "utils/TypedValueUtils.h"
#include "utils/ProtoUtils.h"

namespace Types {

    TypedValue::TypedValue(const org::polypheny::prism::ProtoValue &proto_value)
            : serialized(std::make_shared<org::polypheny::prism::ProtoValue>(proto_value)),
              is_serialized(true),
              is_deserialized(false),
              value_case(proto_value.value_case()) {}

    TypedValue::TypedValue(bool value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kBoolean),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(int32_t value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kInteger),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(int64_t value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kLong),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const mpf_class &value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(float value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kFloat),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(double value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kDouble),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const std::chrono::system_clock::time_point &value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kDate),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const std::chrono::milliseconds &value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kTime),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const std::chrono::system_clock::time_point &timestamp_value, bool is_timestamp)
            : value(timestamp_value),
              value_case(is_timestamp ? org::polypheny::prism::ProtoValue::ValueCase::kTimestamp
                                      : org::polypheny::prism::ProtoValue::ValueCase::kDate),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const Types::Interval &value)
            : value(std::make_unique<Types::Interval>(value)),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kInterval),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const std::unordered_map<std::string, TypedValue> &document_value)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kDocument),
              is_serialized(false),
              is_deserialized(true) {
        std::unordered_map<std::string, std::unique_ptr<TypedValue>> ptr_map;
        for (const auto &pair: document_value) {
            ptr_map.emplace(pair.first, std::make_unique<TypedValue>(pair.second));
        }
        value = std::move(ptr_map);
    }

    TypedValue::TypedValue()
            : value(std::monostate{}),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kNull),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const std::list<TypedValue> &values)
            : value_case(org::polypheny::prism::ProtoValue::ValueCase::kList),
              is_serialized(false),
              is_deserialized(true) {
        std::list<std::unique_ptr<TypedValue>> ptr_values;
        for (const auto &val: values) {
            ptr_values.emplace_back(std::make_unique<TypedValue>(val));
        }
        value = std::move(ptr_values);
    }

    TypedValue::TypedValue(const std::string &value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kString),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const std::vector<uint8_t> &value)
            : value(value),
              value_case(org::polypheny::prism::ProtoValue::ValueCase::kBinary),
              is_serialized(false),
              is_deserialized(true) {}

    TypedValue::TypedValue(const TypedValue &other)
            : value_case(other.value_case),
              is_serialized(other.is_serialized),
              is_deserialized(other.is_deserialized) {
        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                value = std::get<bool>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                value = std::get<int32_t>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                value = std::get<int64_t>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                value = std::get<mpf_class>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                value = std::get<float>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                value = std::get<double>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                value = std::get<std::chrono::system_clock::time_point>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                value = std::get<std::chrono::milliseconds>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                value = std::get<std::chrono::system_clock::time_point>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                value = std::make_unique<Interval>(*std::get<std::unique_ptr<Interval>>(other.value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                value = std::get<std::string>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                value = std::get<std::vector<uint8_t>>(other.value);
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList: {
                std::list<std::unique_ptr<TypedValue>> ptr_list;
                for (const auto &item: std::get<std::list<std::unique_ptr<TypedValue>>>(other.value)) {
                    ptr_list.emplace_back(std::make_unique<TypedValue>(*item));
                }
                value = std::move(ptr_list);
            }
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument: {
                std::unordered_map<std::string, std::unique_ptr<TypedValue>> ptr_map;
                for (const auto &pair: std::get<std::unordered_map<std::string, std::unique_ptr<TypedValue>>>(
                        other.value)) {
                    ptr_map.emplace(pair.first, std::make_unique<TypedValue>(*pair.second));
                }
                value = std::move(ptr_map);
            }
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                value = std::monostate{};
                break;
            default:
                throw std::runtime_error("Unsupported value case in TypedValue copy constructor");
        }

        if (other.is_serialized) {
            serialized = std::make_shared<org::polypheny::prism::ProtoValue>(*other.serialized);
        }
    }

/*
    TypedValue::TypedValue(TypedValue &&other) noexcept
            : value_case(other.value_case),
              is_serialized(other.is_serialized),
              is_deserialized(other.is_deserialized),
              serialized(std::move(other.serialized)) {
        value = std::move(other.value);
        other.value_case = org::polypheny::prism::ProtoValue::ValueCase::VALUE_NOT_SET;
    }
    */


    TypedValue &TypedValue::operator=(const TypedValue &other) {
        if (this != &other) {
            value_case = other.value_case;
            is_serialized = other.is_serialized;
            is_deserialized = other.is_deserialized;

            switch (value_case) {
                case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                    value = std::get<bool>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                    value = std::get<int32_t>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                    value = std::get<int64_t>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                    value = std::get<mpf_class>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                    value = std::get<float>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                    value = std::get<double>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                    value = std::get<std::chrono::system_clock::time_point>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                    value = std::get<std::chrono::milliseconds>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                    value = std::get<std::chrono::system_clock::time_point>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                    value = std::make_unique<Interval>(*std::get<std::unique_ptr<Interval>>(other.value));
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kString:
                    value = std::get<std::string>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                    value = std::get<std::vector<uint8_t>>(other.value);
                    break;
                case org::polypheny::prism::ProtoValue::ValueCase::kList: {
                    std::list<std::unique_ptr<TypedValue>> ptr_list;
                    for (const auto &item: std::get<std::list<std::unique_ptr<TypedValue>>>(other.value)) {
                        ptr_list.emplace_back(std::make_unique<TypedValue>(*item));
                    }
                    value = std::move(ptr_list);
                    break;
                }
                case org::polypheny::prism::ProtoValue::ValueCase::kDocument: {
                    std::unordered_map<std::string, std::unique_ptr<TypedValue>> ptr_map;
                    for (const auto &pair: std::get<std::unordered_map<std::string, std::unique_ptr<TypedValue>>>(
                            other.value)) {
                        ptr_map.emplace(pair.first, std::make_unique<TypedValue>(*pair.second));
                    }
                    value = std::move(ptr_map);
                    break;
                }
                case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                    value = std::monostate{};
                    break;
                default:
                    throw std::runtime_error("Unsupported value case in TypedValue copy assignment operator");
            }

            if (other.is_serialized) {
                serialized = std::make_shared<org::polypheny::prism::ProtoValue>(*other.serialized);
            }
        }
        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const TypedValue &typed_value) {
        return Utils::TypedValueUtils::write_typed_value_to_stream(os, const_cast<TypedValue&>(typed_value));
    }

    std::shared_ptr<org::polypheny::prism::ProtoValue> TypedValue::serialize() {
        if (is_serialized) {
            return serialized;
        }
        auto proto_value = std::make_shared<org::polypheny::prism::ProtoValue>();

        switch (value_case) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                proto_value->mutable_boolean()->set_boolean(std::get<bool>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                proto_value->mutable_integer()->set_integer(std::get<int32_t>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                proto_value->mutable_long_()->set_long_(std::get<int64_t>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal: {
                auto unscale_value_and_scale = Utils::ProtoUtils::mpf_get_unscaled_value_and_scale(
                        std::get<mpf_class>(value));
                auto *big_decimal = proto_value->mutable_big_decimal();
                big_decimal->set_unscaled_value(unscale_value_and_scale.first);
                big_decimal->set_scale(unscale_value_and_scale.second);
                break;
            }
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                proto_value->mutable_float_()->set_float_(std::get<float>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                proto_value->mutable_double_()->set_double_(std::get<double>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                *proto_value->mutable_date() = *Utils::ProtoUtils::date_to_proto(
                        std::get<std::chrono::system_clock::time_point>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                *proto_value->mutable_time() = *Utils::ProtoUtils::time_to_proto(
                        std::get<std::chrono::milliseconds>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                *proto_value->mutable_timestamp() = *Utils::ProtoUtils::timestamp_to_proto(
                        std::get<std::chrono::system_clock::time_point>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                *proto_value->mutable_interval() = *std::get<std::unique_ptr<Interval>>(value)->serialize();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                proto_value->mutable_string()->set_string(std::get<std::string>(value));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                proto_value->mutable_binary()->set_binary(
                        Utils::ProtoUtils::vector_to_string(std::get<std::vector<uint8_t>>(value)));
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList: {
                auto proto_list = proto_value->mutable_list();
                for (const auto &item: std::get<std::list<std::unique_ptr<TypedValue>>>(value)) {
                    *proto_list->add_values() = *item->serialize();
                }
                break;
            }
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument: {
                auto proto_document = proto_value->mutable_document();
                auto *proto_entries = proto_document->mutable_entries();
                for (const auto &pair: std::get<std::unordered_map<std::string, std::unique_ptr<TypedValue>>>(value)) {
                    auto *proto_entry = proto_entries->Add();
                    proto_entry->mutable_key()->mutable_string()->set_string(pair.first);
                    *proto_entry->mutable_value() = *pair.second->serialize();
                }
                break;
            }
            case org::polypheny::prism::ProtoValue::ValueCase::kFile:
                proto_value->mutable_file()->set_binary(
                        Utils::ProtoUtils::vector_to_string(std::get<std::vector<uint8_t>>(value)));
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
                value = serialized->boolean().boolean();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                value = serialized->integer().integer();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                value = serialized->long_().long_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal: {
                mpf_class big_decimal = Utils::ProtoUtils::bytes_to_mpf(serialized->big_decimal().unscaled_value());
                Utils::ProtoUtils::scale_by_10_to_neg_x(big_decimal, serialized->big_decimal().scale());
                value = big_decimal;
                break;
            }
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                value = serialized->float_().float_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                value = serialized->double_().double_();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                value = Utils::ProtoUtils::proto_to_date(serialized->date());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                value = Utils::ProtoUtils::proto_to_time(serialized->time());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                value = Utils::ProtoUtils::proto_to_timestamp(serialized->timestamp());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                value = std::make_unique<Types::Interval>(serialized->interval());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                value = serialized->string().string();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                value = Utils::ProtoUtils::string_to_vector(serialized->binary().binary());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList: {
                std::list<std::unique_ptr<TypedValue>> list_value;
                for (const auto &proto_value: serialized->list().values()) {
                    list_value.emplace_back(std::make_unique<TypedValue>(proto_value));
                }
                value = std::move(list_value);
                break;
            }
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument: {
                std::unordered_map<std::string, std::unique_ptr<TypedValue>> map_value;
                for (auto &entry: serialized->document().entries()) {
                    map_value.emplace(entry.key().string().string(), std::make_unique<TypedValue>(entry.value()));
                }
                value = std::move(map_value);
                break;
            }
            case org::polypheny::prism::ProtoValue::ValueCase::kFile:
                value = Utils::ProtoUtils::string_to_vector(serialized->file().binary());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                value = std::monostate{};
                break;
            default:
                throw std::runtime_error("Encountered unknown data type");
        }
        is_deserialized = true;
    }

    bool TypedValue::is_null() {
        return value_case == org::polypheny::prism::ProtoValue::ValueCase::kNull;
    }

    bool TypedValue::as_boolean() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kBoolean) {
            throw std::runtime_error("This value can not be retrieved as a boolean.");
        }
        return std::get<bool>(value);
    }

    int32_t TypedValue::as_int32_t() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kInteger) {
            throw std::runtime_error("This value can not be retrieved as a signed 32 bit integer.");
        }
        return std::get<int32_t>(value);
    }

    int64_t TypedValue::as_int64_t() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kLong) {
            throw std::runtime_error("This value can not be retrieved as a signed 64 bit integer.");
        }
        return std::get<int64_t>(value);
    }

    mpf_class TypedValue::as_big_decimal() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal) {
            throw std::runtime_error("This value can not be retrieved as a big decimal.");
        }
        return std::get<mpf_class>(value);
    }

    float TypedValue::as_float() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kFloat) {
            throw std::runtime_error("This value can not be retrieved as a float.");
        }
        return std::get<float>(value);
    }

    double TypedValue::as_double() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kDouble) {
            throw std::runtime_error("This value can not be retrieved as a double.");
        }
        return std::get<double>(value);
    }

    std::chrono::system_clock::time_point TypedValue::as_date() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kDate) {
            throw std::runtime_error("This value can not be retrieved as a date.");
        }
        return std::get<std::chrono::system_clock::time_point>(value);
    }

    std::chrono::milliseconds TypedValue::as_time() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kTime) {
            throw std::runtime_error("This value can not be retrieved as a time.");
        }
        return std::get<std::chrono::milliseconds>(value);
    }

    std::chrono::system_clock::time_point TypedValue::as_timestamp() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kTimestamp) {
            throw std::runtime_error("This value can not be retrieved as a timestamp.");
        }
        return std::get<std::chrono::system_clock::time_point>(value);
    }

    Types::Interval TypedValue::as_interval() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kInterval) {
            throw std::runtime_error("This value can not be retrieved as an interval.");
        }
        return *std::get<std::unique_ptr<Interval>>(value);
    }

    std::unordered_map<std::string, TypedValue> TypedValue::as_document() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kDocument) {
            throw std::runtime_error("This value cannot be retrieved as a map.");
        }

        std::unordered_map<std::string, TypedValue> result;
        for (const auto &pair: std::get<std::unordered_map<std::string, std::unique_ptr<TypedValue>>>(value)) {
            result.emplace(pair.first, *pair.second);
        }
        return result;
    }

    std::list<TypedValue> TypedValue::as_list() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kList) {
            throw std::runtime_error("This value can not be retrieved as a list.");
        }
        std::list<TypedValue> result;
        for (const auto &item: std::get<std::list<std::unique_ptr<TypedValue>>>(value)) {
            result.emplace_back(*item);
        }
        return result;
    }

    std::string TypedValue::as_string() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kString) {
            throw std::runtime_error("This value can not be retrieved as a string.");
        }
        return std::get<std::string>(value);
    }

    std::vector<uint8_t> TypedValue::as_bytes() {
        if (!is_deserialized) {
            deserialize();
        }
        if (value_case != org::polypheny::prism::ProtoValue::ValueCase::kBinary) {
            throw std::runtime_error("This value can not be retrieved as a vector of bytes.");
        }
        return std::get<std::vector<uint8_t>>(value);
    }

    org::polypheny::prism::ProtoValue::ValueCase TypedValue::get_value_case() {
        return value_case;
    }


} // namespace Types
