//
// Created by tobi on 12.06.24.
//

#ifndef POLYPHENY_CPP_DRIVER_STATEMENT_H
#define POLYPHENY_CPP_DRIVER_STATEMENT_H

#include <string>

namespace Connection {

    class Statement {
    private:
        std::string language_name;
        std::string statement;
        std::string namespace_name;
        bool has_namespace;
    public:
        Statement(std::string language_name, std::string statement);
        Statement(std::string language_name, std::string statement, std::string  namespace_name);
        bool get_has_namespace();
        std::string get_language_name();
        std::string get_statement();
        std::string get_namespace_name();
    };

} // Connection

#endif //POLYPHENY_CPP_DRIVER_STATEMENT_H
