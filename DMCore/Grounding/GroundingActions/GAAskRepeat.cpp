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
// GAASKREPEAT.CPP - implementation of the AskRepeat and NotifyAndAskRepeat 
//                   grounding action classes; this grounding action asks the 
//                   user to repeat what they said
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
//   [2004-12-28] (antoine): added constructor with configuration
//   [2004-03-01] (dbohus): added GNotifyAndAskRepeat
//   [2004-02-16] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "GAAskRepeat.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the ask repeat agency
//
//-----------------------------------------------------------------------------

DEFINE_EXECUTE_AGENT(_CAskRepeat,
    IS_NOT_DTS_AGENT()

    private:
    // the agent for which we are doing the ask repeat prompt
    CMARequest *pRequestAgent;
    
    public:
    // D: set the request agent
    void SetRequestAgent(CMARequest *pARequestAgent) {
        pRequestAgent = pARequestAgent;
    }

    // D: return the request agent
    CMARequest* GetRequestAgent() {
        return pRequestAgent;
    }
    
    // D: the execute routine
	virtual TDialogExecuteReturnCode Execute() {

        // if we need to do notification, do it
        if(GetParameterValue("notify") == "true") {
            if((pDMCore->GetBindingHistorySize() > 1) && 
                (pDMCore->GetBindingResult(-2).bNonUnderstanding)) {
                // issue the subsequent non-understanding prompt
                pOutputManager->Output(this, 
                    "inform subsequent_nonunderstanding", fsSystem);
            } else {
                // issue the inform non-understanding prompt
                pOutputManager->Output(this, 
                    "inform nonunderstanding", fsSystem);
            }
        }

        // issue the ask repeat prompt
        pOutputManager->Output(this, 
            "request nonunderstanding_askrepeat", fsUser);
        // increment the execute counter on that agent
        pRequestAgent->IncrementExecuteCounter();
        // set this agent as completed
		SetCompleted(ctSuccess);
        // and clean it off the execution stack (hacky)
        pDMCore->PopAgentFromExecutionStack(this);
		// finally, return a request for a new input pass
		return dercTakeFloor;
	}
)

//-----------------------------------------------------------------------------
//
// D: Implementation of the ask repeat action
//
//-----------------------------------------------------------------------------

// Constructor with configuration
CGAAskRepeat::CGAAskRepeat(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the ask repeat action
string CGAAskRepeat::GetName() {
    return "ASK_REPEAT";
}

// D: Run the action
#pragma warning (disable:4100)
void CGAAskRepeat::Run(void *pParams) {
    
    // look for the agency for doing the ask repeat
    _CAskRepeat *pAskRepeatAgent = (_CAskRepeat *)AgentsRegistry["/_AskRepeat"];
    if(!pAskRepeatAgent) {
        // if not found, create it
        pAskRepeatAgent = (_CAskRepeat *)
            AgentsRegistry.CreateAgent("_CAskRepeat", "/_AskRepeat");
	    // initialize it
	    pAskRepeatAgent->Initialize();
        // and register it
        pAskRepeatAgent->Register();
    } else {
        // o/w just reset it
        pAskRepeatAgent->Reset();
    }

    // set the dynamic agent ID to the name of the agent
    pAskRepeatAgent->SetDynamicAgentID(
        ((CMARequest *)pParams)->GetName());    
    // sets the agent for which we are doing the ask_repeat
    pAskRepeatAgent->SetRequestAgent((CMARequest *)pParams);
	// sets the configuration
	pAskRepeatAgent->SetConfiguration(s2sConfiguration);

    // and add the ask repeat agent on the stack
    pDMCore->ContinueWith(pGroundingManager, pAskRepeatAgent);
}
#pragma warning (default:4100)

// D: Register the agent used by this grounding action
void CGAAskRepeat::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CAskRepeat"))
        AgentsRegistry.RegisterAgentType("_CAskRepeat", 
            _CAskRepeat::AgentFactory);
}
