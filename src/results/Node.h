//
// Created by tobi on 12.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_NODE_H
#define POLYPHENY_CPP_DRIVER_NODE_H

#include "results/GraphElement.h"

#include "org/polypheny/prism/graph_frame.pb.h"

namespace Results {

    /**
     * @brief Represents a node in a graph query result.
     *
     * This class provides information about a node, including its ID, name, and labels,
     * as well as any additional properties stored in the underlying map.
     *
     * This class does not yet offer any additional methods above the ones provided by the graph element base class.
     */
    class Node : public GraphElement {
    public:
        explicit Node(const org::polypheny::prism::ProtoNode& proto_node, std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client);

    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_NODE_H
