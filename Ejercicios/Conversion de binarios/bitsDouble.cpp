#include <iostream>
#include <bitset>
#include <cstdint>
#include <string>
#include <sstream>

void mostrarBits(double numero)
{
    union
    {
        double input;
        uint64_t output;
    } data;
    data.input = numero;

    // Mostrar todos los bits
    std::cout << "Todos los bits: "
              << "signo(1)|exponente(11)|mantisa(52):\nseeeeeeeee.emmmmmmmmm.mmmmmmmmm.mmmmmmmmm.mmmmmmmmm.mmmmmmmmm.mm\n"
              << std::bitset<64>(data.output) << std::endl;

    // Bits de signo
    std::cout << "Bits de signo: " << std::bitset<1>(data.output >> 63) << std::endl;

    // Bits de exponente
    std::cout << "Bits de exponente: " << std::bitset<11>((data.output >> 52) & 0x7FF) << std::endl;

    // Bits de mantisa
    std::cout << "Bits de mantisa: " << std::bitset<52>(data.output & 0xFFFFFFFFFFFFF) << std::endl;
}

double binarioADouble(const std::string &binario)
{
    // Convertir la cadena binaria a un número double
    std::bitset<64> bits(binario);
    union
    {
        uint64_t input;
        double output;
    } data;
    data.input = bits.to_ullong();
    return data.output;
}

int main()
{
    char opcion;
    std::cout << "¿Desea ingresar el número en formato double (d) o binario (b)? ";
    std::cin >> opcion;

    if (opcion == 'd')
    {
        double numero;
        std::cout << "Ingrese un número double: ";
        std::cin >> numero;
        mostrarBits(numero);
    }
    else if (opcion == 'b')
    {
        std::string binario;
        std::cout << "Ingrese el número en formato binario (64 bits) :: signo(1)|exponente(11)|mantisa(52):\nseeeeeeeee.emmmmmmmmm.mmmmmmmmm.mmmmmmmmm.mmmmmmmmm.mmmmmmmmm.mm \n";
        std::cin >> binario;
        double numero = binarioADouble(binario);
        std::cout << "El número double correspondiente es: " << numero << std::endl;
    }
    else
    {
        std::cout << "Opción no válida." << std::endl;
    }

    return 0;
}
