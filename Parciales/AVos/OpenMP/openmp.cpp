#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cmath>
#include <chrono>
#include <omp.h>

using namespace std;

struct Pixel
{
  unsigned char red, green, blue;
};

#pragma pack(push, 1)
struct BMPHeader
{
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
vector<vector<Pixel>> leerArchivoBMP(const char *nombreArchivo, BMPHeader &header)
{
  ifstream archivo(nombreArchivo, ios::binary);
  if (!archivo)
  {
    cerr << "No se pudo abrir el archivo" << endl;
    exit(1);
  }
  archivo.read(reinterpret_cast<char *>(&header), sizeof(BMPHeader));
  archivo.seekg(header.dataOffSet, ios::beg);
  vector<vector<Pixel>> matriz(header.height, vector<Pixel>(header.width));
  for (int i = 0; i < header.height; i++)
  {
    for (int j = 0; j < header.width; j++)
    {
      archivo.read(reinterpret_cast<char *>(&matriz[i][j]), sizeof(Pixel));
    }
    archivo.seekg(header.width % 4, ios::cur);
  }
  archivo.close();
  return matriz;
}

vector<vector<Pixel>> rotarImagen(const vector<vector<Pixel>> &matrizPixeles, const int grados, BMPHeader &header)
{
  double radianes = grados * M_PI / 180.0;
  double centroXOriginal = matrizPixeles[0].size() / 2.0;
  double centroYOriginal = matrizPixeles.size() / 2.0;
  double radio = sqrt(centroXOriginal * centroXOriginal + centroYOriginal * centroYOriginal);
  
  vector<vector<Pixel>> matrizRotada(static_cast<int>(2 * radio), vector<Pixel>(static_cast<int>(2 * radio)));
  double centroXNuevo = matrizRotada[0].size() / 2.0;
  double centroYNuevo = matrizRotada.size() / 2.0;

  #pragma omp parallel for
  for (size_t i = 0; i < matrizRotada.size(); ++i)
  {
    for (size_t j = 0; j < matrizRotada[0].size(); ++j)
    {
      double x = (j - centroXNuevo) * cos(radianes) - (i - centroYNuevo) * sin(radianes) + centroXOriginal;
      double y = (j - centroXNuevo) * sin(radianes) + (i - centroYNuevo) * cos(radianes) + centroYOriginal;
      if (x >= 0 && x < matrizPixeles[0].size() && y >= 0 && y < matrizPixeles.size())
      {
  #pragma omp atomic      
        matrizRotada[i][j] = matrizPixeles[static_cast<int>(y)][static_cast<int>(x)];
      }
    }
  }
  return matrizRotada;
}


vector<vector<Pixel>> escalarImagen(const vector<vector<Pixel>> &matrizPixeles, const int width, const int height, BMPHeader &header)
{
  double centroX = width / 2.0;
  double centroY = height / 2.0;
  vector<vector<Pixel>> matrizEscalada(height, vector<Pixel>(width));

  #pragma omp parallel for 
  for (size_t i = 0; i < height; ++i)
  {
    for (size_t j = 0; j < width; ++j)
    {
      double x = j * (matrizPixeles[0].size() / static_cast<double>(width));
      double y = i * (matrizPixeles.size() / static_cast<double>(height));

      if (x >= 0 && x < matrizPixeles[0].size() && y >= 0 && y < matrizPixeles.size())
      {
  #pragma omp atomic    
        matrizEscalada[i][j] = matrizPixeles[static_cast<int>(y)][static_cast<int>(x)];
      }
    }
  }
  return matrizEscalada;
}

void escribirArchivoBMP(const char *nombreArchivo, const vector<vector<Pixel>> &matriz)
{
  ofstream archivo(nombreArchivo, ios::binary);
  if (!archivo)
  {
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

  archivo.write(reinterpret_cast<char *>(&header), sizeof(BMPHeader));

  for (int i = 0; i < matriz.size(); ++i)
  {
    for (int j = 0; j < matriz[0].size(); ++j)
    {
      archivo.write(reinterpret_cast<const char *>(&matriz[i][j]), sizeof(Pixel));
    }
    for (int k = 0; k < matriz[0].size() % 4; ++k)
    {
      char paddingByte = 0;
      archivo.write(&paddingByte, 1);
    }
  }
  archivo.close();
}

int main(int argc, char *argv[])
{
  BMPHeader headerImagenEntrada;
  vector<vector<Pixel>> matrizImagenEntrada = leerArchivoBMP(argv[1], headerImagenEntrada);
  vector<vector<Pixel>> matrizImagenSalida;
  if (string(argv[2]) == "Rotar")
  {
    if (argc != 4)
    {
      cerr << "Uso: " << argv[0] << " <nombreArchivo>" << argv[1] << " <Operacion (Rotar)>" << argv[2] << " <Medida (Grados)>";
      return 1;
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    matrizImagenSalida = rotarImagen(matrizImagenEntrada, atoi(argv[3]), headerImagenEntrada);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Tiempo de ejecucion: " << duration << " ms" << endl;

    escribirArchivoBMP("rotada.bmp", matrizImagenSalida);
  }
  else if (string(argv[2]) == "Escalar")
  {
    if (argc != 5)
    {
      cerr << "Uso: " << argv[0] << " <nombreArchivo>" << argv[1] << " <Operacion (Escalar)>" << argv[2] << " <Medida (Width)>"
           << argv[3] << " <Medida (Height)>" << endl;
      return 1;
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    matrizImagenSalida = escalarImagen(matrizImagenEntrada, atoi(argv[3]), atoi(argv[4]), headerImagenEntrada);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    cout << "Tiempo de ejecucion: " << duration << " ms" << endl;

    escribirArchivoBMP("escalada.bmp", matrizImagenSalida);
  }
  else
  {
    cerr << "Uso: " << argv[0] << " <nombreArchivo>" << argv[1] << " <Operacion (Rotar/Escalar)>" << argv[2] << " <Medida(Grados/Width)>"
         << argv[3] << " <Height (si se va a escalar)>";
    return 1;
  }
  return 0;
}
