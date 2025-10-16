// ----------------------------------------------------------------------------
//
//  Copyright (C) 2025 Thomas and Mathis Braschler <thomas.braschler@gmail.com>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------

package com.mathis.aeolusnative.AeolusSynth;

/**
 * AeolussynthManager implements calls from the Java part of the application to the native part via
 * jni native calls. The corresponding C-routines are in AeolusSynth_jni_functions cpp, and
 * generally consists in corresponding calls to the synth object, which is the main
 * object orchestrating the Aeolus C implementation, and is of type AeolusSynthesizer.
 * <br /><br />
 * Note that this class is only for Java-to-C method invocation. This covers most of the
 * communication between the Android Java application and the Aeolus C implementation. However, there
 * are in some circumstances asynchronous callbacks where the timing originates from the C implementation.
 * In this case, the invocation is oppose, from C to Java. This direction is handled by a different class,
 * namely AeolusUIManager.
 */
public class AeolussynthManager {
    // Load the required part of the C implementation
    static {
        System.loadLibrary("AeolusAndroid");
    }

    /** Initialization of the Aeolus synthesizer. This should be the first call to this class,
     * typically during Android activity initialization. Here, this is invoked in the initMidiAeolus
     * method defined in baseAeolusMidiActivity
     */
    public static native void initAeolussynth();

    /**
     * Check whether the C part of Aeolus is still initializing
     * @return True when initializing, false once initialization is completed
     */
    public static native boolean isInitializing();

    /**
     * Turn note on.
     * @param channel Midi channel (numbered 0-15)
     * @param key Midi key (60 is middle C)
     * @param velocity Volume of key
     */
    public static native void AeolusSynthNoteOn(byte channel, byte key, byte velocity);
    /**
     * Turn note off.
     * @param channel Midi channel (numbered 0-15)
     * @param key Midi key (60 is middle C)
     * @param velocity Volume of key
     */

    public static native void AeolusSynthNoteOff(byte channel, byte key, byte velocity);


    /** Possiblity to stop and release Aeolus entire Aeolus machinery. Not used at present,
     * Android takes care of stopping the activities depending on user and device actions
     */
    public static native void stopAeolussynth();

    /**
     * Test routine to see if there is sound produced independently of overall settings
     */
    public static native void AeolusSynthTest();

    /** Number of Aeolus rank divisions.
     * The number of divisions is configured through the "definition" file in the "Aeolus" instrument
     * directory
     * @return Returns the number of divisions configured for the Aeolus instrument
     */
    public static native int getNumberDivisions();

    /**
     * Get the label defined for a given division
     * @param index The index of the division
     * @return The label of the division to show in the user interface
     */

    public static native String getLabelForDivision(int index);

    /**
     * Number of steps defined for division. The number of stops defines the number of on/off
     * stop elements to be shown in the user interface
     * <br /><br />
     * There are a number of ranks (pipe registers) in a division, but some of these
     * ranks are used as mixtures. To include the mixtures, the notion of stops is used:
     * a stop corresponds to a on/off knob in the user interface; in some cases,
     * this corresponds to a single rank (=simulation unit=register), some of these
     * are predefined mixtures.
     * @param index The index of the division
     * @return The number of stops defined for this division
     */

    public static native int get_n_StopsForDivision(int index);

    /**
     * Obtain the stop label.
     * <br /><br />
     * The indexation via index_stop is based on the Java User Interface, that is, the
     * stops are consecutively numbered. Note, a stop is a on/off button in the user interface,
     * corresponding to either pure
     * ranks or a pre-defined mixture<br />
     * This function gets the stop lables as configured in the C implementation of Aeolus, which means
     * ultimately from the "definition" instrument definition file.
     * @param index_division Index of the division the stop belongs to
     * @param index_stop Index of the stop among the step elements shown
     * @return The label of the stop
     */

    public static native String getStopLabel(int index_division, int index_stop);

    /**
     * Is the stop activated?
     * @param index_division Index of the division the stop belongs to
     * @param index_stop Index of the stop among the step elements shown
     * @return True when the stop is currently activated, false if not. Also returns
     * false if the index_stop variable does not correspond to an existing stop
     */
    public static native boolean getStopActivated(int index_division, int index_stop);

    /**
     * Activate a stop, that is, have it play incoming notes (provided some other conditions, such
     * as having the midi channel routed to the division in which the stop resides).
     * <br /><br />
     * Stops are activated in asynchronous, non-blocking fashion in order not to interfere
     * with audio processing. This means that upon return of this function, the stop
     * itself might not be activated yet. The activation of the stop will generate a synchronization
     * callback. To be precise, once the stop is activated, AeolusUIManager.onStopsUpdated()
     * will be called from the C code. If you need to do updating or checking depending on the
     * state of the stop, do so by registering an object with AeolusUIManager that implements the
     * interface InterfaceUpdater. Upon
     * stop changes, the onActiveStopsChanged of your InterfaceUpdater object will be called, allowing
     * you to react to the actual value after stop update.
     * @param index_division Index of the division the stop belongs to
     * @param index_stop Index of the stop among the step elements shown
     */
    public static native void activateStop(int index_division, int index_stop);
    /**
     * Deactivate a stop, which means that the stop cease to contribute to sound generation (technically, it
     * completely ceases once the stopping and reverberation effects are over).
     * <br /><br />
     * Stops are deactivated in asynchronous, non-blocking fashion in order not to interfere
     * with audio processing. This means that upon return of this function, the stop
     * itself might not be deactivated yet. The deactivation of the stop will generate a synchronization
     * callback. To be precise, once the stop is deactivated, AeolusUIManager.onStopsUpdated()
     * will be called from the C code. If you need to do updating or checking depending on the
     * state of the stop, do so by registering an object with AeolusUIManager that implements the
     * interface InterfaceUpdater. Upon
     * stop changes, the onActiveStopsChanged of your InterfaceUpdater object will be called, allowing
     * you to react to the actual value after stop update.
     * @param index_division Index of the division the stop belongs to
     * @param index_stop Index of the stop among the step elements shown
     */
    public static native void deactivateStop(int index_division, int index_stop);

    /** Get the number of midi channels that can be routed to divisions
     * @return Number of midi channels mappable to divisions. At present,
     * by default, this is 16, corresponding to available midi channels 0-15
     */

    public static native int getNumberMidiChannels();

    /** Get the division mapping for a given midi channel
     * <br /><br /> In Aeolus, each of the 16 midi channels has an associated midimapping.
     * This function queries the division-mapping part of the midi configuration. This is a
     * 4-bit information, encoding the routing of the midichannel to a maximum of 4 divisions.
     * The routing works bitwise:<br />
     * 0b0001 = 1 indicates routing to division 0 (i.e. the first division)<br />
     * 0b0010 = 2 indicates routing to division 1 <br />
     * 0b0100 = 4 indicates routing to division 2 <br />
     * 0b1000 = 8 indicates routing to division 3 <br />
     * Combinations like 0b1101 indicate routing to multiple divisions (here: 0,2,3)<br />
     * 0 indicates that the midichannel is not played by any division.
     * Routing to more than 4 divisions is not foreseen at present.
     * @param midiIndex The midi channel to be queried
     * @return The mapping byte associated with the midichannel.
     */

    public static native byte queryMidiMap(int midiIndex);

    /** Indicate whether a midi channel is mapped to a division
     * @param myDivisionIndex Index of the division of interest
     * @param myMidiChannelIndex Midichannel
     * @param isChecked True for mapping the channel to the division, false other wise
     */

    public static native void setMidiMapping(int myDivisionIndex, int myMidiChannelIndex, boolean isChecked);

    /**
     * Get the division volume.
     * @param index Index of the division
     * @return Volume associated with division, as a linear gain factor (default value without gain or loss is 1)
     */
    public static native float getDivisionVolume(int index);

    /**
     * Set the division volume.
     * @param index Index of the division
     * @param divisionGain Linear gain for the selected division. 1=no gain or loss.
     */
    public static native void setDivisionVolume(int index, float divisionGain);

    /**
     * Toggle the activation of the tremulant for the division
     * Toggle means that if the tremulant is on, it will be switched off, and visa-versa
     * @param divisionIndex Index of the division. If the division has no tremulant configured, the function call will
     *                      be ignored.
     */

    public static native void toggleTremulant(int divisionIndex);

    /** Does the division have a tremulant possibility?
     * @param divisionIndex The index of the division
     * @return True if tremulant is configured, false otherwise. This only indicates whether a tremulant is configured,
     * use the tremulantIsActive function to check whether it is activated at present
     */

    public static native boolean hasTremulant(int divisionIndex);

    /**
     * Is tremulant of selected division active at present?
     * @param division_index The index of the division to be queried
     * @return True if the tremulant of the selected division is activated. False if not activated, or
     * if the division does not have a tremulant
     */

    public static native boolean tremulantIsActive(int division_index);

    /**
     * Get the number of preconfigured tunigs available. These are defined in the scale.h and scale.cpp files
     * @return The number of available tunings
     */
    public static native int get_n_tunings();

    /**
     * Get the label of the i-th tuning configured
     * @param i Index of the tuning
     * @return Human-readable label for displaying or selecting the tuning
     */
    public static native String getTuningLabel(int i);

    /** Get the currently active tuning (= temperament or also scale, the denomination
     * varies a bit in different places in the Aeolus and Android implementation
     * @return The index of the currently selected tuning
     */
    public static native int getCurrentTuning();

    public static void reTune(int temperament) {
        reTune(temperament, getBaseFrequency());
    }

    public static native void reTune(int temperament, float baseFrequency);


    /**
     * Get the presently set base frequency for the entire organ
     * @return Base tuning frequency, as float
     */
    public static native float getBaseFrequency();

    /** Are we presently performing a retuning operation (i.e changing base tuning frequency or
     * changing temperament or both)?
     * @return true if retuning, false otherwise. First-time loading is not considered retuning.
     */
    public static native boolean isRetuning();

    public static native void panicoff();

    public static native void panicon();
}
