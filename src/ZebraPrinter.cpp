#include "ZebraPrinter.hpp"

ZebraPrinter::ZebraPrinter() {
    // GUID standard pour les interfaces d'impression USB Windows
    printerGUID = {0x28d4583d, 0x0606, 0x11d2, {0xb0, 0x3a, 0x00, 0x60, 0x97, 0x09, 0x53, 0x02}};
    cachedPath = "";
}

// Fonction de bas niveau pour trouver le chemin matériel de l'imprimante
std::string ZebraPrinter::findDevicePath() {
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&printerGUID, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) return "";

    SP_DEVICE_INTERFACE_DATA devIntfData;
    devIntfData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    std::string path = "";

    if (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &printerGUID, 0, &devIntfData)) {
        DWORD size = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &devIntfData, NULL, 0, &size, NULL);
        std::vector<char> buffer(size);
        auto detailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(buffer.data());
        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &devIntfData, detailData, size, NULL, NULL)) {
            path = detailData->DevicePath;
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return path;
}

// Tentative d'écriture avec timeouts pour ne pas bloquer Java
bool ZebraPrinter::tryWrite(const std::string& path, const std::string& data) {
    HANDLE hFile = CreateFileA(path.c_str(), 
                               GENERIC_WRITE, 
                               FILE_SHARE_READ | FILE_SHARE_WRITE, 
                               NULL, 
                               OPEN_EXISTING, 
                               FILE_ATTRIBUTE_NORMAL, 
                               NULL);

    if (hFile == INVALID_HANDLE_VALUE) return false;

    // Sécurité : Timeout de 500ms pour éviter de geler l'app si l'USB bugge
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.WriteTotalTimeoutConstant = 500; 
    SetCommTimeouts(hFile, &timeouts);

    DWORD written;
    bool success = WriteFile(hFile, data.c_str(), (DWORD)data.length(), &written, NULL);
    
    // On valide que tout a été envoyé
    if (success && written != data.length()) success = false;

    CloseHandle(hFile);
    return success;
}

// Logique principale : Essai direct -> si échec -> scan -> essai final
bool ZebraPrinter::sendZPL(const std::string& data) {
    if (!cachedPath.empty()) {
        if (tryWrite(cachedPath, data)) return true;
    }

    // Le cache est mort ou vide, on rescane le bus USB
    cachedPath = findDevicePath();
    if (!cachedPath.empty()) {
        return tryWrite(cachedPath, data);
    }

    return false;
}