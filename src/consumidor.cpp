/** @file consumidor.cpp
 *
 * @brief Consumidor.
 */

#include "MessageQueue.hpp"
#include "RingBuffer.hpp"
#include "TagData.hpp"
#include "logger_thread.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

std::atomic<bool> running{true};
void signal_handler(int signum) {
    std::cout << "\n[SIGINT o SIGTERM recibida, cerrando...]\n";
    running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGPIPE, SIG_IGN); /* Ignoro el ^C pipeado desde el hilo */

    try {
        MessageQueue<TagData> queue("/tmp", 'B', true);
        RingBuffer buff(10);

        thread logger(logger_thread, std::ref(buff), std::ref(running));

        ofstream logfile("consumidor.log", std::ios::app);

        cout << "====   CONSUMIDOR DE MENSAJES   ====\n";

        string line;
        int count = 0;

        while(running) {
            try {
                TagData data = queue.receive();
                buff.push(data.to_string());
                // data.print(std::cout);
                // data.print(logfile);
            }
            catch(const std::runtime_error& e) {
                if(!running) break; /* Por las dudas verifico */
                throw;
            }
        }

        cout << "esperando a que termine el hilo\n";
        logger.join();
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "Perhaps, the producer closed the queque first.\n";
        return 1;
    }
}

/*** end of file ***/
