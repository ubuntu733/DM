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
// GAWHATCANISAY.CPP - implementation of the WhatCanISay and 
//                     NotifyAndWhatCanISay grounding action classes;
//                     this grounding action invokes the what_Can_i_say prompt
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

#include "GAWhatCanISay.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the what can i say agency
//
//-----------------------------------------------------------------------------

DEFINE_INFORM_AGENT(_CWhatCanISay,
    IS_NOT_DTS_AGENT();
    
    private:
    // D: the agent for which we are giving the what_can_i_say help
    CDialogAgent *pRequestAgent;

    public:
    // D: set the request agent
    void SetRequestAgent(CDialogAgent *pARequestAgent) {
        pRequestAgent = pARequestAgent;
    }

    // D: return the request agent
    CDialogAgent* GetRequestAgent() {
        return pRequestAgent;
    }

    // D: the execute routine
	virtual TDialogExecuteReturnCode Execute() {
        
        // check if we need to do tips        
        if(GetParameterValue("tips") == "true") {
            pOutputManager->Output(this, 
		        ":non-listening inform nonunderstanding", fsSystem);
            pOutputManager->Output(this, 
		        ":non-listening inform generic_tips", fsSystem);
		// o/w check if we need to do a notify
		} else if(GetParameterValue("notify") == "true") {
            if((pDMCore->GetBindingHistorySize() > 1) && 
                (pDMCore->GetBindingResult(-2).bNonUnderstanding)) {
                // issue the subsequent non-understanding prompt
                vector<COutput*> voTemp = pOutputManager->Output(this, 
                    "inform subsequent_nonunderstanding", fsSystem);
 				voOutputs.insert(voOutputs.end(), voTemp.begin(), voTemp.end());
           } else {
                // issue the inform non-understanding prompt
                vector<COutput*> voTemp = pOutputManager->Output(this, 
                    "inform nonunderstanding", fsSystem);
 				voOutputs.insert(voOutputs.end(), voTemp.begin(), voTemp.end());
            }
		}

        // issue the explain more prompt, but only if we don't have 
        // the terse flag set
        if(GetParameterValue("terse") != "true") {
            vector<COutput*> voTemp = pOutputManager->Output(pRequestAgent, 
                pRequestAgent->CreateVersionedPrompt("explain_more"), fsSystem);
 				voOutputs.insert(voOutputs.end(), voTemp.begin(), voTemp.end());
        }
        
        // depeding on the or_start_over flag
		if(GetParameterValue("or_start_over") != "true") {
            // issue the what can i say prompt
            vector<COutput*> voTemp = pOutputManager->Output(pRequestAgent, 
                pRequestAgent->CreateVersionedPrompt("what_can_i_say"), fsUser);
 			voOutputs.insert(voOutputs.end(), voTemp.begin(), voTemp.end());
		} else {
            // issue the what can i say or start over prompt
            vector<COutput*> voTemp = pOutputManager->Output(pRequestAgent, 
                pRequestAgent->CreateVersionedPrompt(
                    "what_can_i_say_or_startover"), fsUser);
 			voOutputs.insert(voOutputs.end(), voTemp.begin(), voTemp.end());
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
// D: Implementation of the what can i say action
//
//-----------------------------------------------------------------------------

// A: Constructor with configuration
CGAWhatCanISay::CGAWhatCanISay(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the what can i say action
string CGAWhatCanISay::GetName() {
    return "WHAT_CAN_I_SAY";
}

// D: Run the action
void CGAWhatCanISay::Run(void *pParams) {
    
    // look for the agency for doing the what can i say
    string sAgencyName = FormatString("/_WhatCanISay[%s]", 
		    	((CMARequest *)pParams)->GetName().c_str());
    _CWhatCanISay *pWhatCanISayAgent = 
        (_CWhatCanISay *)AgentsRegistry[sAgencyName];
    if(!pWhatCanISayAgent) {
        // if not found, create it
        pWhatCanISayAgent = (_CWhatCanISay *)
            AgentsRegistry.CreateAgent("_CWhatCanISay", sAgencyName);
    	// initialize it
	    pWhatCanISayAgent->Initialize();
        // and register it
        pWhatCanISayAgent->Register();
    } else {
        // o/w reset it
        pWhatCanISayAgent->Reset();
    }

    // set the dynamic agent ID to the name of the agent
    pWhatCanISayAgent->SetDynamicAgentID(
        ((CMARequest *)pParams)->GetName());
    // sets the agent for which we are doing the notify_nonunderstanding
    pWhatCanISayAgent->SetRequestAgent((CMARequest *)pParams);
	// sets the configuration
	pWhatCanISayAgent->SetConfiguration(s2sConfiguration);

	// and add the explicit confirm agency on the stack
    pDMCore->ContinueWith(pGroundingManager, pWhatCanISayAgent);

}

// D: Register the agent used by this grounding action
void CGAWhatCanISay::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CWhatCanISay"))
        AgentsRegistry.RegisterAgentType("_CWhatCanISay", 
            _CWhatCanISay::AgentFactory);
}