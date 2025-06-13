#include <termios.h>
#include <iostream>
void configPort(int baud, std::string serialDevice){
    struct termios options;
    tcgetattr(serialDevice, &options);
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
    tcsetattr(serialDevice, TCSANOW, &options);
}

int main(int argc, char *argv[]){
    if(argc < 2){
        std::cout << "Usage: " << argv[0] << " <serial device> <baud rate>" << std::endl;
        return 1;
    }
    std::string serialDevice = argv[1];
    int baud = atoi(argv[2]);
    configPort(baud, serialDevice);
    int serialDeviceFD = open(serialDevice.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if(serialDeviceFD < 0){
        std::cout << "Error opening serial device" << std::endl;
        return 1;
    }
    int bytesRead = 0;
    char buffer[1024];
    while(true){
        bytesRead = read(serialDeviceFD, buffer, 1024);
        if(bytesRead > 0){
            std::cout << buffer << std::endl;
        }
    }
    return 0;
}