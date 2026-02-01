#include "ZebraPrinter.hpp"

ZebraPrinter::ZebraPrinter() {
    // GUID standard pour les périphériques d'impression USB
    printerGUID = {0x28d4583d, 0x0606, 0x11d2, {0xb0, 0x3a, 0x00, 0x60, 0x97, 0x09, 0x53, 0x02}};
    cachedPath = "";
}

std::string ZebraPrinter::findDevicePath() {
    // 1. Récupérer la liste de tous les périphériques d'impression USB connectés
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&printerGUID, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) return "";

    SP_DEVICE_INTERFACE_DATA devIntfData;
    devIntfData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    std::string path = "";

    // 2. Parcourir la liste pour trouver spécifiquement une Zebra (VID 0a5f)
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &printerGUID, i, &devIntfData); i++) {
        DWORD size = 0;
        // Demander la taille requise pour les détails du périphérique
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &devIntfData, NULL, 0, &size, NULL);
        
        if (size > 0) {
            std::vector<char> buffer(size);
            auto* detailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(buffer.data());
            detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            // Récupérer le chemin réel (ex: \\?\usb#vid_0a5f&pid_00fb#...)
            if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &devIntfData, detailData, size, NULL, NULL)) {
                std::string currentPath = detailData->DevicePath;
                
                // Conversion en minuscule pour un filtrage fiable
                std::string lowerPath = currentPath;
                for(char &c : lowerPath) c = (char)tolower((unsigned char)c);

                // FILTRE : On vérifie si c'est bien une Zebra (Vendor ID : 0a5f)
                if (lowerPath.find("vid_0a5f") != std::string::npos) {
                    path = currentPath;
                    break; // On a trouvé notre Zebra, on arrête la boucle
                }
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return path;
}

bool ZebraPrinter::tryWrite(const std::string& path, const std::string& data) {
    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) return false;

    // Timeout de sécurité pour ne pas bloquer l'app si l'imprimante sature
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.WriteTotalTimeoutConstant = 500; 
    SetCommTimeouts(hFile, &timeouts);

    DWORD written;
    bool success = WriteFile(hFile, data.c_str(), (DWORD)data.length(), &written, NULL);
    
    CloseHandle(hFile);
    return success && (written == data.length());
}

bool ZebraPrinter::sendZPL(const std::string& data) {
    // On essaie le chemin en cache d'abord (très rapide)
    if (!cachedPath.empty() && tryWrite(cachedPath, data)) return true;

    // Si ça échoue, on rescane (cas où l'imprimante a été débranchée/rebranchée)
    cachedPath = findDevicePath();
    if (!cachedPath.empty()) {
        return tryWrite(cachedPath, data);
    }

    return false;
}