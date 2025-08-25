// Simple radio demo - standalone version similar to original radio.cpp
#include <iostream>
#include <cstring>
#include <chrono>
#include <array>

#include <fcntl.h>
#include <cerrno>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <libv4l2.h>

namespace {
// Constants to avoid magic numbers
constexpr unsigned int DEFAULT_FREQUENCY_HZ = 140000000U;
constexpr int MAX_READ_COUNT = 1000; // Reduced from 100000 for faster demo
constexpr int BUFFER_SIZE = 4096;

template<int bufferSize>
class Radio {
private:
    int fd;
    int buffer_count;
    int buffer_index;
    int buffer_ready;
public:
    explicit Radio(const char *file) : fd(-1), buffer_count(0), buffer_index(0), buffer_ready(0) {
        fd = v4l2_open(file, O_RDWR);
        if(fd < 0){
            std::cout << "Error opening " << file << ": " << strerror(errno) << std::endl;
        }
    }
    
    // Rule of five - explicitly delete copy operations for RAII resource management
    Radio(const Radio&) = delete;
    Radio& operator=(const Radio&) = delete;
    Radio(Radio&&) = delete;
    Radio& operator=(Radio&&) = delete;
    
    void Config(){
        if (fd < 0) return;
        
        // get device capabilities
        struct v4l2_capability cap{};
        if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
            std::cerr << "Error querying capabilities: " << strerror(errno) << std::endl;
        }

        // set software radio device to 140Mhz center frequency, and 6Mhz IQ bandwidth
        struct v4l2_frequency freq{};
        freq.tuner = 0;
        freq.type = V4L2_TUNER_RADIO;
        freq.frequency = DEFAULT_FREQUENCY_HZ;
        if (v4l2_ioctl(fd, VIDIOC_S_FREQUENCY, &freq) == -1) {
            std::cerr << "Error setting frequency: " << strerror(errno) << std::endl;
        }

        // set device to use IQ mode
        struct v4l2_modulator mod{};
        mod.index = 0;
        mod.capability = V4L2_TUNER_CAP_LOW;
        mod.rangelow = 0;
        mod.rangehigh = 0;
        if (v4l2_ioctl(fd, VIDIOC_S_MODULATOR, &mod) == -1) {
            std::cerr << "Error setting modulator: " << strerror(errno) << std::endl;
        }

        // 6Mhz input rf bandwidth
        struct v4l2_frequency freq2{};
        freq2.tuner = 0;
        freq2.type = V4L2_TUNER_RADIO;
        freq2.frequency = 0;
        if (v4l2_ioctl(fd, VIDIOC_S_FREQUENCY, &freq2) == -1) {
            std::cerr << "Error setting bandwidth: " << strerror(errno) << std::endl;
        }
    }
    
    ~Radio() {
        if (fd >= 0) {
            v4l2_close(fd);
        }
    }
    
    int read(char *buffer, int buffer_size) {
        if (fd < 0) {
            return -1;
        }
        
        ssize_t bytes_read = v4l2_read(fd, buffer, buffer_size);
        if (bytes_read < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0; // No data available
            }
            return -1; // Error
        }
        
        return static_cast<int>(bytes_read);
    }
};

// use raii to calculate the time of a code block
class Timer{
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start;
public:
    Timer(): start(Clock::now()){}
    double elapsed(){
        Clock::time_point end = Clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
    }
    ~Timer(){
        std::cout << "elapsed time: " << elapsed() << " seconds" << std::endl;
    }
};

} // namespace

// main function
int main(){
    Timer t;
    // open /dev/swradio0
    Radio<BUFFER_SIZE> radio{"/dev/swradio0"};

    radio.Config();
    
    int count = 0;
    int successful_reads = 0;

    std::cout << "Starting radio demo (will try " << MAX_READ_COUNT << " reads)..." << std::endl;
    
    // start receiving raw samples in while loop
    while(count < MAX_READ_COUNT){
        // read samples
        std::array<char, BUFFER_SIZE> buffer{};
        int bytes_read = radio.read(buffer.data(), BUFFER_SIZE);
        
        if (bytes_read > 0) {
            successful_reads++;
        } else if (bytes_read < 0) {
            std::cerr << "Read error at iteration " << count << std::endl;
            break;
        }
        
        count++;
        
        // Print progress every 100 reads
        if (count % 100 == 0) {
            std::cout << "Progress: " << count << "/" << MAX_READ_COUNT 
                      << " (" << successful_reads << " successful)" << std::endl;
        }
    }
    
    std::cout << "Demo completed. Total reads: " << count 
              << ", Successful: " << successful_reads << std::endl;
    
    return 0;
}