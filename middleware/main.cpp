#include <array>
#include <cmath>
class Demodulator{
    using SignalBuffer = std::array<4096>;
    void process(SignalBuffer s){
        // apply polyphase decimating filter to incoming signal