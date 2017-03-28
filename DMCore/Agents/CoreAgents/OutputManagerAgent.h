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
// OUTPUTMANAGERAGENT.H   - definition of the output manager agent class. This 
//                          agent forwards the output thourgh the brige onto
//                          the NLG/Graphical Interface, etc
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

#pragma once
#ifndef __OUTPUTMANAGERAGENT_H__
#define __OUTPUTMANAGERAGENT_H__

//include <windows.h>
//#include <winuser.h>
#include "Utils/Utils.h"
#include "DMCore/Agents/Agent.h"
#include "DMCore/Agents/DialogAgents/DialogAgent.h"
#include "DMCore/Concepts/AllConcepts.h"
#include "DMCore/Outputs/Output.h"
#include "DMCore/message/message.h"

// A: Output Device parameters
// if defined, device will send notifications back to output manager
#define OD_NOTIFIES 0x1

//-----------------------------------------------------------------------------
// A: Describes destination for a prompt (output device) sent to be outputted 
//    with a key name, an external server call, and any parameters that modify 
//    how the Output Manager should handle the output
//-----------------------------------------------------------------------------
typedef struct {
	string sName;			// name of the output device
	string sServerCall;		// external server name (module.function for galaxy)
	int iParams;			// other parameters for the output device
} TOutputDevice;


//-----------------------------------------------------------------------------
// A: COutputHistory Class - 
//     Keeps record of outputs sent to generation and utterances built, and
//	   status of success of outputting those utterances
//-----------------------------------------------------------------------------
class COutputHistory {

private:
	// private members
	//
	
	vector<string> vsUtterances;		// history of utterances as strings
	vector<COutput*> vopOutputs;		// history of outputs

	unsigned int uiCurrentID;			// next id to be added to history

public:
	//---------------------------------------------------------------------
	// Constructors and destructor
	//---------------------------------------------------------------------
	//
	// Default constructor
	COutputHistory();

	// Destructor
	virtual ~COutputHistory();

public:
	//---------------------------------------------------------------------
	// public methods
	//---------------------------------------------------------------------

	// Method for generating a string representation of the history of
	// outputs. Used basically for logging purposes
	string ToString();

	// Methods for adding to/removing from the history
	// 
	unsigned int AddOutput(COutput* pOutput, string sUtterance);
	void Clear();

	// Methods for searching/accessing the history of outputs
	// 
	unsigned int GetSize();
	
	string GetUtteranceAt(unsigned int iIndex);
	COutput* GetOutputAt(unsigned int iIndex);
	COutput* operator[](unsigned int iIndex);
};	

//-----------------------------------------------------------------------------
// A: COutputManagerAgent Class - 
//     This class implements the core agent which handles output management in
//	   the RavenClaw dialog manager.
//-----------------------------------------------------------------------------

class COutputManagerAgent : public CAgent {

private:
	//---------------------------------------------------------------------
	// Private members
	//---------------------------------------------------------------------
	//
	// the history of outputs 
	COutputHistory ohHistory;			

	// list of registered output devices
	vector <TOutputDevice> vodOutputDevices;
	// index to the default output device
	unsigned int iDefaultOutputDevice;

	// list of last outputs sent, about which we haven't been notified of 
    // success
	vector <COutput *> vopRecentOutputs; 	
	
	// counter for generating output IDs
	int iOutputCounter;

	// critical section object for enforcing concurrency control
    //CRITICAL_SECTION csCriticalSection;

    // the class of outputs to be used
    string sOutputClass;

public:
	//---------------------------------------------------------------------
	// Constructor and destructor
	//---------------------------------------------------------------------
	//
	// Default constructor
	COutputManagerAgent(string sAName, 
						string sAConfiguration = "",
						string sAType = "CAgent:COutputManagerAgent");

	// Virtual destructor
	virtual ~COutputManagerAgent();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

	//---------------------------------------------------------------------
	// CAgent Class overwritten methods 
	//---------------------------------------------------------------------
	//
	virtual void Reset();	// overwritten Reset

public:

	//---------------------------------------------------------------------
	// OutputManagerAgent class specific public methods
	//---------------------------------------------------------------------

    // Setting the output class to be used
    void SetOutputClass(string sAOutputClass);

	// Methods for registering and accessing output devices
	bool RegisterOutputDevice(string sName, string sServerCall, 
							  int iParams = 0);
	void SetDefaultOutputDevice(string sName);
	TOutputDevice *GetOutputDevice(string sName);
	TOutputDevice *GetDefaultOutputDevice();
	string GetDefaultOutputDeviceName();


	// Output functions
	//

	// Send a set of prompts to the output
	vector<COutput*> Output(CDialogAgent* pGeneratorAgent, string sPrompts, TFloorStatus fsFinalFloorStatus);	

	// Do a repeat
	void Repeat();

	// Gets notification back from output server of results of latest sending
	void Notify(int iOutputId, int iBargeinPos, string sConveyance, 
				string sTaggedUtt);
		
	// Gets a preliminary notification back from output server about 
	// what utterance is about to be spoken
	void PreliminaryNotify(int iOutputId, string sTaggedUtt);

    // Cancels a concept notification request
    void CancelConceptNotificationRequest(CConcept* pConcept);

    // Changes a concept notification request
    void ChangeConceptNotificationPointer(CConcept* pOldConcept, 
        CConcept* pNewConcept);

	// Returns the list of prompts that are waiting for notifications
	string GetPromptsWaitingForNotification();

private:

	//---------------------------------------------------------------------
	// OutputManagerAgent class specific private (helper) methods
	//---------------------------------------------------------------------
 
    // Sends the output to the outside world (i.e. NLG server if in a 
	// Galaxy configuration)
	//
	string output(COutput* pOutput);

	// Helper methods for Notify
	unsigned int getRecentOutputIndex(int iConceptId);
};

#endif // __OUTPUTMANAGERAGENT_H__
