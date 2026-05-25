#include "server.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace {
namespace asio = boost::asio;
using asio::ip::tcp;

std::string trim_crlf(std::string value) {
    while (!value.empty() && (value.back() == '\n' || value.back() == '\r')) {
        value.pop_back();
    }
    return value;
}

std::string to_upper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](const unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

std::optional<long long> compute_max_from_line(const std::string& line) {
    std::istringstream iss(line);
    long long value = 0;
    long long best = 0;
    bool has_number = false;
    while (iss >> value) {
        if (!has_number) {
            best = value;
            has_number = true;
        } else {
            best = std::max(best, value);
        }
    }
    if (!has_number || !iss.eof()) {
        return std::nullopt;
    }
    return best;
}

bool parse_timer_command(const std::string& line, int& seconds) {
    std::istringstream iss(line);
    std::string keyword;
    if (!(iss >> keyword) || keyword != "timer") {
        return false;
    }
    if (!(iss >> seconds) || seconds < 0) {
        return false;
    }
    std::string tail;
    if (iss >> tail) {
        return false;
    }
    return true;
}

class AsyncSession : public std::enable_shared_from_this<AsyncSession> {
public:
    AsyncSession(tcp::socket socket, asio::io_context& io_context)
        : socket_(std::move(socket)),
          strand_(asio::make_strand(io_context)),
          timer_(io_context) {}

    void start() {
        read_line();
    }

private:
    void read_line() {
        auto self = shared_from_this();
        asio::async_read_until(socket_, input_, '\n',
            asio::bind_executor(strand_, [self](const boost::system::error_code& ec, std::size_t) {
                self->on_read(ec);
            }));
    }

    void on_read(const boost::system::error_code& ec) {
        if (ec) {
            return;
        }
        std::istream input_stream(&input_);
        std::string line;
        std::getline(input_stream, line);
        line = trim_crlf(line);

        int delay_seconds = 0;
        if (parse_timer_command(line, delay_seconds)) {
            send_timer_ready(delay_seconds);
            return;
        }

        auto self = shared_from_this();
        asio::post(strand_, [self, line] {
            const auto max_value = compute_max_from_line(line);
            std::string output;
            if (max_value.has_value()) {
                output = "Максимум: " + std::to_string(*max_value) + "\n";
            } else {
                const auto upper = to_upper(line);
                output = std::to_string(upper.size()) + ": " + upper + "\n";
            }
            self->write(output);
        });
    }

    void send_timer_ready(int seconds) {
        auto self = shared_from_this();
        const std::string ready = "Ready in " + std::to_string(seconds) + " sec\n";
        asio::async_write(socket_, asio::buffer(ready),
            asio::bind_executor(strand_, [self, seconds](const boost::system::error_code& ec, std::size_t) {
                if (ec) {
                    return;
                }
                self->timer_.expires_after(std::chrono::seconds(seconds));
                self->timer_.async_wait(asio::bind_executor(self->strand_, [self](const boost::system::error_code& timer_ec) {
                    if (timer_ec) {
                        return;
                    }
                    self->write("Done!\n");
                }));
            }));
    }

    void write(const std::string& message) {
        auto self = shared_from_this();
        auto data = std::make_shared<std::string>(message);
        asio::async_write(socket_, asio::buffer(*data),
            asio::bind_executor(strand_, [self, data](const boost::system::error_code&, std::size_t) {}));
    }

    tcp::socket socket_;
    asio::strand<asio::io_context::executor_type> strand_;
    asio::streambuf input_;
    asio::steady_timer timer_;
};

class AsyncServer {
public:
    explicit AsyncServer(unsigned short port)
        : io_context_(),
          acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)) {}

    void run(std::size_t thread_count) {
        start_accept();
        std::vector<std::thread> workers;
        workers.reserve(thread_count);
        for (std::size_t i = 0; i < thread_count; ++i) {
            workers.emplace_back([this] { io_context_.run(); });
        }
        for (auto& worker : workers) {
            worker.join();
        }
    }

private:
    void start_accept() {
        acceptor_.async_accept([this](const boost::system::error_code& ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<AsyncSession>(std::move(socket), io_context_)->start();
            }
            start_accept();
        });
    }

    asio::io_context io_context_;
    tcp::acceptor acceptor_;
};
}

void run_sync_server(unsigned short port) {
    asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        boost::asio::streambuf input;
        boost::system::error_code read_ec;
        asio::read_until(socket, input, '\n', read_ec);
        if (read_ec && read_ec != asio::error::eof) {
            continue;
        }

        std::istream input_stream(&input);
        std::string line;
        std::getline(input_stream, line);
        line = trim_crlf(line);
        const auto upper = to_upper(line);
        const std::string response = std::to_string(upper.size()) + ": " + upper + "\n";

        boost::system::error_code write_ec;
        asio::write(socket, asio::buffer(response), write_ec);
        if (write_ec) {
            continue;
        }
    }
}

void run_async_server(unsigned short port, std::size_t thread_count) {
    AsyncServer server(port);
    server.run(std::max<std::size_t>(1, thread_count));
}
