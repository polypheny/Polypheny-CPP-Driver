//
// Created by tobi on 30.05.24.
//

#include "Document.h"

namespace Types {
    Document::Document() {}
    Document::Document(const ProtoDocument& document) {
        for (const auto& entry : document.entries()) {
            if (entry.key().value_case() == ProtoValue::kStringValue) {
                std::string key = entry.key().string_value();
                TypedValue value(entry.value());
                this->emplace(key, value);
            }
        }
    }
} // Types