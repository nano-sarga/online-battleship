#include "socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "board.h"

int main() {
    int player_socket = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    Connect(player_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    std::cout << "Connected to server\n";

    std::string username;
    std::cout << "Enter your username: \n";
    std::cin >> username;

    int len = username.size();
    Send(player_socket, &len, sizeof(len));
    Send(player_socket, username.c_str(), len);

    int lenm;
    Recv(player_socket, &lenm, sizeof(lenm));
    char buf[256];
    Recv(player_socket, buf, lenm);
    buf[lenm] = '\0';
    std::cout << buf;

    Board my_board;
    board_init(my_board);
    //ship_placement(my_board);
    std::cout << "\nFinal board:\n";
    board_display(my_board);

    Send(player_socket, my_board.grid, sizeof(my_board.grid));

    close(player_socket);
}
