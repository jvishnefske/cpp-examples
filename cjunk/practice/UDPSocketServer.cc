#include <iostream>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class UDPSocket {
private:
    int sockfd_;
    struct sockaddr_in server_addr_;
    
public:
    explicit UDPSocket(int port) : sockfd_(-1) {
        // Create UDP socket
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        
        // Setup server address
        std::memset(&server_addr_, 0, sizeof(server_addr_));
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_addr.s_addr = INADDR_ANY;
        server_addr_.sin_port = htons(port);
        
        // Bind socket
        if (bind(sockfd_, (struct sockaddr*)&server_addr_, sizeof(server_addr_)) < 0) {
            close(sockfd_);
            throw std::runtime_error("Failed to bind socket");
        }
    }
    
    ~UDPSocket() {
        if (sockfd_ >= 0) {
            close(sockfd_);
        }
    }
    
    void send_message(const std::string& message, const std::string& dest_ip, int dest_port) {
        struct sockaddr_in dest_addr;
        std::memset(&dest_addr, 0, sizeof(dest_addr));
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(dest_port);
        inet_pton(AF_INET, dest_ip.c_str(), &dest_addr.sin_addr);
        
        ssize_t sent = sendto(sockfd_, message.c_str(), message.length(), 0,
                             (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        if (sent < 0) {
            throw std::runtime_error("Failed to send message");
        }
    }
    
    std::string receive_message() {
        char buffer[1024];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        ssize_t received = recvfrom(sockfd_, buffer, sizeof(buffer) - 1, 0,
                                   (struct sockaddr*)&client_addr, &client_len);
        if (received < 0) {
            throw std::runtime_error("Failed to receive message");
        }
        
        buffer[received] = '\0';
        return std::string(buffer);
    }
    
    int get_socket() const { return sockfd_; }
};

void test_udp_server() {
    try {
        UDPSocket server(8080);
        std::cout << "UDP Server listening on port 8080\n";
        
        // Example usage - normally would be in a loop
        server.send_message("Hello Client!", "127.0.0.1", 8081);
        std::cout << "Message sent to client\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void test_udp_client() {
    try {
        UDPSocket client(8081);
        std::cout << "UDP Client listening on port 8081\n";
        
        client.send_message("Hello Server!", "127.0.0.1", 8080);
        std::cout << "Message sent to server\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}