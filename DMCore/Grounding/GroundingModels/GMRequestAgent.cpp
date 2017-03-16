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
// GMREQUESTAGENT.CPP - implementation of the CGMRequestAgent grounding model 
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
//   [2004-02-24] (dbohus): addeded support for full state and collapsed state
//   [2004-02-11] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "GMRequestAgent.h"
#include "DMCore/Agents/DialogAgents/BasicAgents/MARequest.h"
#include "DMCore/Log.h"

//-----------------------------------------------------------------------------
// D: Constructors, destructors
//-----------------------------------------------------------------------------

// D: default constructor, just calls upon the CGroundingModel constructor
CGMRequestAgent::CGMRequestAgent(string sAModelPolicy, string sAName): 
    CGroundingModel(sAModelPolicy, sAName) {
    pRequestAgent = NULL;
    // this model has 3 states
    bdBeliefState.Resize(3);
}

// D: constructor from reference
CGMRequestAgent::CGMRequestAgent(CGMRequestAgent& rAGMRequestAgent) {
    sModelPolicy = rAGMRequestAgent.sModelPolicy;
    sName = rAGMRequestAgent.sName;
    pPolicy = rAGMRequestAgent.pPolicy;
    viActionMappings = rAGMRequestAgent.viActionMappings;
    sExplorationMode = rAGMRequestAgent.sExplorationMode;
    fExplorationParameter = rAGMRequestAgent.fExplorationParameter;
    pRequestAgent = rAGMRequestAgent.pRequestAgent;
    stFullState = rAGMRequestAgent.stFullState;
    bdBeliefState = rAGMRequestAgent.bdBeliefState;
}

// D: destructor - does nothing so far
CGMRequestAgent::~CGMRequestAgent() {
}

//-----------------------------------------------------------------------------
// D: Grounding model factory method
//-----------------------------------------------------------------------------

CGroundingModel* CGMRequestAgent::GroundingModelFactory(string sModelPolicy) {
    return new CGMRequestAgent(sModelPolicy);
}

//-----------------------------------------------------------------------------
// D: Member access methods
//-----------------------------------------------------------------------------

// D: return the type of the grounding model (as string)
string CGMRequestAgent::GetType() {
    return "request_default";
}

// D: Set the name: block this method (the name is automatically set to the
//    agent-qualified path of the concept it grounds)
void CGMRequestAgent::SetName(string sAName) {
    // issue an error
    Error("Cannot perform SetName on a request agent grounding model.");
}

// D: Get the name of the model
string CGMRequestAgent::GetName() {
    if (pRequestAgent != NULL) {
        return pRequestAgent->GetName();
    } else {
        return "UNKNOWN";
    }
}

// D: Set the agent handled
void CGMRequestAgent::SetRequestAgent(CDialogAgent* pARequestAgent) {
    pRequestAgent = pARequestAgent;
}

// D: Get the agent handled
CDialogAgent* CGMRequestAgent::GetRequestAgent() {
    return pRequestAgent;
}

//-----------------------------------------------------------------------------
// D: Grounding model specific methods
//-----------------------------------------------------------------------------

// D: Cloning the model
CGroundingModel* CGMRequestAgent::Clone() {
    return new CGMRequestAgent(*this);
}

// D: Overwritten method for loading the model policy
// Returns false if there was a problem loading the policy.
bool CGMRequestAgent::LoadPolicy() {
    // first call the inherited LoadPolicy
    if(!CGroundingModel::LoadPolicy()) {
        return false;
    } else if(!bExternalPolicy) {
        // then check that the model has the presumed state-space
        if(pPolicy.size() != 3) {
            FatalError(FormatString("Error in CGMRequestAgent::LoadPolicy(). "\
                "Invalid state-space size (3 states expected, %d found).", 
                pPolicy.size()));
            return false;
        } else if((pPolicy[0].sStateName != SS_FAILED) ||
                  (pPolicy[1].sStateName != SS_UNDERSTANDING) ||
                  (pPolicy[2].sStateName != SS_NONUNDERSTANDING)) {
            FatalError("Error in CGMRequestAgent::LoadPolicy(). Invalid "\
                "state-space.");
            return false;
        }
    }
    return true;
}

// D: Compute the full state for this model
void CGMRequestAgent::computeFullState() {
    // clear the full state
    stFullState.Clear();
    // set the failed variable
    stFullState["failed"] = BoolToString(pRequestAgent->HasFailed());
    // set the nonunderstanding state variable
    stFullState["nonunderstanding"] = 
        BoolToString(pDMCore->LastTurnNonUnderstanding());

    // now go through the last input data and add all that to the state
    stFullState.Add(pInteractionEventManager->GetLastInput()->GetProperties());
}

// D: Compute the belief state for this model
void CGMRequestAgent::computeBeliefState() {
    // there are 3 possible states in this grounding model:
    // FAILED, NONUNDERSTANDING and UNDERSTANDING

    // check if it's failed
    if(stFullState["failed"] == "true") {
        bdBeliefState[SI_FAILED] = 1;
        bdBeliefState[SI_NONUNDERSTANDING] = 0;
        bdBeliefState[SI_UNDERSTANDING] = 0;
    } else {
        // o/w check the non-understanding/understanding        
        bdBeliefState[SI_FAILED] = 0;
        // the state is non-understanding if the last turn was a 
        // non-understanding
        bdBeliefState[SI_NONUNDERSTANDING] = 
            (stFullState["nonunderstanding"] == "true")?(float)1:(float)0;
        // the state is understanding otherwise
        bdBeliefState[SI_UNDERSTANDING] = 
            1 - bdBeliefState[SI_NONUNDERSTANDING];
    }

    // and invalidate the suggested action (not computed yet for this state)
    iSuggestedActionIndex = -1;
}

// D: Runs the action (also transmitting the concept the action is ran on
//    as a pointer)
void CGMRequestAgent::RunAction(int iActionIndex) {
    // obtains a pointer to the action from the grounding manager
    // and runs it tranmitting the concept as a parameter
    pGroundingManager->operator [](iActionIndex)->
        Run((void *)pRequestAgent);
}