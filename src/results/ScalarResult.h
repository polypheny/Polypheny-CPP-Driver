//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_SCALARRESULT_H
#define POLYPHENY_CPP_DRIVER_SCALARRESULT_H

#include "Result.h"

namespace Results {

    /**
     * @brief Result containing a single scalar value like an update count.
     *
     * This class represents a results that consists of a single scalar value. Such results are returned on data manipulation statements where the number of affected entries is returned.
     */
    class ScalarResult : public Result {
    public:
        explicit ScalarResult(uint64_t scalar);

        /**
        * @brief Retrieve the value of the scalar result.
        */
        uint64_t get_scalar();

    private:
        uint64_t scalar;
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_SCALARRESULT_H
