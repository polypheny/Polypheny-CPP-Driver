#include "Graph.h"

#if !POLYPHENY_PRISM_STREAMING
namespace Results {

    Graph::Graph(const org::polypheny::prism::ProtoGraph &graph,
                 std::shared_ptr<Communication::PrismInterfaceClient> client) {
        for (const auto &node : graph.nodes()) {
            nodes.emplace_back(node, client);
        }
        for (const auto &edge : graph.edges()) {
            edges.emplace_back(edge, client);
        }
    }

}
#endif
