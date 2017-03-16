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
// DMCORE.H - implements the dialog core thread
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
//   [2003-05-13] (dbohus): changed so that configuration parameters are in a 
//                           hash, which gets also logged
//   [2003-02-14] (dbohus): added pGroundingManager agent
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-04-13] (dbohus): moved all functionality out into the DMCoreAgent
//   [2002-01-26] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __DMCORE_H__
#define __DMCORE_H__

//#include <windows.h>
#include "Utils/Utils.h"
#include "Agents/CoreAgents/AllCoreAgents.h"

//-----------------------------------------------------------------------------
// Declarations for the core dialog management agents
//-----------------------------------------------------------------------------
extern CDMCoreAgent			    *pDMCore;
extern COutputManagerAgent	    *pOutputManager;
extern CInteractionEventManagerAgent		*pInteractionEventManager;
extern CTrafficManagerAgent	    *pTrafficManager;
extern CStateManagerAgent	    *pStateManager;
extern CDTTManagerAgent		    *pDTTManager;
extern CGroundingManagerAgent   *pGroundingManager;

// D: the main thread of the dialog core
int DialogCoreThread();

#endif // __DMCORE_H__
