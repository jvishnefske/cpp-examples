#include <array>
#include <cmath>
class Demodulator{
    using SignalBuffer = std::array<double, 4096>; // Added type to std::array
    void process(SignalBuffer s){
        // apply polyphase decimating filter to incoming signal
