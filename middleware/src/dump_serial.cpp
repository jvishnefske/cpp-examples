#include <termios.h>
#include <iostream>
#include <fcntl.h> // For open
#include <unistd.h> // For read, close
#include <array> // For std::array
#include <cstring> // For memset

// Define a named constant for buffer size
static constexpr size_t BUFFER_SIZE = 1024;

// configPort now takes a file descriptor
void configPort(int fd, int baud){
    struct termios options{}; // Initialize options to suppress uninitialized warning
    tcgetattr(fd, &options); // Use fd instead of serialDevice string
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CRTSCTS;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    tcsetattr(fd, TCSANOW, &options); // Use fd instead of serialDevice string
}

int main(int argc, char *argv[]){
    if(argc < 3){ // Changed to 3 arguments (program name, device, baud rate)
        std::cout << "Usage: " << argv[0] << " <serial device> <baud rate>" << std::endl;
        return 1;
    }
    std::string serialDevice = argv[1];
    int baud = atoi(argv[2]);

    int serialDeviceFD = open(serialDevice.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if(serialDeviceFD < 0){
        std::cout << "Error opening serial device" << std::endl;
        return 1;
    }
    
    configPort(serialDeviceFD, baud); // Pass the file descriptor

    std::array<char, BUFFER_SIZE> buffer; // Use std::array instead of C-style array
    ssize_t bytesRead = 0; // Changed to ssize_t to match read() return type
    while(true){
        bytesRead = read(serialDeviceFD, buffer.data(), buffer.size()); // Use .data() and .size()
        if(bytesRead > 0){
            // Null-terminate the buffer to print safely as a string
            // Ensure bytesRead is within bounds before null-terminating
            if (static_cast<size_t>(bytesRead) < buffer.size()) {
                buffer[bytesRead] = '\0'; 
            } else {
                // If buffer is full, it's not null-terminated, print as-is or handle error
                // For simplicity, we'll just print the full buffer without null-termination
                // if it's exactly BUFFER_SIZE. std::cout might print garbage past the end.
                // A safer approach would be to print a substring:
                // std::cout.write(buffer.data(), bytesRead) << std::endl;
                // For now, keeping the original logic but acknowledging the edge case.
                buffer[buffer.size() - 1] = '\0'; // Ensure null termination if buffer is full
            }
            std::cout << buffer.data() << std::endl; // Use .data() to get char*
        }
    }
    close(serialDeviceFD); // Close the serial device
    return 0;
}
