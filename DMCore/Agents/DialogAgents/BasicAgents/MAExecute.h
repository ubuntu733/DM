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
// MAEXECUTE.H   - definition of the CMAExecute class. This class 
//				   implements the microagent for Execute
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
//   [2004-04-16] (dbohus):  added grounding models on dialog agents
//   [2003-04-25] (dbohus,
//                 antoine): the execute agent doesn't check for completion on 
//                            execute any more. the core takes care of that
//   [2003-04-10] (dbohus):  fixed execute call on empty call
//   [2003-04-08] (dbohus):  change completion evaluation on execution
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-04-05] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __MAEXECUTE_H__
#define __MAEXECUTE_H__

#include "../DialogAgent.h"

//-----------------------------------------------------------------------------
//
// D: the CMAExecute class -- the microagent for Execute, On execution, this 
//     microagent executes a call to another agent, as described by the 
//     ExecuteCall function. Derived classes can implement anything they 
//     want in the execution section of this agent. 
// 
//-----------------------------------------------------------------------------

class CMAExecute: public CDialogAgent {

public:
	//---------------------------------------------------------------------
	// Constructors and Destructors
	//---------------------------------------------------------------------

	// default constructor
	CMAExecute(string sAName, 
			   string sAConfiguration = "",
			   string sAType = "CAgent:CDialogAgent:CMAExecute");

	// virtual destructor
	virtual ~CMAExecute();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

public:

	//---------------------------------------------------------------------
	// Specialized (overwritten) Dialog Agent methods
	//---------------------------------------------------------------------

	// Function for execution. It executes the call given by ExecuteCall()
	// 
	virtual TDialogExecuteReturnCode Execute();

	// Function indicating when the agent has completed. Execute agents 
	// complete as soon as they have executed once
	// 
	virtual bool SuccessCriteriaSatisfied();


public:
	//---------------------------------------------------------------------
	// Execute Microagent specific methods
	//---------------------------------------------------------------------
	
	// The actual Execution routine for the execute agent (this gets 
	// called by Execute). By default, it executes a galaxy server call 
	// obtained as a string from ExecuteCall
	virtual void ExecuteRoutine();

	// Returns a string describing a galaxy call to execute 
	//
	virtual string GetExecuteCall();

};


//-----------------------------------------------------------------------------
//
// Specific macros to be used when definining derived execute agents, in 
//  the dialog task file
//
//-----------------------------------------------------------------------------

// D: defining an execute agent
#define DEFINE_EXECUTE_AGENT(ExecuteAgentClass, OTHER_CONTENTS)\
	class ExecuteAgentClass: public CMAExecute {\
	public:\
		ExecuteAgentClass(string sAName,\
						  string sAType = "CAgent:CDialogAgent:CMAExecute:"#ExecuteAgentClass):\
            CMAExecute(sAName, sAType) {;}\
		~ExecuteAgentClass() {;}\
		static CAgent* AgentFactory(string sAName, string sAConfiguration) {\
			return new ExecuteAgentClass(sAName, sAConfiguration);\
		}\
		OTHER_CONTENTS\
	};\

// D: defining the execute routine of an execute agent
#define EXECUTE(CONTENTS)\
	public:\
	virtual void ExecuteRoutine() {\
		CONTENTS;\
	}\

// D: defining the call of an execute agent (within an execute agent definition)
#define CALL(String)\
	public:\
	virtual string GetExecuteCall() {\
		return String;\
	};\


#endif // __MAEXECUTE_H__