//
// Created by j on 10/23/21.
//

#include "fftw3.h"
#include <vector> // Added missing include for std::vector

// a c++ raii class to take the complex fft from a container of doubles
class FFT {
public:
    // Marked 'data' as [[maybe_unused]] to suppress the unused parameter warning
    FFT(const std::vector<double>& data [[maybe_unused]]) {

    }
}; // Added missing semicolon for class definition
