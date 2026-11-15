#include <catch2/catch_all.hpp>
#include <PolyphenyCPPDriver.h>

#include <condition_variable>
#include <functional>
#include <queue>

namespace {

    using org::polypheny::prism::Request;
    using org::polypheny::prism::Response;

    class ScriptedTransport final : public Transport::Transport {
    public:
        using Handler = std::function<void(const Request &, Response &)>;

        ScriptedTransport(int major, int minor, bool compatible, Handler handler)
                : major(major), minor(minor), compatible(compatible), handler(std::move(handler)) {}

        void connect() override {}

        void send_message(const std::string &message) override {
            Request request;
            REQUIRE(request.ParseFromString(message));
            Response response;
            response.set_id(request.id());
            response.set_last(true);
            if (request.has_connection_request()) {
                const auto &connection = request.connection_request();
#if POLYPHENY_PRISM_STREAMING
                REQUIRE(connection.major_api_version() == 5);
                REQUIRE(connection.minor_api_version() == 1);
                REQUIRE(connection.properties().at("namespace") == "inventory");
                REQUIRE(connection.properties().at("autocommit") == "false");
                REQUIRE(connection.features_size() == 2);
#else
                REQUIRE(connection.major_api_version() == 2);
                REQUIRE(connection.minor_api_version() == 2);
                REQUIRE(connection.connection_properties().namespace_name() == "inventory");
                REQUIRE(connection.connection_properties().has_is_auto_commit());
                REQUIRE_FALSE(connection.connection_properties().is_auto_commit());
#endif
                auto *reply = response.mutable_connection_response();
                reply->set_major_api_version(major);
                reply->set_minor_api_version(minor);
                reply->set_is_compatible(compatible);
            } else if (request.has_disconnect_request()) {
                response.mutable_disconnect_response();
            } else {
                handler(request, response);
            }
            {
                std::lock_guard<std::mutex> lock(mutex);
                responses.push(response.SerializeAsString());
            }
            ready.notify_one();
        }

        std::string receive_message() override {
            std::unique_lock<std::mutex> lock(mutex);
            ready.wait(lock, [&] { return closed || !responses.empty(); });
            if (responses.empty()) {
                throw std::runtime_error("Test transport is closed");
            }
            auto response = std::move(responses.front());
            responses.pop();
            return response;
        }

        void shutdown() noexcept override {
            {
                std::lock_guard<std::mutex> lock(mutex);
                closed = true;
            }
            ready.notify_one();
        }

    private:
        int major;
        int minor;
        bool compatible;
        Handler handler;
        std::mutex mutex;
        std::condition_variable ready;
        std::queue<std::string> responses;
        bool closed = false;
    };

    Connection::ConnectionProperties properties() {
        Connection::ConnectionProperties settings;
        settings.set_default_namespace("inventory");
        settings.set_is_auto_commit(false);
        return settings;
    }

    constexpr int server_major = POLYPHENY_PRISM_STREAMING ? 5 : 2;
    constexpr int server_minor = 1;

}

TEST_CASE("An incompatible Prism server is reported as an exception") {
    auto settings = properties();
    auto transport = std::make_unique<ScriptedTransport>(1, 9, false, nullptr);
    REQUIRE_THROWS_WITH(Connection::Connection(settings, std::move(transport)),
                        Catch::Matchers::ContainsSubstring("Incompatible API version"));
}

TEST_CASE("Entity search stays available on a compatible older minor version") {
    auto settings = properties();
    auto transport = std::make_unique<ScriptedTransport>(server_major, server_minor, true,
        [](const Request &request, Response &response) {
            REQUIRE(request.has_entities_request());
            REQUIRE(request.entities_request().namespace_name() == "public");
            REQUIRE_FALSE(request.entities_request().has_entity_pattern());
            response.mutable_entities_response()->add_entities()->mutable_table()->set_table_name("items");
        });
    Connection::Connection connection(settings, std::move(transport));
    REQUIRE(connection.get_server_api_version() == std::make_pair(server_major, server_minor));
    const auto response = connection.search_entities("public");
    REQUIRE(response.entities_size() == 1);
    REQUIRE(response.entities(0).table().table_name() == "items");
    REQUIRE_THROWS_WITH(connection.get_document_collections("inventory"),
                        Catch::Matchers::ContainsSubstring("Document collection metadata"));
}

#if !POLYPHENY_PRISM_STREAMING
TEST_CASE("Document collection metadata preserves every field and filters other entities") {
    auto settings = properties();
    auto transport = std::make_unique<ScriptedTransport>(2, 2, true, [](const Request &request, Response &response) {
        REQUIRE(request.has_entities_request());
        REQUIRE(request.entities_request().namespace_name() == "inventory");
        REQUIRE(request.entities_request().has_entity_pattern());
        REQUIRE(request.entities_request().entity_pattern() == "item%");

        auto *entities = response.mutable_entities_response();
        entities->add_entities()->mutable_table()->set_table_name("ignored");
        auto *document = entities->add_entities()->mutable_document();
        document->set_namespace_name("inventory");
        document->set_collection_name("items");
        auto *field = document->add_fields();
        field->set_field_path("nested.count");
        field->set_type_name("int");
        field->set_is_optional(true);
        document->set_sample_document_json(R"({"nested":{"count":2}})");
        auto *index = document->add_document_indexes();
        index->set_field_path("nested.count");
        index->set_index_type("btree");
        index->set_unique(true);

        auto *empty = entities->add_entities()->mutable_document();
        empty->set_namespace_name("inventory");
        empty->set_collection_name("items_empty");
    });
    Connection::Connection connection(settings, std::move(transport));
    const auto collections = connection.get_document_collections("inventory", "item%");
    REQUIRE(collections.size() == 2);
    CHECK(collections[0].namespace_name == "inventory");
    CHECK(collections[0].collection_name == "items");
    REQUIRE(collections[0].fields.size() == 1);
    CHECK(collections[0].fields[0].field_path == "nested.count");
    CHECK(collections[0].fields[0].type_name == "int");
    CHECK(collections[0].fields[0].is_optional);
    CHECK(collections[0].sample_document_json == R"({"nested":{"count":2}})");
    REQUIRE(collections[0].document_indexes.size() == 1);
    CHECK(collections[0].document_indexes[0].field_path == "nested.count");
    CHECK(collections[0].document_indexes[0].index_type == "btree");
    CHECK(collections[0].document_indexes[0].unique);
    CHECK(collections[1].collection_name == "items_empty");
    CHECK_FALSE(collections[1].sample_document_json.has_value());
    CHECK(collections[1].fields.empty());
    CHECK(collections[1].document_indexes.empty());
}
#endif

TEST_CASE("Inline FILE values own their contents and can be sent repeatedly") {
    const std::string bytes("A\0\xff", 3);
    org::polypheny::prism::ProtoValue encoded;
    encoded.mutable_file()->set_binary(bytes);
    Types::TypedValue value(encoded, nullptr);
    auto file = value.as_file();
    CHECK(Utils::ProtoUtils::vector_to_string(Utils::ProtoUtils::collect_file(file)) == bytes);
    file = value.as_file();
    CHECK(Utils::ProtoUtils::vector_to_string(Utils::ProtoUtils::collect_file(file)) == bytes);
    const std::unique_ptr<org::polypheny::prism::ProtoValue> first(value.serialize(nullptr));
    const std::unique_ptr<org::polypheny::prism::ProtoValue> second(value.serialize(nullptr));
    CHECK(first->file().binary() == bytes);
    CHECK(second->file().binary() == bytes);
}
