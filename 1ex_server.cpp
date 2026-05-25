#include <iostream>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

void 1ex_server() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));

        std::cout << "Ñåðâåð çàïóùåí. Îæèäàíèå ïîäêëþ÷åíèé..." << std::endl;

        tcp::socket socket(io_context);
        acceptor.accept(socket);

        std::cout << "Êëèåíò ïîäêëþ÷åí." << std::endl;

        char data[1024];
        size_t length = socket.read_some(boost::asio::buffer(data));
        std::string client_message(data, length);

        std::string response = "Ñîîáùåíèå ïîëó÷åíî: " + client_message + "\n";
        boost::asio::write(socket, boost::asio::buffer(response));

        std::cout << "Îòâåò îòïðàâëåí." << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "Îøèáêà ñåðâåðà: " << e.what() << std::endl;
    }
}
