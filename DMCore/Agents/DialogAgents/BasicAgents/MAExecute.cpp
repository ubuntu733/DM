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
// MAEXECUTE.CPP - implementation of the CMAExecute class. This class 
//				   implements the microagent for Execute
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
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2004-04-16] (dbohus):  added grounding models on dialog agents
//   [2003-04-25] (dbohus,
//                 antoine): the execute agent doesn't check for completion on 
//                            execute any more. the core takes care of that
//   [2003-04-10] (dbohus):  fixed execute call on empty call
//   [2003-04-08] (dbohus):  change completion evaluation on execution
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-04-05] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#include "MAExecute.h"
#include "DMCore/Core.h"

//-----------------------------------------------------------------------------
//
// D: the CMAExecute class -- the microagent for Execute
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------------------

// D: default constructor
CMAExecute::CMAExecute(string sAName, 
					   string sAConfiguration, 
					   string sAType): 
	CDialogAgent(sAName, sAConfiguration, sAType) {
}

// D: virtual destructor - does nothing
CMAExecute::~CMAExecute() {
}

//-----------------------------------------------------------------------------
// D: Static function for dynamic agent creation
//-----------------------------------------------------------------------------
CAgent* CMAExecute::AgentFactory(string sAName, string sAConfiguration) {
	return new CMAExecute(sAName, sAConfiguration);
}

//-----------------------------------------------------------------------------
//
// Specialized (overwritten) CDialogAgent methods
//
//-----------------------------------------------------------------------------

// D: Execute function: just executes the call, as given by ExecuteCall()
TDialogExecuteReturnCode CMAExecute::Execute() {

	// call the execute routine
	ExecuteRoutine();

    // increment the execute counter
    IncrementExecuteCounter();

	// and return with continue execution
	return dercContinueExecution;
}

// D: SuccessCriteriaSatisfied: Request agents are completed as soon as 
//    they have executed
bool CMAExecute::SuccessCriteriaSatisfied() {
	return (iExecuteCounter > 0);
}

//-----------------------------------------------------------------------------
//
// Execute Microagent specific methods
//
//-----------------------------------------------------------------------------

// D: the actual routine to be executed by this agent
void CMAExecute::ExecuteRoutine() {
	// first obtain the execute call
	string sExecuteCall = GetExecuteCall();
    
    // if there is a call to be made
    if(sExecuteCall != "") {
	    // call on the galaxy stub agent to execute that particular call
	    pTrafficManager->Call(this, sExecuteCall);
    }
}

// D: Returns the execution call as a string. Derived classes are to 
//    overwrite this
string CMAExecute::GetExecuteCall() {
	// by default, nothing
	return "";
}
