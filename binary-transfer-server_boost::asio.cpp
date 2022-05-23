//#define ASIO_STANDLONE
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <sstream>
#include <fstream>
#include <mutex>

using namespace std;

using namespace boost::asio;
using ip::tcp;

typedef unsigned char  uint_8;
typedef unsigned short uint_16;
typedef unsigned int   uint_32;

#define PORT 1649
#define CLIENT_QUEUE 20

int client_index;

bool message_error(boost::system::error_code ec) {
    if(ec.failed()) {
        cout << ec << endl;
        return 1;
    }

    return 0;
}

void write_file(vector<uint_8>& data) {
    ofstream fp("binary_file", ios::binary);

    fp.write((char*)&data[0], data.size());
    fp.close();
}

uint_32 recv_append(std::vector<uint_8>& buf, ip::tcp::socket& fd) {
    std::array<uint_8, 1024> buf_temp;
    int len_recv = fd.receive(boost::asio::buffer(buf_temp, 1024));

    if(len_recv > 0) {
        copy(buf_temp.begin(), buf_temp.begin()+len_recv, back_inserter(buf));
    }

    return len_recv;
}

std::vector<uint_8> get_current_packet(std::vector<uint_8>& buf, ip::tcp::socket& fd) {
    uint_32 len_recv, len_data;

    if((len_recv = recv_append(buf, fd)) == 0) {
        return vector<uint_8>();
    }

    memcpy(&len_data, &buf[0], sizeof(int));

    while(len_recv-sizeof(int) < len_data) {
        len_recv += recv_append(buf, fd);
    }

    vector<uint_8> payload(buf.begin()+sizeof(int), buf.begin()+len_data);
    buf.erase(buf.begin(), buf.begin()+len_data+sizeof(int));
    return payload;
}

void handle_client(ip::tcp::socket& fd, int id) {
    vector<uint_8> buf;

    for(;;) {
        auto packet = get_current_packet(buf, fd);

        if(packet.empty()) {
            cout << "O Cliente " << id << " se desconectou" << endl;
            fd.close();
            return;
        }

        write_file(packet);
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
    acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));

    if(not message_error(ec)) {
        cout << "Servidor iniciado com sucesso" << endl;
    }

    // lembrar de limpar as conexões que estão fechadas e reorganizar o array.. depois.
    vector<ip::tcp::socket> client_fd;

    for(;;) {
        // bloqueia até receber clientes
        client_fd.push_back(acceptor_.accept(ec));
        message_error(ec);
        cout << "O cliente " << client_index << " se conectou" << endl;
        thread t_client(handle_client, ref(client_fd.back()), client_index);
        client_index++;
        t_client.detach();
    }

    return 0;
}
