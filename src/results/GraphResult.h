//
// Created by tobi on 01.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_GRAPHRESULT_H
#define POLYPHENY_CPP_DRIVER_GRAPHRESULT_H

#include "results/Result.h"
#include "results/GraphElement.h"

#include "org/polypheny/prism/graph_frame.pb.h"

namespace Connection {
    class Cursor;
}

namespace Results {
    class GraphElement;
}

namespace Results {

    /**
     * @brief Represents the result of a graph query.
     *
     * This class provides access to nodes and edges resulting from a graph query,
     * including paths flattened into alternating node and edge elements,
     * and supports iteration over them.
     */
    class GraphResult : public Result {
    public:
        GraphResult(const org::polypheny::prism::GraphFrame &frame, bool is_last, Connection::Cursor *cursor);

        /**
         * @brief Destructor for the GraphResult class.
         *
         * Destruction automatically closes the server-side result. Use close() to release it earlier.
         */
        ~GraphResult() override = default;

        /**
         * @brief Iterator class for nodes and edges, including flattened path traversals.
         *
         * New elements are automatically fetched providing seamless iteration over the result of a query.
         */
        class GraphElementIterator {
        public:
            GraphElementIterator(std::shared_ptr<GraphResult> result, size_t index);

            bool operator==(const GraphElementIterator &other) const;

            bool operator!=(const GraphElementIterator &other) const;

            std::shared_ptr<Results::GraphElement> operator*();

            GraphElementIterator &operator++();

        private:
            std::shared_ptr<GraphResult> result;
            size_t index;
        };

        /**
         * @brief Returns an iterator to the beginning of the graph element collection being the elements of this result.
         *
         * @return An iterator to the first graph element.
         */
        GraphElementIterator begin();

        /**
         * @brief Returns an iterator to the end of the graph element collection being the elements of this result.
         *
         * @return An iterator to the past-the-end graph element.
         */
        GraphElementIterator end();

    private:
        void add_graph_elements(const org::polypheny::prism::GraphFrame &graph_frame);

        void fetch_more();
        void ensure_available(size_t index);

        std::vector<std::shared_ptr<GraphElement>> elements;
        bool is_fully_fetched;
    };

} // Results

#endif //POLYPHENY_CPP_DRIVER_GRAPHRESULT_H
