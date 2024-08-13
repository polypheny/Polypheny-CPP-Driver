//
// Created by tobi on 12.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_GRAPHELEMENT_H
#define POLYPHENY_CPP_DRIVER_GRAPHELEMENT_H

#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <typeinfo>

#include "types/TypedValue.h"
#include "types/Interval.h"
#include "results/GraphElementType.h"

namespace Results {

    /**
     * @brief Base class for all graph elements (nodes, edges, and paths).
     *
     * This class provides common properties and methods for all graph elements,
     * including an ID, name, and labels.
     */
    class GraphElement : public std::unordered_map<std::string, Types::TypedValue> {
    protected:
        std::string id;
        std::string name;
        std::vector<std::string> labels;
        GraphElementType element_type;
        std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client;


    public:
        /**
         * @brief Template method to unwrap the graph element to a specific derived type (Node, Edge, Path).
         *
         * @tparam T The type to unwrap to.
         * @return The unwrapped type.
         * @throws std::runtime_error if the cast is not valid.
         */
        template<typename T>
        T unwrap() {
            if (auto ptr = dynamic_cast<T *>(this)) {
                return *ptr;
            }
            throw std::runtime_error("Not a wrapper for " + std::string(typeid(T).name()));
        }

        GraphElement(std::string id, std::string name, GraphElementType type, std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client);

        /**
         * @brief Gets the unique identifier of the graph element.
         *
         * @return A reference to the ID string.
         */
        const std::string& get_id() const;

        /**
         * @brief Gets the name of the graph element.
         *
         * @return A reference to the name string.
         */
        const std::string& get_name() const;

        /**
         * @brief Gets the labels associated with the graph element.
         *
         * @return A reference to the vector of labels.
         */
        const std::vector<std::string>& get_labels() const;

        /**
         * @brief Returns wether the graph element is a node or an edge.
         *
         * @return A value form the graph element type enum.
         */
        GraphElementType get_element_type() const;
    };
}  // Results

#endif //POLYPHENY_CPP_DRIVER_GRAPHELEMENT_H
