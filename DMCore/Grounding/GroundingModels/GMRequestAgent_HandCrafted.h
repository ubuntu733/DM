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
// GMREQUESTAGENT.H   - definition of the CGMRequestAgent grounding model class 
//                      turn-level grouding in request agents
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

#pragma once
#ifndef __GMREQUESTAGENT_HANDCRAFTED_H__
#define __GMREQUESTAGENT_HANDCRAFTED_H__

#include "GMRequestAgent.h"

//-----------------------------------------------------------------------------
// CGMRequestAgent_HandCrafted Class - This is grounding model class for 
//         request agents using a hand-crafted policy
//-----------------------------------------------------------------------------

// D: forward declaration of the CDialogAgent class (since it's used internally
//    in the CGMConcept)
class CDialogAgent;

//-----------------------------------------------------------------------------
// CCondition: a class containing a condition on a feature 
//             (e.g. "confidence > 0.3")
//-----------------------------------------------------------------------------
class CCondition {

protected:
	//---------------------------------------------------------------------
	// protected class members
	//---------------------------------------------------------------------
	typedef enum TComparator {
		EqualTo,
		NotEqualTo,
		GreaterThan,
		LessThan,
		GreaterThanOrEqualTo,
		LessThanOrEqualTo
	} TComparator;

	typedef enum TValueType {
		Bool,
		Number,
		String
	} TValueType;

	string sFeature;
	string sTestValue;
	TComparator cComparator;
	TValueType vtValueType;

public:

    //---------------------------------------------------------------------
	// Constructors, Destructors
	//---------------------------------------------------------------------

	CCondition(string sFeature, string sValue, TValueType vtValueType, 
		TComparator cComparator);
	CCondition(string sCondStr);

	bool Matches(CState &sState);

};

//-----------------------------------------------------------------------------
// CGMState: a class containing an abstract dialog state (i.e. a conjunction
//           of conditions)
//-----------------------------------------------------------------------------
class CGMState {

protected:
	string sName;
	vector<CCondition> vcConditions;

public:

    //---------------------------------------------------------------------
	// Constructors, Destructors
	//---------------------------------------------------------------------

	CGMState(string sAName);
	~CGMState();

	//---------------------------------------------------------------------
	// Public methods
    //---------------------------------------------------------------------

	void AddCondition(CCondition &cCondition);
	bool Matches(CState &sState);
};

class CGMRequestAgent_HandCrafted: public CGMRequestAgent {

protected:
	vector<CGMState> vsStates;
	

public:

    //---------------------------------------------------------------------
	// Constructors, Destructors
	//---------------------------------------------------------------------

    CGMRequestAgent_HandCrafted(string sAModelPolicy = "", 
		string sAName = "UNKNOWN");
    CGMRequestAgent_HandCrafted(
		CGMRequestAgent_HandCrafted& rAGMRequestAgent_HandCrafted);
    virtual ~CGMRequestAgent_HandCrafted();

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

protected:
	//---------------------------------------------------------------------
	// Auxiliary, overwritten protected methods
    //---------------------------------------------------------------------

    // Overwritten method that computes the full state for this model
    virtual void computeFullState();

    // Overwritten method that computes the belief state for this model
    virtual void computeBeliefState();
};

#endif // __GMREQUESTAGENT_HANDCRAFTED_H__