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

typedef unsigned char byte;
typedef unsigned int  uint;

void write_file(std::vector<byte> buf) {
    std::ofstream fp("servidor.csv", std::ios::binary);

    fp.write((char*)&buf[0], buf.size());
    fp.close();
}

// EXTRAI LEN_DATA BYTES DE SRC
std::vector<byte> extract_data(std::vector<byte>& src, int len_data) {
    std::vector<byte> ret(src.begin()+sizeof(int), src.begin()+sizeof(int)+len_data);
    src.erase(src.begin(), src.begin()+sizeof(int)+len_data);
    return ret;
}

// EMPURRA OS DADOS EM BUF
uint recv_(int fd, std::vector<byte>& buf) {
    byte aux[1024];
    int len = recv(fd, aux, sizeof(aux), NULL);
    for(int i = 0; i < len; i++) buf.push_back(aux[i]);
    return len;
}

// QUEBRA OS PACOTES RECEBIDOS (QUE PODEM SER PEDAÇOS DE UM COMANDO/MENSAGEM) E OS JUNTA CERTINHO
// NO FINAL.. E DISPACHA.
void client_handle(int fd) {
    for(;;) {
        std::vector<byte> buf, data;
        uint len = recv_(fd, buf);

        if(len == 0) {
            break;
        }

        int len_data; memcpy(&len_data, &buf[0], sizeof(int));

        while(len-sizeof(int) < len_data) {
            len += recv_(fd, buf);
        }
		
		// 
        data = extract_data(buf, len_data);

        write_file(data);
        std::cout << "len data: "  << data.size() << std::endl;
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
	
	// ACEITA REQUISIÇÕES DE CLIENTES
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
