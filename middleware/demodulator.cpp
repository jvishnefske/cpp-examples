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
