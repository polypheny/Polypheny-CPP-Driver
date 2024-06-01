#include "CallbackQueue.h"

namespace Communication {

    template<typename T>
    CallbackQueue<T>::CallbackQueue(
            std::function<const T &(const org::polypheny::prism::Response &)> &response_extractor)
            : b_is_completed(false), extract_response(
            static_cast<const std::function<org::polypheny::prism::StatementResponse &(
                    const org::polypheny::prism::Response &)>>(static_cast<const std::function<org::polypheny::prism::StatementResponse &(
                    const org::polypheny::prism::Response &)>>(response_extractor))) {}

    template<typename T>
    void CallbackQueue<T>::await_completion() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        is_completed.wait(lock, [this] { return b_is_completed; });
    }

    template<typename T>
    T CallbackQueue<T>::take_next() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        has_next.wait(lock, [this] { return !message_queue.empty() || propagated_exception != nullptr; });
        throw_received_exception();
        T message = message_queue.front();
        message_queue.pop();
        return message;
    }

    template<typename T>
    void CallbackQueue<T>::on_next(const org::polypheny::prism::Response &message) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        message_queue.push(extract_response(message));
        has_next.notify_one();
    }

    template<typename T>
    void CallbackQueue<T>::on_error(std::exception_ptr exception) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        this->propagated_exception = exception;
        has_next.notify_one();
    }

    template<typename T>
    void CallbackQueue<T>::on_completed() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        b_is_completed = true;
        is_completed.notify_all();
    }

    template<typename T>
    void CallbackQueue<T>::throw_received_exception() {
        if (propagated_exception) {
            std::rethrow_exception(propagated_exception);
        }
    }

} // namespace Communication
