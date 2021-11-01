#include <regex>
#include <future>
#include <iostream>
#include <fstream>
#include <sstream>
//include json::Value header
#include "json/json.h"
#include <asio.hpp>


Json::Value parse_json(const std::string& json_string)
{
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(json_string, root);
    if (!parsingSuccessful)
    {
        throw std::runtime_error("Failed to parse JSON: " + reader.getFormattedErrorMessages());
    }
    return root;
}
// http client
class http_client
{
    public:
    http_client(asio::io_context& io_context, const std::string& host, const std::string& port)
        : resolver_(io_context), socket_(io_context)
    {
        asio::ip::tcp::resolver::query query(host, port);
        resolver_.async_resolve(query, [this](const asio::error_code& ec, asio::ip::tcp::resolver::iterator endpoint_iterator)
        {
            if (!ec)
            {
                asio::async_connect(socket_, endpoint_iterator, [this](const asio::error_code& ec, asio::ip::tcp::resolver::iterator)
                {
                    if (!ec)
                    {
                        do_read();
                    }
                });
            }
        });
    }

    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_, max_length),
            [this, self](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    std::cout.write(data_, length);
                    do_read();
                }
            });
    }

};