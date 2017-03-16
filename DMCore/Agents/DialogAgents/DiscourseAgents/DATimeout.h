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
// DATIMEOUT.CPP -  definition of the CTimeoutSuspend and CTimeoutTerminate
//					agents. These agents implements a default dialog behavior 
//					for handling timeouts: upon successive timeouts the system 
//					will enter suspend mode (or alternatively terminate the
//					execution)
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
//	 [2005-02-08] (antoine): added a Sleep before TT_Terminate returns so that
//							 the system doesn't hang up before saying its last
//							 utterance
//   [2003-03-28] (antoine): changed the are_you_still_there request agent
//                           to an open request agent
//   [2002-10-18] (dbohus): added timeout/terminate agency
//   [2002-07-16] (dbohus): deemed stable version 0.5
//   [2002-07-05] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

// D: COMMENTS
//    When using the timeout discourse agents, the following prompts need to be 
//    defined:
//    - inform you_not_there: i.e. "I assume you are no longer there."
//    - inform sorry_i_will_shutdown: i.e. "I am closing this session now. Bye bye."
//    - request are_you_still_there: i.e. "Are you still there?"
//    - request resume_after_timeout: i.e. "The session is currently suspended.
//      To resume the dialog, please say \"Resume\""

#pragma once
#ifndef __DATIMEOUT_H__
#define __DATIMEOUT_H__

#include "DMCore/Core.h"
#include "DMCore/Agents/AllAgents.h"

// D: this function registers the timeout/suspend agency and all it's subagents 
//    types with the registry
void RegisterTimeoutSuspendAgency();

// D: this function registers the timeout/terminate agency and all it's 
//    subagents types with the registry
void RegisterTimeoutTerminateAgency();

#endif // __DATIMEOUT_H__