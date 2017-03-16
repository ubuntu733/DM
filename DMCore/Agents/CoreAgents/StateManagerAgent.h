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
// STATEMANAGERAGENT.H   - defines the state manager agent class. This agent
//						   keeps a history of states the the DM went through 
//						   throughout the conversation. It can also provide
//						   access to variuos "collapsed" states, which can be
//						   useful for learning
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
//	 [2007-06-02] (antoine): fixed GetLastState and operator[] so that they
//							 return reference to TDialogState instead of 
//							 copies of these objects
//   [2005-10-20] (antoine): added GetStateAsString method
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2004-03-28] (dbohus): added functionality for defining the dialog state
//                           names via an external configuration file
//   [2004-03-25] (dbohus): added functionality for broadcasting the dialogue
//                           state to other components
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-04-08] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------


#pragma once
#ifndef __STATEMANAGERAGENT_H__
#define __STATEMANAGERAGENT_H__

#include "Utils/Utils.h"
#include "DMCore/Agents/Agent.h"
#include "DMCore/Agents/DialogAgents/DialogAgent.h"
#include "DMCore/Agents/CoreAgents/DMCoreAgent.h"

//-----------------------------------------------------------------------------
// CStateManagerAgent Class - 
//   This class is an agent which keeps track of state information in the 
//   dialog manager
//-----------------------------------------------------------------------------

// D: structure describing the state of the dialog manager at some point
typedef struct {
	TFloorStatus fsFloorStatus;			// who has the floor?
    string sFocusedAgentName;			// the name of the focused agent
	TExecutionStack esExecutionStack;	// the execution stack
    TExpectationAgenda eaAgenda;		// the expectation agenda
	TSystemAction saSystemAction;		// the current system action
	string sInputLineConfiguration;		// string representation of the input
										// line config at this state (lm, etc)
    int iTurnNumber;					// the current turn number
	int iEHIndex;						// the execution history index matching 
										// this dialog state
	string sStateName;					// the name of the current dialog state
} TDialogState;

// D: the CStateManager class definition
class CStateManagerAgent : public CAgent {

public:
	// dialog state name definitions
	STRING2STRING s2sDialogStateNames;

	// private vector containing a history of the states that the DM went 
	// through
	vector<TDialogState, allocator<TDialogState> > vStateHistory;

    // variable containing the state broadcast address
    string sStateBroadcastAddress;

public:

	//---------------------------------------------------------------------
	// Constructor and destructor
	//---------------------------------------------------------------------
	//
	// Default constructor
	CStateManagerAgent(string sAName, 
					   string sAConfiguration  = "",
					   string sAType = "CAgent:CStateManagerAgent");

	// Virtual destructor
	virtual ~CStateManagerAgent();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

	//---------------------------------------------------------------------
	// CAgent Class overwritten methods 
	//---------------------------------------------------------------------
	//
	virtual void Reset();	// overwritten Reset

public:

	//---------------------------------------------------------------------
	// StateManagerAgent class specific public methods
	//---------------------------------------------------------------------

	// Load the dialog state name mapping from a file
	void LoadDialogStateNames(string sFileName);

    // Set the expectation agenda broadcast address
  void SetStateBroadcastAddress(string sAStateBroadcastAddress);

    // Broadcast the state to the other components in the system
  void BroadcastState();

    // Updates the state information
	void UpdateState();

	// Returns a string representing the state
	string GetStateAsString(TDialogState dsState);
	string GetStateAsString();

	// Access to the length of the history
	int GetStateHistoryLength();

    // Access the last state
  TDialogState &GetLastState();

	// Indexing operator to access states 
	TDialogState &operator[](unsigned int i);

};

#endif // __STATEMANAGERAGENT_H__
