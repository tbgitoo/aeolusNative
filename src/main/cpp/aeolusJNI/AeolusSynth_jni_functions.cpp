#include <jni.h>
#include <unistd.h>
#include <__memory/unique_ptr.h>


#include "AeolusSynth_jni_functions.h"
#include "../aeolusSynthesizer/Synthesizer/include/AeolusOscillator.h"
#include "../aeolusSynthesizer/Synthesizer/include/AeolusSynthesizer.h"

#include "../midi_general/MidiSpec.h"

static std::unique_ptr< Aeolussynthesizer::AeolusSynthesizer> synth = nullptr;

static const char* privateStorageRoot = nullptr;

static Lfq_u32  note_queue (256);
static Lfq_u32  comm_queue (256);
static Lfq_u8   midi_queue (1024);

static bool self_test_running=false;



extern "C" [[maybe_unused]] JNIEXPORT void JNICALL Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_initAeolussynth
        ( JNIEnv* env, jclass) {
    initAeolusSynth();
    // Setup synthesizer
    //settings = new_JUCE_settings();
    //synth = new_JUCE_synth(settings);
    //adriver = new_JUCE_audio_driver(settings, synth);

}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL Java_com_mathis_aeolusnative_AeolusSynth_AeolusFileInstallation_nativeSetStorageRoot
        (JNIEnv* env, jclass,jstring privateStorageRootJava) {

    privateStorageRoot=env->GetStringUTFChars( privateStorageRootJava,nullptr);


    __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                        "AeolusSynth_jni", "Stops storaed at %s",privateStorageRoot);

    if(synth!= nullptr)
    {
        synth->setStopsPath(privateStorageRoot);
    }


    // Setup synthesizer
    //settings = new_JUCE_settings();
    //synth = new_JUCE_synth(settings);
    //adriver = new_JUCE_audio_driver(settings, synth);

}


// Make sure to set an appropriate storage path beforehand, that's where the stops live,
// without those, nothing works
void initAeolusSynth()
{
    if(synth==nullptr) {

        __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                            "initAeolusSynth", "First time initialization of synthesizer");

        synth = std::make_unique<Aeolussynthesizer::AeolusSynthesizer>(&note_queue, &comm_queue,
                                                                       &midi_queue,
                                                                       privateStorageRoot);
    }
    synth->play();
}


extern "C" [[maybe_unused]] JNIEXPORT void JNICALL Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_stopAeolussynth
        (JNIEnv* env, jclass) {

    __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                        "stopAeolusSynth", "JNI: Stop AeolusSynth");

    stopAeolusSynth();
    // Setup synthesizer
    //delete_JUCE_audio_driver(adriver);
    //delete_JUCE_synth(synth);
    //delete_JUCE_settings(settings);

    //adriver = 0;
    //synth = 0;
    //settings = 0;
}

void stopAeolusSynth()
{

    synth= nullptr;
}



extern "C" [[maybe_unused]] JNIEXPORT void JNICALL Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthNoteOn
        (JNIEnv* env, jclass m,jbyte channel, jbyte key, jbyte velocity)
{

    if(synth== nullptr)
    {
        initAeolusSynth();
    }



    synth->noteon(channel,key,velocity);


}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthNoteOff
        (JNIEnv* env, jclass m,jbyte channel, jbyte key, jbyte velocity)
{
    if(synth== nullptr)
    {
        initAeolusSynth();
    }


    synth->noteoff(channel,key,velocity);



}

extern "C" [[maybe_unused]] JNIEXPORT void JNICALL Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthTest
(JNIEnv* env, jclass m) {
    __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                        "AeolusSynthJNI", "Self-Test");
    if(synth== nullptr)
    {
        __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                            "AeolusSynthJNI", "Init");
        initAeolusSynth();
    }
    if(!self_test_running) {
        synth->activateRank(0, 0);
        synth->activateRank(0, 2);
        Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthNoteOn(env, m, 2,60,127);
        Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthNoteOn(env, m, 2,64,127);
        Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthNoteOn(env, m, 2,67,127);
        self_test_running=true; // start playing middle C,E,G
    } else {
        Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthNoteOff(env, m, 2,60,127);
        Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthNoteOff(env, m, 2,64,127);
        Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_AeolusSynthNoteOff(env, m, 2,67,127);
        self_test_running=false; // stop playing middle C,E,G
    }

}



void aeolus_synth_noteon( int chan, int key, int vel)
{
    if(synth== nullptr)
    {
        __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                            "AeolusSynthJNI", "Init");
        initAeolusSynth();
    }
    synth->noteon(chan,key,vel);
}

void aeolus_synth_noteoff( int chan, int key, int vel)
{
    if(synth== nullptr)
    {
        __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                            "AeolusSynthJNI", "Init");
        initAeolusSynth();
    }
    synth->noteoff(chan,key,vel);
}

 void sendMidiDataToAeolusSynth(uint8_t* data, int numBytes)
{
    if(numBytes>=2)
    {
        if(((data[0] & 0xF0) >> 4) ==  kMIDIChanCmd_NoteOn)
        {
            aeolus_synth_noteon( data[0] &0x0F, data[1], data[2]);
        }
        if(((data[0] & 0xF0) >> 4) ==  kMIDIChanCmd_NoteOff)
        {
            aeolus_synth_noteoff( data[0] &0x0F, data[1], data[2]);
        }
    }

}


extern "C"
[[maybe_unused]] JNIEXPORT jboolean JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_isInitializing(JNIEnv *env, jobject thiz) {
    return synth->isInitializing();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getNumberDivisions(JNIEnv *env,
                                                                            jclass clazz) {
    return synth->get_n_divisions();
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getLabelForDivision(JNIEnv *env,
                                                                             jclass clazz,
                                                                             jint index) {

    return env->NewStringUTF(synth->getLabelForDivision(index));

}
extern "C"
[[maybe_unused]] JNIEXPORT jint JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_get_1n_1StopsForDivision(JNIEnv *env,
                                                                                  jclass clazz,
                                                                                  jint index) {
    return synth->get_n_stops_for_division(index);
    // TODO: implement get_n_StopsForDivision()
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getStopLabel(JNIEnv *env, jclass clazz,
                                                                      jint index_division, jint index_stop) {
    // TODO: implement getStopLabel()
    return env->NewStringUTF(synth->getLabelForStop(index_division,index_stop));
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getStopActivated(JNIEnv *env, jclass clazz,
                                                                          jint index_division,
                                                                          jint index_stop) {
    return synth->getStopActivated(index_division, index_stop);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_activateStop(JNIEnv *env, jclass clazz,
                                                                      jint index_division,
                                                                      jint index_stop) {
    synth->activateStop(index_division,index_stop);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_deactivateStop(JNIEnv *env, jclass clazz,
                                                                        jint index_division,
                                                                        jint index_stop) {
    __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                        "AeolusSynth_jni_functions", "deactivate");

    synth->deactivateStop(index_division,index_stop);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getNumberMidiChannels(JNIEnv *env,
                                                                               jclass clazz) {
    return synth->get_midimap_length();
}

extern "C"
JNIEXPORT jbyte JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_queryMidiMap(JNIEnv *env, jclass clazz,
                                                                      jint midi_index) {

    return (jbyte)(synth->get_midi_map_entry(midi_index));
}
extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_setMidiMapping(JNIEnv *env, jclass clazz,
                                                                        jint my_division_index,
                                                                        jint my_midi_channel_index,
                                                                        jboolean is_checked) {
    synth->setMidiMapBit(my_division_index,my_midi_channel_index,is_checked);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getDivisionVolume(JNIEnv *env,
                                                                           jclass clazz,
                                                                           jint index) {
    return synth->getVolumeForDivision(index);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_setDivisionVolume(JNIEnv *env,
                                                                           jclass clazz,
                                                                           jint index,
                                                                           jfloat division_gain) {
    synth->setVolumeForDivision(index,division_gain);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_toggleTremulant(JNIEnv *env, jclass clazz,
                                                                         jint my_index) {
    if(synth->tremulantIsOn(my_index))
    {
        synth->deactivateTremulantForDivision(my_index);
    } else {
        synth->activateTremulantForDivision(my_index);
    }
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_hasTremulant(JNIEnv *env, jclass clazz,
                                                                      jint division_index) {
    return synth->division_has_tremulant(division_index);



}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_tremulantIsActive(JNIEnv *env,
                                                                           jclass clazz,
                                                                           jint division_index) {
    return synth->tremulantIsOn(division_index);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_get_1n_1tunings(JNIEnv *env,
                                                                         jclass clazz) {
    return synth->get_n_tunings();
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getTuningLabel(JNIEnv *env, jclass clazz,
                                                                        jint i) {
    return   env->NewStringUTF(synth->getTuningLabel(i));
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getCurrentTuning(JNIEnv *env,
                                                                          jclass clazz) {
   return synth->getCurrentTuning();
}
extern "C"
JNIEXPORT jfloat JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_getBaseFrequency(JNIEnv *env,
                                                                          jclass clazz) {
    return synth->getBaseFrequency();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_reTune(JNIEnv *env, jclass clazz,
                                                                jint temperament,
                                                                jfloat base_frequency) {
    synth->retune(temperament,base_frequency);
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_isRetuning(JNIEnv *env, jclass clazz) {
    return synth->is_retuning();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_panicoff(JNIEnv *env, jclass clazz) {
    // TODO: implement panicoff()
    for(int chan=0; chan<16;chan++) {
        for(int key=36; key<=96;key++)
        synth->noteoff(chan, key, 127);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_mathis_aeolusnative_AeolusSynth_AeolussynthManager_panicon(JNIEnv *env, jclass clazz) {
    // TODO: implement panicon()
    for(int chan=0; chan<16;chan++) {
        for(int key=36; key<=96;key++)
            synth->noteon(chan, key, 127);
    }
}

