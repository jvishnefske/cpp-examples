#include <asio.hpp>

// bind to socket, and foward websocket requests to handler
template<class Handler>
class WebSocketServer {
public:
    WebSocketServer(asio::io_service &io_service, const std::string &address, const std::string &port, Handler &handler)
            : acceptor_(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
              socket_(io_service),
              handler_(handler)
    {
        start_accept();
    }

private:
    void start_accept()
    {
        acceptor_.async_accept(socket_,
                               [this](std::error_code ec) {
                                   if (!ec) {
                                       handler_.handle_connection(std::move(socket_));
                                       start_accept();
                                   }
                               });
    }

    asio::ip::tcp::acceptor acceptor_;
    asio::ip::tcp::socket socket_;
    Handler &handler_;
};

// handler for websocket requests
class WebSocketHandler {
public:
    WebSocketHandler(asio::io_service &io_service, const std::string &address, const std::string &port)
            : server_(io_service, address, port, *this)
    {
    }

private:
    void handle_connection(asio::ip::tcp::socket &socket)
    {
        std::cout << "new connection" << std::endl;
        asio::async_read(
                socket, asio::buffer(
                        "GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\nSec-WebSocket-Version: 13\r\n\r\n",
                        50),
                [this](std::error_code ec, std::size_t length) {
                    if (!ec) {
                        std::cout << "received request" << std::endl;
                        asio::async_write(
                                socket, asio::buffer(
                                        "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: HSmrc0sMlYUkAGmm5OPpG2HaGWk=\r\n\r\n",
                                        50),
                                [this](std::error_code ec, std::size_t length) {
                                    if (!ec) {
                                        std::cout << "sent response" << std::endl;
                                        asio::async_read(
                                                socket, asio::buffer(data_, data_.size()),
                                                [this](std::error_code ec,
                                                       std::size_t length) {
                                                    if (!ec) {
                                                        std::cout << "received data"
                                                                  << std::endl;
                                                        asio::async_write(
                                                                socket,
                                                                asio::buffer(
                                                                        data_,
                                                                        length),
                                                                [this](std::error_code ec,
                                                                       std::size_t length) {
                                                                    if (!ec) {
                                                                        std::cout
                                                                                << "sent data"
                                                                                << std::endl;
                                                                    }
                                                                });
                                                    }
                                                });
                                    }
                                });
                    }
                });
    }

    WebSocketServer<WebSocketHandler> server_;
    std::string data_ = "Hello, world!";
};

int test_websocket_server(int argc, char *argv[])
{
    try {
        asio::io_service io_service;
        WebSocketHandler handler(io_service, "localhost", "8080");
        io_service.run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

template<class ClientHandler>
class WebSocketClient {
public:
    WebSocketClient(asio::io_service &io_service, const std::string &address, const std::string &port,
                    ClientHandler &handler)
            : socket_(io_service),
              handler_(handler)
    {
        start_connect(address, port);
    }

private:
    void start_connect(const std::string &address, const std::string &port)
    {
        asio::async_connect(socket_, asio::ip::tcp::endpoint(asio::ip::address::from_string(address), std::stoi(port)),
                            [this](std::error_code ec) {
                                if (!ec) {
                                    handler_.handle_connection(std::move(socket_));
                                }
                            });
    }

    asio::ip::tcp::socket socket_;
    ClientHandler &handler_;
};

// handler for websocket requests
class WebSocketClientHandler {
public:
    WebSocketClientHandler(asio::io_service &io_service, const std::string &address, const std::string &port)
            : client_(io_service, address, port, *this)
    {
    }

private:
    void handle_connection(asio::ip::tcp::socket &socket)
    {
        std::cout << "connected" << std::endl;
        asio::async_write(socket, asio::buffer(
                                  "GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\nSec-WebSocket-Version: 13\r\n\r\n",
                                  50),
                          [this](std::error_code ec, std::size_t length) {
                              if (!ec) {
                                  std::cout << "sent request" << std::endl;
                                  asio::async_read(socket, asio::buffer(data_, data_.size()),
                                                   [this](std::error_code ec, std::size_t length) {
                                                       if (!ec) {
                                                           std::cout << "received response" << std::endl;
                                                           asio::async_write(socket, asio::buffer(data_, length),
                                                                             [this](std::error_code ec,
                                                                                    std::size_t length) {
                                                                                 if (!ec) {
                                                                                     std::cout << "sent data"
                                                                                               << std::endl;
                                                                                 }
                                                                             });
                                                       }
                                                   });
                              }
                          });
    }

    WebSocketClient<WebSocketClientHandler> client_;
    std::string data_ = "Hello, world!";
};

int test_websocket_client()
{
    try {
        asio::io_service io_service;
        WebSocketClientHandler handler(io_service, "localhost", "8080");
        io_service.run();
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

int main(int argc, char *argv[])
{
    test_websocket_server();
    test_websocket_client(argc, argv);
    return 0;
}