#include <jni.h>
#include <GLES3/gl3.h>
#include <android/log.h>

#define LOG_TAG "DerbMBattle"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C"
JNIEXPORT void JNICALL
Java_com_derbmaroc_battle_MainActivity_nativeInit(JNIEnv* env, jobject thiz) {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    LOGI("Native Init Called");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_derbmaroc_battle_MainActivity_nativeRender(JNIEnv* env, jobject thiz) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
