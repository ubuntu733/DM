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
// INTERACTIONEVENT.CPP - encapsulates an interaction event such as a notification
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

#include "InteractionEvent.h"

#include "DMCore/Log.h"

//---------------------------------------------------------------------
// Constructor and destructor
//---------------------------------------------------------------------
//

// A: Default constructor
// 此处做了修改，让confidence默认为1.0
CInteractionEvent::CInteractionEvent():fConfidence(1.0) {}

// A: Specifies an event type
CInteractionEvent::CInteractionEvent(string sAType) {
	sType = sAType;
  fConfidence = 1.0;
}

// A: Destructor
CInteractionEvent::~CInteractionEvent() {}


//---------------------------------------------------------------------
// Public methods to access private members
//---------------------------------------------------------------------
//

// A: Returns a string describing the event type
string CInteractionEvent::GetType() {
	return sType;
}

// A: Returns the event ID
int CInteractionEvent::GetID() {
	return iID;
}

// A: Indicates whether this event is a completed one or a partial update
bool CInteractionEvent::IsComplete() {
	return bComplete;
}

void CInteractionEvent::SetComplete(bool complete) {
  bComplete = complete;
}
// A: Returns the confidence about the event
float CInteractionEvent::GetConfidence() {
	return fConfidence;
}

// A: Returns the string value for a property of the event
string CInteractionEvent::GetStringProperty(string sSlot) {
	STRING2STRING::iterator i = s2sProperties.find(sSlot);
	if (i != s2sProperties.end()) {
		return i->second;
	} else {
		Log(WARNING_STREAM, "Property %s not found in event. "\
			"Returned empty string.", sSlot.c_str());
		return "";
	}
}

// A: Returns the int value for a property of the event
int CInteractionEvent::GetIntProperty(string sSlot) {
	STRING2STRING::iterator i = s2sProperties.find(sSlot);
	if (i != s2sProperties.end()) {
		return atoi(i->second.c_str());
	} else {
		Log(WARNING_STREAM, "Property %s not found in event. "\
			"Returned 0.", sSlot.c_str());
		return 0;
	}
}

// A: Returns the float value for a property of the event
float CInteractionEvent::GetFloatProperty(string sSlot) {
	STRING2STRING::iterator i = s2sProperties.find(sSlot);
	if (i != s2sProperties.end()) {
		return (float)atof(i->second.c_str());
	} else {
		Log(WARNING_STREAM, "Property %s not found in event. "\
			"Returned 0.", sSlot.c_str());
		return 0.0f;
	}
}

// A: Checks if a certain property is defined for the event
bool CInteractionEvent::HasProperty(string sSlot) {
	return s2sProperties.find(sSlot) != s2sProperties.end();
}

// A: Sets a property value
void CInteractionEvent::SetProperty(string sSlot, string sValue) {
	s2sProperties[sSlot] = sValue;
}

// A: Returns the hash of event properties
STRING2STRING &CInteractionEvent::GetProperties() {
	return s2sProperties;
}

// A: Returns a string representation of the event
string CInteractionEvent::ToString() {
	string sEvent;

	sEvent = FormatString("Type\t%s\n", sType.c_str());
	sEvent += FormatString("Complete\t%d\n", bComplete);

	// returns the contents of the input hash
	if(s2sProperties.size() > 0) {
		STRING2STRING::iterator iPtr;
		for(iPtr = s2sProperties.begin(); 
			iPtr != s2sProperties.end(); 
			iPtr++) {

			sEvent += FormatString("  %s = %s\n", 
				(iPtr->first.c_str()),
				(iPtr->second.c_str()));
		}
	}
	
	// finally return the string
	return sEvent;
}

//---------------------------------------------------------------------
// Methods to test and access event properties
//---------------------------------------------------------------------

// A: Check if a certain expectation is matched by the input 
// D: fixed bug to match case-insensitive
bool CInteractionEvent::Matches(string sGrammarExpectation) {

	// remove the "[" and "]" around the expectation
	string sTemp = sGrammarExpectation.substr( 1, sGrammarExpectation.size()-2);	

	// extracts the expectation channel from the grammar expectation string
	string sChannel, sExpectation;
	SplitOnFirst(sTemp, ":", sChannel, sExpectation);
	if (sExpectation == "") {
		sExpectation = sChannel;
		sChannel = "";
	}

	// the expectation is not for this type of event, no match
	if ((sChannel != "")&&(sChannel != sType)) {
		return false;
	}

	// traverses the hash and matches the given expectation
	STRING2STRING::iterator iPtr;
	for(iPtr = s2sProperties.begin(); 
		iPtr != s2sProperties.end(); 
		iPtr++) {
			if (matchesSlot(sExpectation, iPtr->first.c_str())) {
				return true;
			}
	}
	
	return false;
}

// A: Returns the string corresponding to a given expectation in the input
string CInteractionEvent::GetValueForExpectation(string sGrammarExpectation) {
	string matched_value;

	// remove the "[" and "]" around the expectation
	string sTemp = sGrammarExpectation.substr( 1, sGrammarExpectation.size()-2);	

	// extracts the expectation channel from the grammar expectation string
	string sChannel, sExpectation;
	SplitOnFirst(sTemp, ":", sChannel, sExpectation);
	if (sExpectation == "") {
		sExpectation = sChannel;
		sChannel = "";
	}

	// the expectation is not for this type of event, no match
	if ((sChannel != "")&&(sChannel != sType)) {
		Warning(FormatString("Channel mismatch for %s, empty value used.", 
			sGrammarExpectation.c_str()));
		return "";
	}

	// traverses the hash, searching for the best match for the expectation
	// "best" = (slot name matches) & (shallowest/broadest)
	STRING2STRING::iterator iPtr;
	for(iPtr = s2sProperties.begin(); 
		iPtr != s2sProperties.end(); 
		iPtr++) {

		if (matchesSlot(sExpectation, iPtr->first)) {
	
			// returns the value of the matched slot
			matched_value = iPtr->second;
			if (matched_value == "") {
				Warning(FormatString("Event property %s has empty value.",
					sGrammarExpectation.c_str()));
			}
			return matched_value;
		}
	}

	// no matching property found, log a warning and return ""
	Warning(FormatString("No event property found matching %s.", 
		sGrammarExpectation.c_str()));
	return "";
}

// A: Matches a slot with an expectation pattern
//    allowing for wild cards
#pragma warning (disable:4127)
bool CInteractionEvent::matchesSlot( string pattern, string slot) {
	unsigned int pos_slot = 0;
	unsigned int len_slot = 0;
	unsigned int pos_patt = 0;
	unsigned int len_patt = 0;
	string sub_slot, sub_patt;

	// removes the "[" and "]" around the slot
	slot = slot.substr( 1, slot.size()-2);

	// gets the first level in the pattern
	if ((int)pattern.find_first_of( '.', pos_patt) > (int)pos_patt) {
		len_patt = pattern.find_first_of( '.', pos_patt) - pos_patt;
	}
	else {
		// no more '.', get the end of the pattern
		len_patt = pattern.size() - pos_patt;
	}
	sub_patt = pattern.substr( pos_patt, len_patt);
	pos_patt += len_patt + 1;

	// gets the first level in the slot name
	if ((int)slot.find_first_of( '.', pos_slot) > (int)pos_slot) {
		len_slot = slot.find_first_of( '.', pos_slot) - pos_slot;
	}
	else {
		// no more '.', get the end of the slot name
		len_slot = slot.size() - pos_slot;
	}
	sub_slot = slot.substr( pos_slot, len_slot);
	pos_slot += len_slot + 1;

	// traverses the expectation pattern and the slot name
	while (true) {

		// compare the level names
		if (strcasecmp( sub_slot.c_str(), sub_patt.c_str()) == 0) {
			
			// the final subslot of both the pattern and the slot matched:
			// we won!
			if ((pos_patt >= pattern.size())&&(pos_slot >= slot.size())) {
				return true;
			}

			// we reached the end of the pattern but not that of the slo => fail
			if (pos_patt >= pattern.size()) {
				return false;
			}

			if ((int)pattern.find_first_of( '.', pos_patt) > (int)pos_patt) {
				len_patt = pattern.find_first_of( '.', pos_patt) - pos_patt;
			}
			else {
				// no more '.', get the end of the pattern
				len_patt = pattern.size() - pos_patt;
			}
			sub_patt = pattern.substr( pos_patt, len_patt);

			// move the position index for the pattern
			pos_patt += len_patt + 1;
		}

		// we reached the end of the slot but not that of the pattern => fail
		if (pos_slot >= slot.size()) {
			return false;
		}

		// gets the next level in the slot name
		if ((int)slot.find_first_of( '.', pos_slot) > (int)pos_slot) {
			len_slot = slot.find_first_of( '.', pos_slot) - pos_slot;
		}
		else {
			// no more '.', get the end of the slot name
			len_slot = slot.size() - pos_slot;
		}
		sub_slot = slot.substr( pos_slot, len_slot);

		// moves the position index for the slot
		pos_slot += len_slot + 1;
	}
}
#pragma warning (default:4127)
void CInteractionEvent::SetProperties(STRING2STRING& s2sProperties) {
  STRING2STRING::iterator iPtr;
  for(iPtr=s2sProperties.begin(); iPtr != s2sProperties.end(); iPtr++) {
    SetProperty(iPtr->first,iPtr->second);
  }
}



