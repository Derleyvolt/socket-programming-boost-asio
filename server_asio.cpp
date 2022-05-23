//#define ASIO_STANDLONE
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

using namespace boost::asio;
using ip::tcp;

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;

#define PORT 80
#define CLIENT_QUEUE 20

bool message_error(boost::system::error_code ec) {
    if(ec.failed()) {
        cout << ec << endl;
        return 1;
    }
}

vector<unsigned char> recv_entire_packet(ip::tcp::socket& fd) {

}

void handle_client(io_context& context, ip::tcp::socket& fd) {
    vector<int> arr = {1, 2, 3, 4, 5};
    //boost::asio::streambuf* buf = arr.data();

    //std::streambuf *at = arr.data();

    for(;;) {
        int len = fd.read_some(arr);
    }
}

int main() {
    boost::system::error_code ec;
    io_context context;

    // tcp::acceptor acceptor_(context, ip::tcp::endpoint(ip::tcp::v4(), PORT));
    // é equivalente ao abaixo

    ip::tcp::endpoint endpoint(ip::tcp::v4(), PORT);

    tcp::acceptor acceptor_(context);
    
    acceptor_.open(endpoint.protocol(), ec);
    acceptor_.bind(endpoint, ec);
    acceptor_.listen(CLIENT_QUEUE, ec);

    if(not message_error(ec)) {
        cout << "Servidor iniciado com sucesso" << endl;
    }

    ip::tcp::socket socket(context);

    for(;;) {
        // block wainting for clients
        acceptor_.accept(socket, ec); 
        message_error(ec);
        thread t_client(handle_client, context, socket);
        t_client.detach();
    }

    return 0;
}
