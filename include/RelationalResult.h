#ifndef POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H
#define POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H

#include "Result.h"
#include "relational_frame.pb.h"
#include "RelationalMetadata.h"
#include "Row.h"
#include <memory>
#include <list>
#include <iterator>

namespace Connection {
    class Cursor;
}

namespace Results {

    class RelationalResult : public Result {
    public:
        explicit RelationalResult(const org::polypheny::prism::RelationalFrame &relational_frame,
                                  bool is_last, Connection::Cursor *cursor);

        [[nodiscard]] std::shared_ptr<RelationalMetadata> get_metadata() const;

        class RowIterator;

        RowIterator begin();

        RowIterator end();

    private:
        std::shared_ptr<RelationalMetadata> metadata;
        Connection::Cursor *cursor;
        std::list<Row> rows;
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

} // Results

#endif //POLYPHENY_CPP_DRIVER_RELATIONALRESULT_H
