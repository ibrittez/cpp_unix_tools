/** @file consumidor.cpp
 *
 * @brief Consumidor.
 */

#include "MessageQueue.hpp"
#include "RingBuffer.hpp"
#include "TagData.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

using namespace std;

std::atomic<bool> gConsumidorRunning{true};
static void signal_handler(int signum) {
    gConsumidorRunning = false;
}

int main() {
    std::signal(SIGUSR1, signal_handler);
    std::signal(SIGINT, SIG_IGN);
    std::signal(SIGTERM, SIG_IGN);

    try {
        MessageQueue<TagData> queue("/tmp", 'B', true);
        RingBuffer buff(10);

        ofstream logfile("consumidor.log", std::ios::app);

        cout << "[consumidor] iniciando...\n";

        string line;
        int count = 0;

        while(gConsumidorRunning) {
            try {
                TagData data = queue.receive();
                buff.push(data.to_string());
                // data.print(std::cout);
                // data.print(logfile);
            }
            catch(const std::runtime_error& e) {
                if(!gConsumidorRunning) break; /* Por las dudas verifico */
                throw;
            }
        }

        cout << "[consumidor] cerrado con éxito" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "[consumidor] Error: " << e.what() << "\n";
        std::cerr << "[consumidor] Perhaps, the producer closed the queque first.\n";
        return 1;
    }
}

/*** end of file ***/
