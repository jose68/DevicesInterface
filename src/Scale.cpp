#include "Scale.hpp"
#include <cstdio>

int Scale::readWeight(
    const std::string& port,
    int baudrate,
    int dataBits,
    int stopBits,
    int parity,
    const std::string& request,
    char* output,
    int maxLen,
    int timeoutMs) 
{
    if (port.empty() || request.empty() || !output) return -1;

    // Ouverture du port COM (ex: "COM1")
    HANDLE hSerial = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, 
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    
    if (hSerial == INVALID_HANDLE_VALUE) return -2;

    // Configuration des paramètres série (DCB)
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    if (GetCommState(hSerial, &dcb)) {
        dcb.BaudRate = baudrate;
        dcb.ByteSize = (BYTE)dataBits;
        dcb.StopBits = (stopBits == 2) ? TWOSTOPBITS : ONESTOPBIT;
        dcb.Parity = (BYTE)parity;
        dcb.fParity = (parity > 0);
        SetCommState(hSerial, &dcb);
    }

    // Configuration des timeouts
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadTotalTimeoutConstant = timeoutMs;
    timeouts.ReadIntervalTimeout = 50; 
    SetCommTimeouts(hSerial, &timeouts);

    // Nettoyage des buffers
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);

    // Envoi de la requête (on ajoute \r\n car les balances attendent souvent une fin de ligne)
    DWORD written;
    char cmd[128];
    int cmdLen = _snprintf(cmd, sizeof(cmd), "%s\r\n", request.c_str());
    if (!WriteFile(hSerial, cmd, (DWORD)cmdLen, &written, NULL)) {
        CloseHandle(hSerial);
        return -3;
    }

    // Lecture de la réponse
    DWORD bytesRead = 0;
    if (ReadFile(hSerial, output, (DWORD)maxLen - 1, &bytesRead, NULL) && bytesRead > 0) {
        output[bytesRead] = '\0'; // Null-terminator pour la chaîne C
    }

    CloseHandle(hSerial);
    return (int)bytesRead;
}