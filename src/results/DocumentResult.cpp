//
// Created by tobi on 01.06.24.
//

#include "DocumentResult.h"
#include "src/connection/Cursor.h"

namespace Results {
    DocumentResult::DocumentResult(const org::polypheny::prism::DocumentFrame& document_frame, bool is_last, Connection::Cursor *cursor) : Result(ResultType::DOCUMENT) {}
} // Results