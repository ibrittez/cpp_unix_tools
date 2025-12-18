/** @file TcpServer.hpp
 *
 * @brief TCP server
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <atomic>
#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

class TcpServer {
public:
    TcpServer(string port) {
        struct addrinfo hints, *server;
        int r;

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // Para bind a cualquier interfaz

        r = getaddrinfo(nullptr, "8080", &hints, &server);
        if(r != 0) {
            cerr << "[Server] getaddrinfo error: " << gai_strerror(r) << "\n";
            return;
        }

        serverFd_ = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
        if(serverFd_ == -1) {
            perror("[Server] socket");
            freeaddrinfo(server);
            return;
        }

        /* permito que se reuse la dirección */
        int opt = 1;
        setsockopt(serverFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        /* bindeo */
        r = bind(serverFd_, server->ai_addr, server->ai_addrlen);
        if(r == -1) {
            perror("[Server] bind");
            close(serverFd_);
            freeaddrinfo(server);
            serverFd_ = -1;
            return;
        }

        /* Listen */
        r = listen(serverFd_, 5);
        if(r == -1) {
            perror("[Server] listen");
            close(serverFd_);
            freeaddrinfo(server);
            serverFd_ = -1;
            return;
        }

        freeaddrinfo(server);
    }

    int accept_connection(void) {
        sockaddr_storage clientAddr{};
        socklen_t len = sizeof(clientAddr);

        int clientFd = accept(serverFd_, (sockaddr*)&clientAddr, &len);
        return clientFd;
    }

    int accept_connection_timeout(int timeout_ms) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(serverFd_, &rfds);

        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;

        int r = select(serverFd_ + 1, &rfds, nullptr, nullptr, &tv);

        if(r <= 0) {
            return -1; // timeout o EINTR
        }

        sockaddr_storage clientAddr{};
        socklen_t len = sizeof(clientAddr);
        return accept(serverFd_, (sockaddr*)&clientAddr, &len);
    }

    int post(string line, int clientFd) {
        ssize_t n = write(clientFd, line.data(), line.size());
        return n;
    }

    void close_server(void) {
        if(serverFd_ != -1) {
            close(serverFd_);
            serverFd_ = -1;
        }
    }
private:
    int serverFd_;
};

#endif /* TCPSERVER_H */

/*** end of file ***/
