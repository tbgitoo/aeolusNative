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
// ----------------------------------------------------------------------------


#ifndef AEOLUS_IMIDI_ANDROID_H
#define AEOLUS_IMIDI_ANDROID_H


#include <cstdlib>
#include <cstdio>
#include "../../clthreads/include/clthreads.h"
#include "../../aeolus/source/imidi.h"


/** @brief Midi handling class in the Aeolus Android context
 *
 * In Android, midi is initialized from java via the Amidi library, such that direct
 * interaction with the midi hardware driver is not needed here anymore. For this
 * reason, the on_open_midi() and on_close_midi() functions from the base class Imidi are overriden with functions
 * that do nothing. The midi hardware driver interaction is instead found in AppMidiManager.cpp <br /:
 * This class thus has the function of dummy midi handler to satisfy the dependencies in the Aeolus, while
 * actual incoming midi messages are handled through AppMidiManager.
 */
class MidiAndroidAeolus : public Imidi
{
public:
    MidiAndroidAeolus (Lfq_u32 *qnote, Lfq_u8 *qmidi, uint16_t *midimap, const char *appname);



    
protected:

    /** Disabled on_open_midi function (does nothing) */
    void on_open_midi() override;
    /** Disabled on_close_midi function (does nothing) */
    void on_close_midi() override;
    /** Disabled terminate function (does nothing) */
    void on_terminate() override;


};


#endif
