// here is the Radio class using RAII with std::array<4096> buffer instead of malloc
#include <iostream>
#include <cstring>
#include <chrono>
#include <array>

#include <fcntl.h>
#include <cerrno>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <libv4l2.h>

namespace{
// Constants to avoid magic numbers
constexpr unsigned int DEFAULT_FREQUENCY_HZ = 140000000U;
constexpr int MAX_READ_COUNT = 100000;
constexpr int BUFFER_SIZE = 4096;

template<int bufferSize>
class Radio {
private:
    int fd;
    //int buffer_size;
    int buffer_count;
    int buffer_index;
    //std::array<char, 4096> buffer;
    int buffer_ready;
public:
    explicit Radio(const char *file);
    
    // Rule of five - explicitly delete copy operations for RAII resource management
    Radio(const Radio&) = delete;
    Radio& operator=(const Radio&) = delete;
    Radio(Radio&&) = delete;
    Radio& operator=(Radio&&) = delete;
    
    void Config(){
        // get device capabilities
        struct v4l2_capability cap{};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
        if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
            std::cerr << "Error querying capabilities: " << strerror(errno) << std::endl;
        }

        // set software radio device to 140Mhz center frequency, and 6Mhz IQ bandwidth
        struct v4l2_frequency freq{};
        freq.tuner = 0;
        freq.type = V4L2_TUNER_RADIO;
        freq.frequency = DEFAULT_FREQUENCY_HZ;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
        if (v4l2_ioctl(fd, VIDIOC_S_FREQUENCY, &freq) == -1) {
            std::cerr << "Error setting frequency: " << strerror(errno) << std::endl;
        }

        // set device to use IQ mode
        struct v4l2_modulator mod{};
        mod.index = 0;
        mod.capability = V4L2_TUNER_CAP_LOW;
        mod.rangelow = 0;
        mod.rangehigh = 0;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
        if (v4l2_ioctl(fd, VIDIOC_S_MODULATOR, &mod) == -1) {
            std::cerr << "Error setting modulator: " << strerror(errno) << std::endl;
        }

        // 6Mhz input rf bandwidth
        struct v4l2_frequency freq2{};
        freq2.tuner = 0;
        freq2.type = V4L2_TUNER_RADIO;
        freq2.frequency = 0;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
        if (v4l2_ioctl(fd, VIDIOC_S_FREQUENCY, &freq2) == -1) {
            std::cerr << "Error setting bandwidth: " << strerror(errno) << std::endl;
        }
    }
    ~Radio();
    auto read(char *buffer, int buffer_size) -> int;
    auto read_ready() -> int;
    auto read_index() -> int;
    auto read_count() -> int;

    //int read(char *buffer, int buffer_size);
};





    template<int bufferSize>

int Radio<bufferSize>::read_ready(){
    return buffer_ready;
}
    template<int bufferSize>

int Radio<bufferSize>::read_index(){
    return buffer_index;
}
    template<int bufferSize>

int Radio<bufferSize>::read_count(){
    return buffer_count;
}

    template<int bufferSize>
    Radio<bufferSize>::~Radio()
    {
        v4l2_close(fd);
    }

    template<int bufferSize>
    Radio<bufferSize>::Radio(const char *file) : fd(-1), buffer_count(0), buffer_index(0), buffer_ready(0)
    {
        fd = v4l2_open(file, O_RDWR);
        if(fd < 0){
            std::cout << "Error opening " << file << ": " << strerror(errno) << std::endl;
            //throw std::runtime_error("could not open");
        }
    }

    template<int bufferSize>
    int Radio<bufferSize>::read(char *buffer, int buffer_size)
    {
        if(buffer_ready == 0){
            return 0;
        }
        memcpy(buffer, buffer + buffer_index * buffer_size, buffer_size);
        buffer_index = (buffer_index + 1) % buffer_count;
        buffer_ready--;
        return buffer_size;
    }

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
        std::cout << "elapsed time: " << elapsed() << std::endl;
    }
    
};

} // namespace

// main function
int main(){
    Timer t;
    // open /dev/swradio0
    Radio<BUFFER_SIZE> radio{"/dev/swradio0"};

    int count = 0;

    // start receiving raw samples in while loop
    while(count < MAX_READ_COUNT){
        // read samples
        std::array<char, BUFFER_SIZE> buffer{};
        radio.read(buffer.data(), BUFFER_SIZE);
        count++;
    }
    
    return 0;
}

