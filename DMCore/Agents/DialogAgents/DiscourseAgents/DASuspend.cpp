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
// DASUSPEND.CPP - specification of the CSuspend class. This class implements 
//                 the default dialog agency for suspending the dialog task 
//				   execution. 
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
//	 [2004-12-24] (antoine): added the possibility to define a DTMF key to
//                           trigger this agent using the agent configuration
//   [2003-03-10] (dbohus): adapted so that it also informs the user that it's
//                          resuming
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-04-28] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

// D: COMMENTS
//    When using the suspend discourse agent, the following prompt needs to be 
//    defined:
//    - request resume_from_suspend: i.e. "The dialog is currently suspended. 
//      To resume the dialog, please say resume"
//    - inform resuming_from_suspend: i.e. "Okay, we are now resuming the dialog."
//    - explicit_confirm _Suspend_trigger: i.e. "Did you say you wanted to 
//                                               suspend this conversation?"

#include "DASuspend.h"
#include "DialogTask/DialogTask.h"

// D: /Suspend
//    defines the Suspend Agency. this agency is activated on a Suspend request, 
//    and will just block everything until it hears a resume
DEFINE_AGENCY( CSuspend, 
	PRECONDITION(false)
    CAN_TRIGGER_DURING_GROUNDING
	public:
	virtual string TriggeredByCommands() {
		sTriggeredByCommands = "@[Suspend]";
		// Adds a DTMF key to the trigger expectation if specified
		// in the agent configuration
		if (HasParameter("dtmf_trigger")) {
			sTriggeredByCommands += ", @[" + GetParameterValue("dtmf_trigger") + "]";
		}
		sTriggerCommandsGroundingModelSpec = "expl";
        return sTriggeredByCommands;
	}
	
	IS_MAIN_TOPIC()
	CONCEPT_BINDING_POLICY(WITHIN_TOPIC_ONLY)
	DEFINE_SUBAGENTS(
		SUBAGENT(RequestResume, CRequestResume, "none")
        SUBAGENT(InformResuming, CInformResuming, "none")
	)
	DEFINE_CONCEPTS(
		BOOL_USER_CONCEPT(resume_from_suspend, "none")
	)
	ON_COMPLETION(
		Reset();
	)
)

// D: /Suspend/RequestResume
//    this agent will request a resume 
DEFINE_REQUEST_AGENT( CRequestResume,
	REQUEST_CONCEPT(resume_from_suspend)
	PROMPT(OAAS("say([request_resume_from_suspend])")
		   GALS("request resume_from_suspend"))
	GRAMMAR_MAPPING("![Resume]>true, ![StartOver]>true, ![Continue]>true")
	TIMEOUT_PERIOD(1000000)
)

// D: /Suspend/InformResuming
//    this agent will inform the user that we are resuming the execution
DEFINE_INFORM_AGENT( CInformResuming,
	PROMPT(OAAS("say([resuming_from_suspend])")
		   GALS("inform resuming_from_suspend"))
)

// A: /Suspend/InformContext
//    this agent will reestablish the dialog context
DEFINE_INFORM_AGENT( CInformContext,
	PRECONDITION(pDMCore->GetDTSAgentPreviouslyInFocus(this)->EstablishContextPrompt() != "")

	public:
	virtual TDialogExecuteReturnCode Execute() {
		// on completion, reestablish the context
		// obtain the previously focused agent
		CDialogAgent* pdaDTSFocused = 
            pDMCore->GetDTSAgentPreviouslyInFocus(this);

		// and issue it's establish_context prompt, if it has one
		pOutputManager->Output(pdaDTSFocused, 
			pdaDTSFocused->EstablishContextPrompt(), fsFree);

		return dercTakeFloor;
	}
)

// D: Registering function for the suspend agency components
void RegisterSuspendAgency() {
	DECLARE_AGENT(CSuspend)
		DECLARE_AGENT(CRequestResume)
        DECLARE_AGENT(CInformResuming)
		DECLARE_AGENT(CInformContext)
}