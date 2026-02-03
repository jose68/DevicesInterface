#include <jni.h>
#include "ZebraPrinter.hpp"
#include "Scale.hpp"


static ZebraPrinter globalPrinter;

extern "C" {

    JNIEXPORT jboolean JNICALL Java_com_dhl_printer_PrinterService_printZplDirect(JNIEnv *env, jobject obj, jstring data) {
        if (data == NULL) return JNI_FALSE;

        const char* pData = env->GetStringUTFChars(data, NULL);
        std::string zplData(pData);
        
        bool success = globalPrinter.sendZplDirect(zplData);

        env->ReleaseStringUTFChars(data, pData);
        return success ? JNI_TRUE : JNI_FALSE;
    }

    JNIEXPORT jboolean JNICALL Java_com_dhl_printer_PrinterService_printZplSpooler(JNIEnv *env, jobject obj, jstring name, jstring data) {
        if (name == NULL || data == NULL) return JNI_FALSE;

        const jchar* pName = env->GetStringChars(name, NULL);
        const char* pData = env->GetStringUTFChars(data, NULL);

        std::wstring printerName((const wchar_t*)pName);
        std::string zplData(pData);

        bool success = globalPrinter.sendZplSpooler(printerName, zplData);

        env->ReleaseStringChars(name, pName);
        env->ReleaseStringUTFChars(data, pData);

        return success ? JNI_TRUE : JNI_FALSE;
    }

    JNIEXPORT jstring JNICALL Java_com_dhl_scale_ScaleService_nativeReadWeight(JNIEnv *env, jobject obj, jstring port, jint baud, jint data, jint stop, jint parity, jstring req, jint timeout)
    {
        if (port == NULL || req == NULL) return NULL;

        const char* pPort = env->GetStringUTFChars(port, NULL);
        const char* pReq = env->GetStringUTFChars(req, NULL);
        
        char buffer[256] = {0};

        Scale scale;

        int bytes = scale.readWeight(
            pPort, 
            (int)baud, 
            (int)data, 
            (int)stop, 
            (int)parity, 
            pReq, 
            buffer, 
            256, 
            (int)timeout
        );

        env->ReleaseStringUTFChars(port, pPort);
        env->ReleaseStringUTFChars(req, pReq);

        return (bytes > 0) ? env->NewStringUTF(buffer) : NULL;
    }
}