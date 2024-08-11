//
// Created by tobi on 10.08.24.
//

#ifndef POLYPHENY_CPP_DRIVER_BINARYPRISMOUTPUTSTREAM_H
#define POLYPHENY_CPP_DRIVER_BINARYPRISMOUTPUTSTREAM_H

#include <vector>
#include <thread>
#include <atomic>
#include "PrismOutputStream.h"
#include "connection/PrismInterfaceClient.h"
#include "types/TypedValue.h"
#include "stream_responses.pb.h"

namespace Streaming {

    class BinaryPrismOutputStream : public PrismOutputStream {
    public:
        explicit BinaryPrismOutputStream(std::vector<uint8_t>& data);

        void build_and_run(uint32_t statement_id, uint64_t stream_id, std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client) override;

        void run() override;

    private:
        std::vector<uint8_t> data;
        std::shared_ptr<Communication::PrismInterfaceClient> client;
    };

} // namespace Streaming

#endif // POLYPHENY_CPP_DRIVER_BINARYPRISMOUTPUTSTREAM_H
