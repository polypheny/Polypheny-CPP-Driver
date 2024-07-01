//
// Created by tobi on 12.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_EDGE_H
#define POLYPHENY_CPP_DRIVER_EDGE_H

#include "results/GraphElement.h"
#include "graph_frame.pb.h"

namespace Results {

    /**
     * @brief Represents the direction of an edge in the result of a graph query.
     */
    enum class EdgeDirection {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        NONE
    };

    /**
     * @brief Represents an edge in the result of a graph query.
     *
     * This class provides information about an edge, including its source and target node ids
     * and the direction of the edge.
     */
    class Edge : public GraphElement {
    private:
        std::string source;
        std::string target;
        EdgeDirection direction;

        static EdgeDirection get_edge_direction_from_proto(org::polypheny::prism::ProtoEdge_Direction direction);

    public:
        Edge(const org::polypheny::prism::ProtoEdge &proto_edge);

        /**
         * @brief Gets the source node id of the edge.
         *
         * @return A reference to the source node string.
         */
        const std::string &get_source() const;


        /**
         * @brief Gets the target node id of the edge.
         *
         * @return A reference to the target node string.
         */
        const std::string &get_target() const;

        /**
         * @brief Gets the direction of the edge.
         *
         * @return The direction of the edge.
         */
        EdgeDirection get_direction() const;
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_EDGE_H
