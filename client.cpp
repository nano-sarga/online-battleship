#include "socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include "board.h"

int main() {
    int sock = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    Connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    std::cout << "Connected to server\n";

    std::string username;
    std::cout << "Enter your username: \n";
    std::cin >> username;

    int len = username.size();
    Send(sock, &len, sizeof(len));
    Send(sock, username.c_str(), len);

    int lenm;
    Recv(sock, &lenm, sizeof(lenm));

    char buf[256];
    if (lenm > 255) lenm = 255;
    Recv(sock, buf, lenm);
    buf[lenm] = '\0';
    std::cout << buf;

    Board my_board, enemy_board;
    board_init(my_board);
    board_init(enemy_board);
    random_placement(my_board);
    //ship_placement(my_board);
    std::cout << "\nFinal board:\n";
    board_display(my_board);

    Send(sock, my_board.grid, sizeof(my_board.grid));

    while (true) {
        char msg[64];
        int r = Recv(sock, msg, sizeof(msg));
        if (r <= 0) break;

        if (r < 64) msg[r] = '\0';
        else msg[63] = '\0';

        std::cout << msg;

        std::string m(msg);

        if (m.find("Your turn") != std::string::npos) {
            char row;
            int col;

            std::cout << "Enter shot (A-J 0-9): ";
            std::cin >> row >> col;

            char shot[2];
            shot[0] = row;
            shot[1] = char('0' + col);

            Send(sock, shot, sizeof(shot));

            r = Recv(sock, msg, sizeof(msg));
            if (r <= 0) break;
            std::string result(msg, r);
            std::cout << result;

            int x = toupper(row) - 'A';
            int y = col;
            if (result.find("Hit") != std::string::npos)
                enemy_board.grid[x][y] = 'x';
            else
                enemy_board.grid[x][y] = 'o';

            std::cout << "\nEnemy grid:\n";
            enemy_board_display(enemy_board);
        }
    }

    close(sock);
}
