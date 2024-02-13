#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <ctime>

#define SHM_SIZE 1024

char *conectar_memoria_compartida(int id_memoriaCompartida) {
    char *memoria_Compartida = static_cast<char *>(shmat(id_memoriaCompartida, nullptr, 0));
    if (memoria_Compartida == reinterpret_cast<char *>(-1)) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    return memoria_Compartida;
}

void desconectar_memoria_compartida(char *memoria_Compartida) {
    if (shmdt(memoria_Compartida) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
}

void escritor(int id_memoriaCompartida) {
    char *memoria_Compartida = conectar_memoria_compartida(id_memoriaCompartida);

    time_t rawtime;
    struct tm *infoFecha;
    char textoFecha[80];

    while (true) {
        std::cout << "Escribe un texto para transmitir a la memoria compartida (presiona 'x' para terminar): ";
        std::cout.flush();

        char msgUsuario[100];
        std::cin.getline(msgUsuario, sizeof(msgUsuario));

        if (msgUsuario[0] == 'x' || msgUsuario[0] == 'X') {
            break;  // Termina la transmisión si el usuario ingresa 'x' o 'X'
        }

        time(&rawtime);
        infoFecha = localtime(&rawtime);
        strftime(textoFecha, sizeof(textoFecha), "%Y-%m-%d %H:%M:%S", infoFecha);

        // Escribe el texto en la memoria compartida con formato
        sprintf(memoria_Compartida, "%s", msgUsuario);

        std::cout << "Escrito en memoria: " << msgUsuario << " - (" << textoFecha << ")\n";
    }

    desconectar_memoria_compartida(memoria_Compartida);
}

int main() {
    key_t key = ftok("/tmp", 3);
    int id_memoriaCompartida = shmget(key, SHM_SIZE, IPC_CREAT | 0666);

    if (id_memoriaCompartida == -1) {
        perror("shmget");  //shmget es es una llamada al sistema en sistemas operativos tipo Unix, diseñada para obtener un identificador asociado a un segmento de memoria compartida
        exit(EXIT_FAILURE);
    }

    escritor(id_memoriaCompartida);

    return 0;
}
