//
// Created by tobi on 11.06.24.
//

#include "utils/TypedValueUtils.h"
#include "types/Interval.h"
#include <iostream>
#include <chrono>
#include <list>
#include <vector>
#include <iomanip>
#include <ctime>
#include <gmpxx.h>

namespace Utils::TypedValueUtils {

    std::ostream &write_typed_value_to_stream(std::ostream &os, Types::TypedValue &typed_value) {
        switch (typed_value.get_value_case()) {
            case org::polypheny::prism::ProtoValue::ValueCase::kBoolean:
                os << std::boolalpha << typed_value.as_boolean();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInteger:
                os << typed_value.as_int32_t();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kLong:
                os << typed_value.as_int64_t();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBigDecimal:
                os << typed_value.as_big_decimal();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kFloat:
                os << typed_value.as_float();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDouble:
                os << typed_value.as_double();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDate:
                write_date_to_stream(os, typed_value.as_date());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTime:
                write_time_to_stream(os, typed_value.as_time());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kTimestamp:
                write_timestamp_to_stream(os, typed_value.as_timestamp());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kInterval:
                os << typed_value.as_interval();
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kString:
                os << "\"" << typed_value.as_string() << "\"";
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kBinary:
                os << bytes_to_hex_string(typed_value.as_bytes());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kList:
                write_list_to_stream(os, typed_value.as_list());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kDocument: {
                auto map = typed_value.as_document();
                write_map_to_stream(os, map);
                break;
            }
            case org::polypheny::prism::ProtoValue::ValueCase::kFile:
                write_file_to_stream(os, typed_value.as_bytes());
                break;
            case org::polypheny::prism::ProtoValue::ValueCase::kNull:
                os << "null";
                break;
            default:
                throw std::runtime_error("Encountered unknown data type");
        }
        return os;
    }

    std::tm to_tm(const std::chrono::system_clock::time_point &time_point) {
        std::time_t time_t = std::chrono::system_clock::to_time_t(time_point);
        std::tm tm;
#ifdef _WIN32
        gmtime_s(&tm, &time_t);
#else
        gmtime_r(&time_t, &tm);
#endif
        return tm;
    }

    std::ostream &write_date_to_stream(std::ostream &os, const std::chrono::system_clock::time_point &value) {
        std::tm tm = to_tm(value);
        os << std::put_time(&tm, "%Y-%m-%d");
        return os;
    }

    std::ostream &write_time_to_stream(std::ostream &os, const std::chrono::milliseconds &value) {
        auto hours = std::chrono::duration_cast<std::chrono::hours>(value);
        auto minutes = std::chrono::duration_cast<std::chrono::minutes>(value % std::chrono::hours(1));
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(value % std::chrono::minutes(1));
        auto milliseconds = value % std::chrono::seconds(1);

        os << std::setw(2) << std::setfill('0') << hours.count() << ":"
           << std::setw(2) << std::setfill('0') << minutes.count() << ":"
           << std::setw(2) << std::setfill('0') << seconds.count() << "."
           << std::setw(3) << std::setfill('0') << milliseconds.count();
        return os;
    }

    std::ostream &write_timestamp_to_stream(std::ostream &os, const std::chrono::system_clock::time_point &value) {
        std::tm tm = to_tm(value);
        auto time_since_epoch = value.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(time_since_epoch) - seconds;

        os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "."
           << std::setw(3) << std::setfill('0') << milliseconds.count();
        return os;
    }

    std::ostream &write_list_to_stream(std::ostream &os, std::list<Types::TypedValue> value) {
        os << "[";
        bool first = true;
        for (auto &item: value) {
            if (!first) {
                os << ", ";
            }
            first = false;
            write_typed_value_to_stream(os, item);
        }
        os << "]";
        return os;
    }

    std::ostream &write_binary_to_stream(std::ostream &os, const std::vector<uint8_t> &value) {
        os << std::hex << std::setfill('0');
        for (const auto &byte: value) {
            os << std::setw(2) << static_cast<int>(byte);
        }
        return os;
    }

    std::ostream &write_file_to_stream(std::ostream &os, const std::vector<uint8_t> &value) {
        return write_binary_to_stream(os, value);
    }

    std::ostream &write_map_to_stream(std::ostream &os, std::unordered_map<std::string, Types::TypedValue> &map) {
        os << "{";
        bool first = true;
        for (auto &pair : map) {
            if (!first) {
                os << ", ";
            }
            first = false;
            os << "\"" << pair.first << "\": ";
            write_typed_value_to_stream(os, pair.second);
        }
        os << "}";
        return os;
    }

    std::string bytes_to_hex_string(const std::vector<uint8_t>& bytes) {
        std::ostringstream oss;
        for (const auto& byte : bytes) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        return oss.str();
    }
}