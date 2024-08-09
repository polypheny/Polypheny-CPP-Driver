//
// Created by tobi on 12.06.24.
//

#include "Edge.h"

namespace Results {
    Edge::Edge(const org::polypheny::prism::ProtoEdge &proto_edge)
            : GraphElement(proto_edge.id(), proto_edge.name(), GraphElementType::EDGE),
              left(proto_edge.left()),
              right(proto_edge.right()),
              direction(get_edge_direction_from_proto(proto_edge.direction())) {

        labels.assign(proto_edge.labels().begin(), proto_edge.labels().end());
        for (const auto &property: proto_edge.properties()) {
            emplace(property.key().string().string(), property.value());
        }
    }

    const std::string &Edge::get_left() const {
        return left;
    }

    const std::string &Edge::get_right() const {
        return right;
    }

    EdgeDirection Edge::get_direction() const {
        return direction;
    }

    EdgeDirection Edge::get_edge_direction_from_proto(org::polypheny::prism::ProtoEdge_Direction direction) {
        if (direction == org::polypheny::prism::ProtoEdge_Direction::ProtoEdge_Direction_UNSPECIFIED) {
            throw std::runtime_error("Edge missing direction specification");
        }
        switch (direction) {
            case org::polypheny::prism::ProtoEdge_Direction::ProtoEdge_Direction_NONE:
                return EdgeDirection::NONE;
            case org::polypheny::prism::ProtoEdge_Direction::ProtoEdge_Direction_LEFT_TO_RIGHT:
                return EdgeDirection::LEFT_TO_RIGHT;
            case org::polypheny::prism::ProtoEdge_Direction::ProtoEdge_Direction_RIGHT_TO_LEFT:
                return EdgeDirection::RIGHT_TO_LEFT;
            default:
                throw std::runtime_error("Should never be thrown.");
        }
    }
} // Results