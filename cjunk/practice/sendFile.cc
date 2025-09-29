#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

class FileSocket {
private:
    int sockfd_;
    
public:
    explicit FileSocket() : sockfd_(-1) {
        sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd_ < 0) {
            throw std::runtime_error("Failed to create socket");
        }
    }
    
    ~FileSocket() {
        if (sockfd_ >= 0) {
            close(sockfd_);
        }
    }
    
    void connect_to(const std::string& host, int port) {
        struct sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
        
        if (connect(sockfd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw std::runtime_error("Failed to connect to server");
        }
    }
    
    void bind_and_listen(int port) {
        struct sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);
        
        if (bind(sockfd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            throw std::runtime_error("Failed to bind socket");
        }
        
        if (listen(sockfd_, 5) < 0) {
            throw std::runtime_error("Failed to listen on socket");
        }
    }
    
    int accept_connection() {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(sockfd_, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            throw std::runtime_error("Failed to accept connection");
        }
        return client_fd;
    }
    
    int get_socket() const { return sockfd_; }
};

class FileTransfer {
public:
    static std::vector<char> read_file(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<char> buffer(size);
        file.read(buffer.data(), size);
        return buffer;
    }
    
    static void write_file(const std::string& filename, const std::vector<char>& data) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot create file: " + filename);
        }
        file.write(data.data(), data.size());
    }
    
    static void send_file_data(int socket_fd, const std::vector<char>& data) {
        size_t total_sent = 0;
        while (total_sent < data.size()) {
            ssize_t sent = send(socket_fd, data.data() + total_sent, 
                               data.size() - total_sent, 0);
            if (sent < 0) {
                throw std::runtime_error("Failed to send data");
            }
            total_sent += sent;
        }
    }
    
    static std::vector<char> receive_file_data(int socket_fd, size_t expected_size) {
        std::vector<char> buffer(expected_size);
        size_t total_received = 0;
        
        while (total_received < expected_size) {
            ssize_t received = recv(socket_fd, buffer.data() + total_received,
                                   expected_size - total_received, 0);
            if (received <= 0) {
                throw std::runtime_error("Failed to receive data");
            }
            total_received += received;
        }
        return buffer;
    }
};

void test_file_server() {
    try {
        FileSocket server;
        server.bind_and_listen(9090);
        std::cout << "File server listening on port 9090\n";
        
        // Accept one connection for demo
        int client_fd = server.accept_connection();
        std::cout << "Client connected\n";
        
        // Send a sample file (would normally read from disk)
        std::string sample_data = "Hello from file server!";
        std::vector<char> data(sample_data.begin(), sample_data.end());
        FileTransfer::send_file_data(client_fd, data);
        
        close(client_fd);
        std::cout << "File sent successfully\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }
}

void test_file_client() {
    try {
        FileSocket client;
        client.connect_to("127.0.0.1", 9090);
        std::cout << "Connected to file server\n";
        
        // Receive file data
        std::vector<char> received_data = FileTransfer::receive_file_data(
            client.get_socket(), 23); // Expected size
        
        std::string received_str(received_data.begin(), received_data.end());
        std::cout << "Received: " << received_str << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}