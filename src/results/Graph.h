#ifndef POLYPHENY_CPP_DRIVER_GRAPH_H
#define POLYPHENY_CPP_DRIVER_GRAPH_H

#include "PrismConfiguration.h"
#include "Node.h"
#include "Edge.h"

#if !POLYPHENY_PRISM_STREAMING
namespace Results {

    /** A graph value, including isolated nodes. Element ordering is unspecified. */
    class Graph {
    public:
        Graph(const org::polypheny::prism::ProtoGraph &graph,
              std::shared_ptr<Communication::PrismInterfaceClient> client = nullptr);

        [[nodiscard]] const std::vector<Node> &get_nodes() const noexcept { return nodes; }
        [[nodiscard]] const std::vector<Edge> &get_edges() const noexcept { return edges; }

    private:
        std::vector<Node> nodes;
        std::vector<Edge> edges;
    };

}
#endif

#endif
