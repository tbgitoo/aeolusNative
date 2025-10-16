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


#ifndef AEOLUS_TIFACE_H
#define AEOLUS_TIFACE_H


#include "../../aeolus/source/iface.h"
#include "textInterfaceIO.h"

/**
 * Command line reader of the original Aeolus implementation for Linux (https://github.com/SimulPiscator/aeolus).
 * This functionality is not used here.
 */
class Reader : public H_thread
{
public:

    Reader (Edest *, int);
    virtual ~Reader ();

    void read ();

private:

    virtual void thr_main ();

protected:
    const char* presentCommandLine="";
    bool lineReady=false;


};


/** Text interface implementation from the original -Aeolus linux application (
 * https://github.com/SimulPiscator/aeolus).
 * Here, implements the native part of updating the information about the user interface state,
 * and a series of debug print routines which can be used to print information about the state of
 * the Aeolus user interface to the Android log.*/

class Tiface : public Iface
{
public:

    Tiface ();
    virtual ~Tiface ();
    void stop () override;

    bool isInitializing() override;
    int get_n_divisions() override;
    const char* getLabelForDivision(int division_index) override;


protected:
    /** Main thread loop, reads incoming ITC messages with a timeout, and handles them. This continues
     * until an EV_EXIT event is received, causing the thread to return and stop. */
    void thr_main () override;
    /** @brief Handle incoming ITC messages
     *
     * This function is called from thr_main. Depending on the type of the ITC message, different sub-routines are invoked.
     * Generally, the aim of the messaging would be the update of the user interface; here, the class focuses on
     * Logging since the Android interace updates are handled more specifically in daughter classes.
     * @param M The incoming ITC message
     */
    void handle_mesg (ITC_mesg *M);
    void handle_time ();
    virtual void handle_ifc_ready ();
    virtual void handle_ifc_init (M_ifc_init *);
    void handle_ifc_mcset (M_ifc_chconf *);
    virtual void handle_ifc_retune (M_ifc_retune *);
    /** @brief Clear the activation state of all the user interface element in a given group
     *
     * The state of the user interface elements is stored as a binary mask indicating the state
     * of each element via the bits of the mask. This function sets the binary mask of the
     * user interface group conveyed by M to all zeroes.
     *
     * @param M ITC message from which the group identity is taken
     */
    virtual void handle_ifc_grclr (M_ifc_ifelm *M);
    /** @brief Clear the activation state of a given user interface element in a given group
     *
     * The state of the user interface elements is stored as a binary mask indicating the state
     * of each element via the bits of the mask. This function sets the bit corresponding to the
     * interface element in the given group, both conveyed by M, to zero.
     *
     * @param M ITC message from which the group and interace element identity are taken
     */
    virtual void handle_ifc_elclr (M_ifc_ifelm *);
    /** @brief Activate a given user interface element
   *
   * The state of the user interface elements is stored as a binary mask indicating the state
   * of each element via the bits of the mask. This function sets the bit corresponding to the
   * interface element in the given group, both conveyed by M, to 1.
   *
   * @param M ITC message from which the group and interace element identity are taken
   */
    virtual void handle_ifc_elset (M_ifc_ifelm *);
    /** @brief Retuning is done
     *
     * The slave thread has completed retuning, notify the user of the completion.
     */
    virtual void handle_ifc_retuning_done();
    /**
     * Attributes of a user interface element have changed, update user interface
     *
     * @param M Interface element identification for the updated element (i.e. message with group, id within group)
     */
    void handle_ifc_elatt (M_ifc_ifelm *M);
    /**
    * @brief Command on command line has been received, parse and handle
     *
     * This is part of the linux command line interface, not used here in this Android implementation
    * @param M Message with the line of command
    */
    void handle_ifc_txtip (M_ifc_txtip *);
    /**
     * Print general information about the user interface configuration to the Android log
     */
    void print_info ();
    /**
     * Print the midi->keyboard mapping to the Android log
     */
    void print_midimap ();
    /**
     * Print the keyboard configuration to the Android log
     */
    void print_keybdd ();
    /**
     * Print information about the divisions to the Android log
     */
    void print_divisd ();
    /**
     * Print summary about the stops to the Android log
     * @param group The group of stops of interest
     */
    void print_stops_short (int group);
    /**
     * Print summary about the stops to the Android log
     * @param group The group of stops of interest
     */
    void print_stops_long (int);
    /** Remove strange characters from the label
     * @param pointer to the label, the content of this pointer will be changed
     */
    void rewrite_label (const char *p);
    /**
     * @brief Parse a command line command.
     *
     * This method is not used in the Android implementation because the application is not used
     * from the command line, but through the app interface
     * @param Command line command to be processed
     */
    void parse_command (const char *p);
    /**
     * Part of the parse command framework, not used
     */
    void command_s (const char *);
    /**
     * Find group id from group name
     *
     * @param p Group name
     * @return Group id, or -1 if not found
     */
    int  find_group (const char *p);
    /**
     * Find user interface element id from name
     * @param p Name of the user interface element
     * @param g Group id
     * @return Index of the user interface element in the group, or -1 if not found
     */
    int  find_ifelm (const char *p, int g);

    /**
     * Special extra group indices
     */
    int  comm1 (const char *);
    // Not needed without command line interface
    // Reader          _reader;
    bool            _stop; // stop flag for the main thread routine, will return at next iteration when _stop is set true
    bool            _init; // This is true during the initialization phase, and false afterwards
    M_ifc_init     *_initdata; // User interface settings encapsulated as ITC message
    M_ifc_chconf   *_mididata; // Pointer to the midi interface settings encapsulated as ITC message
    uint32_t        _ifelms [NGROUP]; // Activation state for the user interface elements as binary mask per group
    char            _tempstr [500]; // Temperorary string allocated for string operations

    textInterfaceIO tIO; // output to the Android log



};


#endif
