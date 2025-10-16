// ----------------------------------------------------------------------------
//
//  Copyright (C) 2003-2013 Fons Adriaensen <fons@linuxaudio.org>
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


#include "MidiAndoidAeolus.h"



void MidiAndroidAeolus::on_open_midi() {

}

void MidiAndroidAeolus::on_close_midi() {

}

void MidiAndroidAeolus::on_terminate() {

}

MidiAndroidAeolus::MidiAndroidAeolus(Lfq_u32 *qnote, Lfq_u8 *qmidi, uint16_t *midimap,
                                     const char *appname) : Imidi(qnote, qmidi, midimap, appname) {

}
