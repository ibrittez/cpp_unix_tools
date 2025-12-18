/** @file consumidor.cpp
 *
 * @brief Consumidor.
 */

#include "LogMessage.hpp"
#include "MessageQueue.hpp"
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
        cout << "[consumidor] iniciando...\n";

        MessageQueue<LogMessage> logQueue("/tmp", 'L', true);
        MessageQueue<TagData> rxQueue("/tmp", 'B', false);

        while(gConsumidorRunning) {
            try {
                TagData data = rxQueue.receive();
                LogMessage msg{LogMsgType::TAG_DATA, data};
                logQueue.send(msg);
            }
            catch(const std::runtime_error& e) {
                if(!gConsumidorRunning) break; /* Por las dudas verifico */
                throw;
            }
        }

        cout << "[consumidor] eliminado cola de mensajes" << std::endl;
        logQueue.remove();
        cout << "[consumidor] cerrado con éxito" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "[consumidor] Error: " << e.what() << "\n";
        std::cerr << "[consumidor] Perhaps, the producer closed the queque first.\n";
        return 1;
    }
}

/*** end of file ***/
