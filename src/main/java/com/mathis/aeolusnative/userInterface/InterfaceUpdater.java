package com.mathis.aeolusnative.userInterface;

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



/**
 * Interface with which objects that need to respond to User Interface update requests from the
 * C implementation of Aeolus need to comply
 * Such Interface update responsers also need to register themselves with the AeolusUIManager
 */

public interface InterfaceUpdater {


    /**
     * The active stops have changed on the C-side and should be updated in the Android graphical
     * interface
     */
    public void onActiveStopsChanged();

    /**
     * This event is triggered when
     * basic loading by the Aeolus C-code is finished and the user interface can be completed
     */
    public void onLoadComplete();

    /**
     * Event called when
     * Retuning is completed on the C-side
     */

    public void onRetuned();
}

