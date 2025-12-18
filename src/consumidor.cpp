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
        RingBuffer buff(10);

        cout << "[consumidor] iniciando...\n";

        MessageQueue<TagData> rxQueue("/tmp", 'B', false);

        while(gConsumidorRunning) {
            try {
                buff.push(data.to_string());
                TagData data = rxQueue.receive();
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
