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

#ifndef MIDI_SYNTH_ANDROID_AEOLUS_USER_INTERFACE_H
#define MIDI_SYNTH_ANDROID_AEOLUS_USER_INTERFACE_H


#include "tiface.h"
#include "android_aeolus_user_interface_jni.h"
/**
 * Android-specific additions to handling the user interface state. This class invokes the parent methods
 * from Tiface for the native aspects of handling the user interface state and additionally notifies
 * the Android java user interface of changes as appropriate. This serves the synchronization of the
 * Java interface with the Aeolus synthesizer state regarding updates that need to be communicated from the
 * native part to the Java interface.
 */
class android_aeolus_user_interface : public  Tiface  {

protected:
    /**
     * Initialize the user interface core data with the incoming message
     * @param M Incoming data for initialization of the user interface
     */
    void handle_ifc_init (M_ifc_init * M) override;
    /** Interface loading is complete. Notify the Android Java user interface */
    void handle_ifc_ready () override;
    /** The elements of a user interface element group need to be cleared in the user interface. Take the
     * necessary actions (see the parent class Tiface), and also notify of the Android Java user interface
     * @param M The ITC message for indicating the group to be cleared
     * @see Tiface
     */
    void handle_ifc_grclr (M_ifc_ifelm *M) override;
    /** A given user interface element needs to be cleared. Take the
     * necessary actions (see the parent class Tiface), and also notify of the Android Java user interface
     * @param M The ITC message for indicating the user interface element to be cleared
     * @see Tiface
     */
    void handle_ifc_elclr (M_ifc_ifelm *M) override;
    /** A given user interface element needs to be activated. Take the
    * necessary actions (see the parent class Tiface), and also notify of the Android Java user interface
    * @param M The ITC message for indicating the user interface element to be activated
    * @see Tiface
    */
    void handle_ifc_elset (M_ifc_ifelm *M) override;
    /**
     * Retuning is done. Notify the Android Java interface.
     */
    void handle_ifc_retuning_done() override;



};




#endif //MIDI_SYNTH_ANDROID_AEOLUS_USER_INTERFACE_H
