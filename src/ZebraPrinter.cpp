#include "ZebraPrinter.hpp"
#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <initguid.h>
#include <devguid.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <winspool.h>

// GUID standard pour l'interface USBPRINT
DEFINE_GUID(GUID_DEVINTERFACE_USBPRINT, 0x28d78fad, 0x5a12, 0x11d1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);

ZebraPrinter::ZebraPrinter() : cachedPath("") {}


bool ZebraPrinter::tryWrite(const std::string& path, const std::string& data) {
    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD written;
    bool success = WriteFile(hFile, data.c_str(), (DWORD)data.length(), &written, NULL);
    
    CloseHandle(hFile);
    return success && (written == data.length());
}

std::string ZebraPrinter::getInterfacePath() {
    unsigned long listSize = 0;
    
    // GUID de classe d'interface USB brut
    // GUID_DEVINTERFACE_USB_DEVICE {A5DCBF10-6530-11D2-901F-00C04FB951ED}
    static const GUID GUID_USB_DEVICE = {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};
    CM_Get_Device_Interface_List_SizeA(&listSize, (LPGUID)&GUID_USB_DEVICE, NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
    
    if (listSize > 1) {
        std::vector<char> buffer(listSize);
        if (CM_Get_Device_Interface_ListA((LPGUID)&GUID_USB_DEVICE, NULL, buffer.data(), listSize, CM_GET_DEVICE_INTERFACE_LIST_PRESENT) == CR_SUCCESS) {
            char* currentStr = buffer.data();
            while (*currentStr != '\0') {
                std::string path = currentStr;
                std::string lowerPath = path;
                for(char &c : lowerPath) c = (char)tolower((unsigned char)c);

                if (lowerPath.find("vid_0a5f") != std::string::npos) {
                    return path; 
                }
                currentStr += strlen(currentStr) + 1;
            }
        }
    }
    
    return "";
}

std::string ZebraPrinter::findDevicePath() {

    std::string finalPath = "";
    // GUID de classe d'interface USBPRINT
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USBPRINT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    
    if (hDevInfo != INVALID_HANDLE_VALUE) {
        SP_DEVICE_INTERFACE_DATA interfaceData;
        interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_USBPRINT, i, &interfaceData); i++) {
            DWORD size = 0;
            SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, NULL, 0, &size, NULL);

            PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(size);
            detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

            if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, detailData, size, NULL, NULL)) {
                std::string path = detailData->DevicePath; // Ex: \\?\usb#vid_0a5f&pid_0173...
                
                if (path.find("vid_0a5f") != std::string::npos) {
                    finalPath = path;
                    free(detailData);
                    break;
                }
            }
            free(detailData);
        }
        SetupDiDestroyDeviceInfoList(hDevInfo);
    }

    if (finalPath.empty()) {
        finalPath = getInterfacePath();
    }
    
    return finalPath;
}

bool ZebraPrinter::sendZplDirect(const std::string& data) {
    if (!cachedPath.empty() && tryWrite(cachedPath, data)) {
        return true;
    }

    cachedPath.clear();

    cachedPath = findDevicePath();
    if (!cachedPath.empty()) {
        return tryWrite(cachedPath, data);
    }

    return false;
}

bool ZebraPrinter::sendZplSpooler(const std::wstring& printerName, const std::string& data) {
    HANDLE hPrinter = NULL;
    DOC_INFO_1W di = { (LPWSTR)L"Zebra Print Job", NULL, (LPWSTR)L"RAW" };
    DWORD dwBytesWritten = 0;
    BOOL success = FALSE;

    if (OpenPrinterW((LPWSTR)printerName.c_str(), &hPrinter, NULL)) {
        if (StartDocPrinterW(hPrinter, 1, (LPBYTE)&di)) {
            if (StartPagePrinter(hPrinter)) {
                success = WritePrinter(hPrinter, (LPVOID)data.c_str(), (DWORD)data.length(), &dwBytesWritten);
                EndPagePrinter(hPrinter);
            }
            EndDocPrinter(hPrinter);
        }
        ClosePrinter(hPrinter);
    }
    return (success && dwBytesWritten == data.length());
}