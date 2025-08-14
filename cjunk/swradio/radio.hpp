#pragma once

#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace swradio {

// Constants for radio configuration
namespace config {
    constexpr int DEFAULT_BUFFER_SIZE = 4096;
    constexpr double DEFAULT_FREQUENCY_MHZ = 140.0;
    constexpr double DEFAULT_BANDWIDTH_MHZ = 6.0;
    constexpr int MAX_READ_ATTEMPTS = 100000;
} // namespace config

// Modern RAII timer class
class Timer {
public:
    Timer();
    ~Timer();
    
    double elapsed() const;
    
private:
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_time;
};

// Abstract interface for radio devices
class IRadioDevice {
public:
    virtual ~IRadioDevice() = default;
    
    virtual bool configure(double frequency_mhz, double bandwidth_mhz) = 0;
    virtual int read_samples(std::vector<char>& buffer, size_t max_bytes) = 0;
    virtual bool is_ready() const = 0;
    virtual std::string get_device_info() const = 0;
};

// Software-defined radio implementation
template<size_t BufferSize = config::DEFAULT_BUFFER_SIZE>
class SoftwareRadio : public IRadioDevice {
public:
    explicit SoftwareRadio(const std::string& device_path);
    ~SoftwareRadio() override;
    
    // Delete copy operations for RAII safety
    SoftwareRadio(const SoftwareRadio&) = delete;
    SoftwareRadio& operator=(const SoftwareRadio&) = delete;
    
    // Allow move operations
    SoftwareRadio(SoftwareRadio&&) noexcept;
    SoftwareRadio& operator=(SoftwareRadio&&) noexcept;
    
    bool configure(double frequency_mhz, double bandwidth_mhz) override;
    int read_samples(std::vector<char>& buffer, size_t max_bytes) override;
    bool is_ready() const override;
    std::string get_device_info() const override;
    
    size_t get_buffer_size() const { return BufferSize; }
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

// Factory function for creating radio devices
std::unique_ptr<IRadioDevice> create_radio_device(const std::string& device_path);

} // namespace swradio