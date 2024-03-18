#include <iostream>
#include <fstream>
#include <bitset>
#include <iomanip> // Para usar la manipulación de salida hex

void mostrarArchivoBinario(const std::string &nombreArchivo)
{
    std::ifstream archivo(nombreArchivo, std::ios::binary);

    if (!archivo)
    {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return;
    }

    char caracter;
    int contador = 0;

    while (archivo.get(caracter))
    {
        if (caracter <= 1)
        {
            std::bitset<8> binario(caracter);
            std::cout << "[" << std::setw(6) << contador << "] -> {    } : "
                      << "(B) " << std::setw(16) << binario
                      << " | (O) " << std::setw(12) << std::oct << static_cast<unsigned int>(caracter)
                      << " | (D) " << std::setw(12) << static_cast<unsigned int>(caracter)
                      << " | (H) " << std::setw(8) << std::uppercase << std::hex << static_cast<unsigned int>(caracter)
                      << std::nouppercase << std::dec << std::endl;
        }
        else if (caracter == '\n')
        {
            std::bitset<8> binario(caracter);
            std::cout << "[" << std::setw(6) << contador << "] -> {" << std::setw(4) << "\\n} : "
                      << "(B) " << std::setw(16) << binario
                      << " | (O) " << std::setw(12) << std::oct << static_cast<unsigned int>(caracter)
                      << " | (D) " << std::setw(12) << static_cast<unsigned int>(caracter)
                      << " | (H) " << std::setw(8) << std::uppercase << std::hex << static_cast<unsigned int>(caracter)
                      << std::nouppercase << std::dec << std::endl;
        }
        else
        {
            std::bitset<8> binario(caracter);
            std::cout << "[" << std::setw(6) << contador << "] -> {" << std::setw(4) << caracter << "} : "
                      << "(B) " << std::setw(16) << binario
                      << " | (O) " << std::setw(12) << std::oct << static_cast<unsigned int>(caracter)
                      << " | (D) " << std::setw(12) << std::dec << static_cast<unsigned int>(caracter)
                      << " | (H) " << std::setw(8) << std::uppercase << std::hex << static_cast<unsigned int>(caracter)
                      << std::endl;
        }
        contador++;
    }

    std::cout << "Fin del Archivo" << std::endl;

    archivo.close();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <nombre_archivo>" << std::endl;
        return 1;
    }

    std::string nombreArchivo = argv[1];

    mostrarArchivoBinario(nombreArchivo);

    return 0;
}
