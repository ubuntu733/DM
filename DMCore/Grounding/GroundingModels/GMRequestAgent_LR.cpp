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
// GMREQUESTAGENT_LR.H - definition of the CGMRequestAgent_LR grounding model 
//                       class: this model takes actions based on computing
//                       the probability of success for each action according
//                       to a set of features. 
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
//   [2005-12-14] (dbohus): started working on this based on GMRequestAgent
// 
//-----------------------------------------------------------------------------

#include "GMRequestAgent_LR.h"
#include "DMCore/Agents/DialogAgents/BasicAgents/MARequest.h"
#include "DMCore/Log.h"

//-----------------------------------------------------------------------------
// D: Constructors, destructors
//-----------------------------------------------------------------------------

// D: default constructor, just calls upon the CGroundingModel constructor
CGMRequestAgent_LR::CGMRequestAgent_LR(string sAModelPolicy, string sAName): 
    CGMRequestAgent(sAModelPolicy, sAName) {
}

// D: constructor from reference
CGMRequestAgent_LR::CGMRequestAgent_LR(CGMRequestAgent_LR& rAGMRequestAgent_LR) {
    sModelPolicy = rAGMRequestAgent_LR.sModelPolicy;
    sName = rAGMRequestAgent_LR.sName;
    pPolicy = rAGMRequestAgent_LR.pPolicy;
    viActionMappings = rAGMRequestAgent_LR.viActionMappings;
    sExplorationMode = rAGMRequestAgent_LR.sExplorationMode;
    fExplorationParameter = rAGMRequestAgent_LR.fExplorationParameter;
    pRequestAgent = rAGMRequestAgent_LR.pRequestAgent;
    stFullState = rAGMRequestAgent_LR.stFullState;
    bdBeliefState = rAGMRequestAgent_LR.bdBeliefState;
}

// D: destructor - does nothing so far
CGMRequestAgent_LR::~CGMRequestAgent_LR() {
}

//-----------------------------------------------------------------------------
// D: Grounding model factory method
//-----------------------------------------------------------------------------

CGroundingModel* CGMRequestAgent_LR::GroundingModelFactory(string sModelPolicy) {
    return new CGMRequestAgent_LR(sModelPolicy);
}

//-----------------------------------------------------------------------------
// D: Member access methods
//-----------------------------------------------------------------------------

// D: Access to type
string CGMRequestAgent_LR::GetType() {
    return "request_lr";
}

//-----------------------------------------------------------------------------
// D: Grounding model specific methods
//-----------------------------------------------------------------------------

// D: Cloning the model
CGroundingModel* CGMRequestAgent_LR::Clone() {
    return new CGMRequestAgent_LR(*this);
}

// D: Overwritten method for loading the model policy
bool CGMRequestAgent_LR::LoadPolicy() {

	// get the string data
    string sData = pGroundingManager->GetPolicy(sModelPolicy);

    // parse it - first break it into lines
    TStringVector vsLines = PartitionString(sData, "\n");

	unsigned int i = 0;
    while(i < vsLines.size()) {
        // check that it's not a commentary or an empty line
        if(vsLines.empty() || (vsLines[i][0] == '#') || 
            (vsLines[i][0] == '/') || (vsLines[i][0] == '%') ||
			Trim(vsLines[i]).empty()) {
            i++;
            continue;
        }

		// get the line
		string sLine = Trim(vsLines[i]);

		// check if we have the exploration mode line
		if(ToLowerCase(sLine.substr(0, 16)) == "exploration_mode") {
            // read a line in the format EXPLORATION_MODE=...
            string sTemp1, sTemp2;
            SplitOnFirst(vsLines[i], "=", sTemp1, sTemp2);
            // set the exploration mode
            sExplorationMode = ToLowerCase(Trim(sTemp2));
		}
	
		// o/w, we basically expect LR_MODEL_PSUCC(action_name)
		if(ToLowerCase(sLine.substr(0, 14)) == "lr_model_psucc") {
			// then go on and read the parameters for each individual
			// feature in the model

			string sFoo, sAction;
			SplitOnFirst(sLine, "(", sFoo, sAction);
			sAction = TrimRight(sAction, " \n)");

			// construct the model
			TLRModel lrmModel;
			
			// now continue through the lines
			i++;
			while((i < vsLines.size()) && 
				(ToLowerCase(vsLines[i].substr(0,8)) != "v_matrix")) {
				string sParam, sCoef;
				SplitOnFirst(vsLines[i], "=", sParam, sCoef);
				lrmModel.vsFeatures.push_back(Trim(sParam));
				lrmModel.vfCoefs.push_back((float)atof(sCoef.c_str()));
				i++;
			}

		    // now that we have found v_matrix, read the matrix in
			// first check that we are not out of the file yet
			if(i >= vsLines.size()) 
				FatalError(FormatString(
					"Error loading request_lr model policy [%s].", 
					sAction.c_str()));
					
			// allocate the variance matrix
			int iNumParams = lrmModel.vfCoefs.size();
			lrmModel.pfVar = new float[iNumParams * iNumParams];

			int ij = 0;
			i++;
			while((i < vsLines.size()) &&
				(ToLowerCase(vsLines[i].substr(0,3)) != "end")) {
				TStringVector vsCoefs = PartitionString(Trim(vsLines[i]), " \t\n");
				for(int ii = 0; ii < iNumParams; ii++) 
					lrmModel.pfVar[ij*iNumParams + ii] = 
						(float)atof(Trim(vsCoefs[ii]).c_str());
				i++;
				ij++;
			}

			// store the action index in the viActionMappins
			viActionMappings.push_back(
				pGroundingManager->GroundingActionNameToIndex(sAction));

			// finally store the model in the vector
			vLRModels.push_back(lrmModel);
		}

		// advance to the next line
		i++;
    }

    // resize the bdActionValues vector accordingly
	bdActionValues.Resize(viActionMappings.size());
	
    return true;
}

// D: Computes the expected values of the various actions and returns a 
//    corresponding probability distribution over the actions
void CGMRequestAgent_LR::ComputeActionValuesDistribution() {

    // for each action
    for(unsigned int a = 0; a < viActionMappings.size(); a++) {

        // if the action is available (in the current state)
        if(ActionIsAvailable(a)) {

            // compute its expected utility by running the corresponding
		    // logistic regression model		
    		
		    // compute eta (the linear feature combination)
		    double fETA = 0;
		    // go through all the features, and also store them
		    vector<double> vfFeatures;
		    int iNumParams = vLRModels[a].vfCoefs.size();
		    int i;
		    for(i = 0; i < iNumParams; i++) {    			
			    // obtain the feature value
			    double fFeatureValue = 
			        pGroundingManager->GetGroundingFeature(vLRModels[a].vsFeatures[i]);
			    // store it 
			    vfFeatures.push_back(fFeatureValue);
			    // add to eta
			    fETA += fFeatureValue * vLRModels[a].vfCoefs[i];
		    }

		    // compute yhat (bdActionValues[a])
		    bdActionValues[a] = (float)(1 / (1 + exp(-fETA)));

		    // compute vxb
		    double dVXB = 0;
		    for(i = 0; i < iNumParams; i++) {
			    // compute one column
			    double dTemp = 0;
			    for(unsigned int j = 0; j < vLRModels[a].vfCoefs.size(); j++) {
				    dTemp += vfFeatures[j] * vLRModels[a].pfVar[j*iNumParams+i];
				}
			    dVXB += dTemp * vfFeatures[i];
		    }

		    // compute dxb
		    double dDXB = 1.95996398454005 * sqrt(dVXB);

		    // compute bound1 and bound2
		    float fBound1 = (float)(1 / (1 + exp(-fETA+dDXB)));
		    float fBound2 = (float)(1 / (1 + exp(-fETA-dDXB)));
    		
		    // decide which one is the min and the max of those
		    if(fBound1 < fBound2) {
			    bdActionValues.LowBound(a) = fBound1;
			    bdActionValues.HiBound(a) = fBound2;
		    } else {
			    bdActionValues.LowBound(a) = fBound2;
			    bdActionValues.HiBound(a) = fBound1;
		    }
		
		} else {
		    // o/w the action is not currently available, so don't use it
		    bdActionValues[a] = UNAVAILABLE_ACTION;
			bdActionValues.LowBound(a) = UNAVAILABLE_ACTION;
			bdActionValues.HiBound(a) = UNAVAILABLE_ACTION;
	    }
    }
}

// D: Compute the suggested action index - be default, the action that 
//    maximizes the expected utility
int CGMRequestAgent_LR::ComputeSuggestedActionIndex() {

    // first check if we already computed a suggested action index
    if(iSuggestedActionIndex != -1)
        return iSuggestedActionIndex;

    // now check if we need to use an external policy
    if(bExternalPolicy) {
        iSuggestedActionIndex = 
            pepiExternalPolicy->ComputeSuggestedActionIndex(stFullState);
        return iSuggestedActionIndex;
    }

    // compute the values for actions from this state
    ComputeActionValuesDistribution();

    // now, depending on the type of exploration mode
    if(sExplorationMode == "greedy") {
        // if greedy, choose the optimal action
        iSuggestedActionIndex = bdActionValues.GetModeEvent();
    } else if(sExplorationMode == "stochastic") {
        // if stochastic, just choose an action randomly from the value distr
        iSuggestedActionIndex = bdActionValues.GetRandomlyDrawnEvent();
    } else if(sExplorationMode == "epsilon-greedy") {
        // if epsilon-greedy, 
        iSuggestedActionIndex = 
            bdActionValues.GetEpsilonGreedyEvent(fExplorationParameter);
    } else if(sExplorationMode == "soft-max") {
        // if soft-max
        iSuggestedActionIndex =
            bdActionValues.GetSoftMaxEvent(fExplorationParameter);
	} else if(sExplorationMode == "highest-upper-bound") {
        // if highest upper bound 
        iSuggestedActionIndex = 
			bdActionValues.GetMaxHiBoundEvent();
	} else {
        // o/w there's an error
        FatalError(FormatString("Unknown exploration mode: %s.", 
            sExplorationMode.c_str()));
    }

    // if no action is available, issue a fatal error
    if(iSuggestedActionIndex == -1)
        FatalError("Could not select an action.");

    // apply the mapping
    iSuggestedActionIndex = viActionMappings[iSuggestedActionIndex];
    
    // and return the value
    return iSuggestedActionIndex;

}

// D: Log the state and the suggested action of the model
void CGMRequestAgent_LR::LogStateAction() {
    // log the computation results
    Log(GROUNDINGMODELX_STREAM, 
        "Grounding model %s [TYPE=%s;CLASS=%s;EM=%s;EP=%.2f]:\n"
        "Action values (dumped below):\n%s\n"
        "Suggested action: %s\n",
        GetName().c_str(), GetType().c_str(), GetModelPolicy().c_str(), 
        sExplorationMode.c_str(), fExplorationParameter, 
        actionValuesToString().c_str(), 
        pGroundingManager->
        GroundingActionIndexToName(iSuggestedActionIndex).c_str());
}

// D: Compute whether or not an action is available in the current state
#pragma warning (disable:4100)
bool CGMRequestAgent_LR::ActionIsAvailable(int iActionIndex) {
    // all actions are always available
    return true;
}
#pragma warning (default:4100)