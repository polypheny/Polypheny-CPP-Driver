#include "CallbackQueue.h"

namespace Communication {

    void CallbackQueue::await_completion() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        is_completed.wait(lock, [this] { return b_is_completed; });
        if (propagated_exception) {
            std::rethrow_exception(propagated_exception);
        }
    }

    org::polypheny::prism::Response CallbackQueue::take_next() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        has_next.wait(lock, [this] { return !message_queue.empty() || propagated_exception != nullptr; });
        throw_received_exception();
        org::polypheny::prism::Response message = message_queue.front();
        message_queue.pop();
        return message;
    }

    void CallbackQueue::on_next(const org::polypheny::prism::Response &message) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            message_queue.push(message);
        }
        has_next.notify_one();
    }

    void CallbackQueue::on_error(std::exception_ptr exception) {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            this->propagated_exception = exception;
            b_is_completed = true;
        }
        has_next.notify_all();
        is_completed.notify_all();
    }

    void CallbackQueue::on_completed() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            b_is_completed = true;
        }
        is_completed.notify_all();
    }

    void CallbackQueue::throw_received_exception() {
        if (!propagated_exception) {
            return;
        }
        std::rethrow_exception(propagated_exception);
    }

} // namespace Communication
