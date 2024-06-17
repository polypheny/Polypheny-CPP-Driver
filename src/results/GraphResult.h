//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_GRAPHRESULT_H
#define POLYPHENY_CPP_DRIVER_GRAPHRESULT_H

#include "results/Result.h"
#include "results/GraphElement.h"
#include "graph_frame.pb.h"

namespace Connection {
    class Cursor;
}

namespace Results {

    class GraphResult : public Result {
    public:
        GraphResult(const org::polypheny::prism::GraphFrame &frame, bool is_last, Connection::Cursor *cursor);

        ~GraphResult();

        class GraphElementIterator {
        public:
            GraphElementIterator(std::shared_ptr<GraphResult> result, size_t index);

            bool operator==(const GraphElementIterator &other) const;

            bool operator!=(const GraphElementIterator &other) const;

            std::shared_ptr<GraphElement> operator*();

            GraphElementIterator &operator++();

        private:
            std::shared_ptr<GraphResult> result;
            size_t index;
        };

        GraphElementIterator begin();

        GraphElementIterator end();

    private:
        void add_graph_elements(const org::polypheny::prism::GraphFrame &graph_frame);

        void fetch_more();

        std::vector<std::shared_ptr<GraphElement>> elements;
        bool is_fully_fetched;
        Connection::Cursor *cursor;
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_GRAPHRESULT_H
