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
// OUTPUTMANAGERAGENT.CPP - implements the output manager agent class. This 
//                          agent forwards the output through the bridge to
//                          the output media
// 
// ----------------------------------------------------------------------------
// 
// BEFORE MAKING CHANGES TO THIS CODE, please read the appropriate 
// documentation, available in the Documentation folder. 
//
// ANY SIGNIFICANT CHANGES made should be reflected back in the documentation
// file(s)
//
// ANY CHANGES made (even small bug fixed, should be reflected in the history
// below, in reverse chronological order
// 
// HISTORY --------------------------------------------------------------------
//
//   [2006-06-15] (antoine): merged with latest RavenClaw1 version
//   [2005-01-26] (antoine): modified output so that it handles the 
//                           ":non-listening" flag
//   [2005-01-11] (antoine): modified Notify so that it increments the repeat
//							  counter of the output
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2004-02-25] (dbohus):  added methods for cancelling and changing concept
//                            notification requests
//   [2002-10-25] (dbohus): Redid the concept conveyance notifications
//   [2002-10-21] (dbohus): Added output classes 
//   [2002-10-15] (dbohus): Cleaned up Repeat and Notify
//   [2002-10-14] (dbohus): Eliminated the promptsqueue, no longer needed
//   [2002-10-12] (dbohus): Changed concurrency control to work with
//                          WinAPI criticalsection objects
//   [2002-10-11] (dbohus): Removed concurrency control for COutputHistory
//							as it is not needed
//   [2002-10-10] (dbohus): Unified methods for adding to the history of 
//                          outputs. There's a single one now: AddOutput
//   [2002-10-10] (dbohus): Started revisioning/understanding Drew's code
//   [2002-09-25] (dbohus): Fixed bug when outputting empty prompts
//   [2002-07-14] (agh)   : Added Repeat() functionality
//   [2002-06-25] (dbohus): replaced COutput::CreateWithClones() with the 
//							corresponding parametrized version of Create()
//	 [2002-06-20] (agh)   : robustified Notify() and added use of 
//							CreateWithClones() function.
//   [2002-06-14] (dbohus): started polishing this, changing it to support
//                          the introduction of the COutput class
//	 [2002-05-30] (agh)   : Baseline version complete. Included thread safety.
//   [2002-05-24] (agh)   : Finished basic functionality, Notification, added
//							support for different output media
//	 [2002-04-03] (agh)   : added COutputHistory() class
//   [2002-03-28] (agh)   : began output functions
//   [2002-02-03] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

//#include "DMInterfaces/DMInterface.h"
#include "OutputManagerAgent.h"
#include "DMCore/Agents/Registry.h"
#include "DMCore/Core.h"

#include "DMCore/Outputs/FrameOutput.h"

#include <iostream>
using namespace std;
//-----------------------------------------------------------------------------
//
// A: COutputHistory class methods
//
//-----------------------------------------------------------------------------

static TDialogState *ds;

//-----------------------------------------------------------------------------
// A: Constructors and destructors
//-----------------------------------------------------------------------------
// A: Default constructor
COutputHistory::COutputHistory() {
}

// A: virtual destructor
COutputHistory::~COutputHistory() {
	Clear();
}

//-----------------------------------------------------------------------------
// COutputHistory public methods 
//-----------------------------------------------------------------------------
// DA: Generates a string representation of the history out outputs
string COutputHistory::ToString() {
	// build the string in sResult
	string sResult = "OUTPUT HISTORY\nid\t utterance\n";
	sResult += "-----------------------------------------------------------"\
		       "---------------------\n";

	// add all the utterances in the history in reverse chronological order
	for (int i = vsUtterances.size() - 1; 
		 (i >= 0) && (5 >= vsUtterances.size() - i); i--) 
		sResult += FormatString("%d\t%s\n", vopOutputs[i]->iOutputId, 
								vsUtterances[i].c_str());
    return sResult;
}

// A: appends request to history as sent to the external output agent/server.
//    Should get called on DM Interface thread
unsigned int COutputHistory::AddOutput(COutput* pOutput, string sUtterance) {

	// push the output and actual utterance on the history vectors
	vopOutputs.push_back(pOutput);
	vsUtterances.push_back(sUtterance);
	uiCurrentID++;

	// return the current id
	return uiCurrentID;
}

// A: deletes history and starts a new one, beginning with ID uiNewStart
//    should be called on DM Core thread
void COutputHistory::Clear() {
	// destroy all the output objects in the history
	for (unsigned int i = 0; i < vopOutputs.size(); i++ ) 
		delete vopOutputs[i];
	
	vopOutputs.clear();
	vsUtterances.clear();
	uiCurrentID = 0;
}

// DA: return the size of the output history
unsigned int COutputHistory::GetSize() {
	return vopOutputs.size();
}

// DA: return the utterance at a particular index, starting from the
//     most recent
string COutputHistory::GetUtteranceAt(unsigned int iIndex) {
	int iSize = vsUtterances.size();
    if ( (int)(iSize - 1 - iIndex) < 0 ) {
        FatalError(FormatString("Invalid index in output utterance history: "\
            "index = %d, history_size = %d", iSize, iIndex));
        return "";
    }
	else return vsUtterances[iSize - 1 - iIndex];
}

// DA: accesses the indexth element of the array, counting 0 as the
//     most recent.
COutput* COutputHistory::GetOutputAt(unsigned int iIndex) {
	int iSize = vopOutputs.size();
    if ( (int)(iSize - 1 - iIndex) < 0 ) {
        FatalError(FormatString("Invalid index in output utterance history: "\
            "index = %d, history_size = %d", iSize, iIndex));
		return NULL;
    }
	else return vopOutputs[iSize - 1 - iIndex];
}

// D: a shortcut operator for the GetOutputAt 
inline COutput* COutputHistory::operator [](unsigned int iIndex) {
   return GetOutputAt(iIndex);
}


//-----------------------------------------------------------------------------
//
// Methods for the COutputManagerAgent class
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AD: Constructors and destructors
//-----------------------------------------------------------------------------
// AD: constructor
COutputManagerAgent::COutputManagerAgent(string sAName,
										 string sAConfiguration, 
										 string sAType) : 
	CAgent(sAName, sAConfiguration, sAType) {

	// set an invalid value for the output device since we don't have 
	// any output devices registered yet
	iDefaultOutputDevice = 0;

	// initialize prompt ids
	iOutputCounter = 0;

	// create the critical section object
    //InitializeCriticalSection(&csCriticalSection);

    // initialize the output class by default to frameoutput
    sOutputClass = "FrameOutput";
}

// AD: Virtual destructor
COutputManagerAgent::~COutputManagerAgent() {
	// delete all the outputs that are still in the RecentOutputs list (if any)
	// (the outputs that have been moved to the history will be destroyed
	// by the COutputHistory destructor)
	for(unsigned int i=0; i < vopRecentOutputs.size(); i++) 
		delete vopRecentOutputs[i];

    // delete the critical section object
    //DeleteCriticalSection(&csCriticalSection);
}

//-----------------------------------------------------------------------------
// D: Static function for dynamic agent creation
//-----------------------------------------------------------------------------
CAgent* COutputManagerAgent::AgentFactory(string sAName, string sAConfiguration) {
	return new COutputManagerAgent(sAName, sAConfiguration);
}

//-----------------------------------------------------------------------------
// D: CAgent class overwritten methods
//-----------------------------------------------------------------------------

// D: the overwritten Reset method
void COutputManagerAgent::Reset() {
	// reinitialize everything
	ohHistory.Clear();
	iOutputCounter = 0;

	// delete the list of recent outputs
	for(unsigned int i=0; i < vopRecentOutputs.size(); i++) 
		delete vopRecentOutputs[i];
	vopRecentOutputs.clear();
}

//-----------------------------------------------------------------------------
// D: Method for setting the output class
//-----------------------------------------------------------------------------
void COutputManagerAgent::SetOutputClass(string sAOutputClass) {
    sOutputClass = sAOutputClass;
}

//-----------------------------------------------------------------------------
// A: Methods for registering output devices
//-----------------------------------------------------------------------------

// A: adds output device to list of registered output devices. Output devices
//    have to be registered before they are used
bool COutputManagerAgent::RegisterOutputDevice(string sName, string sServerCall,
											   int iParams ) {
	// check if the device is not already registered
	for ( unsigned int i = 0; i < vodOutputDevices.size(); i++ )
		if ( vodOutputDevices[i].sName == sName ) {
			Log(OUTPUTMANAGER_STREAM, "Device %s already registered", 
				                      sName.c_str());
			return false;
		}

	// if not, add it to the list; start by constructing the appropriate 
	// structure
	TOutputDevice od;
	od.sName = sName;
	od.sServerCall = sServerCall;
	od.iParams = iParams;

	// finally, add it to the list
	vodOutputDevices.push_back(od);
	return true;
}

// A: Sets the default output device. Will invoke a fatal error if device 
//    does not exist.
void COutputManagerAgent::SetDefaultOutputDevice(string sName) {
	// look for the device in the list of registered devices
	for ( unsigned int i = 0; i < vodOutputDevices.size(); i++ )
		if ( vodOutputDevices[i].sName == sName ) {
			iDefaultOutputDevice = i;
			return;
		}
	FatalError("Cannot set default output device to " + sName + ". Device "\
		       "not registered.");
}

// A: looks output device up in registered list; returns null if it's not found
TOutputDevice *COutputManagerAgent::GetOutputDevice(string sName) {
	// if no name specified, return the default output device
	if ( sName == "" )
		return GetDefaultOutputDevice();
	// check through the the list of output devices
	for ( unsigned int i = 0; i < vodOutputDevices.size(); i++ )
		if ( sName == vodOutputDevices[i].sName )
			return &(vodOutputDevices[i]);
	return NULL;
}

// AD: returns pointer to default output device, used when no device was 
//     specified in a prompt
TOutputDevice *COutputManagerAgent::GetDefaultOutputDevice() {
	if((iDefaultOutputDevice >= 0) && 
	   (iDefaultOutputDevice < vodOutputDevices.size())) {
		return &(vodOutputDevices[iDefaultOutputDevice]);
	} else {
		return NULL;
	}
}

// D: returns the name of the default output device
string COutputManagerAgent::GetDefaultOutputDeviceName() {
	if((iDefaultOutputDevice >= 0) && 
	   (iDefaultOutputDevice < vodOutputDevices.size())) {
		return vodOutputDevices[iDefaultOutputDevice].sName;
	} else {
		return "";
	}
}

//-----------------------------------------------------------------------------
// A: Output methods
//-----------------------------------------------------------------------------

// AD: Constructs the appropriate output for each prompt and sends it to device
//     specified in sPrompts.  Also sends out any enqueued prompts first.
//     syntax for sPrompts: {...prompt1...}{...prompt2...}...{...promptn...}
//     see documentation for internal details of prompts
//     输入待解决问题：
//     1.现阶段使用命令行进行输出promot()字符串
vector<COutput*> COutputManagerAgent::Output(CDialogAgent* pGeneratorAgent, 
								 string sPrompts, TFloorStatus fsFinalFloorStatus) {

	// check that we know which dialog agent sent ordered this output
	if ( pGeneratorAgent == NULL )
        FatalError(FormatString("No generator agent specified for output "\
							    "(dump below).\n%s", sPrompts.c_str()));

    // check that the prompts are correctly enclosed in braces {}
    sPrompts = Trim(sPrompts);
    if((sPrompts != "") && (sPrompts[0] != '{')) 
        sPrompts = "{" + sPrompts + "}";

	// vector of pointers to the generated output objects to return to the calling agent
	vector <COutput *> voOutputs;

	// while there are still prompts, process them and send them out
	while ( sPrompts != "" )  {

		// get the first prompt from the queue
    	int iFirstPromptLength = 
			FindClosingQuoteChar(sPrompts, 1, '{', '}');
	    string sFirstPrompt = sPrompts.substr(0, iFirstPromptLength);
	    sPrompts = sPrompts.substr(iFirstPromptLength, 
								   sPrompts.size() - iFirstPromptLength);

		// check that the prompt is not empty; if so, ignore it and continue
		// looping
		if(sFirstPrompt == "{}") continue;

		// Sets the post-prompt floor status
		TFloorStatus fsFloor = fsSystem;
		if (sPrompts == "")
			fsFloor = fsFinalFloorStatus;

        // log the prompt
        Log(OUTPUTMANAGER_STREAM, "Processing output prompt %d from %s. (dump "\
			"below)\n%s", iOutputCounter, pGeneratorAgent->GetName().c_str(),
			sFirstPrompt.c_str());
      pushMessage(output_queue,sFirstPrompt);
 //   cout << sFirstPrompt << endl;
		// create the new output; if we are in a Galaxy configuration, 
		// it's a CFrameOutput; if in an OAA configuration, it's a 
		// CLFOutput
    // 现在统一使用galaxy框架下的代码来实现
     //   COutput *pOutput = (COutput*)(new CFrameOutput);
    //原来代码是根据galaxy和OAA框架的不同生成不同的输出结果类
	 /* #ifdef GALAXY*/
        //if(sOutputClass == "FrameOutput") 
				//pOutput = (COutput*)(new CFrameOutput);
        //else {
            //FatalError(FormatString("Output manager configured with an unknown "
                //"output class: %s", sOutputClass.c_str()));        
        //}
		//#endif

		//#ifdef OAA
        //if(sOutputClass == "LFOutput") 
				//COutput *pOutput = (COutput*)(new CLFOutput);
        //else {
            //FatalError(FormatString("Output manager configured with an unknown "
                //"output class: %s", sOutputClass.c_str()));        
        //}
		//#endif

	 /* if(!pOutput->Create(pGeneratorAgent->GetName(), */
							//pStateManager->GetStateHistoryLength()-1,
							//sFirstPrompt, fsFloor, iOutputCounter)) {
            //// if the output could not be created, deallocate it and ignore
            //delete pOutput;
            //continue;            
        //}

		//// store a pointer to the created output to send back to the calling agent
		//voOutputs.push_back(pOutput);

		//if (pOutput->GetAct() == "repeat") {
			//Repeat();
			//continue;
		//}

		//// send the output
		//string sOutputSpecification = output(pOutput);
    ////实现命令行输出
 ////   cout<< sOutputSpecification <<endl;
		//// log the activity, if it has successfully taken place
		//string sTurnId = "User:???";
		//if (pInteractionEventManager->GetLastInput())
			//sTurnId = "User:" + pInteractionEventManager->GetLastInput()->GetStringProperty("[uttid]");
		//if(sOutputSpecification != "") 
			//Log(OUTPUTMANAGER_STREAM, 
                //"Processed output prompt %d (state %d) and sent it to %s"\
				//" (dump below) [%s]\n%s", iOutputCounter, 
				//pStateManager->GetStateHistoryLength()-1,
				//pOutput->sOutputDeviceName.c_str(), sTurnId.c_str(),
				//sOutputSpecification.c_str());		

		//// and finally increase the output counter
		//iOutputCounter++;
  }

	return voOutputs;
}

// AD: Constructs and sends prompt that will repeat last utterance
void COutputManagerAgent::Repeat() {

    // guard for safe access using the critical section 
	//EnterCriticalSection(&csCriticalSection);
	
    unsigned int i;
	unsigned int uiChosen = (unsigned int)-1;

    // check if there is something in history to repeat
    if (ohHistory.GetSize() == 0) {
        Warning("Output history is empty, there is nothing to be repeated.");
        // leave the critical section 
        //LeaveCriticalSection(&csCriticalSection);
        return;
    }

	// if we have been notified of all outputs
	if ( ( vopRecentOutputs.size() == 0 ) && 
		 ( ohHistory[0]->GetConveyance() == cConveyed ) && 
		 !( ohHistory[0]->CheckFlag(":non-repeatable") ) )
        // and the last one is repeatable, then choose that one
		uiChosen = 0;
	else {
		i = 0;
		if ( (vopRecentOutputs.size() == 0) && 
			 (ohHistory[0]->GetConveyance() != cConveyed) )
			i = 1;

        // now go through history, most recent first, and decide which to repeat
		for ( ; i < ohHistory.GetSize(); i++ )	{
			if ( !(ohHistory[i]->CheckFlag(":non-repeatable") )) {
				uiChosen = i;
				break;
			}
		}
	}

    // if nothing got chosen so far
	if ( uiChosen == -1 ) {
		Log(OUTPUTMANAGER_STREAM, 
			"No repeatable outputs found.  Repeating most recent.");
		if ( (vopRecentOutputs.size()) != 0 || 
			 (ohHistory[0]->GetConveyance() == cConveyed) || (ohHistory.GetSize() == 1) )
			uiChosen = 0;
		else
			uiChosen = 1;
	}

    // log the decision 
	Log(OUTPUTMANAGER_STREAM, "Repeating: %s", ohHistory.GetUtteranceAt(uiChosen).c_str());
	
    // clone old output and give it a new output id
	COutput *opToRepeat = ohHistory[uiChosen]->Clone(iOutputCounter);

	// set the dialog state to the current one
//	opToRepeat->SetDialogStateIndex(pStateManager->GetStateHistoryLength()-1);
//	opToRepeat->SetDialogState(pStateManager->GetStateAsString());

    // leave the critical section 
    //LeaveCriticalSection(&csCriticalSection);

    // send out the output
	output(opToRepeat);

    // increment the output counter
	iOutputCounter++;
}

// AD: Incorporates conveyance information and move the output from the recent
//     list to the history
void COutputManagerAgent::Notify(int iOutputId, int iBargeinPos, 
								 string sConveyance, string sTaggedUtt) {

    // log the reception of the notify message
    Log(OUTPUTMANAGER_STREAM, "Received final output notification frame. id: %d; "\
                              "bargein: %d; Conveyance Info: (dump below)\n"\
                              "%s\nTagged Utterance: (dump below)\n%s",
                              iOutputId, iBargeinPos, sConveyance.c_str(), 
                              sTaggedUtt.c_str());
	
	// parse the conveyance information string
	vector<string> vsParsedConveyance = PartitionString(sConveyance, " ");

	// obtain an index to the corresponding output
	unsigned int iIndex = getRecentOutputIndex(iOutputId);
	if ( iIndex == -1 ) {
		Warning(FormatString("Received notification about output %d, which doesn't "\
                             "exist is the list of recent outputs. Ignoring it.", 
                             iOutputId));
		return;
	}

	// step through concept/position pairs and set conveyance status of concepts
	for ( unsigned int i = 0; i < vsParsedConveyance.size(); i += 2 ) {
        if((iBargeinPos == -1) || 
            (atoi(vsParsedConveyance[i+1].c_str()) < iBargeinPos)) {
            vopRecentOutputs[iIndex]->NotifyConceptConveyance(
                vsParsedConveyance[i], cConveyed);
        } else {
            vopRecentOutputs[iIndex]->NotifyConceptConveyance(
                vsParsedConveyance[i], cFailed);        
        }
	}

	// set whether or not entire output was conveyed (was if iBargeinPos == -1)
	if ( iBargeinPos == -1 )
		vopRecentOutputs[iIndex]->SetConveyance(cConveyed);
	else
		vopRecentOutputs[iIndex]->SetConveyance(cFailed);

	// increments the number of times this prompt has been uttered
	vopRecentOutputs[iIndex]->IncrementRepeatCounter();

	// guard for safe access
	//EnterCriticalSection(&csCriticalSection);

	// move last prompt frame to history
    // normalize the tagged utterance before
    sTaggedUtt = Trim(sTaggedUtt, " \n");
	ohHistory.AddOutput(vopRecentOutputs[iIndex], sTaggedUtt);
	vopRecentOutputs.erase(vopRecentOutputs.begin()+iIndex);
	
    // guard for safe access
    //LeaveCriticalSection(&csCriticalSection);
	
    // finally, log the new history on the OUTPUTHISTORY_STREAM
	Log(OUTPUTHISTORY_STREAM, ohHistory.ToString());
}

// D: Gets information about prompts that are about to be sent out
void COutputManagerAgent::PreliminaryNotify(int iOutputId, string sTaggedUtt) {

	// log the reception of the notify message
    Log(OUTPUTMANAGER_STREAM, 
		"Received preliminary output notification frame. id: %d; "
        "Tagged Utterance: (dump below)\n%s",
        iOutputId, sTaggedUtt.c_str());
	
	// obtain an index to the corresponding output
	unsigned int iIndex = getRecentOutputIndex(iOutputId);
	if ( iIndex == -1 ) {
		Warning(FormatString("Received notification about output %d, which doesn't "\
                             "exist is the list of recent outputs. Ignoring it.", 
                             iOutputId));
		return;
	}

    // now go through the prompt and compute the concepts
    vector<string> svWords = PartitionString(sTaggedUtt, " ");
    string sConcept;    
    for(unsigned int i = 0; i < svWords.size(); i++) {
        string sWord = Trim(svWords[i]);
        if(sWord[0] == '<') {
            // if we have the beginning of a concept
            sConcept = sWord.substr(1, sWord.length()-1);
            // and notify that we are doing an ICT on it
            CConcept* pConcept = vopRecentOutputs[iIndex]->GetConceptByName(sConcept);
            if(pConcept) {
				for (int j = vopRecentOutputs[iIndex]->GetDialogStateIndex();
					(j < pStateManager->GetStateHistoryLength()) &&
					((*pStateManager)[j].fsFloorStatus != fsUser); j++) 
					pDMCore->SignalUnplannedImplicitConfirmOnConcept(j, pConcept);
                // now log that we found a concept
                Log(OUTPUTMANAGER_STREAM, "Signaling UNPLANNED_IMPL_CONF on concept %s", 
                    sConcept.c_str());
            }
        }
    }

}

// D: Cancels a concept notification request
void COutputManagerAgent::CancelConceptNotificationRequest(
    CConcept* pConcept) {
    // guard for safe access
    //EnterCriticalSection(&csCriticalSection);
    // remove notification from recent outputs
    for(unsigned int i = 0; i < vopRecentOutputs.size(); i++)
        vopRecentOutputs[i]->CancelConceptNotificationRequest(pConcept);
    // remove notification from history outputs
    for(unsigned int i = 0; i < ohHistory.GetSize(); i++) 
        ohHistory[i]->CancelConceptNotificationRequest(pConcept);
    // leave critical section
    //LeaveCriticalSection(&csCriticalSection);
}

// D: Changes a concept notification request
void COutputManagerAgent::ChangeConceptNotificationPointer(
    CConcept* pOldConcept, CConcept* pNewConcept) {
    // guard for safe access
    //EnterCriticalSection(&csCriticalSection);
    // remove notification from recent outputs
    for(unsigned int i = 0; i < vopRecentOutputs.size(); i++)
        vopRecentOutputs[i]->ChangeConceptNotificationPointer(
            pOldConcept, pNewConcept);
    // remove notification from history outputs
    for(unsigned int i = 0; i < ohHistory.GetSize(); i++) 
        ohHistory[i]->ChangeConceptNotificationPointer(
            pOldConcept, pNewConcept);
    // leave critical section
    //LeaveCriticalSection(&csCriticalSection);
}

// D: Return the list of prompts that are waiting for notification
string COutputManagerAgent::GetPromptsWaitingForNotification() {
	// construct the string
	string sResult = "";
	// guard for safe access
	//EnterCriticalSection(&csCriticalSection);
    for(unsigned int i = 0; i < vopRecentOutputs.size(); i++)
		sResult = FormatString("%s %d", sResult.c_str(), 
			vopRecentOutputs[i]->iOutputId);
	
	// leave critical section
	//LeaveCriticalSection(&csCriticalSection);

	// return the string 
	return sResult;
}

//-----------------------------------------------------------------------------
// A: COutputManager private (helper) methods
//-----------------------------------------------------------------------------

// A: This is the function that actually sends the output to the interaction 
//    manager
string COutputManagerAgent::output(COutput* pOutput) {

	// obtain the string representation of the output
	string sOutput = pOutput->ToString();
  cout<< sOutput <<endl;
  //还未实现自然语言生成模块，暂时注释，待改动
	// if we are in a Galaxy configuration, send requests through the Galaxy 
	// interface
 /* #ifdef GALAXY	*/
	//TGIGalaxyActionCall gcGalaxyCall;
	//gcGalaxyCall.sModuleFunction = "main";
	//gcGalaxyCall.sActionType = "system_utterance";
	//gcGalaxyCall.s2sProperties.insert(STRING2STRING::value_type(":inframe", 
															//sOutput));
 
	//gcGalaxyCall.s2sProperties.insert(STRING2STRING::value_type(":id", 
        //FormatString("%s:%.3d", DMI_GetSessionID().c_str(), iOutputCounter)));
	//gcGalaxyCall.s2sProperties.insert(STRING2STRING::value_type(":utt_count", 
        //FormatString("%d", iOutputCounter)));
	//gcGalaxyCall.s2sProperties.insert(STRING2STRING::value_type(":dialog_state_index", 
       //FormatString("%d", pOutput->GetDialogStateIndex())));
	//gcGalaxyCall.s2sProperties.insert(STRING2STRING::value_type(":dialog_state", 
       //pOutput->GetDialogState()));
	//gcGalaxyCall.s2sProperties.insert(STRING2STRING::value_type(":dialog_act", 
       //pOutput->GetAct()));
	//gcGalaxyCall.s2sProperties.insert(STRING2STRING::value_type(":final_floor_status", 
		//pOutput->GetFinalFloorStatusLabel()));
	//// Adds the output flags to the galaxy frame
	//for (unsigned int i = 0; i < pOutput->vsFlags.size(); i++) {
		//gcGalaxyCall.s2sProperties.insert(STRING2STRING::value_type(
			//pOutput->vsFlags[i], "true"));		
	//}

    //// retrieve the current thread id
    //DWORD dwThreadId = GetCurrentThreadId();

    //// send the message to the Galaxy Interface Thread
    //PostThreadMessage(g_idDMInterfaceThread, WM_GALAXYACTIONCALL,
                      //(WPARAM)&gcGalaxyCall, dwThreadId);	
	
	//// and wait for a reply
	//MSG Message;
	//GetMessage(&Message, NULL, WM_ACTIONFINISHED, WM_ACTIONFINISHED);
	//#endif // GALAXY

	//// if we are in an OAA configuration, send requests through the OAA
	//// interface
	//#ifdef OAA
	//TOIOAACall oaacOAACall;
	//oaacOAACall.picltGoal = icl_NewTermFromString((char *)sOutput.c_str());
	//// check that the OAA goal was successfully created
	//if(oaacOAACall.picltGoal == NULL) {
		//Error(FormatString("Error creating ICL Term for output (dump below). "\
							 //"Output will not be sent.\n%s", sOutput.c_str()));
		//return "";
	//}
	//oaacOAACall.picltInitialParams = icl_NewTermFromString("[]");
	//oaacOAACall.ppicltOutParams = NULL;
	//oaacOAACall.ppicltSolutions = NULL;

	//// retrieve the current thread id
	//DWORD dwThreadId = GetCurrentThreadId();

	//// send the message to the OAA Interface Thread
    //PostThreadMessage(g_idDMInterfaceThread, WM_OAACALL,
                      //(WPARAM)&oaacOAACall, dwThreadId);	
	
	//// and wait for a reply
	//MSG Message;
	//GetMessage(&Message, NULL, WM_ACTIONFINISHED, WM_ACTIONFINISHED);
	//#endif

	//// record request to await notification, if notifications are enabled for 
    //// that device
	//if ( GetOutputDevice(pOutput->sOutputDeviceName)->iParams & OD_NOTIFIES ) {
        //// guard access to vopRecentRequests by critical section
        //EnterCriticalSection(&csCriticalSection);
		//vopRecentOutputs.push_back(pOutput);
        //LeaveCriticalSection(&csCriticalSection);
    //} else {
        //// o/w move the output directly to the history of outputs...
        //// guard for safe access
			//EnterCriticalSection(&csCriticalSection);
        //// and before moving, also set the output as conveyed, since there
        //// will be no future notification
        //pOutput->SetConveyance(cConveyed);
        //ohHistory.AddOutput(pOutput, sOutput);
        //// guard for safe access
        //LeaveCriticalSection(&csCriticalSection);
    //}

    return sOutput;
}

// A: utility function that finds the index of an output in the recent outputs
//    list, based on the output id
unsigned int COutputManagerAgent::getRecentOutputIndex(int iOutputId) {
    // go through the list and look for that id
	for (unsigned int i = 0; i < vopRecentOutputs.size(); i++ ) {
		if ( vopRecentOutputs[i]->iOutputId == iOutputId )
			return i;
	}
    // if not found, return -1
	return (unsigned int)-1;
}
