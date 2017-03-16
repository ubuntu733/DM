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
// GMREQUESTAGENT_EXPERIMENT.CPP - implementation of the CGMRequestAgent 
//                      grounding model class turn-level grouding in request 
//                      agents
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

#include "GMRequestAgent_Experiment.h"
#include "DMCore/Agents/DialogAgents/BasicAgents/MARequest.h"
#include "DMCore/Log.h"

//-----------------------------------------------------------------------------
// A: Constructors, destructors
//-----------------------------------------------------------------------------

// A: default constructor, just calls upon the CGroundingModel constructor
CGMRequestAgent_Experiment::CGMRequestAgent_Experiment(string sAModelPolicy, 
    string sAName): CGMRequestAgent(sAModelPolicy, sAName) {
    // this model has 5 states
    bdBeliefState.Resize(5);
}

// A: constructor from reference
CGMRequestAgent_Experiment::CGMRequestAgent_Experiment(
    CGMRequestAgent_Experiment& rAGMRequestAgent_Experiment) {
    
    sModelPolicy = rAGMRequestAgent_Experiment.sModelPolicy;
    sName = rAGMRequestAgent_Experiment.sName;
    pPolicy = rAGMRequestAgent_Experiment.pPolicy;
    viActionMappings = rAGMRequestAgent_Experiment.viActionMappings;
    sExplorationMode = rAGMRequestAgent_Experiment.sExplorationMode;
    fExplorationParameter = rAGMRequestAgent_Experiment.fExplorationParameter;
    pRequestAgent = rAGMRequestAgent_Experiment.pRequestAgent;
    stFullState = rAGMRequestAgent_Experiment.stFullState;
    bdBeliefState = rAGMRequestAgent_Experiment.bdBeliefState;
}

// A: destructor - does nothing so far
CGMRequestAgent_Experiment::~CGMRequestAgent_Experiment() {
}

//-----------------------------------------------------------------------------
// D: Grounding model factory method
//-----------------------------------------------------------------------------

CGroundingModel* CGMRequestAgent_Experiment::GroundingModelFactory(
    string sModelPolicy) {
    return new CGMRequestAgent_Experiment(sModelPolicy);
}

//-----------------------------------------------------------------------------
// D: Member access methods
//-----------------------------------------------------------------------------

// D: Access to type
string CGMRequestAgent_Experiment::GetType() {
    return "request_experiment";
}

//-----------------------------------------------------------------------------
// A: Grounding model specific methods
//-----------------------------------------------------------------------------

// A: Cloning the model
CGroundingModel* CGMRequestAgent_Experiment::Clone() {
    return new CGMRequestAgent_Experiment(*this);
}

// A: Overwritten method for loading the model policy
bool CGMRequestAgent_Experiment::LoadPolicy() {
    // first call the inherited LoadPolicy
    if(!CGroundingModel::LoadPolicy()) {
        return false;
    } else if(!bExternalPolicy) {
        // then check that the model has the presumed state-space
        if(pPolicy.size() != 10) {
            FatalError(FormatString("Error in CGMRequestAgent_Experiment::LoadPolicy(). "\
                "Invalid state-space size (10 states expected, %d found).", 
                pPolicy.size()));
            return false;
        } else if((pPolicy[0].sStateName != SS_FAILED) ||
                  (pPolicy[1].sStateName != SS_UNDERSTANDING) ||
                  (pPolicy[2].sStateName != SS_VERY_FIRST_NONUNDERSTANDING_CTL) ||
                  (pPolicy[3].sStateName != SS_NONUNDERSTANDING_1_CTL) ||
                  (pPolicy[4].sStateName != SS_NONUNDERSTANDING_2_CTL) ||
                  (pPolicy[5].sStateName != SS_NONUNDERSTANDING_MORE_CTL) ||
				  (pPolicy[6].sStateName != SS_VERY_FIRST_NONUNDERSTANDING_EXP) ||
                  (pPolicy[7].sStateName != SS_NONUNDERSTANDING_1_EXP) ||
                  (pPolicy[8].sStateName != SS_NONUNDERSTANDING_2_EXP) ||
                  (pPolicy[9].sStateName != SS_NONUNDERSTANDING_MORE_EXP)) {
            FatalError(FormatString("Error in CGMRequestAgent_Experiment::LoadPolicy(). Invalid "\
                "state-space. [%s]", pPolicy[2].sStateName.c_str()).c_str());
            return false;
        }
    }
    return true;
}

// D: Compute the full state for this model
void CGMRequestAgent_Experiment::computeFullState() {
    // clear the full state
    stFullState.Clear();
    // set the failed variable
    stFullState["failed"] = BoolToString(pRequestAgent->HasFailed());

	// gets the condition (control or experimental) from the grounding manager's
	// configuration hash
	if (pGroundingManager->HasParameter("condition")) {
		stFullState["condition"] = pGroundingManager->GetParameterValue("condition");
	} else {
		stFullState["condition"] = "CTL";	
	}

    // set the number of consecutive nonunderstandings variable
	stFullState["total_num_nonunderstandings"] = IntToString(pDMCore->GetTotalNumberNonUnderstandings());

    // set the number of consecutive nonunderstandings variable
	stFullState["num_nonunderstandings"] = IntToString(pDMCore->GetNumberNonUnderstandings());

    // now go through the last input data and add all that to the state
    stFullState.Add(pInteractionEventManager->GetLastInput()->GetProperties());
}

// D: Compute the belief state for this model
void CGMRequestAgent_Experiment::computeBeliefState() {
    // check if it's failed
    if(stFullState["failed"] == "true") {
        bdBeliefState[SI_FAILED] = 1;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE_CTL] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1_EXP] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2_EXP] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE_EXP] = 0;
        bdBeliefState[SI_UNDERSTANDING] = 0;
    } else if (atoi(stFullState["num_nonunderstandings"].c_str()) >= 3) {
        bdBeliefState[SI_FAILED] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2_CTL] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_EXP] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1_EXP] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2_EXP] = 0;
        bdBeliefState[SI_UNDERSTANDING] = 0;
		if (stFullState["condition"] == "CTL") {
	        bdBeliefState[SI_NONUNDERSTANDING_MORE_CTL] = 1;
			bdBeliefState[SI_NONUNDERSTANDING_MORE_EXP] = 0;
		} else {
	        bdBeliefState[SI_NONUNDERSTANDING_MORE_CTL] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_MORE_EXP] = 1;
		}
    } else if (stFullState["num_nonunderstandings"] == "2") {
        bdBeliefState[SI_FAILED] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE_CTL] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_EXP] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1_EXP] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE_EXP] = 0;
        bdBeliefState[SI_UNDERSTANDING] = 0;
		if (stFullState["condition"] == "CTL") {
	        bdBeliefState[SI_NONUNDERSTANDING_2_CTL] = 1;
			bdBeliefState[SI_NONUNDERSTANDING_2_EXP] = 0;
		} else {
	        bdBeliefState[SI_NONUNDERSTANDING_2_CTL] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_2_EXP] = 1;
		}
    } else if (stFullState["num_nonunderstandings"] == "1") {
		if (stFullState["total_num_nonunderstandings"] == "1") {
			bdBeliefState[SI_FAILED] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_1_CTL] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_2_CTL] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_MORE_CTL] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_1_EXP] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_2_EXP] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_MORE_EXP] = 0;
			bdBeliefState[SI_UNDERSTANDING] = 0;
			if (stFullState["condition"] == "CTL") {
				bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 1;
				bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_EXP] = 0;
			} else {
				bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 0;
				bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_EXP] = 1;
			}
		}
		else {
			bdBeliefState[SI_FAILED] = 0;
			bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_2_CTL] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_MORE_CTL] = 0;
			bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_EXP] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_2_EXP] = 0;
			bdBeliefState[SI_NONUNDERSTANDING_MORE_EXP] = 0;
			bdBeliefState[SI_UNDERSTANDING] = 0;
			if (stFullState["condition"] == "CTL") {
				bdBeliefState[SI_NONUNDERSTANDING_1_CTL] = 1;
				bdBeliefState[SI_NONUNDERSTANDING_1_EXP] = 0;
			} else {
				bdBeliefState[SI_NONUNDERSTANDING_1_CTL] = 0;
				bdBeliefState[SI_NONUNDERSTANDING_1_EXP] = 1;
			}
		}
	} else {
        bdBeliefState[SI_FAILED] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE_CTL] = 0;
        bdBeliefState[SI_VERY_FIRST_NONUNDERSTANDING_CTL] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_1_EXP] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_2_EXP] = 0;
        bdBeliefState[SI_NONUNDERSTANDING_MORE_EXP] = 0;
        bdBeliefState[SI_UNDERSTANDING] = 1;
    }

    // and invalidate the suggested action (not computed yet for this state)
    iSuggestedActionIndex = -1;
}
