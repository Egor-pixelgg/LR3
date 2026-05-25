#include <iostream>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

void 1ex_client() {
    try {
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        std::cout << "Ïîäêëþ÷åíî ê ñåðâåðó." << std::endl;
        std::cout << "Ââåäèòå ñîîáùåíèå äëÿ ñåðâåðà: ";
        std::string message;
        std::getline(std::cin, message);

        message += "\n";
        boost::asio::write(socket, boost::asio::buffer(message));

        char response[1024];
        size_t response_length = socket.read_some(boost::asio::buffer(response));

        std::cout << "Îòâåò îò ñåðâåðà: ";
        std::cout.write(response, response_length);
        std::cout << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Îøèáêà êëèåíòà: " << e.what() << std::endl;
    }
}
