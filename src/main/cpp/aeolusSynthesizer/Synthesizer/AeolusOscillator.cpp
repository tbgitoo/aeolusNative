#include "include/AeolusOscillator.h"
#include "../UserInterface/tiface.h"
#include <cmath>
#include <getopt.h>
#include <dirent.h>


namespace   Aeolussynthesizer {

    AeolusOscillator::AeolusOscillator(AeolusAudioProcessingDelegate* processingDelegate)
    : _processingDelegate(processingDelegate)

    {

    }



    void AeolusOscillator::onPlaybackStopped() {
    }


    void AeolusOscillator::onAudioReady(float *audioData, int32_t framesCount,
                                        oboe::ChannelCount channelCount) {
        _processingDelegate->fillAudioBuffer(audioData,framesCount,channelCount);

    }



}  // namespace AeolusSynthesizer
