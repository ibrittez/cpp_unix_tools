/** @file logger_thread.cpp
 *
 * @brief Logger tcp thread
 */

#include "RingBuffer.hpp"
#include "TcpServer.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <sys/select.h>
#include <thread>
#include <unistd.h>

using namespace std;

void logger_thread(RingBuffer& ring, std::atomic<bool>& running) {
    string port = "8080";
    TcpServer server(port);

    cout << "[Logger] Listening on port " << port << " ...\n";

    // Loop principal: aceptar conexiones
    while(running) {
        int clientFd = server.accept_connection_timeout(250);
        if(clientFd == -1) { continue; }

        cout << "[Logger] Client connected\n";

        /* atiendo al cliente (post retorna <= 0 en desconexión). */
        while(running) {
            try {
                if(server.post(ring.pop(), clientFd) <= 0) break;
                if(server.post("\n", clientFd) <= 0) break;
            }
            catch(const runtime_error&) {
                this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        close(clientFd);
        cout << "[Logger] Client connection closed\n";
    }

    server.close_server();

    cout << "[Logger] Thread exiting gracefully\n";
}

/*** end of file ***/
