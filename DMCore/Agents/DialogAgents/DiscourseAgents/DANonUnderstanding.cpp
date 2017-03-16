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
// DANONUNDERSTANDING.CPP - implementation (specification) of discourse agent 
//							classed dealing with non-understandings
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
//	 [2003-06-25] (antoine): created the NonUnderstanding agency and its subagent
//							 based on the NonUnderstanding code in RoomLine 
//							(by dbohus) and the Help discourse agent.
//   [2002-07-03] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

// AR: COMMENTS
//		The NonUnderstanding agency makes use of the ExplainMore and WhatCanISay
//		prompts of request agents. You have to define those for it to work 
//		properly.

#include "DANonUnderstanding.h"
#include "DialogTask/DialogTask.h"

DEFINE_AGENCY( CNonUnderstanding, 
    PRECONDITION(false)
    ON_CREATION(C("last_nonunderstood_turn") = -2)
    DEFINE_CONCEPTS(
        INT_SYSTEM_CONCEPT(last_nonunderstood_turn)
		STRING_SYSTEM_CONCEPT(calling_agent)
    )
    DEFINE_SUBAGENTS(
        SUBAGENT(FirstNonUnderstanding, CFirstNonUnderstanding, "none")
        SUBAGENT(SubsequentNonUnderstanding, CSubsequentNonUnderstanding, "none")
    )

)


// NonUnderstanding/FirstNonUnderstanding
DEFINE_INFORM_AGENT( CFirstNonUnderstanding,
    TRIGGERED_BY((pDMCore->LastTurnNonUnderstanding()) && 
                 ((int)C("last_nonunderstood_turn") < pDMCore->GetLastInputTurnNumber() - 1));

	virtual TDialogExecuteReturnCode Execute() {
		// obtain the previously focused agent
		CDialogAgent *pdaFocused = pDMCore->GetAgentPreviouslyInFocus(this);

		// stores its name so that it can be used if further non-understandings occur
		C("calling_agent") = pdaFocused->GetName();

		// if the original agent has reached its maximum attempt count
		// mark it as failed and continue
		if (pdaFocused->GetExecuteCounter() >= pdaFocused->GetMaxExecuteCounter()) {
			pdaFocused->SetCompleted(ctFailed);
			return dercContinueExecution;
		}

		// obtain the explain_more prompt
		string sExplainMorePrompt = pdaFocused->CreateVersionedPrompt("explain_more");

        // and then issue the explain more prompt
		pOutputManager->Output(pdaFocused, sExplainMorePrompt, fsUser);

        // finally set this agent as completed
		SetCompleted(ctSuccess);

        // and clean it off the execution stack (hacky)
        pDMCore->PopAgentFromExecutionStack(this);

		// and return a request for a new input pass
		return dercTakeFloor;
	}

	ON_COMPLETION(
        C("last_nonunderstood_turn") = pDMCore->GetLastInputTurnNumber();
        Reset();
    )
)

// NonUnderstanding/SubsequentNonUnderstanding
DEFINE_INFORM_AGENT( CSubsequentNonUnderstanding,
    TRIGGERED_BY(pDMCore->LastTurnNonUnderstanding() && 
                 ((int)C("last_nonunderstood_turn") == pDMCore->GetLastInputTurnNumber() - 1) &&
				 (!((CDialogAgent*)AgentsRegistry[(string)C("calling_agent")])->HasCompleted()));

	virtual TDialogExecuteReturnCode Execute() {
		// retrieves the agent from the registry given its stored name
		CDialogAgent *pdaFocused = (CDialogAgent*)AgentsRegistry[(string)C("calling_agent")];

		// if the original agent has reached its maximum attempt count
		// mark it as failed and continue
		if (pdaFocused->GetExecuteCounter() >= pdaFocused->GetMaxExecuteCounter()) {
			pdaFocused->SetCompleted(ctFailed);
			return dercContinueExecution;
		}

    	// and the what can i say prompt
        string sWhatCanISayPrompt = pdaFocused->CreateVersionedPrompt("what_can_i_say");

        // and finally the what_can_i_say prompt
        pOutputManager->Output(pdaFocused, sWhatCanISayPrompt, fsUser);

        // finally set this agent as completed
		SetCompleted(ctSuccess);

        // and clean it off the execution stack (hacky)
        pDMCore->PopAgentFromExecutionStack(this);

		// and return a request for a new input pass
		return dercTakeFloor;
	}

	ON_COMPLETION(
        C("last_nonunderstood_turn") = pDMCore->GetLastInputTurnNumber();
        Reset();
    )
)

// D: the Dialog Non-Understanding agency
DEFINE_AGENCY( CDialogNonUnderstanding, 
	IS_MAIN_TOPIC()
)

// D: the Channel Non-Understanding agency
DEFINE_AGENCY( CChannelNonUnderstanding, 
	IS_MAIN_TOPIC()
)
	
// D: this function registers the various non-understanding agencies
void RegisterNonUnderstandingAgents() {
	DECLARE_AGENT(CDialogNonUnderstanding)
	DECLARE_AGENT(CChannelNonUnderstanding)
	DECLARE_AGENT(CNonUnderstanding)
		DECLARE_AGENT(CFirstNonUnderstanding)
		DECLARE_AGENT(CSubsequentNonUnderstanding)
}