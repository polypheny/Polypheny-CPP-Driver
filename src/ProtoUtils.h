//
// Created by tobi on 30.05.24.
//

#ifndef POLYPHENY_CPP_DRIVER_PROTOUTILS_H
#define POLYPHENY_CPP_DRIVER_PROTOUTILS_H

#include <string>
#include <vector>
#include <cstdint>

namespace Utils {

    class ProtoUtils {
    public:
        static std::vector <uint8_t> string_to_vector(const std::string &string);
        static std::string vector_to_string(const std::vector <uint8_t> &vector);
    };
} // Utils

#endif //POLYPHENY_CPP_DRIVER_PROTOUTILS_H
