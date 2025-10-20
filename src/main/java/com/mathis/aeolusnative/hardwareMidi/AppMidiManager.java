package com.mathis.aeolusnative.hardwareMidi;

import android.media.midi.MidiDevice;

import com.mathis.midiBase.hardwareMidiNativeReceiver;
import com.mathis.midiBase.hardwareMidiNativeSetup;

public class AppMidiManager implements hardwareMidiNativeSetup, hardwareMidiNativeReceiver {

    private hardwareMidiNativeReceiver mMidiMessageReceiver=null;

    /**
     * Init the native tiers
     * This permits callback with natively received midi messages
     */
    @Override
    public native void initNative();

    /**
     * Start midi reading thread in the native implementation
     *
     * @param receiveDevice The selected midi receive device
     * @param portNumber    Possiblity to set a specific port, by default, provide 0
     */
    @Override
    public native void startReadingMidi(MidiDevice receiveDevice, int portNumber);

    @Override
    public native void stopReadingMidi();

    /**
     * Set the receiver for retransmission of incoming midi messages
     *
     * @param receiver Class that should do the actual handling of the incoming midi messages
     */
    @Override
    public void setMessageReceiver(hardwareMidiNativeReceiver receiver) {
        mMidiMessageReceiver=receiver;
    }

    /**
     * Callback function that will be called with the received midi message data
     *
     * @param message Raw midi message, byte-wise
     */
    @Override
    public void onNativeMessageReceive(byte[] message) {
        if(mMidiMessageReceiver!=null)
        {
            mMidiMessageReceiver.onNativeMessageReceive(message);
        }
    }
}
