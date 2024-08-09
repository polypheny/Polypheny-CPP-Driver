//
// Created by tobi on 12.06.24.
//

#include "Node.h"

namespace Results {
    Node::Node(org::polypheny::prism::ProtoNode proto_edge)
            : GraphElement(proto_edge.id(), proto_edge.name(), GraphElementType::NODE) {
        labels.assign(proto_edge.labels().begin(), proto_edge.labels().end());
        for (const auto &property: proto_edge.properties()) {
            emplace(property.key().string().string(), property.value());
        }
    }
} // Results