//
// Created by tobi on 10.08.24.
//

#ifndef POLYPHENY_CPP_DRIVER_PRISMOUTPUTSTREAM_H
#define POLYPHENY_CPP_DRIVER_PRISMOUTPUTSTREAM_H

#include <thread>
#include <atomic>
#include <stdexcept>
#include "connection/PrismInterfaceClient.h"

namespace Streaming {

    class PrismOutputStream {
    public:
        virtual ~PrismOutputStream();

        void set_statement_id(uint32_t statement_id);

        void set_stream_id(uint64_t stream_id);

        virtual void build_and_run(uint32_t statement_id, uint64_t stream_id,
                                   std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client) = 0;

        virtual void run() = 0;

    protected:
        std::atomic<uint32_t> statement_id;
        std::atomic<uint64_t> stream_id;
        std::string name;

        std::atomic<bool> is_statement_id_set;
        std::atomic<bool> is_stream_id_set;

        std::thread streaming_thread;
        std::atomic<bool> stop_flag{false};
    };
} // namespace Streaming

#endif // POLYPHENY_CPP_DRIVER_PRISMOUTPUTSTREAM_H
