//
// Created by tobi on 01.06.24.
//

#include "GraphResult.h"

namespace Results {
    GraphResult::GraphResult(const org::polypheny::prism::GraphFrame &graph_frame) : Result(ResultType::GRAPH) {}
} // Results