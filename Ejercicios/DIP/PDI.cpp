#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <omp.h>

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

// Prototipos de funciones
vector<vector<Pixel>> leerArchivoBMP(const char* nombreArchivo);
void guardarMatrizEnBMP(const char* nombreArchivo, const vector<vector<Pixel>>& matriz);
void aplicarConvolucion(const vector<vector<Pixel>>& entrada, vector<vector<Pixel>>& salida, const vector<vector<int>>& kernel);
//void suavizarImagen(vector<vector<Pixel>>& matriz);
void suavizarImagen(vector<vector<Pixel>>& matriz, double porcentajeSuavizado);
void detectarBordesPrewitt(vector<vector<Pixel>>& matriz);
//void realzarCaracteristicas(vector<vector<Pixel>>& matriz);
void realzarCaracteristicas(vector<vector<Pixel>>& matriz, double valorEscalar);
void detectarBordesPrewitt_OMP(vector<vector<Pixel>>& matriz);
void realzarCaracteristicas_OMP(vector<vector<Pixel>>& matriz, double valorEscalar);
void suavizarImagen_OMP(vector<vector<Pixel>>& matriz, double porcentajeSuavizado);

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
    // Por ejemplo, suavizar la imagen, detectar bordes o realzar características

    //suavizarImagen(matriz, 265.0);
    //detectarBordesPrewitt(matriz);
    //realzarCaracteristicas(matriz,2.50);
    //suavizarImagen_OMP(matriz, 255.0);
    detectarBordesPrewitt_OMP(matriz);
    //realzarCaracteristicas_OMP(matriz,2.50);

    // Guardar la matriz en un nuevo archivo BMP
    guardarMatrizEnBMP(nombreArchivoEscrituraBMP, matriz);

    return 0;
}


//===[Funciones de IO archivos BMP]=============================================================================


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
//====[Funciones de Procesado de Imagenes secuenciales]=================================================

// Función para aplicar convolución a una matriz de píxeles
void aplicarConvolucion(const vector<vector<Pixel>>& entrada, vector<vector<Pixel>>& salida, const vector<vector<int>>& kernel) {
    int altura = entrada.size();
    int ancho = entrada[0].size();
    int tamanoKernel = kernel.size();
    int sumaKernel = 0;

    // Calcular la suma de los valores del kernel
    for (int i = 0; i < tamanoKernel; ++i) {
        for (int j = 0; j < tamanoKernel; ++j) {
            sumaKernel += kernel[i][j];
        }
    }

    // Aplicar convolución a cada píxel de la imagen
    for (int i = 0; i < altura; ++i) {
        for (int j = 0; j < ancho; ++j) {
            int sumaAzul = 0, sumaVerde = 0, sumaRojo = 0;

            for (int m = 0; m < tamanoKernel; ++m) {
                for (int n = 0; n < tamanoKernel; ++n) {
                    int x = i + m - tamanoKernel / 2;
                    int y = j + n - tamanoKernel / 2;

                    if (x >= 0 && x < altura && y >= 0 && y < ancho) {
                        sumaAzul += entrada[x][y].blue * kernel[m][n];
                        sumaVerde += entrada[x][y].green * kernel[m][n];
                        sumaRojo += entrada[x][y].red * kernel[m][n];
                    }
                }
            }

            // Normalizar y asegurar que los valores estén en el rango [0, 255]
            sumaAzul = min(max(sumaAzul / sumaKernel, 0), 255);
            sumaVerde = min(max(sumaVerde / sumaKernel, 0), 255);
            sumaRojo = min(max(sumaRojo / sumaKernel, 0), 255);

            salida[i][j].blue = static_cast<unsigned char>(sumaAzul);
            salida[i][j].green = static_cast<unsigned char>(sumaVerde);
            salida[i][j].red = static_cast<unsigned char>(sumaRojo);
        }
    }
}

/*
// Función para suavizar la imagen
void suavizarImagen(vector<vector<Pixel>>& matriz) {
    vector<vector<int>> kernelSuavizado = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
    aplicarConvolucion(matriz, matriz, kernelSuavizado);
}
*/
void suavizarImagen(vector<vector<Pixel>>& matriz, double porcentajeSuavizado) {
    vector<vector<int>> kernelSuavizado = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}}; // Kernel de suavizado (filtro Gaussiano)

    int tamanoKernel = kernelSuavizado.size();
    int sumaKernel = 16;  // Suma total de los valores del kernel (1+2+1+2+4+2+1+2+1)

    // Aplicar suavizado a cada píxel de la imagen
    for (int i = 1; i < matriz.size() - 1; ++i) {
        for (int j = 1; j < matriz[0].size() - 1; ++j) {
            int sumaAzul = 0, sumaVerde = 0, sumaRojo = 0;

            for (int m = 0; m < tamanoKernel; ++m) {
                for (int n = 0; n < tamanoKernel; ++n) {
                    int x = i + m - tamanoKernel / 2;
                    int y = j + n - tamanoKernel / 2;

                    sumaAzul += matriz[x][y].blue * kernelSuavizado[m][n];
                    sumaVerde += matriz[x][y].green * kernelSuavizado[m][n];
                    sumaRojo += matriz[x][y].red * kernelSuavizado[m][n];
                }
            }

            // Normalizar y aplicar el porcentaje de suavizado
            sumaAzul = static_cast<int>(matriz[i][j].blue + (porcentajeSuavizado / 100.0) * (sumaAzul / sumaKernel - matriz[i][j].blue));
            sumaVerde = static_cast<int>(matriz[i][j].green + (porcentajeSuavizado / 100.0) * (sumaVerde / sumaKernel - matriz[i][j].green));
            sumaRojo = static_cast<int>(matriz[i][j].red + (porcentajeSuavizado / 100.0) * (sumaRojo / sumaKernel - matriz[i][j].red));

            // Asegurar que los valores estén en el rango [0, 255]
            matriz[i][j].blue = static_cast<unsigned char>(min(max(sumaAzul, 0), 255));
            matriz[i][j].green = static_cast<unsigned char>(min(max(sumaVerde, 0), 255));
            matriz[i][j].red = static_cast<unsigned char>(min(max(sumaRojo, 0), 255));
        }
    }
}


void detectarBordesPrewitt(vector<vector<Pixel>>& matriz) {
    vector<vector<int>> kernelPrewittX = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
    vector<vector<int>> kernelPrewittY = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};

    int altura = matriz.size();
    int ancho = matriz[0].size();

    vector<vector<int>> resultadoX(altura, vector<int>(ancho, 0));
    vector<vector<int>> resultadoY(altura, vector<int>(ancho, 0));

    // Aplicar convolución por canales separados
    for (int c = 0; c < 3; ++c) {
        for (int i = 1; i < altura - 1; ++i) {
            for (int j = 1; j < ancho - 1; ++j) {
                int sumaX = 0, sumaY = 0;

                for (int m = 0; m < 3; ++m) {
                    for (int n = 0; n < 3; ++n) {
                        int x = i + m - 1;
                        int y = j + n - 1;

                        sumaX += matriz[x][y].blue * kernelPrewittX[m][n];
                        sumaY += matriz[x][y].blue * kernelPrewittY[m][n];
                    }
                }

                resultadoX[i][j] += sumaX;
                resultadoY[i][j] += sumaY;
            }
        }
    }

    // Combinar los resultados en una imagen de magnitudes de bordes
    for (int i = 0; i < altura; ++i) {
        for (int j = 0; j < ancho; ++j) {
            // Calcular la magnitud
            int magnitud = static_cast<int>(sqrt(pow(resultadoX[i][j], 2) + pow(resultadoY[i][j], 2)));

            // Truncar a entero y asegurar que los valores estén en el rango [0, 255]
            matriz[i][j].blue = static_cast<unsigned char>(min(max(magnitud, 0), 255));
            matriz[i][j].green = static_cast<unsigned char>(min(max(magnitud, 0), 255));
            matriz[i][j].red = static_cast<unsigned char>(min(max(magnitud, 0), 255));
        }
    }
}




// Función para realzar características en la imagen
/*
void realzarCaracteristicas(vector<vector<Pixel>>& matriz) {
    vector<vector<int>> kernelRealce = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}};
    aplicarConvolucion(matriz, matriz, kernelRealce);
}
*/

void realzarCaracteristicas(vector<vector<Pixel>>& matriz, double valorEscalar) {
    // Aplicar realce a cada píxel de la imagen
    for (int i = 1; i < matriz.size() - 1; ++i) {
        for (int j = 1; j < matriz[0].size() - 1; ++j) {
            int sumaAzul = 0, sumaVerde = 0, sumaRojo = 0;

            // Realce aplicado al píxel actual y sus vecinos
            sumaAzul = static_cast<int>(matriz[i][j].blue + valorEscalar * (matriz[i][j].blue - matriz[i-1][j-1].blue));
            sumaVerde = static_cast<int>(matriz[i][j].green + valorEscalar * (matriz[i][j].green - matriz[i-1][j-1].green));
            sumaRojo = static_cast<int>(matriz[i][j].red + valorEscalar * (matriz[i][j].red - matriz[i-1][j-1].red));

            // Asegurar que los valores estén en el rango [0, 255]
            matriz[i][j].blue = static_cast<unsigned char>(min(max(sumaAzul, 0), 255));
            matriz[i][j].green = static_cast<unsigned char>(min(max(sumaVerde, 0), 255));
            matriz[i][j].red = static_cast<unsigned char>(min(max(sumaRojo, 0), 255));
        }
    }
}


//===[DIP en paralero con OpenMP]===================================

void detectarBordesPrewitt_OMP(vector<vector<Pixel>>& matriz) {
    vector<vector<int>> kernelPrewittX = {{-1, 0, 1}, {-1, 0, 1}, {-1, 0, 1}};
    vector<vector<int>> kernelPrewittY = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}};

    int altura = matriz.size();
    int ancho = matriz[0].size();

    vector<vector<int>> resultadoX(altura, vector<int>(ancho, 0));
    vector<vector<int>> resultadoY(altura, vector<int>(ancho, 0));

    // Aplicar convolución por canales separados en paralelo
    #pragma omp parallel for
    for (int i = 1; i < altura - 1; ++i) {
        for (int j = 1; j < ancho - 1; ++j) {
            int sumaX = 0, sumaY = 0;

            // Realizar convolución por canales
            #pragma omp simd reduction(+:sumaX, sumaY)
            for (int m = 0; m < 3; ++m) {
                for (int n = 0; n < 3; ++n) {
                    int x = i + m - 1;
                    int y = j + n - 1;

                    sumaX += matriz[x][y].blue * kernelPrewittX[m][n];
                    sumaY += matriz[x][y].blue * kernelPrewittY[m][n];
                }
            }

            #pragma omp critical
            {
                resultadoX[i][j] += sumaX;
                resultadoY[i][j] += sumaY;
            }
        }
    }

    // Combinar los resultados en una imagen de magnitudes de bordes en paralelo
    #pragma omp parallel for
    for (int i = 0; i < altura; ++i) {
        for (int j = 0; j < ancho; ++j) {
            // Calcular la magnitud
            int magnitud = static_cast<int>(sqrt(pow(resultadoX[i][j], 2) + pow(resultadoY[i][j], 2)));

            // Truncar a entero y asegurar que los valores estén en el rango [0, 255]
            #pragma omp critical
            {
                matriz[i][j].blue = static_cast<unsigned char>(min(max(magnitud, 0), 255));
                matriz[i][j].green = static_cast<unsigned char>(min(max(magnitud, 0), 255));
                matriz[i][j].red = static_cast<unsigned char>(min(max(magnitud, 0), 255));
            }
        }
    }
}


void realzarCaracteristicas_OMP(vector<vector<Pixel>>& matriz, double valorEscalar) {
    int altura = matriz.size();
    int ancho = matriz[0].size();

    // Aplicar realce a cada píxel de la imagen en paralelo
    #pragma omp parallel for collapse(2)
    for (int i = 1; i < altura - 1; ++i) {
        for (int j = 1; j < ancho - 1; ++j) {
            int sumaAzul = 0, sumaVerde = 0, sumaRojo = 0;

            // Realce aplicado al píxel actual y sus vecinos
            sumaAzul = static_cast<int>(matriz[i][j].blue + valorEscalar * (matriz[i][j].blue - matriz[i-1][j-1].blue));
            sumaVerde = static_cast<int>(matriz[i][j].green + valorEscalar * (matriz[i][j].green - matriz[i-1][j-1].green));
            sumaRojo = static_cast<int>(matriz[i][j].red + valorEscalar * (matriz[i][j].red - matriz[i-1][j-1].red));

            // Asegurar que los valores estén en el rango [0, 255]
            #pragma omp critical
            {
                matriz[i][j].blue = static_cast<unsigned char>(min(max(sumaAzul, 0), 255));
                matriz[i][j].green = static_cast<unsigned char>(min(max(sumaVerde, 0), 255));
                matriz[i][j].red = static_cast<unsigned char>(min(max(sumaRojo, 0), 255));
            }
        }
    }
}


void suavizarImagen_OMP(vector<vector<Pixel>>& matriz, double porcentajeSuavizado) {
    vector<vector<int>> kernelSuavizado = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}}; // Kernel de suavizado (filtro Gaussiano)

    int tamanoKernel = kernelSuavizado.size();
    int sumaKernel = 16;  // Suma total de los valores del kernel (1+2+1+2+4+2+1+2+1)

    int altura = matriz.size();
    int ancho = matriz[0].size();

    // Aplicar suavizado a cada píxel de la imagen en paralelo
    #pragma omp parallel for collapse(2)
    for (int i = 1; i < altura - 1; ++i) {
        for (int j = 1; j < ancho - 1; ++j) {
            int sumaAzul = 0, sumaVerde = 0, sumaRojo = 0;

            #pragma omp parallel for reduction(+:sumaAzul, sumaVerde, sumaRojo) collapse(2)
            for (int m = 0; m < tamanoKernel; ++m) {
                for (int n = 0; n < tamanoKernel; ++n) {
                    int x = i + m - tamanoKernel / 2;
                    int y = j + n - tamanoKernel / 2;

                    sumaAzul += matriz[x][y].blue * kernelSuavizado[m][n];
                    sumaVerde += matriz[x][y].green * kernelSuavizado[m][n];
                    sumaRojo += matriz[x][y].red * kernelSuavizado[m][n];
                }
            }

            // Normalizar y aplicar el porcentaje de suavizado
            sumaAzul = static_cast<int>(matriz[i][j].blue + (porcentajeSuavizado / 100.0) * (sumaAzul / sumaKernel - matriz[i][j].blue));
            sumaVerde = static_cast<int>(matriz[i][j].green + (porcentajeSuavizado / 100.0) * (sumaVerde / sumaKernel - matriz[i][j].green));
            sumaRojo = static_cast<int>(matriz[i][j].red + (porcentajeSuavizado / 100.0) * (sumaRojo / sumaKernel - matriz[i][j].red));

            // Asegurar que los valores estén en el rango [0, 255]
            #pragma omp critical
            {
                matriz[i][j].blue = static_cast<unsigned char>(min(max(sumaAzul, 0), 255));
                matriz[i][j].green = static_cast<unsigned char>(min(max(sumaVerde, 0), 255));
                matriz[i][j].red = static_cast<unsigned char>(min(max(sumaRojo, 0), 255));
            }
        }
    }
}
