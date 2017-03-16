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
// DASTARTOVER.CPP - implementation of the CStartOver class. This class 
//                   implements a dialog agent which restarts the dialog on
//                   a start-over command
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
//   [2004-12-30] (antoine): added the possibility to define the language
//                           model and/or DTMF keys for confirmation
//	 [2004-12-24] (antoine): added the possibility to define a DTMF key to
//                           trigger this agent using the agent configuration
//   [2003-03-17] (dbohus): added starting over inform
//   [2003-02-13] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "DAStartOver.h"
#include "DialogTask/DialogTask.h"

// D: COMMENTS
//    When using the startover discourse agency, the following prompt needs
//    to be defined:
//    - request sure_start_over: i.e. "Are you sure you want to start over?"
//    - inform starting_over: i.e. "Okay. Starting over."

// D: /StartOver - restarts the dialog on a start-over command
DEFINE_AGENCY( CStartOver,
    CAN_TRIGGER_DURING_GROUNDING
	public:
	virtual string TriggeredByCommands() {
		sTriggeredByCommands = "@[StartOver]";
		// Adds a DTMF key to the trigger expectation if specified
		// in the agent configuration
		if (HasParameter("dtmf_trigger")) {
			sTriggeredByCommands += ", @[" + GetParameterValue("dtmf_trigger") + "]";
		}
		sTriggerCommandsGroundingModelSpec = "";
        return sTriggeredByCommands;
	}
	
	CONCEPT_BINDING_POLICY(WITHIN_TOPIC_ONLY)
    DEFINE_CONCEPTS(
        BOOL_USER_CONCEPT(sure_start_over, "")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(AskAreYouSureStartOver, CAskAreYouSureStartOver, "none")
        SUBAGENT(InformStartingOver, CInformStartingOver, "none")
        SUBAGENT(ExecuteStartOver, CExecuteStartOver, "none")
    )
	ON_INITIALIZATION(
		// (antoine) This is very hacky... The configuration does not 
		// contain the grounding model specification at creation time,  
		// only when Initialize is called by the Run method of the 
		// ExplicitConfirm grounding action, so we create the 
		// grounding model now instead of in the SUBAGENT directive
		// (see GAExplicitConfirm for another case of this)
		if (HasParameter("grounding_model"))
			SubAgents[0]->CreateGroundingModel(GetParameterValue("grounding_model"));
	)
    SUCCEEDS_WHEN(IS_FALSE(sure_start_over) ||
                  SUCCEEDED(ExecuteStartOver))
    ON_COMPLETION(RESET)
)

// D: /StartOver/AskAreYouSureStartOver
DEFINE_REQUEST_AGENT( CAskAreYouSureStartOver,
    REQUEST_CONCEPT(sure_start_over)

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

// D: /StartOver/InformStartingOver
DEFINE_INFORM_AGENT( CInformStartingOver,
    PROMPT("inform starting_over")
)

// D: /StartOver/ExecuteStartOver
DEFINE_EXECUTE_AGENT( CExecuteStartOver,
	virtual TDialogExecuteReturnCode Execute() {
        // increment the execute counter
        IncrementExecuteCounter();
		// return a session restart dialog code
		return dercRestartDialog;
	}
)

// D: this function registers the startover agent type
void RegisterStartOverAgency() {
	DECLARE_AGENT(CStartOver)
        DECLARE_AGENT(CAskAreYouSureStartOver)
        DECLARE_AGENT(CInformStartingOver)
        DECLARE_AGENT(CExecuteStartOver)
}