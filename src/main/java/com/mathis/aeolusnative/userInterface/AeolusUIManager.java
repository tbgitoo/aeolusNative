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

package com.mathis.aeolusnative.userInterface;

import android.util.Log;

/**
 * This class specifically transmits update requests coming from the C implementation of Aeolus:
 * Changes in activation of stops, and changes in tremulus activation
 * This class transmits the notification by receiving the calls from the C part via jni callbacks, and
 * transmits the notification to java listeners (which implement the interface "InterfaceUpdater").
 * <br /><br />
 * This is a one-way callback class, transmitting only from C to Java.
 * If on the contrary, you are looking for predefined functions that allow you to invoke Aeolus C routines from Java, you need to
 * take a look at the AeolusSynthManager class.
 *
 */

public class AeolusUIManager {
    // Load the static library
    static {System.loadLibrary("AeolusUserInterface");}

    /**
     * Interface updater to be notified upon incoming notifications
     */
    protected static InterfaceUpdater theUpdater=null;

    /**
     * Set the current interface updater client
     * @param u The new interface updater client. Set null if no notification messages should be
     *          transmitted further
     */

    public static void setAeolusUserInterfaceUpdater(InterfaceUpdater u)
    {
        theUpdater=u;
    }

    /**
     * Method called from C implementation: aeolusSynthesizer/UserInterface/android_aeolus_user_interface.cpp
     * function AndroidAeolusUserInterfaceonStopsUpdated
     * This is invoked when the interface thread is informed about a change in interface element settings
     * via a message posted on the ITC inter-thread communication system
     * Do not delete this function, the C implementation of Aeolus calls this and needs this
     */

    public static void onStopsUpdated()
    {

        if(theUpdater != null)
        {

            theUpdater.onActiveStopsChanged();
        }
    }


    /**
     * Method called from C implementation: aeolusSynthesizer/UserInterface/android_aeolus_user_interface.cpp
     * function AndroidAeolusUserInterfaceOnLoadComplete()
     * This is invoked when the interface thread is informed about completion of basic loading of the
     * aeolus machinery (including wavetable preparation if necessary). Once the C-loading is complete
     * we can proceed with construction of the Java visible user interface
     */

    public static void onLoadComplete()
    {

        if(theUpdater != null)
        {
            theUpdater.onLoadComplete();
        }
    }

    /**
     * Event triggered when retuning (change of frequency, temperament or both) is complete
     * In Aeolus, this takes a while because the wavetables are recalculated. For this reason,
     * it is useful to indicate the state of retuning to the user. This function transmits the notification
     * of completion of retuning to the InterfaceUpdater object configured via setAeolusUserInterfaceUpdater
     */

   public static void onRetuned()
   {
       Log.i("AeolusUIManager","onRetuned called");
       if(theUpdater != null)
       {
           theUpdater.onRetuned();
       }
   }


    /**
     * Set up the native C / Java communication. This should be done during activity initialization
     */

    public static native void initNative();



}
