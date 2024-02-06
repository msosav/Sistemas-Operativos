#include <iostream>
#include <vector>
#include <random>
#include <thread>

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

// Función para multiplicar matrices en paralelo utilizando hilos
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

// Función para multiplicar matrices en paralelo utilizando hilos
void multiplicarMatricesConHilos(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B,
                                  std::vector<std::vector<int>>& resultado, size_t numHilos) {
    std::vector<std::thread> hilos(numHilos);

    for (size_t i = 0; i < numHilos; ++i) {
        size_t startRow = i * (A.size() / numHilos);
        size_t endRow = (i == numHilos - 1) ? A.size() : (i + 1) * (A.size() / numHilos);

        hilos[i] = std::thread(matrixMultiply, std::ref(A), std::ref(B), std::ref(resultado), startRow, endRow);
    }

    // Esperar a que todos los hilos terminen
    for (size_t i = 0; i < numHilos; ++i) {
        hilos[i].join();
    }
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

    const size_t numHilos = 2;

    // Multiplicar matrices utilizando hilos
    multiplicarMatricesConHilos(A, B, resultado, numHilos);

    // Imprimir matriz de resultado
    verMatriz("Resultado", resultado);

    return 0;
}
