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
// GMCONCEPT.H   - definition of the CGMConcept grounding model class for
//                 concept grouding
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
//   [2004-02-24] (dbohus): added support for full state and collapsed state
//   [2004-02-09] (dbohus): changed model data to "policy"
//   [2003-04-02] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __GMCONCEPT_H__
#define __GMCONCEPT_H__

#include "GroundingModel.h"

//-----------------------------------------------------------------------------
// Description of the model: 
//   - the model for now has only 3 states: 
//     INACTIVE - the concept does not have a current value
//     GROUNDED - the concept is grounded
//     NOT_GROUNDED - the concept is not grounded
//     (the can be ambiguity over the last 2 states)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Model defines
//-----------------------------------------------------------------------------

#define SI_INACTIVE 0
#define SS_INACTIVE "INACTIVE"

#define SI_CONFIDENT 1
#define SS_CONFIDENT "CONFIDENT"

#define SI_UNCONFIDENT 2
#define SS_UNCONFIDENT "UNCONFIDENT"

#define SI_GROUNDED 3
#define SS_GROUNDED "GROUNDED"


//-----------------------------------------------------------------------------
// CGMConcept Class - This is grounding model class for concepts
//-----------------------------------------------------------------------------

// D: forward declaration of the CConcept class (since it's used internally
//    in the CGMConcept)
class CConcept;

class CGMConcept: public CGroundingModel {

protected:
	//---------------------------------------------------------------------
	// protected class members
	//---------------------------------------------------------------------

    CConcept* pConcept;		// pointer to the concept it is handling

public:

    //---------------------------------------------------------------------
	// Constructors, Destructors
	//---------------------------------------------------------------------

    CGMConcept(string sAModelPolicy = "", string sAName = "UNKNOWN");
    CGMConcept(CGMConcept& rAGMConcept);
    virtual ~CGMConcept();

	//---------------------------------------------------------------------
	// Static factory method
    //---------------------------------------------------------------------
    
    static CGroundingModel* GroundingModelFactory(string sModelPolicy);

    //---------------------------------------------------------------------
	// Member access methods
	//---------------------------------------------------------------------

    // Access to type
    virtual string GetType();

    // Overwritten method to block setting the name on the grounding model
    virtual void SetName(string sAName);

    // Overwritten method to return the name of the grounding model 
    // (which is the same as the agent-qualified name of the concept it
    // handles)
    virtual string GetName();

    // Setting and getting a pointer to the concept handled
    void SetConcept(CConcept* pAConcept);
    CConcept* GetConcept();

    //---------------------------------------------------------------------
	// Overwritten virtual methods which implement the specific concept-
    // grounding model functionality
	//---------------------------------------------------------------------

    // Overwritten method for cloning the model
    virtual CGroundingModel* Clone();

    // Overwritten method for loading the model policy
    virtual bool LoadPolicy();

	// Overwritten method for running the actions (also transmits the 
    // concept that the action is ran on to the action)
    virtual void RunAction(int iActionIndex);

    // Overwritten method for logging the current state of the model and 
    // the suggested action
    virtual void LogStateAction();

protected:
	//---------------------------------------------------------------------
	// Auxiliary, overwritten protected methods
    //---------------------------------------------------------------------

    // Overwritten method that computes the full state for this model
    virtual void computeFullState();

    // Overwritten method that computes the belief state for the model
    virtual void computeBeliefState();
};

#endif // __GMCONCEPT_H__
