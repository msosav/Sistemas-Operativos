#include <iostream>
#include <fstream>
#include <bitset>

class BinaryFileWriter
{
public:
    BinaryFileWriter(const std::string &fileName) : fileName(fileName)
    {
        outputFile.open(fileName, std::ios::binary | std::ios::out);
        if (!outputFile.is_open())
        {
            std::cerr << "Error: Unable to open file " << fileName << std::endl;
        }
    }

    ~BinaryFileWriter()
    {
        if (outputFile.is_open())
        {
            outputFile.close();
        }
    }

    void writeBinaryString(const std::string &binaryString)
    {
        /*
        std::bitset<8> byte; // Representa un byte (8 bits) y conforma un registro de 128 bytes
        for (size_t i = 0; i < binaryString.length(); i += 8) {
            std::string byteString = binaryString.substr(i, 8);
            byte = std::bitset<8>(byteString);
            outputFile.write(reinterpret_cast<const char*>(&byte), sizeof(byte));
        }
        //*/
        //========
        //*
        std::bitset<128> byte; // Representa un byte (8 bits) y conforma un registro de 128 bytes
        for (size_t i = 0; i < binaryString.length(); i += 128)
        {
            std::string byteString = binaryString.substr(i, 128);
            byte = std::bitset<128>(byteString);
            outputFile.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
        }
        //*/
    }

private:
    std::ofstream outputFile;
    std::string fileName;
};

int main()
{
    std::cout << "Ingrese la cadena binaria (solo 0s y 1s): ";
    std::string binaryInput;
    std::cin >> binaryInput;

    // Verificar que la cadena binaria solo contiene 0s y 1s
    if (binaryInput.find_first_not_of("01") != std::string::npos)
    {
        std::cerr << "Error: La cadena binaria solo debe contener 0s y 1s." << std::endl;
        return 1;
    }

    BinaryFileWriter fileWriter("output.bin");
    fileWriter.writeBinaryString(binaryInput);

    std::cout << "Archivo binario generado con éxito: output.bin" << std::endl;

    return 0;
}
