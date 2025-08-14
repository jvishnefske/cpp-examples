#include <variant>
#include <future>
#include <memory>
#include <iostream>
#include <functional>

// Simple Actor pattern implementation
template<typename T>
class Actor {
private:
    std::shared_ptr<T> data_;
    
public:
    explicit Actor(std::shared_ptr<T> data) : data_(std::move(data)) {}
    
    template<typename F>
    auto execute(F func) -> decltype(func(*data_)) {
        return func(*data_);
    }
    
    T& get() { return *data_; }
    const T& get() const { return *data_; }
};

template<typename T>
std::shared_ptr<Actor<T>> make_actor(std::shared_ptr<T> ptr) {
    return std::make_shared<Actor<T>>(ptr);
}

template<typename T>
std::shared_ptr<Actor<T>> make_actor(T&& value) {
    return std::make_shared<Actor<T>>(std::make_shared<T>(std::forward<T>(value)));
}

// Example usage class
class Foo {
public:
    int value = 42;
    
    void print() const {
        std::cout << "Foo value: " << value << std::endl;
    }
    
    void increment() {
        ++value;
    }
    
    void set_value(int new_value) {
        value = new_value;
    }
};

void test_actor_pattern() {
    std::cout << "=== Testing Actor Pattern ===\n";
    
    // Create actor with shared data
    auto foo_ptr = std::make_shared<Foo>();
    auto actor = make_actor(foo_ptr);
    
    // Execute operations on the actor
    actor->execute([](Foo& f) { f.print(); });
    actor->execute([](Foo& f) { f.increment(); });
    actor->execute([](Foo& f) { f.print(); });
    actor->execute([](Foo& f) { f.set_value(100); });
    actor->execute([](Foo& f) { f.print(); });
    
    // Access the underlying data
    std::cout << "Direct access: " << actor->get().value << std::endl;
}

void async_actor_operation(std::shared_ptr<Actor<Foo>> actor, int new_value) {
    std::cout << "Async operation setting value to: " << new_value << std::endl;
    actor->execute([new_value](Foo& f) { 
        f.set_value(new_value);
        f.print();
    });
}

void test_async_actors() {
    std::cout << "\n=== Testing Async Actors ===\n";
    
    auto foo_ptr = std::make_shared<Foo>();
    auto actor = make_actor(foo_ptr);
    
    // Launch async operations
    auto future1 = std::async(std::launch::async, async_actor_operation, actor, 200);
    auto future2 = std::async(std::launch::async, async_actor_operation, actor, 300);
    
    // Wait for completion
    future1.wait();
    future2.wait();
    
    std::cout << "Final value: " << actor->get().value << std::endl;
}

int main() {
    test_actor_pattern();
    test_async_actors();
    return 0;
}