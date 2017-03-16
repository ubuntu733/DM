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
// MAREQUEST.H - implementation of the CMARequest class. This class implements 
//               the microagent for Request
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
//                 antoine): the request agent doesn't check for completion on 
//                            execute any more. the core takes care of that
//   [2003-04-08] (dbohus):  change completion evaluation on execution
//   [2002-11-20] (dbohus):  changed behavior so that it's an "open request" if
//                            no requested concept is specified
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-21] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __MAREQUEST_H__
#define __MAREQUEST_H__

#include "../DialogAgent.h"
#include "DMCore/Outputs/Output.h"

//-----------------------------------------------------------------------------
//
// D: the CMARequest class -- the microagent for Request, On execution, this 
//     microagent outputs a request, and expects a concept
// 
//-----------------------------------------------------------------------------

class CMARequest: public CDialogAgent {

protected:
	// list of prompts planned by this agent
	vector<COutput*> voOutputs;

	// the input index at the time of the agent's latest execution
	int iInputTurnNumberAtExecution;

public:
	//---------------------------------------------------------------------
	// Constructors and Destructors
	//---------------------------------------------------------------------

	// default constructor
	CMARequest(string sAName, 
			   string sAConfiguration = "",
			   string sAType = "CAgent:CDialogAgent:CMARequest");

	// virtual destructor
	virtual ~CMARequest();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

public:

	//---------------------------------------------------------------------
	// Specialized (overwritten) Dialog Agent methods
	//---------------------------------------------------------------------

	// Function for execution. It first outputs the request prompt, then 
	// it waits for an answer
	// 
	virtual TDialogExecuteReturnCode Execute();

	// Resets the agent
	virtual void Reset();

	// Function for defining the list of concepts that the dialog agent 
	// expects at a certain point. 
	// 
	virtual int DeclareExpectations(TConceptExpectationList&
									celExpectationList); 

	// Function implementing the precondition for execution: the default
	// precondition for request agents is that the concept is not already
	// available
	//
	virtual bool PreconditionsSatisfied();

	// Function indicating when the agent has successfully completed. Request 
	// agents complete once the concept requested has been grounded
	// 
	virtual bool SuccessCriteriaSatisfied();

	// Function indicating when the agent has completed without achieving its
	// goal. 
	virtual bool FailureCriteriaSatisfied();

	// Request agents keep the floor during their whole execution
	// (i.e. until the system is done speaking the prompts)
	virtual bool RequiresFloor() {return true;}


	// The prompt method: returns the prompt for the request agent
    // 
	virtual string Prompt();

public:
	//---------------------------------------------------------------------
	// Request Microagent specific methods
	//---------------------------------------------------------------------
	
	// Returns the concept mapping. This string describes how the grammar 
	// concepts in the input map into values for the concept. The string is 
	// used by DeclareExpectations to construct the actual grammar concept 
	// expectations
	virtual string GrammarMapping();

	// Returns the name of the requested concept
	virtual string RequestedConceptName();
		
	// Returns a pointer to the expected concept
	virtual CConcept* RequestedConcept();

	// Returns the duration of the timeout in milliseconds
	virtual int GetTimeoutPeriod();

    // Returns the non-understanding threshold
    virtual float GetNonunderstandingThreshold();
};


//-----------------------------------------------------------------------------
//
// Specific macros to be used when definining derived dialog request agents, in 
//  the dialog task file
//
//-----------------------------------------------------------------------------

// D: defining a request agent
#define DEFINE_REQUEST_AGENT(RequestAgentClass, OTHER_CONTENTS)\
	class RequestAgentClass: public CMARequest {\
	public:\
		RequestAgentClass(string sAName,\
						  string sAType = "CAgent:CDialogAgent:CMARequest:"#RequestAgentClass): \
             CMARequest(sAName, sAType) {;}\
		~RequestAgentClass() {;}\
		static CAgent* AgentFactory(string sAName, string sAConfiguration) {\
			return new RequestAgentClass(sAName, sAConfiguration);\
		}\
		OTHER_CONTENTS\
	};\

// D: defining the concept of a request agent
#define REQUEST_CONCEPT(ConceptName)\
	public:\
	virtual string RequestedConceptName() {\
        return(ReplaceSubString(#ConceptName, "#", GetDynamicAgentID()));\
	};\
	
// D: defining the concept mapping used by the request agent
#define GRAMMAR_MAPPING(GrammarMappingAsString)\
	public:\
	virtual string GrammarMapping() {\
		return (string)(GrammarMappingAsString);\
	};\

// D: defining the timeout duration for the request agent in seconds
#define TIMEOUT_PERIOD(Period)\
	public:\
	virtual int GetTimeoutPeriod() {\
		return(Period);\
	};\

// D: defining the rejection/nonundersatnding threhold
#define NONUNDERSTANDING_THRESHOLD(Threshold)\
    public:\
    virtual float GetNonunderstandingThreshold() {\
        return (Threshold);\
    };\

#endif // __MAREQUEST_H__