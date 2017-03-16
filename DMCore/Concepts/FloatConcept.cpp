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
// FLOATCONCEPT.CPP - implementation of the CFloatConcept class
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
//   [2004-04-16] (dbohus): removed grounding models from constructors and 
//                           added them as a separate method
//   [2003-04-14] (dbohus): added support for OwnerDialogAgent
//   [2003-04-03] (dbohus): added support for concept grounding models
//   [2003-04-01] (dbohus): added support for iTurnLastUpdated
//   [2003-03-19] (dbohus): eliminated ValueToString, transformed 
//                           ValueToStringWithConf into ValueSetToString
//   [2003-01-29] (dbohus): added equality and comparison operators
//   [2003-01-29] (dbohus): added CFloat define
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

#include "FloatConcept.h"
#include "DMCore/Log.h"

//-----------------------------------------------------------------------------
// CFloatHyp class - this class keeps a pair: <float-value, conf>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CFloatHyp: Constructors and Destructors
//-----------------------------------------------------------------------------

// D: default constructor
CFloatHyp::CFloatHyp() {
	ctHypType = ctFloat;
	fValue = 0.0;
	fConfidence = 0.0;
}

// D: copy constructor
CFloatHyp::CFloatHyp(CFloatHyp& rAFloatHyp) {
	ctHypType = ctFloat;
	fValue = rAFloatHyp.fValue;
	fConfidence = rAFloatHyp.fConfidence;
}

// D: from value + confidence constructor
CFloatHyp::CFloatHyp(float fAValue, float fAConfidence) {
	ctHypType = ctFloat;
	fValue = fAValue;
	fConfidence = fAConfidence;
}

//-----------------------------------------------------------------------------
// CFloatHyp: Overwritten, CFloatHyp specific methods
//-----------------------------------------------------------------------------

// D: assignment operator from float
CHyp& CFloatHyp::operator = (float fAValue) {
	fValue = fAValue;
	fConfidence = 1.0;
	return *this;
}

// D: assignment operator from another valconf
CHyp& CFloatHyp::operator = (CHyp& rAHyp) {
    if(&rAHyp != this) {
	    // check the type
	    if(rAHyp.GetHypType() != ctFloat) {
		    // if it's not a float, signal an error
		    FatalError("Assignment operator from a different hyp type called on "\
			    "float hyp. Cannot perform conversion.");
		    return *this;
	    }

	    CFloatHyp& rAFloatHyp = (CFloatHyp&)rAHyp;
	    fValue = rAFloatHyp.fValue;
	    fConfidence = rAFloatHyp.fConfidence;
    }
	return *this;
}

// D: equality operator
bool CFloatHyp::operator == (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctFloat) {
		// if it's not a float, signal an error
		FatalError("Equality operator with a different hyp type called on "\
		  	  "float hyp. Cannot perform conversion.");
		return false;
	}

    return fValue == ((CFloatHyp&)rAHyp).fValue;
}

// D: comparison operator
bool CFloatHyp::operator < (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctFloat) {
		// if it's not a float, signal an error
		FatalError("Comparison operator < with a different hyp type called on "\
		  	  "float hyp. Cannot perform conversion.");
		return false;
	}

    return fValue < ((CFloatHyp&)rAHyp).fValue;
}

// D: comparison operator
bool CFloatHyp::operator > (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctFloat) {
		// if it's not a float, signal an error
		FatalError("Comparison operator > with a different hyp type called on "\
		  	  "float hyp. Cannot perform conversion.");
		return false;
	}

    return fValue > ((CFloatHyp&)rAHyp).fValue;
}

// D: comparison operator
bool CFloatHyp::operator <= (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctFloat) {
		// if it's not a float, signal an error
		FatalError("Comparison operator <= with a different hyp type called on "\
		  	  "float hyp. Cannot perform conversion.");
		return false;
	}

    return fValue <= ((CFloatHyp&)rAHyp).fValue;
}

// D: comparison operator
bool CFloatHyp::operator >= (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctFloat) {
		// if it's not a float, signal an error
		FatalError("Comparison operator >= with a different hyp type called on "\
		  	  "float hyp. Cannot perform conversion.");
		return false;
	}

    return fValue >= ((CFloatHyp&)rAHyp).fValue;
}

// D: Indexing operator - return an error, should never be called
CHyp* CFloatHyp::operator [] (string sItem) {
	FatalError("Indexing operator [] called on CFloatHyp.");
    return NULL;
}

// D: Convert value to string
string CFloatHyp::ValueToString() {
	return FormatString("%.4f", fValue);
}

// D: Convert hyp to string
string CFloatHyp::ToString() {
	return FormatString("%.4f%s%.2f", fValue, VAL_CONF_SEPARATOR, 
				 fConfidence);
}

// D: Get the valconf from a string
void CFloatHyp::FromString(string sString) {
	// separate the string into value and confidence 
	string sValue, sConfidence;
	SplitOnFirst(sString, VAL_CONF_SEPARATOR, sValue, sConfidence);
	sValue = Trim(sValue);
	sConfidence = Trim(sConfidence);
	
	// get the value
	if(sscanf(sValue.c_str(), "%f", &fValue) != 1) {
		FatalError(FormatString("Cannot perform conversion to CFloatHyp "\
						   "from %s.", sString.c_str()));
	};

	// get the confidence 
    if(sConfidence == "") {
        fConfidence = 1.0;
    } else {
	    if(sscanf(sConfidence.c_str(), "%f", &fConfidence) != 1) {
		    FatalError(FormatString("Cannot perform conversion to CFloatHyp "\
						    "from %s.", sString.c_str()));
	    }
    }
}



//-----------------------------------------------------------------------------
// CFloatConcept class - this is the float concept class. It overloads and
//						provides some new constructors, and operators
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CFloatConcept: Constructors and destructors
//-----------------------------------------------------------------------------

// D: constructor by name
CFloatConcept::CFloatConcept(string sAName, TConceptSource csAConceptSource):
	CConcept(sAName, csAConceptSource, DEFAULT_FLOAT_CARDINALITY) {
	ctConceptType = ctFloat;
}

// D: constructor from name + value + confidence
CFloatConcept::CFloatConcept(string sAName, float fAValue, float fAConfidence,
							 TConceptSource csAConceptSource) {
	ctConceptType = ctFloat;
	csConceptSource = csAConceptSource;
	sName = sAName;
    pOwnerDialogAgent = NULL;
    pOwnerConcept = NULL;
    pGroundingModel = NULL;
    vhCurrentHypSet.push_back(new CFloatHyp(fAValue, fAConfidence));
    iNumValidHyps = 1;
    iCardinality = DEFAULT_FLOAT_CARDINALITY;
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
CConcept& CFloatConcept::operator = (bool bAValue) {
    // check that it's not a history concept
    if(bHistoryConcept) 
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
	ClearCurrentHypSet();
	// add the hypothesis (this will notify the change)
    AddHyp(new CFloatHyp((float)bAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
    return *this;
}

// D: assignment operator from integer
CConcept& CFloatConcept::operator = (int iAValue) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
	ClearCurrentHypSet();
	// add the hypothesis (this will notify the change)
    AddHyp(new CFloatHyp((float)iAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
    return *this;
}

// D: assignment operator from float
CConcept& CFloatConcept::operator = (float fAValue) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
	ClearCurrentHypSet();
    // add the hypothesis (this will notify the change)
    AddHyp(new CFloatHyp(fAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: assignment operator from double
CConcept& CFloatConcept::operator = (double dAValue) {
    // check that it's not a history concept
    if(bHistoryConcept) 
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
	ClearCurrentHypSet();
    // add the hypothesis (this will notify the change)
    AddHyp(new CFloatHyp((float)dAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: boolean cast operator: calls upon the float cast and turns it into a
//    bool
CFloatConcept::operator bool() {
    return (operator float() != 0);
}

// D: integer cast operator: calls upon the float cast one and turns it into
//    an integer
CFloatConcept::operator int() {
	return (int)operator float();
}

// D: float cast operator: returns the max confidence value in the set 
CFloatConcept::operator float() {
	if(IsUpdatedAndGrounded() && GetTopHyp()) {
		return ((CFloatHyp *)GetTopHyp())->fValue;
	} else {
        return 0.0;
	}
}

// D: returns an empty clone of the concept (basically just preserving the
//    type, but not the contents
CConcept* CFloatConcept::EmptyClone() {
    return new CFloatConcept;
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the current hypothesis set and belief
// manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CFloatConcept::HypFactory() {
	return new CFloatHyp();
}
