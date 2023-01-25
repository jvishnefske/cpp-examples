#include <asio.hpp>
#include <future>
#include <variant>
#include <queue>
class WebSocketServer{
public:
    WebSocketServer(int port,const std::string address, handler);
private:
    void handle_connection();
    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
    std::function<void(std::string_view)> m_receivedData;
    std::queue<std::shared_ptr<asio::ip::tcp::socket>> m_queue;
    asio::io_service io_service;
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
WebSocketServer::WebSocketServer(int port,const std::string address, std::function<void(std::string_view)>handler):
        m_receivedData{handler}
{
    asio::ip::tcp::resolver resolver(io_service);
    asio::ip::tcp::resolver::query query(address, std::to_string(port));
    asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
    asio::ip::tcp::endpoint endpoint = *iterator;
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
    asio::ip::tcp::socket socket_;
    std::function<void(std::string_view)> m_receivedData;
    asio::io_service io_service;
};

void WebSocketClient::handle_connection(){
    asio::async_read(socket_, asio::buffer("GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection:         upgrade\r\nSec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\nSec-WebSocket-Version: 13\r\n\r\n", 50), [this](const auto& ec, const auto& bytes_transferred){
        if(!ec){
            std::string_view data(reinterpret_cast<const char*>(bytes_transferred.data()), bytes_transferred.size());
            m_receivedData(data);
            asio::async_write(socket_, asio::buffer("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n\r\n", 50), [this](const auto& ec, const auto& bytes_transferred){
                if(!ec){
                    std::cout << "sent data" << std::endl;
                    start_accept();
                }
            });
        }
    });
}
WebSocketClient::WebSocketClient(const std::string& address, int port, std::function<void(std::string_view)> handler):
        m_receivedData{handler}
{
    asio::ip::tcp::resolver resolver(io_service);
    asio::ip::tcp::resolver::query query(address, std::to_string(port));
    asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
    asio::ip::tcp::endpoint endpoint = *iterator;
    socket_.open(endpoint.protocol());
    socket_.set_option(asio::ip::tcp::socket::reuse_address(true));
    socket_.connect(endpoint);
    handle_connection();
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
