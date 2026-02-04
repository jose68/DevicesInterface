#ifndef ZEBRAPRINTER_HPP
#define ZEBRAPRINTER_HPP

#include <string>
#include <windows.h>

class ZebraPrinter {
public:
    ZebraPrinter();
    ~ZebraPrinter() = default;

    bool sendZplDirect(const std::string& data);
    bool sendZplSpooler(const std::wstring& printerName, const std::string& data);
private:
    std::string cachedPath;
    std::string findDevicePath();
    std::string getInterfacePath();
    bool tryWrite(const std::string& path, const std::string& data);
};

#endif