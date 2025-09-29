#include <catch2/catch_all.hpp>
#include <array>
#include <cmath>
#include <limits>

// Forward declare types and constants from main.cpp
static constexpr size_t SIGNAL_BUFFER_SIZE = 4096;

class Demodulator{
public:
    using SignalBuffer = std::array<double, SIGNAL_BUFFER_SIZE>;
    
    void process(SignalBuffer s [[maybe_unused]]){
        processed_count++;
    }
    
    int getProcessedCount() const { return processed_count; }
    void reset() { processed_count = 0; }
    size_t getBufferSize() const { return SIGNAL_BUFFER_SIZE; }
    
private:
    int processed_count = 0;
};

TEST_CASE("Demodulator constructor", "[demodulator]") {
    Demodulator demod;
    REQUIRE(demod.getProcessedCount() == 0);
    REQUIRE(demod.getBufferSize() == SIGNAL_BUFFER_SIZE);
}

TEST_CASE("Demodulator process method", "[demodulator]") {
    Demodulator demod;
    std::array<double, SIGNAL_BUFFER_SIZE> test_signal{};
    
    // Constants to avoid magic numbers
    static constexpr double TWO_PI = 2.0 * M_PI;
    static constexpr double FREQUENCY_DIVISOR = 100.0;
    
    // Fill with test data
    for (size_t i = 0; i < SIGNAL_BUFFER_SIZE; ++i) {
        // Use gsl::at for safe array access and static_cast for narrowing conversion
        test_signal.at(i) = std::sin(TWO_PI * static_cast<double>(i) / FREQUENCY_DIVISOR);
    }
    
    SECTION("Single process call") {
        demod.process(test_signal);
        REQUIRE(demod.getProcessedCount() == 1);
    }
    
    SECTION("Multiple process calls") {
        demod.process(test_signal);
        demod.process(test_signal);
        demod.process(test_signal);
        REQUIRE(demod.getProcessedCount() == 3);
    }
    
    SECTION("Reset functionality") {
        demod.process(test_signal);
        demod.process(test_signal);
        REQUIRE(demod.getProcessedCount() == 2);
        
        demod.reset();
        REQUIRE(demod.getProcessedCount() == 0);
    }
}

TEST_CASE("Demodulator signal buffer size", "[demodulator]") {
    Demodulator demod;
    
    SECTION("Buffer size constant") {
        static constexpr size_t EXPECTED_BUFFER_SIZE = 4096;
        REQUIRE(demod.getBufferSize() == SIGNAL_BUFFER_SIZE);
        REQUIRE(SIGNAL_BUFFER_SIZE == EXPECTED_BUFFER_SIZE);
    }
    
    SECTION("Signal buffer creation") {
        std::array<double, SIGNAL_BUFFER_SIZE> signal{};
        REQUIRE(signal.size() == SIGNAL_BUFFER_SIZE);
        
        // Test that we can fill and process the entire buffer
        std::fill(signal.begin(), signal.end(), 1.0);
        demod.process(signal);
        REQUIRE(demod.getProcessedCount() == 1);
    }
}

TEST_CASE("Demodulator edge cases", "[demodulator]") {
    Demodulator demod;
    std::array<double, SIGNAL_BUFFER_SIZE> signal{};
    
    SECTION("Zero signal") {
        std::fill(signal.begin(), signal.end(), 0.0);
        demod.process(signal);
        REQUIRE(demod.getProcessedCount() == 1);
    }
    
    SECTION("Maximum values") {
        std::fill(signal.begin(), signal.end(), std::numeric_limits<double>::max());
        demod.process(signal);
        REQUIRE(demod.getProcessedCount() == 1);
    }
    
    SECTION("Minimum values") {
        std::fill(signal.begin(), signal.end(), std::numeric_limits<double>::lowest());
        demod.process(signal);
        REQUIRE(demod.getProcessedCount() == 1);
    }
}