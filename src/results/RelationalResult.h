#ifndef POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H
#define POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H

#include <memory>
#include <list>
#include <iterator>

#include "Result.h"
#include "org/polypheny/prism/relational_frame.pb.h"
#include "results/RelationalMetadata.h"
#include "results/Row.h"

namespace Connection {
    class Cursor;
}

namespace Results {

    /**
     * @brief Represents the result of a relational query.
     *
     * This class provides access to metadata and rows resulting from a relational query,
     * and supports iteration over the rows.
     */
    class RelationalResult : public Result {
    public:
        /**
         * @brief Gets the metadata for the relational result.
         *
         * The metadata contains information such as column names, database internal data types.
         *
         * @return A shared pointer to the RelationalMetadata object.
         */
        [[nodiscard]] std::shared_ptr<RelationalMetadata> get_metadata() const;

        /**
         * @brief Destructor for the RelationalResult class.
         *
         * It's the programmers responsibility to call this destructor once a result is no longer needed.
         * This internally tells the connected Polypheny instance to free the associated resources.
         */
        ~RelationalResult();

        /**
         * @brief Iterator class for iterating over rows in the relational result.
         *
         * New rows are automatically fetched providing seamless iteration over all results of a query.
         */
        class RowIterator;

        /**
         * @brief Returns an iterator to the beginning of the row collection being the rows of this result.
         *
         * @return An iterator to the first row.
         */
        RowIterator begin();


        /**
         * @brief Returns an iterator to the end of the row collection being the rows of this result.
         *
         * @return An iterator to the past-the-end row.
         */
        RowIterator end();

        explicit RelationalResult(const org::polypheny::prism::RelationalFrame &relational_frame,
                                  bool is_last, Connection::Cursor *cursor);

    private:
        std::shared_ptr<RelationalMetadata> metadata;
        Connection::Cursor *cursor;
        std::list<Results::Row> rows;
        bool is_fully_fetched;

        void add_rows(const org::polypheny::prism::RelationalFrame &relationalFrame);

        void fetch_more();

    };

    class RelationalResult::RowIterator : public std::iterator<std::input_iterator_tag, Row> {
    public:
        RowIterator(std::shared_ptr<RelationalResult> result, size_t index = 0);

        bool operator==(const RowIterator &other) const;

        bool operator!=(const RowIterator &other) const;

        Row &operator*();

        RowIterator &operator++();

    private:
        std::shared_ptr<RelationalResult> result;
        size_t index;
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H
