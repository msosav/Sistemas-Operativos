#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Pixel {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

#pragma pack(push, 1)
struct BMPHeader {
    char signature[2];
    int fileSize;
    int reserved;
    int dataOffset;
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
        cerr << "No se pudo abrir el archivo BMP" << endl;
        exit(1);
    }

    BMPHeader header;
    archivo.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    if (header.bitsPerPixel != 24) {
        cerr << "El archivo BMP debe tener 24 bits por píxel" << endl;
        exit(1);
    }

    // Mover el puntero al inicio de los datos de píxeles
    archivo.seekg(header.dataOffset, ios::beg);

    vector<vector<Pixel>> matriz(header.height, vector<Pixel>(header.width));

    for (int i = 0; i < header.height; ++i) {
        for (int j = 0; j < header.width; ++j) {
            archivo.read(reinterpret_cast<char*>(&matriz[i][j]), sizeof(Pixel));
        }
        archivo.seekg(header.width % 4, ios::cur);
    }
    archivo.close();
    return matriz;
}


void guardarMatrizEnBMP(const char* nombreArchivo, const vector<vector<Pixel>>& matriz) {
    ofstream archivo(nombreArchivo, ios::binary);

    if (!archivo) {
        cerr << "No se pudo crear el archivo BMP" << endl;
        exit(1);
    }

    BMPHeader header;
    header.signature[0] = 'B';
    header.signature[1] = 'M';
    header.fileSize = sizeof(BMPHeader) + matriz.size() * ((3 * matriz[0].size()) + (matriz[0].size() % 4)) + 2; // +2 for padding
    header.reserved = 0;
    header.dataOffset = sizeof(BMPHeader);
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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <nombre_del_archivo_entrada.bmp> <nombre_del_archivo_salida.bmp>" << endl;
        return 1;
    }
    const char* nombreArchivoLecturaBMP = argv[1];
    const char* nombreArchivoEscrituraBMP = argv[2];


    // Leer el archivo BMP y obtener la matriz de píxeles
    vector<vector<Pixel>> matriz = leerArchivoBMP(nombreArchivoLecturaBMP);

    // Realizar operaciones en la matriz si es necesario...

    // Guardar la matriz en un nuevo archivo BMP
    guardarMatrizEnBMP(nombreArchivoEscrituraBMP, matriz);

    return 0;
}
