#ifndef SCALE_HPP
#define SCALE_HPP

#include <windows.h>
#include <string>

class Scale {
public:
    Scale() = default;
    ~Scale() = default;

    /**
     * Lit le poids depuis la balance via RS232
     * @return Le nombre d'octets lus, ou un code d'erreur négatif
     */
    int readWeight(
        const std::string& port,
        int baudrate,
        int dataBits,
        int stopBits,
        int parity,
        const std::string& request,
        char* output,
        int maxLen,
        int timeoutMs
    );
};

#endif