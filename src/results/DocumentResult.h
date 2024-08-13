//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_DOCUMENTRESULT_H
#define POLYPHENY_CPP_DRIVER_DOCUMENTRESULT_H

#include "results/Result.h"
#include "types/TypedValue.h"

#include "org/polypheny/prism/document_frame.pb.h"

namespace Connection {
    class Cursor;
}

namespace Results {

    /**
     * @brief Represents the result of a document query.
     *
     * This class provides access to documents resulting from a document query,
     * and supports iteration over them. Each document is represented as a unordered map of string keyes TypedValue objects.
     * The strings correspond to the field names of the document. The values are provided as TypedValues offering getters to native C++ types.
     * For more details on TypedValues please refer to the corresponding documentation.
     */
    class DocumentResult : public Result {
    public:
        DocumentResult(const org::polypheny::prism::DocumentFrame &document_frame, bool is_last, Connection::Cursor *cursor);

        /**
         * @brief Destructor for the DocumentResult class.
         *
         * It's the programmer's responsibility to call this destructor once a result is no longer needed.
         * This internally tells the connected Polypheny instance to free the associated resources.
         */
        ~DocumentResult();

        /**
         * @brief Iterator class for iterating over documents in the document result.
         *
         * New documents are automatically fetched providing seamless iteration over all results of a query.
         */
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

        /**
         * @brief Returns an iterator to the beginning of the document collection being the documents of this result.
         *
         * @return An iterator to the first document.
         */
        DocumentIterator begin();

        /**
         * @brief Returns an iterator to the end of the document collection being the documents of this result.
         *
         * @return An iterator to the past-the-end document.
         */
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
