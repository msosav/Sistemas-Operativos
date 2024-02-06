#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>

// Función para imprimir una matriz
template<typename T>
void verMatriz(const std::string& texto, const std::vector<std::vector<T>>& matriz) {
    std::cout << texto << ":\n";
    for (const auto& fila : matriz) {
        for (const T& elemento : fila) {
            std::cout << elemento << " ";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

// Función para generar una matriz con dimensiones especificadas
template<typename T>
std::vector<std::vector<T>> generarMatriz(size_t filas, size_t columnas) {
    std::vector<std::vector<T>> matriz(filas, std::vector<T>(columnas));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis(1, 10);

    for (size_t i = 0; i < filas; ++i) {
        for (size_t j = 0; j < columnas; ++j) {
            matriz[i][j] = dis(gen);
        }
    }

    return matriz;
}

// Función para multiplicar matrices en paralelo utilizando procesos con memoria compartida
void matrixMultiply(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B,
                    std::vector<std::vector<int>>& resultado, size_t startRow, size_t endRow) {
    for (size_t i = startRow; i < endRow; ++i) {
        for (size_t j = 0; j < B[0].size(); ++j) {
            for (size_t k = 0; k < A[0].size(); ++k) {
                resultado[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Función para multiplicar matrices en paralelo utilizando procesos
void multiplicarMatricesConProcesos(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B,
                                    std::vector<std::vector<int>>& resultado, size_t numProcesos) {
    size_t numRows = A.size();
    size_t numCols = B[0].size();

    // Crear matriz de resultado utilizando memoria compartida
    std::vector<std::vector<int>> sharedMatrix(numRows, std::vector<int>(numCols, 0));
    int* sharedResult = static_cast<int*>(mmap(NULL, numRows * numCols * sizeof(int),
                                               PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));

    for (size_t i = 0; i < numRows; ++i) {
        for (size_t j = 0; j < numCols; ++j) {
            sharedResult[i * numCols + j] = resultado[i][j];
        }
    }
    //------------------------------------
    std::vector<pid_t> pids(numProcesos);
    for (size_t i = 0; i < numProcesos; ++i) {
        pids[i] = fork();

        if (pids[i] == 0) {  // Proceso hijo
            size_t startRow = i * (numRows / numProcesos);
            size_t endRow = (i == numProcesos - 1) ? numRows : (i + 1) * (numRows / numProcesos);

            // Convertir la memoria compartida de nuevo a una matriz
            std::vector<std::vector<int>> sharedMatrixCopy(numRows, std::vector<int>(numCols, 0));
            for (size_t row = 0; row < numRows; ++row) {
                for (size_t col = 0; col < numCols; ++col) {
                    sharedMatrixCopy[row][col] = sharedResult[row * numCols + col];
                }
            }

            // Realizar la multiplicación de matrices
            matrixMultiply(A, B, sharedMatrixCopy, startRow, endRow);

            // Copiar los resultados de vuelta a la memoria compartida
            for (size_t row = 0; row < numRows; ++row) {
                for (size_t col = 0; col < numCols; ++col) {
                    sharedResult[row * numCols + col] = sharedMatrixCopy[row][col];
                }
            }

            exit(0);  // Salir del proceso hijo después de la multiplicación
        } else if (pids[i] < 0) {  // Error al crear el proceso
            std::cerr << "Error al crear el proceso hijo.\n";
            return;
        }
    }
    //-------------------------------
    // Esperar a que todos los procesos hijos terminen
    for (size_t i = 0; i < numProcesos; ++i) {
        waitpid(pids[i], NULL, 0);
    }
    //--------------------------------
    // Convertir la memoria compartida de nuevo a una matriz
    for (size_t row = 0; row < numRows; ++row) {
        for (size_t col = 0; col < numCols; ++col) {
            resultado[row][col] = sharedResult[row * numCols + col];
        }
    }
    //---------------------------------
    // Liberar la memoria compartida
    munmap(sharedResult, numRows * numCols * sizeof(int));
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <filas> <columnas>\n";
        return 1;
    }

    // Convertir argumentos a enteros
    size_t numRows = std::atoi(argv[1]);
    size_t numCols = std::atoi(argv[2]);

    // Generar matrices A y B con dimensiones específicas
    std::vector<std::vector<int>> A = generarMatriz<int>(numRows, numCols);
    std::vector<std::vector<int>> B = generarMatriz<int>(numRows, numCols);
    std::vector<std::vector<int>> resultado(numRows, std::vector<int>(numCols, 0));

    // Imprimir matrices de entrada
    verMatriz("Matriz A", A);
    verMatriz("Matriz B", B);

    const size_t numProcesos = 2;

    // Multiplicar matrices utilizando procesos
    multiplicarMatricesConProcesos(A, B, resultado, numProcesos);

    // Imprimir matriz de resultado
    verMatriz("Resultado", resultado);

    return 0;
}
