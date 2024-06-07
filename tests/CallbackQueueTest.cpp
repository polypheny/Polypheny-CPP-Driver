#include <catch2/catch_all.hpp>
#include "src/connection/CallbackQueue.h"
#include <thread>
#include <exception>

TEST_CASE("CallbackQueue await_completion without exception") {
    Communication::CallbackQueue queue;
    std::thread completion_thread([&queue]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue.on_completed();
    });

    REQUIRE_NOTHROW(queue.await_completion());
    completion_thread.join();
}

TEST_CASE("CallbackQueue await_completion with exception") {
    Communication::CallbackQueue queue;
    std::thread error_thread([&queue]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue.on_error(std::make_exception_ptr(std::runtime_error("Test error")));
    });

    REQUIRE_THROWS_AS(queue.await_completion(), std::runtime_error);
    error_thread.join();
}

TEST_CASE("CallbackQueue take_next without exception") {
    Communication::CallbackQueue queue;
    org::polypheny::prism::Response response;
    response.mutable_commit_response();

    std::thread next_thread([&queue, &response]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue.on_next(response);
    });

    auto message = queue.take_next();
    REQUIRE(message.has_commit_response() == true);
    next_thread.join();
}

TEST_CASE("CallbackQueue take_next with exception") {
    Communication::CallbackQueue queue;
    std::thread error_thread([&queue]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        queue.on_error(std::make_exception_ptr(std::runtime_error("Test error")));
    });

    REQUIRE_THROWS_AS(queue.take_next(), std::runtime_error);
    error_thread.join();
}

TEST_CASE("CallbackQueue on_next and take_next") {
    Communication::CallbackQueue queue;
    org::polypheny::prism::Response response1;
    response1.mutable_close_statement_response();
    org::polypheny::prism::Response response2;
    response2.mutable_rollback_response();

    queue.on_next(response1);
    queue.on_next(response2);

    auto message1 = queue.take_next();
    REQUIRE(message1.has_close_statement_response());

    auto message2 = queue.take_next();
    REQUIRE(message2.has_rollback_response());
}

TEST_CASE("CallbackQueue on_completed") {
    Communication::CallbackQueue queue;
    queue.on_completed();
    REQUIRE_NOTHROW(queue.await_completion());
}

TEST_CASE("CallbackQueue on_error") {
    Communication::CallbackQueue queue;
    queue.on_error(std::make_exception_ptr(std::runtime_error("Test error")));
    REQUIRE_THROWS_AS(queue.await_completion(), std::runtime_error);
}
