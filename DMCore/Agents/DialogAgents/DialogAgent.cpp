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
// DIALOGAGENT.CPP - definition of the CDialogAgent class. This class defines 
//                   the basic capabilities of a DialogAgent: execution, agenda
//                   definition, etc, etc
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
//   [2005-10-22] (antoine): Added methods RequiresFloor and 
//							 IsConversationSynchronous to regulate turn-taking
//                           and asynchronous agent planning/execution
//   [2005-01-21] (jsherwan): Added input line config functionality
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2004-05-24] (dbohus):  fixed bug in ExplainMore, WhatCanISay and Timeout
//                            prompt generation
//   [2004-04-24] (dbohus):  added Create method
//   [2004-04-24] (dbohus):  added indices to the last input and binding 
//                            results
//   [2004-04-17] (dbohus):  added support for declaring the grounding models
//                            subsumed by the concept
//   [2004-04-16] (dbohus):  added grounding models on dialog agents
//   [2004-04-01] (dbohus):  fixed potential buffer overrun problem
//   [2003-10-14] (dbohus):  added dynamic ids
//   [2003-07-11] (antoine): added function and macro LanguageModel to allow 
//							 agents to specify a LM for speech recognition
//   [2003-05-09] (dbohus):  fixed bug on default failure criteria
//   [2003-04-15] (dbohus,
//                 antoine): introduced support for *-type expectations
//   [2003-04-09] (dbohus):  created OnCreation and OnInitialization
//   [2003-04-08] (dbohus):  changed so that completion is evaluated on each
//                            call to HasCompleted()
//   [2003-04-03] (dbohus,
//                 antoine): lowercased all grammar expectations declared
//   [2003-02-13] (dbohus):  added dercRestartDialog to return codes for agents
//   [2003-01-27] (dbohus):  split ReOpen into ReOpenTopic and ReOpenConcepts
//   [2003-01-22] (dbohus):  added reset and reopen counters and methods to 
//                            access them   
//   [2002-11-20] (dbohus):  added support for maximum number of attempts on an
//                            agent (after that it will terminate with failure)
//   [2002-11-20] (dbohus):  changed so that an agent can complete with success
//                            or failure
//   [2002-11-20] (dbohus):  added support for customizing binding filters in 
//                            concept expectations / grammar mappings
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-13] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#include "DialogAgent.h"
#include "DMCore/Core.h"
#include "DMCore/Agents/Registry.h"
#include "DialogTask/DialogTask.h"

// NULL dialog agent: this object is used designate invalid dialog agent
// references
CDialogAgent NULLDialogAgent("NULL");

//-----------------------------------------------------------------------------
//
// Constructors and destructors
//
//-----------------------------------------------------------------------------
// D: Default constructor
CDialogAgent::CDialogAgent(string sAName, string sAConfiguration, string sAType): 
	CAgent(sAName, sAConfiguration, sAType) {
	// initialize the class members: concepts, subagents, parent, etc
	sDialogAgentName = sAName;
	pdaParent = NULL;
	pdaContextAgent = NULL;
    pGroundingModel = NULL; 
	bCompleted = false;
    ctCompletionType = ctFailed;
	bBlocked = false;
    bDynamicAgent = false;
    sDynamicAgentID = "";
    sTriggeredByCommands = "";
    sTriggerCommandsGroundingModelSpec = "";
    iExecuteCounter = 0;
    iResetCounter = 0;
    iReOpenCounter = 0;
    iTurnsInFocusCounter = 0;
    iLastInputIndex = -1;
    iLastExecutionIndex = -1;
    iLastBindingsIndex = -1;
	bInheritedParentInputConfiguration = false;
}

// D: Virtual destructor
CDialogAgent::~CDialogAgent() {
	// delete all the concepts
	unsigned int i;
	for(i=0; i < Concepts.size(); i++) 
		delete Concepts[i];

	// delete all the subagents
	for(i=0; i < SubAgents.size(); i++) 
		delete SubAgents[i];

    // delete the grounding model
    if(pGroundingModel != NULL) {
        delete pGroundingModel;
        pGroundingModel = NULL;
    }

	// set the parent to NULL
	pdaParent = NULL;
}


//-----------------------------------------------------------------------------
// Static function for dynamic agent creation
//-----------------------------------------------------------------------------
CAgent* CDialogAgent::AgentFactory(string sAName, string sAConfiguration) {
	// this method should never end up being called (since CDialogAgent is an 
	// abstract class) , therefore do a fatal error if this happens
	FatalError("AgentFactory called on CDialogAgent (abstract) class.");
	return NULL;
}

//-----------------------------------------------------------------------------
//
// CAgent overwritten methods
//
//-----------------------------------------------------------------------------

// D: Registers the agent - for a dialog agent, we are registering using the
//    full path in the dialog tree, and we are registering the children
void CDialogAgent::Register() {
	// register this agent
	AgentsRegistry.RegisterAgent(sName, this);
	// and all its subagents
    for(unsigned int i=0; i < SubAgents.size(); i++)
        SubAgents[i]->Register();
    // finally, create the trigger concept
	CreateTriggerConcept();
}

// D: Initializes the agent, gets called after creation
void CDialogAgent::Create() {
	// creates the concepts
	CreateConcepts();
	// calls OnCreation
	OnCreation();
}

// D: Initializes the agent, gets called after construction
void CDialogAgent::Initialize() {	
    // calls OnInitialization
    OnInitialization();
}


// D: Reset triggers an initialize
void CDialogAgent::Reset() {
	// clears all the concepts
	for(unsigned int i = 0; i < Concepts.size(); i++) 
		Concepts[i]->Clear();
	// and calls reset on all the subagents
	for(unsigned int i = 0; i < SubAgents.size(); i++)
		SubAgents[i]->Reset();
    // reset the other member variables
	bCompleted = false;
    ctCompletionType = ctFailed;
	bBlocked = false;
	iExecuteCounter = 0;
    iReOpenCounter = 0;
    iResetCounter++;
    iTurnsInFocusCounter = 0;
    iLastInputIndex = -1;
    iLastExecutionIndex = -1;
    iLastBindingsIndex = -1;
    // finally, call the OnInitialization
    OnInitialization();
}

// J: Start copy from Agent.cpp and renamed s2sConfiguration -> s2sInputLineConfiguration
// A: Parses a configuration string into a hash of parameters
void CDialogAgent::SetInputConfiguration( string sConfiguration) {
	
	string sItem, sSlot, sValue;
	
	// while there are still thing left in the string
	while(!sConfiguration.empty()) {
		
		// get the first item
		SplitOnFirst(sConfiguration, ",", sItem, sConfiguration, '%');
		sItem = Trim(sItem);

		// gets the slot and the value
		SplitOnFirst(sItem, "=", sSlot, sValue, '%');
		sSlot = Trim( sSlot);
		sValue = Trim( sValue);

		// fills in the configuration hash
		SetInputConfigurationParameter( sSlot, sValue);
	}	
}

// A: sets an individual parameter
void CDialogAgent::SetInputConfigurationParameter( string sSlot, string sValue) {
	s2sInputLineConfiguration.insert(STRING2STRING::value_type( sSlot, sValue));
}

// A: tests if a given parameter exists in the configuration
bool CDialogAgent::HasInputConfigurationParameter( string sSlot) {
	return s2sInputLineConfiguration.find( sSlot) != s2sInputLineConfiguration.end();
}

// A: gets the value for a given parameter
string CDialogAgent::GetInputConfigurationParameterValue( string sSlot) {

	STRING2STRING::iterator i = s2sInputLineConfiguration.find( sSlot);

	if (i == s2sInputLineConfiguration.end()) {
		return "";
	}
	else {
		return (string)((*i).second);
	}

	return "";
}
// J: End copy from Agent.cpp and renamed s2sConfiguration -> s2sInputLineConfiguration

// J: Default Input Config Init string (will be overridden) by most dialog agents
string CDialogAgent::InputLineConfigurationInitString() {
	return "";
}

// J: Returns the name of the derived input line config. Only calculates derived config
// once per session
STRING2STRING CDialogAgent::GetInputLineConfiguration() {
	if (!bInheritedParentInputConfiguration)
	{
		bInheritedParentInputConfiguration = true;
		// Sets hash based on init string
		SetInputConfiguration(InputLineConfigurationInitString());
		if(pdaParent)
			InheritParentInputConfiguration();
	}
	return s2sInputLineConfiguration;
}

// J: Selectively inherits parent's input line configuration
void CDialogAgent::InheritParentInputConfiguration() {
	STRING2STRING s2sParentConfig = pdaParent->GetInputLineConfiguration();
	if(s2sParentConfig.size() > 0) {
		STRING2STRING::iterator iPtr;
		for(iPtr = s2sParentConfig.begin(); 
			iPtr != s2sParentConfig.end(); 
			iPtr++) {

			// only if the agent hasn't had this slot filled in yet
			// will it take its parent's value
			if (!HasInputConfigurationParameter(iPtr->first))
				SetInputConfigurationParameter(iPtr->first, iPtr->second);
		}
	}
}

//-----------------------------------------------------------------------------
//
// Fundamental Dialog Agent methods. Most of these are to be 
// overwritten by derived agent classes
//
//-----------------------------------------------------------------------------

// D: create the concepts for this agent: does nothing, is to be overwritten 
//    by derived classes
void CDialogAgent::CreateConcepts() {
}

// D: returns true if the dialog agent is executable - by default, returns true
bool CDialogAgent::IsExecutable() {
	return true;
}

// D: the Execute: for this class, it merely returns continue execution
TDialogExecuteReturnCode CDialogAgent::Execute() {
    // increment the execute counter
    IncrementExecuteCounter();

	return dercContinueExecution;
}

// D: the DeclareExpectations: for this class, it calls DeclareExpectations for
//    all the subagents; also, if the agent is triggered by a command, 
//    it adds that expectation
int CDialogAgent::DeclareExpectations(TConceptExpectationList& 
									    rcelExpectationList) {
	int iExpectationsAdded = 0;
	bool bExpectCondition = ExpectCondition();

	// if there's a trigger, construct the expectation list for that trigger
	if(TriggeredByCommands() != "") {
		TConceptExpectationList celTriggerExpectationList;

		// construct the expectation list for the triggering commands
        parseGrammarMapping(
            C("_%s_trigger", sDialogAgentName.c_str()).GetAgentQualifiedName(), 
            TriggeredByCommands(), 
			celTriggerExpectationList);

		// go through it and add stuff to the current agent expectation list
		for(unsigned int i = 0; i < celTriggerExpectationList.size(); i++) {
			// set the expectation to bind the trigger to true
            celTriggerExpectationList[i].bmBindMethod = bmExplicitValue;
			celTriggerExpectationList[i].sExplicitValue = "true";

			// if the expect condition is not satisfied, disable this 
			// trigger expectation and set the appropriate reason
			if(!bExpectCondition) {
				celTriggerExpectationList[i].bDisabled = true;
				celTriggerExpectationList[i].sReasonDisabled = 
					"expect condition false";
			}

			rcelExpectationList.push_back(celTriggerExpectationList[i]);
			iExpectationsAdded++;
		}
	}

	// finally go through the subagents and gather their expectations
	for(unsigned int i=0; i < SubAgents.size(); i++) {
		iExpectationsAdded += 
			SubAgents[i]->DeclareExpectations(rcelExpectationList);
	}
	return iExpectationsAdded;
}

// D: Declares the concepts that the agent subsumes
void CDialogAgent::DeclareConcepts(
    TConceptPointersVector& rcpvConcepts, 
    TConceptPointersSet& rcpsExclude) {

    // first copy the agent's concepts
    for(unsigned int i = 0; i < Concepts.size(); i++)
        Concepts[i]->DeclareConcepts(rcpvConcepts, rcpsExclude);

    // and the do the same for all the subagents
    for(unsigned int i = 0; i < SubAgents.size(); i++) 
        SubAgents[i]->DeclareConcepts(rcpvConcepts, rcpsExclude);
}

// D: create a grounding model for this agent
void CDialogAgent::CreateGroundingModel(string sGroundingModelSpec) {
    if(!(pGroundingManager->GetConfiguration().bGroundTurns) || 
		(sGroundingModelSpec == "none") || 
		(sGroundingModelSpec == ""))
        pGroundingModel = NULL;
    else {
		// extract the model type and policy
		string sModelType, sModelPolicy;
		if(!SplitOnFirst(sGroundingModelSpec, ".", sModelType, sModelPolicy)) {
			// if there is no model type, set it to the default 
			// grounding manager configuration
			sModelType = pGroundingManager->GetConfiguration().sTurnGM;
			sModelPolicy = sGroundingModelSpec;
		}
		// create the grounding model
        pGroundingModel = 
            pGroundingManager->CreateGroundingModel(sModelType, sModelPolicy);
		// intialize it
        pGroundingModel->Initialize();
		// set the request agent
        ((CGMRequestAgent *)pGroundingModel)->SetRequestAgent(this);
    }
}

// D: obtain the grounding model for an agent
CGroundingModel* CDialogAgent::GetGroundingModel() {
    return pGroundingModel;
}

// D: Declares the grounding models that the agent subsumes
void CDialogAgent::DeclareGroundingModels(
    TGroundingModelPointersVector& rgmpvModels, 
    TGroundingModelPointersSet& rgmpsExclude) {

    // first put in the grounding model associated with this agent
    if(pGroundingModel && 
        rgmpsExclude.find(pGroundingModel) == rgmpsExclude.end()) {
        rgmpvModels.push_back(pGroundingModel);
        rgmpsExclude.insert(pGroundingModel);
    }

    // the get the grounding models from the agents' concepts
    for(unsigned int i = 0; i < Concepts.size(); i++)
        Concepts[i]->DeclareGroundingModels(rgmpvModels, rgmpsExclude);

    // and the do the same for all the subagents
    for(unsigned int i = 0; i < SubAgents.size(); i++) 
        SubAgents[i]->DeclareGroundingModels(rgmpvModels, rgmpsExclude);
}

// D: Returns a boolean indicating if the expectations declared are active 
//    or not. For this class, it always returns true.
bool CDialogAgent::ExpectCondition() {
	return true;
}


// D: the DeclareBindingPolicy function: for the default agent, it always
//    returns MIXED_INITIATIVE
string CDialogAgent::DeclareBindingPolicy() {
	return MIXED_INITIATIVE;
}

// D: the DeclareFocusClaims: for this class, it checks it's own ClaimsFocus
//    condition and it's command trigger condition if one exists, 
//    then it calls DeclareFocusClaims for all the subagents
int CDialogAgent::DeclareFocusClaims(TFocusClaimsList& fclFocusClaims) {
	int iClaimsAdded = 0;

	// check its own claim focus condition and command trigger condition if 
	// one exists
	bool bDeclareFocusClaim = ClaimsFocus();
	if(!TriggeredByCommands().empty()) {
		bDeclareFocusClaim = bDeclareFocusClaim || 
            C(FormatString("_%s_trigger", sDialogAgentName.c_str())).IsUpdatedAndGrounded();
	}

	// declare the focus claim, in case we have one
    if(bDeclareFocusClaim) {
		TFocusClaim fcClaim;
		fcClaim.sAgentName = sName;
		fcClaim.bClaimDuringGrounding = ClaimsFocusDuringGrounding();
		fclFocusClaims.push_back(fcClaim);
		iClaimsAdded++;
		// and also clear the triggering concept, if there is one
		if(!TriggeredByCommands().empty())
            C(FormatString("_%s_trigger", sDialogAgentName.c_str())).Clear();
	}

	// then call it for the subagents, so that they can also claim focus
	// if needed 
	for(unsigned int i=0; i < SubAgents.size(); i++) {
		iClaimsAdded += SubAgents[i]->DeclareFocusClaims(fclFocusClaims);
	}

	// finally return the number of claims added
	return iClaimsAdded;
}

// D: the Precondition: for this class, it does nothing (always returns
//    true)
bool CDialogAgent::PreconditionsSatisfied() {
	return true;
}

// D: the focus claim condition: for this class, it does always returns
//    false
bool CDialogAgent::ClaimsFocus() {
	return false;
}

// D: indicates if the agent claims the focus while grounding is in progress
//    by default, this is false
bool CDialogAgent::ClaimsFocusDuringGrounding() {
    return false;
}

// D: the string describing the grammar concepts which trigger this 
//    agent: the default agent implicitly is not triggered by 
//    commands
string CDialogAgent::TriggeredByCommands() {
	return "";
}

// D: this method creates a triggering concept, in case one is needed (if the
//    agent is to be triggered by a command
void CDialogAgent::CreateTriggerConcept() {
	// if the agent is to be triggered by a user command, 
	if(!TriggeredByCommands().empty()) {
        // add a trigger concept
        Concepts.push_back(new CBoolConcept(
            FormatString("_%s_trigger", sDialogAgentName.c_str()), 
            csUser));
        // set the grounding model
        Concepts.back()->CreateGroundingModel(
            sTriggerCommandsGroundingModelSpec);
        // and set it's owner dialog agent
        Concepts.back()->SetOwnerDialogAgent(this);
	}
}

// D: the SuccessCriteriaSatisfied method: by default an agent completes 
//    successfully when all the subagents have completed
bool CDialogAgent::SuccessCriteriaSatisfied() {
	// check that all subagents have completed
	for(unsigned int i = 0; i < SubAgents.size(); i++)
		if(!SubAgents[i]->HasCompleted())
			return false;

	return true;
}

// D: the FailureCriteriaSatisfied method: by default an agent completes 
//    with a failure when the number of attempts at execution exceeds the 
//    number of maximum attempts, and the success criteria has not been
//    met yet
bool CDialogAgent::FailureCriteriaSatisfied() {
	bool bFailed = (iExecuteCounter >= GetMaxExecuteCounter()) && 
        !SuccessCriteriaSatisfied();

	if (bFailed)
		Log(DIALOGTASK_STREAM, "Agent reached max attempts (%d >= %d), failing",
			iExecuteCounter, GetMaxExecuteCounter());

	return bFailed;
}

// D: the GetMaxExecuteCounter function specifies how many times an agent it
//    to be executed before it terminates with a failure (by default a 
//    very large int)
int CDialogAgent::GetMaxExecuteCounter() {
    return 10000;
}

// D: the OnCreation method: by default, nothing happens upon the creation
//    of the agent
void CDialogAgent::OnCreation() {
}

// D: the OnDestruction method: by default, nothing happens upon the destruction
//    of the agent
void CDialogAgent::OnDestruction() {
    // go through all the subagents and call the method
	for(unsigned int i=0; i < SubAgents.size(); i++) 
		SubAgents[i]->OnDestruction();
}

// D: the OnInitialization method: by default, nothing happens upon 
//    initialization
void CDialogAgent::OnInitialization() {
}

// D: the OnCompletion method: by default, nothing happens upon completion
void CDialogAgent::OnCompletion() {
}

// D: the ReOpen method: by default, ReOpen calls upon ReOpenConcepts
//    and ReOpenTopic
void CDialogAgent::ReOpen() {
    ReOpenConcepts();
    ReOpenTopic();
}

// D: the ReOpenConcepts method: by default, ReOpenConcepts calls ReOpen on
//    all the concepts held by that agent (and its children)
void CDialogAgent::ReOpenConcepts() {
    // call ReOpen on all the concepts
    for(unsigned int i = 0; i < Concepts.size(); i++)
        Concepts[i]->ReOpen();
    // call ReOpenConcepts on all the subagents
	for(unsigned int i = 0; i < SubAgents.size(); i++)
		SubAgents[i]->ReOpenConcepts();
}

// D: the ReOpenTopic method
void CDialogAgent::ReOpenTopic() {
    // set completion to false
    bCompleted = false;
    ctCompletionType = ctFailed;
    // unblock the agent
	bBlocked = false;
    // reset the counters
	iExecuteCounter = 0;
    iReOpenCounter++;
    iTurnsInFocusCounter = 0;
	// call ReOpenTopic on all the subagents
	for(unsigned int i = 0; i < SubAgents.size(); i++)
		SubAgents[i]->ReOpenTopic();
    // call the OnInitialization method
    OnInitialization();
}

// D: the IsAMainTopic: implicitly the agent is not a main topic
bool CDialogAgent::IsAMainTopic() {
	return false;
}

// D: the prompt: by default nothing
string CDialogAgent::Prompt() {
	return "";
}

// D: the timeout prompt: by default returns the same thing as the prompt
//    but adds version=timeout
string CDialogAgent::TimeoutPrompt() {
    string sPrompt = Trim(Prompt());
    // check that the prompt is not empty
    if(sPrompt == "") 
        return "";

    string sTimeoutPrompt;

    // check if we are dealing with a composed prompt
    if(sPrompt[0] == '{') {
        // then insert timeout in each of them
        do {
            // find the closing bracket
            int iClosingPos = FindClosingQuoteChar(sPrompt, 1, '{', '}');
            // add to the timeout prompt
            sTimeoutPrompt += sPrompt.substr(0, iClosingPos - 1);
            sTimeoutPrompt += " version=timeout}";
            // check if we're done
            if(iClosingPos >= (int)sPrompt.length()) break;
            sPrompt = sPrompt.substr(iClosingPos, 
                sPrompt.length() - iClosingPos);
        } while(sPrompt != "");

    } else {
        // o/w the prompt is simple, so just add version=timeout at 
        // the end
        sTimeoutPrompt = sPrompt + " version=timeout";
    }

    return sTimeoutPrompt;
}

// D: the explain more prompt: by default adds a version=explain_more to 
//    the default prompt
string CDialogAgent::ExplainMorePrompt() {
    
    string sPrompt = Trim(Prompt());
    // check that the prompt is not empty
    if(sPrompt == "") 
        return "";

    string sExplainMorePrompt;

    // check if we are dealing with a composed prompt
    if(sPrompt[0] == '{') {
        // then insert explain more in each of them
        do {
            // find the closing bracket
            int iClosingPos = FindClosingQuoteChar(sPrompt, 1, '{', '}');
            // add to the explain-more prompt
            sExplainMorePrompt += sPrompt.substr(0, iClosingPos - 1);
            sExplainMorePrompt += " version=explain_more}";
            // check if we're done
            if(iClosingPos >= (int)sPrompt.length()) break;
            sPrompt = sPrompt.substr(iClosingPos, 
                sPrompt.length() - iClosingPos);
        } while(sPrompt != "");
    } else {
        // o/w the prompt is simple, so just add version=explain_more at 
        // the end
        sExplainMorePrompt = sPrompt + " version=explain_more";
    }

    return sExplainMorePrompt;
}

// D: this function creates a versioned prompt
string CDialogAgent::CreateVersionedPrompt(string sVersion) {
    string sPrompt = Trim(Prompt());
    // check that the prompt is not empty
    if(sPrompt == "") 
        return "";

    // the versioned prompt
    string sVersionedPrompt;

    // check if we are dealing with a composed prompt
    if(sPrompt[0] == '{') {
        // then insert the version in each of them
        do {
            // find the closing bracket
            int iClosingPos = FindClosingQuoteChar(sPrompt, 1, '{', '}');
            // add to the timeout prompt
            sVersionedPrompt += sPrompt.substr(0, iClosingPos - 1);
            sVersionedPrompt += " version=" + sVersion + "}";
            // check if we're done
            if(iClosingPos >= (int)sPrompt.length()) break;
            sPrompt = sPrompt.substr(iClosingPos, 
                sPrompt.length() - iClosingPos);
        } while(sPrompt != "");

    } else {
        // o/w the prompt is simple, so just add version=timeout at 
        // the end
        sVersionedPrompt = sPrompt + " version=" + sVersion;
    }

    return sVersionedPrompt;
}

// D: the establish_context prompt: by default calls upon the parent agent, 
//    if there is one
string CDialogAgent::EstablishContextPrompt() {
	if(pdaContextAgent && (pdaContextAgent != this))
		return pdaContextAgent->EstablishContextPrompt();
	else if(pdaParent)
		return pdaParent->EstablishContextPrompt();
	else
		return "";
}

// D: the "what can i say" prompt: by default returns the default prompt
//    but adds version=what_can_i_say
string CDialogAgent::WhatCanISayPrompt() {
    string sPrompt = Trim(Prompt());
    // check that the prompt is not empty
    if(sPrompt == "") 
        return "";

    string sWhatCanISayPrompt;

    // check if we are dealing with a composed prompt
    if(sPrompt[0] == '{') {
        // then insert what can i say in each of them
        do {
            // find the closing bracket
            int iClosingPos = FindClosingQuoteChar(sPrompt, 1, '{', '}');
            // add to the what_can_i_say prompt
            sWhatCanISayPrompt += sPrompt.substr(0, iClosingPos - 1);
            sWhatCanISayPrompt += " version=what_can_i_say}";
            // check if we're done
            if(iClosingPos >= (int)sPrompt.length()) break;
            sPrompt = sPrompt.substr(iClosingPos, 
                sPrompt.length() - iClosingPos);
        } while(sPrompt != "");
    } else {
        // o/w the prompt is simple, so just add version=what_can_i_say at 
        // the end
        sWhatCanISayPrompt = sPrompt + " version=what_can_i_say";
    }

    return sWhatCanISayPrompt;
}

// D: Virtual function which specifies if this is a task agent or not
//    (by default all agents are task agents)
bool CDialogAgent::IsDTSAgent() {
    return true;
}

// A: Virtual function which specifies if the execution of this agent
//    has to be synchronized with the actual flow of the conversation
//    or if it can be anticipated (i.e. whether this execution yields
//    side effects for the conversation)
bool CDialogAgent::IsConversationSynchronous() {
    return false;
}

// A: Virtual function which specifies if this agent requires the 
//    floor for its execution (by default, agents do not)
bool CDialogAgent::RequiresFloor() {
    return false;
}

// Virtual function used to cancel the effects of an agent which 
// was executed within the DM but not realized (i.e. there was an
// interruption of the normal flow of the dialogue due to a user
// barge-in or another external event)
// By default: decrement execution counter and set to incomplete
void CDialogAgent::Undo() {
	iExecuteCounter--;
	ResetCompleted();
}

//-----------------------------------------------------------------------------
// 
// Other methods, mainly for providing access to public and protected members
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Relative access to Concepts
//-----------------------------------------------------------------------------
// D: the function returns a reference to the concept pointed by the 
//    relative concept path in sConceptPath
CConcept& CDialogAgent::C(string sConceptPath) {	
	
	string sAgentPath, sConceptName;	
	// check if we have an agent path in the concept name
	if(SplitOnLast(sConceptPath, "/", sAgentPath, sConceptName)) {
		// if yes, identify the agent and call the local concept retrieval for
		// that agent
		return A(sAgentPath).LocalC(sConceptName);
	} else {
		// if not, go and try to find the concept locally
		return LocalC(sConceptPath);
	}
}

// D: A printf-like version of the C() function
CConcept& CDialogAgent::C(const char *lpszConceptPath, ...) {
	static char buffer[STRING_MAX];

	// get the arguments
	va_list pArgs;
	va_start(pArgs, lpszConceptPath);

	// print the path into the buffer
	vsnprintf(buffer, STRING_MAX, lpszConceptPath, pArgs);

	// and finally call the standard C() function to deal with it
	return C((string)buffer);
}

// D: the function returns a pointer to a local concept indicated by 
//    sConceptName. If the concept is not found locally, we try to locate
//    it in the parent
CConcept& CDialogAgent::LocalC(string sConceptName) {

	// if the agent has a defined context agent,
	// look for the concept there
	CDialogAgent *pdaNextContext = NULL;
	if (pdaContextAgent) {
		pdaNextContext = pdaContextAgent;
	} else {
		pdaNextContext = pdaParent;
	}

    // convert the eventual # signs with agent dynamic id
    sConceptName = ReplaceSubString(sConceptName, "#", GetDynamicAgentID());

	// Optimization code: if no concepts, then try the parent directly 
	// (if a parent exists)
	if(Concepts.size() == 0) {
		if(pdaNextContext) return pdaNextContext->LocalC(sConceptName);
		else {
			// if there's no parent, we're failing
			FatalError("Concept " + sConceptName + 
					   " could not be located (accessed).");
			return NULLConcept;
		}
	}

	// split the concept into base and rest in case we deal with a complex
	// concept (i.e. arrays or structures i.e. hotel.name)
	string sBaseConceptName, sRest;
	SplitOnFirst(sConceptName, ".", sBaseConceptName, sRest);

	// A: Checks if we want a merged history version of the concept
	bool bMergeConcept = false;
	if (sBaseConceptName.at(0) == '@') {
		bMergeConcept = true;
		sBaseConceptName = sBaseConceptName.substr(1,sBaseConceptName.length());
	}

	// now go through the list of concepts and see if you can find it
	for(unsigned int i=0; i < Concepts.size(); i++) {
		if(Concepts[i]->GetName() == sBaseConceptName) {
			// if we have found it
			if(bMergeConcept) {
				CConcept *pcMerged = Concepts[i]->operator[](sRest).CreateMergedHistoryConcept();
				return *pcMerged;
			} else {
				return Concepts[i]->operator[](sRest);
			}
		}
	}

	// if it was not in the concepts owned by this agency, try in the parent/context
	// check if there is any parent/context
	if(!pdaNextContext) {
		// if there's no parent, we're failing
		FatalError("Concept " + sConceptName + " could not be identified "\
				   "in the dialog task hierarchy.");
		return NULLConcept;
	} else {
		// if there's a parent/context, check in there
		return pdaNextContext->LocalC(sConceptName);
	}
}

//-----------------------------------------------------------------------------
// Relative access to Agents
//-----------------------------------------------------------------------------
// D: the function returns a pointer to the agent pointed by the relative
//    agent path in sDialogAgentPath
CDialogAgent& CDialogAgent::A(string sDialogAgentPath) {

	// split the relative agent path into the first component (until /)
	// and the rest
	string sFirstComponent, sRest;
	
	if(SplitOnFirst(sDialogAgentPath, "/", sFirstComponent, sRest)) {

		// if split is successful, recurse based on the first component 
		if(sFirstComponent == "") {

			// the path starts at the root, so it's an absolute path
			// to an agent. We can (optimally) find the agent using the 
			// registry
			CDialogAgent* pdaAgent = (CDialogAgent*)
				                     AgentsRegistry[sDialogAgentPath];
			if(pdaAgent) {
				// if the agent was found
				return *pdaAgent;
			} else {
				// if the agent was not found, fail
				FatalError("Agent " + sDialogAgentPath + " does not exist"\
						   " in the dialog task hierarchy.");
				return NULLDialogAgent;
			}

		} else if(sFirstComponent == "..") {
			
			// then start at the parent
			if(!pdaParent) {
				// if there's no parent, we're failing
				FatalError("Agent " + sDialogAgentPath + " could not be "\
					       "identified in the dialog task hierarchy "\
						   "relative to " + GetName() + ".");
				return NULLDialogAgent;
			} else {
				// otherwise recurse on the parent
				return pdaParent->A(sRest);
			}

		} else if (sFirstComponent == ".") {
			
			// then it's in this agent
			return A(sRest);

		} else {

			// then it must be one of the descendants. Locate quickly using
			// the registry
			CDialogAgent* pdaAgent = (CDialogAgent*)
									 AgentsRegistry[GetName() + "/" + sDialogAgentPath];
			if(pdaAgent) {
				// if the agent was found
				return *pdaAgent;
			} else {
				// if the agent was not found, fail
				FatalError("Agent " + sDialogAgentPath + " could not be "\
					       "identified in the dialog task hierarchy "\
						   "relative to " + GetName() + ".");
				return NULLDialogAgent;
			}
		}

	} else {
		// we have no path

		// it is possible that the agent referent is just "" at this point, 
		// which means we need to return the current agent
		if((sDialogAgentPath == "") || (sDialogAgentPath == "."))
			return *this;

		// or it is possible that it's just a reference to the parent ..
		if(sDialogAgentPath == "..") {
			if(!pdaParent) {
				// if there's no parent, we're failing
				FatalError("Agent .. could not be "\
					       "identified in the dialog task hierarchy "\
						   "relative to " + GetName() + ".");
				return NULLDialogAgent;
			} else {
				// otherwise recurse on the parent
				return *pdaParent;
			}
		}

		// if not, try and find the agent locally (it has to 
		// be one of the subagents). Locate quickly using the registry.
		CDialogAgent* pdaAgent = (CDialogAgent*)
							     AgentsRegistry[GetName() + "/" + 
												sDialogAgentPath];
		if(pdaAgent) {
			// if the agent was found
			return *pdaAgent;
		} else {
			// if the agent was not found, fail
			FatalError("Agent " + sDialogAgentPath + " could not be "\
				       "identified in the dialog task hierarchy "\
					   "relative to " + GetName() + ".");
			return NULLDialogAgent;
		}
	} 
}

// D: A printf-like version of the A() function
CDialogAgent& CDialogAgent::A(const char *lpszDialogAgentPath, ...) {
	static char buffer[STRING_MAX];

	// get the arguments
	va_list pArgs;
	va_start(pArgs, lpszDialogAgentPath);

	// print the path into the buffer
	vsnprintf(buffer, STRING_MAX, lpszDialogAgentPath, pArgs);

	// and finally call the standard A() function to deal with it
	return A((string)buffer);
}

//-----------------------------------------------------------------------------
// Adding and Deleting subagents
//-----------------------------------------------------------------------------

// D: add a subagent, in the location indicated by pdaWhere and mmMethod. for
//    the mmLastChild and mmFirstChild methods, pdaWhere will be NULL
void CDialogAgent::AddSubAgent(CDialogAgent* pdaWho, CDialogAgent* pdaWhere, 
							   TAddSubAgentMethod asamMethod) {
	TAgentsVector::iterator iPtr = SubAgents.begin();
    // insert it in the right place
	switch(asamMethod) {
		case asamAsFirstChild:
			SubAgents.insert(SubAgents.begin(), pdaWho);
			break;
		case asamAsLastChild:
			SubAgents.insert(SubAgents.end(), pdaWho);
			break;	
		case asamAsRightSibling:
			while((iPtr != SubAgents.end()) && ((*iPtr) != pdaWhere))
				iPtr++;
			iPtr++;
			SubAgents.insert(iPtr, pdaWho);
			break;
		case asamAsLeftSibling:
			while((iPtr != SubAgents.end()) && ((*iPtr) != pdaWhere))
				iPtr++;
			SubAgents.insert(iPtr, pdaWho);
			break;
	}
    // set the parent
	pdaWho->SetParent(this);
    // set it to dynamic
    pdaWho->SetDynamicAgent();
    // and register it
    pdaWho->Register();
}

// D: deletes a subagent
void CDialogAgent::DeleteSubAgent(CDialogAgent* pdaWho) {
	TAgentsVector::iterator iPtr;
	for(iPtr = SubAgents.begin(); iPtr != SubAgents.end(); iPtr++) 
		if((*iPtr) == pdaWho) {
            // eliminate it
			SubAgents.erase(iPtr);
            // call the on destruction method
            pdaWho->OnDestruction();
            // finally, destroy it (this will also unregister it)            
			delete pdaWho;
			return;
		}	
}

// D: deletes all the dynamic subagents
void CDialogAgent::DeleteDynamicSubAgents() {
	TAgentsVector::iterator iPtr;
    bool bFound;
    do {
        bFound = false;
	    for(iPtr = SubAgents.begin(); iPtr != SubAgents.end(); iPtr++) 
            if((*iPtr)->IsDynamicAgent()) {
                CDialogAgent* pdaWho = (*iPtr);
                // eliminate it
			    SubAgents.erase(iPtr);
			    // call the OnDestruction method
			    pdaWho->OnDestruction();
                // finally, destroy it (this will also unregister it)
			    delete pdaWho;
                bFound = true;
                break;
            }
    } while(bFound);

    // now recursively call it on the remaining subagents
    for(iPtr = SubAgents.begin(); iPtr != SubAgents.end(); iPtr++)
        (*iPtr)->DeleteDynamicSubAgents();
}

//-----------------------------------------------------------------------------
// Operations related to setting and getting the parent for a dialog 
// agent
//-----------------------------------------------------------------------------
// D: set the parent
void CDialogAgent::SetParent(CDialogAgent* pdaAParent) {
	// set the new parent
	pdaParent = pdaAParent;
	// and update the name of the agent
	UpdateName();
}

// D: return the parent
CDialogAgent* CDialogAgent::GetParent() {
	return pdaParent;
}

// D: updates the name of the agent, by looking up the parent and concatenating
//    names as /name/name/name. Also calls UpdateName for the children, since 
//    their names need to be updated in this case, too
void CDialogAgent::UpdateName() {
	// analyze if we have or not a parent, and update the name
	if(pdaParent) {
		sName = pdaParent->GetName() + "/" + sDialogAgentName;
	} else {
		sName = "/" + sDialogAgentName;
	}
	
	// and now update the children, too
	for(unsigned int i=0; i < SubAgents.size(); i++) 
		SubAgents[i]->UpdateName();
}

//-----------------------------------------------------------------------------
// Operations related to setting and getting the context agent for a dialog 
// agent
//-----------------------------------------------------------------------------
// D: set the context agent
void CDialogAgent::SetContextAgent(CDialogAgent* pdaAContextAgent) {
	// set the new context agent
	pdaContextAgent = pdaAContextAgent;
}

// D: return the context agent
CDialogAgent* CDialogAgent::GetContextAgent() {
    // if there is a context agent
    if(pdaContextAgent) 
        // then return that
        return pdaContextAgent;
    else 
        // o/w return self
        return this;
}

//-----------------------------------------------------------------------------
// Obtaining the main topic
//-----------------------------------------------------------------------------
// D: returns the main topic for an agent
CDialogAgent* CDialogAgent::GetMainTopic() {
	// if this is a main topic, return it
	if(IsAMainTopic()) return this;
	else if(!pdaParent) {
		Log(DMCORE_STREAM, "%s has no parent -> MainTopic=NULL", sName.c_str());
		// if it's not a main topic and it doesn't have a parent, return NULL
		return NULL;
	} else {
		// return the main topic of its parent
		return pdaParent->GetMainTopic();
	}
}

//-----------------------------------------------------------------------------
// Access to completed, reset, reopen information
//-----------------------------------------------------------------------------

// D: return the agent completion status
bool CDialogAgent::HasCompleted() {
    // if the agent has the completed flag set, return true
    if(bCompleted) return true;

    // o/w check HasSucceeded and HasFailed
	return HasSucceeded() || HasFailed();
}

// D: set the agent completion status
void CDialogAgent::SetCompleted(TCompletionType ctACompletionType) {
	bCompleted = true;
    ctCompletionType = ctACompletionType;
}

// D: resets the agent completion status
void CDialogAgent::ResetCompleted() {
    bCompleted = false;
    ctCompletionType = ctFailed;
}

// D: indicates if the agent has completed with a failure
bool CDialogAgent::HasFailed() {

    // if the agent is already marked as failed, return true
    if(bCompleted && (ctCompletionType == ctFailed))
        return true;
    
    // o/w check if the failure condition was recently matched
    return FailureCriteriaSatisfied();
}

// D: indicates if the agent has completed successfully
bool CDialogAgent::HasSucceeded() {

    // if the agent is already marked as succeeded, return true
    if(bCompleted && (ctCompletionType == ctSuccess)) 
        return true;

    // o/w check if the success criterion was recently matched
    return SuccessCriteriaSatisfied();
}

//-----------------------------------------------------------------------------
// Access to Blocked/Unblocked information
//-----------------------------------------------------------------------------
// D: return true if one of the agent's ancestors is blocked
bool CDialogAgent::IsAgentPathBlocked() {
	// go recursively through the parents to find out if there's anything 
	// blocked
	if(pdaParent)
		return pdaParent->IsAgentPathBlocked() || IsBlocked();
	else 
		return IsBlocked();
}

// D: return true if the agent is blocked
bool CDialogAgent::IsBlocked() {
	return bBlocked;
}

// D: block the agent
void CDialogAgent::Block() {
	// set blocked to true
	bBlocked = true;
	// and call recursively for all the subagents
	for(unsigned int i=0; i < SubAgents.size(); i++) 
		SubAgents[i]->Block();
}

// D: unblock the agent
void CDialogAgent::UnBlock() {
	// set blocked to false
	bBlocked = false;
	// and call recursively for all the subagents
	for(unsigned int i=0; i < SubAgents.size(); i++) 
		SubAgents[i]->UnBlock();
}

//-----------------------------------------------------------------------------
// Access to dynamic agent ID information
//-----------------------------------------------------------------------------
// D: sets the dynamic agent flag
void CDialogAgent::SetDynamicAgent() {
    bDynamicAgent = true;
    for(unsigned int i = 0; i < SubAgents.size(); i++)
        SubAgents[i]->SetDynamicAgent();
}

// D: obtains the dynamic agent flag
bool CDialogAgent::IsDynamicAgent() {
    return bDynamicAgent;
}

// D: setting the dynamic agent ID
void CDialogAgent::SetDynamicAgentID(string sADynamicAgentID) {
    // set the agent to dynamic
    bDynamicAgent = true;
    // set the dynamic agent ID
    sDynamicAgentID = sADynamicAgentID;
    // and set it for its subagents, too
    for(unsigned int i = 0; i < SubAgents.size(); i++)
        SubAgents[i]->SetDynamicAgentID(sDynamicAgentID);
}

// D: returning the dynamic agent ID
string CDialogAgent::GetDynamicAgentID() {
    return sDynamicAgentID;
}

//-----------------------------------------------------------------------------
// Execute counter methods, Turns in focus count methods
//-----------------------------------------------------------------------------
// D: increment the execute count
void CDialogAgent::IncrementExecuteCounter() {
    iExecuteCounter++;
}

// D: obtain the value of the execute count
int CDialogAgent::GetExecuteCounter() {
    return iExecuteCounter;
}

//-----------------------------------------------------------------------------
// Access to reset and reopen information
//-----------------------------------------------------------------------------
// D: check if an agent was reset
bool CDialogAgent::WasReset() {
    return iResetCounter > 0;
}

// D: check if an agent was reopened
bool CDialogAgent::WasReOpened() {
    return iReOpenCounter > 0;
}

//-----------------------------------------------------------------------------
// Access to turns in focus counter
//-----------------------------------------------------------------------------
// D: increment the turns in focus counter
void CDialogAgent::IncrementTurnsInFocusCounter() {
    iTurnsInFocusCounter++;
}

// D: obtain the value of the turns in focus counter
int CDialogAgent::GetTurnsInFocusCounter() {
    return iTurnsInFocusCounter;
}

//-----------------------------------------------------------------------------
// Access to the last input index 
//-----------------------------------------------------------------------------
// D: set the last input index 
void CDialogAgent::SetLastInputIndex(int iInputIndex) {
    iLastInputIndex = iInputIndex;
}

// D: obtain a pointer to the last input index
int CDialogAgent::GetLastInputIndex() {
    return iLastInputIndex;
}

//-----------------------------------------------------------------------------
// Access to the last execution index 
//-----------------------------------------------------------------------------
// A: set the last execution index 
void CDialogAgent::SetLastExecutionIndex(int iExecutionIndex) {
    iLastExecutionIndex = iExecutionIndex;
}

// D: obtain a pointer to the last execution index
int CDialogAgent::GetLastExecutionIndex() {
    return iLastExecutionIndex;
}

//-----------------------------------------------------------------------------
// Access to the last bindings index
//-----------------------------------------------------------------------------
// D: set the last bindings index 
void CDialogAgent::SetLastBindingsIndex(int iBindingsIndex) {
    iLastBindingsIndex = iBindingsIndex;
}

// D: get the last bindings index 
int CDialogAgent::GetLastBindingsIndex() {
    return iLastBindingsIndex;
}

//-----------------------------------------------------------------------------
// 
// Protected methods for parsing various declarative constructs
//
//-----------------------------------------------------------------------------
	
// D: Parse a grammar mapping into a list of expectations
void CDialogAgent::parseGrammarMapping(string sConceptNames, 
	
	string sGrammarMapping, TConceptExpectationList& rcelExpectationList) {

	// empty the list
	rcelExpectationList.clear();

	// parse it, construct the appropriate expectation structures and add them
	// to the list 

	string sItem;	// take one item at a time
	
	// while there are still thing left in the string
	while(!sGrammarMapping.empty()) {
		
		TConceptExpectation ceExpectation; // the concept expectation definition

		// get the first item
		SplitOnFirst(sGrammarMapping, ",", sItem, sGrammarMapping);
		sItem = Trim(sItem);

		// process the item and construct the expectation
		string sLeftSide, sRightSide;

		// decide what is the binding method
        if(SplitOnFirst(sItem, ">", sLeftSide, sRightSide)) {
            sRightSide = Trim(sRightSide);
            if(sRightSide[0] == ':') 
                ceExpectation.bmBindMethod = bmBindingFilter;
            else
                ceExpectation.bmBindMethod = bmExplicitValue;
        } else {
            ceExpectation.bmBindMethod = bmSlotValue;
        }

		sLeftSide = Trim(sLeftSide);

		// analyze if the expectation is open at this point or not
		// (i.e. do we have [slot] or ![slot] or @[slot] or @(agent,agent)[slot])
		if(sLeftSide[0] == '[') {
			// if a simple concept mapping, then we declare it only if it's 
			// under the main topic (disable it otherwise)
			ceExpectation.bDisabled = 
                !pDTTManager->IsAncestorOrEqualOf(
                    pDMCore->GetCurrentMainTopicAgent()->GetName(), sName);
			if(ceExpectation.bDisabled) {
				ceExpectation.sReasonDisabled = "[] not under topic";
			}
			ceExpectation.sGrammarExpectation = sLeftSide;
            ceExpectation.sExpectationType = "";
		} else if(sLeftSide[0] == '!') {
			// if a ![] concept mapping, declare it only if we are under focus
			ceExpectation.bDisabled = !pDMCore->AgentIsInFocus(this);
			if(ceExpectation.bDisabled) {
				ceExpectation.sReasonDisabled = "![] not under focus";
			}
			ceExpectation.sGrammarExpectation = 
                sLeftSide.substr(1, sLeftSide.length()-1);
            ceExpectation.sExpectationType = "!";
		} else if((sLeftSide[0] == '@') || (sLeftSide[0] == '*')) {
            if(sLeftSide[1] == '[') {
			    // if a @[] or *[] concept mapping, then always declare it
			    ceExpectation.bDisabled = false;
			    ceExpectation.sGrammarExpectation = 
                    sLeftSide.substr(1, sLeftSide.length()-1);
            } else if(sLeftSide[1] == '(') {
                // if a @(agent,agent)[] or *(agent,agent)[] concept mapping, 
                // then declare it only if the focus is under one of those agents
                // start by constructing the list of agents
                string sAgents;
                SplitOnFirst( sLeftSide, ")", sAgents, ceExpectation.sGrammarExpectation);
                sAgents = sAgents.substr(2, sAgents.length()-2);
                TStringVector vsAgents = PartitionString(sAgents, ";");

                // figure out the focused task agent
                CDialogAgent* pdaDTSAgentInFocus = pDMCore->GetDTSAgentInFocus();
                if(!pdaDTSAgentInFocus) 
                    FatalError("Could not find a DTS agent in focus.");
                string sFocusedAgentName = pdaDTSAgentInFocus->GetName();

                // go through the agents in the list and figure out if they contain the
                // focus
                ceExpectation.bDisabled = true;
                for(unsigned int i = 0; i < vsAgents.size(); i++) {
                    if(pDTTManager->IsAncestorOrEqualOf(A(vsAgents[i]).GetName(), 
                            sFocusedAgentName)) {
                        ceExpectation.bDisabled = false;
                        break;
                    }
                }
                if(ceExpectation.bDisabled) {
                    ceExpectation.sReasonDisabled = 
                        FormatString("%c(%s) not containing focus", sLeftSide[0], 
                            sAgents.c_str());
                }        
            }
            // finally, set the expectation type
            ceExpectation.sExpectationType = FormatString("%c", sLeftSide[0]);
		}

		// close the expectation if the agent path is blocked
		if(IsAgentPathBlocked()) {
			ceExpectation.bDisabled = true;
			ceExpectation.sReasonDisabled = "agent path blocked";
		}

		// if we bind an explicitly specified concept value
		if(ceExpectation.bmBindMethod == bmExplicitValue) {
			// set the sExplicitValue member
			ceExpectation.sExplicitValue = sRightSide;
        } else if(ceExpectation.bmBindMethod == bmBindingFilter) {
            // set the sBindingFilterName member
            ceExpectation.sBindingFilterName = 
                sRightSide.substr(1, sRightSide.length() - 1);
        }
		
		// fill in the rest of the members of the expectation structure
		ceExpectation.pDialogAgent = this;
		ceExpectation.vsOtherConceptNames = PartitionString(sConceptNames, ";,");
    //暂时
    //cout << "---------------查看初始化agenda时vsOtherConceptNames[0]的值------------------"<<endl;
    //vector<string> tmp = PartitionString(sConceptNames,";,");
    //cout << "---------------"<<tmp.front()<<"--------------------------"<<endl;
	    // now go through this and replace the names with the agent qualified
	    // versions
	    for(unsigned int i = 0; i < ceExpectation.vsOtherConceptNames.size(); i++){
	        ceExpectation.vsOtherConceptNames[i] = 
	            C(Trim(ceExpectation.vsOtherConceptNames[i])).GetAgentQualifiedName();
          //cout << "经过C函数处理过后为"<<ceExpectation.vsOtherConceptNames[i]<<endl;
      }
	    // now assing the name of the main expected concept
		ceExpectation.sConceptName = ceExpectation.vsOtherConceptNames[0];
    //cout << "最终sConceptName为"<<ceExpectation.sConceptName<<endl;
		// and delete the first one from the others list
		ceExpectation.vsOtherConceptNames.erase(
		    ceExpectation.vsOtherConceptNames.begin());

        // finally lowecase the grammar expectation
        ceExpectation.sGrammarExpectation = 
            ToLowerCase(ceExpectation.sGrammarExpectation);

		// add the expectation to the list
		rcelExpectationList.push_back(ceExpectation);
	}
}

