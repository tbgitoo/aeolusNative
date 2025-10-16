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
// https://github.com/JanWilczek/android-wavetable-synthesizer/blob/main/app/src/main/cpp/WavetableOscillator.cpp ,
// The android-wavetable-synthesizer repository is also under a GPL v3.0 license.
// ----------------------------------------------------------------------------

#ifndef MIDI_SYNTH_AEOLUSOSCILLATOR_H
#define MIDI_SYNTH_AEOLUSOSCILLATOR_H

#include <vector>
#include "../../../SynthesizerBase/include/AudioSource.h"
#include "../../../aeolus/source/model.h"
#include "../../../aeolus/source/slave.h"
#include "../../../aeolus/source/iface.h"
#include "../../AeolusSignalProcessing/AeolusAudioProcessingDelegate.h"
#include <android/log.h>

namespace Aeolussynthesizer {
    /**
     * @brief The AeolusOscillator collaborates with an AudioPlayer in that it provides the audio data
     * upon request by the AudioPlayer
     *
     * In this Aeolus implementation, the audio synthesis and interaction with oboe is
     * orchestrated by the AeolusSynthesizer object. The AeolusSynthesizer holds and AudioPlayer,
     * which is directly responsible for configuration of the Oboe audio driver system and acts
     * as the primary entry point for the audio data callbacks by Oboe. For obtaining audio data,
     * the AudioPlayer calls this class, and specifically the AeolusOscillator::onAudioReady function.<br />
     * The onAudioReady function
     * of this class is thus responsible of providing the audio data<br />
     * This architecture with division of labor for wavetable synthesis
     * is based on the work by Jan Wilczek (e.g. https://github.com/JanWilczek/android-wavetable-synthesizer).
     */
    class AeolusOscillator : public synthesizerBase::AudioSource
            {
    public:
        // The processing delegate can be called back to help with numerical processing when new
        // audio data is needed
        explicit AeolusOscillator(AeolusAudioProcessingDelegate* processingDelegate);



        /**
         * Invoked when the audio playback stops. Here, does nothing.
         */
        void onPlaybackStopped() override;



        /**
       * @brief Main audio callback function: A buffer is ready for processing, fill it with audio data
       *
       * This function function invoked by the AudioPlayer, upon call back by Oboe,
       * when the Oboe audio driver needs new audio data<br />
       * This function invokes the processingDelegate's onAudioReady function. In this Aeolus Android
       * implementation, the processing delegate is the main AeolusSynthesizer object, such that the Aeolus
       * syntheszier frame work with the ranks and audio effects is invoked to provide the necessary audio data.
       *
       * @param audioData Pointer to the start of the audio data buffer, preallocated by oboe, of length
       *                  framesCount*channelCount and with an intended blockwise structure float[channelCount][framesCount]
       * @param framesCount Number of samples to be supplied in each channel
       * @param channelCount Number of channels, e.g., number of blocks. Mono=1, Stereo=2
       */
        void onAudioReady(float* audioData, int32_t framesCount, oboe::ChannelCount channelCount) override;





    protected:
        /** Object that will provide the actual audio data */
        AeolusAudioProcessingDelegate* _processingDelegate=nullptr;

    };


}  // namespace AeolusSynthesizer


#endif
