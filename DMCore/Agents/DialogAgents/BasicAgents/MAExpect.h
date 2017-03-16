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
// MAEXPECT.H - implementation of the CMAExpect class. This class implements 
//               the microagent for Expect
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
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-03-21] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __MAEXPECT_H__
#define __MAEXPECT_H__

#include "../DialogAgent.h"

//-----------------------------------------------------------------------------
//
// D: the CMAExpect class -- the microagent for Expect
//     On execution, this microagent does not do anything. However, it will
//     define its own expectation in terms of concepts, and can lead to 
//     new concept bindings
// 
//-----------------------------------------------------------------------------

class CMAExpect: public CDialogAgent {

public:
	//---------------------------------------------------------------------
	// Constructors and Destructors
	//---------------------------------------------------------------------

	// default constructor
	CMAExpect(string sAName,
			  string sAConfiguration,
			  string sAType = "CAgent:CDialogAgent:CMAExpect");

	// virtual destructor
	virtual ~CMAExpect();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

public:

	//---------------------------------------------------------------------
	// Specialized (overwritten) Dialog Agent methods
	//---------------------------------------------------------------------

	// Function implementing the precondition for execution
	// of that agent. For this class, it always returns false, since expect
	// agents are never to be executed
	virtual bool PreconditionsSatisfied();

	// Function which specifies if the dialog agent is executable (in this
	// case returns false)
	virtual bool IsExecutable();

	// Function for execution. Although it does nothing (Expect agents are
	// not to be directly executed), it needs to be defined so that the
	// class does not remain abstract.
	virtual TDialogExecuteReturnCode Execute();

	// Function for defining the list of concepts that the dialog agent 
	// expects at a certain point. 
	virtual int DeclareExpectations(TConceptExpectationList&
									celExpectationList); 

	// The SetCompleted() virtual function is overwritten so that the 
	// agent can never be set to incompleted. Expect agents are always
	// completed (they don't need to be executed)
	virtual void SetCompleted(TCompletionType ctACompletionType = ctSuccess);

public:
	//---------------------------------------------------------------------
	// Expect Microagent specific methods
	//---------------------------------------------------------------------

	// Returns the concept mapping. This string describes how the grammar 
	// concepts in the input map into values for the concept. The string is 
	// used by DeclareExpectations to construct the actual grammar concept 
	// expectations
	//
	virtual string GrammarMapping();

	// Returns the name of the expected concept
	//
	virtual string ExpectedConceptName();

	// Returns a pointer to the expected concept
	//
	virtual CConcept* ExpectedConcept();

};


//-----------------------------------------------------------------------------
//
// Specific macros to be used when definining derived dialog expect agents, in 
//  the dialog task file
//
//-----------------------------------------------------------------------------

// D: defining an expect agent
#define DEFINE_EXPECT_AGENT(ExpectAgentClass, OTHER_CONTENTS)\
	class ExpectAgentClass: public CMAExpect {\
	public:\
		ExpectAgentClass(string sAName,\
						 string sAType = "CAgent:CDialogAgent:CMAExpect:"#ExpectAgentClass):\
            CMAExpect(sAName, sAType) {;}\
		~ExpectAgentClass() {;}\
		static CAgent* AgentFactory(string sAName, string sAConfiguration) {\
			return new ExpectAgentClass(sAName, sAConfiguration);\
		}\
		OTHER_CONTENTS\
	};\

// D: defining the concept of an expect agent
#define EXPECT_CONCEPT(ConceptName)\
	public:\
	virtual string ExpectedConceptName() {\
    return(#ConceptName);\
	};\

// D: defining the concept mapping used by the expectation agent
#define GRAMMAR_MAPPING(GrammarMappingAsString)\
	public:\
	virtual string GrammarMapping() {\
		return (string)(GrammarMappingAsString);\
	};\


#endif // __MAEXPECT_H__