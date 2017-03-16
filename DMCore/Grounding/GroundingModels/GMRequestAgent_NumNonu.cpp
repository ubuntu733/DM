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
// GMREQUESTAGENT_NUMNONU.CPP - implementation of the CGMRequestAgent grounding model 
//                      class turn-level grouding in request agents
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
//   [2006-01-31] (dbohus): added support for dynamically registering grounding
//                          model types
//   [2004-12-29] (antoine): started working on this based on GMRequestAgent
// 
//-----------------------------------------------------------------------------

#include "GMRequestAgent_NumNonu.h"
#include "DMCore/Agents/DialogAgents/BasicAgents/MARequest.h"
#include "DMCore/Log.h"

//-----------------------------------------------------------------------------
// A: Constructors, destructors
//-----------------------------------------------------------------------------

// A: default constructor, just calls upon the CGroundingModel constructor
CGMRequestAgent_NumNonu::CGMRequestAgent_NumNonu(string sAModelPolicy, 
    string sAName): CGMRequestAgent(sAModelPolicy, sAName) {
    // this model has 6 states
    bdBeliefState.Resize(6);
}

// A: constructor from reference
CGMRequestAgent_NumNonu::CGMRequestAgent_NumNonu(CGMRequestAgent_NumNonu& 
    rAGMRequestAgent_NumNonu) {
    
    sModelPolicy = rAGMRequestAgent_NumNonu.sModelPolicy;
    sName = rAGMRequestAgent_NumNonu.sName;
    pPolicy = rAGMRequestAgent_NumNonu.pPolicy;
    viActionMappings = rAGMRequestAgent_NumNonu.viActionMappings;
    sExplorationMode = rAGMRequestAgent_NumNonu.sExplorationMode;
    fExplorationParameter = rAGMRequestAgent_NumNonu.fExplorationParameter;
    pRequestAgent = rAGMRequestAgent_NumNonu.pRequestAgent;
    stFullState = rAGMRequestAgent_NumNonu.stFullState;
    bdBeliefState = rAGMRequestAgent_NumNonu.bdBeliefState;
}

// A: destructor - does nothing so far
CGMRequestAgent_NumNonu::~CGMRequestAgent_NumNonu() {
}

//-----------------------------------------------------------------------------
// D: Grounding model factory method
//-----------------------------------------------------------------------------

CGroundingModel* CGMRequestAgent_NumNonu::GroundingModelFactory(
    string sModelPolicy) {
    return new CGMRequestAgent_NumNonu(sModelPolicy);
}

//-----------------------------------------------------------------------------
// D: Member access methods
//-----------------------------------------------------------------------------

// Access to type
string CGMRequestAgent_NumNonu::GetType() {
    return "request_numnonu";
}

//-----------------------------------------------------------------------------
// A: Grounding model specific methods
//-----------------------------------------------------------------------------

// A: Cloning the model
CGroundingModel* CGMRequestAgent_NumNonu::Clone() {
    return new CGMRequestAgent_NumNonu(*this);
}

// A: Overwritten method for loading the model policy
bool CGMRequestAgent_NumNonu::LoadPolicy() {
    // first call the inherited LoadPolicy
    if(!CGroundingModel::LoadPolicy()) {
        return false;
    } else if(!bExternalPolicy) {
        // then check that the model has the presumed state-space
        if(pPolicy.size() != 6) {
            FatalError(FormatString("Error in CGMRequestAgent_NumNonu::LoadPolicy(). "\
                "Invalid state-space size (6 states expected, %d found).", 
                pPolicy.size()));
            return false;
        } else if((pPolicy[0].sStateName != SS_FAILED) ||
                  (pPolicy[1].sStateName != SS_UNDERSTANDING) ||
                  (pPolicy[2].sStateName != SS_VERY_FIRST_NONUNDERSTANDING) ||
                  (pPolicy[3].sStateName != SS_NONUNDERSTANDING_1) ||
                  (pPolicy[4].sStateName != SS_NONUNDERSTANDING_2) ||
                  (pPolicy[5].sStateName != SS_NONUNDERSTANDING_MORE)) {
            FatalError(FormatString("Error in CGMRequestAgent_NumNonu::LoadPolicy(). Invalid "\
                "state-space. [%s]", pPolicy[2].sStateName.c_str()).c_str());
            return false;
        }
    }
    return true;
}

// D: Compute the full state for this model
void CGMRequestAgent_NumNonu::computeFullState() {
    // clear the full state
    stFullState.Clear();
    // set the failed variable
    stFullState["failed"] = BoolToString(pRequestAgent->HasFailed());

    // set the number of consecutive nonunderstandings variable
	stFullState["total_num_nonunderstandings"] = IntToString(pDMCore->GetTotalNumberNonUnderstandings());

    // set the number of consecutive nonunderstandings variable
	stFullState["num_nonunderstandings"] = IntToString(pDMCore->GetNumberNonUnderstandings());

    // now go through the last input data and add all that to the state
    stFullState.Add(pInteractionEventManager->GetLastInput()->GetProperties());
}

// D: Compute the belief state for this model
void CGMRequestAgent_NumNonu::computeBeliefState() {
    // there are 3 possible states in this grounding model:
    // FAILED, NONUNDERSTANDING and UNDERSTANDING

    // check if it's failed
    if(stFullState["failed"] == "true") {
        bdBeliefState[SI_FAILED] = 1;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE] = 0;
        bdBeliefState[SI_UNDERSTANDING] = 0;
    } else if (atoi(stFullState["num_nonunderstandings"].c_str()) >= 3) {
        bdBeliefState[SI_FAILED] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE] = 1;
        bdBeliefState[SI_UNDERSTANDING] = 0;
    } else if (stFullState["num_nonunderstandings"] == "2") {
        bdBeliefState[SI_FAILED] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2] = 1;
        bdBeliefState[SI_NONUNDERSTANDING_MORE] = 0;
        bdBeliefState[SI_UNDERSTANDING] = 0;
    } else if (stFullState["num_nonunderstandings"] == "1") {
		if (stFullState["total_num_nonunderstandings"] == "1") {
			bdBeliefState[SI_FAILED] = 0;
			bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING] = 1;
			bdBeliefState[SI_NONUNDERSTANDING_1] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_2] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_MORE] = 0;
			bdBeliefState[SI_UNDERSTANDING] = 0;
		}
		else {
			bdBeliefState[SI_FAILED] = 0;
			bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_1] = 1;
			bdBeliefState[SI_NONUNDERSTANDING_2] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_MORE] = 0;
			bdBeliefState[SI_UNDERSTANDING] = 0;
		}
	} else {
        bdBeliefState[SI_FAILED] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE] = 0;
        bdBeliefState[SI_UNDERSTANDING] = 1;
    }

    // and invalidate the suggested action (not computed yet for this state)
    iSuggestedActionIndex = -1;
}
