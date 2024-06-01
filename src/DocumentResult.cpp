//
// Created by tobi on 01.06.24.
//

#include "DocumentResult.h"

namespace Results {
    DocumentResult::DocumentResult(const org::polypheny::prism::DocumentFrame& document_frame) : Result(ResultType::DOCUMENT) {}
} // Results