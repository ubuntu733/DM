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
// STRINGCONCEPT.CPP - implementation of the CStringConcept class
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
//   [2004-05-19] (dbohus): major makeover on concepts, introduced frame 
//                           concepts and structure concepts which support 
//                           grounding
//   [2004-05-15] (dbohus): cleaned up concepts code, added Update as well as
//                           various types of update methods
//   [2004-04-25] (dbohus): added WaitingConveyance flag and functionality
//   [2004-04-16] (dbohus): removed grounding models from constructors and 
//                           added them as a separate method
//   [2003-04-14] (dbohus): added support for OwnerDialogAgent
//   [2003-04-03] (dbohus): added support for concept grounding models
//   [2003-04-01] (dbohus): added support for iTurnLastUpdated
//   [2003-03-19] (dbohus): eliminated ValueToString, transformed 
//                           ValueToStringWithConf into ValueSetToString
//   [2003-01-29] (dbohus): added equality and comparison operators
//   [2003-01-29] (dbohus): added CString define
//   [2002-12-09] (dbohus): major revamp on concepts: added support for 
//                           reopening concepts and maintaing a history of
//                           previous values 
//   [2002-12-01] (dbohus): fixed bug in CStringHyp::FromString
//   [2002-12-01] (dbohus): added SetValueFromString and UpdateValueFromString
//                           functions on concepts
//   [2002-11-26] (dbohus): fixed bug in assign operator when assigning to self
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2002-02-10] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "StringConcept.h"
#include "DMCore/Log.h"

//-----------------------------------------------------------------------------
// CStringHyp class - this class keeps a pair: <CString-value, conf>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CStringHyp: Constructors and Destructors
//-----------------------------------------------------------------------------

// D: default constructor
CStringHyp::CStringHyp() {
	ctHypType = ctString;
	sValue = "";
	fConfidence = 0.0;
}

// D: copy constructor
CStringHyp::CStringHyp(CStringHyp& rAStringHyp) {
	ctHypType = ctString;
	sValue = rAStringHyp.sValue;
	fConfidence = rAStringHyp.fConfidence;
}

// D: from value + confidence constructor
CStringHyp::CStringHyp(string sAValue, float fAConfidence) {
	ctHypType = ctString;
	sValue = sAValue;
	fConfidence = fAConfidence;
}

//-----------------------------------------------------------------------------
// CStringHyp: Overwritten, CStringHyp specific methods
//-----------------------------------------------------------------------------

// D: assignment operator from string
CHyp& CStringHyp::operator = (string sAValue) {
	sValue = sAValue;
	fConfidence = 1.0;
	return *this;
}

// D: assignment operator from char*
CHyp& CStringHyp::operator = (char *lpszValue) {
	string aValue(lpszValue);
	sValue = aValue;
	fConfidence = 1.0;
	return *this;
}

// D: assignment operator from another ValConf
CHyp& CStringHyp::operator = (CHyp& rAHyp) {
    if(&rAHyp != this) {
	    // check the type
	    if(rAHyp.GetHypType() != ctString) {
		    // if it's not a string, signal an error
		    FatalError("Assignment operator from a different hyp type called on "\
			    "string hyp. Cannot perform conversion.");
		    return *this;
	    }

	    CStringHyp& rAStringHyp = (CStringHyp&)rAHyp;
	    sValue = rAStringHyp.sValue;
	    fConfidence = rAStringHyp.fConfidence;
    }
	return *this;
}

// D: equality operator
bool CStringHyp::operator == (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctString) {
		// if it's not a string, signal an error
		FatalError("Equality operator with a different hyp type called on "\
		  	  "string hyp. Cannot perform conversion.");
		return false;
	}

    return sValue == ((CStringHyp&)rAHyp).sValue;
}

// D: Comparison operator
#pragma warning (disable:4100)
bool CStringHyp::operator < (CHyp& rAHyp) {    
    // signal an error
	FatalError("Comparison operator < called on CStringHyp.");
	return false;
}

// D: Comparison operator
bool CStringHyp::operator > (CHyp& rAHyp) {    
    // signal an error
	FatalError("Comparison operator > called on CStringHyp.");
	return false;
}

// D: Comparison operator
bool CStringHyp::operator <= (CHyp& rAHyp) {    
    // signal an error
	FatalError("Comparison operator <= called on CStringHyp.");
	return false;
}

// D: Comparison operator
bool CStringHyp::operator >= (CHyp& rAHyp) {    
    // signal an error
	FatalError("Comparison operator >= called on CStringHyp.");
	return false;
}
#pragma warning (default:4100)

// D: Indexing operator - return an error, should never be called
CHyp* CStringHyp::operator [] (string sItem) {
	FatalError("Indexing operator [] called on CStringHyp.");
    return NULL;
}

// D: Convert value to string
string CStringHyp::ValueToString() {
	return sValue;
}

// D: Convert hyp to string
string CStringHyp::ToString() {
	return sValue + FormatString("%s%.2f", VAL_CONF_SEPARATOR, fConfidence);
}

// D: Get the hyp from a string
void CStringHyp::FromString(string sString) {
	// separate the string into value and confidence 
	string sConfidence;
	SplitOnFirst(sString, VAL_CONF_SEPARATOR, sValue, sConfidence);
	sValue = Trim(sValue);
	sConfidence = Trim(sConfidence);

	// cut the apostrophes if in there
	if((sValue[0] == '"') && 
	   (sValue[sValue.length()-1] == '"') && 
	   (sValue.length()>1)) {
		sValue = sValue.substr(1, sValue.length()-2);
	}
	
	// get the confidence 
    if(sConfidence == "") {
        fConfidence = 1.0;
    } else {
	    if(sscanf(sConfidence.c_str(), "%f", &fConfidence) != 1) {
		    FatalError(FormatString("Cannot perform conversion to CStringHyp from "\
						    "%s.", sString.c_str()));
	    }
    }
}



//-----------------------------------------------------------------------------
// CStringConcept class - this is the string concept class. It overloads and
//						provides some new constructors, and operators
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CStringConcept: Constructors and destructors
//-----------------------------------------------------------------------------

// D: constructor by name
CStringConcept::CStringConcept(string sAName, TConceptSource csAConceptSource):
    CConcept(sAName, csAConceptSource, DEFAULT_STRING_CARDINALITY) {
	ctConceptType = ctString;
}

// D: constructor from name + value + confidence
CStringConcept::CStringConcept(string sAName, string sAValue, float fAConfidence,
							   TConceptSource csAConceptSource) {
	ctConceptType = ctString;
	csConceptSource = csAConceptSource;
	sName = sAName;
    pOwnerDialogAgent = NULL;
    pOwnerConcept = NULL;
    pGroundingModel = NULL;
    vhCurrentHypSet.push_back(new CStringHyp(sAValue, fAConfidence));
    iNumValidHyps = 1;
    iCardinality = DEFAULT_STRING_CARDINALITY;
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
CConcept& CStringConcept::operator = (bool bAValue) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

	// o/w clear the current value
	ClearCurrentHypSet();  
	// add the hypothesis (this will notify the change)    	
	if(bAValue)
		AddHyp(new CStringHyp("true", 1.0));
	else
		AddHyp(new CStringHyp("false", 1.0));

	// set the concept to grounded
	SetGroundedFlag(true);
    return *this;
}

// D: assignment operator from integer
CConcept& CStringConcept::operator = (int iAValue) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

    // o/w clear the current value
    ClearCurrentHypSet();
    // and fill in the integer
    char lpszBuffer[256];
	sprintf(lpszBuffer, "%d", iAValue);
	// add the hypothesis (this will notify the change)
    AddHyp(new CStringHyp((string)lpszBuffer, 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: assignment operator from float
CConcept& CStringConcept::operator = (float fAValue) {
    // check that it's not a history concept
    if(bHistoryConcept) 
        FatalError(FormatString("Cannot assign to concept (%s) history.",
            sName.c_str()));

    // o/w clear the current value
    ClearCurrentHypSet();
    // and fill in the float
    char lpszBuffer[256];
	sprintf(lpszBuffer, "%.4f", fAValue);
	// add the hypothesis (this will notify the change)
    AddHyp(new CStringHyp(string(lpszBuffer), 1.0));
	// set the concept to grounded
	SetGroundedFlag(true);
	return *this;
}

// D: assignment operator from double
CConcept& CStringConcept::operator = (double dAValue) {
	return operator=((float)dAValue);
}

// D: returns an empty clone of the concept (basically just preserving the
//    type, but not the contents
CConcept* CStringConcept::EmptyClone() {
    return new CStringConcept;
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the current hypothesis set and belief
// manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CStringConcept::HypFactory() {
	return new CStringHyp();
}
