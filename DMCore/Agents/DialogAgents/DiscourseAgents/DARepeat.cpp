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
// DAREPEAT.CPP -  implementation of the CRepeat class. This class implements 
//                 the default dialog agency for repeat
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
//   [2002-10-20] (dbohus): fixed bug for taking the agent off the stack
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-04-16] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "DARepeat.h"
#include "DialogTask/DialogTask.h"

// D: /Repeat
//    merely calls upon the output manager to repeat the last output
DEFINE_EXECUTE_AGENT( CRepeat, 
	ON_COMPLETION(RESET)

	public:
	virtual string TriggeredByCommands() {
		sTriggeredByCommands = "@[Repeat]";
		// Adds a DTMF key to the trigger expectation if specified
		// in the agent configuration
		if (HasParameter("dtmf_trigger")) {
			sTriggeredByCommands += ", @[" + GetParameterValue("dtmf_trigger") + "]";
		}
		sTriggerCommandsGroundingModelSpec = "";
        return sTriggeredByCommands;
	}
	
	virtual TDialogExecuteReturnCode Execute() {
		// issue a repeat from the output manager
		pOutputManager->Repeat();
		// finally set this agent as completed
		SetCompleted(ctSuccess);
        // and clean it off the execution stack (hacky)
        pDMCore->PopAgentFromExecutionStack(this);
		// and return a request for a new input pass
		return dercTakeFloor;
	}
)

// D: this function registers the repeat agent type
void RegisterRepeatAgent() {
	DECLARE_AGENT(CRepeat)
}