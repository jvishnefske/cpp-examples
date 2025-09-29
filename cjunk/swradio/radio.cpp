#include "radio.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <array>

#include <fcntl.h>
#include <cerrno>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <libv4l2.h>

namespace swradio {

// Timer implementation
Timer::Timer() : start_time(Clock::now()) {}

Timer::~Timer() {
    std::cout << "Timer elapsed: " << elapsed() << " seconds" << std::endl;
}

double Timer::elapsed() const {
    auto end_time = Clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time).count();
}

// SoftwareRadio PIMPL implementation
template<size_t BufferSize>
class SoftwareRadio<BufferSize>::Impl {
public:
    int fd;
    bool ready;
    double frequency_mhz;
    double bandwidth_mhz;
    std::string device_path;
    
    explicit Impl(const std::string& path) 
        : fd(-1), ready(false), frequency_mhz(0.0), bandwidth_mhz(0.0), device_path(path) {
        fd = v4l2_open(path.c_str(), O_RDWR);
        if (fd < 0) {
            throw std::runtime_error("Failed to open device " + path + ": " + strerror(errno));
        }
    }
    
    ~Impl() {
        if (fd >= 0) {
            v4l2_close(fd);
        }
    }
    
    bool configure_device(double freq_mhz, double bw_mhz) {
        if (fd < 0) {
            return false;
        }
        
        // Get device capabilities
        struct v4l2_capability cap{};
        if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
            std::cerr << "Error querying capabilities: " << strerror(errno) << std::endl;
            return false;
        }
        
        // Set frequency
        struct v4l2_frequency freq{};
        freq.tuner = 0;
        freq.type = V4L2_TUNER_RADIO;
        freq.frequency = static_cast<unsigned int>(freq_mhz * 1000000);
        
        if (v4l2_ioctl(fd, VIDIOC_S_FREQUENCY, &freq) == -1) {
            std::cerr << "Error setting frequency: " << strerror(errno) << std::endl;
            return false;
        }
        
        // Set IQ mode modulator
        struct v4l2_modulator mod{};
        mod.index = 0;
        mod.capability = V4L2_TUNER_CAP_LOW;
        mod.rangelow = 0;
        mod.rangehigh = 0;
        
        if (v4l2_ioctl(fd, VIDIOC_S_MODULATOR, &mod) == -1) {
            std::cerr << "Error setting modulator: " << strerror(errno) << std::endl;
            return false;
        }
        
        frequency_mhz = freq_mhz;
        bandwidth_mhz = bw_mhz;
        ready = true;
        
        return true;
    }
    
    int read_device_samples(std::vector<char>& buffer, size_t max_bytes) {
        if (fd < 0 || !ready) {
            return -1;
        }
        
        buffer.resize(std::min(max_bytes, BufferSize));
        
        ssize_t bytes_read = v4l2_read(fd, buffer.data(), buffer.size());
        if (bytes_read < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0; // No data available
            }
            return -1; // Error
        }
        
        buffer.resize(bytes_read);
        return static_cast<int>(bytes_read);
    }
};

// SoftwareRadio template implementation
template<size_t BufferSize>
SoftwareRadio<BufferSize>::SoftwareRadio(const std::string& device_path)
    : pimpl(std::make_unique<Impl>(device_path)) {}

template<size_t BufferSize>
SoftwareRadio<BufferSize>::~SoftwareRadio() = default;

template<size_t BufferSize>
SoftwareRadio<BufferSize>::SoftwareRadio(SoftwareRadio&& other) noexcept
    : pimpl(std::move(other.pimpl)) {}

template<size_t BufferSize>
SoftwareRadio<BufferSize>& SoftwareRadio<BufferSize>::operator=(SoftwareRadio&& other) noexcept {
    if (this != &other) {
        pimpl = std::move(other.pimpl);
    }
    return *this;
}

template<size_t BufferSize>
bool SoftwareRadio<BufferSize>::configure(double frequency_mhz, double bandwidth_mhz) {
    return pimpl ? pimpl->configure_device(frequency_mhz, bandwidth_mhz) : false;
}

template<size_t BufferSize>
int SoftwareRadio<BufferSize>::read_samples(std::vector<char>& buffer, size_t max_bytes) {
    return pimpl ? pimpl->read_device_samples(buffer, max_bytes) : -1;
}

template<size_t BufferSize>
bool SoftwareRadio<BufferSize>::is_ready() const {
    return pimpl ? pimpl->ready : false;
}

template<size_t BufferSize>
std::string SoftwareRadio<BufferSize>::get_device_info() const {
    if (!pimpl) {
        return "Invalid device";
    }
    
    return "SoftwareRadio Device: " + pimpl->device_path + 
           ", Frequency: " + std::to_string(pimpl->frequency_mhz) + " MHz" +
           ", Bandwidth: " + std::to_string(pimpl->bandwidth_mhz) + " MHz" +
           ", Buffer Size: " + std::to_string(BufferSize) + " bytes";
}

// Factory function implementation
std::unique_ptr<IRadioDevice> create_radio_device(const std::string& device_path) {
    try {
        return std::make_unique<SoftwareRadio<config::DEFAULT_BUFFER_SIZE>>(device_path);
    } catch (const std::exception& e) {
        // For testing with fallback devices like /dev/null, create a mock that always succeeds
        if (device_path == "/dev/null") {
            auto device = std::make_unique<SoftwareRadio<config::DEFAULT_BUFFER_SIZE>>(device_path);
            // Note: This will still throw for /dev/null, but that's expected behavior
            // The main.cpp handles this gracefully
        }
        throw; // Re-throw the original exception
    }
}

// Explicit template instantiation for common buffer sizes
template class SoftwareRadio<config::DEFAULT_BUFFER_SIZE>;
template class SoftwareRadio<1024>;
template class SoftwareRadio<8192>;

} // namespace swradio