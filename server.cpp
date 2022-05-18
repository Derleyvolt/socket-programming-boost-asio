// Server side C/C++ program to demonstrate Socket
// programming
#include <iostream>
#include <string>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <fstream>
#include <sstream>

#define PORT 8080

void write_file(unsigned char* buf, int len) {
    std::ofstream fp("servidor.csv", std::ios::binary);

    fp.write((char*)buf, len);
    fp.close();
}

void client_handle(int client_fd) {
    int len;
    std::vector<unsigned char> buf(1024);
    for(;;) {
        len = recv(client_fd, buf.data(), buf.size(), NULL);

        std::cout << "tamanho do primeiro pacote recebido: " << len << std::endl;

        if(len == -1) {
            std::cout << "Algum erro ocorreu" << std::endl;
            continue;
        }

        if(len == 0) {
            return; // closed conection
        }

        write_file(buf, len);
        std::cout << "Arquivo criado com sucesso" << std::endl;
    }
}

int main(int argc, char const* argv[]) {
	int server_fd, client_fd, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = { 0 };
	char* hello = "Hello from server";

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
    // É opcional, no entanto ajuda a reusar uma porta.. evita erros do tipo: 'a porta já está em uso'
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	address.sin_family       = AF_INET;     // ipv4
	address.sin_addr.s_addr  = INADDR_ANY;  // ip local
	address.sin_port         = htons(PORT); // passando os bytes da porta pra 'network byte order' 

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

    // blocking
	if (listen(server_fd, 20) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

    std::cout << "Server iniciado" << std::endl;

    for(;;) {
        if(listen(server_fd, 20) == -1) {
            std::cout << "Erro na funcao listen" << std::endl;
            continue;
        }

        if ((client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cout << "Erro na funcao accept" << std::endl;
        }

        std::thread tclient(client_handle, client_fd);
        tclient.detach();
    }

	return 0;
}
