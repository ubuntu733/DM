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
// GAYIELDTURN.CPP   - implementation of the YieldTurn grounding action class; 
//                      this grounding action does nothing, essentially 
//                      yielding the turn back to the user
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
//   [2004-07-19] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "GAYieldTurn.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the yield turn agency
//
//-----------------------------------------------------------------------------

DEFINE_EXECUTE_AGENT(_CYieldTurn,
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
        // increment the execute counter on that agent
        pRequestAgent->IncrementExecuteCounter();
        // set this agent as completed
		SetCompleted(ctSuccess);
        // and clean it off the execution stack (hacky)
        pDMCore->PopAgentFromExecutionStack(this);
		// finally, return a request for a new input pass
		return dercYieldFloor;
	}
)

//-----------------------------------------------------------------------------
//
// D: Implementation of the yield turn action
//
//-----------------------------------------------------------------------------

// Constructor with configuration
CGAYieldTurn::CGAYieldTurn(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the yield turn action
string CGAYieldTurn::GetName() {
    return "YIELD_TURN";
}

// D: Run the action
void CGAYieldTurn::Run(void *pParams) {
    
    // look for the agency for doing the repeat prompt
    string sAgencyName = FormatString("/_YieldTurn[%s]", 
		    	((CMARequest *)pParams)->GetName().c_str());
    _CYieldTurn *pYieldTurnAgent = 
        (_CYieldTurn *)AgentsRegistry[sAgencyName];
    if(!pYieldTurnAgent) {
        // if not found, create it
        pYieldTurnAgent = (_CYieldTurn *)
            AgentsRegistry.CreateAgent("_CYieldTurn", sAgencyName);
    	// initialize it
	    pYieldTurnAgent->Initialize();
        // and register it
        pYieldTurnAgent->Register();
    } else {
        // o/w reset it
        pYieldTurnAgent->Reset();
    }

    // set the dynamic agent ID to the name of the agent
    pYieldTurnAgent->SetDynamicAgentID(
        ((CMARequest *)pParams)->GetName());
    // sets the agent for which we are doing the notify_nonunderstanding
    pYieldTurnAgent->SetRequestAgent((CMARequest *)pParams);

    // and add the yield turn agency on the stack
    pDMCore->ContinueWith(pGroundingManager, pYieldTurnAgent);

}

// D: Register the agent used by this grounding action
void CGAYieldTurn::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CYieldTurn"))
        AgentsRegistry.RegisterAgentType("_CYieldTurn", 
            _CYieldTurn::AgentFactory);
}