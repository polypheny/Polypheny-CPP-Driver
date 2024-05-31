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

    std::list<TypedValue> ProtoUtils::proto_to_list(const org::polypheny::prism::ProtoList &list) {
        google::protobuf::RepeatedPtrField<::org::polypheny::prism::ProtoValue> values = list.values();
        std::list<TypedValue> result;
        for (const org::polypheny::prism::ProtoValue &value: values) {
            result.emplace_back(value);
        }
        return result;
    }

    org::polypheny::prism::ProtoTimestamp
    ProtoUtils::timestamp_to_proto(const std::chrono::system_clock::time_point &timestamp) {
        org::polypheny::prism::ProtoTimestamp proto_timestamp;
        proto_timestamp.set_timestamp(
                std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count());
        return proto_timestamp;
    }

    org::polypheny::prism::ProtoDate ProtoUtils::date_to_proto(const std::chrono::system_clock::time_point &date) {
        org::polypheny::prism::ProtoDate proto_date;
        uint64_t millis_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                date.time_since_epoch()).count();
        proto_date.set_date(millis_since_epoch / MILLIS_PER_DAY);
        return proto_date;
    }

    org::polypheny::prism::ProtoTime ProtoUtils::time_to_proto(const std::chrono::milliseconds &time) {
        org::polypheny::prism::ProtoTime proto_time;
        proto_time.set_time(time.count());
        return proto_time;
    }

    org::polypheny::prism::ProtoList ProtoUtils::list_to_proto(const std::list<TypedValue> &typed_values) {
        org::polypheny::prism::ProtoList proto_list;
        for (const Types::TypedValue &typed_value: typed_values) {
            *proto_list.add_values() = typed_value.serialize();
        }
        return proto_list;
    }

}