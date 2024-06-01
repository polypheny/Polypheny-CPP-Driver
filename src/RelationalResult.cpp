//
// Created by tobi on 01.06.24.
//

#include "RelationalResult.h"

namespace Results {
    RelationalResult::RelationalResult(const org::polypheny::prism::RelationalFrame &relational_frame) : Result(
            ResultType::RELATIONAL) {}
} // Results