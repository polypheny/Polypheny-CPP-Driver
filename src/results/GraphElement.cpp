//
// Created by tobi on 12.06.24.
//

#include "GraphElement.h"

#include <utility>

namespace Results {
    GraphElement::GraphElement(std::string id, std::string name, GraphElementType type)
            : id(std::move(id)), name(std::move(name)), element_type(type) {}

    const std::string &GraphElement::get_id() const {
        return id;
    }

    const std::string &GraphElement::get_name() const {
        return name;
    }

    const std::vector<std::string> &GraphElement::get_labels() const {
        return labels;
    }

    GraphElementType GraphElement::get_element_type() const {
        return element_type;
    }


} // Results