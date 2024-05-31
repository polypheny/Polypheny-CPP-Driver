//
// Created by tobi on 30.05.24.
//

#ifndef POLYPHENY_CPP_DRIVER_PROTOUTILS_H
#define POLYPHENY_CPP_DRIVER_PROTOUTILS_H

#include <chrono>
#include <string>
#include <vector>
#include <cstdint>
#include "value.pb.h"
#include"TypedValue.h"

namespace Utils {

    class ProtoUtils {
    public:
        static std::vector<uint8_t> string_to_vector(const std::string &string);

        static std::string vector_to_string(const std::vector<uint8_t> &vector);

        static std::chrono::system_clock::time_point
        proto_to_timestamp(const org::polypheny::prism::ProtoTimestamp &timestamp);

        static std::chrono::milliseconds proto_to_time(const org::polypheny::prism::ProtoTime &time);

        static std::chrono::system_clock::time_point proto_to_date(const org::polypheny::prism::ProtoDate &date);

        static std::list<TypedValue> proto_to_list(const org::polypheny::prism::ProtoList &list);

        static org::polypheny::prism::ProtoTimestamp
        timestamp_to_proto(const std::chrono::system_clock::time_point &timestamp);

        static org::polypheny::prism::ProtoDate date_to_proto(const std::chrono::system_clock::time_point &date);

        static org::polypheny::prism::ProtoTime time_to_proto(const std::chrono::milliseconds &time);

        static org::polypheny::prism::ProtoList list_to_proto(const std::list<TypedValue> &typed_values);
    };
} // Utils

#endif //POLYPHENY_CPP_DRIVER_PROTOUTILS_H
