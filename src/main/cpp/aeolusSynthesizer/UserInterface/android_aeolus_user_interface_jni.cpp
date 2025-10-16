//
// Created by thoma on 15/10/2025.
//

/* JNI stuff */

/**
 * Initializes JNI interface stuff, specifically the info needed to call back
 * into the Java layer when MIDI data is received.
 */
#include <jni.h>
#include <android/log.h>

// The Data Callback
JavaVM* theJvmUserInterface;
jmethodID callbackAeolusReady;
jmethodID callbackStopsUpdated;
jmethodID callbackRetuned;
jclass AeolusUserInterfaceManagerClass;



extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_userInterface_AeolusUIManager_initNative(
        JNIEnv* env, [[maybe_unused]] jclass theClass) {
env->GetJavaVM(&theJvmUserInterface);


// Setup the receive data callback (into Java)

AeolusUserInterfaceManagerClass = static_cast<jclass>(env->NewGlobalRef(
        (jobject) env->FindClass(
                "com/mathis/aeolusnative/userInterface/AeolusUIManager")));

callbackAeolusReady =
env ->GetStaticMethodID(AeolusUserInterfaceManagerClass, "onLoadComplete", "()V");

callbackStopsUpdated=
env ->GetStaticMethodID(AeolusUserInterfaceManagerClass,"onStopsUpdated","()V");

callbackRetuned=
env ->GetStaticMethodID( AeolusUserInterfaceManagerClass, "onRetuned", "()V");
}


void AndroidAeolusUserInterfaceOnLoadComplete() {
    JNIEnv* env;
    theJvmUserInterface->AttachCurrentThread(&env, NULL);
    if (env == NULL) {
        __android_log_print(android_LogPriority::ANDROID_LOG_ERROR,
                            "android_aeolus_user_interface", "Error retrieving JNI Env");
    }


    // Signal to the Java application that the loading part of the Aeolus functions is complete
    env->CallStaticVoidMethod(AeolusUserInterfaceManagerClass, callbackAeolusReady);


}

void AndroidAeolusUserInterfaceonStopsUpdated() {
    JNIEnv* env;
    theJvmUserInterface->AttachCurrentThread(&env, NULL);
    if (env == NULL) {
        __android_log_print(android_LogPriority::ANDROID_LOG_ERROR,
                            "android_aeolus_user_interface", "Error retrieving JNI Env");
    }



    // Signal to the Java application that the loading part of the Aeolus functions is complete
    env->CallStaticVoidMethod(AeolusUserInterfaceManagerClass, callbackStopsUpdated);


}

void AndroidAeolusUserInterfaceonRetuned()
{
    JNIEnv* env;
    theJvmUserInterface->AttachCurrentThread(&env, NULL);
    if (env == NULL) {
        __android_log_print(android_LogPriority::ANDROID_LOG_ERROR,
                            "android_aeolus_user_interface", "Error retrieving JNI Env");
    }

    env->CallStaticVoidMethod(AeolusUserInterfaceManagerClass, callbackRetuned);

}
