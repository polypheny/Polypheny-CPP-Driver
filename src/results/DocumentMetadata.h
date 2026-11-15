#ifndef POLYPHENY_CPP_DRIVER_DOCUMENTMETADATA_H
#define POLYPHENY_CPP_DRIVER_DOCUMENTMETADATA_H

#include <optional>
#include <string>
#include <vector>

namespace Results {

    struct DocumentField {
        std::string field_path;
        std::string type_name;
        bool is_optional;
    };

    struct DocumentIndex {
        std::string field_path;
        std::string index_type;
        bool unique;
    };

    struct DocumentMetadata {
        std::string namespace_name;
        std::string collection_name;
        std::vector<DocumentField> fields;
        std::optional<std::string> sample_document_json;
        std::vector<DocumentIndex> document_indexes;
    };

}

#endif
