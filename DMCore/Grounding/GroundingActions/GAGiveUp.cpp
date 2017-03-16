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
// GAGIVEUP.CPP - definition of the GiveUp grounding action class; this 
//                this grounding action abandons the current session 
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
//   [2006-01-30] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "GAGiveUp.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the ask repeat agency
//
//-----------------------------------------------------------------------------

DEFINE_EXECUTE_AGENT(_CGiveUp,
    IS_NOT_DTS_AGENT()

    // D: the execute routine
	virtual TDialogExecuteReturnCode Execute() {

        // issue the giveup prompt
        pOutputManager->Output(this, 
            ":non-interruptable :non-listening inform nonunderstanding_giveup", fsFree);
		// finally, return finish dialog
		return dercFinishDialogAndCloseSession;
	}
)

//-----------------------------------------------------------------------------
//
// D: Implementation of the give up action
//
//-----------------------------------------------------------------------------

// Constructor with configuration
CGAGiveUp::CGAGiveUp(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the ask repeat action
string CGAGiveUp::GetName() {
    return "GIVE_UP";
}

// D: Run the action
#pragma warning (disable:4100)
void CGAGiveUp::Run(void *pParams) {
    
    // look for the agency for doing the ask repeat
    _CGiveUp *pGiveUpAgent = (_CGiveUp *)AgentsRegistry["/_GiveUp"];
    if(!pGiveUpAgent) {
        // if not found, create it
        pGiveUpAgent = (_CGiveUp *)
            AgentsRegistry.CreateAgent("_CGiveUp", "/_GiveUp");
	    // initialize it
	    pGiveUpAgent->Initialize();
        // and register it
        pGiveUpAgent->Register();
    } else {
        // o/w just reset it
        pGiveUpAgent->Reset();
    }

	// sets the configuration
	pGiveUpAgent->SetConfiguration(s2sConfiguration);

    // and add the ask repeat agent on the stack
    pDMCore->ContinueWith(pGroundingManager, pGiveUpAgent);
}
#pragma warning (default:4100)

// D: Register the agent used by this grounding action
void CGAGiveUp::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CGiveUp"))
        AgentsRegistry.RegisterAgentType("_CGiveUp", 
            _CGiveUp::AgentFactory);
}
