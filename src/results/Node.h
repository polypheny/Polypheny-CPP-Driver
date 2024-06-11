//
// Created by tobi on 12.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_NODE_H
#define POLYPHENY_CPP_DRIVER_NODE_H

#include "graph_frame.pb.h"
#include "results/GraphElement.h"

namespace Results {

    class Node : public GraphElement {
    public:
        Node(org::polypheny::prism::ProtoNode proto_edge);
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_NODE_H
