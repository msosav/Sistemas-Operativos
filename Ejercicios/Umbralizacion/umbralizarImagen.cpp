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
vector<vector<Pixel>> leerArchivoBMP(const char* nombreArchivo, BMPHeader &header) {
  ifstream archivo(nombreArchivo, ios::binary);
  if (!archivo) {
    cerr << "No se pudo abrir el archivo" << endl;
    exit(1);
  }
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

vector<vector<Pixel>> umbralizarMatriz(const vector<vector<Pixel>> matrizPixeles, const BMPHeader &header) {
  std::cout << "Ancho: " << header.width << " Alto: " << header.height << std::endl;
  vector<vector<Pixel>> matrizUmbralizada(header.height, vector<Pixel>(header.width));
  #pragma omp parallel for
  for (int i = 0; i < header.height; i++) {
    for (int j = 0; j < header.width; j++) {
      #pragma omp atomic
      int promedio = (matrizPixeles[i][j].red + matrizPixeles[i][j].green + matrizPixeles[i][j].blue) / 3;
      if (promedio > 128) {
        matrizUmbralizada[i][j].red = 255;
        matrizUmbralizada[i][j].green = 255;
        matrizUmbralizada[i][j].blue = 255;
      } else {
        matrizUmbralizada[i][j].red = 0;
        matrizUmbralizada[i][j].green = 0;
        matrizUmbralizada[i][j].blue = 0;
      }
    }
  }
  return matrizUmbralizada;
}

void escribirArchivoBMP(const char* nombreArchivo, const BMPHeader &header, const vector<vector<Pixel>> &matrizPixeles) {
  ofstream archivo(nombreArchivo, ios::binary);
  if (!archivo) {
    cerr << "No se pudo abrir el archivo" << endl;
    exit(1);
  }
  archivo.write(reinterpret_cast<const char*>(&header), sizeof(BMPHeader));
  for (int i = 0; i < header.height; i++) {
    for (int j = 0; j < header.width; j++) {
      archivo.write(reinterpret_cast<const char*>(&matrizPixeles[i][j]), sizeof(Pixel));
    }
  }
  archivo.close();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Uso: " << argv[0] << " <nombreArchivo>" << endl;
    return 1;
  }
  BMPHeader header;
  vector<vector<Pixel>> matrizPixeles = leerArchivoBMP(argv[1], header);
  auto start_time = std::chrono::high_resolution_clock::now();
  vector<vector<Pixel>> matrizUmbralizada = umbralizarMatriz(matrizPixeles, header);
  escribirArchivoBMP("umbralizada.bmp", header, matrizUmbralizada);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float, std::milli> duration = end_time - start_time;
  std::cout << "Tiempo de ejecución: " << duration.count() << " ms" << std::endl;
  return 0;
}
