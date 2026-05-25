#include "client.hpp"

#include <iostream>
#include <string>

#include <boost/asio.hpp>

namespace {
namespace asio = boost::asio;
using asio::ip::tcp;
}

void run_sync_client(const std::string& host, unsigned short port, const std::string& message) {
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::socket socket(io_context);

    auto endpoints = resolver.resolve(host, std::to_string(port));
    asio::connect(socket, endpoints);

    const std::string payload = message + "\n";
    asio::write(socket, asio::buffer(payload));

    boost::asio::streambuf response;
    asio::read_until(socket, response, '\n');
    std::istream response_stream(&response);
    std::string line;
    std::getline(response_stream, line);
    std::cout << line << '\n';
}

void run_async_client(const std::string& host, unsigned short port, const std::string& message) {
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::socket socket(io_context);

    auto endpoints = resolver.resolve(host, std::to_string(port));
    asio::connect(socket, endpoints);

    const std::string payload = message + "\n";
    asio::write(socket, asio::buffer(payload));

    boost::asio::streambuf response;
    asio::read_until(socket, response, '\n');
    std::istream response_stream(&response);
    std::string first_line;
    std::getline(response_stream, first_line);
    std::cout << first_line << '\n';

    if (first_line.rfind("Ready in ", 0) == 0) {
        boost::asio::streambuf second_response;
        asio::read_until(socket, second_response, '\n');
        std::istream second_stream(&second_response);
        std::string second_line;
        std::getline(second_stream, second_line);
        std::cout << second_line << '\n';
    }
}
