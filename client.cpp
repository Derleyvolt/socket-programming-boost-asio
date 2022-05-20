#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <vector>

#define PORT 8080

typedef unsigned char byte;

void read_file(std::vector<byte>& buf, std::string file_name) {
    std::fstream fp(file_name, std::ios::binary | std::ios::in);

    // aponta o ponteiro pro final do arquivo
    fp.seekg(0, std::ios::end);
    int len = fp.tellg(); // retorna a posição do ponteiro
    fp.seekg(0);          // traz o ponteiro pro início do arquivo

    buf.resize(len+4);

    memcpy(&buf[0], &len, sizeof(int));

    fp.read((char*)&buf[4], len);
    fp.close();
}

void menu(std::vector<byte>& buf) {
	std::cout << "Digite o nome do arquivo que deseja enviar para o servidor" << std::endl;
	std::string aux; std::cin >> aux;
	read_file(buf, aux);
}

int main(int argc, char const* argv[]) {
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(PORT);

	std::cout << "Digite o ip ao qual voce deseja se conectar" << std::endl;
	std::string server_ip; std::cin >> server_ip;

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, server_ip.c_str(), &serv_addr.sin_addr) <= 0) {
		std::cout << "Endereco invalido" << std::endl;
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		std::cout << "Conexao faliu" << std::endl;
		return -1;
	} else {
		std::cout << "Conectado com sucesso" << std::endl << std::endl;
	}

    std::vector<byte> arr;
    menu(arr);

	send(sock, (char*)arr.data(), arr.size(), 0);
    std::cout << "Arquivo enviado" << std::endl;
	close(sock);
    return 0;
}
