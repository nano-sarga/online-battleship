#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

int Socket(int domain, int type, int protocol) {
    int res = socket(domain, type, protocol);
    if (res == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    return res;
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    int res = connect(sockfd, addr, addrlen);
    if (res == -1) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
}

ssize_t Send(int sockfd, const void *buf, size_t len, int flags) {
    ssize_t res = send(sockfd, buf, len, flags);
    if (res == -1) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

ssize_t Recv(int sockfd, void *buf, size_t len, int flags) {
    ssize_t res = recv(sockfd, buf, len, flags);
    if (res == -1) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

int main() {
    int sock = Socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);

    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    Connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    std::cout << "Connected to server\n";
    char buffer[1024];

    while(true) {
        std::string msg;
        std::getline(std::cin, msg);
        Send(sock, msg.c_str(), msg.size(), 0);
        int bytes = Recv(sock, buffer, sizeof(buffer)-1, 0);
        if(bytes <= 0) {
            break;
        }
        buffer[bytes] = '\0';
        std::cout << "Server: " << buffer << std::endl;
    }

    close(sock);
}