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
    Player p1;
    Player p2;
    bool p1_turn;
};

bool safe_recv(int sock, void* buf, size_t len) {
    ssize_t r = Recv(sock, buf, len);
    return r > 0;
}

int main() {
    int server_fd = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    Bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    Listen(server_fd, 10);
    std::cout << "Waiting for players...\n";

    Player p1, p2;
    int addrlen = sizeof(address);
    p1.player_socket = Accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    std::cout << "\aPlayer1 connected!\n";

    int len1;
    if (!safe_recv(p1.player_socket, &len1, sizeof(len1))) { return 0; }
    if (len1 > 255) len1 = 255;

    char buf1[256];
    if (!safe_recv(p1.player_socket, buf1, len1)) { return 0; }
    buf1[len1] = '\0';
    p1.name = buf1;

    p2.player_socket = Accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    std::cout << "\aPlayer2 connected!\n";

    int len2;
    if (!safe_recv(p2.player_socket, &len2, sizeof(len2))) { return 0; }
    if (len2 > 255) len2 = 255;

    char buf2[256];
    if (!safe_recv(p2.player_socket, buf2, len2)) { return 0; }
    buf2[len2] = '\0';
    p2.name = buf2;

    std::cout << "[SERVER] Players: " << p1.name << " vs " << p2.name << "\n";

    Game game;
    game.p1 = p1;
    game.p2 = p2;
    game.p1_turn = true; // первый ход

    std::string msg1 = "Welcome " + game.p1.name + "! Your opponent is " + game.p2.name + "\n";
    std::string msg2 = "Welcome " + game.p2.name + "! Your opponent is " + game.p1.name + "\n";

    int l1 = msg1.size();
    Send(game.p1.player_socket, &l1, sizeof(l1));
    Send(game.p1.player_socket, msg1.c_str(), l1);

    int l2 = msg2.size();
    Send(game.p2.player_socket, &l2, sizeof(l2));
    Send(game.p2.player_socket, msg2.c_str(), l2);

    if (!safe_recv(p1.player_socket, game.p1.board.grid, sizeof(game.p1.board.grid))) { return 0; }
    if (!safe_recv(p2.player_socket, game.p2.board.grid, sizeof(game.p2.board.grid))) { return 0; }

    std::cout << "Boards received\n";

    bool player1_turn = true;

    while (true) {
        Player &attacker = player1_turn ? game.p1 : game.p2;
        Player &defender = player1_turn ? game.p2 : game.p1;

        int attacker_sock = attacker.player_socket;
        int defender_sock = defender.player_socket;

        Send(attacker_sock, "Your turn\n", 10);
        Send(defender_sock, "Wait for opponent...\n", 21);

        char shot[2];
        if (!safe_recv(attacker_sock, shot, sizeof(shot))) {
            std::cout << attacker.name << " disconnected\n";
            break;
        }

        char row = shot[0];
        int col = shot[1] - '0';

        std::cout << attacker.name << " shoots " << row << col << "\n";

        bool hit = shoot(defender.board, row, col);
        std::string result = hit ? "Hit!\n" : "Miss!\n";

        Send(attacker_sock, result.c_str(), result.size());
        Send(defender_sock, result.c_str(), result.size());

        if (win(defender.board)) {
            Send(attacker_sock, "You win!\n", 9);
            Send(defender_sock, "You lose!\n", 10);
            std::cout << attacker.name << " wins!\n";
            break;
        }

        player1_turn = !player1_turn;
    }

    close(p1.player_socket);
    close(p2.player_socket);
    close(server_fd);
}
