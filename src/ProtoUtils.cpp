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

    std::chrono::system_clock::time_point ProtoUtils::proto_to_timestamp(const org::polypheny::prism::ProtoTimestamp& timestamp) {
        return std::chrono::system_clock::time_point(std::chrono::milliseconds(timestamp.timestamp()));
    }

    std::chrono::system_clock::time_point ProtoUtils::proto_to_date(const org::polypheny::prism::ProtoDate& date) {
        uint64_t millis_since_epoch = date.date() * MILLIS_PER_DAY;
        return std::chrono::system_clock::time_point(std::chrono::milliseconds(millis_since_epoch));
    }

    std::chrono::milliseconds ProtoUtils::proto_to_time(const org::polypheny::prism::ProtoTime& time) {
        return std::chrono::milliseconds(time.time());
    }


}