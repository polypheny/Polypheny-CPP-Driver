#include "PolyphenyCPPDriver.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: ExampleGraph <existing-graph-namespace>\n";
        return 1;
    }

    try {
        Connection::Connection connection("127.0.0.1", "pa", "");
        Connection::Cursor cursor(connection);
        cursor.set_fetch_size(100);
        const std::string graph_namespace = argv[1];

        // Graph-only projections stream nodes and edges. Fetching follows every page.
        auto result = cursor.execute("cypher", "MATCH (*) RETURN *", graph_namespace);
        for (auto element : *result->unwrap<Results::GraphResult>()) {
            std::cout << (element->get_element_type() == GraphElementType::NODE ? "Node " : "Edge ")
                      << element->get_id() << '\n';
            for (const auto &property : *element) {
                std::cout << "  " << property.first << ": " << property.second << '\n';
            }
        }
        result->close();

#if !POLYPHENY_PRISM_STREAMING
        // On Prism 2.3, mixed projections preserve ordered paths as typed row cells.
        auto paths = cursor.execute("cypher", "MATCH p=(a)-[r]->(b) RETURN p, 'route' AS title", graph_namespace);
        for (auto &row : *paths->unwrap<Results::RelationalResult>()) {
            auto path = row["p"].as_path();
            std::cout << row["title"].as_string() << ": " << path.get_nodes().size() << " nodes, "
                      << path.get_edges().size() << " edges\n";
        }
#endif
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
