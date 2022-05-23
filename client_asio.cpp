//#define ASIO_STANDLONE
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <mutex>

using namespace std;

using namespace boost::asio;
using ip::tcp;

typedef unsigned char  uint_8;
typedef unsigned short uint_16;
typedef unsigned int   uint_32;

#define PORT 1649

bool message_error(boost::system::error_code ec) {
    if(ec.failed()) {
        cout << ec << endl;
        return 1;
    }

    return 0;
}

int read_file(std::vector<uint_8>& data) {
    cout << "Digite o nome do arquivo a ser enviado" << endl;
    string s; cin >> s;


    ifstream fp(s, ios::binary);

    if(not fp.is_open()) {
        cout << "Algo errado ao tentar abrir o arquivo " << s << endl;
        this_thread::sleep_for(std::chrono::seconds(2));
        return -1;
    }

    int len = 0;

    if(fp.is_open()) {
        fp.seekg(0, ios::end);
        len = fp.tellg();
        fp.seekg(0);
    }

    cout << "tamanho: " << len << endl;

    data.resize(len+sizeof(int));

    memcpy(&data[0], &len, sizeof(int));

    fp.read((char*)&data[4], len);

    return 1;
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
    vector<uint_8> data;

    for(;;) {
        while(read_file(data) == -1);
        send_all(fd, data);
    }
}

int main() {
    boost::system::error_code ec;
    io_context context;

    ip::tcp::endpoint server_addr(ip::make_address("127.0.0.1", ec), PORT);

    //acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

    ip::tcp::socket socket(context);

    socket.connect(server_addr, ec);

    if(not message_error(ec)) {
        cout << "Conectado" << endl;
    }

    client(socket);
    return 0;
}
