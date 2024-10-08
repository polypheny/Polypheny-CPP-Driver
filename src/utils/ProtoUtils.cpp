//
// Created by tobi on 30.05.24.
//

#include "ProtoUtils.h"
#include "types/Interval.h"

namespace Utils::ProtoUtils {
    constexpr long long MILLIS_PER_DAY = 86400000LL;

    std::vector<uint8_t> string_to_vector(const std::string &string) {
        return {string.begin(), string.end()};
    }

    std::string vector_to_string(const std::vector<uint8_t> &vector) {
        return {vector.begin(), vector.end()};
    }

    std::chrono::system_clock::time_point
    proto_to_timestamp(const org::polypheny::prism::ProtoTimestamp &timestamp) {
        return std::chrono::system_clock::time_point(std::chrono::milliseconds(timestamp.timestamp()));
    }

    std::chrono::system_clock::time_point proto_to_date(const org::polypheny::prism::ProtoDate &date) {
        uint64_t millis_since_epoch = date.date() * MILLIS_PER_DAY;
        return std::chrono::system_clock::time_point(std::chrono::milliseconds(millis_since_epoch));
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
        uint64_t millis_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                date.time_since_epoch()).count();
        proto_date->set_date(millis_since_epoch / MILLIS_PER_DAY);
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
            *proto_list->add_values() = *typed_value.serialize(streaming_index);
        }
        return proto_list;
    }

    mpf_class bytes_to_mpf(const std::string &input) {
        mpz_t temp;
        mpz_init(temp);
        mpz_import(temp, input.size(), 1, sizeof(char), 0, 0, input.data());
        mpf_class result;
        mpf_set_z(result.get_mpf_t(), temp);
        mpz_clear(temp);
        return result;
    }

    void scale_by_10_to_neg_x(mpf_class &number, int32_t x) {
        mpf_class scale_factor;
        mpf_init_set_ui(scale_factor.get_mpf_t(), 10);
        mpf_pow_ui(scale_factor.get_mpf_t(), scale_factor.get_mpf_t(), x);
        number /= scale_factor;
    }


    std::pair<std::string, int32_t> mpf_get_unscaled_value_and_scale(const mpf_class &mpf_value) {
        mp_exp_t exponent;
        std::string str_val = mpf_value.get_str(exponent, 10);
        std::string::size_type point_pos = str_val.find('.');
        if (point_pos != std::string::npos) {
            str_val.erase(point_pos, 1);
        }
        mpz_class mantissa(str_val, 10);
        int32_t scale = str_val.length() - exponent;
        size_t count;
        auto binary_data = mpz_export(nullptr, &count, 1, 1, 0, 0, mantissa.get_mpz_t());
        std::string unscaled_value(reinterpret_cast<char *>(binary_data), count);
        free(binary_data);

        return std::make_pair(unscaled_value, scale);
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
