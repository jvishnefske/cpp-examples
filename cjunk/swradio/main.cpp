#include "radio.hpp"
#include <iostream>
#include <vector>
#include <stdexcept>

namespace {
    constexpr const char* DEFAULT_DEVICE_PATH = "/dev/swradio0";
    constexpr const char* FALLBACK_DEVICE_PATH = "/dev/null"; // For testing when hardware not available
}

int main() {
    try {
        swradio::Timer operation_timer;
        
        std::cout << "Starting Software Defined Radio Demo\n";
        std::cout << "=====================================\n";
        
        // Try to create radio device - fallback to mock if hardware not available
        std::unique_ptr<swradio::IRadioDevice> radio;
        
        try {
            radio = swradio::create_radio_device(DEFAULT_DEVICE_PATH);
            std::cout << "Successfully opened hardware device: " << DEFAULT_DEVICE_PATH << '\n';
        } catch (const std::exception& e) {
            std::cout << "Hardware device not available, using fallback for demo: " 
                      << e.what() << '\n';
            radio = swradio::create_radio_device(FALLBACK_DEVICE_PATH);
        }
        
        // Configure the radio
        const bool config_success = radio->configure(
            swradio::config::DEFAULT_FREQUENCY_MHZ,
            swradio::config::DEFAULT_BANDWIDTH_MHZ
        );
        
        if (!config_success) {
            std::cerr << "Failed to configure radio device\n";
            return 1;
        }
        
        std::cout << radio->get_device_info() << '\n';
        
        if (!radio->is_ready()) {
            std::cerr << "Radio device is not ready\n";
            return 1;
        }
        
        // Read samples
        std::vector<char> sample_buffer;
        sample_buffer.reserve(swradio::config::DEFAULT_BUFFER_SIZE);
        
        int successful_reads = 0;
        constexpr int target_reads = 1000; // Reduced from 100000 for faster demo
        
        std::cout << "Reading " << target_reads << " sample buffers...\n";
        
        for (int count = 0; count < target_reads; ++count) {
            const int bytes_read = radio->read_samples(
                sample_buffer, 
                swradio::config::DEFAULT_BUFFER_SIZE
            );
            
            if (bytes_read > 0) {
                ++successful_reads;
                
                // Print progress every 100 reads
                if ((count + 1) % 100 == 0) {
                    std::cout << "Progress: " << (count + 1) << "/" << target_reads 
                              << " (" << successful_reads << " successful)\n";
                }
            } else if (bytes_read < 0) {
                std::cerr << "Read error occurred at iteration " << count << '\n';
                break;
            }
            // bytes_read == 0 means no data available, continue
        }
        
        std::cout << "\nDemo completed successfully!\n";
        std::cout << "Total successful reads: " << successful_reads << "/" << target_reads << '\n';
        
        if (successful_reads == 0) {
            std::cout << "Note: No data was read. This is normal when using fallback device.\n";
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred\n";
        return 1;
    }
}