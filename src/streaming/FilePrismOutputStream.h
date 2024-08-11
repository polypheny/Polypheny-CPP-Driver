//
// Created by tobi on 11.08.24.
//

#ifndef POLYPHENY_CPP_DRIVER_FILEPRISMOUTPUTSTREAM_H
#define POLYPHENY_CPP_DRIVER_FILEPRISMOUTPUTSTREAM_H

#include "PrismOutputStream.h"
#include <atomic>
#include <memory>  // For std::shared_ptr
#include "connection/PrismInterfaceClient.h"

namespace Streaming {

    class FilePrismOutputStream : public PrismOutputStream {
    public:
        explicit FilePrismOutputStream(std::shared_ptr<std::streambuf> fileStream);

        void build_and_run(uint32_t statement_id, uint64_t stream_id,
                           std::shared_ptr<Communication::PrismInterfaceClient> prism_interface_client) override;

        void run() override;

    private:
        std::shared_ptr<std::streambuf> fileStream;
        std::shared_ptr<Communication::PrismInterfaceClient> client;
    };

} // namespace Streaming

#endif //POLYPHENY_CPP_DRIVER_FILEPRISMOUTPUTSTREAM_H
