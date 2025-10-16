// ----------------------------------------------------------------------------
//
//  Copyright (C) 2025 Mathis and Thomas Braschler <thomas.braschler@gmail.com>
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
// The concept of this synthesizer is in part inspired by
// https://github.com/JanWilczek/android-wavetable-synthesizer/blob/main/app/src/main/cpp/WavetableSynthesizer.cpp ,
// The android-wavetable-synthesizer repository is also under a GPL v3.0 license.
// ----------------------------------------------------------------------------

#ifndef MIDI_SYNTH_AEOLUSSYNTHESIZER_H
#define MIDI_SYNTH_AEOLUSSYNTHESIZER_H

#include <vector>
#include "../../../SynthesizerBase/include/Synthesizer.h"
#include "AeolusOscillator.h"
#include "../../../aeolus/source/iface.h"
#include "../../../aeolus/source/audio.h"
#include "../../../aeolus/source/imidi.h"

#define max_rank_in_stops 5



namespace Aeolussynthesizer {
    /**
     * @brief Central synthesizer class commanding the Aeolus C implementation
     *
     * This is the class definition for the main synthesizer object of the Aeolus Android application.
     * The AeolusSynthesizer orchestrates the audio synthesis, interaction with oboe, as well as the Aeolus
     * model, slave and user interface threads.
     */
    class AeolusSynthesizer: public synthesizerBase::Synthesizer, public AeolusAudio, public AeolusAudioProcessingDelegate {

    public:
        /**
         * Constructor, includes starting the model, slave and ui thread
         * @param qnote The note queue. This is globally instantiated in AeolusSynth_jni_functions.cpp
         * @param qcomm Interthread communication queue. This is globally instantiated in AeolusSynth_jni_functions.cpp
         * @param qmidi Midi command queue. This is globally instantiated in AeolusSynth_jni_functions.cpp
         * @param stopsPath Path to the stop definition (ae0) files
         */
        AeolusSynthesizer( Lfq_u32 *qnote, Lfq_u32 *qcomm,Lfq_u8* qmidi,
                                   const char *stopsPath);


        ~AeolusSynthesizer() override;
        /** Start playing audio. This starts the regular callbacks from oboe, which through
         * a cascade of function calls involving the _audioPlayer and _defaultOscillator lead to invocation
         * of the fillAudioBuffer method
         */
        void play() override;
        /**
         * Interrupt audio playing via callbacks from oboe
         */
        void stop() override;

        /**
         * Set the overall volume
         * @param volume The overall volume as linear gain factor
         */
        void setVolume(float volume) override;




        /**
         * Get the currently configured audio data source
         * @return audio data source
         */
        AeolusOscillator* getAudioSource() override;

        /**
         * Use the default oscillator to use Aeolus
         */
        virtual void useDefaultOscillator();

        /**
         * Set the path to the stops (ae0 files)
         * @param stopsPath The new stops path
         */
        void setStopsPath(const char* stopsPath);


        /** Start audio. This notifies the other threads that the audio synthesis is ready
         *
         */
        void  start () override;
        /**
     * @brief Callback for audio synthesis invoked when oboe needs new audiio data
     *
     * This function is invoked through a chain of function calls when oboe indicates that data
     * for audio output is needed (i.e. in the default configuration, the primary responder is the _audioPlayer, followed
     * by AeolusOscillator, and ultimately this function ). Audio data is obtained through Aeolus audio synthesis
     * algorithm.<br />
     *
     * audioData represents a pre-reserved float array of length framesCount*channelCount, with a block
     * structure with channelCount blocks of each block consisting of framesCount consecutive floats. I.e.
     * This can for example be achieved with an array of the structure float[channelCount][framesCount]. Oboe
     * expects that the values provided be in the interval -1.0 to +1.0
     * @param audioData Pointer to the start of the audio data buffer, preallocated by oboe, of length
     *                  framesCount*channelCount and with an intended blockwise structure float[channelCount][framesCount]
     * @param framesCount Number of samples to be supplied in each channel
     * @param channelCount Number of channels, e.g., number of blocks. Mono=1, Stereo=2
     */
        void fillAudioBuffer(float *audioData, int32_t framesCount,
                                     oboe::ChannelCount channelCount) override;

        /**
         * Start playing the midi note given by key, on the midi channel given by chan, at velocity vel
         * @param chan Midi channel (0-15)
         * @param key Midi note key
         * @param vel Midi velocity
         */
        void noteon(int chan, int key, int vel);

        /**
         * Stop playing the midi note given by key, on the midi channel given by chan, with velocity vel
         * @param chan Midi channel (0-15)
         * @param key Midi note key
         * @param vel Midi velocity
         */

        void noteoff(int chan, int key, int vel);

        /** Direct activation of a rank for all keyboard input.
         * @param division_id The division within which the rank resides
         * @param rank_id The id of the rank to activate within the division
         */
        void activateRank(int division_id, int rank_id);

        /** Activate a stop for the preconfigured keyboards
         *
         * @param division_id ID of the division in which the stop is found
         * @param stop_id ID of the stop within the division
         */
        void activateStop(int division_id, int stop_id);
        /** Deactivate a stop for the preconfigured keyboards
        *
        * @param division_id ID of the division in which the stop is found
        * @param stop_id ID of the stop within the division
        */
        void deactivateStop(int division_id, int stop_id);
        /** Direct deactivation of a rank for all keyboard input.
         * @param division_id The division within which the rank resides
         * @param rank_id The id of the rank to activate within the division
         */
        void stopRank(int division_id, int rank_id);

        /**
         * Query whether we are still in the initialization phase
         * @return True if initializing, false otherwise
         */

        bool isInitializing();

        /**
         * Get the numboer of divisions (sets of ranks)
         */

        int get_n_divisions();
        /**
         * Get the number of available tunings
         * @return The number of available predefined tunigns
         */

        int get_n_tunings();
        /**
         * Get the text label of a division
         * @param division_index The index of the division
         * @return The text label
         */
        const char* getLabelForDivision(int division_index);

        /**
         * Get the text label for a tuning
         * @param index_tuning The index of the tuning for which we want to get the lable
         * @return The text label
         */

        const char* getTuningLabel(int index_tuning);
        /**
         * Get the index of the tuning currently in use
         * @return The tuning index
         */
        int getCurrentTuning();
        /**
         * Currently used base frequency for tuning, default is 440Hz for the base la
         * @return The currently used base frequency for tuning
         */
        float getBaseFrequency();
        /**
         * Retune the Aeolus instrument
         * @param temperament The index of the desired tuning
         * @param base_frequency The new base frequency (default, 440Hz)
         */
        void retune(int temperament,float base_frequency);
        /**
         * @brief Get the number of stops configured in the user inteface for a given division.
         *
         * Note that the number of stops in the user interface is sometimes identical, and sometimes
         * different from the number of ranks in the division. <br />
         * Indeed, in many cases, each rank is addressed individually with an associated stop (a stop is a on/off button in the user
         * interface). However, there are other cases as well. For example,
         * a number of ranks can be played as predefined mixture and
         * have an associated stop for the mixture; there may also be ranks in a division that do not
         * have an associated stop in the user interface.<br /><br />
         * Finally, technically,
         * the number of stops is defined not directly for the division, but for the user interface group.
         * In the present implementation
         * of Aeolus there is exactly 1 group per division with the same index, and so the stops can
         * be accessed via the division index.
         * @param division_index The division for which we need the number of stops in the user interface
         * @return The number of
         */
        int get_n_stops_for_division(int division_index);

        /**
         * @brief Get the text label of a stop in the user interface
         *
         * A stop is a on/off button in the user interface which serves to switch on/off ranks or mixtures of ranks of
         * the division. The label is displayed in the user interface along with the button. <br />
         * The stop labels are defined in instrument definition file.
         * @param division_index The division
         * @param index_stop The index of the stop within the division
         * @return Text label to be displayed in the user interface
         */
        const char* getLabelForStop(int index_division,int index_stop);
        /**
         * @brief Is the stop activated?
         *
         * Stops are on/off buttons in the user interface and serve to switch off or on the ranks. A rank
         * is a set of pipes; when a rank is on, it is receptive to incoming midi events (provided the midi channel
         * is mapped to a keyboard for which the rank is active) and can thus play incoming midi notes. The stops
         * are linked to either single ranks or mixtures of ranks, and can provide activation / deactivation of the linked
         * ranks. Note that since midi input is needed, by itself, stop activation does not produce a sound.
         * @param index_division The index of the division in which the stop resides
         * @param index_stop The index of the stop within the division
         * @return True if the rank is activated, false otherwise
         */
        bool getStopActivated(int index_division,int index_stop);

        /**
         * @brief Main loop for the AeolusSynthesizer message handling thread
         *
         * This function starts the message handling thread. Note that methods of the AeolusSnythesizer
         * class are invoked in different thread settings:<br />
         * - The AeolusSynthesizer runs its own thread through this function. The purpose of this thread
         *   is to allow asynchronous handling of incoming communication messages from the other threads<br />
         * - Oboe runs its own, high priority thread and invokes the audio functions (through _audioPlayer,
         *   and callback to the fillAudioBuffer of the AeolusSynthesizer class)<br />
         * - And finally, through jni, various functions are invoked from the main Android app
         */
        void thr_main () override;
        /**
         * For stops representing mixtures of ranks, the maximum number of ranks that can be mixed in
         * a single stop.
         * @return The maximum number of ranks associated with a single stop
         */
        int maxRanksPerStop();

        /**
         * @brief Does this division have a tremulant?
         *
         * A tremulant is an audio effect that imposes a small sinusoidal amplitude variation on
         * top of the steady audio signal generated by the pipes. In Aeolus, the tremulant
         * effect is activated for the sound signal stemming from a given division.
         * @param division_index The division for which the tremulant state should be queried
         * @return True if the tremulant is active, false if it isn't or if the division has no tremulant.
         */
        bool division_has_tremulant(int division_index);
        /**
         * @brief Turn on the tremulant audio effect for this division
         *
         * Calling this function will only have an effect on divisions which actually have a tremulant as
         * per the instrument definition file.
         * @param division_id The index of the division
         */
        void activateTremulantForDivision(int division_id);
        /**
         * @brief Turn off the tremulant audio effect for this division
         *
         * Calling this function will only have an effect on divisions which actually have a tremulant as
         * per the instrument definition file.
         * @param division_id The index of the division
         */

        void deactivateTremulantForDivision(int division_id);

        /**
         * @brief Are we presently performing a retuning operation?
         *
         * Retuning of the Aeolus instrument is triggered by the change in temperament, base tuning frequency,
         * or both. This causes the Aeolus implementation to recalculate the ae1 files containing the pre-calculated
         * wavetables. While the recalculation operation is being performed by the slave thread, this
         * function returns true.
         * @return True when performing retuning.
         */

        bool is_retuning();


    protected:


        /** @brief Allocate output buffer for audio processing
         *
         * For audio output, the Aeolus synthesizer fills its own audio buffer, in which the
         * output audio signal is stored. Once audio synthesis complete, the audio amplitude values
         * are copied to the oboe audio buffer to drive the audio system in fillAudioBuffer<br />
         * This approach permits to have the various collaborating audio synthesis classes (divisions,
         * audio sections, ranks, ...) having fixed, preconfigured buffer pointers, with transfer
         * to the variable oboe buffer pointer only once synthesis is complete.
         */
         void allocateOutputBuffer();
         /** @brief Default oscillator for running Aeolus
          *
          * The default oscillator is configured during AeolusSynthesizer object construction and routes
          * oboe audio data request callbacks back to the AeolusSynthesizer::fillAudioBuffer method
          * This permits to use the Aeolus audio synthesizing machinery to provide the audio data needed
          * by oboe.
          */
         std::unique_ptr<Aeolussynthesizer::AeolusOscillator> _defaultOscillator = nullptr;
         /**
          * Mutex used for thread locking for sensitive parts
          */
         std::mutex _mutex;
         /**
          * Path to the .ae0 rank (and rank mixture) definition files
          */
         const char* _stopsPath="";

        /**
         * Model, for configuration and model thread
         */
        Model* model;
        /**
         * Slave, for execution of particularly time intense tasks such as loading, calculating or saving the ranks,
         * starts its own thread
         */
        Slave* slave;


        /** Interthread controller, ensures messaging between the different ITC threads
         */
        ITC_ctrl itcc;

        /**
         * User interface, including its own message handling thread
         */
        std::unique_ptr<Iface> _ui = nullptr; // User interface component, needed by aeolus setup

        /**
         * Midi interface. Used for midi message relay. In this implementation, there is no
         * proper midi thread under the control of the _midiInterface, instead, the incoming midi events
         * are handled in first line by AppMidiManager
         */
        std::unique_ptr<Imidi> _midiInterface = nullptr;

        /** Internal midi messaging queue */
        Lfq_u8  *_qmidi;


        const char* stop_directory; // individual stops, for registers
        const char* instrument_directory; // Instrument overall construction
        const char* wave_directory; //  directory to store pre-calculated wave tables

        bool isPlaying=false; // is the oboe audio generation running?

        /** Returns the index of a user interface element in the group associated with division.
        * This finds runs the interface elements of the group associated with the division and
        * attemps to find the index_stop_within_type-th user interface element corresponding to the given type
        * If no such element can be found, the function returns -1
         * @param index_division The division to be examined
         * @param index_stop_within_type Within the selected element_type, find the element at position index_stop_within_type
         * @param element_type Type of element (DIVRANK, KBDRANK, COUPLER, TREMUL), cf. Ifelm::Ifelm_type
         * @return The absolute interface element index within its group (across all types), or -1 if no corresponding
         * interface element could be found.
         */
        int getAbsoluteInterfaceElementIndex(int index_division, int index_stop_within_type, Ifelm::Ifelm_type element_type);

        /**
         * @brief Get the interface element pointer for a given stop
         *
         * The absolute index required for this function can be obtained with getIfelmIndexForStop
         * @param index_division The division in which the stop resides
         * @param index_stop The absolute index of the stop (e.g. interace element of type DIVRANK)
         * @return Pointer to the interface element representing the desired stop
         */

        Ifelm* getIfelmForStop(int index_division, int index_stop);

        /** Returns the index of a stop user interface element in the group associated with division.
        * This finds runs the interface elements of the group associated with the division and
        * attemps to find the index_stop_within_type-th stop (DIVRANK-type) user interface element
        * If no such element can be found, the function returns -1
         * @param index_division The division to be examined
         * @param index_stop Within ranks, find the element at position index_stop
         * @return The absolute interface element index, or -1 if no corresponding
         * interface element could be found.
         */

        int getIfelmIndexForStop(int index_division, int index_stop);
        /**
         * Get the absolute user interface element index for the tremulant of a division
         * @param index_division The index of the division
         * @return The user element index of the tremulant, or -1 if the division has no tremulant
         */
        int getIfelmIndexForTremulant(int index_division);


    };

}

#endif
