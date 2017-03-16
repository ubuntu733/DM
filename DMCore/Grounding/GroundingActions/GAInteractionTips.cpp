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
// GAINTERACTIONTIPS.CPP - definition of the InteractionTips grounding action
//                         class; this grounding action instructs the user 
//                         about how to speak to the system
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

#include "GAInteractionTips.h"
#include "DialogTask/DialogTask.h"


//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the notify and repeat prompt agency
//
//-----------------------------------------------------------------------------

DEFINE_EXECUTE_AGENT(_CInteractionTips,
private:
    // D: the agent for which we are repeating this prompt
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
        pOutputManager->Output(this, 
			":non-interruptable :non-listening inform nonunderstanding_interaction_tips",
			fsSystem);
        // issue the prompt again
        pOutputManager->Output(pRequestAgent, pRequestAgent->Prompt(), fsUser);
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
// D: Implementation of the notify and repeat prompt action
//
//-----------------------------------------------------------------------------

// D: Constructor with configuration
CGAInteractionTips::CGAInteractionTips(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the notify and repeat prompt action
string CGAInteractionTips::GetName() {
    return "INTERACTION_TIPS";
}

// D: Run the action
void CGAInteractionTips::Run(void *pParams) {
    
    // look for the agency for doing the notify and repeat prompt
    string sAgencyName = FormatString("/_InteractionTips[%s]", 
		    	((CMARequest *)pParams)->GetName().c_str());
    _CInteractionTips *pInteractionTipsAgent = 
        (_CInteractionTips *)AgentsRegistry[sAgencyName];
    if(!pInteractionTipsAgent) {
        // if not found, create it
        pInteractionTipsAgent = (_CInteractionTips *)
            AgentsRegistry.CreateAgent("_CInteractionTips", sAgencyName);
    	// initialize it
	    pInteractionTipsAgent->Initialize();
        // and register it
        pInteractionTipsAgent->Register();
    } else {
        // o/w reset it
        pInteractionTipsAgent->Reset();
    }

    // set the dynamic agent ID to the name of the agent
    pInteractionTipsAgent->SetDynamicAgentID(
        ((CMARequest *)pParams)->GetName());
    // sets the agent for which we are doing the notify_nonunderstanding
    pInteractionTipsAgent->SetRequestAgent((CMARequest *)pParams);

    // and add the explicit confirm agency on the stack
    pDMCore->ContinueWith(pGroundingManager, pInteractionTipsAgent);

}
// D: Register the agent used by this grounding action
void CGAInteractionTips::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CInteractionTips"))
        AgentsRegistry.RegisterAgentType("_CInteractionTips", 
            _CInteractionTips::AgentFactory);
}
