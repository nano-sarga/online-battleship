#include "socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>


int main() {
    int player_socket = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    Connect(player_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    std::cout << "Connected to server\n";


    close(player_socket);
}
