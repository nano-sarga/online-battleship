#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

int Socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    if (res == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    return res;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = bind(sockfd, addr, addrlen);
    if (res == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void Listen(int sockfd, int backlog) {
    int res = listen(sockfd, backlog);
    if (res == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int res = accept(sockfd, addr, addrlen);
    if (res == -1) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    return res;
}



int main() {
    int server_fd = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // любой интерфейс
    address.sin_port = htons(12345);      // порт 12345

    Bind(server_fd, (struct sockaddr*)&address, sizeof(address));

    Listen(server_fd, 10);
    std::cout << "Waiting for client...\n";

    int addrlen = sizeof(address);
    int client_socket = Accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    std::cout << "Client connected!\n";

    char buffer[1024] = {0};
    while(true) {
        int valread = read(client_socket, buffer, 1024);
        if(valread <= 0) break;
        buffer[valread] = '\0';
        std::cout << "Client says: " << buffer << "\n";

        std::string reply = "Server got your message";
        send(client_socket, reply.c_str(), reply.size(), 0);
    }

    close(client_socket);
    close(server_fd);
}
