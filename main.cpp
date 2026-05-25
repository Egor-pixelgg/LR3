#include <iostream>
#include <string>

#include "client.hpp"
#include "server.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage:\n";
        std::cout << "  lr3 server-sync <port>\n";
        std::cout << "  lr3 client-sync <host> <port> <message>\n";
        std::cout << "  lr3 server-async <port> [threads]\n";
        std::cout << "  lr3 client-async <host> <port> <message>\n";
        return 1;
    }

    const std::string mode = argv[1];

    try {
        if (mode == "server-sync" && argc == 3) {
            run_sync_server(static_cast<unsigned short>(std::stoi(argv[2])));
            return 0;
        }

        if (mode == "client-sync" && argc >= 5) {
            const std::string host = argv[2];
            const auto port = static_cast<unsigned short>(std::stoi(argv[3]));
            const std::string message = argv[4];
            run_sync_client(host, port, message);
            return 0;
        }

        if (mode == "server-async" && (argc == 3 || argc == 4)) {
            const auto port = static_cast<unsigned short>(std::stoi(argv[2]));
            const std::size_t threads = argc == 4 ? static_cast<std::size_t>(std::stoul(argv[3])) : 4;
            run_async_server(port, threads);
            return 0;
        }

        if (mode == "client-async" && argc >= 5) {
            const std::string host = argv[2];
            const auto port = static_cast<unsigned short>(std::stoi(argv[3]));
            const std::string message = argv[4];
            run_async_client(host, port, message);
            return 0;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    std::cerr << "Invalid arguments\n";
    return 1;
}
