//
// Created by tobi on 12.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_EDGE_H
#define POLYPHENY_CPP_DRIVER_EDGE_H

#include "results/GraphElement.h"
#include "graph_frame.pb.h"

namespace Results {

    class Edge : public GraphElement {
    private:
        std::string source;
        std::string target;
        org::polypheny::prism::ProtoEdge_Direction direction;

    public:
        Edge(const org::polypheny::prism::ProtoEdge &proto_edge);

        const std::string &get_source() const;

        const std::string &get_target() const;

        org::polypheny::prism::ProtoEdge_Direction get_direction() const;
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_EDGE_H
