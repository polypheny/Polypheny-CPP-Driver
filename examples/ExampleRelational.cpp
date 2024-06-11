#include "PolyphenyCPPDriver.h"

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

int main() {
    // valid connection parameters to connect to a polypheny instance
    std::string host = "localhost";
    std::string username = "pa";
    std::string password = "";

    // open a connection to the polypheny instance
    Connection::Connection database_connection = Connection::Connection(host, username, password);

    // get a cursor operating on the previously opened connection
    Connection::Cursor cursor(database_connection);

    // execute a statement using this cursor
    std::unique_ptr<Results::Result> result = cursor.execute("sql", "SELECT * FROM emp;", "public");

    // the result can now be processed
    if (result->get_result_type() != Results::ResultType::RELATIONAL) {
        throw std::runtime_error("Received result is of unexpected type");
    }

    auto* relational_result = result->unwrap<Results::RelationalResult>();
    auto metadata = relational_result->get_metadata();
    uint32_t columns_count = metadata->get_column_count();

    // Retrieve column labels
    std::vector<std::string> column_labels(columns_count);
    std::vector<int> column_widths(columns_count);
    for (uint32_t i = 0; i < columns_count; ++i) {
        column_labels[i] = metadata->get_column_meta(i).get_column_label();
        column_widths[i] = column_labels[i].size();
    }

    // Retrieve rows
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

    // Print table
    print_separator(column_widths);
    print_row(column_labels, column_widths);
    print_separator(column_widths);
    for (const auto& row : rows) {
        print_row(row, column_widths);
    }
    print_separator(column_widths);

    return 0;
}