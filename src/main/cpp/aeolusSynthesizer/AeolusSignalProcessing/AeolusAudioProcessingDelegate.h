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

#ifndef MIDI_SYNTH_AEOLUSAUDIOPROCESSINGDELEGATE_H
#define MIDI_SYNTH_AEOLUSAUDIOPROCESSINGDELEGATE_H


#include <cstdint>

/**
 * @brief Interface class for classes that provide audio data by filling the Audio buffer
 *
 * The audio system here is implemented through oboe.
 * The oboe system is configured in the high priority mode here.
 * See <a href="https://chromium.googlesource.com/external/github.com/google/oboe/+/refs/heads/1.4-stable/docs/FullGuide.md#using-a-high-priority-callback">
 * oboe documentation</a>
 * for a detailed explanation on the topic. <br >In this Android Aeolus implementation, the central AeolusSynthesizer object
 * sets up a synthesizerBase::OboeAudioPlayer as the primary callback for oboe, and configures the OboeAudioPlayer to call back
 * AeolusOscillator when invoked with
 * onAudioReady by oboe. Along with pointer formatting, AeolusOscillator in turn calls back the processing delegate that implements
 * the AeolusAudioProcessingDelegate interface defined here via the fillAudioBuffer defined in this class.
 * In this Aeolus Android implementation, the choice was made that the AeolusSynthesizer main object itself
 * be the processing delegate implementing the AeolusAudioProcessingDelegate protocol,
 * centralizing setup and audio processing.
 */
class AeolusAudioProcessingDelegate {

public:
    /**
     * @brief A buffer is ready for processing, fill it with audio data
     *
     * This function is invoked through a chain of function calls when oboe indicates that data
     * for audio output is needed. This invocation takes place from a high priority thread,
     * avoid blocking or time consuming tasks.<br />
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
    virtual void fillAudioBuffer(float *audioData, int32_t framesCount,
                                        oboe::ChannelCount channelCount)=0;



};


#endif //MIDI_SYNTH_AEOLUSAUDIOPROCESSINGDELEGATE_H
