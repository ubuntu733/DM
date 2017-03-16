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
// INTCONCEPT.CPP - implementation of the CIntConcept class
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
//   [2003-01-29] (dbohus): added CInt define
//   [2002-12-09] (dbohus): major revamp on concepts: added support for 
//                           reopening concepts and maintaing a history of
//                           previous values 
//   [2002-12-01] (dbohus): added SetValueFromString and UpdateValueFromString
//                           functions on concepts
//   [2002-11-26] (dbohus): fixed bug in assign operator when assigning to self
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-02-06] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "IntConcept.h"
#include "DMCore/Log.h"

//-----------------------------------------------------------------------------
// CIntHyp class - this class keeps a pair: <interger-value, conf>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CIntHyp: Constructors and Destructors
//-----------------------------------------------------------------------------

// D: default constructor
CIntHyp::CIntHyp() {
	ctHypType = ctInt;
	iValue = 0;
	fConfidence = 0.0;
}

// D: copy constructor
CIntHyp::CIntHyp(CIntHyp& rAIntHyp) {
	ctHypType = ctInt;
	iValue = rAIntHyp.iValue;
	fConfidence = rAIntHyp.fConfidence;
}

// D: from value + confidence constructor
CIntHyp::CIntHyp(int iAValue, float fAConfidence) {
	ctHypType = ctInt;
	iValue = iAValue;
	fConfidence = fAConfidence;
}

//-----------------------------------------------------------------------------
// CIntHyp: Overwritten, CIntHyp specific methods
//-----------------------------------------------------------------------------

// D: assignment operator from int
CHyp& CIntHyp::operator = (int iAValue) {
	iValue = iAValue;
	fConfidence = 1.0;
	return *this;
}

// D: assignment operator from another valconf
CHyp& CIntHyp::operator = (CHyp& rAHyp) {
    if(&rAHyp != this) {
	    // check the type
	    if(rAHyp.GetHypType() != ctInt) {
		    // if it's not an int, signal an error
		    FatalError("Assignment operator from a different hyp type called on "\
			    "int hyp. Cannot perform conversion.");
		    return *this;
	    }

	    CIntHyp& rAIntHyp = (CIntHyp&)rAHyp;
	    iValue = rAIntHyp.iValue;
	    fConfidence = rAIntHyp.fConfidence;
    }
	return *this;
}

// D: equality operator
bool CIntHyp::operator == (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctInt) {
		// if it's not an int, signal an error
		FatalError("Equality operator with a different hyp type called on "\
		  	  "int hyp. Cannot perform conversion.");
		return false;
	}

    return iValue == ((CIntHyp&)rAHyp).iValue;
}

// D: comparison operator
bool CIntHyp::operator < (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctInt) {
		// if it's not an int, signal an error
		FatalError("Comparison operator < with a different hyp type called on "\
		  	  "int hyp. Cannot perform conversion.");
		return false;
	}

    return iValue < ((CIntHyp&)rAHyp).iValue;
}

// D: comparison operator
bool CIntHyp::operator > (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctInt) {
		// if it's not an int, signal an error
		FatalError("Comparison operator > with a different hyp type called on "\
		  	  "int hyp. Cannot perform conversion.");
		return false;
	}

    return iValue > ((CIntHyp&)rAHyp).iValue;
}

// D: comparison operator
bool CIntHyp::operator <= (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctInt) {
		// if it's not an int, signal an error
		FatalError("Comparison operator <= with a different hyp type called on "\
		  	  "int hyp. Cannot perform conversion.");
		return false;
	}

    return iValue <= ((CIntHyp&)rAHyp).iValue;
}

// D: comparison operator
bool CIntHyp::operator >= (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctInt) {
		// if it's not an int, signal an error
		FatalError("Comparison operator >= with a different hyp type called on "\
		  	  "int hyp. Cannot perform conversion.");
		return false;
	}

    return iValue >= ((CIntHyp&)rAHyp).iValue;
}

// D: Indexing operator - return an error, should never be called
CHyp* CIntHyp::operator [] (string sItem) {
	FatalError("Indexing operator [] called on CIntHyp.");
    return NULL;
}

// D: Convert value to string
string CIntHyp::ValueToString() {
	return FormatString("%d", iValue);
}

// D: Convert hyp to string
string CIntHyp::ToString() {
	return FormatString("%d%s%.2f", iValue, VAL_CONF_SEPARATOR, fConfidence);
}

// D: Get the hyp from a string
void CIntHyp::FromString(string sString) {

	// separate the string into value and confidence 
	string sValue, sConfidence;
	SplitOnFirst(sString, VAL_CONF_SEPARATOR, sValue, sConfidence);
	sValue = Trim(sValue);
	sConfidence = Trim(sConfidence);
	
	// get the value
	if(sscanf(sValue.c_str(), "%d", &iValue) != 1) {
		FatalError(FormatString("Cannot perform conversion to CIntHyp "\
						   "from %s.", sString.c_str()));
	};

	// get the confidence 
    if(sConfidence == "") {
        fConfidence = 1.0;
    } else {
	    if(sscanf(sConfidence.c_str(), "%f", &fConfidence) != 1) {
		    FatalError(FormatString("Cannot perform conversion to CIntHyp "\
						    "from %s.", sString.c_str()));
	    };
    }
}



//-----------------------------------------------------------------------------
// CIntConcept class - this is the integer concept class. It overloads and
//						provides some new constructors, and operators
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CIntConcept: Constructors and destructors
//-----------------------------------------------------------------------------

// D: constructor by name
CIntConcept::CIntConcept(string sAName, TConceptSource csAConceptSource):
	CConcept(sAName, csAConceptSource, DEFAULT_INT_CARDINALITY) {
	ctConceptType = ctInt;
}

// D: constructor from name + value + confidence
CIntConcept::CIntConcept(string sAName, int iAValue, float fAConfidence,
						 TConceptSource csAConceptSource) {
	ctConceptType = ctInt;
	csConceptSource = csAConceptSource;
	sName = sAName;
    pOwnerDialogAgent = NULL;
    pOwnerConcept = NULL;
    pGroundingModel = NULL;
    vhCurrentHypSet.push_back(new CIntHyp(iAValue, fAConfidence));
    iNumValidHyps = 1;
    iCardinality = DEFAULT_INT_CARDINALITY;
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
CConcept& CIntConcept::operator = (bool bAValue) {
    // check that it's not a history concept
    if(bHistoryConcept) 
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
    ClearCurrentHypSet();
	// add the hypothesis (this will notify the change)
    AddHyp(new CIntHyp((int)bAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: assignment operator from integer
CConcept& CIntConcept::operator = (int iAValue) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
    ClearCurrentHypSet();
    // add the hypothesis (this will notify the change)
    AddHyp(new CIntHyp(iAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: assignment operator from float
CConcept& CIntConcept::operator = (float fAValue) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
    ClearCurrentHypSet();
    // add the hypothesis (this will notify the change)
    AddHyp(new CIntHyp((int)fAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: assignment operator from double
CConcept& CIntConcept::operator = (double dAValue) {
    // check that it's not a history concept
    if(bHistoryConcept) 
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
    ClearCurrentHypSet();
    // add the hypothesis (this will notify the change)
    AddHyp(new CIntHyp((int)dAValue, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: bool cast operator: returns 1 or 0
CIntConcept::operator bool() {
    return (operator int() != 0);
}

// D: integer cast operator: returns the max confidence value in the set 
CIntConcept::operator int() {
    if(IsUpdatedAndGrounded() && GetTopHyp()) {
		return ((CIntHyp *)GetTopHyp())->iValue;
	} else {
        return 0;
	}
}

// D: returns an empty clone of the concept (basically just preserving the
//    type, but not the contents
CConcept* CIntConcept::EmptyClone() {
    return new CIntConcept;
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the current hypothesis set and belief
// manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CIntConcept::HypFactory() {
	return new CIntHyp();
}
