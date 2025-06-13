#include <termios.h>
#include <iostream>
#include <fcntl.h> // For open
#include <unistd.h> // For read, close

// configPort now takes a file descriptor
void configPort(int fd, int baud){
    struct termios options;
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

    int bytesRead = 0;
    char buffer[1024];
    while(true){
        bytesRead = read(serialDeviceFD, buffer, 1024);
        if(bytesRead > 0){
            // Null-terminate the buffer to print safely as a string
            buffer[bytesRead] = '\0'; 
            std::cout << buffer << std::endl;
        }
    }
    close(serialDeviceFD); // Close the serial device
    return 0;
}
