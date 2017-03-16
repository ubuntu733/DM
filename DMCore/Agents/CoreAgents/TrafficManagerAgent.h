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
// TRAFFICMANAGERAGENT.H - definition of the CTrafficManagerAgent class. This 
//						   core agent forwards calls to other galaxy / OAA 
//						   modules lying outside the dialog manager, and gets
//						   the results back
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
//   [2003-03-13] (antoine): modified CTrafficManagerAgent::Call so that it 
//                            only sends the value of the mode of confidence 
//                            distribution of each concept (instead of 
//                            value1;value2;value3...)
//   [2002-12-09] (dbohus):  updated to log bindings on returns from a galaxy
//                            call
//   [2002-12-09] (dbohus):  added support for sending merged history concepts
//                            i.e. name<@concept
//   [2002-06-10] (dbohus):  changed name to traffic manager agent, started 
//							 work on accomodating OAA calls
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-01-02] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __TRAFFICMANAGERAGENT_H__
#define __TRAFFICMANAGERAGENT_H__

#include "Utils/Utils.h"
#include "DMCore/Agents/Agent.h"
#include "DMCore/Agents/DialogAgents/DialogAgent.h"
#include "DMCore/Concepts/Concept.h"

//-----------------------------------------------------------------------------
// CTrafficManagerAgent Class - 
//   This class is an agent which actually implements a stub towards 
//   external Galaxy / OAA servers
//-----------------------------------------------------------------------------

// D: structure describing a slot-value pair transmitted as input to an outside
//    server call
typedef struct {
	string sSlotName;
	string sSlotValue;
} TSlotValuePair;

// D: structure for a slot-concept* pair which describes how the output from an 
//    outside server call is bound to concepts
typedef struct {
	string sSlotName;
	CConcept* pConcept;
} TSlotConceptPair;
	
// D: structure describing a call to an outside server
typedef struct {
	// the name of the module and/or function called
	//   - for galaxy, this is in the Module.FunctionName form 
	//	 - for OAA, it's just a function name, where it goes will be decided
	//     by the facilitator
	string sModuleFunction;
	// the input parameters for the call
	vector<TSlotValuePair, allocator<TSlotValuePair> > vToParams;
	// the output parameters for the call
	vector<TSlotConceptPair, allocator<TSlotConceptPair> > vFromParams;
	// blocking/non-blocking flag
	bool bBlockingCall;
} TExternalCallSpec;

// D: The actual CTrafficManagerAgent class

class CTrafficManagerAgent : public CAgent {

public:

	//-------------------------------------------------------------------------
	// Constructors and destructors
	//-------------------------------------------------------------------------
	//
	// Default constructor
	CTrafficManagerAgent(string sAName, 
						 string sAConfiguration = "",
						 string sAType = "CAgent:CTrafficManagerAgent");

	// Virtual destructor
	virtual ~CTrafficManagerAgent();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

public:

	//-------------------------------------------------------------------------
	// Traffic manager agent specific public methods
	//-------------------------------------------------------------------------
	// 
	// Makes a call through the DMInterface to a server lying outside the 
	// dialog manager and returns the result 
	//
	void Call(CDialogAgent* pCallerAgent, string sCall);

private:

	//-------------------------------------------------------------------------
	// Traffic manager agent specific private methods
	//-------------------------------------------------------------------------
	// 
	// Makes a call (described by a TExternalCallSpec structure) through 
	// the Galaxy Interface to another galaxy server, and returns the result 
	//
	void galaxyCall(TExternalCallSpec ecsCall);

	// Makes a call (described by a TExternalCallSpec structure) through 
	// the OAA Interface to another OAA module, and returns the result 
	//
	void oaaCall(TExternalCallSpec ecsCall);
};

#endif // __TRAFFICMANAGERAGENT_H__