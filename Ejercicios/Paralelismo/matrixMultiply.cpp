#include <iostream>
#include <vector>
#include <random>

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

// Función para multiplicar matrices de manera secuencial
void matrixMultiply_Secuencial(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B,
                    std::vector<std::vector<int>>& resultado) {
    for (size_t i = 0; i < A.size(); ++i) {
        for (size_t j = 0; j < B[0].size(); ++j) {
            for (size_t k = 0; k < A[0].size(); ++k) {
                resultado[i][j] += A[i][k] * B[k][j];
            }
        }
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

    // Multiplicar matrices de manera secuencial
    matrixMultiply_Secuencial(A, B, resultado);

    // Imprimir matriz de resultado
    verMatriz("Resultado", resultado);

    return 0;
}
