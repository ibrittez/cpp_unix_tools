#include <atomic>
#include <csignal>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

std::atomic<bool> app{true};
void sigint_handler(int signum) {
    app = false;
}

int main(void) {
    std::signal(SIGINT, sigint_handler);
    std::signal(SIGTERM, sigint_handler);
    std::signal(SIGPIPE, SIG_IGN); /* Ignoro el ^C pipeado desde el hilo */

    pid_t pidProductor = fork();

    if(pidProductor == 0) {
        std::cout << "[app]:[simulador] inicializado con pid: " << getpid() << std::endl;
        execlp("./productor", "productor", nullptr);
        perror("exec failed");
        exit(1);
    }

    pid_t pidConsumidor = fork();

    if(pidConsumidor == 0) {
        std::cout << "[app]:[consumidor] inicializado con pid: " << getpid() << std::endl;
        execlp("./consumidor", "consumidor", nullptr);
        perror("exec failed");
        exit(1);
    }

    pid_t pidLogger = fork();

    if(pidLogger == 0) {
        std::cout << "[app]:[logger] inicializado con pid: " << getpid() << std::endl;
        execlp("./logger", "logger", nullptr);
        perror("exec failed");
        exit(1);
    }
    while(app) { ; }

    std::cout << "[app] intentando finalizar el logger" << std::endl;
    kill(pidLogger, SIGUSR1);
    std::cout << "[app] intentando finalizar el consumidor" << std::endl;
    kill(pidConsumidor, SIGUSR1);

    sleep(1);

    std::cout << "[app] intentando finalizar el simulador" << std::endl;
    kill(pidProductor, SIGUSR1);

    sleep(1);


    // si siguen vivos, a la mierda
    kill(pidLogger, SIGKILL);
    kill(pidProductor, SIGKILL);
    kill(pidConsumidor, SIGKILL);

    waitpid(pidLogger, nullptr, 0);
    waitpid(pidProductor, nullptr, 0);
    waitpid(pidConsumidor, nullptr, 0);

    std::cout << "[app] aplicación cerrada con éxito" << std::endl;

    return 0;
}
