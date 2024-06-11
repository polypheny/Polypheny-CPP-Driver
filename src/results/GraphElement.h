//
// Created by tobi on 12.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_GRAPHELEMENT_H
#define POLYPHENY_CPP_DRIVER_GRAPHELEMENT_H

#include "types/TypedValue.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <typeinfo>

namespace Results {

    class GraphElement : public std::unordered_map<std::string, Types::TypedValue> {
    protected:
        std::string id;
        std::string name;
        std::vector<std::string> labels;

    public:
        template<typename T>
        T unwrap() {
            if (auto ptr = dynamic_cast<T *>(this)) {
                return *ptr;
            }
            throw std::runtime_error("Not a wrapper for " + std::string(typeid(T).name()));
        }

        GraphElement(std::string id, std::string name);

        const std::string& get_id() const;

        const std::string& get_name() const;

        const std::vector<std::string>& get_labels() const;
    };
}  // Results

#endif //POLYPHENY_CPP_DRIVER_GRAPHELEMENT_H
