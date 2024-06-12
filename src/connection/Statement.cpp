//
// Created by tobi on 12.06.24.
//

#include "Statement.h"

#include <utility>

namespace Connection {
    Statement::Statement(std::string language_name, std::string statement)
    : language_name(std::move(language_name)),
    statement(std::move(statement)),
    has_namespace(false)
    {}

    Statement::Statement(std::string language_name, std::string statement, std::string namespace_name)
            : language_name(std::move(language_name)),
              statement(std::move(statement)),
              namespace_name(namespace_name),
              has_namespace(true)
    {}

    bool Statement::get_has_namespace() {
        return has_namespace;
    }

    std::string Statement::get_language_name() {
        return language_name;
    }

    std::string Statement::get_statement() {
        return statement;
    }

    std::string Statement::get_namespace_name() {
        if (!has_namespace) {
            throw std::runtimeerror("This statment does not specify a namespace.");
        }
        return namespace_name;
    }
} // Connection