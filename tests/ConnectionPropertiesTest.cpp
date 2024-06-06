#include <catch2/catch_all.hpp>
#include "ConnectionProperties.h"

TEST_CASE("ConnectionProperties constructor and getters") {
    Connection::ConnectionProperties props("localhost", 5432, "user", "pass", "default_ns", false, true);

    REQUIRE(props.get_host() == "localhost");
    REQUIRE(props.get_port() == 5432);
    REQUIRE(props.get_username() == "user");
    REQUIRE(props.get_password() == "pass");
    REQUIRE(props.get_default_namespace() == "default_ns");
    REQUIRE_FALSE(props.get_is_password_required());
    REQUIRE(props.get_is_auto_commit() == true);
}

TEST_CASE("ConnectionProperties setters") {
    Connection::ConnectionProperties props("localhost", 5432, "user", "pass", "default_ns", false, true);

    props.set_host("127.0.0.1");
    REQUIRE(props.get_host() == "127.0.0.1");

    props.set_port(3306);
    REQUIRE(props.get_port() == 3306);

    props.set_username("new_user");
    REQUIRE(props.get_username() == "new_user");

    props.set_password("new_pass");
    REQUIRE(props.get_password() == "new_pass");

    props.set_default_namespace("new_ns");
    REQUIRE(props.get_default_namespace() == "new_ns");

    props.set_is_password_required(false);
    REQUIRE_FALSE(props.get_is_password_required());

    props.set_is_auto_commit(false);
    REQUIRE(props.get_is_auto_commit() == false);
}

TEST_CASE("ConnectionProperties default constructor values") {
    Connection::ConnectionProperties props;

    REQUIRE(props.get_host() == "localhost");
    REQUIRE(props.get_port() == 20590);
    REQUIRE(props.get_username() == "pa");
    REQUIRE(props.get_password().empty());
    REQUIRE(props.get_default_namespace() == "public");
    REQUIRE(props.get_is_password_required() == true);
    REQUIRE(props.get_is_auto_commit() == true);
}
