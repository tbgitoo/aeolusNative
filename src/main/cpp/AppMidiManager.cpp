/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Thomas Braschler and Mathis Braschler, 2025:
 * Modified to fit the needs of this project, in particular, removed midi output part
 * and implemented transmission of the incoming messages to the Aeolus synthesizer
 *
 */
#include <cinttypes>
#include <jni.h>
#include <pthread.h>
#include <cstdio>
#include <unistd.h>

#include <atomic>
#include <string>


#include <amidi/AMidi.h>


#include "midi_general/MidiSpec.h"

#include "aeolusJNI/AeolusSynth_jni_functions.h"



// AMidi device, provided by Java and retained here
static AMidiDevice* sNativeReceiveDevice = NULL;
// The thread only reads this value, so no special protection is required.
static AMidiOutputPort* sMidiOutputPort = NULL;

// Midi reading thread, works natively and independently on Android once correctly configured and
// launched
static pthread_t sReadThread;
// are we currently reading midi messages with the midi thread
static std::atomic<bool> sReading(false);



// Java virtual machine for callbacks
JavaVM* theJvm;
// This represents the Java AppMidiManager
jobject dataCallbackObj;
// Callback on the java side, to be invoked when we recieve midi messages here, on the native side
// Configured to represent the onNativeMessageReceive method of AppMidiManager
jmethodID midDataCallback;

/**
 * This method is invoked when the native amidi implementation receives a midi message from a hardware port.
 * The role of this function is to invoke,
 * via the jni interface, the Java AppMidiManager.onNativeMessageReceive routine to transmit to Android the
 * received data. In this way, Android is aware of incoming hardware midi messages and can show or otherwise
 * process them (or also ignore them if not needed; the native midi treatment of hardware midi messages is independent
 * on the Android part).
 * @param data Raw data array
 * @param numBytes Number of bytes in the raw data array
 */
static void SendTheReceivedData(uint8_t* data, int numBytes) {
    JNIEnv* env;
    theJvm->AttachCurrentThread(&env, NULL);
    if (env == NULL) {
        __android_log_print(android_LogPriority::ANDROID_LOG_ERROR,
                            "AppMidiManager", "SendTheReceivedData: Error retrieving JNI Env");

    }

    // Allocate the Java array and fill with received data
    jbyteArray ret = env->NewByteArray(numBytes);
    env->SetByteArrayRegion(ret, 0, numBytes, (jbyte*)data);

    // send it to the (Java) callback
    env->CallVoidMethod(dataCallbackObj, midDataCallback, ret);


}

#if 0
// unblock this method if logging of the midi messages is required.
/**
 * Formats a midi message set and outputs to the log
 * @param   timestamp   The timestamp for when the message(s) was received
 * @param   dataBytes   The MIDI message bytes
 * @params  numDataBytew    The number of bytes in the MIDI message(s)
 */
static void logMidiBuffer(int64_t timestamp, uint8_t* dataBytes, size_t numDataBytes) {
#define DUMP_BUFFER_SIZE 1024
    char midiDumpBuffer[DUMP_BUFFER_SIZE];
    memset(midiDumpBuffer, 0, sizeof(midiDumpBuffer));
    int pos = snprintf(midiDumpBuffer, DUMP_BUFFER_SIZE,
            "%" PRIx64 " ", timestamp);
    for (uint8_t *b = dataBytes, *e = b + numDataBytes; b < e; ++b) {
        pos += snprintf(midiDumpBuffer + pos, DUMP_BUFFER_SIZE - pos,
                "%02x ", *b);
    }
    LOGD("%s", midiDumpBuffer);
}
#endif

/*
 * Receiving API
 */
/**
 * This routine polls the input port and dispatches received data to
 *  - the application-provided (Java) callback.
 *  - directly to the Aeolus synthesizer
 */
static void* readThreadRoutine(void* context) {
    (void)context;  // unused

    sReading = true;
    // AMidiOutputPort* outputPort = sMidiOutputPort.load();
    AMidiOutputPort* outputPort = sMidiOutputPort;

    const size_t MAX_BYTES_TO_RECEIVE = 128;
    uint8_t incomingMessage[MAX_BYTES_TO_RECEIVE];

    while (sReading) {
        // AMidiOutputPort_receive is non-blocking, so let's not burn up the CPU
        // unnecessarily
        usleep(2000);

        int32_t opcode;
        size_t numBytesReceived;
        int64_t timestamp;
        ssize_t numMessagesReceived = AMidiOutputPort_receive(
                outputPort, &opcode, incomingMessage, MAX_BYTES_TO_RECEIVE,
                &numBytesReceived, &timestamp);

        if (numMessagesReceived < 0) {
            __android_log_print(android_LogPriority::ANDROID_LOG_WARN,
                                "AppMidiManager", "readThreadRoutine: Failure receiving MIDI data %zd", numMessagesReceived);

            // Exit the thread
            sReading = false;
        }
        if (numMessagesReceived > 0 && numBytesReceived >= 0) {
            if (opcode == AMIDI_OPCODE_DATA &&
                (incomingMessage[0] & kMIDISysCmdChan) != kMIDISysCmdChan) {
                // (optionally) Dump to log
                // Here the received data will need to be sent to the Aeolus synthesizer

                sendMidiDataToAeolusSynth(incomingMessage, numBytesReceived);

                SendTheReceivedData(incomingMessage, numBytesReceived);
            } else if (opcode == AMIDI_OPCODE_FLUSH) {
                // ignore
            }
        }
    }  // end while(sReading)

    return NULL;
}

//
// JNI Functions
//
extern "C" {

/**
 * Native implementation of AppMidiManager.startReadingMidi() method.
 * Opens the first "output" port from specified MIDI device for sReading.
 * @param   env  JNI Env pointer.
 * @param   (unnamed)   AppMidiManager (Java) object.
 * @param   midiDeviceObj   (Java) MidiDevice object.
 * @param   portNumber      The index of the "output" port to open.
 */
void Java_com_mathis_midiSynth_hardwareMidi_AppMidiManager_startReadingMidi(
        JNIEnv* env, jobject, jobject midiDeviceObj, jint portNumber) {
    AMidiDevice_fromJava(env, midiDeviceObj, &sNativeReceiveDevice);
    // int32_t deviceType = AMidiDevice_getType(sNativeReceiveDevice);
    // ssize_t numPorts = AMidiDevice_getNumOutputPorts(sNativeReceiveDevice);

    AMidiOutputPort* outputPort;
    AMidiOutputPort_open(sNativeReceiveDevice, portNumber, &outputPort);

    // sMidiOutputPort.store(outputPort);
    sMidiOutputPort = outputPort;

    // Start read thread
    // pthread_init(true);
    /*int pthread_result =*/pthread_create(&sReadThread, NULL, readThreadRoutine,
                                           NULL);
}

/**
 * Native implementation of the (Java) AppMidiManager.stopReadingMidi() method.
 * @param   (unnamed)   JNI Env pointer.
 * @param   (unnamed)   TBMidiManager (Java) object.
 */
void Java_com_mathis_midiSynth_hardwareMidi_AppMidiManager_stopReadingMidi(JNIEnv*,
                                                                jobject) {
    // need some synchronization here
    sReading = false;
    pthread_join(sReadThread, NULL);

    /*media_status_t status =*/AMidiDevice_release(sNativeReceiveDevice);
    sNativeReceiveDevice = NULL;
}



/**
 * Native implementation of the AppMidiManager.initNative method
 * Initializes JNI interface stuff, specifically the info needed to call back
 * into the Java layer when MIDI data is received.
 */
JNICALL void Java_com_mathis_midiSynth_hardwareMidi_AppMidiManager_initNative(
        JNIEnv* env, jobject instance) {
    env->GetJavaVM(&theJvm);

    // Setup the receive data callback (into Java)
    jclass clsAppMidiManager =
            env->FindClass("com/mathis/midiSynth/hardwareMidi/AppMidiManager");
    dataCallbackObj = env->NewGlobalRef(instance);
    midDataCallback =
            env->GetMethodID(clsAppMidiManager, "onNativeMessageReceive", "([B)V");
}





}  // extern "C"
