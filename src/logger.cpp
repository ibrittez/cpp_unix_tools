/** @file logger.cpp
 *
 * @brief Logger
 */

#include "Logger.hpp"
#include "LogMessage.hpp"
#include "MessageQueue.hpp"
#include "System.hpp"
#include "TagData.hpp"
#include "TcpServer.hpp"
#include <atomic>
#include <csignal>
#include <string>
#include <thread>

using namespace std;

void tcp_server_thread(TcpSink& sink, std::string port, std::atomic<bool>& running) {
    TcpServer server(port);

    cout << "[logger][tcp-sink:" + port + "] escuchando conexiones" << endl;
    while(running) {
        int fd = server.accept_connection_timeout(250);
        if(fd >= 0) {
            cout << "[logger][tcp-sink:" + port + "] nuevo cliente conectado" << endl;
            sink.add_client(fd);
        }
    }

    cout << "[logger][tcp-sink:" + port + "] cerrando servidor tcp...\n";
    server.close_server();
}

std::atomic<bool> gLoggerRunning{true};
static void signal_handler(int signum) {
    gLoggerRunning = false;
}

int main() {
    std::signal(SIGUSR1, signal_handler);
    std::signal(SIGINT, SIG_IGN);
    std::signal(SIGTERM, SIG_IGN);
    std::signal(SIGPIPE, SIG_IGN);

    try {
        cout << "[logger] inicializando sink...\n";
        MessageQueue<LogMessage> logQueue("/tmp", 'L', false);

        Logger logger;

        StdoutSink stdoutSink;
        TcpSink tcpSink8080;
        TcpSink tcpSink8090;
        FileSink fileSink("/tmp/rfid_log.log");

        // logger.add_sink(&stdoutSink);
        logger.add_sink(&tcpSink8080);
        logger.add_sink(&tcpSink8090);
        logger.add_sink(&fileSink);

        std::thread tcpThread8080(tcp_server_thread, std::ref(tcpSink8080), "8080", std::ref(gLoggerRunning));
        std::thread tcpThread8090(tcp_server_thread, std::ref(tcpSink8090), "8090", std::ref(gLoggerRunning));

        cout << "[logger] comenzando a loggear...\n";

        while(gLoggerRunning) {
            try {
                LogMessage msg = logQueue.receive();
                logger.log(msg);
            }
            catch(const std::runtime_error& e) {
                if(!gLoggerRunning) break; /* Por las dudas verifico */
                throw;
            }
        }

        cout << "[logger] cerrando logger...\n";
        gLoggerRunning = false;
        tcpThread8080.join();
        tcpThread8090.join();
        cout << "[logger] cerrado con éxito" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << "[logger] Error: " << e.what() << "\n";
        std::cerr << "[logger] Perhaps, the producer closed the queque first.\n";
        return 1;
    }

    return EXIT_SHUTDOWN;
}

/*** end of file ***/
