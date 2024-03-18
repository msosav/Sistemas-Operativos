#include <iostream>
#include <bitset>
#include <string>

void mostrarBitsPorLetras(const std::string &texto)
{
    for (char c : texto)
    {
        std::bitset<8> bits(c);
        std::cout << bits << " ";
    }
    std::cout << std::endl;
}

std::string textoDesdeBits(const std::string &bits)
{
    std::string texto;
    for (size_t i = 0; i < bits.size(); i += 8)
    {
        std::bitset<8> byte(bits.substr(i, 8));
        texto += static_cast<char>(byte.to_ulong());
    }
    return texto;
}

int main()
{
    char opcion;
    std::cout << "¿Desea ingresar un texto (t) o una serie de bits (b)? ";
    std::cin >> opcion;

    if (opcion == 't')
    {
        std::string texto;
        std::cout << "Ingrese un texto: ";
        std::cin.ignore(); // Ignorar el salto de línea después de la entrada anterior
        std::getline(std::cin, texto);

        std::cout << "Representación en bits de cada letra:" << std::endl;
        mostrarBitsPorLetras(texto);
    }
    else if (opcion == 'b')
    {
        std::string bits;
        std::cout << "Ingrese una serie de bits: ";
        std::cin >> bits;

        std::string texto = textoDesdeBits(bits);
        std::cout << "Texto correspondiente: " << texto << std::endl;
    }
    else
    {
        std::cout << "Opción no válida." << std::endl;
    }

    return 0;
}
