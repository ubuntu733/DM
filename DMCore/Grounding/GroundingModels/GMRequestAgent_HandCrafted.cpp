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
// GROUNDINGMODEL.CPP - implementation of the CGroundingModel base class for
//                      grounding models
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
//   [2008-06-03] (antoine): started working on this based on GMRequestAgent
// 
//-----------------------------------------------------------------------------


#include "GroundingModel.h"
#include "GMRequestAgent_HandCrafted.h"
#include "DMCore/Core.h"

//---------------------------------------------------------------------
//
// A: CCondition : a class containing a condition on a feature 
//                (e.g. "confidence > 0.3")
//
//---------------------------------------------------------------------

CCondition::CCondition(string sAFeature, string sATestValue, 
					   TValueType vtAValueType,	TComparator cAComparator) {
	sFeature = sAFeature;
	sTestValue = sATestValue;
	vtValueType = vtAValueType;
	cComparator = cAComparator;
}

// Constructs a CCondition object from a string representation
CCondition::CCondition(string sCondStr) {
	
	vector<string> vsStrings = PartitionString(sCondStr, " ");
	sFeature = vsStrings[0];
	sTestValue = vsStrings[2];
	if ((vsStrings[1] == "=") || (vsStrings[1] == "==")) {
		cComparator = EqualTo;
		if (IsANumber(sTestValue)) {
			vtValueType = Number;
		} else {
			vtValueType = String;
		}
	} else if (vsStrings[1] == "!=") {
		cComparator = NotEqualTo;
		if (IsANumber(sTestValue)) {
			vtValueType = Number;
		} else {
			vtValueType = String;
		}
	} else if (vsStrings[1] == "<") {
		cComparator = LessThan;
		vtValueType = Number;
	} else if (vsStrings[1] == ">") {
		cComparator = GreaterThan;
		vtValueType = Number;
	} else if (vsStrings[1] == "<=") {
		cComparator = LessThanOrEqualTo;
		vtValueType = Number;
	} else if (vsStrings[1] == ">=") {
		cComparator = GreaterThanOrEqualTo;
		vtValueType = Number;
	}	
}


bool CCondition::Matches(CState &sState) {
	
	string sValue = sState[sFeature];
	if (sValue.empty()) {
		Log(WARNING_STREAM, "Feature %s not found.", sFeature.c_str());
		return false;
	}

	if (vtValueType == Number) {
		// Perform numeric comparison
		double dValue = atof(sValue.c_str());
		double dTest = atof(sTestValue.c_str());
		if (cComparator == EqualTo)
			return dValue == dTest;
		else if (cComparator == NotEqualTo)
			return dValue != dTest;
		else if (cComparator == LessThan)
			return dValue < dTest;
		else if (cComparator == GreaterThan)
			return dValue > dTest;
		else if (cComparator == LessThanOrEqualTo)
			return dValue <= dTest;
		else if (cComparator == GreaterThanOrEqualTo)
			return dValue >= dTest;
	} else if (vtValueType == String) {
		// Peform string comparison
		if (cComparator == EqualTo)
			return sValue == sTestValue;
		else if (cComparator == NotEqualTo)
			return sValue != sTestValue;
	}

	Log(WARNING_STREAM, "Error when testing for feature %s (%s ? %s), "
		"returning false", sFeature.c_str(), sFeature.c_str(), sValue.c_str());
	return false;
}

//---------------------------------------------------------------------
//
// A: CGMState : a class containing an abstract dialog state 
//               (i.e. a conjunction of conditions)
//
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// A: Constructors, Destructors
//---------------------------------------------------------------------

CGMState::CGMState(string sAName) {
	sName = sAName;
	vcConditions.clear();
}

CGMState::~CGMState() {}

//---------------------------------------------------------------------
// A: Public methods
//---------------------------------------------------------------------

void CGMState::AddCondition(CCondition &cCondition) {
	vcConditions.push_back(cCondition);
}

bool CGMState::Matches(CState &sState) {
	for (unsigned int i = 0; i < vcConditions.size(); i++) {
		if (!vcConditions[i].Matches(sState))
			return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// A: Constructors and destructors
//-----------------------------------------------------------------------------

// A: default constructor, calls upon the CGMRequestAgent constructor
CGMRequestAgent_HandCrafted::CGMRequestAgent_HandCrafted(string sAModelPolicy, 
														 string sAName) :
CGMRequestAgent(sAModelPolicy, sAName)  {

	bdBeliefState.Resize(0);
	vsStates.clear();

}

// A: constructor from reference
CGMRequestAgent_HandCrafted::CGMRequestAgent_HandCrafted(
	CGMRequestAgent_HandCrafted &rAGMRequestAgent_HandCrafted) {
    sModelPolicy = rAGMRequestAgent_HandCrafted.sModelPolicy;
    sName = rAGMRequestAgent_HandCrafted.sName;
    pPolicy = rAGMRequestAgent_HandCrafted.pPolicy;
    viActionMappings = rAGMRequestAgent_HandCrafted.viActionMappings;
    sExplorationMode = rAGMRequestAgent_HandCrafted.sExplorationMode;
    fExplorationParameter = rAGMRequestAgent_HandCrafted.fExplorationParameter;
    pRequestAgent = rAGMRequestAgent_HandCrafted.pRequestAgent;
    stFullState = rAGMRequestAgent_HandCrafted.stFullState;
    bdBeliefState = rAGMRequestAgent_HandCrafted.bdBeliefState;
	vsStates = rAGMRequestAgent_HandCrafted.vsStates;
}

CGMRequestAgent_HandCrafted::~CGMRequestAgent_HandCrafted() {
}

//-----------------------------------------------------------------------------
// A: Grounding model factory method
//-----------------------------------------------------------------------------

CGroundingModel* CGMRequestAgent_HandCrafted::GroundingModelFactory(string sModelPolicy) {
    return new CGMRequestAgent_HandCrafted(sModelPolicy);
}

//-----------------------------------------------------------------------------
// A: Member access methods
//-----------------------------------------------------------------------------

// A: return the type of the grounding model (as string)
string CGMRequestAgent_HandCrafted::GetType() {
    return "request_handcrafted";
}

//-----------------------------------------------------------------------------
// A: Grounding model specific methods
//-----------------------------------------------------------------------------

// A: Cloning the model
CGroundingModel* CGMRequestAgent_HandCrafted::Clone() {
    return new CGMRequestAgent_HandCrafted(*this);
}

// A: Loads the model policy (from the grounding manager agent)
bool CGMRequestAgent_HandCrafted::LoadPolicy() {
    
	enum {
		Header,
		StateDefinitions,
		ValueMatrix} sSection = Header;
	
	// get the string data
    string sData = pGroundingManager->GetPolicy(sModelPolicy);

    // parse it - first break it into lines
    TStringVector vsLines = PartitionString(sData, "\n");
    unsigned int i = 0;
    while(i < vsLines.size()) {
        // check that it's not a commentary or an empty line
        if(vsLines[i].empty() || (vsLines[i][0] == '#') || 
            (vsLines[i][0] == '/') || (vsLines[i][0] == '%')) {
            i++;
            continue;
        }

        // Read the header (defining exlporation mode, etc)
		if (sSection == Header) {
            // read a line in the format EXPLORATION_MODE=...
            // or external_policy_host=...
			if (vsLines[i].substr(0,6) == "STATE[") {
				sSection = StateDefinitions;
			} else {
				string sTemp1, sTemp2;
				SplitOnFirst(vsLines[i], "=", sTemp1, sTemp2);
				if(ToLowerCase(Trim(sTemp1)) == "exploration_mode") {
					// set the exploration mode
					sExplorationMode = ToLowerCase(Trim(sTemp2));
					i++;
					continue;
				} else if(ToLowerCase(Trim(sTemp1)) == "exploration_parameter") {
					// set the exploration parameter
					fExplorationParameter = (float)atof(Trim(sTemp2).c_str());
					i++;
					continue;
				}
			}
		}

		// Read the state definitions
		if (sSection == StateDefinitions) {
			if (vsLines[i].substr(0,6) != "STATE[") {
				sSection = ValueMatrix;
			} else {
				string sStateName = vsLines[i].substr(6,vsLines[i].length()-7);
				CGMState *pgmsState = new CGMState(sStateName);
				i++;
				while ((i < vsLines.size()) && (vsLines[i] != "END")) {
					CCondition *pcCondition = new CCondition(vsLines[i]);
					pgmsState->AddCondition(*pcCondition);
					i++;
				}
				vsStates.push_back(*pgmsState);
				i++;
				continue;
			}
		} 

		// Read the value matrix
		if (sSection == ValueMatrix) {
			// obtain the list of actions
			TStringVector vsActions = PartitionString(vsLines[i], " \t");
			// check that there are some actions in the model
			if(vsActions.size() < 1) return false;
			// construct the action index vector
			for(unsigned int a = 0; a < vsActions.size(); a++)
				viActionMappings.push_back(
					pGroundingManager->GroundingActionNameToIndex(vsActions[a]));

			i++;
			while (i < vsLines.size()) {
				// if it's not the first line, then it will be a line containing a 
				// state and the values
				TStringVector vsValues = PartitionString(vsLines[i], " \t");
				// check that there's enough values
				if(vsValues.size() != viActionMappings.size() + 1) 
					return false;
	            
				// construct the state-action-utility datastructure
				TStateActionsValues savData;
				savData.sStateName = vsValues[0];
				for(unsigned int a = 1; a < vsValues.size(); a++) {
					if(vsValues[a] == "-") {
						// check if we have an unavailable action
						savData.i2fActionsValues[a-1] = 
							(float)UNAVAILABLE_ACTION;
					} else {
						// o/w get the utility
						savData.i2fActionsValues[a-1] = 
							(float)atof(vsValues[a].c_str());
					}
				}

				// push it in the policy
				pPolicy.push_back(savData);

				i++;
			}
       }
       i++;
    }

    // resize bdActionValues and bdBeliefStates accordingly
	bdActionValues.Resize(viActionMappings.size());
	bdBeliefState.Resize(vsStates.size());
	
    return true;
}

// D: Compute the full state for this model
void CGMRequestAgent_HandCrafted::computeFullState() {
    // clear the full state
    stFullState.Clear();

    // set the failed variable
    stFullState["failed"] = BoolToString(pRequestAgent->HasFailed());

	// now go through the last input data and add all that to the state
    stFullState.Add(pInteractionEventManager->GetLastInput()->GetProperties());
}

// D: Compute the belief state for this model
void CGMRequestAgent_HandCrafted::computeBeliefState() {

	// check to find which of the developer-defined states
	// match the current full state
	for (unsigned int i = 0; i < vsStates.size(); i++) {
		if (vsStates[i].Matches(stFullState)) {
			bdBeliefState[i] = 1;
		} else {
			bdBeliefState[i] = 0;
		}
	}

    // and invalidate the suggested action (not computed yet for this state)
    iSuggestedActionIndex = -1;
}

