#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <errno.h>
#include <signal.h>
// open a udp socket, and parse quic dataframes
class Socket;
class response{
    uint16_t timestamp;
    std::array<char,4> channelName
    enum DataType type;
};

class Socket{
    public:
    // create a socket
    Socket(const std::string_view port);
    // send data
    void send(std::string_view);
    // receive data
    void receive(std::string_view);
    // close the socket
    void close();
};

// create a socket
Socket::Socket(const std::string_view port)
{
    // create a socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        std::cerr << "socket() failed" << std::endl;
        return;
    }
    // bind the socket to the port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(std::stoi(port));
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "bind() failed" << std::endl;
        return;
    }
    // set the socket to non-blocking
;    int flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0)
    {
        std::cerr << "fcntl() failed" << std::endl;
        return;
    }
    flags |= O_NONBLOCK;
    if (fcntl(sock, F_SETFL, flags) < 0)
    {
        std::cerr << "fcntl() failed" << std::endl;
        return;
    }
    // save the socket
    _sock = sock;
}

// send data
void Socket::send(std::string_view data)
{
    // send the data
    ssize_t n = send(_sock, data.data(), data.size(), 0);
    if (n < 0)
    {
        std::cerr << "send() failed" << std::endl;
        return;
    }
}

// receive data
void Socket::receive(std::string_view data)
{
    // receive the data
    ssize_t n = recv(_sock, data.data(), data.size(), 0);
    if (n < 0)
    {
        std::cerr << "recv() failed" << std::endl;
        return;
    }
}

// close the socket
void Socket::close()
{
    // close the socket
    close(_sock);
}
void test_socket(){
    Socket s("1234");
    s.send("hello");
    s.receive("hello");
    s.close();
}

// open a udp socket, and parse quic dataframes
int main(){
    test_socket();
}

