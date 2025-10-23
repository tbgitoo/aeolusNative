//
// Created by thoma on 15/10/2025.
//

#ifndef MIDI_SYNTH_ANDROID_AEOLUS_USER_INTERFACE_JNI_H
#define MIDI_SYNTH_ANDROID_AEOLUS_USER_INTERFACE_JNI_H

/**
 * @file android_aeolus_user_interface_jni.h
 * @brief JNI functions that are called from the native side
 *
 * This file declares functions that are called from the native C++ code
 * to notify the Java layer of events in the Aeolus synthesizer. This
 * is primarily the completion of tasks such as loading, updating stops and
 * retuning that take a long time and are run in the background before
 * their completion is notified back to the Java layer.
 */

/**
 * @brief Called when the stops of the Aeolus synthesizer have been updated.
 *
 * This function is called from the native code to signal to the Java
 * application that the stops have been updated, meaning that the wavetables
 * have been loaded or calculated according to the current settings.
 */
void AndroidAeolusUserInterfaceonStopsUpdated();

/**
 * @brief Called when the Aeolus synthesizer has been retuned.
 *
 * This function is called from the native code to signal to the Java
 * application that the synthesizer has been retuned, i.e. that the
 * wavetables have been recalculated to reflect the new tuning settings.
 */
void AndroidAeolusUserInterfaceonRetuned();

/**
 * @brief Called when the loading of the Aeolus synthesizer is complete.
 *
 * This function is called from the native code to signal to the Java
 * application that the synthesizer has finished its initial loading.
 */
void AndroidAeolusUserInterfaceOnLoadComplete();

#endif //MIDI_SYNTH_ANDROID_AEOLUS_USER_INTERFACE_JNI_H
