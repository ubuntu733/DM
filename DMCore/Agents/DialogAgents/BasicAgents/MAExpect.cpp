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
// MAEXPECT.CPP - implementation of the CMAExpect class. This class 
//				   implements the microagent for Expect
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
//   [2002-03-20] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "MAExpect.h"
#include "DMCore/Core.h"

//-----------------------------------------------------------------------------
//
// D: the CMAExpect class -- the microagent for Expect
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------------------

// D: default constructor
CMAExpect::CMAExpect(string sAName, string sAConfiguration, string sAType): 
	CDialogAgent(sAName, sAConfiguration, sAType) {

	// the expect agent is always completed
	bCompleted = true;
    ctCompletionType = ctSuccess;
}

// D: virtual destructor - does nothing
CMAExpect::~CMAExpect() {
}

//-----------------------------------------------------------------------------
// D: Static function for dynamic agent creation
//-----------------------------------------------------------------------------
CAgent* CMAExpect::AgentFactory(string sAName, string sAConfiguration) {
	return new CMAExpect(sAName, sAConfiguration);
}

//-----------------------------------------------------------------------------
//
// Specialized (overwritten) CDialogAgent methods
//
//-----------------------------------------------------------------------------

// D: Preconditions function: returns false / expect agents are not to be
//    executed
bool CMAExpect::PreconditionsSatisfied() {
	return false;
}

// D: Specifies if the agent is executable
bool CMAExpect::IsExecutable() {
	return false;
}

// D: Execute function: does nothing
TDialogExecuteReturnCode CMAExpect::Execute() {
	return dercContinueExecution;
}

// D: Define the expectations: add to the list the expectations of this 
//    particular agent
int CMAExpect::DeclareExpectations(TConceptExpectationList& 
								   celExpectationList) {
	int iExpectationsAdded = 0;
	TConceptExpectationList celLocalExpectationList;
	bool bExpectCondition = ExpectCondition();

	// first get the expectations from the local "expected" concept
    string sExpectedConceptName = ExpectedConceptName();
    string sGrammarMapping = GrammarMapping();
    if(!sExpectedConceptName.empty() && !sGrammarMapping.empty())
	    parseGrammarMapping(
            C(sExpectedConceptName).GetAgentQualifiedName(), sGrammarMapping, 
		    celLocalExpectationList);

	// now go through it and add stuff to the 
	for(unsigned int i = 0; i < celLocalExpectationList.size(); i++) {
		// if the expect condition is not satisfied, disable this 
		// expectation and set the appropriate reason
		if(!bExpectCondition) {
			celLocalExpectationList[i].bDisabled = true;
			celLocalExpectationList[i].sReasonDisabled = "expect condition false";
		}
		celExpectationList.push_back(celLocalExpectationList[i]);
		iExpectationsAdded++;
	}

	// now add whatever needs to come from the CDialogAgent side
	iExpectationsAdded += 
		CDialogAgent::DeclareExpectations(celExpectationList);

	// and finally return the total number of added expectations
	return iExpectationsAdded;
}

// D: The SetCompleted() virtual function is overwritten so that the 
// agent can never be set to incompleted. Expect agents are always
// completed (they don't need to be executed)
// 
#pragma warning (disable:4100)
void CMAExpect::SetCompleted(TCompletionType ctACompletionType) {
	Error(FormatString("Expect agent %s cannot be forced to incompleted.", 
		sName.c_str()));
}
#pragma warning (default:4100)

//-----------------------------------------------------------------------------
//
// Expect Microagent specific methods
//
//-----------------------------------------------------------------------------

// D: Returns a string describing the concept mapping (nothing for this class)
//    Is to be overwritten by derived classes
string CMAExpect::GrammarMapping() {
	return "";
}

// D: Returns the name of the expected concept (nothing for this class); is to 
//    be overwritten by derived classes
string CMAExpect::ExpectedConceptName() {
	return "";
}

// D: Returns a pointer to the expected concept (uses the concept name)
CConcept* CMAExpect::ExpectedConcept() {
	return &(C(ExpectedConceptName()));
}
