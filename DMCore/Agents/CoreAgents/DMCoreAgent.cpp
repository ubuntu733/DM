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
// DMCOREAGENT.CPP - implements the core dialog management agent, that handles
//				     execution and input passes (agenda generation, bindings, 
//				     etc)
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
//   [2007-03-05] (antoine): changed Execute so that grounding and dialog agents
//							 are only executed once the floor is free and all 
//							 pending prompt notifications have been received 
//							 (previously, only the floor mattered)
//   [2005-06-20] (antoine): copied DMCoreAgent
//
//-----------------------------------------------------------------------------

//#include "DMInterfaces/DMInterface.h"
#include "DMCore/Agents/Registry.h"
#include "DMCore/Core.h"
#ifdef GALAXY
#include "DMCore/Events/GalaxyInteractionEvent.h"
#endif
#include "DMCoreAgent.h"


// *** *** BIG QUESTION: What core stuff do we log, and where ?
// 1. We need to log the compiled agenda at each input pass
// 2. Maybe it would be nice to log the execution stack, or at least a trace
//    of it. 
// 3. Not only the open concepts, but also the bindings...

// *** Concept Binding and generally the Input PASS need to be revisited 
//     since they were not implemented completely

// A: A vector of strings containing labels for TFloorStatus variables
vector<string> vsFloorStatusLabels;


//-----------------------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------------------

// D: constructor
CDMCoreAgent::CDMCoreAgent(string sAName, string sAConfiguration,
						   string sAType) : 
	CAgent(sAName, sAConfiguration, sAType) {
    bFocusClaimsPhaseFlag = false;
	fsFloorStatus = fsSystem;
	iTurnNumber = 0;
    csoStartOverFunct = NULL;
	vsFloorStatusLabels.push_back("unknown");
	vsFloorStatusLabels.push_back("user");
	vsFloorStatusLabels.push_back("system");
	vsFloorStatusLabels.push_back("free");
}

// D: virtual destructor - does nothing so far
CDMCoreAgent::~CDMCoreAgent() {
}

//-----------------------------------------------------------------------------
// Static function for dynamic agent creation
//-----------------------------------------------------------------------------
CAgent* CDMCoreAgent::AgentFactory(string sAName, string sAConfiguration) {
	return new CDMCoreAgent(sAName, sAConfiguration);
}

//-----------------------------------------------------------------------------
// CAgent class overwritten methods
//-----------------------------------------------------------------------------

// D: the overwritten Reset method
void CDMCoreAgent::Reset() {
	// clear the class members
	esExecutionStack.clear();
	ehExecutionHistory.clear();
    bhBindingHistory.clear();
	eaAgenda.celSystemExpectations.clear();
	eaAgenda.vCompiledExpectations.clear();
}

//-----------------------------------------------------------------------------
//
// CoreAgent specific methods
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//
// DIALOG CORE EXECUTION
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// D: this function does the actual dialog task execution, by executing the
//    agents that are on the stack, and issuing input passes when appropriate
//-----------------------------------------------------------------------------

void CDMCoreAgent::Execute() {

	// create & initialize the dialog task
	pDTTManager->CreateDialogTree();

	// The floor is initially free
	SetFloorStatus(fsFree);

	// put the root of the dialog task on the execution stack
	Log(DMCORE_STREAM, "Starting Dialog Task execution.");
	ContinueWith(this, pDTTManager->GetDialogTaskTreeRoot());

	// creates the initial dialog state
	pStateManager->UpdateState();

	// do the while loop for execution
	while(!esExecutionStack.empty()) {
    //暂时
    //list<TExecutionStackItem>::iterator i;
    //cout <<"**************打印堆栈信息*************"<<endl;
    //for(i=esExecutionStack.begin();i!=esExecutionStack.end();i++){
      //TExecutionStackItem tmp = *i;
      //cout<< "angent是" << tmp.pdaAgent->GetName() <<endl;
    //}
    //cout << "**************************************"<<endl;
		string sTurnId = "User:???";
		if (pInteractionEventManager->GetLastInput())
			sTurnId = "User:" + pInteractionEventManager->GetLastInput()->GetStringProperty("[uttid]");

		Log(DMCORE_STREAM, "Starting DMCore processing [%s]", sTurnId.c_str());

		// eliminate all the agents that have completed from the execution
		// stack
		popCompletedFromExecutionStack();

		Log(DMCORE_STREAM, "Eliminated completed agents from stack [%s]", sTurnId.c_str());

		// if the execution stack is empty now, break the loop
        if(esExecutionStack.empty()) 
            break; 

		// Performs grounding only when the floor is free and we got all our notifications
    // 按照android版本修改
		if ((GetFloorStatus() == fsFree) && true) {
			//(pOutputManager->GetPromptsWaitingForNotification() == "")) {

			if (pGroundingManager->HasPendingRequests() || 
			 pGroundingManager->HasScheduledConceptGroundingRequests())
				pGroundingManager->Run();
			// now pop completed
			int iPopped = popCompletedFromExecutionStack();

			Log(DMCORE_STREAM, "Performed grounding [%s]", sTurnId.c_str());

			// now while new grounding requests appear, keep running the 
			// grounding process
			while((iPopped > 0) && 
				pGroundingManager->HasUnprocessedConceptGroundingRequests()) {
				// run it
				pGroundingManager->Run();
				// eliminate all the agents that have completed (potentially as a 
				// result of the grounding phase) from the execution stack
				iPopped = popCompletedFromExecutionStack();
			}

			Log(DMCORE_STREAM, "Completed grounding on [%s]", sTurnId.c_str());
		}

		// now, run the focus analysis process if the core was flagged to
        // do so, and if there are no scheduled grounding activities
        if(bFocusClaimsPhaseFlag) {
  	        // Analyze the need for a focus shift, and resolve it if necessary
            if(assembleFocusClaims())
	            resolveFocusShift();
            // reset the flag
            bFocusClaimsPhaseFlag = false;
        }

        // if the execution stack is empty now, break the loop
        if(esExecutionStack.empty()) 
            break;            

        // grab the first (executable) dialog agent from the stack
        CDialogAgent* pdaAgentInFocus = GetAgentInFocus();

		// check that we found a proper one (if there's nothing else to be
		//  executed, we're done)
		if(!pdaAgentInFocus) break;

		// if the floor is not free (or we're still waiting for notifications), do not execute
		// agents that require the floor, just wait for the next event
    //cout << pdaAgentInFocus->GetName() <<endl;
    //cout << pdaAgentInFocus->HasCompleted() <<endl;
/*    cout << pdaAgentInFocus->RequiresFloor() <<endl;*/
    //cout << (GetFloorStatus()==fsFree) <<endl;
    //cout << (pOutputManager->GetPromptsWaitingForNotification()== "")<<endl;
		//if (pdaAgentInFocus->RequiresFloor() && 
			//!((GetFloorStatus() == fsFree) && 
				//(pOutputManager->GetPromptsWaitingForNotification() == ""))) {
			//AcquireNextEvent();
			//continue;
		//}

		// and execute it
		Log(DMCORE_STREAM, "Executing dialog agent %s [%s]", 
			pdaAgentInFocus->GetName().c_str(), sTurnId.c_str()); 
		
        // mark the time it was executed
	 /* ehExecutionHistory[esExecutionStack.front().iEHIndex].\*/
            //vtExecutionTimes.push_back(GetTime());
		
        // execute it
		TDialogExecuteReturnCode dercResult = pdaAgentInFocus->Execute();
		ehExecutionHistory[esExecutionStack.front().iEHIndex].bExecuted = true;

        // and now analyze the return
		switch(dercResult) {
		case dercContinueExecution:
			// continue the execution
			break;
		
        case dercFinishDialog:
            // finish the dialog
			Log(DMCORE_STREAM, "Dialog Task Execution completed. Dialog finished");
			return;
        
        case dercFinishDialogAndCloseSession:
			// tell the hub to close the session
			/*Log(DMCORE_STREAM, "Sending close_session to the hub");*/
			//DMI_SendEndSession();
            //// finish the dialog
			/*Log(DMCORE_STREAM, "Dialog Task Execution completed. Dialog finished");*/
			return;
        
        case dercRestartDialog:
            // call the start over routine
            StartOver();
            break;

		case dercYieldFloor:

			// gives the floor to the user
			SetFloorStatus(fsUser);

			// wait for the next event
			AcquireNextEvent();
            break;

		case dercTakeFloor:

			// gives the floor to the user
			SetFloorStatus(fsSystem);

			// wait for the next event
			AcquireNextEvent();
            break;

		case dercWaitForEvent:

			// wait for the next event
			AcquireNextEvent();
            break;
		}
	}

	// tell the hub to close the session
	Log(DMCORE_STREAM, "Sending close_session to the hub");
	//DMI_SendEndSession();
	Log(DMCORE_STREAM, "Dialog Task Execution completed. Dialog finished.");
}

//-----------------------------------------------------------------------------
// A: Waits for and processes the next real-world event
//-----------------------------------------------------------------------------
#pragma warning (disable:4127)
void CDMCoreAgent::AcquireNextEvent() {

	pInteractionEventManager->WaitForEvent();

	// Unqueue event
	CInteractionEvent *pieEvent = pInteractionEventManager->GetNextEvent();
  //cout << "event"<<pieEvent->GetType()<<"完成情况" << pieEvent->IsComplete() <<endl;
	// try and bind concepts 
	TBindingsDescr bdBindings;
	bindConcepts(bdBindings);
  //暂时
  //cout <<"***************显示绑定数量*************"<<endl;
  //cout << "iConceptsBlocked=";
  //cout << bdBindings.iConceptsBlocked <<endl;
  //cout << "iConceptsBound=";
  //cout << bdBindings.iConceptsBound <<endl;
  //cout << "msiSlotsMatched="<<bdBindings.iSlotsMatched<<endl;
  //cout << "***************************************"<<endl;
  //暂时
  //cout << "*****************显示eaagenda.vCompiledExpectations信息***********"<<endl;
  //for (int i = 0; i<eaAgenda.vCompiledExpectations.size(); i++) {
    //TMapCE::iterator iptr;
    //for(iptr = eaAgenda.vCompiledExpectations[i].mapCE.begin();
        //iptr != eaAgenda.vCompiledExpectations[i].mapCE.end();
        //iptr++) {
      //cout << "第"<<i<<"层的slot是"<<iptr->first<<endl;
    //}
    //cout << "第"<<i<<"层的angent是"<<eaAgenda.vCompiledExpectations[i].pdaGenerator->GetName()<<endl;
  //}
  //cout << "********************************************************************"<<endl;
  //cout << "******************显示eaAgenda.celSystemExpectations信息*********"<<endl;
  //for (int i=0; i<eaAgenda.celSystemExpectations.size();i++){
    //TConceptExpectation tmp = eaAgenda.celSystemExpectations[i];
    //cout << "第" << i << "个";
    //cout << "agent 是"<<tmp.pDialogAgent->GetName()<<endl;
    //cout << "contesxAgent为"<<tmp.pDialogAgent->GetContextAgent()->GetName()<<endl;
    //cout << "父节点为"<<tmp.pDialogAgent->GetParent()->GetName()<<endl;
    //cout << "concept 是"<<tmp.sConceptName<<endl;
    //cout << "vsOtherConceptNames大小为"<<tmp.vsOtherConceptNames.size()<<endl;
    //cout <<"sExplicitValue是"<<tmp.sExplicitValue<<endl;
    //cout << "Grammar 是"<<tmp.sGrammarExpectation<<endl;
    //cout << "bindmethod 是"<<tmp.bmBindMethod<<endl;
    //cout << "bDisabled是"<<tmp.bDisabled<<endl;
    //cout << "sReasonDisabled是"<<tmp.sReasonDisabled<<endl;
  //}
  //cout << "******************************************************************"<<endl;	
   //add the binding results to history
	bhBindingHistory.push_back(bdBindings);

	// Set the bindings index on the focused agent
	GetAgentInFocus()->SetLastBindingsIndex(bhBindingHistory.size() - 1);

	// signal the need for a focus claims phase
	SignalFocusClaimsPhase();

	Log(DMCORE_STREAM, "Acquired new %s event.", pieEvent->GetType().c_str());

	// update the floor status if the event specifies it
	if (pieEvent->HasProperty("[floor_status]"))
		SetFloorStatus(StringToFloorStatus(pieEvent->GetStringProperty("[floor_status]")));

	// Process event
	if (pieEvent->GetType() == IET_USER_UTT_START) {

	} else if (pieEvent->GetType() == IET_USER_UTT_END ||
               pieEvent->GetType() == IET_GUI) {

		if (pieEvent->IsComplete()) {
			// Set the last input on the focused agent
			GetAgentInFocus()->SetLastInputIndex(iTurnNumber);
			GetAgentInFocus()->IncrementTurnsInFocusCounter();

			iTurnNumber++;

			// signal the need for a turn grounding
			pGroundingManager->RequestTurnGrounding();
      //根据android版本修改
      pStateManager->UpdateState();
			Log(DMCORE_STREAM, "Processed new input [User:%s]", pieEvent->GetStringProperty("[uttid]").c_str());

			return;
		}

	} else if (pieEvent->GetType() == IET_SYSTEM_UTT_START) {
    //目前没做输入，暂时注释
		// sends notification information to the OutputManager
 /*   pOutputManager->PreliminaryNotify(pieEvent->GetIntProperty("[utt_count]"), */
			//pieEvent->GetStringProperty("[tagged_prompt]"));

		Log(DMCORE_STREAM, "Processed preliminary output notification.");
	} else if (pieEvent->GetType() == IET_SYSTEM_UTT_END) {
    //暂时注释
		// sends notification information to the OutputManager
	 /* pOutputManager->Notify(pieEvent->GetIntProperty("[utt_count]"), */
			//pieEvent->GetIntProperty("[bargein_pos]"),
			//pieEvent->GetStringProperty("[conveyance]"),
			//pieEvent->GetStringProperty("[tagged_prompt]"));

		Log(DMCORE_STREAM, "Processed output notification.");
	} else if (pieEvent->GetType() == IET_SYSTEM_UTT_CANCELED) {
    //暂时注释
		// sends notification information to the OutputManager
	 /* pOutputManager->Notify(pieEvent->GetIntProperty("[utt_count]"), 0, "", "");*/

		Log(DMCORE_STREAM, "Output cancel notification processed.");		
	} else if (pieEvent->GetType() == IET_DIALOG_STATE_CHANGE) {

		pStateManager->UpdateState();
		//pStateManager->BroadcastState();

	} else {
	}
}
#pragma warning (default:4127)

// J:	Updates the speech recognizer's configuration to the one
//		specified by the agent on the top of the stack.
//		For instance, the hash [(set_lm=my_lm),(set_dtmf_len=4)] will be 
//		converted into a GalFrame [..., :set_lm=my_lm, :set_dtmf_len=4, ...]
void CDMCoreAgent::updateInputLineConfiguration() {
    // grab the first dialog agent from the stack
	CDialogAgent* pdaCurrent = esExecutionStack.front().pdaAgent;

	// obtain the hash describing the entire configuration
	STRING2STRING s2sConfig = pdaCurrent->GetInputLineConfiguration();
	
	// log it
	Log(DMCORE_STREAM, "Input line configuration dumped below.\n%s", 
	    S2SHashToString(s2sConfig, "\n").c_str());
	
	if (s2sConfig.size() > 0) {

		// if we are in a Galaxy configuration, send requests through the Galaxy 
		// interface
		#ifdef GALAXY	
		TGIGalaxyCall gcGalaxyCall;
		gcGalaxyCall.sModuleFunction = "sphinx.set_config";
		gcGalaxyCall.bBlockingCall = false;     
		if(s2sConfig.size() > 0) {
			STRING2STRING::iterator iPtr;
			for(iPtr = s2sConfig.begin(); 
				iPtr != s2sConfig.end(); 
				iPtr++) {

					gcGalaxyCall.s2sInputs.insert(STRING2STRING::value_type(":"+iPtr->first, 
																	iPtr->second));
			}
		}
		// retrieve the current thread id
		DWORD dwThreadId = GetCurrentThreadId();

		// send the message to the Galaxy Interface Thread
		PostThreadMessage(g_idDMInterfaceThread, WM_GALAXYCALL,
							(WPARAM)&gcGalaxyCall, dwThreadId);	

		// and wait for a reply
		MSG Message;
		GetMessage(&Message, NULL, WM_ACTIONFINISHED, WM_ACTIONFINISHED);
		#endif // GALAXY
			
	}
}

// D: Registers a customized binding filter
void CDMCoreAgent::RegisterBindingFilter(string sBindingFilterName, 
                                         TBindingFilterFunct bffFilter) {
    // check if it's already in the map
    if(s2bffFilters.find(sBindingFilterName) != s2bffFilters.end()) {
        Warning(FormatString("Could not add binding filter %s, as it already "\
                             "exists.", sBindingFilterName.c_str()));            
    } else {
        // add it if not already in the map
        s2bffFilters.insert(
            STRING2BFF::value_type(sBindingFilterName, bffFilter));
    }
}

//---------------------------------------------------------------------
// A: Methods for accessing Interface-specific variables
//---------------------------------------------------------------------

int CDMCoreAgent::GetIntSessionID() {
	//return DMI_GetIntSessionID();
  return 1;
}

//-----------------------------------------------------------------------------
// D: Expectation Agenda functions
//-----------------------------------------------------------------------------

// D: collects and logs the concepts that undergo grounding
void CDMCoreAgent::dumpConcepts() {

	// the string in which we construct the concepts dump
	string sConceptsDump;

	// first collect the list of concepts that undergo grounding 
    TConceptPointersVector cpvConcepts;
    // use a set to record the seen grounding models and avoid duplicates
    TConceptPointersSet cpsExcludeConcepts;
    TExecutionStack::iterator iPtr;
	for(iPtr = esExecutionStack.begin(); 
        iPtr != esExecutionStack.end(); 
		iPtr++) {
		// gather the concepts from each element on the stack               
        iPtr->pdaAgent->DeclareConcepts(cpvConcepts, cpsExcludeConcepts);
    }

	// now go through the list of concept, and for all the ones that have a 
	// grounding model, log them
	for(unsigned int i = 0; i < cpvConcepts.size(); i++) {
		if(cpvConcepts[i]->GetGroundingModel()) {
			sConceptsDump += FormatString(
				"Concept: %s\n%s", 
				cpvConcepts[i]->GetAgentQualifiedName().c_str(),
				cpvConcepts[i]->HypSetToString().c_str());
		}
	}

	// now log the concepts
	Log(CONCEPT_STREAM, "Grounding-relevant concepts dumped below:\n%s", 
        sConceptsDump.c_str());
}


// D: dumps the execution stack to the log
void CDMCoreAgent::dumpExecutionStack() {
    Log(DMCORE_STREAM, "Execution stack dumped below:\n%s", 
        executionStackToString().c_str());
}

// A: converts the current execution stack into a string representation
string CDMCoreAgent::executionStackToString() {
	return executionStackToString(esExecutionStack);
}

// D: converts the given execution stack into a string representation
string CDMCoreAgent::executionStackToString(TExecutionStack es) {
    string sResult;

    // iterate through the execution stack
    TExecutionStack::iterator iPtr;
	for(iPtr = es.begin(); 
        iPtr != es.end(); 
        iPtr++) {
        sResult += FormatString("  %s\n", iPtr->pdaAgent->GetName().c_str());
    }

    // finally, return the string
    return sResult;
}

// D: this method clears the current system action
void CDMCoreAgent::clearCurrentSystemAction() {
	saSystemAction.setcpRequests.clear();
	saSystemAction.setcpExplicitConfirms.clear();
	saSystemAction.setcpImplicitConfirms.clear();
	saSystemAction.setcpUnplannedImplicitConfirms.clear();
}

// D: this method computes the current system action
void CDMCoreAgent::computeCurrentSystemAction() {
	
	// now go through the levels of the agenda, and look for implicit 
	// confirms, explicit confirms and requests
	bool bFoundFocus = false;
	for(unsigned int l = 0; 
		!bFoundFocus && (l < eaAgenda.vCompiledExpectations.size()); 
		l++) {

		// set that we have found the focus
		bFoundFocus = true;

		TMapCE::iterator iPtr;
		// iterate through the compiled expectations from that level
		for(iPtr = eaAgenda.vCompiledExpectations[l].mapCE.begin(); 
			iPtr != eaAgenda.vCompiledExpectations[l].mapCE.end(); 
			iPtr++) {

			string sSlotExpected = iPtr->first;
			TIntVector& rvIndices = iPtr->second;

			// go through the indices
			for(unsigned int i = 0; i < rvIndices.size(); i++) {

				// grab the expectation
				TConceptExpectation& rceExpectation = 
					eaAgenda.celSystemExpectations[rvIndices[i]];

				// find the concept name
				CConcept* pConcept = &(rceExpectation.pDialogAgent->
					C(rceExpectation.sConceptName));

				// find the agent name
				string sAgentName = rceExpectation.pDialogAgent->GetName();

				// now check if the agent is an implicit confirmation
				if(sAgentName.find("_ImplicitConfirmExpect") != -1) {
					// find the name of the confirmed concept
					string sFoo, sConfirmedConcept;
					SplitOnFirst(sAgentName, "[", sFoo, sConfirmedConcept);
					sConfirmedConcept = TrimRight(sConfirmedConcept, "]");

					// signal that we are implicitly confirming that concept
					SignalImplicitConfirmOnConcept(
						&(pDTTManager->GetDialogTaskTreeRoot()->C(sConfirmedConcept)));

					// then we have an implicit confirmation on top of the 
					// stack, mark that this was actually not the focus level
					bFoundFocus = false;
					// then continue
					continue;
				}

				// now check if the agent is an explicit confirmation
				if(sAgentName.find("_ExplicitConfirm") != -1) {
					// find the name of the confirmed concept
					string sFoo, sConfirmedConcept;
					SplitOnFirst(sAgentName, "[", sFoo, sConfirmedConcept);
					sConfirmedConcept = TrimRight(sConfirmedConcept, "]");

					// signal that we are implicitly confirming that concept
					SignalExplicitConfirmOnConcept(
						&(pDTTManager->GetDialogTaskTreeRoot()->C(sConfirmedConcept)));
				}

				// if the expectation is open 
				if(!rceExpectation.bDisabled) {
					// then mark this concept as requested
					saSystemAction.setcpRequests.insert(pConcept);
					// and also go through the other concepts in the expectation and 
					// mark them as requested
                    for(unsigned int i = 0; 
                        i < rceExpectation.vsOtherConceptNames.size(); i++) {
                        saSystemAction.setcpRequests.insert(&(rceExpectation.pDialogAgent->
					        C(rceExpectation.vsOtherConceptNames[i])));
                    }					
				}				
			}
		}
	}

    // now log the current system action 
	Log(DMCORE_STREAM, FormatString("System action dumped below.\n%s", 
		currentSystemActionToString().c_str()));
    
}

// D: this method creates a string representation of the system action   
string CDMCoreAgent::currentSystemActionToString() {
	return systemActionToString(saSystemAction);
}

// D: this method creates a string representation of the system action   
string CDMCoreAgent::systemActionToString(TSystemAction saASystemAction) {

	// now log the current system action
	string sResult = "REQUEST(";
	set<CConcept *>::iterator iPtr;
	for(iPtr = saASystemAction.setcpRequests.begin(); 
		iPtr != saASystemAction.setcpRequests.end();
		iPtr++) {
		sResult += FormatString("%s,", (*iPtr)->GetAgentQualifiedName().c_str());
	}
	sResult = TrimRight(sResult, ",");
	sResult += ")\nEXPL_CONF(";
	for(iPtr = saASystemAction.setcpExplicitConfirms.begin(); 
		iPtr != saASystemAction.setcpExplicitConfirms.end();
		iPtr++) {
		sResult += FormatString("%s,", (*iPtr)->GetAgentQualifiedName().c_str());
	}
	sResult = TrimRight(sResult, ",");
	sResult += ")\nIMPL_CONF(";
	for(iPtr = saASystemAction.setcpImplicitConfirms.begin(); 
		iPtr != saASystemAction.setcpImplicitConfirms.end();
		iPtr++) {
		sResult += FormatString("%s,", (*iPtr)->GetAgentQualifiedName().c_str());
	}
	sResult = TrimRight(sResult, ",");
	sResult += ")\nUNPLANNED_IMPL_CONF(";
	for(iPtr = saASystemAction.setcpUnplannedImplicitConfirms.begin(); 
		iPtr != saASystemAction.setcpUnplannedImplicitConfirms.end();
		iPtr++) {
		sResult += FormatString("%s,", (*iPtr)->GetAgentQualifiedName().c_str());
	}
	sResult = TrimRight(sResult, ",");
	sResult += ")";
	
	return sResult;
}

// D: assembles the expectation agenda
void CDMCoreAgent::assembleExpectationAgenda() {

	Log(DMCORE_STREAM, "Expectation Agenda Assembly Phase initiated.");
	
	// first collect and compile the expectation agenda
	compileExpectationAgenda();

	// then enforce the binding policies as specified on each level
	enforceBindingPolicies();

	// dump agenda to the log
	Log(EXPECTATIONAGENDA_STREAM, "Concept expectation agenda dumped below:"
		+ expectationAgendaToString());

	Log(DMCORE_STREAM, "Expectation Agenda Assembly Phase completed "\
		"(%d levels).", eaAgenda.vCompiledExpectations.size());
}

// D: gathers the expectations and compiles them in an fast accessible
//    form
// D: definition of an internal type: a set of pointers to an agent
typedef set<CDialogAgent*, less <CDialogAgent*>, allocator <CDialogAgent*> > 
	TDialogAgentSet;

void CDMCoreAgent::compileExpectationAgenda() {

	// log the activity
	Log(DMCORE_STREAM, "Compiling Expectation Agenda ...");

	// first clear up the last agenda
	eaAgenda.celSystemExpectations.clear();
	eaAgenda.vCompiledExpectations.clear();

	// get the list of system expectations. To do this, we traverse 
	// the execution stack, and add expectations from all the agents, each 
	// on the appropriate level; also keep track of the expectations
	// that are already declared so as not to duplicate them by this
	// traversal
	int iLevel = 0;
	TDialogAgentSet setPreviouslySeenAgents;	// the set of agents already
												// seen on the previous levels
	TDialogAgentSet setCurrentlySeenAgents;		// the set of agents seen
												// on the current level
									
	TExecutionStack::iterator iPtr;
	for(iPtr = esExecutionStack.begin(); 
        iPtr != esExecutionStack.end(); 
        iPtr++) {
		
		// remember how big the system expectation agenda was so far
		int iStartIndex = eaAgenda.celSystemExpectations.size();
		
		// gather expectations of the agent on the stack indicated by iPtr
		iPtr->pdaAgent->DeclareExpectations(eaAgenda.celSystemExpectations);

		// now go thourgh those new expectations and compile them (create 
		// the corresponding entry into the vCompiledExpectations array)
		TCompiledExpectationLevel celLevel;
		// set the agent that generated this level
		celLevel.pdaGenerator = iPtr->pdaAgent;
		for(unsigned int i = iStartIndex; 
			i < eaAgenda.celSystemExpectations.size();
			i++) {

			// check that the agent was not already seen on the previous
			// level (in this case, avoid duplicating its expectation)
			if(setPreviouslySeenAgents.find(
				eaAgenda.celSystemExpectations[i].pDialogAgent) != 
				setPreviouslySeenAgents.end()) {
				continue;
			}

			// insert this agent in the list of currently seen agents
			setCurrentlySeenAgents.insert(
				eaAgenda.celSystemExpectations[i].pDialogAgent);

			string sSlotExpected = 
				eaAgenda.celSystemExpectations[i].sGrammarExpectation;
			TMapCE::iterator iPtr2;
			if((iPtr2 = celLevel.mapCE.find(sSlotExpected)) != 
                celLevel.mapCE.end()) {
				// if this grammar slot is already expected at this level
    		    // just add to the vector of pointers
				TIntVector& rvIndices = (*iPtr2).second;
    		    rvIndices.push_back(i);
			} else {
				// if the concept is NOT already expected at this level
				// then add it to the hash of compiled expectations
				TIntVector ivTemp;
				ivTemp.push_back(i);
				celLevel.mapCE.insert(
					TMapCE::value_type(sSlotExpected, ivTemp));
			}			
		}

		// finally, we have assembled and compiled this level of expectations,
		// push it on the array, 
		eaAgenda.vCompiledExpectations.push_back(celLevel);

		// update the set of already seen agents
		setPreviouslySeenAgents.insert(setCurrentlySeenAgents.begin(), 
									   setCurrentlySeenAgents.end());

		// and move to the next level
		iLevel++;	
	}

	// log the activity
	Log(DMCORE_STREAM, "Compiling Expectation Agenda completed.");
}

// D: goes through the compiled agenda, and modifies it according to the 
//    binding policies as specified by each level's generator agents
void CDMCoreAgent::enforceBindingPolicies() {

	// log the activity
	Log(DMCORE_STREAM, "Enforcing binding policies ...");

	// at this point, this only consists of blocking the upper levels if a 
	// WITHIN_TOPIC_ONLY policy is detected
	for(unsigned int i = 0; i < eaAgenda.vCompiledExpectations.size(); i++) {
		// get the binding policy for this level
		string sBindingPolicy = 
			eaAgenda.vCompiledExpectations[i].pdaGenerator->DeclareBindingPolicy();
		if(sBindingPolicy == WITHIN_TOPIC_ONLY) {
			// if WITHIN_TOPIC_ONLY, then all the expectations from the upper
			// levels of the agenda are disabled
			for(unsigned int l = i+1; 
				l < eaAgenda.vCompiledExpectations.size(); 
				l++) {
				// go through the whole level and disable all expectations
				TMapCE::iterator iPtr;
				for(iPtr = eaAgenda.vCompiledExpectations[l].mapCE.begin(); 
					iPtr != eaAgenda.vCompiledExpectations[l].mapCE.end(); 
					iPtr++) {
					// access the indices 
					TIntVector& rivTemp = iPtr->second;
					for(unsigned int ii = 0; ii < rivTemp.size(); ii++) {
						int iIndex = rivTemp[ii];
                        // don't disable it if it's a *-type expectation
                        if(eaAgenda.celSystemExpectations[iIndex].\
                            sExpectationType != "*") {
						    eaAgenda.celSystemExpectations[iIndex].bDisabled = 
                                true;
						    eaAgenda.celSystemExpectations[iIndex].sReasonDisabled = 
							    "within-topic binding policy"; // *** add on what
                        }
					}
				}
			}
			// break the for loop since it already doesn't matter what the 
			// policy is on upper contexts
			break;
		}
	}

	// log the activity
	Log(DMCORE_STREAM, "Enforcing binding policies completed.");
}

// D: generates a string representation of the expectation agenda
//    this string is used so far only for logging purposes
string CDMCoreAgent::expectationAgendaToString() {
	string sResult;
	// go through all the levels of the agenda
	for(unsigned int l = 0; l < eaAgenda.vCompiledExpectations.size(); l++) {
		sResult += FormatString("\n Level %d: generated by %s", l, 
			eaAgenda.vCompiledExpectations[l].pdaGenerator->GetName().c_str());
		TMapCE::iterator iPtr;
		// iterate through the compiled expectations from that level
		for(iPtr = eaAgenda.vCompiledExpectations[l].mapCE.begin(); 
			iPtr != eaAgenda.vCompiledExpectations[l].mapCE.end(); 
			iPtr++) {
			string sSlotExpected = iPtr->first;
			TIntVector& rvIndices = iPtr->second;
			// convert expectations to string description
			for(unsigned int i = 0; i < rvIndices.size(); i++) {
				TConceptExpectation& rceExpectation = 
					eaAgenda.celSystemExpectations[rvIndices[i]];
				sResult += (rceExpectation.bDisabled)?"\n  X ":"\n  O ";
				sResult += rceExpectation.sGrammarExpectation + " -> (" + 
						   rceExpectation.pDialogAgent->GetName() + ")" +
						   rceExpectation.sConceptName;
				if(rceExpectation.bDisabled) {
					sResult += " [X-" + rceExpectation.sReasonDisabled + "]";
				}
			}
		}
	}
	// finally, return the string
	return sResult;
}

// A: generates a string representation of the current agenda
string CDMCoreAgent::expectationAgendaToBroadcastString() {
	return expectationAgendaToBroadcastString(eaAgenda);
}

// D: generates a string representation of the expectation agenda
//    that is used to broadcast it to the outside world
string CDMCoreAgent::expectationAgendaToBroadcastString(TExpectationAgenda eaBAgenda) {
	string sResult;
    STRING2STRING s2sAllOpenGrammarExpectations;
	// go through all the levels of the agenda
	for(unsigned int l = 0; l < eaBAgenda.vCompiledExpectations.size(); l++) {
        sResult += FormatString("\n%d:", l);
        TMapCE::iterator iPtr;
		// iterate through the compiled expectations from that level
		for(iPtr = eaBAgenda.vCompiledExpectations[l].mapCE.begin(); 
			iPtr != eaBAgenda.vCompiledExpectations[l].mapCE.end(); 
			iPtr++) {
			string sSlotExpected = iPtr->first;
			TIntVector& rvIndices = iPtr->second;

            TIntVector vOpenIndices;
            set<CConcept *> scpOpenConcepts;
            TIntVector vClosedIndices;
            set<CConcept *> scpClosedConcepts;

            for(unsigned int i = 0; i < rvIndices.size(); i++) {
		    	TConceptExpectation& rceExpectation = 
			    	eaBAgenda.celSystemExpectations[rvIndices[i]];
				// determine the concept under consideration
                CConcept* pConcept = 
                    &(rceExpectation.pDialogAgent->C(
                        rceExpectation.sConceptName));
                
                // test that the expectation is not disabled
                if(!eaBAgenda.celSystemExpectations[rvIndices[i]].bDisabled) {
                    if(scpOpenConcepts.find(pConcept) == scpOpenConcepts.end()) {
	                    // add it to the open indices list
						vOpenIndices.push_back(rvIndices[i]);
                        // add the concept to the open concepts list
                        scpOpenConcepts.insert(pConcept);
                        // if by any chance it's already in the closed concepts, 
                        set<CConcept *>::iterator iPtr;
                        if((iPtr = scpClosedConcepts.find(pConcept)) != 
                            scpClosedConcepts.end()) {
                            // remove it from there
                            scpClosedConcepts.erase(iPtr);
                        }
                    }
                } else {
                    // o/w if the expectation is disabled
                    if((scpClosedConcepts.find(pConcept) == scpClosedConcepts.end()) &&
                        (scpOpenConcepts.find(pConcept) == scpOpenConcepts.end())) {
	                    // add it to the closed indices list
						vClosedIndices.push_back(rvIndices[i]);
                        // add the concept to the closed concepts list
                        scpClosedConcepts.insert(pConcept);
                    }                        
                }
			}

            // now add the first one in the open indices, if there is any
            // in there

            if(vOpenIndices.size() > 0) {
                TConceptExpectation& rceExpectation = 
                    eaBAgenda.celSystemExpectations[vOpenIndices[0]];
				sResult += "\n"; // (air)
				sResult += "O" + rceExpectation.sGrammarExpectation;

                sResult += 
                    (rceExpectation.bmBindMethod == bmExplicitValue)?"V,":"S,";
            } 

            // finally, add all the blocked ones
            for(unsigned int i = 0; i < vClosedIndices.size(); i++) {
                TConceptExpectation& rceExpectation = 
                    eaBAgenda.celSystemExpectations[vClosedIndices[i]];                    
				sResult += "\n"; // (air)
                sResult += "X" + rceExpectation.sGrammarExpectation;
                sResult += 
                    (rceExpectation.bmBindMethod == bmExplicitValue)?"V,":"S,";                    
            }
		}
        // cut the last comma
        sResult = TrimRight(sResult, ",");
	}
	// finally, return the string
	return Trim(sResult, "\n");
}

// D: generates a string representation of the bindings description
string CDMCoreAgent::bindingsDescrToString(TBindingsDescr& rbdBindings) {
	string sResult;
	// go through all the attempted bindings
    for(unsigned int i = 0; i < rbdBindings.vbBindings.size(); i++) {
        if(rbdBindings.vbBindings[i].bBlocked)
            sResult += FormatString("Fail:    Level.%d\t%s->(%s)%s [%s]\n", 
                rbdBindings.vbBindings[i].iLevel, 
    			rbdBindings.vbBindings[i].sGrammarExpectation.c_str(), 
    			rbdBindings.vbBindings[i].sAgentName.c_str(), 
	    		rbdBindings.vbBindings[i].sConceptName.c_str(), 
		    	rbdBindings.vbBindings[i].sReasonDisabled.c_str());
        else 
		    sResult += FormatString("Success: Level.%d\t%s(%s%s%f)->(%s)%s\n", 
                rbdBindings.vbBindings[i].iLevel, 
                rbdBindings.vbBindings[i].sGrammarExpectation.c_str(), 
			    rbdBindings.vbBindings[i].sValue.c_str(), 
			    VAL_CONF_SEPARATOR, 
			    rbdBindings.vbBindings[i].fConfidence, 
			    rbdBindings.vbBindings[i].sAgentName.c_str(), 
			    rbdBindings.vbBindings[i].sConceptName.c_str());
    }
	// go through all the forced updates
	for(unsigned int i = 0; i < rbdBindings.vfcuForcedUpdates.size(); i++) {
		if(rbdBindings.vfcuForcedUpdates[i].iType == FCU_EXPLICIT_CONFIRM) 
			sResult += FormatString("Forced update [explicit_confirm] on %s: %s", 
				rbdBindings.vfcuForcedUpdates[i].sConceptName.c_str(), 
				rbdBindings.vfcuForcedUpdates[i].bUnderstanding?
					"understanding":"non-understanding");
	}

	// finally, return
	return sResult;
}

// D: binds the concepts from the input parse into the agenda according to the
//    current interaction policy, then returns a charaterization of binding
//    success/failure in bhiResults
void CDMCoreAgent::bindConcepts(TBindingsDescr& rbdBindings) {
Log(DMCORE_STREAM, "Concepts Binding Phase initiated.");

	// initialize to zero the number of concepts bound and blocked
	rbdBindings.sEventType = pInteractionEventManager->GetLastEvent()->GetType();
	rbdBindings.iConceptsBlocked = 0;
    rbdBindings.iConceptsBound = 0;
	rbdBindings.iSlotsMatched = 0;
	rbdBindings.iSlotsBlocked = 0;

	// hash which stores the slots that matched and how many times they did
	// so
	map<string, int> msiSlotsMatched;

	// hash which stores the slots that were blocked and how many times they were
	// blocked
	map<string, int> msiSlotsBlocked;
    
	// go through each concept expectation level and try to bind things
	for(unsigned int iLevel = 0; 
		iLevel < eaAgenda.vCompiledExpectations.size(); 
		iLevel++) {

		// go through the hash of expected slots at that level
		TMapCE::iterator iPtr;
		for(iPtr = eaAgenda.vCompiledExpectations[iLevel].mapCE.begin();
			iPtr != eaAgenda.vCompiledExpectations[iLevel].mapCE.end();
			iPtr++) {
		
			string sSlotExpected = iPtr->first;	// the grammar slot expected
			TIntVector& rvIndices = iPtr->second;	// indices in the system 
													//   expectation list

			// if the slot actually exists in the parse, then try to bind it
			if(pInteractionEventManager->LastEventMatches(sSlotExpected)) {

				Log(DMCORE_STREAM, "Event matches %s.", sSlotExpected.c_str());

				// go through the array of indices and construct another array
				// which contains only the indices of "open" expectations, 
                // excluding any expectations that redundanly match to the same 
                // concept
				TIntVector vOpenIndices;
                set<CConcept *> scpOpenConcepts;

                // also construct another array which contains the indices of 
                // "closed" expectations, excluding any expectations that 
                // redundantly match the same concept
                TIntVector vClosedIndices;
                set<CConcept *> scpClosedConcepts;

                for(unsigned int i = 0; i < rvIndices.size(); i++) {
					// determine the concept under consideration
                    CConcept* pConcept = 
                        &(eaAgenda.celSystemExpectations[rvIndices[i]].pDialogAgent->C(
                            eaAgenda.celSystemExpectations[rvIndices[i]].sConceptName));
                    
                    // test that the expectation is not disabled
                    if(!eaAgenda.celSystemExpectations[rvIndices[i]].bDisabled) {
                        if(scpOpenConcepts.find(pConcept) == scpOpenConcepts.end()) {
	                        // add it to the open indices list
						    vOpenIndices.push_back(rvIndices[i]);
                            // add the concept to the open concepts list
                            scpOpenConcepts.insert(pConcept);
                            // if by any chance it's already in the closed concepts, 
                            set<CConcept *>::iterator iPtr;
                            if((iPtr = scpClosedConcepts.find(pConcept)) != 
                                scpClosedConcepts.end()) {
                                // remove it from there
                                scpClosedConcepts.erase(iPtr);
                            }
                        }
                    } else {
                        // o/w if the expectation is disabled
                        if((scpClosedConcepts.find(pConcept) == scpClosedConcepts.end()) &&
                           (scpOpenConcepts.find(pConcept) == scpOpenConcepts.end())) {
	                        // add it to the closed indices list
						    vClosedIndices.push_back(rvIndices[i]);
                            // add the concept to the closed concepts list
                            scpClosedConcepts.insert(pConcept);
                        }                        
                    }
				}

				// the slot value to be bound
				string sSlotValue;

				// and the confidence score
				float fConfidence = pInteractionEventManager->GetLastEventConfidence();

				if(vOpenIndices.size() > 0) {
                    // check that the confidence is strictly 
                    // above the current nonunderstanding threshold
                    if(fConfidence > fNonunderstandingThreshold) {
					    // check for multiple bindings on a level
					    if(vOpenIndices.size() > 1) {
						    // if there are multiple bindings possible, log that 
                            // as a warning for now *** later we need to deal with 
                            // this by adding disambiguation agencies
						    string sAgents;
						    for(unsigned int i=0; i < vOpenIndices.size(); i++) {
							    sAgents += 
								    eaAgenda.celSystemExpectations[vOpenIndices[i]].\
                                    pDialogAgent->GetName() + 
								    " tries to bind to " + 
								    eaAgenda.celSystemExpectations[vOpenIndices[i]].\
                                    sConceptName + 
								    "\n";
						    }
						    Warning(FormatString("Multiple binding for grammar "\
							    "concept %s. Agents dumped below. Binding "\
							    "performed just for the first agent.\n%s", 
							    sSlotExpected.c_str(), sAgents.c_str()));
					    }

					    // now bind the grammar concept to the first agent expecting 
					    // this slot; obtain the value for that grammar slot
					    sSlotValue = 
						    pInteractionEventManager->GetValueForExpectation(sSlotExpected);

					    // do the actual concept binding
					    performConceptBinding(sSlotExpected, sSlotValue, 
                            fConfidence, vOpenIndices[0], 
							pInteractionEventManager->LastEventIsComplete());

					    // now that we've bound at this level, invalidate this 
					    // expected slot on all the other levels
					    for(unsigned int iOtherLevel = iLevel + 1; 
						    iOtherLevel < eaAgenda.vCompiledExpectations.size(); 
						    iOtherLevel++) {					
						    eaAgenda.vCompiledExpectations[iOtherLevel].\
                                mapCE.erase(sSlotExpected);
					    }
                    } else {
                        // o/w the confidence is below the nonunderstanding
                        // threshold, so we will reject this utterance by
                        // basically not binding anything
                    }
				}

                // write the open binding description (only for the first concept, 
                // the one that actually bound)
                for(unsigned int i = 0; i < vOpenIndices.size(); i++) {
                    if(i == 0) {
                    // check that the confidence is strictly 
                    // above the current nonunderstanding threshold
                    if(fConfidence > fNonunderstandingThreshold) {
                            TBinding bBinding;
                            bBinding.bBlocked = false;
                            bBinding.iLevel = iLevel;
                            bBinding.fConfidence = fConfidence;
                            bBinding.sAgentName = 
                                eaAgenda.celSystemExpectations[vOpenIndices[i]].\
                                pDialogAgent->GetName();
                            bBinding.sConceptName = 
                                eaAgenda.celSystemExpectations[vOpenIndices[i]].\
                                sConceptName;
                            bBinding.sGrammarExpectation = 
                                eaAgenda.celSystemExpectations[vOpenIndices[i]].\
                                sGrammarExpectation;
                            bBinding.sValue = sSlotValue;
                            rbdBindings.vbBindings.push_back(bBinding);
                            rbdBindings.iConceptsBound++;
						    // add the slot to the list of matched slots 
						    msiSlotsMatched[bBinding.sGrammarExpectation] = 1;
						    // in case we find it in the blocked slots, (it could have gotten
						    // there on an earlier level) delete it from there
						    map<string, int>::iterator iPtr;
						    if((iPtr = msiSlotsBlocked.find(bBinding.sGrammarExpectation)) !=
							    msiSlotsBlocked.end()) {
							    msiSlotsBlocked.erase(iPtr);
						    }
                        } else {
                            // o/w if the confidence is not above the threshold
                            TBinding bBlockedBinding;
                            bBlockedBinding.bBlocked = true;
                            bBlockedBinding.iLevel = iLevel;
                            bBlockedBinding.fConfidence = fConfidence;
                            bBlockedBinding.sAgentName = 
                                eaAgenda.celSystemExpectations[vOpenIndices[i]].\
                                pDialogAgent->GetName();
                            bBlockedBinding.sConceptName = 
                                eaAgenda.celSystemExpectations[vOpenIndices[i]].\
                                sConceptName;
                            bBlockedBinding.sGrammarExpectation = 
                                eaAgenda.celSystemExpectations[vOpenIndices[i]].\
                                sGrammarExpectation;
                            bBlockedBinding.sReasonDisabled = 
                                "confidence below nonunderstanding threshold";
                            bBlockedBinding.sValue = sSlotValue;
                            rbdBindings.vbBindings.push_back(bBlockedBinding);
                            rbdBindings.iConceptsBlocked++;
						    // add the slot to the list of matched slots 
						    msiSlotsMatched[bBlockedBinding.sGrammarExpectation] = 1;
						    // in case we find it in the blocked slots, (it could have gotten
						    // there on an earlier level) delete it from there
						    map<string, int>::iterator iPtr;
						    if((iPtr = msiSlotsBlocked.find(bBlockedBinding.sGrammarExpectation)) !=
							    msiSlotsBlocked.end()) {
							    msiSlotsBlocked.erase(iPtr);
						    }
                        }
                    }
                }

                // write the blocked bindings description
                for(unsigned int i = 0; i < vClosedIndices.size(); i++) {
                    TBinding bBlockedBinding;
                    bBlockedBinding.bBlocked = true;
                    bBlockedBinding.iLevel = iLevel;
                    bBlockedBinding.fConfidence = fConfidence;
                    bBlockedBinding.sAgentName = 
                        eaAgenda.celSystemExpectations[vClosedIndices[i]].\
                        pDialogAgent->GetName();
                    bBlockedBinding.sConceptName = 
                        eaAgenda.celSystemExpectations[vClosedIndices[i]].\
                        sConceptName;
                    bBlockedBinding.sGrammarExpectation = 
                        eaAgenda.celSystemExpectations[vClosedIndices[i]].\
                        sGrammarExpectation;
                    bBlockedBinding.sReasonDisabled = 
                        eaAgenda.celSystemExpectations[vClosedIndices[i]].\
                        sReasonDisabled;
                    bBlockedBinding.sValue = sSlotValue;
                    rbdBindings.vbBindings.push_back(bBlockedBinding);
                    rbdBindings.iConceptsBlocked++;
					// add it to the list of blocked slots, if it's not already
					// in the one of matched slots
					if(msiSlotsMatched.find(bBlockedBinding.sGrammarExpectation) ==
						msiSlotsMatched.end()) {
						msiSlotsBlocked[bBlockedBinding.sGrammarExpectation] = 1;
					}
                }
			}
		}
	}

    // for user inputs, update the non-understanding flag
	if (pInteractionEventManager->GetLastEvent()->GetType() == IET_USER_UTT_END ||
		pInteractionEventManager->GetLastEvent()->GetType() == IET_GUI) {
		rbdBindings.bNonUnderstanding = (rbdBindings.iConceptsBound == 0);
	} else {
		rbdBindings.bNonUnderstanding = false;
	}

	// update the slots matched and blocked information
	rbdBindings.iSlotsMatched = msiSlotsMatched.size();
	rbdBindings.iSlotsBlocked = msiSlotsBlocked.size();

    // finally, for user inputs, check if the statistics match what helios 
	// predicted would happen (the helios binding features)
	if (pInteractionEventManager->GetLastEvent()->GetType() == IET_USER_UTT_END) {
		if((pInteractionEventManager->LastEventMatches("[slots_blocked]")) &&
			(pInteractionEventManager->LastEventMatches("[slots_matched]"))) {
			bool bHeliosMatch = true;
			string sH4SlotsBlocked = 
				pInteractionEventManager->GetValueForExpectation("[slots_blocked]");
			string sH4SlotsMatched = 
				pInteractionEventManager->GetValueForExpectation("[slots_matched]");
			if((sH4SlotsBlocked != "N/A") && 
				(atoi(sH4SlotsBlocked.c_str()) != rbdBindings.iSlotsBlocked))
				bHeliosMatch = false;
			if((sH4SlotsMatched != "N/A") && 
				(atoi(sH4SlotsMatched.c_str()) != rbdBindings.iSlotsMatched))
				bHeliosMatch = false;
			if(!bHeliosMatch) {
				Warning("Helios binding features are different from RavenClaw "\
					"obtained values.");
			}
		}
		// finally, perform the forced concept updates
	    performForcedConceptUpdates(rbdBindings);
	} else if (pInteractionEventManager->GetLastEvent()->GetType() == IET_GUI) {
		performForcedConceptUpdates(rbdBindings);
	}

	// and finally log the attempted bindings
	Log(DMCORE_STREAM, "Attempted bindings dumped below:\n%s", 
		bindingsDescrToString(rbdBindings).c_str());						

    // and the general statistics
	Log(DMCORE_STREAM, "Concepts Binding Phase completed (%d concept(s) "\
					   "bound, %d concept(s) blocked out).", 
					   rbdBindings.iConceptsBound, 
                       rbdBindings.iConceptsBlocked);
}

// D: Perform the concept binding
void CDMCoreAgent::performConceptBinding(string sSlotName, string sSlotValue,
    float fConfidence, int iExpectationIndex, bool bIsComplete) {
	
    // obtain a reference to the expectation structure
	TConceptExpectation& ceExpectation = 
		eaAgenda.celSystemExpectations[iExpectationIndex];

    // compute the value we need to bind to that concept
    string sValueToBind = ""; 
	if(ceExpectation.bmBindMethod == bmSlotValue) {
        // bind the slot value
        sValueToBind = sSlotValue;
    } else if(ceExpectation.bmBindMethod == bmExplicitValue) {
	    // bind the explicit value
        sValueToBind = ceExpectation.sExplicitValue;
    } else {
        // bind through a binding function
        STRING2BFF::iterator iPtr;
        if((iPtr = s2bffFilters.find(ceExpectation.sBindingFilterName)) == 
            s2bffFilters.end()) {
            FatalError(FormatString("Could not find binding filter :%s for "\
                "expectation %s generated by agent %s.", 
                ceExpectation.sBindingFilterName.c_str(),
                ceExpectation.sGrammarExpectation.c_str(),
                ceExpectation.pDialogAgent->GetName().c_str()));                
        }
        // if the binding filter was found, call it
        sValueToBind = (*(iPtr->second))(sSlotName, sSlotValue);
    }

    // reset the confidence to 1, if ALWAYS_CONFIDENT is defined
/*    #ifdef ALWAYS_CONFIDENT*/
    //fConfidence = 1.0;
    //#endif

    string sBindingString = 
        FormatString("%s%s%f", sValueToBind.c_str(), VAL_CONF_SEPARATOR, 
                     fConfidence);

    // now bind that particular value/confidence

	if (bIsComplete) {
		// first, create a temporary concept for that
    //cout << "+++++++++开始打印DMCoreAgent.performForcedConceptUpdates()中的update信息"<<endl;
		CConcept *pTempConcept = 
			ceExpectation.pDialogAgent->C(ceExpectation.sConceptName).EmptyClone();
		// assign it from the string
    //cout << "+++++++++pTempConcept名的名字为:"<<pTempConcept->GetName()<<endl;
    pTempConcept->Update(CU_ASSIGN_FROM_STRING, &sBindingString);
    //cout << "+++++++++vhCurrentHypset大小为:"<<pTempConcept->GetNumHyps()<<endl;
    //cout << "+++++++++更新情况为:"<<pTempConcept->IsUpdated()<<endl;
		CConcept &c = ceExpectation.pDialogAgent->C(ceExpectation.sConceptName);

    //cout << "+++++++++Concept的名字为:"<<c.GetName()<<endl;
		// first if the concept has an undergoing grounding request, remove it
	//	if (c.IsUndergoingGrounding())
			//pGroundingManager->RemoveConceptGroundingRequest(&c);

		// now call the binding method 
		c.Update(CU_UPDATE_WITH_CONCEPT, pTempConcept);

    //cout << "+++++++++vhCurrentHypSet的大小为:"<<c.GetNumHyps()<<endl;
		// first if the concept has an undergoing grounding request, remove it
		// finally, deallocate the temporary concept
		delete pTempConcept;
	} else {
		// perform a partial (temporary) binding
		ceExpectation.pDialogAgent->C(ceExpectation.sConceptName).\
			Update(CU_PARTIAL_FROM_STRING, &sBindingString);

	}

    // log it
    Log(DMCORE_STREAM, "Slot %s(%s) bound to concept (%s)%s.", 
   	    sSlotName.c_str(), sBindingString.c_str(), 
	    ceExpectation.pDialogAgent->GetName().c_str(), 
	    ceExpectation.sConceptName.c_str());
}


// D: processes non-understandings
void CDMCoreAgent::processNonUnderstanding() {
    Log(DMCORE_STREAM, "Process Non-Understanding Phase initiated.");

    // sets meta information on the input, specifying it's a non-understanding
    Log(DMCORE_STREAM, "Non-understanding %s detected.", NON_UNDERSTANDING);
    pInteractionEventManager->GetLastInput()->SetProperty("["NON_UNDERSTANDING"]", "true");

    Log(DMCORE_STREAM, "Process Non-Understanding Phase completed.");
}

// D: performs the forced updates for the concepts
void CDMCoreAgent::performForcedConceptUpdates(TBindingsDescr& rbdBindings) {

	Log(DMCORE_STREAM, "Performing forced concept updates ...");

	// now go through the concepts that are waiting to be explicitly 
	// confirmed and perform the corresponding updates on them
	set<CConcept *>::iterator iPtr;
	for(iPtr = saSystemAction.setcpExplicitConfirms.begin(); 
		iPtr != saSystemAction.setcpExplicitConfirms.end(); 
		iPtr++) {
		// check that the concept is still sealed (no update was performed 
		// on it yet)
		if((*iPtr)->IsSealed()) {
			
			// then perform an forced update			
			TForcedConceptUpdate fcu;
			fcu.sConceptName = (*iPtr)->GetName();
			fcu.iType = FCU_EXPLICIT_CONFIRM;
			fcu.bUnderstanding = false;
			CHyp *phOldTopHyp = (*iPtr)->GetTopHyp();

			// log the update
			Log(DMCORE_STREAM, "Performing forced concept update on %s ...", 
				(*iPtr)->GetName().c_str());
			Log(DMCORE_STREAM, "Concept grounding status: %d", 
				pGroundingManager->GetConceptGroundingRequestStatus((*iPtr)));
			(*iPtr)->Update(CU_UPDATE_WITH_CONCEPT, NULL);

			Log(DMCORE_STREAM, "Concept grounding status: %d", 
				pGroundingManager->GetConceptGroundingRequestStatus((*iPtr)));
			// now, if this update has desealed the concept, then we need
			// to run grounding on this concept 
			if(!((*iPtr)->IsSealed()) && 
				(pGroundingManager->GetConceptGroundingRequestStatus(*iPtr) != GRS_EXECUTING)) {
				// now schedule grounding on this concept
				string sAction = pGroundingManager->ScheduleConceptGrounding(*iPtr);
				// if the action scheduled is still explicit confirm
				if(sAction != "EXPL_CONF") {
					// then mark that we have an understanding
					fcu.bUnderstanding = true;
					rbdBindings.bNonUnderstanding = false;
				} else if ((*iPtr)->GetTopHyp() == phOldTopHyp) {
					// if we are still on an explicit confirm on the same hypothesis, 
					// seal it back
					(*iPtr)->Seal();
				}
			}

			// finally, push this into the bindings
			rbdBindings.vfcuForcedUpdates.push_back(fcu);
		}
	}

	// now go through the concepts that are waiting to be implicitly 
	// confirmed and perform the corresponding updates on them
	for(iPtr = saSystemAction.setcpImplicitConfirms.begin(); 
		iPtr != saSystemAction.setcpImplicitConfirms.end(); 
		iPtr++) {
		// check that the concept is still sealed (no update was performed 
		// on it yet)
		if((*iPtr)->IsSealed()) {

			// then perform an forced update			
			TForcedConceptUpdate fcu;
			fcu.sConceptName = (*iPtr)->GetName();
			fcu.iType = FCU_IMPLICIT_CONFIRM;
			fcu.bUnderstanding = false;

			// log the update
			Log(DMCORE_STREAM, FormatString(
				"Performing forced concept update on %s ...", 
				(*iPtr)->GetName().c_str()));
			(*iPtr)->Update(CU_UPDATE_WITH_CONCEPT, NULL);

			// now, if this update has desealed the concept, then we need
			// to run grounding on this concept 
			if(!((*iPtr)->IsSealed()) && 
				(pGroundingManager->GetConceptGroundingRequestStatus(*iPtr) != GRS_EXECUTING)) {
				// first check that it was not already scheduled
				if(pGroundingManager->GetScheduledGroundingActionOnConcept(*iPtr) == "")				    
                    // if not scheduled already, schedule it now				
				    pGroundingManager->ScheduleConceptGrounding(*iPtr);
				// then mark that we have an understanding
				fcu.bUnderstanding = true;
				rbdBindings.bNonUnderstanding = false;
			}

			// finally, push this into the bindings
			rbdBindings.vfcuForcedUpdates.push_back(fcu);
		}
	}

	// finally, go through the concepts that have unplanned implicit confirmations
	// on them and perform the corresponding updates on them
	for(iPtr = saSystemAction.setcpUnplannedImplicitConfirms.begin(); 
		iPtr != saSystemAction.setcpUnplannedImplicitConfirms.end(); 
		iPtr++) {
		// check that the concept is still sealed (no update was performed 
		// on it yet)
		if((*iPtr)->IsSealed()) {

			// then perform an forced update			
			TForcedConceptUpdate fcu;
			fcu.sConceptName = (*iPtr)->GetName();
			fcu.iType = FCU_UNPLANNED_IMPLICIT_CONFIRM;
			fcu.bUnderstanding = false;

			// log the update
			Log(DMCORE_STREAM, FormatString(
				"Performing forced concept update on %s ...", 
				(*iPtr)->GetName().c_str()));
			(*iPtr)->Update(CU_UPDATE_WITH_CONCEPT, NULL);

			// now, if this update has desealed the concept, then we need
			// to run grounding on this concept 
			if(!((*iPtr)->IsSealed())) {
				// first check that it was not already scheduled
				if(pGroundingManager->GetScheduledGroundingActionOnConcept(*iPtr) == "")				    
                    // if not scheduled already, schedule it now				
				    pGroundingManager->ScheduleConceptGrounding(*iPtr);
				// then mark that we have an understanding
				fcu.bUnderstanding = true;
				rbdBindings.bNonUnderstanding = false;
			}

			// finally, push this into the bindings
			rbdBindings.vfcuForcedUpdates.push_back(fcu);
		}
	}

	// log completion of this phase
	Log(DMCORE_STREAM, "Forced concept updates completed.");
}


//-----------------------------------------------------------------------------
// D: Focus Claims functions
//-----------------------------------------------------------------------------
// D: assembles a list of focus claims, and returns the size of that list
int CDMCoreAgent::assembleFocusClaims() {
	Log(DMCORE_STREAM, "Focus Claims Assembly Phase initiated.");
	
	// gather the focus claims, starting with the root of the dialog task tree
	int iClaims = 0;
    TFocusClaimsList fclTempFocusClaims;
	iClaims = 
		pDTTManager->GetDialogTaskTreeRoot()->DeclareFocusClaims(
            fclTempFocusClaims);

    // log the list of claiming agents
    string sLogString;
    if(fclTempFocusClaims.size() == 0) 
        sLogString = "0 agent(s) claiming focus.";
    else
        sLogString = FormatString("%d agent(s) claiming focus (dumped below):\n", 
            fclTempFocusClaims.size());
    for(unsigned int i = 0; i < fclTempFocusClaims.size(); i++) 
        sLogString += FormatString("  %s\n", 
            fclTempFocusClaims[i].sAgentName.c_str());
    Log(DMCORE_STREAM, sLogString.c_str());

    // now prune the claims of agents that have their completion criteria
    // satisfied
    Log(DMCORE_STREAM, "Pruning Focus Claims list.");
    // check if we undergoing some grounding action
    bool bDuringGrounding = 
        pGroundingManager->HasScheduledConceptGroundingRequests()/* ||
        pGroundingManager->HasExecutingConceptGroundingRequests()*/;
    int iClaimsEliminated = 0;
    fclFocusClaims.clear();
    sLogString = "";
    for(unsigned int i = 0; i < fclTempFocusClaims.size(); i++) {
        CDialogAgent* pdaFocusClaimingAgent =
            (CDialogAgent *)AgentsRegistry[fclTempFocusClaims[i].sAgentName];
        if(pdaFocusClaimingAgent->SuccessCriteriaSatisfied() || 
           pdaFocusClaimingAgent->FailureCriteriaSatisfied() ||
           AgentIsActive(pdaFocusClaimingAgent) ||
           (!fclTempFocusClaims[i].bClaimDuringGrounding && bDuringGrounding)) {
            // eliminate the agent from the list of agents claiming focus
            // if they already have the success criteria satisfied or if
            // they are already in focus, or if they cannot trigger during
            // grounding
            iClaimsEliminated++;
            // and mark it in the log string
            sLogString += FormatString("  %s\n", 
                fclTempFocusClaims[i].sAgentName.c_str());
        } else {
            // o/w add it to the real list of claims
            fclFocusClaims.push_back(fclTempFocusClaims[i]);
        }
    }
    // finally, add the prefix for the log string of eliminated agents
    if(iClaimsEliminated == 0) 
        sLogString = "0 agents(s) eliminated from the focus claims list.";
    else 
        sLogString = 
            FormatString("%d agent(s) eliminated from the focus claims list "\
            "(dumped below):\n%s", iClaimsEliminated, sLogString.c_str());

    Log(DMCORE_STREAM, sLogString.c_str());

	Log(DMCORE_STREAM, "Focus Claims Assembly Phase completed "\
					   "(%d claim(s)).", iClaims);
	return iClaims;
}

// D: resolves focus shifts
void CDMCoreAgent::resolveFocusShift() {
	
	// send out a warning if we have a multiple focus shift
	if(fclFocusClaims.size() > 1) {
		string sMessage = "Ambiguous focus shift (claiming agents dump below).\n";
		for(unsigned int i = 0; i < fclFocusClaims.size(); i++) 
			sMessage += fclFocusClaims[i].sAgentName + "\n";
		Warning(sMessage.c_str());
	};

	// put the agents on the stack 
    for(unsigned int i = 0; i < fclFocusClaims.size(); i++) {
	    string sClaimingAgent = fclFocusClaims[i].sAgentName;
	    Log(DMCORE_STREAM, 
            "Adding focus-claiming agent %s on the execution stack.", 
		    sClaimingAgent.c_str());
	    ContinueWith(this, (CDialogAgent *)AgentsRegistry[sClaimingAgent]);
    }
}

//-----------------------------------------------------------------------------
//
// TIMEOUT RELATED METHODS
//
//-----------------------------------------------------------------------------
// D: sets the timeout period
void CDMCoreAgent::SetTimeoutPeriod(int iATimeoutPeriod) {
	iTimeoutPeriod = iATimeoutPeriod;
	//DMI_SetTimeoutPeriod(iTimeoutPeriod);
}

// D: returns the current timeout period
int CDMCoreAgent::GetTimeoutPeriod() {
	return iTimeoutPeriod;
}

// D: sets the default timeout period
void CDMCoreAgent::SetDefaultTimeoutPeriod(int iADefaultTimeoutPeriod) {
	iDefaultTimeoutPeriod = iADefaultTimeoutPeriod;
}

// D: returns the current timeout period
int CDMCoreAgent::GetDefaultTimeoutPeriod() {
	return iDefaultTimeoutPeriod;
}

//-----------------------------------------------------------------------------
//
// METHODS FOR ACCESSING THE NONUNDERSTANDING THRESHOLD
//
//-----------------------------------------------------------------------------

// D: sets the nonunderstanding threshold
void CDMCoreAgent::SetNonunderstandingThreshold(float fANonunderstandingThreshold) {
    fNonunderstandingThreshold = fANonunderstandingThreshold;
} 

// D: gets the nonunderstanding threshold
float CDMCoreAgent::GetNonunderstandingThreshold() {
    return fNonunderstandingThreshold;
}

// D: sets the default nonunderstanding threshold
void CDMCoreAgent::SetDefaultNonunderstandingThreshold(float fANonuThreshold) {
    fDefaultNonunderstandingThreshold = fANonuThreshold;
} 

// D: returns the default nonunderstanding threshold
float CDMCoreAgent::GetDefaultNonunderstandingThreshold() {
    return fDefaultNonunderstandingThreshold;
}

//---------------------------------------------------------------------
// METHODS FOR SIGNALING FLOOR CHANGES
//---------------------------------------------------------------------

void CDMCoreAgent::SetFloorStatus(TFloorStatus fsaFloorStatus) {
	Log(DMCORE_STREAM, "Set floor status to %s", vsFloorStatusLabels[fsaFloorStatus].c_str());
	fsFloorStatus = fsaFloorStatus;
}

void CDMCoreAgent::SetFloorStatus(string sAFloorStatus) {
	SetFloorStatus(StringToFloorStatus(sAFloorStatus));
}

TFloorStatus CDMCoreAgent::GetFloorStatus() {
	return fsFloorStatus;
}

string CDMCoreAgent::FloorStatusToString(TFloorStatus fsAFloor) {
	return vsFloorStatusLabels[fsAFloor];
}

TFloorStatus CDMCoreAgent::StringToFloorStatus(string sAFloor) {
	for (unsigned int i=0; i < vsFloorStatusLabels.size(); i++) {
		if (vsFloorStatusLabels[i] == sAFloor)
			return (TFloorStatus)i;
	}
	return fsUnknown;
}


//-----------------------------------------------------------------------------
//
// METHOD FOR SIGNALING THE NEED FOR A FOCUS CLAIMS PHASE
//
//-----------------------------------------------------------------------------

// D: signal a focus claims phase - set a flag so that the core agent will run
//    a focus claims phase the next chance it gets
void CDMCoreAgent::SignalFocusClaimsPhase(bool bAFocusClaimsPhaseFlag) {
    bFocusClaimsPhaseFlag = bAFocusClaimsPhaseFlag;
}

//-----------------------------------------------------------------------------
//
// ACCESS TO VARIOUS PRIVATE FIELDS (mostly state information for the 
// State Manager Agent)
//
//-----------------------------------------------------------------------------

// D: returns the number of concepts bound in the last input pass
int CDMCoreAgent::LastTurnGetConceptsBound() {
    if(bhBindingHistory.size() == 0) 
        return -1;
    else 
        return bhBindingHistory.back().iConceptsBound;
}

// D: returns true if the last turn was a non-understanding
bool CDMCoreAgent::LastTurnNonUnderstanding() {
	for (int i = bhBindingHistory.size()-1; i >= 0; i--) {
		if (bhBindingHistory[i].sEventType == IET_USER_UTT_END ||
            bhBindingHistory[i].sEventType == IET_GUI) {
			if (bhBindingHistory[i].bNonUnderstanding)
				return true;
			else
				return false;
		}
	}
    return false;
}

// A: returns the number of consecutive non-understandings so far
int CDMCoreAgent::GetNumberNonUnderstandings() {
	int iNumNonunderstandings = 0;
	for (int i = bhBindingHistory.size()-1; i >= 0; i--) {
		if (bhBindingHistory[i].sEventType == IET_USER_UTT_END ||
            bhBindingHistory[i].sEventType == IET_GUI) {
			if (bhBindingHistory[i].bNonUnderstanding) {
				iNumNonunderstandings++;
			}
			else {
				break;
			}
		}
	}
	return iNumNonunderstandings;
}

// A: returns the total number of non-understandings in the current dialog
//    so far
int CDMCoreAgent::GetTotalNumberNonUnderstandings() {
	int iNumNonunderstandings = 0;
	for (int i = 0; i < (int)bhBindingHistory.size(); i++) {
		if (bhBindingHistory[i].bNonUnderstanding) {
			iNumNonunderstandings++;
		}
	}
	return iNumNonunderstandings;
}

//-----------------------------------------------------------------------------
//
// EXECUTION STACK AND HISTORY METHODS
//
//-----------------------------------------------------------------------------

// D: Pushes a new dialog agent on the execution stack 
void CDMCoreAgent::ContinueWith(CAgent* paPusher, CDialogAgent* pdaDialogAgent) {

    // check that the agent is not already on top of the stack
    if(!esExecutionStack.empty() && 
        (esExecutionStack.begin()->pdaAgent == pdaDialogAgent)) {
        Log(DMCORE_STREAM, "Agent %s already on top of the execution stack. "
            "ContinueWith request ignored.", pdaDialogAgent->GetName().c_str());
        return;
    }

	// add an entry in the history; fill in all the slots
	TExecutionHistoryItem ehi;
	ehi.sCurrentAgent = pdaDialogAgent->GetName();
	ehi.sCurrentAgentType = pdaDialogAgent->GetType();
	ehi.bScheduled = true;
	ehi.sScheduledBy = paPusher->GetName();
  //ehi.timeScheduled = GetTime();
	ehi.bExecuted = false;
	ehi.bCommitted = false;
	ehi.bCanceled = false;
	ehi.iStateHistoryIndex = -1;
	// ***ehi.timeTerminated = 0;
	ehExecutionHistory.push_back(ehi);
	
	// and put it on the stack
	TExecutionStackItem esi;
	esi.pdaAgent = pdaDialogAgent;
	esi.iEHIndex = ehExecutionHistory.size() - 1;
	esExecutionStack.push_front(esi);

	// stores the execution index in the agent
	pdaDialogAgent->SetLastExecutionIndex(esi.iEHIndex);

	// signals that the agenda needs to be recomputed
	bAgendaModifiedFlag = true;

    //Log(DMCORE_STREAM, "Agent %s added on the execution stack by %s.", 
        //ehi.sCurrentAgent.c_str(), ehi.sScheduledBy.c_str());
}

// D: Restarts a topic
void CDMCoreAgent::RestartTopic(CDialogAgent* pdaDialogAgent) {
	// first, locate the agent
	TExecutionStack::iterator iPtr;
	for(iPtr = esExecutionStack.begin(); 
		iPtr != esExecutionStack.end(); 
		iPtr++) {
		if(iPtr->pdaAgent == pdaDialogAgent) break;
	}

	// if the agent was nothere in the list, trigger a fatal error
	if(iPtr == esExecutionStack.end()) {
		Warning("Cannot restart the " + pdaDialogAgent->GetName() +
		        " agent. Agent not found on execution stack.");
		return;
	}

    Log(DMCORE_STREAM, "Restarting agent %s.", pdaDialogAgent->GetName().c_str());

    // store the planner of this agent
    CDialogAgent* pdaScheduler = (CDialogAgent *)
	    AgentsRegistry[ehExecutionHistory[iPtr->iEHIndex].sScheduledBy];

    // now clean it off the execution stack
    PopTopicFromExecutionStack(pdaDialogAgent);

    // reopen it
    pdaDialogAgent->ReOpen();

    // and readd it to the stack
    ContinueWith(pdaScheduler, pdaDialogAgent);
}

// D: Registers a custom start over function 
void CDMCoreAgent::RegisterCustomStartOver(TCustomStartOverFunct csoAStartOverFunct) {
    csoStartOverFunct = csoAStartOverFunct;
}

// D: Restarts the dialog 
void CDMCoreAgent::StartOver() {
    if(csoStartOverFunct == NULL) {
        // restart the dialog clear the execution stack
        esExecutionStack.clear();
        // destroy the dialog task tree
        pDTTManager->DestroyDialogTree();
        // recreate the dialog task tree
        pDTTManager->ReCreateDialogTree();
        // restart the execution by putting the root on the stack
        Log(DMCORE_STREAM, "Restarting Dialog Task execution.");
	    ContinueWith(this, pDTTManager->GetDialogTaskTreeRoot());

		pStateManager->UpdateState();
	} else {
        (*csoStartOverFunct)();
    }
}

// D: Pops all the completed agents (and all the agents they have ever planned
//	  for off the execution stack
int CDMCoreAgent::popCompletedFromExecutionStack() {
	TExecutionStack::iterator iPtr;
	bool bFoundCompleted;	// indicates if completed agents were still found
	
    TStringVector vsAgentsEliminated;
    // when no more completed agents can be found, return
	do {
		bFoundCompleted = false;
		// go through the execution stack
		for(iPtr = esExecutionStack.begin(); 
			iPtr != esExecutionStack.end(); 
			iPtr++) {

			// if you find an agent that has completed
			if(iPtr->pdaAgent->HasCompleted()) {
				// pop it off the execution stack
				popTopicFromExecutionStack(iPtr->pdaAgent, vsAgentsEliminated);
				bFoundCompleted = true;
				break;
			}
		}
	} while(bFoundCompleted);

    // when no more completed agents can be found, log and return
    if(vsAgentsEliminated.size() != 0) {
        string sAgents;
        for(unsigned int i = 0; i < vsAgentsEliminated.size(); i++)
            sAgents += FormatString("%s\n", vsAgentsEliminated[i].c_str());

        sAgents = TrimRight(sAgents);

        Log(DMCORE_STREAM, "Eliminated %d completed agent(s) (dumped below) "
            "from the execution stack.\n%s", 
            vsAgentsEliminated.size(), sAgents.c_str());
    }

    // return the number of agents eliminated
    return vsAgentsEliminated.size();
}

// D: Pops a dialog agent from the execution stack
void CDMCoreAgent::popAgentFromExecutionStack(CDialogAgent* pdaADialogAgent,
    TStringVector& rvsAgentsEliminated) {
	// check for empty stack condition
	if(esExecutionStack.empty()) {
		FatalError("Cannot pop the " + pdaADialogAgent->GetName() + 
				   " agent off the execution stack. Stack is empty.");
	}

	// first, locate the agent
	TExecutionStack::iterator iPtr;
	for(iPtr = esExecutionStack.begin(); 
		iPtr != esExecutionStack.end(); 
		iPtr++) {
		if(iPtr->pdaAgent == pdaADialogAgent) break;
	}

	// if the agent was nothere in the list, trigger a fatal error
	if(iPtr == esExecutionStack.end()) {
		FatalError("Cannot pop the " + pdaADialogAgent->GetName() +
				   " agent off the execution stack. Agent not found.");
	}

	// mark the time this agent's execution was terminated
	//ehExecutionHistory[iPtr->iEHIndex].timeTerminated = GetTime();

	// call the agent's OnCompletion method
	iPtr->pdaAgent->OnCompletion();

    // and add it to the list of eliminated agents
    rvsAgentsEliminated.push_back(iPtr->pdaAgent->GetName());

    // eliminate the agent from the stack
	esExecutionStack.erase(iPtr);

	// signals that the agenda needs to be recompiled
	bAgendaModifiedFlag = true;
}

// D: Pops a dialog agent from the execution stack, together with all the 
//    other agents it has ever planned for execution
void CDMCoreAgent::popTopicFromExecutionStack(CDialogAgent* pdaADialogAgent,
                                         TStringVector& rvsAgentsEliminated) {
	// check for empty stack condition
	if(esExecutionStack.empty()) {
		FatalError("Cannot pop the " + pdaADialogAgent->GetName() + 
				   " agent off the execution stack. Stack is empty.");
	}

	// first, locate the agent
	TExecutionStack::iterator iPtr;
	for(iPtr = esExecutionStack.begin(); 
		iPtr != esExecutionStack.end(); 
		iPtr++) {
		if(iPtr->pdaAgent == pdaADialogAgent) break;
	}

	// if the agent was nothere in the list, trigger a fatal error
	if(iPtr == esExecutionStack.end()) {
		FatalError("Cannot pop the " + pdaADialogAgent->GetName() +
				   " agent off the execution stack. Agent not found.");
	}

	// the set of eliminated agents
	set<CDialogAgent*, less<CDialogAgent*> > sEliminatedAgents;
	
	// initialize it with the starting agent
	sEliminatedAgents.insert(iPtr->pdaAgent);

	// mark the time this agent's execution was terminated
	//ehExecutionHistory[iPtr->iEHIndex].timeTerminated = GetTime();

	// call the agent's OnCompletion method
	iPtr->pdaAgent->OnCompletion();

    // and add it to the list of eliminated agents
    rvsAgentsEliminated.push_back(iPtr->pdaAgent->GetName());

    // eliminate the agent from the stack
	esExecutionStack.erase(iPtr);

    // now enter in a loop going through the stack repeatedly until 
    // we didn't find anything else to remove
    bool bFoundAgentToRemove = true;

    while(bFoundAgentToRemove) {

        bFoundAgentToRemove = false;

        // now traverse the stack
        for(iPtr = esExecutionStack.begin(); 
            iPtr != esExecutionStack.end(); 
            iPtr++) {

		    // check to see who planned the current agent
		    CDialogAgent* pdaScheduler = (CDialogAgent *)
			    AgentsRegistry[ehExecutionHistory[iPtr->iEHIndex].sScheduledBy];
		    if(sEliminatedAgents.find(pdaScheduler) != sEliminatedAgents.end()) {
			    // then we need to eliminate this one; so first add it to the 
			    // list of eliminated agents
			    sEliminatedAgents.insert(iPtr->pdaAgent);
			    // mark the time this agent execution was terminated
					//ehExecutionHistory[iPtr->iEHIndex].timeTerminated = GetTime();
			    // call the agent's OnCompletion method
			    iPtr->pdaAgent->OnCompletion();
                // and add it to the list of eliminated agents
                rvsAgentsEliminated.push_back(iPtr->pdaAgent->GetName());
			    // eliminate the agent from the stack
			    esExecutionStack.erase(iPtr);
                // set found one to true
                bFoundAgentToRemove = true; 
                // and break the for loop
                break;
		    } 
        }
	}

	bAgendaModifiedFlag = true;
}

// A: Pops all grounding agents from the execution stack
void CDMCoreAgent::popGroundingAgentsFromExecutionStack(TStringVector& rvsAgentsEliminated) {
	// check for empty stack condition
	if(esExecutionStack.empty()) {
		Log(DMCORE_STREAM, "Execution stack empty: no grounding agent popped");
		return;
	}

	// the set of eliminated agents
	set<CAgent*, less<CAgent*> > sEliminatedAgents;
	
	// initialize it with the grounding manager agent since it
	// schedules all root grounding agents
	sEliminatedAgents.insert(pGroundingManager);

    // now enter in a loop going through the stack repeatedly until 
    // we didn't find anything else to remove
    bool bFoundAgentToRemove = true;

    while(bFoundAgentToRemove) {

        bFoundAgentToRemove = false;

        // now traverse the stack
        for(TExecutionStack::iterator iPtr = esExecutionStack.begin(); 
            iPtr != esExecutionStack.end(); 
            iPtr++) {

		    // check to see who planned the current agent
		    CDialogAgent* pdaScheduler = (CDialogAgent *)
			    AgentsRegistry[ehExecutionHistory[iPtr->iEHIndex].sScheduledBy];
		    if(sEliminatedAgents.find(pdaScheduler) != sEliminatedAgents.end()) {
			    // then we need to eliminate this one; so first add it to the 
			    // list of eliminated agents
			    sEliminatedAgents.insert(iPtr->pdaAgent);
			    // mark the time this agent execution was terminated
					//ehExecutionHistory[iPtr->iEHIndex].timeTerminated = GetTime();
			    // call the agent's OnCompletion method
			    iPtr->pdaAgent->OnCompletion();
                // and add it to the list of eliminated agents
                rvsAgentsEliminated.push_back(iPtr->pdaAgent->GetName());
			    // eliminate the agent from the stack
			    esExecutionStack.erase(iPtr);
                // set found one to true
                bFoundAgentToRemove = true; 
                // and break the for loop
                break;
		    } 
        }
	}

	bAgendaModifiedFlag = true;
}

// A: Rolls back to a previous dialog state (e.g. after a user barge-in)
void CDMCoreAgent::rollBackDialogState(int iState) {

	int iLastEHIndex = (*pStateManager)[iState].iEHIndex;

	for (int i = ehExecutionHistory.size()-1; i > iLastEHIndex; i--) {
		TExecutionHistoryItem ehi = ehExecutionHistory[i];
		// if the agents were indeed executed and not yet canceled
		if (ehi.bExecuted && !ehi.bCanceled) {
			// Undo the execution of the agent
			CDialogAgent *pdaAgent = (CDialogAgent*)
				AgentsRegistry[ehi.sCurrentAgent];
			pdaAgent->Undo();

			// Mark the execution as canceled
			ehi.bCanceled = true;

			Log(DMCORE_STREAM, "Canceled execution of agent %s (state=%d,"
				"iEHIndex=%d).", pdaAgent->GetName().c_str(), 
				iState, i);
		}
	}

	// Now updates the execution stack and the agenda
	TDialogState dsCurrentState = (*pStateManager)[iState];
	fsFloorStatus = dsCurrentState.fsFloorStatus;
	esExecutionStack = dsCurrentState.esExecutionStack;
	eaAgenda = dsCurrentState.eaAgenda;
	saSystemAction = dsCurrentState.saSystemAction;
	// There is no need to recompile the agenda
	bAgendaModifiedFlag = false;

	// And updates the current state
	pStateManager->UpdateState();

	// Log the rollback
	Log(DMCORE_STREAM, "Rolled back to state %d. Current agenda dumped"
		" below\n%s", iState, expectationAgendaToString().c_str());

}

// D: Returns the size of the binding history
int CDMCoreAgent::GetBindingHistorySize() {
    return bhBindingHistory.size();
}

// D: Returns a pointer to the binding result from history
const TBindingsDescr& CDMCoreAgent::GetBindingResult(int iBindingHistoryIndex) {
    // check that the index is within bounds
    if((iBindingHistoryIndex >= 0) || 
        (iBindingHistoryIndex < -(int)bhBindingHistory.size())) {
        FatalError("Out of bounds index on access to binding history.");
        return bhBindingHistory[0];   
    } else {
        return bhBindingHistory[bhBindingHistory.size() + iBindingHistoryIndex];
    }
}

// D: Returns a description of the system action taken on a particular concept
//    (***** this function will need to be elaborated more *****)
TSystemActionOnConcept CDMCoreAgent::GetSystemActionOnConcept(CConcept* pConcept) {
	TSystemActionOnConcept saoc;

	// check if the concept is among the requested concepts
	if(saSystemAction.setcpRequests.find(pConcept) != 
		saSystemAction.setcpRequests.end()) {
		// then we have a request
		saoc.sSystemAction = SA_REQUEST;
	}

	// check if the concept is among the explicitly confirmed concepts
	else if(saSystemAction.setcpExplicitConfirms.find(pConcept) != 
		saSystemAction.setcpExplicitConfirms.end()) {
		// then we have an explicit confirm
		saoc.sSystemAction = SA_EXPL_CONF;
	}

	// check if the concept is among the implicitly confirmed concepts
	else if(saSystemAction.setcpImplicitConfirms.find(pConcept) != 
		saSystemAction.setcpImplicitConfirms.end()) {
		// then we have an implicit confirm
		saoc.sSystemAction = SA_IMPL_CONF;
	}

	// check if the concept is among the unplanned implicitly confirmed concepts
	else if(saSystemAction.setcpUnplannedImplicitConfirms.find(pConcept) != 
		saSystemAction.setcpUnplannedImplicitConfirms.end()) {
		// then we have an implicit confirm
		saoc.sSystemAction = SA_UNPLANNED_IMPL_CONF;
	}

	// check if the concept is among the implicitly confirmed concepts
	else {
		// then we have an "other" type request
		saoc.sSystemAction = SA_OTHER;
	}

	// return 
	return saoc;
}

// D: Signal an explicit confirmation on a concept
void CDMCoreAgent::SignalExplicitConfirmOnConcept(CConcept* pConcept) {
    if(saSystemAction.setcpUnplannedImplicitConfirms.find(pConcept) != 
        saSystemAction.setcpUnplannedImplicitConfirms.end()) {
        // if the concept is currently marked under an unplanned implicit 
        // confirm, then delete it from there 
        saSystemAction.setcpUnplannedImplicitConfirms.erase(pConcept);
    }
    // finally add it to the list
	saSystemAction.setcpExplicitConfirms.insert(pConcept);
}

// D: Signal an implicit confirmation on a concept
void CDMCoreAgent::SignalImplicitConfirmOnConcept(CConcept* pConcept) {
    if(saSystemAction.setcpUnplannedImplicitConfirms.find(pConcept) != 
        saSystemAction.setcpUnplannedImplicitConfirms.end()) {
        // if the concept is currently marked under an unplanned implicit 
        // confirm, then delete it from there 
        saSystemAction.setcpUnplannedImplicitConfirms.erase(pConcept);
    }
    // finally add it to the list
	saSystemAction.setcpImplicitConfirms.insert(pConcept);
}

// D: Signal an unplanned implicit confirmation on a concept
void CDMCoreAgent::SignalUnplannedImplicitConfirmOnConcept(int iState, 
														   CConcept* pConcept) {

	if (iState >= 0) {
		if (((*pStateManager)[iState].saSystemAction.setcpImplicitConfirms.find(pConcept) == 
			 (*pStateManager)[iState].saSystemAction.setcpImplicitConfirms.end()) &&
			((*pStateManager)[iState].saSystemAction.setcpExplicitConfirms.find(pConcept) == 
			 (*pStateManager)[iState].saSystemAction.setcpExplicitConfirms.end())) {
			// if it's not already marked as being explicitly or implicitly confirmed
			// now log the current system action 
			Log(DMCORE_STREAM, FormatString("System action dumped below.\n%s", 
				systemActionToString((*pStateManager)[iState].saSystemAction).c_str()));

			(*pStateManager)[iState].saSystemAction.setcpUnplannedImplicitConfirms.insert(pConcept);
		}
	} else {
		if ((saSystemAction.setcpImplicitConfirms.find(pConcept) == 
			 saSystemAction.setcpImplicitConfirms.end()) &&
			(saSystemAction.setcpExplicitConfirms.find(pConcept) == 
			 saSystemAction.setcpExplicitConfirms.end())) {
			// if it's not already marked as being explicitly or implicitly confirmed
			// now log the current system action 
			Log(DMCORE_STREAM, FormatString("System action dumped below.\n%s", 
				systemActionToString(saSystemAction).c_str()));

			saSystemAction.setcpUnplannedImplicitConfirms.insert(pConcept);
		}
	}

}

// D: Signal an accept action on a concept
void CDMCoreAgent::SignalAcceptOnConcept(CConcept* pConcept) {
	// erase that agent from the explicit confirmations
	// but not from the implicit confirmations, since once an IC
	// is done, we want to perform an IC update on that concept
	saSystemAction.setcpExplicitConfirms.erase(pConcept);
}

// D: Returns the agent on top of the execution stack
CDialogAgent* CDMCoreAgent::GetAgentInFocus() {
    TExecutionStack::iterator iPtr;
    for(iPtr = esExecutionStack.begin(); iPtr != esExecutionStack.end(); iPtr++) {
		if(iPtr->pdaAgent->IsExecutable()) {
			return iPtr->pdaAgent;
		}
    }

    // o/w return NULL
    return NULL;
}

// D: Returns the task agent closest to the top of the execution stack
CDialogAgent* CDMCoreAgent::GetDTSAgentInFocus() {
	TExecutionStack::iterator iPtr;
	for(iPtr = esExecutionStack.begin(); 
        iPtr != esExecutionStack.end(); 
        iPtr++) {
        if(iPtr->pdaAgent->IsDTSAgent() && iPtr->pdaAgent->IsExecutable()) 
			return iPtr->pdaAgent;
	}
    // o/w if no task agent found, return NULL
    return NULL;
}

// D: Returns true if the specified agent is in focus
bool CDMCoreAgent::AgentIsInFocus(CDialogAgent* pdaDialogAgent) {

    // if it's not executable, return false
    if(!pdaDialogAgent->IsExecutable())
        return false;

    // if it's not a task agent
    if(!pdaDialogAgent->IsDTSAgent()) {         
        return GetAgentInFocus() == pdaDialogAgent;
    } 

    // if it is a task agent then get the task agent in focus and check 
    // against that
    return GetDTSAgentInFocus() == pdaDialogAgent;
}

// D: Returns the agent that is previous on the stack
//    to the specified agent (i.e. the previously focused dts agent)
CDialogAgent* CDMCoreAgent::GetAgentPreviouslyInFocus(
    CDialogAgent* pdaDialogAgent) {
	if(esExecutionStack.empty())
		return NULL;
	else {
        // start from the beginning
		TExecutionStack::iterator iPtr = esExecutionStack.begin();
        if(pdaDialogAgent == NULL)
            pdaDialogAgent = GetAgentInFocus();
        // advance till we find the agent
        while((iPtr != esExecutionStack.end()) && 
            (iPtr->pdaAgent != pdaDialogAgent)) iPtr++;
        // if not found, return NULL
		if(iPtr == esExecutionStack.end()) 
			return NULL;
        // o/w advance till we find an executable one or the end
        iPtr++;
        while((iPtr != esExecutionStack.end()) &&
			   !iPtr->pdaAgent->IsExecutable())
            iPtr++;
        // if we're at the end return NULL
		if(iPtr == esExecutionStack.end()) 
			return NULL;
        // o/w return that agent
		else return iPtr->pdaAgent;
	}
}

// D: Returns the dialog task specification agent that is previous on the stack
//    to the specified agent (i.e. the previously focused dts agent)
CDialogAgent* CDMCoreAgent::GetDTSAgentPreviouslyInFocus(
    CDialogAgent* pdaDialogAgent) {
	if(esExecutionStack.empty())
		return NULL;
	else {
        // start from the beginning
		TExecutionStack::iterator iPtr = esExecutionStack.begin();
        if(pdaDialogAgent == NULL)
            pdaDialogAgent = GetAgentInFocus();
        // advance till we find the agent
        while((iPtr != esExecutionStack.end()) && 
            (iPtr->pdaAgent != pdaDialogAgent)) iPtr++;
        // if not found, return NULL
		if(iPtr == esExecutionStack.end()) 
			return NULL;
        // o/w advance one more
        iPtr++;
        // and do it until we reach a DTS agent
        while((iPtr != esExecutionStack.end()) && 
            !iPtr->pdaAgent->IsDTSAgent()) iPtr++;
        // if we're at the end return NULL
		if(iPtr == esExecutionStack.end()) 
			return NULL;
        // o/w return that agent
		else return iPtr->pdaAgent;
	}
}

// D: Returns the current main topic agent
// A: The main topic is now identified by going down the stack
//    instead of using the tree
CDialogAgent* CDMCoreAgent::GetCurrentMainTopicAgent() {
    TExecutionStack::iterator iPtr;
	for(iPtr = esExecutionStack.begin(); 
        iPtr != esExecutionStack.end(); 
        iPtr++) {

		if (iPtr->pdaAgent->IsAMainTopic()) {
			return iPtr->pdaAgent;
		}
    }

	// No main topic found on the stack (probably an error)
	return NULL;
}

// D: Returns true if the agent is an active topic
bool CDMCoreAgent::AgentIsActive(CDialogAgent* pdaDialogAgent) {
	TExecutionStack::iterator iPtr;
	for(iPtr = esExecutionStack.begin(); iPtr != esExecutionStack.end(); iPtr++) {
		if(iPtr->pdaAgent == pdaDialogAgent) 
			return true;
	}
	return false;
}

// D: Eliminates a given agent from the execution stack
void CDMCoreAgent::PopAgentFromExecutionStack(CDialogAgent* pdaADialogAgent) {

    // call upon the private helper function to eliminate the agent
    TStringVector vsAgentsEliminated;
    popAgentFromExecutionStack(pdaADialogAgent, vsAgentsEliminated);

    // if the agent was found, log and return 
    if(vsAgentsEliminated.size() != 0) {
        string sAgents;
        for(unsigned int i = 0; i < vsAgentsEliminated.size(); i++)
            sAgents += FormatString("%s\n", vsAgentsEliminated[i].c_str());

        sAgents = TrimRight(sAgents);

        Log(DMCORE_STREAM, "Eliminated %d agent(s) (dumped below) from the "\
            "execution stack.\n%s", vsAgentsEliminated.size(), sAgents.c_str());
    }
}

// D: Eliminates a given agent from the execution stack, together with all the
//    agents it has planned for execution
void CDMCoreAgent::PopTopicFromExecutionStack(CDialogAgent* pdaADialogAgent) {

    // call upon the private helper function to eliminate the agent
    TStringVector vsAgentsEliminated;
    popTopicFromExecutionStack(pdaADialogAgent, vsAgentsEliminated);

    // if the agent was found, log and return 
    if(vsAgentsEliminated.size() != 0) {
        string sAgents;
        for(unsigned int i = 0; i < vsAgentsEliminated.size(); i++)
            sAgents += FormatString("%s\n", vsAgentsEliminated[i].c_str());

        sAgents = TrimRight(sAgents);

        Log(DMCORE_STREAM, "Eliminated %d agent(s) (dumped below) from the "\
            "execution stack.\n%s", vsAgentsEliminated.size(), sAgents.c_str());
    }
}

// A: Eliminates all grounding agents (i.e. all agents schedules by the 
//    grounding manager) from the execution stack
void CDMCoreAgent::PopGroundingAgentsFromExecutionStack() {

    // call upon the private helper function to eliminate the agent
    TStringVector vsAgentsEliminated;
    popGroundingAgentsFromExecutionStack(vsAgentsEliminated);

    // if the agent was found, log and return 
    if(vsAgentsEliminated.size() != 0) {
        string sAgents;
        for(unsigned int i = 0; i < vsAgentsEliminated.size(); i++)
            sAgents += FormatString("%s\n", vsAgentsEliminated[i].c_str());

        sAgents = TrimRight(sAgents);

        Log(DMCORE_STREAM, "Eliminated %d grounding agent(s) (dumped below) from the "\
            "execution stack.\n%s", vsAgentsEliminated.size(), sAgents.c_str());
    }
}

// A: Returns the last input turn number
int CDMCoreAgent::GetLastInputTurnNumber() {
	return iTurnNumber;
}
