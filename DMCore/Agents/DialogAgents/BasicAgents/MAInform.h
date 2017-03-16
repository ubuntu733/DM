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
// MAINFORM.H   - definition of the CMAInform class. This class implements 
//                the microagent for Inform
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
//   [2005-10-24] (antoine): removed RequiresFloor method (the method inherited
//							 from CDialogAgent is now valid here)
//   [2005-10-19] (antoine): added RequiresFloor method
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2004-04-16] (dbohus):  added grounding models on dialog agents
//   [2003-04-25] (dbohus,
//                 antoine): the inform agent doesn't check for completion on 
//                            execute any more. the core takes care of that
//   [2003-04-08] (dbohus):  change completion evaluation on execution
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-17] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __MAINFORM_H__
#define __MAINFORM_H__

#include "../DialogAgent.h"
#include "DMCore/Outputs/Output.h"

//-----------------------------------------------------------------------------
//
// D: the CMAInform class -- the microagent for Inform
//     On execution, this microagent merely outputs a prompt, and finishes
// 
//-----------------------------------------------------------------------------

class CMAInform: public CDialogAgent {

protected:
	// list of prompts planned by this agent
	vector<COutput*> voOutputs;

public:

	//---------------------------------------------------------------------
	// Constructors and Destructors
	//---------------------------------------------------------------------

	// default constructor
	CMAInform(string sAName, 
			  string sAConfiguration = "",
			  string sAType = "CAgent:CDialogAgent:CMAInform");

	// virtual destructor
	virtual ~CMAInform();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

public:

	//---------------------------------------------------------------------
	// Specialized (overwritten) Dialog Agent methods
	//---------------------------------------------------------------------

	// Function for execution: for this agent, it sends out a prompt
	// through the OutputManager
	// 
	virtual TDialogExecuteReturnCode Execute();

	// Reopens the agent
	virtual void ReOpenTopic();

	// Resets the agent
	virtual void Reset();

	// Function indicating when the agent has successfully completed
	// 
	virtual bool SuccessCriteriaSatisfied();

	// The prompt method: returns the inform prompt
    //
	virtual string Prompt();

	// Inform agents keep the floor during their whole execution
	// (i.e. until the system is done speaking the prompts)
	virtual bool RequiresFloor() {return true;}
};

//-----------------------------------------------------------------------------
//
// Specific macros to be used when definining derived dialog inform agents, in 
//  the dialog task file
//
//-----------------------------------------------------------------------------

// D: defining an inform agent
#define DEFINE_INFORM_AGENT(InformAgentClass, OTHER_CONTENTS)\
	class InformAgentClass: public CMAInform {\
	public:\
		InformAgentClass(string sAName,\
						 string sAType = "CAgent:CDialogAgent:CMAInform:"#InformAgentClass):\
            CMAInform(sAName, sAType) {;}\
		~InformAgentClass() {;}\
		static CAgent* AgentFactory(string sAName, string sAConfiguration) {\
			return new InformAgentClass(sAName, sAConfiguration);\
		}\
		OTHER_CONTENTS\
	};\
	
#endif // __MAINFORM_H__