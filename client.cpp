// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <vector>

#define PORT 8080

void read_file(std::vector<unsigned char>& buf) {
    std::ifstream fp("saida.csv", std::ios::binary);
    //std::ofstream fp_out("teste.csv", std::ios::binary);

    // aponta o ponteiro pro final do arquivo
    fp.seekg(0, std::ios::end);
    int len = fp.tellg(); // retorna a posição do ponteiro
    fp.seekg(0);          // traz o ponteiro pro início do arquivo

    buf.resize(len+4);

    memcpy(&buf[0], &len, sizeof(int));
    std::cout << *(int*)&buf[0] << std::endl;

    fp.read((char*)&buf[4], len);
    //fp_out.write((char*)buf.data(), len);

    fp.close();
    //fp_out.close();

}

int main(int argc, char const* argv[]) {
    std::vector<unsigned char> arrs;
    read_file(arrs);

    return 0;

	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	char buffer[1024] = { 0 };

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port   = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

    std::vector<unsigned char> arr(4);

    read_file(arr);

	send(sock, arr.data(), arr.size(), 0);
    std::cout << "Arquivo enviado" << std::endl;
	return 0;
}
