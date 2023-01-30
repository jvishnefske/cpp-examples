#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <future>
#include <thread>
#include <iostream>
#include <atomic>
#include <queue>
#include <boost/asio.hpp>

class StandardInput
{
    public:
    ~StandardInput(){
        m_running = false;
    }

    bool hasChar();
    char popChar();
    StandardInput();
    void stop(){m_running = false;}
    private:
    std::queue<char> m_queue;
    std::atomic<bool> m_running{true};
    // uses std::launch to read std::cin non blocking.
    void inputCharTask();
    private:
    std::future<void> m_future;
    std::mutex m_mutex;
    std::condition_variable m_condition;
};

StandardInput::StandardInput(){
    m_future = std::async(std::launch::async, &StandardInput::inputCharTask, this);
}



char StandardInput::popChar(){
    std::unique_lock<std::mutex> lock(m_mutex);
    char c = m_queue.front();
    m_queue.pop();
    return c;
}
bool StandardInput::hasChar(){
    std::unique_lock<std::mutex> lock(m_mutex);
    return !m_queue.empty();
}
void StandardInput::inputCharTask(){
    std::cout << "starting input task" <<std::endl;
    while(m_running){
        int c;// = std::cin.get();
        fread(&c, 1, 1, stdin);
        if(c != EOF){
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.push(c);
            m_condition.notify_one();
            std::cout << "pushed " << (char)(c) << std::endl;
        }
    }
    std::cout << "stopping input task" <<std::endl;

}

int main(){
    StandardInput si;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    while(si.hasChar()){
        char c = si.popChar();
        std::cout  << c;
    }
    std::cout << "read loop complete" << std::endl;
    return 0;
}

