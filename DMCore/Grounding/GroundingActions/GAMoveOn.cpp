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
// GAMOVEON.H   - definition of the MoveOn grounding action class; this action 
//                terminates the current request agent with a failure
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
//   [2006-02-15] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------


#include "GAMoveOn.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the fail request agency
//
//-----------------------------------------------------------------------------

DEFINE_EXECUTE_AGENT(_CMoveOn,
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
    virtual TDialogExecuteReturnCode Execute() {        
        // issue the prompt
        if((pDMCore->GetBindingHistorySize() > 1) && 
            (pDMCore->GetBindingResult(-2).bNonUnderstanding)) {
            // issue the subsequent non-understanding prompt
            pOutputManager->Output(this, 
                "inform nonunderstanding_subsequent_moveon", fsFree);
        } else {
            // issue the normal nonunderstanding prompt
            pOutputManager->Output(this, 
                "inform nonunderstanding_moveon", fsFree);
        }
        // terminate the request agent
        pRequestAgent->SetCompleted(ctFailed);
        // increment the execute counter on this agent
        pRequestAgent->IncrementExecuteCounter();
        // set this agent as completed
		SetCompleted(ctSuccess);
        // and clean it off the execution stack (hacky)
        pDMCore->PopAgentFromExecutionStack(this);        
        // and continue
        return dercTakeFloor;
	}
)

//-----------------------------------------------------------------------------
//
// D: Implementation of the fail request action
//
//-----------------------------------------------------------------------------

// A: Constructor with configuration
CGAMoveOn::CGAMoveOn(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the fail request action
string CGAMoveOn::GetName() {
    return "MOVE_ON";
}

// D: Run the action
#pragma warning (disable:4100)
void CGAMoveOn::Run(void *pParams) {
    
    // look for the agency for doing the ask repeat
    _CMoveOn *pMoveOnAgent = (_CMoveOn *)AgentsRegistry["/_MoveOn"];
    if(!pMoveOnAgent) {
        // if not found, create it
        pMoveOnAgent = (_CMoveOn *)
            AgentsRegistry.CreateAgent("_CMoveOn", "/_MoveOn");
    	// initialize it
	    pMoveOnAgent->Initialize();
        // and register it
        pMoveOnAgent->Register();
    } else {
        // o/w reset it
        pMoveOnAgent->Reset();
    }
    // set the dynamic agent ID to the name of the agent
    pMoveOnAgent->SetDynamicAgentID(
        ((CMARequest *)pParams)->GetName());
    // sets the agent for which we are doing the fail request
    pMoveOnAgent->SetRequestAgent((CMARequest *)pParams);

    // and add the fail request agent on the stack
    pDMCore->ContinueWith(pGroundingManager, pMoveOnAgent);

}
#pragma warning (default:4100)

// D: Register the agent used by this grounding action
void CGAMoveOn::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CMoveOn"))
        AgentsRegistry.RegisterAgentType("_CMoveOn", 
            _CMoveOn::AgentFactory);
}