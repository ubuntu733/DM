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
// DAQUIT.H   - definition of the CQuit class. This class implements a dialog 
//              agent which terminates the dialog when the user wants to quit
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
//   [2003-03-19] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "DAQuit.h"
#include "DialogTask/DialogTask.h"

// D: COMMENTS
//    When using the quit discourse agency, the following prompt needs
//    to be defined:
//    - request sure_quit: 
//        i.e. "Are you sure you want to terminate this session?"
//    - inform quitting: i.e. "Okay. Bye bye."

// D: /Quit - terminates the dialog if the user wants to quit
DEFINE_AGENCY( CQuit,
 
    CAN_TRIGGER_DURING_GROUNDING
    
	public:
	virtual string TriggeredByCommands() {
		sTriggeredByCommands = "@[Quit]";
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
        BOOL_USER_CONCEPT(sure_quit, "")
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(AskAreYouSureQuit, CAskAreYouSureQuit, "request_default")
		SUBAGENT(InformQuitting, CInformQuitting, "none")
        SUBAGENT(ExecuteQuit, CExecuteQuit, "none")
    )
    SUCCEEDS_WHEN(IS_FALSE(sure_quit))
    ON_COMPLETION(RESET)
)

// D: /Quit/AskAreYouSureQuit
DEFINE_REQUEST_AGENT( CAskAreYouSureQuit,
    REQUEST_CONCEPT(sure_quit)

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

// D: /Quit/InformQuitting
DEFINE_INFORM_AGENT( CInformQuitting, 
    PROMPT("inform quitting")
)

// D: /Quit/ExecuteQuit
DEFINE_EXECUTE_AGENT( CExecuteQuit,
	virtual TDialogExecuteReturnCode Execute() {
		// return a session completed code
        return dercFinishDialogAndCloseSession;
	}
)

// D: this function registers the startover agent type
void RegisterQuitAgency() {
	DECLARE_AGENT(CQuit)
        DECLARE_AGENT(CAskAreYouSureQuit)
        DECLARE_AGENT(CInformQuitting)
        DECLARE_AGENT(CExecuteQuit)
}