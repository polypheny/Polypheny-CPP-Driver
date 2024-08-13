//
// Created by tobi on 12.06.24.
//

#include "Node.h"

namespace Results {
    Node::Node(const org::polypheny::prism::ProtoNode& proto_node, std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client)
            : GraphElement(proto_node.id(), proto_node.name(), GraphElementType::NODE, prism_interface_client) {
        labels.assign(proto_node.labels().begin(), proto_node.labels().end());
        for (const auto &property : proto_node.properties()) {
            emplace(property.first, Types::TypedValue(property.second, prism_interface_client));
        }
    }
} // Results