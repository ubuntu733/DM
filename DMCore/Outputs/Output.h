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
// OUTPUT.H - definition of the COutput base class. This class represents an 
//			  output from the dialog manager, and is used by the COutputManager
//			  core agent. The class is the base of a hierarchy.
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
//	 [2007-02-08] (antoine): added bIsFinalOutput
//   [2005-10-22] (antoine): added method GetGeneratorAgentName
//   [2005-10-20] (antoine): added a sDialogState field sent along with the
//                           output (this is for the InteractionManager)
//   [2005-01-11] (antoine): added methods for computing and getting a counter
//                            on the number of repeats for a given output
//   [2004-02-25] (dbohus):  added methods for cancelling and changing concept
//                            notification requests
//   [2004-02-24] (dbohus):  changed outputs so that we no longer clone 
//                            concepts but use them directly
//   [2003-10-20] (dbohus):  fixed a bug in clone so that the vector of 
//                            concepts is cloned correctly even when there are
//                            nulls in there
//   [2003-04-10] (dbohus,  
//                 antoine): fixed bug in clone so that the vector of concepts
//                            is indeed cloned
//   [2002-12-03] (dbohus):  fixed bug in FindConceptByName
//   [2002-10-15] (dbohus):  added destructor so that concepts are deleted
//   [2002-10-14] (dbohus):  added GetOutputDevice, GetAct, GetObject, moved 
//                            all members to protected; removed all the Get 
//							  functions put OutputManagerAgent as friend class
//   [2002-10-13] (dbohus):  changed iParams to iFlags, moved flags definitions
//                            here, added CheckFlags function
//   [2002-10-12] (dbohus):  removed FindConcept, since not used so far
//   [2002-06-25] (dbohus):  unified Create and CreateWithClones in a single
//							  create function which takes a bool parameter
//   [2002-06-17] (dbohus):  deemed preliminary stable version 0.5
//	 [2002-06-17] (dbohus):  added method for finding concepts
//	 [2002-06-14] (dbohus):  drafted this class
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include "Utils/Utils.h"
#include "DMCore/Agents/CoreAgents/DMCoreAgent.h"
#include "DMCore/Concepts/Concept.h"

//-----------------------------------------------------------------------------
// D: The COutput class.
//-----------------------------------------------------------------------------

class COutput {

	friend class COutputHistory;		// output history has friend access
	friend class COutputManagerAgent;	// output manager has friend access 

protected:
	// class members 
	//
	string sGeneratorAgentName;			// name of the agent that ordered this
										// output
	int iOutputId;						// the output id
	int iExecutionIndex;				// the index of the execution item
										// corresponding to the generation
										// of this output

	string sDialogState;				// a string representing the dialog 
										// state at which this prompt was issued
	string sAct;						// the act (dialog move)
	string sObject;					    // the object (acted on or with)
	vector<CConcept *> vcpConcepts;		// the list of concepts referred in 
										//  this output
  vector<bool> vbNotifyConcept;       // parallel vector indicating whether
                                        //  the concept conveyance should be 
                                        //  notified or not
	vector<string> vsFlags;				// flags for the output
	string sOutputDeviceName;			// the name of the device this output
										//  should be directed to
	TConveyance cConveyance;			// whether the output was fully conveyed
										// to the recipient
	int iRepeatCounter;					// the number of times this output has 
										//  been uttered (consecutively)
	TFloorStatus fsFinalFloorStatus;			// the floor status at the end of this output
	


public:
	//---------------------------------------------------------------------
	// Constructors and destructors
	//---------------------------------------------------------------------
	// 
	COutput();
	virtual ~COutput();

public:
	//---------------------------------------------------------------------
	// Public abstract methods - to be overwritten by derived classes
	//---------------------------------------------------------------------
	
	// Pure virtual method which creates a certain output from a given
	// string-represented prompt
	//
	virtual bool Create(string sAGeneratorAgentName, int iAExecutionIndex,
						string sAPrompt, TFloorStatus fsAFloor, int iAOutputId) 
            = 0 ;

	// Pure virtual method which generates a string representation for the
	// output that will be sent to the external output component (i.e. 
	// NLG in the Galaxy framework, Gemini in the OAA framework)
	//
	virtual string ToString() = 0; 

	// Pure virtual method for cloning an output
	//
	virtual COutput* Clone(int iNewOutputId) = 0;
	
	//---------------------------------------------------------------------
	// Public methods 
	//---------------------------------------------------------------------

	// Get name of the agent that generated this output
	string GetGeneratorAgentName();

	// Set and Get dialog state information for this output
	//
	int GetDialogStateIndex();
	void SetDialogStateIndex(int iAExecutionIndex);
	string GetDialogState();
	void SetDialogState(string sADialogState);

    // Set and Get conveyance information on the output
	//
	void SetConveyance(TConveyance cAConveyance);
	TConveyance GetConveyance();

	// Set and Get the act for this output
	void SetAct(string sAAct);
	string GetAct();

	// Set and Get the final floor status for this output
	void SetFinalFloorStatus(TFloorStatus fsAFloor);
	TFloorStatus GetFinalFloorStatus();
	string GetFinalFloorStatusLabel();

	// Checks if a certain flag is set
  bool CheckFlag(string sFlag);

    // Notifies a concept of conveyance information
  void NotifyConceptConveyance(string sConceptName, 
        TConveyance cAConveyance);
        
  // Returns a pointer to the concept, given the concept name
  virtual CConcept* GetConceptByName(string sConceptName);

    // Cancels the notification request for one of the concepts
  void CancelConceptNotificationRequest(CConcept* pConcept);

    // Changes the pointers for one of the concepts (this happens on 
    // reopens and other operations which change the concept pointers)
  void ChangeConceptNotificationPointer(CConcept* pOldConcept, 
        CConcept* pNewConcept);

	// Gets the number of times this prompt has been uttered
	int GetRepeatCounter();
	// Increments the repeat counter
	void IncrementRepeatCounter();

protected:
	//---------------------------------------------------------------------
	// Protected methods 
	//---------------------------------------------------------------------

    // base clone helper function to be called by Clone() in derived classes
	//
	virtual void clone(COutput* pOutput, int iNewOutputId);
};

#endif // __OUTPUT_H__
