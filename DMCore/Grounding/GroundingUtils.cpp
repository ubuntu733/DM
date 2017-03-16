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
// GROUNDINGUTILS.CPP - implementation of the CState and CBeliefDistribution 
//                      classes used to model the state and the abstracted
//                      or aggregated state of a grounding model
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
//   [2004-02-24] (dbohus): added CState
//   [2004-02-10] (dbohus): changed so that belief distribution can have 
//                           invalid events
//   [2004-02-09] (dbohus): added epsilon-greedy and soft-max selection
//   [2003-02-12] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------


#include "Utils/Utils.h"
#include "GroundingUtils.h"
#include "DMCore/Log.h"


//-----------------------------------------------------------------------------
//
// D: CState
//
//-----------------------------------------------------------------------------
// D: indexing operator
string& CState::operator[] (string sIndex) {
    return s2sStateVars[sIndex];
}

// D: clear the state
void CState::Clear() {
    s2sStateVars.clear();
}

// D: insert from another hash
void CState::Add(STRING2STRING s2s) {
    s2sStateVars.insert(s2s.begin()++, s2s.end()--);
}

// D: string conversion function
string CState::ToString() {
    return S2SHashToString(s2sStateVars, "\n") + "\n";
}

//-----------------------------------------------------------------------------
//
// D: CBeliefDistribution
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// D: Constructors and destructors
//-----------------------------------------------------------------------------

// D: constructor
CBeliefDistribution::CBeliefDistribution(int iNumEvents) {
    // resize the probabilities vector
    vfProbability.resize(iNumEvents, 0.0);
	vfProbabilityLowBound.resize(iNumEvents, INVALID_EVENT);
	vfProbabilityHiBound.resize(iNumEvents, INVALID_EVENT);	
}

// D: constructor from reference
CBeliefDistribution::CBeliefDistribution(
    CBeliefDistribution& rbdABeliefDistribution) {
    // copy the probabilities vector
    vfProbability = rbdABeliefDistribution.vfProbability;
	vfProbabilityLowBound = rbdABeliefDistribution.vfProbabilityLowBound;
	vfProbabilityHiBound = rbdABeliefDistribution.vfProbabilityHiBound;
}

// D: destructor - does nothing
CBeliefDistribution::~CBeliefDistribution() {
}

// D: resize the event space for the distribution
void CBeliefDistribution::Resize(int iNumEvents) {
    // resize the probabilities vector
    vfProbability.resize(iNumEvents, 0.0);
	vfProbabilityLowBound.resize(iNumEvents, INVALID_EVENT);
	vfProbabilityHiBound.resize(iNumEvents, INVALID_EVENT);
}

//-----------------------------------------------------------------------------
// D: Access to distribution
//-----------------------------------------------------------------------------
// D: Access to probabilities, via the [] operator
float& CBeliefDistribution::operator[] (unsigned int iIndex) {
    // resize the vector if we're falling out of it
    if(iIndex >= vfProbability.size()) {
        vfProbability.resize(iIndex + 1, 0.0);
		vfProbabilityLowBound.resize(iIndex + 1, INVALID_EVENT);
		vfProbabilityHiBound.resize(iIndex + 1, INVALID_EVENT);
    } 
    return vfProbability[iIndex];
}

// D: Access to low bounds
float& CBeliefDistribution::LowBound(unsigned int iIndex) {
    // resize the vector if we're falling out of it
    if(iIndex >= vfProbabilityLowBound.size()) {
        vfProbability.resize(iIndex + 1, 0.0);
		vfProbabilityLowBound.resize(iIndex + 1, INVALID_EVENT);
		vfProbabilityHiBound.resize(iIndex + 1, INVALID_EVENT);
    }
    return vfProbabilityLowBound[iIndex];
}

// D: Access to hi bounds
float& CBeliefDistribution::HiBound(unsigned int iIndex) {
    // resize the vector if we're falling out of it
    if(iIndex >= vfProbabilityHiBound.size()) {
        vfProbability.resize(iIndex + 1, 0.0);
		vfProbabilityLowBound.resize(iIndex + 1, INVALID_EVENT);
		vfProbabilityHiBound.resize(iIndex + 1, INVALID_EVENT);
    }
    return vfProbabilityHiBound[iIndex];
}

// D: Access to the number of valid events
int CBeliefDistribution::GetValidEventsNumber() {
    int iEventsNumber = 0; 
    for(unsigned int i = 0; i < vfProbability.size(); i++)
        if(vfProbability[i] != INVALID_EVENT)
            iEventsNumber++;
    return iEventsNumber;
}

//-----------------------------------------------------------------------------
// D: Functions for transforming the distribution
//-----------------------------------------------------------------------------

// D: Normalize the distribution
void CBeliefDistribution::Normalize() {
    // compute the normalization constant
    float fNormalizer = 0;
    for(unsigned int i = 0; i < vfProbability.size(); i++)
        if(vfProbability[i] != INVALID_EVENT) {
            fNormalizer += vfProbability[i];
        }
    // normalize
    if(fNormalizer != 0) {
        for(unsigned int i = 0; i < vfProbability.size(); i++) 
            if(vfProbability[i] != INVALID_EVENT) {
                vfProbability[i] = vfProbability[i] / fNormalizer;
            }
    }
}

//-----------------------------------------------------------------------------
// Functions for choosing a particular action from the distribution
//-----------------------------------------------------------------------------

// D: return the action with the highest probability/utility
int CBeliefDistribution::GetModeEvent() {
    int iMaxIndex = -1;
    float fMaxProbability = 0;
    for(unsigned int i = 0; i < vfProbability.size(); i++) {
        if(vfProbability[i] > fMaxProbability) {
            fMaxProbability = vfProbability[i];
            iMaxIndex = i;
        }
    }
    return iMaxIndex;
}

// D: return the action with the highest upper bound on the probability/utility
//    if multiple events in the distribution have the same highest upper bound, 
//    choose randomly between them
int CBeliefDistribution::GetMaxHiBoundEvent() {

	// store a vector with the indices that had the max highest bound
    vector<int> viMaxIndex;
	viMaxIndex.push_back(-1);
    float fMaxProbability = 0;
    for(unsigned int i = 0; i < vfProbabilityHiBound.size(); i++) {
        if(vfProbabilityHiBound[i] > fMaxProbability) {
            fMaxProbability = vfProbabilityHiBound[i];
			viMaxIndex.clear();
			viMaxIndex.push_back(i);
        } else if(vfProbabilityHiBound[i] == fMaxProbability) {
			// if we have another maximum store it in the vector
			viMaxIndex.push_back(i);
		}
    }

	// now return randomly from that vector
	if(viMaxIndex.size() == 1)
		return viMaxIndex[0];
	else
		return viMaxIndex[rand() % viMaxIndex.size()];
}

// D: return the event according to an epsilon-greedy policy
int CBeliefDistribution::GetEpsilonGreedyEvent(float fEpsilon) {
    float fRandom = rand() / (float)(RAND_MAX - 1);
    if(fRandom > fEpsilon) {
        // if outside of epsilon, then choose the mode
        return GetModeEvent();
    } else {
        // if within epsilon, then randomly choose between the
        // non-mode events
        int iMode = GetModeEvent();
        
        // check that there are other actions
        int iValidEventsNumber = GetValidEventsNumber();
        if(iValidEventsNumber <= 1)
            return iMode;

        // if we have other actions, choose one of them
        int iTemp = rand() % (GetValidEventsNumber() - 1);
        // find the first valid non-mode event
        int iChoose = 0;
        while((iChoose == iMode) || (vfProbability[iChoose] == INVALID_EVENT))
            iChoose++;

        // now count down from iTemp and for each count, find the next 
        // valid event 
        while(iTemp) {
            iTemp--;
            iChoose++;
            while((iChoose == iMode) || (vfProbability[iChoose] == INVALID_EVENT))
                iChoose++;
        }

        // finally, return the chosen event
        return iChoose;
    }
}

// D: return the event according to a soft-max policy
int CBeliefDistribution::GetSoftMaxEvent(float fTemperature) {
    // compute the exponentiated values in a temporary belief distribution
    CBeliefDistribution bdTemp(vfProbability.size());
    for(unsigned int i = 0; i < vfProbability.size(); i++) 
        if(vfProbability[i] == INVALID_EVENT) {
            bdTemp[i] = INVALID_EVENT;
        } else {
            bdTemp[i] = (float)(exp(vfProbability[i]/fTemperature));
        }
    // normalize
    bdTemp.Normalize();
    // then get a randomly drawn event
    return bdTemp.GetRandomlyDrawnEvent();
}

// D: Randomly draw an event from the probability/utility distribution
int CBeliefDistribution::GetRandomlyDrawnEvent() {
    float fAccumulator = 0;
    float fRandom = rand() / (float)(RAND_MAX - 1);    
    Normalize();
    int lastI = -1;
    for(unsigned int i = 0; i < vfProbability.size(); i++) {
        if(vfProbability[i] != INVALID_EVENT) {
            fAccumulator += vfProbability[i];
            lastI = i;
        }
        if(fAccumulator > fRandom) return i;
    }
    // o/w return the last event
    return lastI;
}