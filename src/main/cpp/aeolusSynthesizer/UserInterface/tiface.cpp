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

/**
 * @file tiface.cpp
 * @brief Implementation of the Tiface and Reader classes.
 *
 * This file contains the implementation of the Tiface and Reader classes, which
 * together provide a text-based user interface for the Aeolus synthesizer.
 * The Reader class is responsible for reading user input, while the Tiface
 * class processes commands and manages the synthesizer's state.
 *
 * Note that this class is not used directly in the aeolus Android codebase,
 * because there is actually no console for input-output available. Instead
 * android_aeolus_user_interface extends this class to provide the necessary
 * functionality for the Android environment.
 */

#include <cstdlib>
#include <cstdio>
#include <cctype>
#include "tiface.h"

/**
 * @class Reader
 * @brief A class for reading user input from the command line.
 *
 * The Reader class is a thread that reads user input from the command line
 * and sends it to the Tiface class for processing.
 */

/**
 * @brief Constructs a Reader object.
 * @param edest A pointer to the destination for events.
 * @param ipind The index of the input port.
 */
Reader::Reader (Edest *edest, int ipind) :
    H_thread (edest, ipind)
{
}

/**
 * @brief Destroys the Reader object.
 */
Reader::~Reader ()
= default;

/**
 * @brief Puts an M_ifc_txtip event into the event queue.
 */
void Reader::read ()
{
    put_event (0, new M_ifc_txtip);
}

/**
 * @brief The main loop of the Reader thread.
 *
 * This function reads user input from the command line and sends it to the
 * Tiface class for processing.
 */
void Reader::thr_main ()
{
    M_ifc_txtip  *M;

    while (true)
    {
	get_event (1);
	M = (M_ifc_txtip *) get_message ();
    if(lineReady) {
        lock();
        if (strlen(presentCommandLine) == 0) {
            M->_line =strdup("");
        } else {
        M->_line = strdup(presentCommandLine);

        strcpy(M->_line,presentCommandLine);
        presentCommandLine="";
        }
        unlock();
        }

        reply  (M);
    }
}

/**
 * @class Tiface
 * @brief A text-based user interface for the Aeolus synthesizer.
 *
 * The Tiface class is responsible for processing user commands, managing the
 * synthesizer's state, and displaying information to the user.
 */

/**
 * @brief Constructs a Tiface object.
 */
Tiface::Tiface () :
    _stop (false),
    _init (true),
    _initdata (nullptr),
    _mididata (nullptr)
{
    int i;

    for (i = 0; i < NGROUP; i++) _ifelms [i] = 0;
}

/**
 * @brief Destroys the Tiface object.
 */
Tiface::~Tiface ()
= default;

/**
 * @brief Stops the Tiface thread.
 */
void Tiface::stop ()
{
}

/**
 * @brief The main loop of the Tiface thread.
 *
 * This function processes events and messages from the event queue.
 */
void Tiface::thr_main ()
{
    set_time (nullptr);
    inc_time (125000);

    while (! _stop)
    {
	switch (get_event ())
	{
        case FM_MODEL:
        case FM_TXTIP:
            handle_mesg (get_message ());
	    break;

        case EV_EXIT:
            return;
	}
    }
    send_event (EV_EXIT, 1);
}

/**
 * @brief Handles a message from the event queue.
 * @param M A pointer to the message.
 */
void Tiface::handle_mesg (ITC_mesg *M)
{
    if(M== nullptr)
    {
        return;
    } else {

        switch (M->type()) {
            case MT_IFC_INIT:
                handle_ifc_init((M_ifc_init *) M);

                break;

            case MT_IFC_READY:
                handle_ifc_ready();
                break;

            case MT_IFC_ELCLR:
                handle_ifc_elclr((M_ifc_ifelm *) M);
                break;

            case MT_IFC_ELSET:
                handle_ifc_elset((M_ifc_ifelm *) M);
                break;

            case MT_IFC_ELATT:
                handle_ifc_elatt((M_ifc_ifelm *) M);
                break;

            case MT_IFC_GRCLR:
                handle_ifc_grclr((M_ifc_ifelm *) M);
                break;

            case MT_IFC_AUPAR:
                break;

            case MT_IFC_DIPAR:
                break;

            case MT_IFC_RETUNE:
                handle_ifc_retune((M_ifc_retune *) M);
                break;

            case MT_IFC_MCSET:
                handle_ifc_mcset((M_ifc_chconf *) M);

                break;

            case MT_IFC_TXTIP:
                handle_ifc_txtip((M_ifc_txtip *) M);
                break;

            case MT_IFC_PRRCL:
                break;

            case MT_IFC_RETUNING_DONE:
                handle_ifc_retuning_done();

            default:
                printf("Received message of unknown type %5ld\n", M->type());
        }
        M->recover();
    }
}

/**
 * @brief Handles the MT_IFC_READY message.
 */
void Tiface::handle_ifc_ready ()
{
        if (_init)
        {
            tIO.handleOutputFromTI("Aeolus is ready");

            print_info ();
            // Reader thread is not needed, this is for the command line
            //_reader.thr_start (SCHED_OTHER, 0, 0);
	        //_reader.read ();
        }
    _init = false;
}

/**
 * @brief Handles the MT_IFC_INIT message.
 * @param M A pointer to the M_ifc_init message.
 */
void Tiface::handle_ifc_init (M_ifc_init *M)
{
    __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                        "Tiface", "Marker 1");
    if (_initdata) _initdata ->recover ();
    _initdata = M_ifc_init::createCopy(M);



}

/**
 * @brief Handles the MT_IFC_MCSET message.
 * @param M A pointer to the M_ifc_chconf message.
 */
void Tiface::handle_ifc_mcset (M_ifc_chconf *M)
{
    if (_mididata) _mididata ->recover ();
    _mididata = M;
    if (!_init) print_midimap ();
}

/**
 * @brief Handles the MT_IFC_RETUNE message.
 * @param M A pointer to the M_ifc_retune message.
 */
void Tiface::handle_ifc_retune (M_ifc_retune *M)
{
    printf ("Retuning Aeolus, A = %3.1lf Hz, %s (%s)\n",
	    M->_freq,
	    _initdata->_temped [M->_temp]._label,
	    _initdata->_temped [M->_temp]._mnemo);
}

/**
 * @brief Handles the MT_IFC_GRCLR message.
 * @param M A pointer to the M_ifc_ifelm message.
 */
void Tiface::handle_ifc_grclr (M_ifc_ifelm *M)
{
    _ifelms [M->_group] = 0;
}

/**
 * @brief Handles the MT_IFC_ELCLR message.
 * @param M A pointer to the M_ifc_ifelm message.
 */
void Tiface::handle_ifc_elclr (M_ifc_ifelm *M)
{
    _ifelms [M->_group] &= ~(1 << M->_ifelm);
}

/**
 * @brief Handles the MT_IFC_ELSET message.
 * @param M A pointer to the M_ifc_ifelm message.
 */
void Tiface::handle_ifc_elset (M_ifc_ifelm *M)
{
    _ifelms [M->_group] |= (1 << M->_ifelm);
}

/**
 * @brief Handles the MT_IFC_ELATT message.
 * @param M A pointer to the M_ifc_ifelm message.
 */
void Tiface::handle_ifc_elatt (M_ifc_ifelm *M)
{
    __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                        "Tiface", "Rewrite label %s",_initdata->_groupd [M->_group]._ifelmd [M->_ifelm]._label);

    rewrite_label (_initdata->_groupd [M->_group]._ifelmd [M->_ifelm]._label);
    char* message;
    asprintf (&message,"Retuning %7s %-1s (%s)\n",
	    _initdata->_groupd [M->_group]._label,
	    _tempstr,
	    _initdata->_groupd [M->_group]._ifelmd [M->_ifelm]._mnemo);
    tIO.handleOutputFromTI(message);
}

/**
 * @brief Handles the MT_IFC_TXTIP message.
 * @param M A pointer to the M_ifc_txtip message.
 */
void Tiface::handle_ifc_txtip (M_ifc_txtip *M)
{
    /* This is the original implementation for a linux command line interpreter
     * Here, in this Android impementation, command line interpretation is disabled
     * because we have no command line anymore */
    if (M->_line == nullptr)
    {
        send_event (EV_EXIT, 1);
	return;
    }

    //parse_command (M->_line);
    //_reader.read ();
}

/**
 * @brief Prints information about the synthesizer.
 */
void Tiface::print_info ()
{
    char* message;
    asprintf (&message,"Application id:  %s", _initdata->_appid);
    tIO.handleOutputFromTI(message);
    delete message;
    asprintf (&message,"Stops directory: %s", _initdata->_stops);
    tIO.handleOutputFromTI(message);
    delete message;
    asprintf (&message,"Stops directory: %s", _initdata->_stops);
    tIO.handleOutputFromTI(message);
    delete message;
    asprintf (&message,"Instrument:      %s", _initdata->_instr);
    tIO.handleOutputFromTI(message);
    delete message;

    print_keybdd ();
    print_divisd ();
    print_midimap ();
}

/**
 * @brief Prints information about the keyboards.
 */
void Tiface::print_keybdd ()
{
    int i, b, k, n;
    char* message;
    asprintf (&message,"Keyboards:");
    tIO.handleOutputFromTI(message);
    delete message;

    for (k = 0; k < NKEYBD; k++)
    {
	n = 0;
	if (_initdata->_keybdd [k]._label [0])
	{
        asprintf (&message," %-7s  midi", _initdata->_keybdd [k]._label);
        tIO.handleOutputFromTI(message);
        delete message;

	    for (i = 0; i < 16; i++)
	    {
		b = _mididata->_bits [i];
		if ((b & 0x1000) && ((b & 7) == k))
		{
            asprintf (&message," %2d", i + 1);
            tIO.handleOutputFromTI(message);
            delete message;
		    n++;
		}
	    }
	    if (!n) tIO.handleOutputFromTI("  -");

	}
    }
}

/**
 * @brief Prints information about the divisions.
 */
void Tiface::print_divisd ()
{
    int i, b, k, n;
    tIO.handleOutputFromTI("Divisions:");

    char* message;

    for (k = 0; k < NDIVIS; k++)
    {
	n = 0;
	if (_initdata->_divisd [k]._label [0])
	{
        asprintf (&message," %-7s  midi", _initdata->_divisd [k]._label);
        tIO.handleOutputFromTI(message);
        delete message;
	    for (i = 0; i < 16; i++)
	    {
		b = _mididata->_bits [i];
		if ((b & 0x2000) && (((b >> 8) & 7) == k))
		{
            asprintf (&message," %2d", i + 1);
            tIO.handleOutputFromTI(message);
            delete message;

		    n++;
		}
	    }
	    if (!n) tIO.handleOutputFromTI("  - ");

	}
    }
}

/**
 * @brief Prints the MIDI routing map.
 */
void Tiface::print_midimap ()
{
    char* message;
    int c, f, k, n;

    tIO.handleOutputFromTI("Midi routing:");

    n = 0;
    for (c = 0; c < 16; c++)
    {
	f = _mididata->_bits [c];
	k = f & 7;
	f >>= 8;
	f >>= 4;
	if (f)
	{
        asprintf (&message," %2d  ", c + 1);
        tIO.handleOutputFromTI(message);
        delete message;

	    if (f & 1) {
            asprintf (&message,"keybd %-7s", _initdata->_keybdd [k]._label);
            tIO.handleOutputFromTI(message);
            delete message;

        }
	    if (f & 2) {
            asprintf (&message,"divis %-7s", _initdata->_divisd [k]._label);
            tIO.handleOutputFromTI(message);
            delete message;

        }
	    if (f & 4) tIO.handleOutputFromTI("instr");
	    printf ("\n");
	    n++;
	}
    }
    if (n == 0)  tIO.handleOutputFromTI(" No channels are assigned.");
}

/**
 * @brief Prints a short list of stops in a group.
 * @param group The group index.
 */
void Tiface::print_stops_short (int group)
{
    int       i, n;
    uint32_t  m;
    char* message;

    rewrite_label (_initdata->_groupd [group]._label);
    asprintf (&message,"Stops in group %s", _tempstr);
    tIO.handleOutputFromTI(message);
    delete message;

    m = _ifelms [group];
    n = _initdata->_groupd [group]._nifelm;
    for (i = 0; i < n; i++)
    {
        asprintf (&message,"  %c %-8s", (m & 1) ? '+' : '-',
                  _initdata->_groupd [group]._ifelmd [i]._mnemo);
        tIO.handleOutputFromTI(message);
        delete message;

	if ((i % 5) == 4) printf ("\n");
	m >>= 1;
    }
    if (n % 5) tIO.handleOutputFromTI(" ");
}

/**
 * @brief Prints a long list of stops in a group.
 * @param group The group index.
 */
void Tiface::print_stops_long (int group)
{
    int       i, n;
    uint32_t  m;
    char* message;

    rewrite_label (_initdata->_groupd [group]._label);
    asprintf (&message,"Stops in group %s", _tempstr);
    tIO.handleOutputFromTI(message);
    delete message;
    m = _ifelms [group];
    n = _initdata->_groupd [group]._nifelm;
    for (i = 0; i < n; i++)
    {
        rewrite_label (_initdata->_groupd [group]._ifelmd [i]._label);
        asprintf (&message,"  %c %-7s %-1s\n", (m & 1) ? '+' : '-',
                  _initdata->_groupd [group]._ifelmd [i]._mnemo, _tempstr);
        tIO.handleOutputFromTI(message);
        delete message;

	m >>= 1;
    }
}

/**
 * @brief Rewrites a label by removing the '$' character.
 * @param p The label to rewrite.
 */
void Tiface::rewrite_label (const char *p)
{
    char *t;

    strcpy (_tempstr, p);
    t = strstr (_tempstr, "-$");
    if (t) strcpy (t, t + 2);
    else
    {
        t = strchr (_tempstr, '$');
        if (t) *t = ' ';
    }
}

/**
 * @brief Parses a command from the user.
 * @param p The command to parse.
 */
void Tiface::parse_command (const char *p)
{
    int c1, c2;

    while (isspace (*p)) p++;
    c1 = *p++;
    if (c1 == 0) return;
    c2 = *p++;
    if (c2 && !isspace (c2))
    {
        tIO.handleOutputFromTI("Bad command");
	return;
    }
    if (c1 == 0) return;
    switch (c1)
    {
    case 'S':
    case 's':
	command_s (p);
	break;

    case 'Q':
    case 'q':
	fclose (stdin);
	break;

    case '!':
	send_event (TO_MODEL, new ITC_mesg (MT_IFC_SAVE));
	break;

    default:
        char* message;
        asprintf (&message,"Unknown command '%c'\n", c1);
            tIO.handleOutputFromTI(message);
            delete message;
    }
}

/**
 * @brief Executes the 's' command.
 * @param p The arguments to the command.
 */
void Tiface::command_s (const char *p)
{
    int  g, i, k, n;
    char s [64];
    char* message;

    if (   (sscanf (p, "%s%n", s, &n) != 1)
	|| ((g = find_group (s)) < 0))
    {
        tIO.handleOutputFromTI("Expected a group name, ? or ??");

	return;
    }
    p += n;
    if (g == NGROUP + 1)
    {
	for (i = 0; i < _initdata->_ngroup; i++) print_stops_short (i);
	return;
    }
    if (g == NGROUP + 2)
    {
	for (i = 0; i < _initdata->_ngroup; i++) print_stops_long (i);
	return;
    }
    if (   (sscanf (p, "%s%n", s, &n) != 1)
	|| ((k = comm1 (s)) < 0))
    {
        tIO.handleOutputFromTI("Expected one of ? ?? + - =");

	return;
    }
    p += n;
    if (k == 0)
    {
	print_stops_short (g);
	return;
    }
    if (k == 1)
    {
	print_stops_long (g);
	return;
    }
    if (k == 4)
    {
        send_event (TO_MODEL, new M_ifc_ifelm (MT_IFC_GRCLR, g, 0));
	k = 2;
    }
    if (k == 2) k = MT_IFC_ELSET;
    else        k = MT_IFC_ELCLR;
    while (sscanf (p, "%s%n", s, &n) == 1)
    {
	i = find_ifelm (s, g);
	if (i < 0){
        asprintf (&message,"No stop '%s' in this group\n", s);
        tIO.handleOutputFromTI(message);
        delete message;

    }
	else send_event (TO_MODEL, new M_ifc_ifelm (k, g, i));
	p += n;
    }
}

/**
 * @brief Finds a group by its name.
 * @param p The name of the group.
 * @return The index of the group, or a negative value if not found.
 */
int Tiface::find_group (const char *p)
{
    int g;

    if (! strcmp (p, "?"))  return NGROUP + 1;
    if (! strcmp (p, "??")) return NGROUP + 2;
    for (g = 0; g < _initdata->_ngroup; g++)
    {
	if (! strcmp (p, _initdata->_groupd [g]._label)) return g;
    }
    return -1;
}

/**
 * @brief Finds an interface element by its name in a group.
 * @param p The name of the interface element.
 * @param g The index of the group.
 * @return The index of the interface element, or a negative value if not found.
 */
int Tiface::find_ifelm (const char *p, int g)
{
    int i, n;

    n = _initdata->_groupd [g]._nifelm;
    for (i = 0; i < n; i++)
    {
	if (! strcmp (p, _initdata->_groupd [g]._ifelmd [i]._mnemo)) return i;
    }
    return -1;
}

/**
 * @brief Finds a command by its name.
 * @param p The name of the command.
 * @return The index of the command, or a negative value if not found.
 */
int Tiface::comm1 (const char *p)
{
    if (! strcmp (p, "?"))  return 0;
    if (! strcmp (p, "??")) return 1;
    if (! strcmp (p, "+"))  return 2;
    if (! strcmp (p, "-"))  return 3;
    if (! strcmp (p, "="))  return 4;
    return -1;
}

/**
 * @brief Checks if the Tiface is initializing.
 * @return True if the Tiface is initializing, false otherwise.
 */
bool Tiface::isInitializing() {
    return _init;
}

/**
 * @brief Gets the number of divisions.
 * @return The number of divisions.
 */
int Tiface::get_n_divisions() {
    if(_initdata==nullptr)
    {
        return 0;
    }
    return _initdata->_ndivis;
}

/**
 * @brief Gets the label for a division.
 * @param division_index The index of the division.
 * @return The label for the division.
 */
const char *Tiface::getLabelForDivision(int division_index) {
    if(_initdata==nullptr)
    {
        return "";
    }
    if((division_index>=get_n_divisions())|(division_index < 0))
    {
        return "";
    }
    return _initdata->_divisd[division_index]._label;
}

/**
 * @brief Handles the MT_IFC_RETUNING_DONE message.
 */
void Tiface::handle_ifc_retuning_done() {

}
