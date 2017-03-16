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

#pragma once
#ifndef __GMREQUESTAGENT_LR_H__
#define __GMREQUESTAGENT_LR_H__

#include "GMRequestAgent.h"

//-----------------------------------------------------------------------------
// Description of the model: 
//  The model does not keep an explicit representation of state; rather it 
//   uses logistic regressin (LR) models to compute the probability of success
//   for each individual action, and then it takes the action with the largest
//   probability (several selection mechanisms are available -- for instance
//   choosing the max probability or the highest upper bound, etc)
//-----------------------------------------------------------------------------

// D: a logistic regression model (with variance information)
typedef struct {
	TStringVector vsFeatures;
	vector<float> vfCoefs;
	float* pfVar;
} TLRModel;

// D: definition for the string-to-lrmodel type
typedef map <string, TLRModel> STRING2LRMODEL;

//-----------------------------------------------------------------------------
// CGMRequestAgent_LR Class - This is grounding model class for request agents
//-----------------------------------------------------------------------------

// D: forward declaration of the CMARequest class (since it's used internally
//    in the CGMConcept)
class CMARequest;

class CGMRequestAgent_LR: public CGMRequestAgent {

protected:
	//---------------------------------------------------------------------
	// protected class members
	//---------------------------------------------------------------------

	// storing the LRModels (the policy is expressed as a series of 
	// logistic regression models)
	vector<TLRModel> vLRModels;

public:

    //---------------------------------------------------------------------
	// Constructors, Destructors
	//---------------------------------------------------------------------

    CGMRequestAgent_LR(string sAModelPolicy = "", 
        string sAName = "UNKNOWN");
    CGMRequestAgent_LR(CGMRequestAgent_LR& rAGMRequestAgent_LR);
    virtual ~CGMRequestAgent_LR();

	//---------------------------------------------------------------------
	// Static factory method
    //---------------------------------------------------------------------
    
    static CGroundingModel* GroundingModelFactory(string sModelPolicy);

    //---------------------------------------------------------------------
	// Member access methods
	//---------------------------------------------------------------------
	
    // Access to type
    virtual string GetType();

    //---------------------------------------------------------------------
	// Overwritten virtual methods which implement the specific 
    // request-agent grounding model functionality
	//---------------------------------------------------------------------

    // Overwritten method for cloning the model
    virtual CGroundingModel* Clone();

    // Overwritten method for loading the model policy
    virtual bool LoadPolicy();

    // Overwritten method that computes the expected values of the 
    // various actions and returns a corresponding probability distribution 
    // over the actions (in this case this also computes the bounds on 
	// the probabilities of succes)
    virtual void ComputeActionValuesDistribution();

	// Overwritten method that only computes the action the model suggests 
	// we should take, but does not run the action yet
    virtual int ComputeSuggestedActionIndex();

    // Overwritten method for logging the current state of the model and 
    // the suggested action
    virtual void LogStateAction();
    
protected:
    //---------------------------------------------------------------------
	// Virtual method which defines which actions are available at any 
	// state
	//---------------------------------------------------------------------

    virtual bool ActionIsAvailable(int iActionIndex);
};

#endif // __GMREQUESTAGENT_LR_H__