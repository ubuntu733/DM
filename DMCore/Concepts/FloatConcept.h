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
// FLOATCONCEPT.H - implementation of the CFloatConcept class
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

#pragma once
#ifndef __FLOATCONCEPT_H__
#define __FLOATCONCEPT_H__

#include "Concept.h"
#include "Utils/Utils.h"

//-----------------------------------------------------------------------------
// CFloatHyp class - this class keeps a pair: <float-value, conf>
//-----------------------------------------------------------------------------

// D: forward declaration of CFloatConcept class
class CFloatConcept;

class CFloatHyp : public CHyp {

    // declare the CFloatConcept class as a friend
    //
    friend class CFloatConcept;

protected:

    //---------------------------------------------------------------------
    // Protected member variables 
	//---------------------------------------------------------------------
	//
	float fValue;                   // the actual value

public:

    //---------------------------------------------------------------------
    // Constructors and Destructors
	//---------------------------------------------------------------------
	//
	CFloatHyp();
	CFloatHyp(CFloatHyp& rAFloatHyp);
	CFloatHyp(float fAValue, float fAConfidence = 1.0);

    //---------------------------------------------------------------------
	// Overwritten, CFloatHyp specific virtual functions
	//---------------------------------------------------------------------

    // Assignment operators
	// 
	CHyp& operator = (float fAValue);
	virtual CHyp& operator = (CHyp& rAHyp);

    // Comparison operators
    //
    virtual bool operator == (CHyp& rAHyp);
    virtual bool operator < (CHyp& rAHyp);
    virtual bool operator > (CHyp& rAHyp);
    virtual bool operator <= (CHyp& rAHyp);
    virtual bool operator >= (CHyp& rAHyp);

    // Indexing operator
    //
    virtual CHyp* operator [](string sItem);

	// String conversion functions
    // 
	virtual string ValueToString();
	virtual string ToString();
	virtual void FromString(string sString);
};

//-----------------------------------------------------------------------------
// CFloatConcept class - this is the float concept class. It overloads and
//						provides some new constructors, and operators
//-----------------------------------------------------------------------------

// define the cardinality of the float class of concepts
#define DEFAULT_FLOAT_CARDINALITY 1000

// first a shortcut define for int values 
#define CFloat(Value) (CConcept &)CFloatConcept((string)"NONAME", Value, 1, csSystem)

class CFloatConcept: public CConcept {

public:
	
    //---------------------------------------------------------------------
    // Constructors and Destructors
	//---------------------------------------------------------------------
	//
	CFloatConcept(string sAName = "NONAME", 
				  TConceptSource csAConceptSource = csUser);
	CFloatConcept(string sAName, float fAValue, float fAConfidence = 1.0,
				  TConceptSource csAConceptSource = csUser);

	//---------------------------------------------------------------------
    // Overwritten methods for overall concept manipulation 
	//---------------------------------------------------------------------
	
    // overwritten operators
    //
	virtual CConcept& operator = (int iAValue);
	virtual CConcept& operator = (bool bAValue);
	virtual CConcept& operator = (float fAValue);
	virtual CConcept& operator = (double dAValue);
	virtual operator bool();
    virtual operator int();
	virtual operator float();

    // overwritten method for constructing an empty clone of the concept
    virtual CConcept* EmptyClone();

    //---------------------------------------------------------------------
    // Overwritten methods related to the current hypothesis set and belief
    // manipulation
	//---------------------------------------------------------------------
	
    // factory method for hypotheses
    virtual CHyp* HypFactory();
};

#endif // __FLOATCONCEPT_H__