# Polypheny C++ Driver

Connect to Polypheny from C++17 over the Prism Query Interface. The default build uses the released Prism 2.2 definitions. It connects to servers that advertise compatibility with Prism 2.x. Document collection metadata requires server support introduced in Prism 2.2. [Prism API 2.2](https://github.com/polypheny/Polypheny-Prism-API/releases/tag/2.2)

## Build and test

Install a C++17 compiler, Meson, Ninja, pkg-config, OpenSSL, GMP and the Protobuf compiler and C++ library from the same installation. The tests use Catch2 3. Meson can fetch the pinned Catch2 version if it cannot find one locally.

```sh
sudo apt-get install g++ meson ninja-build pkg-config protobuf-compiler libprotobuf-dev libssl-dev libgmp-dev catch2
meson setup build
meson compile -C build
meson test -C build --print-errorlogs
```

The build verifies the checksum of the Prism Protobuf archive before generating the C++ bindings. The default test suite runs without a Polypheny instance.

For an application built with Meson, add this repository as a subproject and link the `polypheny_cpp_driver_dep` dependency. Include `PolyphenyCPPDriver.h` in your application. The examples cover SQL, MQL and graph queries. [Query examples](examples)

## Read document collection metadata

This example uses plain TCP on the default Prism port, 20590. Use credentials and a document namespace from your server.

```cpp
#include <iostream>
#include "PolyphenyCPPDriver.h"

int main() {
    Connection::Connection connection("127.0.0.1", "pa", "");

    for (const auto &collection : connection.get_document_collections("inventory")) {
        std::cout << collection.namespace_name << "." << collection.collection_name << '\n';
        for (const auto &field : collection.fields) {
            std::cout << field.field_path << " (" << field.type_name << ")"
                      << (field.is_optional ? " optional" : "") << '\n';
        }
        if (collection.sample_document_json) {
            std::cout << *collection.sample_document_json << '\n';
        }
        for (const auto &index : collection.document_indexes) {
            std::cout << index.field_path << " (" << index.index_type << ")"
                      << (index.unique ? " unique" : "") << '\n';
        }
    }
}
```

Pass an entity pattern as the second argument to narrow the search. Field paths use dot notation, with literal dots and backslashes escaped. A sample is optional, and the index list depends on the collection's storage adapter. Use `search_entities` to access the full Prism response, including relational table metadata.

## Experimental streaming build

Applications that use the experimental Prism 5.1 streaming protocol can build against the pinned streaming schema. This build requires a compatible experimental server.

```sh
meson setup build-streaming -Dprism_streaming=true
meson compile -C build-streaming
meson test -C build-streaming --print-errorlogs
```

To run the live streaming tests, start the experimental server on localhost port 20590 and run `./build-streaming/runTests '[.streaming]'`.
