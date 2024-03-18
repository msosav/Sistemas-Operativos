#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

class AsciiConverter
{
public:
    AsciiConverter(const std::string &text) : text(text) {}

    std::vector<unsigned char> convertToBytes() const
    {
        std::vector<unsigned char> bytes;
        for (char c : text)
        {
            bytes.push_back(static_cast<unsigned char>(c));
        }
        return bytes;
    }

    void printBytes(const std::vector<unsigned char> &bytes)
    {
        std::cout << "Bytes: ";
        for (unsigned char byte : bytes)
        {
            std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    }

    void printBits(const std::vector<unsigned char> &bytes)
    {
        std::cout << "Bits: ";
        for (unsigned char byte : bytes)
        {
            std::bitset<8> bits(byte);
            std::cout << bits << " ";
        }
        std::cout << std::endl;
    }

private:
    std::string text; // Miembro de datos para almacenar el texto
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <nombre_archivo>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    std::cout << "Ingrese el texto: ";
    std::string inputText;
    std::getline(std::cin, inputText);

    AsciiConverter converter(inputText);
    std::vector<unsigned char> bytes = converter.convertToBytes();

    std::ofstream outputFile(fileName, std::ios::binary);
    if (!outputFile.is_open())
    {
        std::cerr << "Error: No se pudo abrir el archivo de salida." << std::endl;
        return 1;
    }

    converter.printBits(bytes);
    outputFile.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
    outputFile.close();

    std::cout << "Cadena de bytes generada y guardada en '" << fileName << "'." << std::endl;

    return 0;
}
