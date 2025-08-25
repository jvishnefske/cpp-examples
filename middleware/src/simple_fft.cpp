//
// Created by j on 10/23/21.
//

#include "simple_fft.h"
#include <iostream>

#ifdef STANDALONE_TEST
int main() {
    // Create some test data
    std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0, 4.0, 3.0, 2.0};
    
    // Create FFT object
    FFT fft(data);
    
    std::cout << "FFT test with " << fft.size() << " samples" << std::endl;
    
    // Get and display some results
    auto real_part = fft.get_real();
    auto imag_part = fft.get_imag();
    
    std::cout << "First few real parts: ";
    for (size_t i = 0; i < std::min(size_t(4), real_part.size()); ++i) {
        std::cout << real_part[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "FFT test completed successfully" << std::endl;
    return 0;
}
#endif
