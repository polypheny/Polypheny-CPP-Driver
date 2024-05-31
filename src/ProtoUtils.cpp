//
// Created by tobi on 30.05.24.
//

#include "ProtoUtils.h"

namespace Utils {
    constexpr long long MILLIS_PER_DAY = 86400000LL;

    std::vector<uint8_t> ProtoUtils::string_to_vector(const std::string &string) {
        return {string.begin(), string.end()};
    }

    std::string ProtoUtils::vector_to_string(const std::vector<uint8_t> &vector) {
        return {vector.begin(), vector.end()};
    }

    std::chrono::system_clock::time_point
    ProtoUtils::proto_to_timestamp(const org::polypheny::prism::ProtoTimestamp &timestamp) {
        return std::chrono::system_clock::time_point(std::chrono::milliseconds(timestamp.timestamp()));
    }

    std::chrono::system_clock::time_point ProtoUtils::proto_to_date(const org::polypheny::prism::ProtoDate &date) {
        uint64_t millis_since_epoch = date.date() * MILLIS_PER_DAY;
        return std::chrono::system_clock::time_point(std::chrono::milliseconds(millis_since_epoch));
    }

    std::chrono::milliseconds ProtoUtils::proto_to_time(const org::polypheny::prism::ProtoTime &time) {
        return std::chrono::milliseconds(time.time());
    }

    std::list<Types::TypedValue> ProtoUtils::proto_to_list(const org::polypheny::prism::ProtoList &list) {
        google::protobuf::RepeatedPtrField<::org::polypheny::prism::ProtoValue> values = list.values();
        std::list<Types::TypedValue> result;
        for (const org::polypheny::prism::ProtoValue &value: values) {
            result.emplace_back(value);
        }
        return result;
    }

    std::unique_ptr<org::polypheny::prism::ProtoTimestamp>
    Utils::ProtoUtils::timestamp_to_proto(const std::chrono::system_clock::time_point &timestamp) {
        auto proto_timestamp = std::make_unique<org::polypheny::prism::ProtoTimestamp>();
        proto_timestamp->set_timestamp(
                std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count());
        return proto_timestamp;
    }

    std::unique_ptr<org::polypheny::prism::ProtoDate>
    Utils::ProtoUtils::date_to_proto(const std::chrono::system_clock::time_point &date) {
        auto proto_date = std::make_unique<org::polypheny::prism::ProtoDate>();
        uint64_t millis_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                date.time_since_epoch()).count();
        proto_date->set_date(millis_since_epoch / MILLIS_PER_DAY);
        return proto_date;
    }


    std::unique_ptr<org::polypheny::prism::ProtoTime>
    Utils::ProtoUtils::time_to_proto(const std::chrono::milliseconds &time) {
        auto proto_time = std::make_unique<org::polypheny::prism::ProtoTime>();
        proto_time->set_time(time.count());
        return proto_time;
    }


    std::unique_ptr<org::polypheny::prism::ProtoList>
    Utils::ProtoUtils::list_to_proto(const std::list<Types::TypedValue> &typed_values) {
        auto proto_list = std::make_unique<org::polypheny::prism::ProtoList>();
        for (const Types::TypedValue &typed_value: typed_values) {
            *proto_list->add_values() = *typed_value.serialize(); // Assuming serialize returns a unique_ptr
        }
        return proto_list;
    }

}