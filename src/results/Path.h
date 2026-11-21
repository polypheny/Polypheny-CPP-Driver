#ifndef POLYPHENY_CPP_DRIVER_PATH_H
#define POLYPHENY_CPP_DRIVER_PATH_H

#include "PrismConfiguration.h"
#include "Node.h"
#include "Edge.h"

#if !POLYPHENY_PRISM_STREAMING
namespace Results {

    /** An ordered traversal. Node and edge sequences preserve the server's order. */
    class Path {
    public:
        Path(const org::polypheny::prism::ProtoPath &path,
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
