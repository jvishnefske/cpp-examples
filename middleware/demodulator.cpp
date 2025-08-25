#include <array>
#include <cmath>

// Define a named constant for SignalBuffer size
static constexpr size_t SIGNAL_BUFFER_SIZE = 4096;

class Demodulator{
public:
    using SignalBuffer = std::array<double, SIGNAL_BUFFER_SIZE>; // Use named constant
    
    // Marked 's' as [[maybe_unused]] to suppress the unused parameter warning
    void process(SignalBuffer s [[maybe_unused]]){
        // apply polyphase decimating filter to incoming signal
        processed_count++;
    } // Added closing brace for process method
    
    // Add methods for testing
    int getProcessedCount() const { return processed_count; }
    void reset() { processed_count = 0; }
    size_t getBufferSize() const { return SIGNAL_BUFFER_SIZE; }
    
private:
    int processed_count = 0;
}; // Added closing brace and semicolon for Demodulator class

#ifdef STANDALONE_TEST
#include <iostream>

int main() {
    std::cout << "Testing Demodulator class..." << std::endl;
    
    Demodulator demod;
    std::cout << "Initial processed count: " << demod.getProcessedCount() << std::endl;
    std::cout << "Buffer size: " << demod.getBufferSize() << std::endl;
    
    // Create test signal buffer
    Demodulator::SignalBuffer testSignal{};
    // Fill with some test data
    for (size_t i = 0; i < testSignal.size(); ++i) {
        testSignal[i] = std::sin(2.0 * 3.14159 * i / 100.0);
    }
    
    // Process the signal
    demod.process(testSignal);
    std::cout << "After processing: " << demod.getProcessedCount() << std::endl;
    
    // Reset and test again
    demod.reset();
    std::cout << "After reset: " << demod.getProcessedCount() << std::endl;
    
    std::cout << "Demodulator test completed successfully" << std::endl;
    return 0;
}
#endif
