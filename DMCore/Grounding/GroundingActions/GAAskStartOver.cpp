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
// GAASKSTARTOVER.H   - definition of the AskStartOver grounding action class; 
//                      this grounding action asks the user if they want to 
//                      restart the dialog, and restarts the dialog if need be
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
//   [2006-01-24] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "GAAskStartOver.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the ask repeat agency
//
//-----------------------------------------------------------------------------

// D: the main agency for the ask start over non-understanding recovery 
// strategy
DEFINE_AGENCY( _CAskStartOver,
    IS_NOT_DTS_AGENT()
	CONCEPT_BINDING_POLICY(WITHIN_TOPIC_ONLY)
    DEFINE_CONCEPTS(
        BOOL_USER_CONCEPT(want_start_over, ""))
    DEFINE_SUBAGENTS(
        SUBAGENT(_AskStartOverAsk, _CAskStartOverAsk, "none")
        SUBAGENT(_AskStartOverInform, _CAskStartOverInform, "none")
        SUBAGENT(_AskStartOverExecute, _CAskStartOverExecute, "none")
    )
    SUCCEEDS_WHEN(IS_FALSE(want_start_over) ||
                  SUCCEEDED(_AskStartOverExecute))
)

// D: /_AskStartOver/_AskStartOverAsk
DEFINE_REQUEST_AGENT( _CAskStartOverAsk,
    IS_NOT_DTS_AGENT()
    PROMPT(":non-interruptable :non-listening request nonunderstanding_askstartover")
    REQUEST_CONCEPT(want_start_over)

    // A: expects a yes or a no (or the equivalent DTMF keys)
	public:
	virtual string GrammarMapping() {
		string sGrammarMappingAsString = "![Yes]>true, ![No]>false";
		// Adds a DTMF key to the trigger expectation if specified
		// in the agent configuration
		if (pdaParent->HasParameter("dtmf_yes")) {
			sGrammarMappingAsString += ", ![" + pdaParent->GetParameterValue("dtmf_yes") + "]>true";
		}
		if (pdaParent->HasParameter("dtmf_no")) {
			sGrammarMappingAsString += ", ![" + pdaParent->GetParameterValue("dtmf_no") + "]>false";
		}
		return sGrammarMappingAsString;
	}
	
	// A: If a language model for confirmation (yes/no) was specified in the 
	//    configuration, use it otherwise use that of the parent agent
	// J: Renamed LanguageModel -> InputLineConfigurationInitString
	string InputLineConfigurationInitString() {
		string sLM = "";
		if (pdaParent->HasParameter("confirmation_lm")) {
			// LM from the grounding action configuration
			sLM = "set_lm="+pdaParent->GetParameterValue("confirmation_lm");
		}
			
		// If DTMF is in use, expect 1 key at this point
		if (pdaParent->HasParameter("dtmf_yes")) {
			if (sLM.length() > 0) sLM += ", ";
			sLM += "set_dtmf_len=1";
		}

		return sLM;
	}
)

// D: /_AskStartOver/_AskStartOverInform
DEFINE_INFORM_AGENT( _CAskStartOverInform,
    PROMPT("inform nonunderstanding_starting_over")
)

// D: /_AskStartOver/_AskStartOverExecute
DEFINE_EXECUTE_AGENT( _CAskStartOverExecute,
	virtual TDialogExecuteReturnCode Execute() {
        // increment the execute counter
        IncrementExecuteCounter();
		// return a session restart dialog code
		return dercRestartDialog;
	}
)

//-----------------------------------------------------------------------------
//
// D: Implementation of the ask start over action
//
//-----------------------------------------------------------------------------

// D: Constructor with configuration
CGAAskStartOver::CGAAskStartOver(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the ask repeat action
string CGAAskStartOver::GetName() {
    return "ASK_STARTOVER";
}

// D: Run the action
#pragma warning (disable:4100)
void CGAAskStartOver::Run(void *pParams) {
    
    // look for the agency for doing the ask start over
    _CAskStartOver *pAskStartOverAgency = 
        (_CAskStartOver *)AgentsRegistry["/_AskStartOver"];
    if(!pAskStartOverAgency) {
        // if not found, create it
        pAskStartOverAgency = (_CAskStartOver *)
            AgentsRegistry.CreateAgent("_CAskStartOver", "/_AskStartOver");
	    // initialize it
	    pAskStartOverAgency->Initialize();
        // and register it
        pAskStartOverAgency->Register();
    	// now create the grounding model
    	if(s2sConfiguration.find("grounding_model") != s2sConfiguration.end()) {
	        pAskStartOverAgency->A("_AskStartOverAsk").CreateGroundingModel(
	            s2sConfiguration["grounding_model"]);
    	} else {    	
	        pAskStartOverAgency->A("_AskStartOverAsk").CreateGroundingModel(
	            "request_default");
	    }
    } else {
        // o/w just reset it
        pAskStartOverAgency->Reset();
    }

    // set the dynamic agent ID to the name of the agent
    pAskStartOverAgency->SetDynamicAgentID(
        ((CMARequest *)pParams)->GetName());
	// sets the configuration
	pAskStartOverAgency->SetConfiguration(s2sConfiguration);
	// sets the context to be the latest agent in focus
	pAskStartOverAgency->SetContextAgent(pDMCore->GetAgentInFocus());
    // and add the ask start over agent on the stack
    pDMCore->ContinueWith(pGroundingManager, pAskStartOverAgency);
}
#pragma warning (default:4100)

// D: Register the agent used by this grounding action
void CGAAskStartOver::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CAskShortAnswerAndReprompt"))
        AgentsRegistry.RegisterAgentType("_CAskStartOver", 
            _CAskStartOver::AgentFactory);
    if(!AgentsRegistry.IsRegisteredAgentType("_CAskStartOverAsk"))
        AgentsRegistry.RegisterAgentType("_CAskStartOverAsk", 
            _CAskStartOverAsk::AgentFactory);
    if(!AgentsRegistry.IsRegisteredAgentType("_CAskStartOverInform"))
        AgentsRegistry.RegisterAgentType("_CAskStartOverInform", 
            _CAskStartOverInform::AgentFactory);
    if(!AgentsRegistry.IsRegisteredAgentType("_CAskStartOverExecute"))
        AgentsRegistry.RegisterAgentType("_CAskStartOverExecute", 
            _CAskStartOverExecute::AgentFactory);
}