/** @file productor.cpp
 *
 * @brief Productor
 */

#include "MessageQueue.hpp"
#include "System.hpp"
#include "TagData.hpp"
#include <atomic>
#include <csignal>
#include <iostream>
#include <random>
#include <string>
#include <unistd.h>

using namespace std;

std::atomic<bool> gProductorRunning{true};
static void signal_handler(int signum) {
    gProductorRunning = false;
}

int main() {
    std::signal(SIGUSR1, signal_handler);
    std::signal(SIGINT, SIG_IGN);
    std::signal(SIGTERM, SIG_IGN);

    try {
        MessageQueue<TagData> queue("/tmp", 'B', true);

        cout << "[simulador] iniciando...\n";

        string line;

        // Setup generadores de números aleatorios
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rssi_dist(-80, -40); // RSSI típico
        std::uniform_real_distribution<> temp_dist(15.0, 30.0); // Temperatura ambiente

        cout << "[simulador] generando datos aleatorios\n";
        for(int i = 0; gProductorRunning; ++i) {
            TagData tag;

            // Generar EPC realista (manufacturer E282 = Axzon, model 450 = Opus)
            snprintf(tag.epc_id, sizeof(tag.epc_id), "E282450%012d", 100000 + i);

            tag.rssi = rssi_dist(gen);
            tag.temperature = temp_dist(gen);
            tag.timestamp = time(nullptr);

            queue.send(tag);
            usleep(1000 * 100);
        }

        cout << "[simulador] eliminado cola de mensajes" << std::endl;
        queue.remove();
        cout << "[simulador] cerrado con éxito" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return EXIT_SHUTDOWN;
}

/*** end of file ***/
