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
// DATERMINATE.CPP - implementation of the CTerminate class. This class 
//                   implements a dialog agent which terminates the session
//                   upon receiving a TERMINATE signal from the input
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
//   [2002-10-07] (dbohus): wrote this, deemed stable version 0.5
// 
//-----------------------------------------------------------------------------

#include "DATerminate.h"
#include "DialogTask/DialogTask.h"


// D: /Terminate
//    terminates the session by returning dercFinishDialog when the input is
//    [TERMINATE_SESSION]
DEFINE_EXECUTE_AGENT( CTerminate, 
	CAN_TRIGGER_DURING_GROUNDING
	TRIGGERED_BY_COMMANDS("*[session:"TERMINATE_SESSION"]", "")
	virtual TDialogExecuteReturnCode Execute() {
		// return a session complete code
		return dercFinishDialog;
	}
)

// D: this function registers the terminate agent type
void RegisterTerminateAgent() {
	DECLARE_AGENT(CTerminate)
}

// D: /TerminateAndCloseSession
//    terminates the session by returning dercFinishDialogAndCloseSession when the input is
//    [TERMINATE_SESSION]
DEFINE_EXECUTE_AGENT( CTerminateAndCloseSession, 
	CAN_TRIGGER_DURING_GROUNDING
	TRIGGERED_BY_COMMANDS("*[session:"TERMINATE_SESSION"]", "")
	virtual TDialogExecuteReturnCode Execute() {
		// return a session complete code
		return dercFinishDialogAndCloseSession;
	}
)

// D: this function registers the terminate agent type
void RegisterTerminateAndCloseSessionAgent() {
	DECLARE_AGENT(CTerminateAndCloseSession)
}