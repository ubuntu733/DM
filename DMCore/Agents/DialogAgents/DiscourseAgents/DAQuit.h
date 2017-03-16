//=============================================================================
//
//   Copyright (c) 2000-2004, Carnegie Mellon University.  
//   All rights reserved.
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer. 
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in
//      the documentation and/or other materials provided with the
//      distribution.
//
//   This work was supported in part by funding from the Defense Advanced 
//   Research Projects Agency and the National Science Foundation of the 
//   United States of America, and the CMU Sphinx Speech Consortium.
//
//   THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
//   ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
//   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//   PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
//   NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//=============================================================================

//-----------------------------------------------------------------------------
// 
// DAQUIT.H   - definition of the CQuit class. This class implements a dialog 
//              agent which terminates the dialog when the user wants to quit
// 
// ----------------------------------------------------------------------------
// 
// BEFORE MAKING CHANGES TO THIS CODE, please read the appropriate 
// documentation, available in the Documentation folder. 
//
// ANY SIGNIFICANT CHANGES made should be reflected back in the documentation
// file(s)
//
// ANY CHANGES made (even small bug fixes, should be reflected in the history
// below, in reverse chronological order
// 
// HISTORY --------------------------------------------------------------------
//
//   [2004-12-30] (antoine): added the possibility to define the language
//                           model and/or DTMF keys for confirmation
//	 [2004-12-24] (antoine): added the possibility to define a DTMF key to
//                           trigger this agent using the agent configuration
//   [2003-03-19] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __DAQUIT_H__
#define __DAQUIT_H__

// D,A: COMMENTS
//    When using the quit discourse agency, the following prompt needs
//    to be defined:
//    - request sure_quit: 
//        i.e. "Are you sure you want to terminate this session?"
//    - inform quitting: i.e. "Okay. Bye bye."
//
//   Configuration parameters for this library agent:
//    - dtmf_trigger : contains the name of a grammar slot that triggers the
//                     agent, in addition to the standard slot (e.g. DTMF_ONE)
//                     This was included to allow triggering by dtmf keys.
//	  - confirmation_lm : the name of a language model to be used by the yes/no
//                        confirmation request agent.
//    - dtmf_yes/dtmf_no : the names of grammar slots that map to resp. yes 
//                         and no in the confirmation request 
//                         (e.g. dtmf_yes=DTMF_ONE, dtmf_no=DTMF_THREE)


#include "DMCore/Core.h"
#include "DMCore/Agents/AllAgents.h"

// D: this function registers the terminate agent 
void RegisterQuitAgency();

#endif // __DAQUIT_H__