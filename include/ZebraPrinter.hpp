#ifndef ZEBRAPRINTER_HPP
#define ZEBRAPRINTER_HPP

#include <windows.h>
#include <setupapi.h>
#include <string>
#include <vector>
#include <algorithm>

class ZebraPrinter {
public:
    ZebraPrinter();
    bool sendZPL(const std::string& data);

private:
    GUID printerGUID;
    std::string cachedPath;

    std::string findDevicePath();
    bool tryWrite(const std::string& path, const std::string& data);
};

#endif