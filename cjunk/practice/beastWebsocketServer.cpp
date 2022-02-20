// data distribution service middleware
#include <functional>
#include <variant>
#include <future>
#include <iostream>
#include <boost/asio.hpp>
// http server
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/http/fields.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/http/fields.hpp>
class Server {
public:
    Server(boost::asio::io_context& ioc,
           std::string const& host,
           std::string const& port)
        : acceptor_(ioc)
        , socket_(ioc)
        , host_(host)
        , port_(port)
    {
        boost::system::error_code ec;
        acceptor_.open(boost::asio::ip::tcp::v4(), ec);
        if (ec)
        {
            throw boost::system::system_error(ec);
        }
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_), ec);
        if (ec)
        {
            throw boost::system::system_error(ec);
        }
        acceptor_.listen(boost::asio::socket_base::max_connections, ec);
        if (ec)
        {
            throw boost::system::system_error(ec);
        }
    }
    void run()
    {
        acceptor_.async_accept(socket_,
                               std::bind(&Server::handle_accept, this, std::placeholders::_1));
    }
    void handle_accept(boost::system::error_code ec)
    {
        if (!ec)
        {
            std::make_shared<Session>(std::move(socket_))->start();
        }
        socket_.close();
        acceptor_.async_accept(socket_,
                               std::bind(&Server::handle_accept, this, std::placeholders::_1));
    }
private:
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    std::string host_;
    std::string port_;
};
class Session {
public:
    Session(boost::asio::ip::tcp::socket socket)
        : socket_(std::move(socket))
    {
    }
    void start()
    {
        do_read();
    }
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                std::bind(&Session::handle_read, this, std::placeholders::_1, std::placeholders::_2));
    }
    void handle_read(boost::system::error_code ec, std::size_t length)
    {
        if (!ec)
        {
            std::cout << "Received: " << std::string(data_, length) << std::endl;
            do_write(data_, length);
        }
        else
        {
            do_read();
        }
    }
    void do_write(const char* data, std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data, length),
                                 std::bind(&Session::handle_write, this, std::placeholders::_1));
    }
    void handle_write(boost::system::error_code ec)
    {
        if (!ec)
        {
            do_read();
        }
    }
private:
    boost::asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};
int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: websocket_echo_server <address> <port>\n";
            return 1;
        }
        boost::asio::io_context ioc;
        Server s(ioc, argv[1], argv[2]);
        s.run();
        ioc.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

