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
// DATIMEOUT.CPP -  definition of the CTimeoutSuspend and CTimeoutTerminate
//					agents. These agents implements a default dialog behavior 
//					for handling timeouts: upon successive timeouts the system 
//					will enter suspend mode (or alternatively terminate the
//					execution)
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
//	 [2005-02-08] (antoine): added a Sleep before TT_Terminate returns so that
//							 the system doesn't hang up before saying its last
//							 utterance
//   [2003-03-28] (antoine): changed the are_you_still_there request agent
//                           to an open request agent
//   [2002-10-18] (dbohus): added timeout/terminate agency
//   [2002-07-16] (dbohus): deemed stable version 0.5
//   [2002-07-05] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

// D: COMMENTS
//    When using the timeout discourse agents, the following prompts need to be 
//    defined:
//    - inform you_not_there: i.e. "I assume you are no longer there."
//    - sorry_i_will_shutdown: i.e. "I am closing this session now. Bye bye."
//    - request are_you_still_there: i.e. "Are you still there?"
//    - request resume_after_timeout: i.e. "The session is currently suspended.
//      To resume the dialog, please say \"Resume\""

#include "DATimeout.h"
#include "DialogTask/DialogTask.h"

//-----------------------------------------------------------------------------
//
// D: The TimeoutSuspend agency: on successive timeouts it attempts to 
//    reestablish the channel, after which it suspends
//
//-----------------------------------------------------------------------------

// D: /TimeoutSuspend
//    The agency handles timeouts, and suspends the execution when the user is
//    not responding any more
DEFINE_AGENCY( CTimeoutSuspend,
	PRECONDITION(false)
	DEFINE_CONCEPTS(
		INT_SYSTEM_CONCEPT(LastTimeoutTurnNumber)
	)
	DEFINE_SUBAGENTS(
		SUBAGENT(TS_HandleFirstTimeout, CTS_HandleFirstTimeout, "none")
		SUBAGENT(TS_ReestablishChannel, CTS_ReestablishChannel, "none")
	)
	ON_CREATION(C("LastTimeoutTurnNumber") = -1)
)

// D: /TimeoutSuspend/TS_HandleFirstTimeout
//    Handles the first aparition of a timeout by issuing the timeout prompt'
//    of the previously focused agent
DEFINE_EXECUTE_AGENT( CTS_HandleFirstTimeout, 
	EXPECT_WHEN(pDMCore->GetLastInputTurnNumber() >
				(int)C("LastTimeoutTurnNumber"))
    CAN_TRIGGER_DURING_GROUNDING
	//TRIGGERED_BY_COMMANDS("@"TIMEOUT_ELAPSED, "none")
	ON_COMPLETION(RESET)
	virtual TDialogExecuteReturnCode Execute() {
		// obtain a pointer to the previously focused agent 
		CDialogAgent* pdaFocused = (CDialogAgent *)
            AgentsRegistry[pStateManager->GetLastState().sFocusedAgentName];
		// and its timeout prompt
		string sPrompt = pdaFocused->CreateVersionedPrompt("timeout");
		// back off to the regular prompt if timeout prompt undefined
		if(sPrompt == "") 
			sPrompt = pdaFocused->Prompt();
		// issue the prompt
		pOutputManager->Output(pdaFocused, sPrompt, fsUser);
		// remember the last timeout number
		C("LastTimeoutTurnNumber") = pDMCore->GetLastInputTurnNumber();
		// set this agent as completed
		SetCompleted(ctSuccess);
        // pop it off the execution stack
        pDMCore->PopAgentFromExecutionStack(this);
		// and return a request for a new input pass
		return dercTakeFloor;
	}
)

// D: /TimeoutSuspend/TS_ReestablishChannel
//    On a second consecutive timeout, this agent will initiate a
//    channel reestablishment dialog, and eventually suspend execution if
//    the user is not there
DEFINE_AGENCY( CTS_ReestablishChannel,
	EXPECT_WHEN(pDMCore->GetLastInputTurnNumber() ==
				(int)C("LastTimeoutTurnNumber"))
	CAN_TRIGGER_DURING_GROUNDING
//	TRIGGERED_BY_COMMANDS("@"TIMEOUT_ELAPSED, "none")
	IS_MAIN_TOPIC()
	ON_COMPLETION(RESET)
	DEFINE_CONCEPTS(
		BOOL_USER_CONCEPT(answer_are_you_there, "")
	)
	DEFINE_SUBAGENTS(
		SUBAGENT(TS_RequestAreYouThere, CTS_RequestAreYouThere, "none")
		SUBAGENT(TS_ExpectAnswerAreYouThere, CTS_ExpectAnswerAreYouThere, "none")
		SUBAGENT(TS_SuspendOnTimeout, CTS_SuspendOnTimeout, "none")
	)
	SUCCEEDS_WHEN(SUCCEEDED(TS_RequestAreYouThere) ||
				  COMPLETED(TS_SuspendOnTimeout))
)

// D: /TimeoutSuspend/TS_ReestablishChannel/TS_RequestAreYouThere
//    Asks the user if he is there
DEFINE_REQUEST_AGENT( CTS_RequestAreYouThere, 
	PROMPT(OAAS("say([are_you_still_there])")
		   GALS("request are_you_still_there"))
	TIMEOUT_PERIOD(10)
)

// D: /TimeoutSuspend/TS_ReestablishChannel/TS_ExpectAnswerAreYouThere
//    Grabs the eventual "Yes" answer from the user
DEFINE_EXPECT_AGENT( CTS_ExpectAnswerAreYouThere,
	EXPECT_CONCEPT(answer_are_you_there)
	GRAMMAR_MAPPING( "[Yes]>true")
)

// D: /TimeoutSuspend/TS_ReestablishChannel/TS_SuspendOnTimeout
//    Suspends execution on the third successive timeout
DEFINE_AGENCY( CTS_SuspendOnTimeout, 
	PRECONDITION(false)
	CAN_TRIGGER_DURING_GROUNDING
	CONCEPT_BINDING_POLICY(WITHIN_TOPIC_ONLY)
	TRIGGERED_BY_COMMANDS(TIMEOUT_ELAPSED, "none")
	DEFINE_CONCEPTS(
		BOOL_USER_CONCEPT(resume, "")
	)
	DEFINE_SUBAGENTS(
		SUBAGENT(TS_InformYouNotThere, CTS_InformYouNotThere, "none")
		SUBAGENT(TS_RequestTimeoutResume, CTS_RequestTimeoutResume, "none")
	)
	SUCCEEDS_WHEN(AVAILABLE(resume))
	ON_COMPLETION(
		A("..").SetCompleted(ctSuccess);
	)
)

// D: /TimeoutSuspend/TS_ReestablishContext/TS_SuspendOnTimeout/TS_InformYouNotThere
DEFINE_INFORM_AGENT( CTS_InformYouNotThere,
	PROMPT(OAAS("say([you_not_there])")
		   GALS("inform you_not_there"))
)

// D: /TimeoutSuspend/TS_ReestablishContext/TS_SuspendOnTimeout/TS_RequestTimeoutResume
DEFINE_REQUEST_AGENT( CTS_RequestTimeoutResume,
	REQUEST_CONCEPT(resume)
	PROMPT(OAAS("say([request_resume_after_timeout])")
	       GALS("request resume_after_timeout"))
	GRAMMAR_MAPPING("![continue]>true, ![resume]>true")
	TIMEOUT_PERIOD(10000000)
)

// D: Function for registering the timeout agents
void RegisterTimeoutSuspendAgency() {
	DECLARE_AGENT(CTimeoutSuspend)
		DECLARE_AGENT(CTS_HandleFirstTimeout)
		DECLARE_AGENT(CTS_ReestablishChannel)
			DECLARE_AGENT(CTS_RequestAreYouThere)
			DECLARE_AGENT(CTS_ExpectAnswerAreYouThere)
			DECLARE_AGENT(CTS_SuspendOnTimeout)
				DECLARE_AGENT(CTS_InformYouNotThere)
				DECLARE_AGENT(CTS_RequestTimeoutResume)
}


//-----------------------------------------------------------------------------
//
// D: The TimeoutTerminate agency: on successive timeouts it attempts to 
//    reestablish the channel, after which it eventually terminates
//    the dialog
//
//-----------------------------------------------------------------------------

// D: /TimeoutTerminate
DEFINE_AGENCY( CTimeoutTerminate,
    IS_NOT_DTS_AGENT()

	PRECONDITION(false)
	DEFINE_CONCEPTS(
		INT_SYSTEM_CONCEPT(LastTimeoutTurnNumber)
	)
	DEFINE_SUBAGENTS(
		SUBAGENT(TT_HandleFirstTimeout, CTT_HandleFirstTimeout, "none")
		SUBAGENT(TT_ReestablishChannel, CTT_ReestablishChannel, "none")
	)
	ON_CREATION(C("LastTimeoutTurnNumber") = -1)
)

// D: /TimeoutTerminate/TT_HandleFirstTimeout
//    Handles the first aparition of a timeout by issuing the timeout prompt'
//    of the previously focused agent
DEFINE_EXECUTE_AGENT( CTT_HandleFirstTimeout, 
    IS_NOT_DTS_AGENT()

	EXPECT_WHEN(pDMCore->GetLastInputTurnNumber() >
				(int)C("LastTimeoutTurnNumber"))
	//TRIGGERED_BY_COMMANDS("@"TIMEOUT_ELAPSED, "none")
	ON_COMPLETION(RESET)
	virtual TDialogExecuteReturnCode Execute() {
		// obtain a pointer to the previously focused agent 
		CDialogAgent* pdaFocused = (CDialogAgent *)
            AgentsRegistry[pStateManager->GetLastState().sFocusedAgentName];
		// and its timeout prompt
		string sPrompt = pdaFocused->CreateVersionedPrompt("timeout");
		// back off to the regular prompt if timeout prompt undefined
		if(sPrompt == "") 
			sPrompt = pdaFocused->Prompt();
		// issue the prompt
		pOutputManager->Output(pdaFocused, sPrompt, fsUser);
		// remember the last timeout number
		C("LastTimeoutTurnNumber") = pDMCore->GetLastInputTurnNumber();
		// set this agent as completed
		SetCompleted(ctSuccess);
        // pop itself from execution stack
        pDMCore->PopAgentFromExecutionStack(this);
		// and return a request for a new input pass
		return dercTakeFloor;
	}
)

// D: /TimeoutTerminate/TT_ReestablishChannel
//    On a second consecutive timeout, this agent will initiate a
//    channel reestablishment dialog, and eventually terminate execution if
//    the user is not there
DEFINE_AGENCY( CTT_ReestablishChannel,
    IS_NOT_DTS_AGENT()

	EXPECT_WHEN(iExecuteCounter==0)
	//TRIGGERED_BY_COMMANDS("@"TIMEOUT_ELAPSED, "none")
	IS_MAIN_TOPIC()
	ON_COMPLETION(RESET)
	DEFINE_CONCEPTS(
		BOOL_USER_CONCEPT(answer_are_you_there, "")
	)
	DEFINE_SUBAGENTS(
		SUBAGENT(TT_RequestAreYouThere, CTT_RequestAreYouThere, "none")
		SUBAGENT(TT_ExpectAnswerAreYouThere, CTT_ExpectAnswerAreYouThere, "none")
		SUBAGENT(TT_TerminateOnTimeout, CTT_TerminateOnTimeout, "none")
	)
	SUCCEEDS_WHEN(
		SUCCEEDED(TT_RequestAreYouThere) ||
		SUCCEEDED(TT_TerminateOnTimeout)
	)
)

// D: /TimeoutTerminate/TT_ReestablishChannel/TT_RequestAreYouThere
//    Asks the user if he is there
DEFINE_REQUEST_AGENT( CTT_RequestAreYouThere,
    IS_NOT_DTS_AGENT()

	PROMPT(OAAS("say([are_you_still_there])")
		   GALS("request are_you_still_there"))
	TIMEOUT_PERIOD(10)
)

// D: /TimeoutSuspend/TT_ReestablishChannel/TT_RequestAreYouThere
//    Grabs the eventual "Yes" answer from the user
DEFINE_EXPECT_AGENT( CTT_ExpectAnswerAreYouThere,
    IS_NOT_DTS_AGENT()

	EXPECT_CONCEPT(answer_are_you_there)
	GRAMMAR_MAPPING("[Yes]>true")
)

// D: /TimeoutTeminate/TT_ReestablishChannel/TT_TerminateOnTimeout
//    Terminates execution on the third successive timeout
DEFINE_AGENCY( CTT_TerminateOnTimeout, 
    IS_NOT_DTS_AGENT()

	PRECONDITION(false)
	CONCEPT_BINDING_POLICY(WITHIN_TOPIC_ONLY)
	TRIGGERED_BY_COMMANDS(TIMEOUT_ELAPSED, "none")
	DEFINE_SUBAGENTS(
		SUBAGENT(TT_InformYouNotThere, CTT_InformYouNotThere, "none")
		SUBAGENT(TT_Terminate, CTT_Terminate, "none")
	)
)

// D: /TimeoutTerminate/TT_ReestablishContext/TT_TerminateOnTimeout/TT_InformYouNotThere
DEFINE_INFORM_AGENT( CTT_InformYouNotThere,
    IS_NOT_DTS_AGENT()

	PROMPT(OAAS("say([you_not_there, sorry_i_will_shutdown])")
           GALS("{inform you_not_there}{inform sorry_i_will_shutdown}"))
)

// D: /TimeoutTerminate/TT_ReestablishContext/TT_SuspendOnTimeout/TT_Terminate
DEFINE_EXECUTE_AGENT( CTT_Terminate,
    IS_NOT_DTS_AGENT()

	virtual TDialogExecuteReturnCode Execute() {
		return dercFinishDialogAndCloseSession;
    }
)

// D: Function for registering the timeout agents
void RegisterTimeoutTerminateAgency() {
	DECLARE_AGENT(CTimeoutTerminate)
		DECLARE_AGENT(CTT_HandleFirstTimeout)
		DECLARE_AGENT(CTT_ReestablishChannel)
			DECLARE_AGENT(CTT_RequestAreYouThere)
			DECLARE_AGENT(CTT_ExpectAnswerAreYouThere)
			DECLARE_AGENT(CTT_TerminateOnTimeout)
				DECLARE_AGENT(CTT_InformYouNotThere)
				DECLARE_AGENT(CTT_Terminate)
}