//#define ASIO_STANDLONE
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <mutex>

// https://www.facebook.com/reel/1175735499930199

using namespace std;

using namespace boost::asio;
using ip::tcp;

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;

#define PORT 80

bool message_error(boost::system::error_code ec) {
    if(ec.failed()) {
        cout << ec << endl;
        return 1;
    }
}

template<typename container>
void send_all(ip::tcp::socket& fd, container& buf) {
    uint sent_bytes = 0;

    // until send all bytes
    while(sent_bytes < buf.size()) {
        sent_bytes += fd.send(buffer(buf));
    }
}

void client(ip::tcp::socket& fd) {
    for(;;) {
        cout << "Digite uma mensagem" << endl;
        string buf; cin >> buf;
        uint sent_bytes = 0;

        send_all(fd, buf);
    }
}

int main() {
    boost::system::error_code ec;
    io_context context;

    ip::tcp::endpoint server_addr(ip::make_address("127.0.0.1", ec), PORT);

    ip::tcp::socket socket(context);

    socket.connect(server_addr, ec);

    if(not message_error(ec)) {
        cout << "Conectado" << endl;
    }

    thread t_handle(client, socket);
    t_handle.join();
    return 0;
}
