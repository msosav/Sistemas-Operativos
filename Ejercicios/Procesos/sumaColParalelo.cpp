#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

int generarNumeroAleatorio(int min, int max) {
  return min + rand() % (max - min + 1);
}

void imprimirMatriz(const std::vector<std::vector<int> > &matriz) {
  for (const auto& fila : matriz) {
    for (int elemento : fila) {
      std::cout << elemento << "\t";
    }
    std::cout << std::endl;
  }
}

std::vector<int> sumaColumnas(const std::vector<std::vector<int> > &matriz) {
  int filas = matriz.size();
  int columnas = matriz[0].size();
  std::vector<int> suma(columnas, 0);
  #pragma omp parallel for
  for (int j = 0; j < columnas; j++) {
    for (int i = 0; i < filas; i++) {
      #pragma omp atomic
      suma[j] += matriz[i][j];
    }
  }
  return suma;
}

int main() {
  std::srand(std::time(0));
  int filas, columnas;
  std::cout << "Ingrese el número de filas: ";
  std::cin >> filas;
  std::cout << "Ingrese el número de columnas: ";
  std::cin >> columnas;
  std::vector<std::vector<int> > matriz(filas, std::vector<int>(columnas, 0));
  for (int i = 0; i < filas; i++) {
    for (int j = 0; j < columnas; j++) {
      matriz[i][j] = generarNumeroAleatorio(1, 10);
    }
  }
  auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<int> suma = sumaColumnas(matriz);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float, std::milli> duration = end_time - start_time;
  std::cout << "Tiempo de ejecución: " << duration.count() << " ms" << std::endl;
  std::cout << std::endl;
  return 0;
}
