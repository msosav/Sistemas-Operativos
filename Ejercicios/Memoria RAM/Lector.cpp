#include <iostream>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <ctime>

#define SHM_SIZE 1024

volatile sig_atomic_t stop = 0;

void sigint_handler(int signum) {
    stop = 1;
}

void *conectar_memoria_compartida(int id_memoriaCompartida) {
    void *memoria_compartida = shmat(id_memoriaCompartida, nullptr, 0);
    if (memoria_compartida == reinterpret_cast<void *>(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return memoria_compartida;
}

void desconectar_memoria_compartida(void *memoria_compartida) {
    if (shmdt(memoria_compartida) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void *lectorTeclado(void *arg) {
    char ch;
    while (true) {
        std::cout << "Presiona 'x' para terminar! \n";
        std::cout.flush();  //presenta el texto de inmediato

        ch = getchar();
        if (ch == 'x' || ch == 'X') {
            kill(getpid(), SIGINT); // Enviar una señal SIGINT para terminar la ejecución (es una interrupción)
            break;
        }
    }

    pthread_exit(nullptr);
}

void lectorMemoria(int id_memoriaCompartida) {
    char *memoria_compartida = static_cast<char *>(conectar_memoria_compartida(id_memoriaCompartida));

    signal(SIGINT, sigint_handler);  //interrupción para atender el teclado
    
    //despuerta un hilo de ejecución para atender el teclado
    pthread_t thread;
    pthread_create(&thread, nullptr, lectorTeclado, nullptr);

    char valor_leido_anterior[SHM_SIZE] = ""; // Almacena el último valor leído

    while (!stop) {
        // Leer datos desde la memoria compartida
        time_t rawtime;
        struct tm *infoFechaHora;
        char textoFechaHora[80];

        

        if (strcmp(valor_leido_anterior, memoria_compartida) != 0) {  //validar que el dato ya se mostro y no se ha cambiando
            //actualizar la fecha de lectura para visulizarla
            time(&rawtime);
            infoFechaHora = localtime(&rawtime);
            strftime(textoFechaHora, sizeof(textoFechaHora), "%Y-%m-%d %H:%M:%S", infoFechaHora);
            //mostrar mensaje
            std::cout << textoFechaHora << " = " << memoria_compartida << "\n";
            strcpy(valor_leido_anterior, memoria_compartida);
        }

        sleep(1); // Esperar un tiempo antes de leer de nuevo
    }

    pthread_join(thread, nullptr); // Esperar a que el hilo termine

    desconectar_memoria_compartida(memoria_compartida);
}

int main() {
    key_t key = ftok("/tmp", 3);
    int id_memoriaCompartida = shmget(key, SHM_SIZE, 0666);

    if (id_memoriaCompartida == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    lectorMemoria(id_memoriaCompartida);

    return 0;
}
