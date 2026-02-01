#include <jni.h>
#include "ZebraPrinter.hpp"

// Instance unique persistante
static ZebraPrinter globalPrinter;

extern "C" {

JNIEXPORT jboolean JNICALL Java_com_votreprojet_ZebraNative_printZPL(JNIEnv *env, jobject obj, jstring zplData) {
    if (zplData == NULL) return JNI_FALSE;

    const char *nativeString = env->GetStringUTFChars(zplData, 0);
    bool result = globalPrinter.sendZPL(nativeString);
    
    env->ReleaseStringUTFChars(zplData, nativeString);
    return (jboolean)result;
}

}