#include <Poco/Thread.h>
#include <Poco/LogStream.h>
#include <Poco/ThreadPool.h>
#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <iostream>
#include <chrono>

void demo_thread_pool() {
    std::cout << "=== Thread Pool Demo ===" << std::endl;
    
    // Get reference to default thread pool
    Poco::ThreadPool& pool = Poco::ThreadPool::defaultPool();
    std::cout << "Thread pool capacity: " << pool.capacity() << std::endl;
    std::cout << "Available threads: " << pool.available() << std::endl;
    std::cout << "Used threads: " << pool.used() << std::endl;
    
    std::cout << "Thread pool demo completed." << std::endl;
}

void demo_logging() {
    std::cout << "=== Logging Demo ===" << std::endl;
    
    Poco::Logger& logger = Poco::Logger::get("DemoLogger");
    
    // Test different log levels
    logger.fatal("This is a fatal message");
    logger.error("This is an error message");
    logger.warning("This is a warning message");
    logger.information("This is an info message");
    logger.debug("This is a debug message");
    
    // Test logger name retrieval
    std::cout << "Logger name: " << logger.name() << std::endl;
    
    std::cout << "Logging demo completed." << std::endl;
}

void demo_thread_features() {
    std::cout << "=== Thread Features Demo ===" << std::endl;
    
    // Test thread sleep functionality
    auto start = std::chrono::steady_clock::now();
    Poco::Thread::sleep(10);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Thread sleep test completed in " << duration.count() << "ms" << std::endl;
    
    // Test current thread yield
    Poco::Thread::yield();
    std::cout << "Thread yield completed." << std::endl;
    
    std::cout << "Thread features demo completed." << std::endl;
}

void demo_logger_levels() {
    std::cout << "=== Logger Levels Demo ===" << std::endl;
    
    Poco::Logger& logger = Poco::Logger::get("LevelLogger");
    
    // Set logger to trace level to ensure all levels are enabled
    logger.setLevel(Poco::Message::PRIO_TRACE);
    
    // Test logger level checks
    if (logger.fatal()) {
        std::cout << "Fatal logging is enabled" << std::endl;
    }
    
    if (logger.error()) {
        std::cout << "Error logging is enabled" << std::endl;
    }
    
    if (logger.warning()) {
        std::cout << "Warning logging is enabled" << std::endl;
    }
    
    if (logger.information()) {
        std::cout << "Info logging is enabled" << std::endl;
    }
    
    if (logger.debug()) {
        std::cout << "Debug logging is enabled" << std::endl;
    }
    
    if (logger.trace()) {
        std::cout << "Trace logging is enabled" << std::endl;
    }
    
    std::cout << "Logger levels demo completed." << std::endl;
}

int main() {
    std::cout << "=== Poco Library Demo ===" << std::endl;
    
    // Run all demo functions to ensure 100% coverage
    demo_logging();
    demo_thread_pool();
    demo_thread_features();
    demo_logger_levels();
    
    std::cout << "=== All demos completed successfully ===" << std::endl;
    return 0;
}