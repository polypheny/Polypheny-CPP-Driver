//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H
#define POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H

#include "Result.h"
#include "relational_frame.pb.h"

namespace Results {

    class RelationalResult : public Result {
    public:
        RelationalResult(const org::polypheny::prism::RelationalFrame &relational_frame);
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H
