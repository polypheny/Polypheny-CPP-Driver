//
// Created by tobi on 10.08.24.
//

#include "PrismOutputStream.h"

namespace Streaming {

    PrismOutputStream::PrismOutputStream()
            : is_stream_id_set(false), is_statement_id_set(false), statement_id(0), stream_id(0) {}

    PrismOutputStream::~PrismOutputStream() {
        if (streaming_thread.joinable()) {
            stop_flag.store(true);
            streaming_thread.join();
        }
    }

    void PrismOutputStream::set_statement_id(uint32_t statement_id) {
        if (this->is_statement_id_set.load()) {
            throw std::runtime_error("Statement id can only be set once.");
        }
        this->statement_id.store(statement_id);
        this->is_statement_id_set.store(true);
    }

    void PrismOutputStream::set_stream_id(uint64_t stream_id) {
        if (this->is_stream_id_set.load()) {
            throw std::runtime_error("Stream id can only be set once.");
        }
        this->stream_id.store(stream_id);
        this->is_stream_id_set.store(true);
    }
} // namespace Streaming
