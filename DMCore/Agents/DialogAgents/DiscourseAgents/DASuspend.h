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
// DASUSPEND.H   - definition of the CSuspend class. This class implements 
//                 the default dialog agency for suspending the dialog task 
//				   execution. 
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
//	 [2004-12-24] (antoine): added the possibility to define a DTMF key to
//                           trigger this agent using the agent configuration
//   [2003-03-10] (dbohus): adapted so that it also informs the user that it's
//                          resuming
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-04-28] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

// D: COMMENTS
//    When using the suspend discourse agent, the following prompt needs to be 
//    defined:
//    - request resume_from_suspend: i.e. "The dialog is currently suspended. 
//      To resume the dialog, please say resume"
//    - inform resuming_from_suspend: i.e. "Okay, we are now resuming the dialog."
//    - explicit_confirm _Suspend_trigger: i.e. "Did you say you wanted to 
//                                               suspend this conversation?"
//
//   Configuration parameters for this library agent:
//    - dtmf_trigger : contains the name of a grammar slot that triggers the
//                     agent, in addition to the standard slot (e.g. DTMF_ONE)
//                     This was included to allow triggering by dtmf keys


#pragma once
#ifndef __DASUSPEND_H__
#define __DASUSPEND_H__

#include "DMCore/Core.h"
#include "DMCore/Agents/AllAgents.h"

// D: this function registers the suspend agent
void RegisterSuspendAgency();

#endif // __DASUSPEND_H__