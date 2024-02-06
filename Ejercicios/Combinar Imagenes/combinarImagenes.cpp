#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

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

vector<vector<Pixel>> combinarImagenes(const vector<vector<Pixel>> matrizPixeles, const vector<vector<Pixel>> matrizPixeles2, const BMPHeader &header) {
  vector<vector<Pixel>> matrizCombinada(header.height, vector<Pixel>(header.width));
  #pragma omp parallel for
  for (int i = 0; i < header.height; i++) {
    for (int j = 0; j < header.width; j++) {
      matrizCombinada[i][j].red = (matrizPixeles[i][j].red + matrizPixeles2[i][j].red) / 2;
      matrizCombinada[i][j].green = (matrizPixeles[i][j].green + matrizPixeles2[i][j].green) / 2;
      matrizCombinada[i][j].blue = (matrizPixeles[i][j].blue + matrizPixeles2[i][j].blue) / 2;
    }
  }
  return matrizCombinada;
}

void escribirArchivoBMP(const char* nombreArchivo, const vector<vector<Pixel>> &matriz) {
  ofstream archivo(nombreArchivo, ios::binary);
  if (!archivo) {
    cerr << "No se pudo abrir el archivo" << endl;
    exit(1);
  }
    BMPHeader header;
    header.signature[0] = 'B';
    header.signature[1] = 'M';
    header.fileSize = sizeof(BMPHeader) + matriz.size() * ((3 * matriz[0].size()) + (matriz[0].size() % 4)) + 2; // +2 for padding
    header.reserved = 0;
    header.dataOffSet = sizeof(BMPHeader);
    header.headerSize = 40;
    header.width = matriz[0].size();
    header.height = matriz.size();
    header.planes = 1;
    header.bitsPerPixel = 24;
    header.compression = 0;
    header.dataSize = matriz.size() * ((3 * matriz[0].size()) + (matriz[0].size() % 4)) + 2; // +2 for padding
    header.horizontalResolution = 0;
    header.verticalResolution = 0;
    header.colors = 0;
    header.importantColors = 0;

    archivo.write(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    // Escribir la matriz en el archivo BMP
    for (int i = 0; i < matriz.size(); ++i) {
        for (int j = 0; j < matriz[0].size(); ++j) {
            archivo.write(reinterpret_cast<const char*>(&matriz[i][j]), sizeof(Pixel));
        }
        // Rellenar con bytes de 0 para la alineación de 4 bytes
        for (int k = 0; k < matriz[0].size() % 4; ++k) {
            char paddingByte = 0;
            archivo.write(&paddingByte, 1);
        }
    }
    archivo.close();
}

int main(int argc, char *argv[]) {
  cout << "Combinar Imagenes" << endl;
  cout << "Cantidad de argumentos: " << argc << endl;
  if (argc != 4) {
    cerr << "Uso: " << argv[0] << " <nombreArchivo1>" << argv[1] << " <nombreArchivo2>" << argv[2] << " <nombreArchivoDeSalida>" << endl;
    return 1;
  }
  BMPHeader headerArchivo1, headerArchivo2, headerArchivoSalida;
  vector<vector<Pixel>> matrizPixeles = leerArchivoBMP(argv[1], headerArchivo1);
  vector<vector<Pixel>> matrizPixeles2 = leerArchivoBMP(argv[2], headerArchivo2);
  if (headerArchivo1.width != headerArchivo2.width || headerArchivo1.height != headerArchivo2.height) {
    cerr << "Las imagenes deben tener el mismo tamaño" << endl;
    return 1;
  }
  vector<vector<Pixel>> matrizCombinada = combinarImagenes(matrizPixeles, matrizPixeles2, headerArchivo1);
  escribirArchivoBMP(argv[3], matrizCombinada);
  return 0;
}

