#import <variant>
#import <future>
template<typename T, class Args...>
class Actor {
public:
    Actor(std::shared_ptr<T> ptr) : ptr_(ptr) {}
    // call ptr_ with arguments
    void operator()(Args...
                    args) {
        ptr_->operator()(args...);
    }
private:
    std::shared_ptr<T> ptr_;
};

template<typename T, class Args...>
std::shared_ptr<Actor<T, Args...>>
make_actor(std::shared_ptr<T> ptr) {
    return std::make_shared<Actor<T, Args...>>(ptr);
}

template<typename T, class Args...>
void
call(std::shared_ptr<Actor<T, Args...>> ptr, Args... args) {
    ptr->operator()(args...);
}

#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <chrono>

class Foo {
public:
    void operator()(int i) {
        std::cout << "Foo: " << i << std::endl;
    }
};

int main() {
    std::shared_ptr<Foo> foo = std::make_shared<Foo>();
    std::shared_ptr<Actor<Foo>> actor = make_actor(foo);
    call(actor, 42);
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(std::async(std::launch::async, call, actor, i));
    }
    for (auto& f : futures) {
        f.wait();
    }
}

