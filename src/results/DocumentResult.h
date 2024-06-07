//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_DOCUMENTRESULT_H
#define POLYPHENY_CPP_DRIVER_DOCUMENTRESULT_H

#include "Result.h"
#include "document_frame.pb.h"

namespace Connection {
    class Cursor;
}

namespace Results {

    class DocumentResult : public Result {
    public:
        explicit DocumentResult(const org::polypheny::prism::DocumentFrame &document_frame, bool is_lst, Connection::Cursor *cursor);
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_DOCUMENTRESULT_H
