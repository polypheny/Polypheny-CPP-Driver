#include <sstream>
#include "PolyphenyCPPDriver.h"
#include "transport/UnixTransport.h"

void print_separator(const std::vector<int>& column_widths) {
    for (int width : column_widths) {
        std::cout << "+-" << std::string(width, '-') << "-";
    }
    std::cout << "+" << std::endl;
}

void print_row(const std::vector<std::string>& row, const std::vector<int>& column_widths) {
    for (size_t i = 0; i < row.size(); ++i) {
        std::cout << "| " << std::setw(column_widths[i]) << std::left << row[i] << " ";
    }
    std::cout << "|" << std::endl;
}

void print_document(const std::unordered_map<std::string, Types::TypedValue> &document) {
    for (const auto &entry : document) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
}

void print_help() {
    std::cout << "Available commands:" << std::endl;
    std::cout << "  help       - Show this help message" << std::endl;
    std::cout << "  exit       - Exit this application" << std::endl;
    std::cout << "  lng <name> - Change the query language to <name>" << std::endl;
    std::cout << "  ns <name>  - Change the namespace to <name>" << std::endl;
    std::cout << "  <query>    - Execute the specified query in the current language and namespace" << std::endl;
}

int main() {
    Connection::ConnectionProperties properties = Connection::ConnectionProperties();
#ifdef _WIN32
    std::unique_ptr<Transport::Transport> transport = std::make_unique<Transport::PlainTCPConnection>("localhost");
# else
    std::unique_ptr<Transport::Transport> transport = std::make_unique<Transport::UnixTransport>("/home/tobi/.polypheny/polypheny-prism.sock");
#endif
    Connection::Connection database_connection(properties, std::move(transport));

    Connection::Cursor cursor(database_connection);

    std::string query;
    std::string currentLanguage = "sql";
    std::string currentNamespace = "public";

    std::cout << "PolyphenyDB C++ Client: Type 'help' to get an overview over the commands available." << std::endl;
    std::cout << "Connected as user '" << properties.get_username() << "'." << std::endl;

    while (true) {
        std::cout << currentLanguage << "@" << currentNamespace << "> ";
        if (!std::getline(std::cin, query)) {
            break;
        }

        if (query == "exit") {
            break;
        }

        if (query == "help") {
            print_help();
            continue;
        }

        if (query.substr(0, 4) == "lng ") {
            currentLanguage = query.substr(4);
            continue;
        }

        if (query.substr(0, 3) == "ns ") {
            currentNamespace = query.substr(3);
            continue;
        }

        try {
            std::unique_ptr<Results::Result> result = cursor.execute(currentLanguage, query, currentNamespace);

            switch (result->get_result_type()) {
                case Results::ResultType::SCALAR: {
                    auto *scalar_result = result->unwrap<Results::ScalarResult>();
                    std::cout << "Update count " << scalar_result->get_scalar() << "." << std::endl;
                    break;
                }
                case Results::ResultType::RELATIONAL: {
                    auto *relational_result = result->unwrap<Results::RelationalResult>();
                    auto metadata = relational_result->get_metadata();
                    uint32_t columns_count = metadata->get_column_count();

                    std::vector<std::string> column_labels(columns_count);
                    std::vector<int> column_widths(columns_count);
                    for (uint32_t i = 0; i < columns_count; ++i) {
                        column_labels[i] = metadata->get_column_meta(i).get_column_label();
                        column_widths[i] = column_labels[i].size();
                    }

                    std::vector<std::vector<std::string>> rows;
                    for (const auto& row : *relational_result) {
                        std::vector<std::string> formatted_row;
                        int col_index = 0;
                        for (const auto& value : row) {
                            std::stringstream ss;
                            ss << value;
                            std::string value_str = ss.str();
                            formatted_row.push_back(value_str);
                            column_widths[col_index] = std::max(column_widths[col_index], (int)value_str.size());
                            col_index++;
                        }
                        rows.push_back(formatted_row);
                    }

                    print_separator(column_widths);
                    print_row(column_labels, column_widths);
                    print_separator(column_widths);
                    for (const auto& row : rows) {
                        print_row(row, column_widths);
                    }
                    print_separator(column_widths);
                    break;
                }
                case Results::ResultType::DOCUMENT: {
                    auto *document_result = result->unwrap<Results::DocumentResult>();
                    std::cout << "---------------------------" << std::endl;
                    for (const auto &document : *document_result) {
                        print_document(document);
                        std::cout << "---------------------------" << std::endl;
                    }
                    break;
                }
                case Results::ResultType::GRAPH: {
                    auto *graph_result = result->unwrap<Results::GraphResult>();
                    std::cout << "---------------------------" << std::endl;
                    for (const auto &graph_element : *graph_result) {
                        for (const auto &property : *graph_element) {
                            std::cout << property.first << ": " << property.second << std::endl;
                        }
                        std::cout << "---------------------------" << std::endl;
                    }
                    break;
                }
                default:
                    std::cerr << "Unknown result type." << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}