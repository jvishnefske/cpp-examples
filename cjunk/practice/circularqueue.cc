#include <array>
#include <mutex>
#include <condition_variable>
using GeneralType = double;
template<class T, int size>
// implement a std::queue container of fixed size.
class CircularContainer{
    std::array<T,size> m_container;
    int head;
    int tail;
    std::mutex m_mutex;
    std::condition_variable m_not_empty;
    std::condition_variable m_not_full;
public:
    using reference = &T;
    void push_back();
    void pop_front();
    reference front();
    reference back();
    bool empty();
    int size();
};

template<class T, int size>
bool CircularContainer<T,size>::empty(){
    return size() == 0;
}
template<class T, int size>
int CircularContainer<T,size>::size(){
    return (tail - head + size) % size;
}
template<class T, int size>
CircularContainer<T,size>::CircularContainer(){
    head = 0;
    tail = 0;
}
template<class T, int size>
CircularContainer<T,size>::~CircularContainer(){
}
template<class T, int size>
void CircularContainer<T,size>::push_back(T& value){
    std::unique_lock<std::mutex> lock(m_mutex);
    while(size() == size){
        m_not_full.wait(lock);
    }
    m_container[tail] = value;
    tail = (tail + 1) % size;
    m_not_empty.notify_one();
}
template<class T, int size>
void CircularContainer<T,size>::pop_front(){
    std::unique_lock<std::mutex> lock(m_mutex);
    while(size() == 0){
        m_not_empty.wait(lock);
    }
    head = (head + 1) % size;
    m_not_full.notify_one();
}
template<class T, int size>
T& CircularContainer<T,size>::front(){
    std::unique_lock<std::mutex> lock(m_mutex);
    while(size() == 0){
        m_not_empty.wait(lock);
    }
    return m_container[head];
}
template<class T, int size>
T& CircularContainer<T,size>::back(){
    std::unique_lock<std::mutex> lock(m_mutex);
    while(size() == 0){
        m_not_empty.wait(lock);
    }
    return m_container[(tail - 1 + size) % size];
}
template<class T, int size>
void CircularContainer<T,size>::print(){
    std::unique_lock<std::mutex> lock(m_mutex);
    while(size() == 0){
        m_not_empty.wait(lock);
    }
    for(int i = 0; i < size; i++){
        std::cout << m_container[(head + i) % size] << " ";
    }
    std::cout << std::endl;
}
template<class T, int size>
void CircularContainer<T,size>::print_reverse(){
    std::unique_lock<std::mutex> lock(m_mutex);
    while(size() == 0){
        m_not_empty.wait(lock);
    }
    for(int i = size - 1; i >= 0; i--){
        std::cout << m_container[(head + i) % size] << " ";
    }
    std::cout << std::endl;
}

void test_queue(){
    CircularContainer<GeneralType,5> queue;
    queue.push_back(1);
    queue.push_back(2);
    queue.push_back(3);
    queue.push_back(4);
    queue.push_back(5);
    queue.print();
    queue.pop_front();
    queue.print();
    queue.pop_front();
    queue.print();
    queue.pop_front();
    queue.print();
    queue.pop_front();
    queue.print();
    queue.pop_front();
    queue.print();
   }
