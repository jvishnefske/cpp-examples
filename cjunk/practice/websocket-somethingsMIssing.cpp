#include "boost/asio.hpp"
#include <future>
#include <variant>
#include <queue>
#include <iostream>
#include <functional>
#include <memory>
#include <string>
namespace asio = boost::asio;
class WebSocketServer{
public:
    WebSocketServer(int port, const std::string& address, std::function<void(std::string_view)> handler);
    void start_accept();
private:
    void handle_connection();
    asio::io_service io_service_;
    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
    std::function<void(std::string_view)> m_receivedData;
    std::queue<std::shared_ptr<asio::ip::tcp::socket>> m_queue;
};

void WebSocketServer::start_accept(){
    acceptor_.async_accept(socket_, [this](const auto& ec){
        if(!ec){
            std::shared_ptr<asio::ip::tcp::socket> new_socket(new asio::ip::tcp::socket(std::move(socket_)));
            m_queue.push(new_socket);
            handle_connection();
        }
    });
}
void WebSocketServer::handle_connection(){
    if(!m_queue.empty()){
        auto socket = m_queue.front();
        m_queue.pop();
        handle_connection(*socket);
    }
}
void WebSocketServer::handle_connection(asio::ip::tcp::socket& socket){
    std::string message;
    std::cout << "new connection" << std::endl;
    asio::async_read(socket, asio::buffer("GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection:         upgrade\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Version: 13\r\n\r\n", 50), [this, &socket](const auto& ec, const auto& bytes_transferred){
        if(!ec){
            std::cout << "received data" << std::endl;
            std::string_view data(reinterpret_cast<const char*>(bytes_transferred.data()), bytes_transferred.size());
            m_receivedData(data);
            std::cout << "data received" << std::endl;
            asio::async_write(socket, asio::buffer("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n\r\n", 50), [this, &socket](const auto& ec, const auto& bytes_transferred){
                if(!ec){
                    std::cout << "sent data" << std::endl;
                    start_accept();
                }
            });
        }
    });
}
WebSocketServer::WebSocketServer(int port, const std::string& address, std::function<void(std::string_view)> handler):
        m_receivedData{handler},
        io_service_(),
        acceptor_(io_service_),
        socket_(io_service_)
{
    (void)address; // Suppress unused parameter warning
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    start_accept();
}

class WebSocketClient{
public:
    WebSocketClient(const std::string& address, int port, std::function<void(std::string_view)> handler);
private:
    void handle_connection();
    asio::io_service io_service_;
    asio::ip::tcp::socket socket_;
    std::function<void(std::string_view)> m_receivedData;
};

void WebSocketClient::handle_connection(){
    // Simplified WebSocket client - just send a basic message
    std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Version: 13\r\n\r\n";
    asio::async_write(socket_, asio::buffer(request), [this](const std::error_code& ec, std::size_t /*bytes_transferred*/){
        if(!ec){
            std::cout << "WebSocket handshake sent" << std::endl;
        }
    });
}
WebSocketClient::WebSocketClient(const std::string& address, int port, std::function<void(std::string_view)> handler):
        m_receivedData{handler},
        io_service_(),
        socket_(io_service_)
{
    try {
        asio::ip::tcp::resolver resolver(io_service_);
        auto results = resolver.resolve(address, std::to_string(port));
        asio::connect(socket_, results);
        handle_connection();
    } catch (const std::exception& e) {
        std::cout << "Connection error: " << e.what() << std::endl;
    }
}

int main(){
    WebSocketServer server(3000, "localhost", [](std::string_view data){
        std::cout << "received data: " << data << std::endl;
    });
    WebSocketClient client(3000, "localhost", [](std::string_view data){
        std::cout << "received data: " << data << std::endl;
    });
    std::cout << "server started" << std::endl;
    std::cout << "client started" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
