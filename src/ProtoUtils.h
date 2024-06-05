#ifndef POLYPHENY_CPP_DRIVER_PROTOUTILS_H
#define POLYPHENY_CPP_DRIVER_PROTOUTILS_H

#include <chrono>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include "value.pb.h"
#include "TypedValue.h"

namespace Types {
    // forward declarations
    class TypedValue;
}

namespace Utils {

    class ProtoUtils {
    public:
        static std::vector<uint8_t> string_to_vector(const std::string &string);

        static std::string vector_to_string(const std::vector<uint8_t> &vector);

        static std::chrono::system_clock::time_point
        proto_to_timestamp(const org::polypheny::prism::ProtoTimestamp &timestamp);

        static std::chrono::milliseconds proto_to_time(const org::polypheny::prism::ProtoTime &time);

        static std::chrono::system_clock::time_point proto_to_date(const org::polypheny::prism::ProtoDate &date);

        static std::list<Types::TypedValue> proto_to_list(const org::polypheny::prism::ProtoList &list);

        static std::unique_ptr<org::polypheny::prism::ProtoTimestamp>
        timestamp_to_proto(const std::chrono::system_clock::time_point &timestamp);

        static std::unique_ptr<org::polypheny::prism::ProtoDate>
        date_to_proto(const std::chrono::system_clock::time_point &date);

        static std::unique_ptr<org::polypheny::prism::ProtoTime> time_to_proto(const std::chrono::milliseconds &time);

        static std::unique_ptr<org::polypheny::prism::ProtoList>
        list_to_proto(std::list<Types::TypedValue> &typed_values);

    };

} // namespace Utils

#endif // POLYPHENY_CPP_DRIVER_PROTOUTILS_H
