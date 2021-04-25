#include <jni.h>
#include <string.h>
#include <android/log.h>   //For logging native C code info to the logcat
#include "ds3231.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "JNI CODE", __VA_ARGS__))


JNIEXPORT jstring JNICALL
Java_net_uttersense_libds3231_DS3231Manager_stringFromJNI(JNIEnv *env, jobject thiz) {

    char mes[200];
    strcpy(mes,jni_test());
    LOGI("In stringFromJNI");

    //char mes[100];
    //strcpy(mes, "Hello from the JNI! - running the C version");
    return (*env)->NewStringUTF(env, mes);

}


JNIEXPORT jint JNICALL
Java_net_uttersense_libds3231_DS3231Manager_initialise(JNIEnv *env, jobject thiz) {

    LOGI("In initialise");

    //Get the JNI Environment:
    glo_env = env;
    LOGI("Local value of JNIEnv pointer: 0x%x", env);
    LOGI("Global value of JNIEnv pointer: 0x%x", glo_env);


    //Get the Java class object and set the JNI parameters:
    jclass clazz = (*env)->GetObjectClass(env,thiz);
    //Get the required iD forJava method callback:
    //jmethodID mid_cb = (*env)->GetMethodID(env,clazz,"updateTextView", "(Ljava/lang/String;)V");

    //Create global variable for class:
    glo_clazz = (*env)->NewGlobalRef(env,clazz);
    glo_obj = (*env)->NewGlobalRef(env,thiz);

    jint rs = (*env)->GetJavaVM(env, &JVM);
    if(rs == JNI_OK)
    {
        LOGI("Captured the JVM as global OK!");
    }


    //Initialise shared library for JNI callback attributes:
    char *callback_name = "updateStrData";
    char *callback_sig = "(Ljava/lang/String;)V";
    init_callbacks(callback_name,callback_sig);

    //initialise device driver
    jint val = init_dev();
    LOGI("JNI Version %d", (*env)->GetVersion(env));
    //Returns 65543 -> 1.6

    LOGI("Value returned from init_dev(): %d", val);
    return val;

}

JNIEXPORT void JNICALL
Java_net_uttersense_libds3231_DS3231Manager_close(JNIEnv *env, jobject thiz) {

    LOGI("In close");
    close_dev();
    LOGI("After close");

    //Delete global references:
    (*env)->DeleteGlobalRef(env,glo_clazz);
    (*env)->DeleteGlobalRef(env,glo_obj);

}

JNIEXPORT void JNICALL
Java_net_uttersense_libds3231_DS3231Manager_startDataFeed(JNIEnv *env, jobject thiz, jint interval) {

    LOGI("Starting timer for DS3231 data feed");
    start_timer((int) interval);



}

JNIEXPORT void JNICALL
Java_net_uttersense_libds3231_DS3231Manager_stopDataFeed(JNIEnv *env, jobject thiz) {

    LOGI("Stopping DS3231 data feed");
    stop_timer();

}

JNIEXPORT void JNICALL
Java_net_uttersense_libds3231_DS3231Manager_setTimerInt(JNIEnv *env, jobject thiz, jint interval) {

    LOGI("Settt the ds3231 timer interval");
    set_timer_int((int) interval);

}

#ifdef __cplusplus
}
#endif
