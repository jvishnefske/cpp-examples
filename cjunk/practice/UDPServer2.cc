#include <functional>
#include <variant>
#include <algorithm>

enum FrameTypeNames{
    PADDING = 0,
    PING =0x1,
    ACK = 0x2,
    RESET_STREAM = 0x4,
    STOP_SENDING = 0x5,
    CRYPTO = 0x6,
    NEW_TOKEN = 0x7,
    STREAM = 0x8,
    MAX_DATA=x10,
    MAX_STREAM_DATA = 0x11,
    MAX_STREAMS = 0x12,
    DATA_BLOCKED = 0x14,
    STREAM_DATA_BLOCKED = 0x15,
    STREAMS_BLOCKED = 0x16,
    NEW_CONNECTION_ID 0x18,
    RETIRE_CONNECTION_ID = 0x19,
    PATH_CHALLANGE = 0x1a,
    PATH_RESPONSE = 0x1b,
    CONNECTION_CLOSE = 0x1c,
    HANDSHAKE_DONE = 0x1e
}

enum States{
    StateRecv,
    StateSizeKnown,
    StateDataReceived,
    StateDataRead,
    StateResetRecieved,
    StateResetRead
}
class Socket{
    public:
    Socket(int port);
    ~Socket();
    void send(std::string);
    std::fuction<void(std::string)> recv;
    private:
    int sock;
    std::function<void(std::string)> recv;
};
Socket::Socket(int port){
    //open udp socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        std::cout << "Error opening socket" << std::endl;
        exit(1);
    }
    //bind socket to port
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        std::cout << "Error binding socket" << std::endl;
        exit(1);
    }
}
Socket::~Socket(){
    close(sock);
}
void Socket::send(std::string data){
    //send data
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = INADDR_ANY;
    sendto(sock, data.c_str(), data.length(), 0, (struct sockaddr *)&client, sizeof(client));
}
void test_socket(){
    Socket s(1234);
    std::function<void(std::string)> callback = [](std::string){
        std::cout << "callback" << std::endl;
    };
    s.recv = callback;
    s.send("hello");
}
// thread to listen to socket, and call recv with anything read.
class SocketThread{
    public:
    SocketThread(Socket *socket);
    ~SocketThread();
    private:
    void run();
    Socket *socket;
    bool m_running
    std::thread thread;
};
SocketThread::SocketThread(Socket *socket){
    this->socket = socket;
    m_running = true;
    thread = std::thread(&SocketThread::run, this);
}
SocketThread::~SocketThread(){
    m_running = false;
    thread.join();
}
void SocketThread::run(){
    while(m_running){
        std::array<1024, char> buffer;
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int bytes_read = recvfrom(socket->sock, buffer.data(), buffer.size(), 0, (struct sockaddr *)&client, &client_len);
        if(bytes_read < 0){
            std::cout << "Error reading from socket" << std::endl;
            exit(1);
        }
        std::string data(buffer.data(), bytes_read);
        socket->recv(data);
    }
}
void test_socket_thread(){
    Socket s(1234);
    std::function<void(std::string)> callback = [](std::string){
        std::cout << "callback" << std::endl;
    };
    s.recv = callback;
    SocketThread st(&s);
    s.send("hello");
}
// thread to listen to socket, and call recv with anything read.
class SocketThread{
    public:
    SocketThread(Socket *socket);
    ~SocketThread();
    private:
    void run();
    Socket *socket;
    std::thread thread;
};
SocketThread::SocketThread(Socket *socket
