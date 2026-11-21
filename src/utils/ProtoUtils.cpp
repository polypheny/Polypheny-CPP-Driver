//
// Created by tobi on 30.05.24.
//

#include "ProtoUtils.h"
#include "types/Interval.h"
#include <limits>

namespace Utils::ProtoUtils {
    constexpr long long MILLIS_PER_DAY = 86400000LL;

    namespace {
        std::chrono::system_clock::time_point checked_time_point(int64_t millis) {
            using Duration = std::chrono::system_clock::duration;
            if constexpr (std::ratio_less_equal<typename Duration::period, std::milli>::value) {
                const auto minimum = std::chrono::duration_cast<std::chrono::milliseconds>(Duration::min()).count();
                const auto maximum = std::chrono::duration_cast<std::chrono::milliseconds>(Duration::max()).count();
                if (millis < minimum || millis > maximum) {
                    throw std::out_of_range("Prism date or timestamp exceeds system_clock's range");
                }
            }
            return std::chrono::system_clock::time_point(std::chrono::duration_cast<Duration>(std::chrono::milliseconds(millis)));
        }
    }

    std::vector<uint8_t> string_to_vector(const std::string &string) {
        return {string.begin(), string.end()};
    }

    std::string vector_to_string(const std::vector<uint8_t> &vector) {
        return {vector.begin(), vector.end()};
    }

    std::chrono::system_clock::time_point
    proto_to_timestamp(const org::polypheny::prism::ProtoTimestamp &timestamp) {
        return checked_time_point(timestamp.timestamp());
    }

    std::chrono::system_clock::time_point proto_to_date(const org::polypheny::prism::ProtoDate &date) {
        if (date.date() < std::numeric_limits<int64_t>::min() / MILLIS_PER_DAY
                || date.date() > std::numeric_limits<int64_t>::max() / MILLIS_PER_DAY) {
            throw std::out_of_range("Prism date exceeds the supported millisecond range");
        }
        return checked_time_point(date.date() * MILLIS_PER_DAY);
    }

    std::chrono::milliseconds proto_to_time(const org::polypheny::prism::ProtoTime &time) {
        return std::chrono::milliseconds(time.time());
    }

    std::list<Types::TypedValue> proto_to_list(const org::polypheny::prism::ProtoList &list,
                                               std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client) {
        google::protobuf::RepeatedPtrField<::org::polypheny::prism::ProtoValue> values = list.values();
        std::list<Types::TypedValue> result;
        for (const org::polypheny::prism::ProtoValue &value: values) {
            result.emplace_back(value, prism_interface_client);
        }
        return result;
    }

    std::unique_ptr<org::polypheny::prism::ProtoTimestamp>
    timestamp_to_proto(const std::chrono::system_clock::time_point &timestamp) {
        auto proto_timestamp = std::make_unique<org::polypheny::prism::ProtoTimestamp>();
        proto_timestamp->set_timestamp(
                std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count());
        return proto_timestamp;
    }

    std::unique_ptr<org::polypheny::prism::ProtoDate>
    date_to_proto(const std::chrono::system_clock::time_point &date) {
        auto proto_date = std::make_unique<org::polypheny::prism::ProtoDate>();
        using Days = std::chrono::duration<int64_t, std::ratio<86400>>;
        const auto days = std::chrono::floor<Days>(date.time_since_epoch()).count();
        proto_date->set_date(days);
        return proto_date;
    }


    std::unique_ptr<org::polypheny::prism::ProtoTime>
    time_to_proto(const std::chrono::milliseconds &time) {
        auto proto_time = std::make_unique<org::polypheny::prism::ProtoTime>();
        proto_time->set_time(time.count());
        return proto_time;
    }


    std::unique_ptr<org::polypheny::prism::ProtoList>
    list_to_proto(std::list<Types::TypedValue> &typed_values,
                  std::shared_ptr<Streaming::StreamingIndex> streaming_index) {
        auto proto_list = std::make_unique<org::polypheny::prism::ProtoList>();
        for (Types::TypedValue &typed_value: typed_values) {
            proto_list->mutable_values()->AddAllocated(typed_value.serialize(streaming_index));
        }
        return proto_list;
    }

    namespace {
        mpz_class signed_integer(const std::string &input) {
            mpz_class integer;
            mpz_import(integer.get_mpz_t(), input.size(), 1, 1, 0, 0, input.data());
            if (!input.empty() && (static_cast<unsigned char>(input.front()) & 0x80)) {
                integer -= mpz_class(1) << (input.size() * 8);
            }
            return integer;
        }

        std::string signed_bytes(const mpz_class &integer) {
            const auto size = (mpz_sizeinbase(integer.get_mpz_t(), 2) + 7) / 8;
            mpz_class encoded = integer;
            if (integer < 0) {
                encoded += mpz_class(1) << (size * 8);
            }
            std::string bytes(size, '\0');
            size_t count = 0;
            mpz_export(bytes.data(), &count, 1, 1, 0, 0, encoded.get_mpz_t());
            bytes.resize(count);
            if (bytes.empty()) {
                return std::string(1, '\0');
            }
            const bool high_bit = (static_cast<unsigned char>(bytes.front()) & 0x80) != 0;
            if ((integer < 0) != high_bit) {
                bytes.insert(bytes.begin(), integer < 0 ? char(0xff) : char(0));
            }
            return bytes;
        }
    }

    mpf_class bytes_to_mpf(const std::string &input) {
        const auto integer = signed_integer(input);
        return mpf_class(integer, std::max<mp_bitcnt_t>(64, input.size() * 8 + 64));
    }

    void scale_by_10_to_neg_x(mpf_class &number, int32_t x) {
        mpf_class scale_factor(10, number.get_prec());
        const auto exponent = x < 0 ? -static_cast<int64_t>(x) : static_cast<int64_t>(x);
        mpf_pow_ui(scale_factor.get_mpf_t(), scale_factor.get_mpf_t(), exponent);
        if (x < 0) {
            number *= scale_factor;
        } else {
            number /= scale_factor;
        }
    }


    std::pair<std::string, int32_t> mpf_get_unscaled_value_and_scale(const mpf_class &mpf_value) {
        mp_exp_t exponent;
        std::string str_val = mpf_value.get_str(exponent, 10);
        if (str_val.empty()) {
            return {std::string(1, '\0'), 0};
        }
        mpz_class mantissa(str_val, 10);
        const auto digits = str_val.size() - (str_val.front() == '-' ? 1 : 0);
        const auto scale = static_cast<int64_t>(digits) - exponent;
        if (scale < std::numeric_limits<int32_t>::min() || scale > std::numeric_limits<int32_t>::max()) {
            throw std::out_of_range("Decimal scale exceeds Prism's signed 32-bit range");
        }
        return {signed_bytes(mantissa), static_cast<int32_t>(scale)};
    }

    std::string decimal_to_string(const org::polypheny::prism::ProtoBigDecimal &decimal) {
        auto digits = signed_integer(decimal.unscaled_value()).get_str();
        const int64_t scale = decimal.scale();
        if (scale == 0) {
            return digits;
        }
        // Avoid allocating gigabytes for a valid extreme-scale value.
        if (scale > 10000 || scale < -10000) {
            return digits + "e" + std::to_string(-scale);
        }
        const bool negative = digits.front() == '-';
        if (negative) {
            digits.erase(digits.begin());
        }
        if (scale < 0) {
            digits.append(static_cast<size_t>(-scale), '0');
        } else if (static_cast<size_t>(scale) >= digits.size()) {
            digits = "0." + std::string(static_cast<size_t>(scale) - digits.size(), '0') + digits;
        } else {
            digits.insert(digits.size() - scale, 1, '.');
        }
        return (negative ? "-" : "") + digits;
    }


    std::vector<uint8_t> collect_file(Types::File &stream) {
        std::vector<uint8_t> collected_data;
        std::vector<char> buffer(Types::File::BUFFER_SIZE);
        while (true) {
            std::streamsize bytes_read = stream.sgetn(buffer.data(), buffer.size());
            if (bytes_read > 0) {
                collected_data.insert(collected_data.end(), buffer.begin(), buffer.begin() + bytes_read);
            }
            if (bytes_read < static_cast<std::streamsize>(buffer.size())) {
                break;
            }
            if (bytes_read == 0) {
                break;
            }
        }
        return collected_data;
    }

    std::string collect_string(Streaming::StringPrismInputStream &stream) {
        std::string collected_data;
        std::vector<char> buffer(Streaming::StringPrismInputStream::BUFFER_SIZE);

        while (true) {
            std::streamsize bytes_read = stream.sgetn(buffer.data(), buffer.size());
            if (bytes_read > 0) {
                collected_data.append(buffer.data(), bytes_read);
            }
            if (bytes_read < static_cast<std::streamsize>(buffer.size())) {
                break;
            }
            if (bytes_read == 0) {
                break;
            }
        }

        return collected_data;
    }
}
