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
// GANOTIFYNONUNDERSTANDING.CPP - implementation of the NotifyNonunderstanding 
//                                grounding action class; 
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
//   [2004-02-23] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------


#include "GANotifyNonunderstanding.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the notify nonunderstanding agency
//
//-----------------------------------------------------------------------------

DEFINE_EXECUTE_AGENT(_CNotifyNonunderstanding,
    IS_NOT_DTS_AGENT()

    private:
    // D: the agent for which we are doing the explainmore prompt
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
        // issue the prompt
        if((pDMCore->GetBindingHistorySize() > 1) && 
            (pDMCore->GetBindingResult(-2).bNonUnderstanding)) {
            // issue the subsequent non-understanding prompt
            pOutputManager->Output(this, 
                "inform subsequent_nonunderstanding", fsUser);
        } else {
            // issue the inform non-understanding prompt
            pOutputManager->Output(this, 
                "inform nonunderstanding", fsUser);
        }
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
// D: Implementation of the notify nonunderstanding action
//
//-----------------------------------------------------------------------------

// A: Constructor with configuration
CGANotifyNonunderstanding::CGANotifyNonunderstanding(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the notify nonunderstanding action
string CGANotifyNonunderstanding::GetName() {
    return "NOTIFY_NONUNDERSTANDING";
}

// D: Run the action
#pragma warning (disable:4100)
void CGANotifyNonunderstanding::Run(void *pParams) {
    
    // look for the agency for doing the ask repeat
    _CNotifyNonunderstanding *pNotifyNonunderstandingAgent = (_CNotifyNonunderstanding *)
        AgentsRegistry["/_NotifyNonunderstanding"];
    if(!pNotifyNonunderstandingAgent) {
        // if not found, create it
        pNotifyNonunderstandingAgent = (_CNotifyNonunderstanding *)
            AgentsRegistry.CreateAgent("_CNotifyNonunderstanding", 
            "/_NotifyNonunderstanding");
    	// initialize it
	    pNotifyNonunderstandingAgent->Initialize();
        // and register it
        pNotifyNonunderstandingAgent->Register();
    } else {
        // o/w reset it
        pNotifyNonunderstandingAgent->Reset();
    }
    // set the dynamic agent ID to the name of the agent
    pNotifyNonunderstandingAgent->SetDynamicAgentID(
        ((CMARequest *)pParams)->GetName());
    // sets the agent for which we are doing the notify_nonunderstanding
    pNotifyNonunderstandingAgent->SetRequestAgent((CMARequest *)pParams);

    // and add the notify nonunderstanding agent on the stack
    pDMCore->ContinueWith(pGroundingManager, pNotifyNonunderstandingAgent);

}
#pragma warning (default:4100)

// D: Register the agent used by this grounding action
void CGANotifyNonunderstanding::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CNotifyNonunderstanding"))
        AgentsRegistry.RegisterAgentType("_CNotifyNonunderstanding", 
            _CNotifyNonunderstanding::AgentFactory);
}
