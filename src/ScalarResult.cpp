//
// Created by tobi on 01.06.24.
//

#include "ScalarResult.h"

namespace Results {
    ScalarResult::ScalarResult(uint64_t scalar) : Result(ResultType::SCALAR), scalar(scalar) {}
} // Results