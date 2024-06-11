//
// Created by tobi on 12.06.24.
//

#include "Edge.h"

namespace Results {
    Edge::Edge(const org::polypheny::prism::ProtoEdge &proto_edge)
            : GraphElement(proto_edge.id(), proto_edge.name()),
              source(proto_edge.source()),
              target(proto_edge.target()),
              direction(proto_edge.direction()) {

        labels.assign(proto_edge.labels().begin(), proto_edge.labels().end());
        for (const auto& property : proto_edge.properties()) {
            emplace(property.key().string().string(), property.value());
        }
    }

    const std::string &Edge::get_source() const {
        return source;
    }

    const std::string &Edge::get_target() const {
        return target;
    }

    org::polypheny::prism::ProtoEdge_Direction Edge::get_direction() const {
        return direction;
    }
} // Results