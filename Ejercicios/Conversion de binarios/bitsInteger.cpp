#include <iostream>
#include <bitset>
#include <cstdint>
#include <string>
#include <sstream>

void mostrarBits(int numero)
{
    std::cout << "Bits: \n.|.........|.........|.........|\n"
              << std::bitset<sizeof(int) * 8>(numero) << std::endl;
}

std::string enteroABinario(int numero)
{
    std::stringstream ss;
    ss << std::bitset<sizeof(int) * 8>(numero);
    return ss.str();
}

int binarioAEntero(const std::string &binario)
{
    return std::bitset<sizeof(int) * 8>(binario).to_ulong();
}

int main()
{
    char opcion;
    std::cout << "¿Desea ingresar el número en formato entero (e) o binario (b)? ";
    std::cin >> opcion;

    if (opcion == 'e')
    {
        int numero;
        std::cout << "Ingrese un número entero: ";
        std::cin >> numero;
        mostrarBits(numero);
    }
    else if (opcion == 'b')
    {
        std::string binario;
        std::cout << "Ingrese el número en formato binario: \n.|.........|.........|.........|\n";
        std::cin >> binario;
        int numero = binarioAEntero(binario);
        std::cout << "El número entero correspondiente es: " << numero << std::endl;
    }
    else
    {
        std::cout << "Opción no válida." << std::endl;
    }

    return 0;
}
