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
// DIALOGAGENCY.CPP - implementation of the CDialogAgency class. This class 
//                    defines the basic capabilities of a DialogAgency, and 
//					  provides useful macros for deriving classes which 
//					  implement particular dialog agencies.
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
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2004-04-21] (jsherwan): added logging to fatal errors in NextAgentToExecute()
//   [2004-04-12] (dbohus): changed order of creating concepts and subagents
//                           so that concepts exist by the time the subagents
//                           are created
//   [2004-04-16] (dbohus):  added grounding models on dialog agents
//   [2003-10-16] (dbohus): fixed bug in execution planning (execution policy
//                           was called twice)
//   [2003-10-15] (dbohus): added macro for execution policy
//   [2003-04-25] (dbohus): agencies don't check for completion on execute 
//                           any more. the core takes care of that
//   [2003-04-09] (dbohus): created OnCreation and OnInitialization
//   [2003-04-08] (dbohus): change completion evaluation on execution
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-03-28] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "DialogAgency.h"
#include "DMCore/Core.h"

//-----------------------------------------------------------------------------
//
// Constructors and destructors
//
//-----------------------------------------------------------------------------
// D: default constructor
CDialogAgency::CDialogAgency(string sAName, 
							 string sAConfiguration, 
							 string sAType):
	CDialogAgent(sAName, sAConfiguration, sAType) {
	// does nothing
};

// D: virtual destructor - does nothing special
CDialogAgency::~CDialogAgency() {
}

//-----------------------------------------------------------------------------
// Static function for dynamic agent creation
//-----------------------------------------------------------------------------
CAgent* CDialogAgency::AgentFactory(string sAName, string sAConfiguration) {
	return new CDialogAgency(sAName, sAConfiguration);
}

//-----------------------------------------------------------------------------
//
// CAgent specific overwritten methods
//
//-----------------------------------------------------------------------------
// D: Create:
void CDialogAgency::Create() {
	// creates the concepts
	CreateConcepts();
	// creates the subagents
	CreateSubAgents();
	// finally calls the OnCreation method
	OnCreation();
}

// D: Initialize: 
void CDialogAgency::Initialize() {
    // calls the OnInitialization method
    OnInitialization();
}

//-----------------------------------------------------------------------------
//
// CDialogAgent specific overwritten methods
//
//-----------------------------------------------------------------------------

// D: create the subagents: does nothing, is to be overwritten by derived 
//    classes
void CDialogAgency::CreateSubAgents() {
}

// D: the Execute routine: plans the execution of subagents, 
TDialogExecuteReturnCode CDialogAgency::Execute() {
	
    // plan the next agent we just determined according to execution policy
	CDialogAgent *pDialogAgent = NextAgentToExecute();
	if(pDialogAgent) {
		pDMCore->ContinueWith(this, pDialogAgent);
        // increment the execute counter
        IncrementExecuteCounter();
		// and continue execution
		return dercContinueExecution;
	} else {
		// o/w it means there's no more agents we can execute, so trigger 
		// a fatal error *** actually maybe we want a "failed" on agencies, 
		// or do we ?
		FatalError("Agency " + GetName() + " failed to complete.");
		return dercFinishDialogAndCloseSession;
	}
}

// D: returns a pointer to the subagent which should be executed 
//    next, according to the current execution policy
CDialogAgent* CDialogAgency::NextAgentToExecute() {
	// figure out which is the next subagent to execute, according to 
	// the current execution policy
	string sCurrentExecutionPolicy = GetExecutionPolicy();
	
	if(sCurrentExecutionPolicy == LEFT_TO_RIGHT_OPEN) {
		// for the left-to-right-open execution policy, 
		// go from the first subagent to the last, and find the one which 
		// has not completed yet and has preconditions satisfied and is not
		// blocked; program it for execution

		for(unsigned int i=0; i < SubAgents.size(); i++) 
			if(!SubAgents[i]->HasCompleted() && 
			   SubAgents[i]->PreconditionsSatisfied() &&
			   !SubAgents[i]->IsBlocked()) {
				// plan that one for execution
				return SubAgents[i];
			}

		// o/w go through all agents again and describe why they didn't execute
	    string sWarning = "Failure information for Agency " + GetName() + "\n";
		for(unsigned int i=0; i < SubAgents.size(); i++) 
			sWarning+="Agent " + SubAgents[i]->GetName() + " has " + \
				(SubAgents[i]->HasCompleted()?"":"not ") + "completed, does " + \
				(SubAgents[i]->PreconditionsSatisfied()?"":"not ") + \
				"have preconditions satisfied, and is " + \
				(SubAgents[i]->IsBlocked()?"":"not ") + "blocked.\n";
		Warning(sWarning);
		// finally return null
		return NULL;
	}
	
	else if(sCurrentExecutionPolicy == LEFT_TO_RIGHT_ENFORCED) {
		// for the left-to-right-enforced execution policy, 
		// go from the first subagent to the last, and find the first 
		// uncompleted one. Block all the others, and plan this one
		// for execution

		unsigned int i=0; 
		while((i < SubAgents.size()) && (SubAgents[i]->HasCompleted())) {
			SubAgents[i]->Block();
			i++;
		}

		// there should be at least an incompleted agent
		if(i == SubAgents.size()) {
			FatalError("All agents are completed in NextAgentToExecute for " + 
					   sName + " agency (with left-to-right-enforced policy).");
		}
				       
		// unblock and remember this one
		SubAgents[i]->UnBlock();
		int iIndexNextAgent = i;

		// block all the other ones 
        for(i++; i < SubAgents.size(); i++) 
			SubAgents[i]->Block();

		//
		return SubAgents[iIndexNextAgent];
	}

	// no other policy is known, so return NULL
	return NULL;
}


// D: the GetExecutionPolicy method: by default, agencies have a left-to-right-open 
//    execution policy
string CDialogAgency::GetExecutionPolicy() {
	return LEFT_TO_RIGHT_OPEN;
}

