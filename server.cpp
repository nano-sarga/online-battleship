#include "socket.h"
#include <unistd.h>
#include <string>
#include <iostream>
#include "board.h"

struct Player {
    std::string name;
    Board board;
    int player_socket;
};

struct Game {
    Player player1;
    Player player2;
    bool player1_turn;
};

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
    std::cout << "\aPlayer1 connected!\n";

    int len1;
    Recv(player1_socket, &len1, sizeof(len1));
    char buf1[256];
    Recv(server_fd, buf1, len1);
    buf1[len1] = '\0';
    std::string player1_usr = buf1;

    int player2_socket = Accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    std::cout << "\aPlayer2 connected!\n";

    int len2;
    Recv(player2_socket, &len2, sizeof(len2));
    char buf2[256];
    Recv(server_fd, buf2, len2);
    buf2[len2] = '\0';
    std::string player2_usr = buf2;

    Game game;
    game.player1.player_socket = player1_socket;
    game.player1.name = player1_usr;
    game.player2.player_socket = player2_socket;
    game.player2.name = player2_usr;
    game.player1_turn = true; // первый ход

    std::string msg1 = "Welcome " + game.player1.name + "! Your opponent is " + game.player2.name + "\n";
    std::string msg2 = "Welcome " + game.player2.name + "! Your opponent is " + game.player1.name + "\n";

    int lenm1 = msg1.size();
    Send(game.player1.player_socket, &len1, sizeof(len1));
    Send(game.player1.player_socket, msg1.c_str(), len1);

    int lenm2 = msg2.size();
    Send(game.player2.player_socket, &len2, sizeof(len2));
    Send(game.player2.player_socket, msg2.c_str(), len2);

    close(player1_socket);
    close(player2_socket);
    close(server_fd);
}
