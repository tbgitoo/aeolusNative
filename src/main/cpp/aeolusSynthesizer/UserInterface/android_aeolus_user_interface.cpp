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

#include <jni.h>
#include "android_aeolus_user_interface.h"




void android_aeolus_user_interface::handle_ifc_init(M_ifc_init * M) {

    if (_initdata) _initdata ->recover ();
    _initdata = M_ifc_init::createCopy(M);
}


void android_aeolus_user_interface::handle_ifc_ready() {
    {
        if (_init)
        {
            AndroidAeolusUserInterfaceOnLoadComplete();
            tIO.handleOutputFromTI("Aeolus is ready");

            print_info ();
            // In the original Linux implementation, we would start the command line reading, here,
            // the user interface is handled by communication with Android, and so this is not needed anymore
            //_reader.thr_start (SCHED_OTHER, 0, 0);
            //_reader.read ();
        }
        _init = false;
    }
}

void android_aeolus_user_interface::handle_ifc_grclr(M_ifc_ifelm *M) {
    Tiface::handle_ifc_grclr( M);
    AndroidAeolusUserInterfaceonStopsUpdated();
}

void android_aeolus_user_interface::handle_ifc_elclr(M_ifc_ifelm *M) {
    Tiface::handle_ifc_elclr( M);
    AndroidAeolusUserInterfaceonStopsUpdated();
}

void android_aeolus_user_interface::handle_ifc_elset(M_ifc_ifelm *M) {
    Tiface::handle_ifc_elset(M);
    AndroidAeolusUserInterfaceonStopsUpdated();

}



void android_aeolus_user_interface::handle_ifc_retuning_done() {
    Tiface::handle_ifc_retuning_done();
    AndroidAeolusUserInterfaceonRetuned();
}


