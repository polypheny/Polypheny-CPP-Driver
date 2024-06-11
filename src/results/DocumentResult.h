//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_DOCUMENTRESULT_H
#define POLYPHENY_CPP_DRIVER_DOCUMENTRESULT_H

#include "results/Result.h"
#include "document_frame.pb.h"
#include "types/TypedValue.h"

namespace Connection {
    class Cursor;
}

namespace Results {

    class DocumentResult : public Result {
    public:
        DocumentResult(const org::polypheny::prism::DocumentFrame &document_frame, bool is_last, Connection::Cursor *cursor);

        class DocumentIterator {
        public:
            DocumentIterator(std::shared_ptr<DocumentResult> result, size_t index);

            bool operator==(const DocumentIterator &other) const;
            bool operator!=(const DocumentIterator &other) const;
            std::unordered_map<std::string, Types::TypedValue>& operator*();
            DocumentIterator &operator++();

        private:
            std::shared_ptr<DocumentResult> result;
            size_t index;
        };

        DocumentIterator begin();
        DocumentIterator end();

    private:
        void add_documents(const org::polypheny::prism::DocumentFrame &document_frame);
        void fetch_more();

        std::vector<std::unordered_map<std::string, Types::TypedValue>> documents;
        bool is_fully_fetched;
        Connection::Cursor *cursor;
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_DOCUMENTRESULT_H
