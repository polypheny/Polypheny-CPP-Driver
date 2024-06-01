//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_GRAPHRESULT_H
#define POLYPHENY_CPP_DRIVER_GRAPHRESULT_H

#include "Result.h"
#include "graph_frame.pb.h"

namespace Results {

    class GraphResult : public Result {
    public:
        GraphResult(const org::polypheny::prism::GraphFrame &graph_frame);
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_GRAPHRESULT_H
