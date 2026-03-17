#include <iostream>
#include <cctype>

const int size = 10;

struct Board {
    char grid[size][size];
};

void board_init(Board &b) {
    for (int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            b.grid[i][j] = '~';
        }
    }
}

void board_display(Board &b) {
    std::cout << "  ";
    for(int i = 0; i < size; i++) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    for(int i = 0; i < size; i++) {
        std::cout << char('A' + i) << ' ';
        for(int j = 0; j < size; j++) {
            std::cout << b.grid[i][j] << ' ';
        }
        std::cout << '\n';
    }
}

bool can_place(Board &b, int x, int y, int length, char orientation) {
    for(int i = 0; i < length; i++) {
        int nx = x + (orientation == 'v' ? i : 0);
        int ny = y + (orientation == 'h' ? i : 0);
        if (nx >= size || ny >= size) { return false; }
        if (b.grid[nx][ny] != '~') { return false; }

        for(int dx = -1; dx <= 1; dx++) {
            for(int dy = -1; dy <= 1; dy++) {
                int tx = nx + dx;
                int ty = ny + dy;
                if(tx >= 0 && tx < size && ty >= 0 && ty < size)
                    if(b.grid[tx][ty] != '~')
                        return false;
            }
        }
    }
    return true;
}

void place_ship(Board &b, int x, int y, int length, char orientation) {
    for(int i = 0; i < length; i++) {
        int nx = x + (orientation == 'v' ? i : 0);
        int ny = y + (orientation == 'h' ? i : 0);
        b.grid[nx][ny] = '#';
    }
}

void ship_placement(Board &b) {
    int ships[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    for (int i = 0; i < 10; i++) {
        int x, y;
        char orientation;
        board_display(b);
        while(true) {

            char row;

            std::cout << "Placing ship of length " << ships[i] << "\n";
            std::cout << "Enter position (A-J) column (0-9) orientation(h/v): ";

            if(!(std::cin >> row >> y >> orientation)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            int x = toupper(row) - 'A';

            if(x < 0 || x >= size || y < 0 || y >= size)
                continue;

            if(orientation != 'h' && orientation != 'v')
                continue;

            if(can_place(b, x, y, ships[i], orientation)) {
                place_ship(b, x, y, ships[i], orientation);
                break;
            }
        }
    }
}

// int main() {
// Board my_board;
// board_init(my_board);
// ship_placement(my_board);
// std::cout << "\nFinal board:\n";
// board_display(my_board);
//
//     return 0;
// }
