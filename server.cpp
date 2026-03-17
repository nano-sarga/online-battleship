#include "socket.h"
#include <unistd.h>
#include <string>
#include <iostream>


int main() {
    int server_fd = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);

    Bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    Listen(server_fd, 10);
    std::cout << "Waiting for players...\n";

    int addrlen = sizeof(address);
    int player1_socket = Accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    std::cout << "Player1 connected!\n";

    int player2_socket = Accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    std::cout << "Player2 connected!\n";


    close(player1_socket);
    close(player2_socket);
    close(server_fd);
}
