#include "Path.h"

#if !POLYPHENY_PRISM_STREAMING
namespace Results {

    Path::Path(const org::polypheny::prism::ProtoPath &path,
               std::shared_ptr<Communication::PrismInterfaceClient> client) {
        if (path.nodes_size() != path.edges_size() + 1) {
            throw std::runtime_error("A graph path must contain one more node than edges");
        }
        for (const auto &node : path.nodes()) {
            nodes.emplace_back(node, client);
        }
        for (const auto &edge : path.edges()) {
            edges.emplace_back(edge, client);
        }
    }

}
#endif
