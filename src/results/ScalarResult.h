//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_SCALARRESULT_H
#define POLYPHENY_CPP_DRIVER_SCALARRESULT_H

#include "Result.h"

namespace Results {

    class ScalarResult : public Result {
    public:
        explicit ScalarResult(uint64_t scalar);
        uint64_t get_scalar();

    private:
        uint64_t scalar;
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_SCALARRESULT_H
