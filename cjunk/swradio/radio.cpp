
// here is the Radio class using RAII with std::array<4096> buffer instead of malloc
#include <iostream>
#include <cstring>


#include <fcntl.h>
#include <cerrno>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <array>
#include <libv4l2.h>

class Radio {
    private:
        int fd;
        int buffer_size;
        int buffer_count;
        int buffer_index;
        std::array<char, 4096> buffer;
        int buffer_ready;
    public:
        Radio(const char *file, int buffer_size, int buffer_count);
        void Config(){

            // get device capabilities
            struct v4l2_capability cap;
            v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap);

            // set software radio device to 140Mhz center frequency, and 6Mhz IQ bandwidth
            struct v4l2_frequency freq;
            freq.tuner = 0;
            freq.type = V4L2_TUNER_RADIO;
            freq.frequency = 140000000;
            v4l2_ioctl(fd, VIDIOC_S_FREQUENCY, &freq);

            // set device to use IQ mode
            struct v4l2_modulator mod;
            mod.index = 0;
            mod.capability = V4L2_TUNER_CAP_LOW;
            mod.rangelow = 0;
            mod.rangehigh = 0;
            v4l2_ioctl(fd, VIDIOC_S_MODULATOR, &mod);

            // 6Mhz input rf bandwidth
            struct v4l2_frequency freq2;
            freq2.tuner = 0;
            freq2.type = V4L2_TUNER_RADIO;
            freq2.frequency = 0;
            v4l2_ioctl(fd, VIDIOC_S_FREQUENCY, &freq2);

        }
        ~Radio();
        int read(char *buffer, int buffer_size);
        int read_ready();
        int read_index();
        int read_count();
};


Radio::Radio(const char *file, int buffer_size, int buffer_count){
    fd = v4l2_open(file, O_RDWR);
    if(fd < 0){
        std::cout << "Error opening " << file << ": " << strerror(errno) << std::endl;
        throw std::runtime_error("could not open");
    }
    buffer_size = buffer_size;
    buffer_count = buffer_count;
    buffer_index = 0;
    buffer_ready = 0;
}

Radio::~Radio(){
    v4l2_close(fd);
}

int Radio::read(char *buffer, int buffer_size){
    if(buffer_ready == 0){
        return 0;
    }
    memcpy(buffer, buffer + buffer_index * buffer_size, buffer_size);
    buffer_index = (buffer_index + 1) % buffer_count;
    buffer_ready--;
    return buffer_size;
}

int Radio::read_ready(){
    return buffer_ready;
}

int Radio::read_index(){
    return buffer_index;
}

int Radio::read_count(){
    return buffer_count;
}

// use raii to calculate the time of a code block
using Clock = std::chrono::high_resolution_clock;
class Timer{
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


// main function
int main(){
    tictoc t;
    (void) t;
    // open /dev/swradio0
    Radio radio("/dev/swradio0", 4096, 16);

    int count=0;

    // start receiving raw samples in while loop
    while(count<1000000){
        // read samples
        char buffer[4096];
        radio.read(buffer, 4096);
    count ++;
    }
}

