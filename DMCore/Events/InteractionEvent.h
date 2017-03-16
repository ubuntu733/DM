//=============================================================================
//
//   Copyright (c) 2000-2005, Carnegie Mellon University.  
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
// INTERACTIONEVENT.H - encapsulates an interaction event such as a notification
//							of prompt delivery, a new user input, barge-in...
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
//   [2005-11-07] (antoine): added support for partial events
//   [2005-09-01] (antoine): first stable version
//   [2005-06-22] (antoine): started this
//
//-----------------------------------------------------------------------------

#pragma once
#ifndef __INTERACTIONEVENT_H__
#define __INTERACTIONEVENT_H__

#include "Utils/Utils.h"

//-----------------------------------------------------------------------------
// CInteractionEvent Class - 
//   This class handles an event related to interaction with the user and the 
//   outside world, it is mostly abstract since implementations depend on
//   the communication framework (Galaxy, OAA...).
//-----------------------------------------------------------------------------

class CInteractionEvent {

protected:
	//---------------------------------------------------------------------
	// Protected members
	//---------------------------------------------------------------------
	//

	// The type of the event
	string sType;

	// Event ID
	int iID;

	// Flag indicating whether this event is completed or not
	bool bComplete;

	// The confidence that the system has in its detection/understanding
	// of the event
	float fConfidence;

	// The hash of properties for this event
	STRING2STRING s2sProperties;

public:

	//---------------------------------------------------------------------
	// Constructor and destructor
	//---------------------------------------------------------------------
	//
	// Default constructor
	CInteractionEvent();	
	CInteractionEvent(string sAType);	

	// Destructor
	virtual ~CInteractionEvent();

public:

	//---------------------------------------------------------------------
	// Public methods
	//---------------------------------------------------------------------
	//
	// Returns a string describing the type of event
	string GetType();

	// Returns the event ID
	int GetID();

	// Indicates whether this event is a completed one or a partial update
	bool IsComplete();
  //添加函数
  void SetComplete(bool complete = true);
	// Returns the confidence about the event
	float GetConfidence();

	// Returns the value for a specific property of the event
	string GetStringProperty(string sSlot);
	int GetIntProperty(string sSlot);
	float GetFloatProperty(string sSlot);
	bool HasProperty(string sSlot);

	// Sets a property
	void SetProperty(string sSlot, string sValue);
  //添加函数
  void SetProperties(STRING2STRING& s2sProperties);
	// Returns the hash of event properties
	STRING2STRING &GetProperties();

	// returns the whole event as a string
	string ToString();

	//---------------------------------------------------------------------
	// Methods to test and access event properties
	//---------------------------------------------------------------------
	
	// indicates if a certain dialog expectation is met by the event
	bool Matches(string sGrammarExpectation);

	// returns the string from the event that corresponds to a given 
	// dialog expectation
	string GetValueForExpectation(string sGrammarExpectation);

private:
	// performs a pattern matching between two slot names
	// allowing for wildcards
	bool matchesSlot( string slot, string pattern);
};

#endif // __INTERACTIONEVENT_H__
