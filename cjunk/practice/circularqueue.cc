#include <array>
#include <atomic>
#include <semaphore>

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
    std::counting_semaphore<N> m_sem_empty, m_sem_full;
public:
    CircularQueue() : m_head(0), m_tail(0), m_sem_empty(0), m_sem_full(N) {
        static_assert(ATOMIC_INT_LOCK_FREE, "our atomic is not lock-free");
    }
    /**
     * @brief Enqueue an element, or return false if the queue is full.
     * @param item
     */
    bool try_push(T const& item) {
        if( m_sem_full.try_acquire()){
            m_data[m_head++ % N] = item;
            m_sem_empty.release();
            return true;
        }
        return false;
    }
    /**
     * @brief enqueue an element, or block until the queue is not full.
     * @param item
     */
     void push(T const& item) {
        m_sem_full.acquire();
        m_data[m_head++ % N] = item;
        m_sem_empty.release();
    }

    T pop() {
        m_sem_empty.acquire();
        T item = m_data[m_tail++ % N];
        m_sem_full.release();
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
        m_sem_full.post();
        return item;
    }
};
void test_queue(){
    CircularQueue<int, 10> myQ{};
//    myQ.push(1);
};