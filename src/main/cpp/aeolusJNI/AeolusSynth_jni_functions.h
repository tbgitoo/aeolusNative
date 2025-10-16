//
// Created by thoma on 16/03/2025.
//

#ifndef AEOLUSSYNTH_JNI_H
#define AEOLUSSYNTH_JNI_H


#include "../aeolusSynthesizer/Synthesizer/include/AeolusSynthesizer.h"

[[maybe_unused]] void sendMidiDataToAeolusSynth(uint8_t* data, int numBytes);

void initAeolusSynth();

void stopAeolusSynth();

void aeolus_synth_noteon( int chan, int key, int vel);

void aeolus_synth_noteoff( int chan, int key, int vel);


#endif //MIDI_SYNTH_NATIVE_LIB_H
