#include "System.hpp"
#include <atomic>
#include <csignal>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void print_exit(const char* name, int status);

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

    usleep(1000 * 10);

    pid_t pidConsumidor = fork();

    if(pidConsumidor == 0) {
        std::cout << "[app]:[consumidor] inicializado con pid: " << getpid() << std::endl;
        execlp("./consumidor", "consumidor", nullptr);
        perror("exec failed");
        exit(1);
    }

    usleep(1000 * 10);

    pid_t pidLogger = fork();

    if(pidLogger == 0) {
        std::cout << "[app]:[logger] inicializado con pid: " << getpid() << std::endl;
        execlp("./logger", "logger", nullptr);
        perror("exec failed");
        exit(1);
    }

    usleep(1000 * 10);

    while(app) { ; }

    int exit;

    std::cout << "[app] intentando finalizar el logger" << std::endl;
    kill(pidLogger, SIGUSR1);
    waitpid(pidLogger, &exit, 0);
    print_exit("logger", exit);

    std::cout << "[app] intentando finalizar el consumidor" << std::endl;
    kill(pidConsumidor, SIGUSR1);
    waitpid(pidConsumidor, &exit, 0);
    print_exit("consumidor", exit);

    std::cout << "[app] intentando finalizar el simulador" << std::endl;
    kill(pidProductor, SIGUSR1);
    waitpid(pidProductor, &exit, 0);
    print_exit("productor", exit);

    kill(pidLogger, SIGKILL);
    kill(pidProductor, SIGKILL);
    kill(pidConsumidor, SIGKILL);

    waitpid(pidLogger, nullptr, 0);
    waitpid(pidProductor, nullptr, 0);
    waitpid(pidConsumidor, nullptr, 0);

    std::cout << "[app] aplicación cerrada con éxito" << std::endl;

    return 0;
}

void print_exit(const char* name, int status) {
    if(!WIFEXITED(status)) {
        std::cout << "[app] " << name << " terminó anormalmente\n";
        return;
    }

    switch(WEXITSTATUS(status)) {
    case EXIT_OK:
    case EXIT_SHUTDOWN:
        std::cout << "[app] " << name << " terminó correctamente\n";
        break;
    case EXIT_FATAL:
        std::cout << "[app] " << name << " error fatal\n";
        break;
    default:
        std::cout << "[app] " << name << " terminó con código " << WEXITSTATUS(status) << "\n";
        break;
    }
}
