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
// GAFAILREQUEST.CPP - implementation of the FailRequest grounding action class
//                     this class is used for issuing a "sorry" prompt when 
//                     an agent fails and we're moving on with the task in a
//                     different manner. However, if you want the true 
//                     MOVE_ON action (which forces an agent to fail), then 
//                     check out GAMoveOn
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
//   [2004-08-08] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------


#include "GAFailRequest.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the fail request agency
//
//-----------------------------------------------------------------------------

DEFINE_INFORM_AGENT(_CFailRequest,
    IS_NOT_DTS_AGENT()
    
    private:
    // D: the agent for which we are doing the fail request 
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

    // D: the prompt
    virtual string Prompt() {
        // issue the prompt
        if((pDMCore->GetBindingHistorySize() > 1) && 
            (pDMCore->GetBindingResult(-2).bNonUnderstanding)) {
            // issue the subsequent non-understanding prompt
            return "inform nonunderstanding_subsequent_failrequest";
        } else {
            // issue the normal nonunderstanding prompt
            return "inform nonunderstanding_failrequest";
        }
	}
)

//-----------------------------------------------------------------------------
//
// D: Implementation of the fail request action
//
//-----------------------------------------------------------------------------

// A: Constructor with configuration
CGAFailRequest::CGAFailRequest(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the fail request action
string CGAFailRequest::GetName() {
    return "FAIL_REQUEST";
}

// D: Run the action
#pragma warning (disable:4100)
void CGAFailRequest::Run(void *pParams) {
    
    // look for the agency for doing the ask repeat
    _CFailRequest *pFailRequestAgent = (_CFailRequest *)
        AgentsRegistry["/_FailRequest"];
    if(!pFailRequestAgent) {
        // if not found, create it
        pFailRequestAgent = (_CFailRequest *)
            AgentsRegistry.CreateAgent("_CFailRequest", 
            "/_FailRequest");
    	// initialize it
	    pFailRequestAgent->Initialize();
        // and register it
        pFailRequestAgent->Register();
    } else {
        // o/w reset it
        pFailRequestAgent->Reset();
    }
    // set the dynamic agent ID to the name of the agent
    pFailRequestAgent->SetDynamicAgentID(
        ((CMARequest *)pParams)->GetName());
    // sets the agent for which we are doing the fail request
    pFailRequestAgent->SetRequestAgent((CMARequest *)pParams);

    // and add the fail request agent on the stack
    pDMCore->ContinueWith(pGroundingManager, pFailRequestAgent);

}
#pragma warning (default:4100)

// D: Register the agent used by this grounding action
void CGAFailRequest::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CFailRequest"))
        AgentsRegistry.RegisterAgentType("_CFailRequest", 
            _CFailRequest::AgentFactory);
}
