//
// Created by tobi on 11.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_TYPEDVALUEUTILS_H
#define POLYPHENY_CPP_DRIVER_TYPEDVALUEUTILS_H

#include "types/TypedValue.h"

namespace Utils::TypedValueUtils {
    std::ostream &write_typed_value_to_stream(std::ostream &os, Types::TypedValue &typed_value);

    std::tm to_tm(const std::chrono::system_clock::time_point &time_point);

    std::ostream &write_date_to_stream(std::ostream &os, const std::chrono::system_clock::time_point &value);

    std::ostream &write_time_to_stream(std::ostream &os, const std::chrono::milliseconds &value);

    std::ostream &write_timestamp_to_stream(std::ostream &os, const std::chrono::system_clock::time_point &value);

    std::ostream &write_list_to_stream(std::ostream &os, std::list<Types::TypedValue> value);

    std::ostream &write_binary_to_stream(std::ostream &os, const std::vector<uint8_t> &value);

    std::ostream &write_file_to_stream(std::ostream &os, const std::vector<uint8_t> &value);

    std::ostream &write_map_to_stream(std::ostream &os, std::unordered_map<std::string, Types::TypedValue> &value);
}

#endif //POLYPHENY_CPP_DRIVER_TYPEDVALUEUTILS_H
