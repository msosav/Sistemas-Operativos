#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <ctime>
#include <chrono>

using namespace std;

struct Pixel {
    unsigned char red, green, blue;
};

#pragma pack(push, 1)
struct BMPHeader {
  char signature[2];
  int fileSize;
  int reserved;
  int dataOffSet;
  int headerSize;
  int width;
  int height;
  short planes;
  short bitsPerPixel;
  int compression;
  int dataSize;
  int horizontalResolution;
  int verticalResolution;
  int colors;
  int importantColors;
};

#pragma pack(pop)
vector<vector<Pixel>> leerArchivoBMP(const char* nombreArchivo) {
  ifstream archivo(nombreArchivo, ios::binary);
  if (!archivo) {
    cerr << "No se pudo abrir el archivo" << endl;
    exit(1);
  }
  BMPHeader header;
  archivo.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));
  std::cout << "Ancho: " << header.width << " Alto: " << header.height << std::endl;
  if (header.bitsPerPixel != 24) {
    cerr << "Solo se soportan imagenes de 24 bits" << endl;
    exit(1);
  }
  archivo.seekg(header.dataOffSet, ios::beg);
  vector<vector<Pixel>> matriz(header.height, vector<Pixel>(header.width));
  for (int i = 0; i < header.height; i++) {
    for (int j = 0; j < header.width; j++) {
      archivo.read(reinterpret_cast<char*>(&matriz[i][j]), sizeof(Pixel));
    }
    archivo.seekg(header.width % 4, ios::cur);
  }
  archivo.close();
  return matriz;
}

void verMatriz(int x1, int y1, int x2, int y2, const vector<vector<Pixel>> matrizPixeles) {
  std::cout << "Matriz de pixeles" << std::endl;
  #pragma omp parallel for
  for (int i = x1; i < x2; i++) {
    for (int j = y1; j < y2; j++) {
      #pragma omp critical
      std::cout << "(" << static_cast<int>(matrizPixeles[i][j].red) << ", "
                << static_cast<int>(matrizPixeles[i][j].green) << ", "
                << static_cast<int>(matrizPixeles[i][j].blue) << ") ";
    }
    std::cout << std::endl;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Uso: " << argv[0] << " <nombreArchivo>" << endl;
    return 1;
  }
  vector<vector<Pixel>> matrizPixeles = leerArchivoBMP(argv[1]);
  auto start_time = std::chrono::high_resolution_clock::now();
    verMatriz(0, 0, 5, 5, matrizPixeles);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float, std::milli> duration = end_time - start_time;
  std::cout << "Tiempo de ejecución: " << duration.count() << " ms" << std::endl;
  return 0;
}
