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
// DAHELP.CPP - specification of the CHelp class. This class implements 
//              the default dialog agency for providing help
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
//   [2004-04-24] (dbohus): changed agents to reopen (instead of reset) on
//                           completion
//   [2004-04-23] (dbohus): added HelpSystemCapabilities
//   [2002-11-26] (dbohus): fixed agents so that they increment the attempt
//                           count when appropriate
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-04-18] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

// D: COMMENTS
//    When using these discourse agents, the following prompts need to be 
//    defined:
//    - inform no_help_available : used when no help is available for a topic, 
//      could say something like : "I'm sorry, there's no help available for 
//      the current topic".
//
//    - inform generic_tips : used to provide generic tips for the interaction, 
//      i.e. "Please speak clearly and in your natural voice... ", the DM 
//      will also send in a _tips_counter concept indicating the number of 
//      the request, so that the NLG can potentially cycle through prompts
//
//    - inform system_capabilities : describes the capabilities of the system
//      i.e. "I am RoomLine, the automated spoken dialogue system ... "

#include "DAHelp.h"
#include "DialogTask/DialogTask.h"

// A: HelpExecutionAgency
//    the help agent. 
DEFINE_AGENCY( CHelpExecutionAgency,
	PRECONDITION(false)

	public:
		CDialogAgent* pdaFocused;

	DEFINE_SUBAGENTS(
		SUBAGENT(HInformHelp, CHInformHelp, "none")
	)

	REOPEN_ON_COMPLETION

	virtual bool IsDTSAgent() {
		return false;
	}
)

// A: HelpExecutionAgency/HInformHelp
DEFINE_INFORM_AGENT(CHInformHelp,
public:
	// A: the Execute routine
	//TDialogExecuteReturnCode CHInformHelp::Execute() {
		TDialogExecuteReturnCode Execute() {
		// call on the output manager to send out the output
		vector<COutput*> voTemp = pOutputManager->Output(this, 
			((CHelpExecutionAgency*)pdaParent)->pdaFocused->EstablishContextPrompt(), 
			fsSystem);
		voOutputs.insert(voOutputs.end(), voTemp.begin(), voTemp.end());
		voTemp = pOutputManager->Output(this, 
			((CHelpExecutionAgency*)pdaParent)->pdaFocused->CreateVersionedPrompt("explain_more"),
			fsFree);
		voOutputs.insert(voOutputs.end(), voTemp.begin(), voTemp.end());
		voTemp = pOutputManager->Output(this,
			((CHelpExecutionAgency*)pdaParent)->pdaFocused->CreateVersionedPrompt("what_can_i_say"),
			fsSystem);
		voOutputs.insert(voOutputs.end(), voTemp.begin(), voTemp.end());

        // and clean it off the execution stack (very hacky)
        pDMCore->PopTopicFromExecutionStack(pdaParent);
		// we also need to recompute the state after cleaning it off
		pStateManager->UpdateState();
		pStateManager->BroadcastState();

		// and return with continue execution
		return dercTakeFloor;
	}

	virtual bool IsDTSAgent() {
		return false;
	}
)

//// A: HelpExecutionAgency/HEstablishContext
//DEFINE_INFORM_AGENT(CHEstablishContext,
//public:
//	virtual string Prompt() {
//        // and the establish_context prompt
//        return ((CHelpExecutionAgency*)pdaParent)->pdaFocused->EstablishContextPrompt();
//	}
//
//	virtual bool IsDTSAgent() {
//		return false;
//	}
//)
//
//// A: HelpExecutionAgency/HExplainMore
//DEFINE_INFORM_AGENT(CHExplainMore,
//public:
//	virtual string Prompt() {
//        // and the establish_context prompt
//        return ((CHelpExecutionAgency*)pdaParent)->pdaFocused->CreateVersionedPrompt("explain_more");
//	}
//
//	virtual bool IsDTSAgent() {
//		return false;
//	}
//)
//
//// A: HelpExecutionAgency/HWhatCanISay
//DEFINE_INFORM_AGENT(CHWhatCanISay,
//public:
//	virtual string Prompt() {
//        // and the establish_context prompt
//        return ((CHelpExecutionAgency*)pdaParent)->pdaFocused->CreateVersionedPrompt("what_can_i_say");
//	}
//
//	virtual bool IsDTSAgent() {
//		return false;
//	}
//)
//
//// A: HelpExecutionAgency/HTriggerInputPass
//DEFINE_EXECUTE_AGENT(CHTriggerInputPass,
//	virtual TDialogExecuteReturnCode Execute() {
//		// clean the help agency off the execution stack (hacky)
//		pDMCore->PopTopicFromExecutionStack(this);
//
//		// increment the execute counter
//		IncrementExecuteCounter();
//
//		// and return a request for a new input pass
//		return dercYieldFloor;	
//	}
//
//	virtual bool IsDTSAgent() {
//		return false;
//	}
//)

// D: /Help
//    the help trigger agent. 
DEFINE_EXECUTE_AGENT( CHelp,
	PRECONDITION(false)

	private:
		CHelpExecutionAgency *pdaHelpAgency;

	public:
	virtual string TriggeredByCommands() {
		sTriggeredByCommands = "@[Help.general_help]";
		// Adds a DTMF key to the trigger expectation if specified
		// in the agent configuration
		if (HasParameter("dtmf_trigger")) {
			sTriggeredByCommands += ", @[" + GetParameterValue("dtmf_trigger") + "]";
		}
		sTriggerCommandsGroundingModelSpec = "";
        return sTriggeredByCommands;
	}

    CAN_TRIGGER_DURING_GROUNDING

	ON_CREATION(
		// creates the Help agency
		pdaHelpAgency = (CHelpExecutionAgency *)
			AgentsRegistry.CreateAgent("CHelpExecutionAgency", 
			"HelpExecutionAgency", "");

		// initializes it
		pdaHelpAgency->Initialize();

		// registers it
		pdaHelpAgency->Register();
	)
	
	ON_DESTRUCTION(
	    if(pdaHelpAgency) {
	        // delete the help agency - first call the OnDestruction method
	        pdaHelpAgency->OnDestruction();
	        // then delete it
	        delete pdaHelpAgency;
	        // then set it to NULL
	        pdaHelpAgency = NULL;
	    }
	)

	virtual TDialogExecuteReturnCode Execute() {
		// gets the previously focused agent
		CDialogAgent *pdaFocused = pDMCore->GetAgentPreviouslyInFocus(this);

		// sets its context to be the last focused agent
		pdaHelpAgency->SetContextAgent(pdaFocused);

		// set pointer to previously focused agent
		pdaHelpAgency->pdaFocused = pdaFocused;

		SetCompleted(ctSuccess);

		pDMCore->ContinueWith(pDMCore, pdaHelpAgency);

		return dercContinueExecution;
	}

	REOPEN_ON_COMPLETION

	virtual bool IsDTSAgent() {
		return false;
	}
)

// D: /EstablishContext
//    establishes the context of the current topic. 
DEFINE_INFORM_AGENT( CHelpEstablishContext, 
	PRECONDITION(false)
	CAN_TRIGGER_DURING_GROUNDING
	EXPECT_WHEN(pDMCore->GetDTSAgentInFocus()->EstablishContextPrompt() != "")
	TRIGGERED_BY_COMMANDS("@[Help.where_are_we], @[EstablishContext]", "")
	virtual TDialogExecuteReturnCode Execute() {
        // obtain the previously focused DTS agent
        CDialogAgent* pdaDTSFocused = pDMCore->GetDTSAgentPreviouslyInFocus(this);
		// obtain the establish_context prompt
		string sEstablishContextPrompt = pdaDTSFocused->EstablishContextPrompt();
		pOutputManager->Output(pdaDTSFocused, sEstablishContextPrompt, fsFree);
		// finally set this agent as completed
		SetCompleted(ctSuccess);
		// and continue the execution
		return dercContinueExecution;
	}
	REOPEN_ON_COMPLETION
)

// D: /HelpGetTips
//    provides generic tips for using the dialog system
DEFINE_INFORM_AGENT( CHelpGetTips, 
	PRECONDITION(false)
	DEFINE_CONCEPTS(
		INT_SYSTEM_CONCEPT(_tips_counter))
	ON_CREATION(C("_tips_counter") = 0)
	CAN_TRIGGER_DURING_GROUNDING
	TRIGGERED_BY_COMMANDS("@[Help.give_me_tips]", "")
	PROMPT("inform generic_tips <_tips_counter")
	ON_COMPLETION(
		C("_tips_counter") = (int)C("_tips_counter") + 1;
		REOPEN_TOPIC(.))
)

// D: /HelpWhatCanISay
//    the whay can i say agent: it reacts only if the previously focused agent
//    has a "what can i say" prompt on it
DEFINE_INFORM_AGENT( CHelpWhatCanISay,
	PRECONDITION(false)
	CAN_TRIGGER_DURING_GROUNDING
	TRIGGERED_BY_COMMANDS("@[Help.what_can_i_say]", "")
	virtual TDialogExecuteReturnCode Execute() {
		// obtain the previously focused agent
		CDialogAgent* pdaFocused = pDMCore->GetAgentPreviouslyInFocus(this);
		// obtain the what_can_i_say prompt
		string sWhatCanISayPrompt = pdaFocused->CreateVersionedPrompt("what_can_i_say");
		pOutputManager->Output(pdaFocused, sWhatCanISayPrompt, fsUser);
		// finally set this agent as completed
		SetCompleted(ctSuccess);
        // and clean it off the execution stack (hacky)
        pDMCore->PopAgentFromExecutionStack(this);
		// and return a request for a new input pass
		return dercYieldFloor;
	}
	REOPEN_ON_COMPLETION
)

// D: /HelpSystemCapabilities
//    provides generic information about the capabilities of the spoken 
//    dialogue system
DEFINE_INFORM_AGENT( CHelpSystemCapabilities, 
	PRECONDITION(false)
	CAN_TRIGGER_DURING_GROUNDING
	TRIGGERED_BY_COMMANDS("@[Help.system_capabilities]", "")
	PROMPT("inform system_capabilities")
	REOPEN_ON_COMPLETION
)

// D: this function registers the help agent
void RegisterHelpAgent() {
	DECLARE_AGENT(CHelp)
	DECLARE_AGENT(CHelpExecutionAgency)
	DECLARE_AGENT(CHInformHelp)
	//DECLARE_AGENT(CHEstablishContext)
	//DECLARE_AGENT(CHExplainMore)
	//DECLARE_AGENT(CHWhatCanISay)
	//DECLARE_AGENT(CHTriggerInputPass)
}

// D: this function registers the help get-tips agent
void RegisterHelpGetTipsAgent() {
	DECLARE_AGENT(CHelpGetTips)
}

// D: this function registers the help establish-context agent
void RegisterHelpEstablishContextAgent() {
	DECLARE_AGENT(CHelpEstablishContext)
}

// D: this function registers the help what-can-i-say agent
void RegisterHelpWhatCanISayAgent() {
	DECLARE_AGENT(CHelpWhatCanISay)
}

// D: this function registers the help system-capabilities agent
void RegisterHelpSystemCapabilitiesAgent() {
	DECLARE_AGENT(CHelpSystemCapabilities)
}
