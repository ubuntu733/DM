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
// BOOLCONCEPT.CPP - implementation of the CBoolConcept class
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
//   [2004-06-02] (dbohus):  added definition of pOwnerConcept, concepts now
//                            check with parent if unclear if they are
//                            grounded
//   [2004-05-30] (dbohus):  changed definition of IsGrounded
//   [2004-05-19] (dbohus):  major makeover on concepts, introduced frame 
//                            concepts and structure concepts which support 
//                            grounding
//   [2004-05-15] (dbohus):  cleaned up concepts code, added Update as well as
//                            various types of update methods
//   [2004-04-25] (dbohus):  added WaitingConveyance flag and functionality
//   [2004-04-16] (dbohus):  removed grounding models from constructors and 
//                            added them as a separate method
//   [2003-04-14] (dbohus): added support for OwnerDialogAgent
//   [2003-04-03] (dbohus): added support for concept grounding models
//   [2003-04-01] (dbohus): added support for iTurnLastUpdated
//   [2003-03-19] (dbohus): eliminated ValueToString, transformed 
//                           ValueToStringWithConf into ValueSetToString
//   [2003-01-29] (dbohus): added equality and comparison operators
//   [2003-01-28] (dbohus): added CBool define 
//   [2002-12-09] (dbohus): major revamp on concepts: added support for 
//                           reopening concepts and maintaing a history of
//                           previous values 
//   [2002-12-01] (dbohus): added SetValueFromString and UpdateValueFromString
//                           functions on concepts
//   [2002-11-26] (dbohus): fixed bug in assign operator when assigning to self
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-02-10] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "BoolConcept.h"
#include "DMCore/Log.h"

#pragma warning (disable:4100)

// declaration of the cTrue and cFalse concepts
CBoolConcept cTrue("_system_True", true, 1, csSystem);
CBoolConcept cFalse("_system_False", false, 1, csSystem);

//-----------------------------------------------------------------------------
// CBoolHyp class - this class keeps a pair: <Boolean-value, conf>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CBoolHyp: Constructors and Destructors
//-----------------------------------------------------------------------------

// D: default constructor
CBoolHyp::CBoolHyp() {
	ctHypType = ctBool;
	bValue = false;
	fConfidence = 0.0;
}

// D: copy constructor
CBoolHyp::CBoolHyp(CBoolHyp& rABoolHyp) {
	ctHypType = ctBool;
	bValue = rABoolHyp.bValue;
	fConfidence = rABoolHyp.fConfidence;
}

// D: from bool + confidence constructor
CBoolHyp::CBoolHyp(bool bAValue, float fAConfidence) {
	ctHypType = ctBool;
	bValue = bAValue;
	fConfidence = fAConfidence;
}

//-----------------------------------------------------------------------------
// CBoolHyp: Overwritten, CBoolHyp specific methods
//-----------------------------------------------------------------------------

// D: assignment operator from bool
CHyp& CBoolHyp::operator = (bool bAValue) {
	bValue = bAValue;
	fConfidence = 1.0;
	return *this;
}

// D: assignment operator from another hyp
CHyp& CBoolHyp::operator = (CHyp& rAHyp) {
    if(&rAHyp != this) {
	    // check the type
	    if(rAHyp.GetHypType() != ctBool) {
		    // if it's not a bool, signal an error
		    FatalError("Assignment operator from a different hyp type called on "\
		  	    "bool hyp. Cannot perform conversion.");
		    return *this;
	    }

	    CBoolHyp& rABoolHyp = (CBoolHyp &)rAHyp;
	    bValue = rABoolHyp.bValue;
	    fConfidence = rABoolHyp.fConfidence;
    }
	return *this;
}

// D: equality operator
bool CBoolHyp::operator == (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctBool) {
		// if it's not a bool, signal an error
		FatalError("Equality operator with a different hyp type called on "\
		  	  "bool hyp. Cannot perform conversion.");
		return false;
	}

    return bValue == ((CBoolHyp&)rAHyp).bValue;
}

// D: comparison operator
bool CBoolHyp::operator < (CHyp& rAHyp) {   
    // signal an error, cannot compare bools
	FatalError("Comparison operator < called on CBoolHyp.");
	return false;
}

// D: comparison operator
bool CBoolHyp::operator > (CHyp& rAHyp) {   
    // signal an error, cannot compare bools
	FatalError("Comparison operator > called on CBoolHyp.");
	return false;
}

// D: comparison operator
bool CBoolHyp::operator <= (CHyp& rAHyp) {   
    // signal an error, cannot compare bools
	FatalError("Comparison operator <= called on CBoolHyp.");
	return false;
}

// D: comparison operator
bool CBoolHyp::operator >= (CHyp& rAHyp) {   
    // signal an error, cannot compare bools
	FatalError("Comparison operator >= called on CBoolHyp.");
	return false;
}

// D: Indexing operator - return an error, should never be called
CHyp* CBoolHyp::operator [] (string sItem) {
	FatalError("Indexing operator [] called on CBoolHyp.");
    return NULL;
}

// D: Convert value to string
string CBoolHyp::ValueToString() {
	return bValue?"true":"false";
}

// D: Convert hyp to string
string CBoolHyp::ToString() {
	string sResult = FormatString("%s%.2f", VAL_CONF_SEPARATOR, fConfidence);
	sResult = (bValue?"true":"false") + sResult;
	return sResult;
}

// D: Get the hyp from a string
void CBoolHyp::FromString(string sString) {
	
	// separate the string into value and confidence 
	string sUpperValue, sConfidence;
	SplitOnFirst(sString, VAL_CONF_SEPARATOR, sUpperValue, sConfidence);
	sUpperValue = ToUpperCase(Trim(sUpperValue));
	sConfidence = Trim(sConfidence);
	
	// get the value
	if(sUpperValue == "TRUE") {
		bValue = true;
	} else if(sUpperValue == "FALSE") {
		bValue = false;	
	} else {
		FatalError(FormatString(
            "Cannot perform conversion to CBoolHyp from %s.", 
            sString.c_str()));
        return;
	}

	// get the confidence 
    if(sConfidence == "") {
        fConfidence = 1.0;
    } else {
	    if(sscanf(sConfidence.c_str(), "%f", &fConfidence) != 1) {
		    FatalError(FormatString(
                "Cannot perform conversion to CBoolHyp from %s.", 
                sString.c_str()));
            return;
	    }
    }
}


//-----------------------------------------------------------------------------
// CBoolConcept class - this is the boolean concept class. It overloads and
//						provides some new constructors, and operators
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CBoolConcept: Constructors and destructors
//-----------------------------------------------------------------------------

// D: constructor by name
CBoolConcept::CBoolConcept(string sAName, TConceptSource csAConceptSource):
    CConcept(sAName, csAConceptSource, DEFAULT_BOOL_CARDINALITY) {
	ctConceptType = ctBool;
}

// D: constructor from name + value + confidence
CBoolConcept::CBoolConcept(string sAName, bool bAValue, float fAConfidence,
						   TConceptSource csAConceptSource) {
	ctConceptType = ctBool;
	csConceptSource = csAConceptSource;
	sName = sAName;
    pOwnerDialogAgent = NULL;
    pOwnerConcept = NULL;
    pGroundingModel = NULL;
    vhCurrentHypSet.push_back(new CBoolHyp(bAValue, fAConfidence));
    iNumValidHyps = 1;
    iCardinality = DEFAULT_BOOL_CARDINALITY;
    iTurnLastUpdated = -1;
    bWaitingConveyance = false;
	cConveyance = cNotConveyed;
    SetHistoryConcept(false);
    pPrevConcept = NULL;
}

//-----------------------------------------------------------------------------
// Overwritten methods for overall concept manipulation
//-----------------------------------------------------------------------------

// D: assignment operator from boolean
CConcept& CBoolConcept::operator = (bool bAValue) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current hypothesis set
    ClearCurrentHypSet();
	// add the hypothesis (this will notify the change)
    AddHyp(new CBoolHyp(bAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: assignment operator from integer
CConcept& CBoolConcept::operator = (int iAValue) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
    ClearCurrentHypSet();
	// add the hypothesis (this will notify the change)
    if(iAValue)
        AddHyp(new CBoolHyp(true, 1.0));
	else
		AddHyp(new CBoolHyp(false, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: boolean cast operator: returns the max confidence value in the set 
CBoolConcept::operator bool() {
	if(IsUpdatedAndGrounded() && GetTopHyp()) {
		return ((CBoolHyp *)GetTopHyp())->bValue;
	} else {
        return false;
	}
}

// D: integer cast operator: retusn 1 or 0
CBoolConcept::operator int() {
    return (int)operator bool();
}

// D: returns an empty clone of the concept (basically just preserving the
//    type, but not the contents
CConcept* CBoolConcept::EmptyClone() {
    return new CBoolConcept;
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the current hypothesis set and belief
// manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CBoolConcept::HypFactory() {
	return new CBoolHyp();
}

#pragma warning (default:4100)