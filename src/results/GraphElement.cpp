//
// Created by tobi on 12.06.24.
//

#include "GraphElement.h"

#include <utility>

namespace Results {
    GraphElement::GraphElement(int id, std::string name)
            : id(std::move(id)), name(std::move(name)) {}

    const std::string &GraphElement::get_id() const {
        return id;
    }

    const std::string &GraphElement::get_name() const {
        return name;
    }

    const std::vector<std::string> &GraphElement::get_labels() const {
        return labels;
    }


} // Results