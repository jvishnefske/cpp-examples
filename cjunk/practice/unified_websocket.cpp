// Unified WebSocket Implementation
// Combines critical features from multiple networking files
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy"
#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <thread>
#include <chrono>
#include <queue>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/websocket.hpp>
#pragma GCC diagnostic pop

namespace net = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
using tcp = net::ip::tcp;

// Forward declarations
class WebSocketSession;
class WebSocketServer;
class WebSocketClient;

// Base class for network operations
class NetworkBase {
protected:
    net::io_context io_context_;
    static constexpr size_t MAX_BUFFER_SIZE = 1024;

public:
    NetworkBase() = default;
    virtual ~NetworkBase() = default;
    
    void run() { io_context_.run(); }
    void stop() { io_context_.stop(); }
};

// WebSocket Session for handling individual connections
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
private:
    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    std::function<void(const std::string&)> message_handler_;

public:
    explicit WebSocketSession(tcp::socket socket, std::function<void(const std::string&)> handler)
        : ws_(std::move(socket))
        , message_handler_(std::move(handler)) {}

    void run() {
        // Accept the WebSocket handshake
        ws_.async_accept([self = shared_from_this()](beast::error_code ec) {
            if (!ec) {
                self->do_read();
            } else {
                std::cerr << "WebSocket accept error: " << ec.message() << std::endl;
            }
        });
    }

    void send(const std::string& message) {
        ws_.async_write(net::buffer(message),
            [self = shared_from_this()](beast::error_code ec, std::size_t /*bytes_transferred*/) {
                if (ec) {
                    std::cerr << "WebSocket write error: " << ec.message() << std::endl;
                }
            });
    }

private:
    void do_read() {
        ws_.async_read(buffer_,
            [self = shared_from_this()](beast::error_code ec, std::size_t /*bytes_transferred*/) {
                if (!ec) {
                    // Process the message
                    std::string message = beast::buffers_to_string(self->buffer_.data());
                    if (self->message_handler_) {
                        self->message_handler_(message);
                    }
                    
                    // Clear buffer and continue reading
                    self->buffer_.clear();
                    self->do_read();
                } else if (ec != websocket::error::closed) {
                    std::cerr << "WebSocket read error: " << ec.message() << std::endl;
                }
            });
    }
};

// WebSocket Server
class WebSocketServer : public NetworkBase {
private:
    tcp::acceptor acceptor_;
    std::function<void(const std::string&)> message_handler_;

public:
    WebSocketServer(int port, std::function<void(const std::string&)> handler)
        : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port))
        , message_handler_(std::move(handler)) {
        
        std::cout << "WebSocket Server started on port " << port << std::endl;
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                // Create a new session for this connection
                std::make_shared<WebSocketSession>(std::move(socket), message_handler_)->run();
            } else {
                std::cerr << "Accept error: " << ec.message() << std::endl;
            }
            
            // Continue accepting new connections
            do_accept();
        });
    }
};

// WebSocket Client
class WebSocketClient : public NetworkBase {
private:
    websocket::stream<tcp::socket> ws_;
    tcp::resolver resolver_;
    beast::flat_buffer buffer_;
    std::function<void(const std::string&)> message_handler_;

public:
    WebSocketClient(const std::string& host, const std::string& port, 
                   std::function<void(const std::string&)> handler)
        : ws_(io_context_)
        , resolver_(io_context_)
        , message_handler_(std::move(handler)) {
        
        connect(host, port);
    }

    void send(const std::string& message) {
        ws_.async_write(net::buffer(message),
            [](beast::error_code ec, std::size_t /*bytes_transferred*/) {
                if (ec) {
                    std::cerr << "Client write error: " << ec.message() << std::endl;
                }
            });
    }

private:
    void connect(const std::string& host, const std::string& port) {
        resolver_.async_resolve(host, port,
            [this](beast::error_code ec, tcp::resolver::results_type results) {
                if (!ec) {
                    net::async_connect(ws_.next_layer(), results,
                        [this](beast::error_code ec, tcp::resolver::results_type::endpoint_type /*endpoint*/) {
                            if (!ec) {
                                do_handshake();
                            } else {
                                std::cerr << "Connect error: " << ec.message() << std::endl;
                            }
                        });
                } else {
                    std::cerr << "Resolve error: " << ec.message() << std::endl;
                }
            });
    }

    void do_handshake() {
        ws_.async_handshake("localhost", "/",
            [this](beast::error_code ec) {
                if (!ec) {
                    std::cout << "WebSocket client connected" << std::endl;
                    do_read();
                } else {
                    std::cerr << "Handshake error: " << ec.message() << std::endl;
                }
            });
    }

    void do_read() {
        ws_.async_read(buffer_,
            [this](beast::error_code ec, std::size_t /*bytes_transferred*/) {
                if (!ec) {
                    std::string message = beast::buffers_to_string(buffer_.data());
                    if (message_handler_) {
                        message_handler_(message);
                    }
                    
                    buffer_.clear();
                    do_read();
                } else if (ec != websocket::error::closed) {
                    std::cerr << "Client read error: " << ec.message() << std::endl;
                }
            });
    }
};

// Simple HTTP Server (for basic web serving)
class SimpleHttpServer : public NetworkBase {
private:
    tcp::acceptor acceptor_;

public:
    SimpleHttpServer(int port) : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)) {
        std::cout << "HTTP Server started on port " << port << std::endl;
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::thread([socket = std::move(socket)]() mutable {
                    handle_http_request(std::move(socket));
                }).detach();
            }
            do_accept();
        });
    }

    static void handle_http_request(tcp::socket socket) {
        try {
            beast::flat_buffer buffer;
            beast::http::request<beast::http::string_body> req;
            
            // Read the HTTP request
            beast::http::read(socket, buffer, req);
            
            // Create a simple response
            beast::http::response<beast::http::string_body> res{
                beast::http::status::ok, req.version()};
            res.set(beast::http::field::server, "Unified WebSocket Server");
            res.set(beast::http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            
            std::string body = R"(
<!DOCTYPE html>
<html>
<head><title>WebSocket Test</title></head>
<body>
<h1>WebSocket Server Running</h1>
<p>Connect to ws://localhost:8080/ for WebSocket communication</p>
</body>
</html>
)";
            res.body() = body;
            res.prepare_payload();
            
            // Send the response
            beast::http::write(socket, res);
        } catch (const std::exception& e) {
            std::cerr << "HTTP request handling error: " << e.what() << std::endl;
        }
    }
};

// Test functions
void test_websocket_server() {
    std::cout << "Starting WebSocket server test..." << std::endl;
    
    WebSocketServer server(8080, [](const std::string& message) {
        std::cout << "Server received: " << message << std::endl;
    });
    
    // Run for a short time for testing
    std::thread server_thread([&server]() {
        server.run();
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    server.stop();
    server_thread.join();
}

void test_http_server() {
    std::cout << "Starting HTTP server test..." << std::endl;
    
    SimpleHttpServer server(8081);
    
    std::thread server_thread([&server]() {
        server.run();
    });
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    server.stop();
    server_thread.join();
}

int main() {
    std::cout << "Unified WebSocket Implementation Demo" << std::endl;
    
    try {
        // Test WebSocket server
        test_websocket_server();
        
        // Test HTTP server
        test_http_server();
        
        std::cout << "All tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}