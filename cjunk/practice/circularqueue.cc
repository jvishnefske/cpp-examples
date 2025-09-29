#include <array>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <cstddef>

/**
 * @brief Circular queue implementation.
 *
 * @tparam T Type of the elements in the queue.
 * @tparam N Maximum number of elements in the queue.
 */
template <typename T, size_t N>
class CircularQueue {
    std::atomic_uint m_head, m_tail;
    std::array<T, N> m_data;
    std::mutex m_mutex;
    std::condition_variable m_cv_empty, m_cv_full;
    std::atomic<size_t> m_count;
public:
    CircularQueue() : m_head(0), m_tail(0), m_count(0) {
        static_assert(ATOMIC_INT_LOCK_FREE, "our atomic is not lock-free");
    }
    /**
     * @brief Enqueue an element, or return false if the queue is full.
     * @param item
     */
    bool try_push(T const& item) {
        std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);
        if (!lock.owns_lock() || m_count >= N) {
            return false;
        }
        m_data[m_head++ % N] = item;
        m_count++;
        m_cv_empty.notify_one();
        return true;
    }
    /**
     * @brief enqueue an element, or block until the queue is not full.
     * @param item
     */
     void push(T const& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv_full.wait(lock, [this] { return m_count < N; });
        m_data[m_head++ % N] = item;
        m_count++;
        m_cv_empty.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv_empty.wait(lock, [this] { return m_count > 0; });
        T item = m_data[m_tail++ % N];
        m_count--;
        // Modulo is not necessary here, but it is a good idea to avoid
        // integer overflow.
        {
            auto expected = m_tail.load();
            const auto desired = expected % N;
            // successfully stores only if we are not preempted by another thread
            // which clobbers the variable. This eliminates the need for mutual exclusion.
            m_tail.compare_exchange_strong(expected, desired);
        }
        {
            auto expected = m_head.load();
            const auto desired = expected % N;
            m_head.compare_exchange_strong(expected, desired);
        }
        m_cv_full.notify_one();
        return item;
    }
};
void test_queue(){
    CircularQueue<int, 10> myQ{};
    myQ.push(1); // Use the queue to avoid unused variable warning
    (void)myQ.pop(); // Suppress unused return value warning
};

int main() {
    test_queue();
    return 0;
}