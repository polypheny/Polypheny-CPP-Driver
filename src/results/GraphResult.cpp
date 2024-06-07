//
// Created by tobi on 01.06.24.
//

#include "GraphResult.h"

namespace Results {
    GraphResult::GraphResult(const org::polypheny::prism::GraphFrame &graph_frame, bool is_last,
                             Connection::Cursor *cursor) : Result(ResultType::GRAPH) {

    }
} // Results