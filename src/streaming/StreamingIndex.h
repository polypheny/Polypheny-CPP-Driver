//
// Created by tobi on 10.08.24.
//

#ifndef POLYPHENY_CPP_DRIVER_STREAMINGINDEX_H
#define POLYPHENY_CPP_DRIVER_STREAMINGINDEX_H

#include <unordered_set>
#include <atomic>
#include <memory>

#include "connection/PrismInterfaceClient.h"
#include "streaming/PrismOutputStream.h"

namespace Streaming {

    class StreamingIndex {
    public:
        explicit StreamingIndex(std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client);

        int64_t register_stream(const std::shared_ptr<PrismOutputStream>& stream);

        void update(uint32_t statement_id, bool is_statement_id_set);

    private:
        std::unordered_set<std::shared_ptr<PrismOutputStream>> streams;
        uint32_t statement_id;
        bool is_statement_id_set;
        std::atomic_long stream_id_generator;
        std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client;
    };

} // namespace Streaming

#endif // POLYPHENY_CPP_DRIVER_STREAMINGINDEX_H
