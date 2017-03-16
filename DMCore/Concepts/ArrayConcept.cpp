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
// ARRAYCONCEPT.CPP - implementation of the CArrayConcept class
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
//   [2004-12-06] (antoine): fixed inconsistencies so that an array is always
//                           considered as an atomic concept when reopened,
//                           tested for availability, etc.
//   [2004-11-11] (jsherwan):added support for insertAt(i, x) to insert at the 
//                            end of an array setting i = n
//   [2004-06-15] (dbohus):  fixed bug in IsUndergoingGrounding
//   [2004-06-02] (dbohus):  added definition of pOwnerConcept, concepts now
//                            check with parent if unclear if they are
//                            grounded
//   [2004-05-30] (dbohus):  changed definition of IsGrounded
//   [2004-05-19] (dbohus):  major makeover on concepts, introduced frame 
//                            concepts and structure concepts which support 
//                            grounding
//   [2004-05-15] (dbohus):  cleaned up concepts code, added Update as well as
//                            various types of update methods
//   [2004-04-20] (jsherwan):added InsertAt() and debugged Delete()
//   [2004-03-29] (dbohus):  fixed some problems in the copy constructor
//   [2004-04-25] (dbohus):  added WaitingConveyance flag and functionality
//   [2004-04-18] (dbohus):  renamed MergedHistory to 
//                            CreateMergedHistoryConcept and added a 
//                            MergeHistory function which merges the history
//                            up into the current value for a concept
//   [2004-04-17] (dbohus):  added support for declaring the grounding models
//                            subsumed by the concept
//   [2004-04-16] (dbohus):  removed grounding models from constructors and 
//                            added them as a separate method
//   [2003-05-28] (antoine): changed ReOpen so that it clears the current value
//   [2003-04-28] (antoine): added the == operator
//   [2003-04-14] (dbohus):  added support for OwnerDialogAgent
//   [2003-04-12] (dbohus):  fixed bug in Restore
//   [2003-04-10] (dbohus):  added Restore on concepts
//   [2003-04-03] (dbohus):  added support for concept grounding models
//   [2003-04-01] (dbohus):  added support for iTurnLastUpdated
//   [2003-03-24] (antoine): implemented MergedHistory
//   [2003-03-19] (dbohus):  eliminated ValueToString, transformed 
//                            ValueToStringWithConf into ValueSetToString
//   [2003-03-13] (antoine): modified *ValueToString, SetValueFromString and 
//                            UpdateValueFromString so that when elements are 
//                            atomic, they do not need to be surrounded by 
//                            braces '{' and '}' (it is now one element per 
//                            line in this case)
//   [2003-03-13] (antoine): added ModeValueToString
//   [2003-01-29] (dbohus):  added equality and comparison operators
//   [2002-12-09] (dbohus):  major revamp on concepts: added support for 
//                            reopening concepts and maintaing a history of
//                            previous values 
//   [2002-12-01] (dbohus):  added SetValueFromString and UpdateValueFromString
//                            functions on concepts
//   [2002-11-26] (dbohus):  fixed bug in assign operator when assigning to 
//                            self
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-23] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#include "ArrayConcept.h"
#include "DMCore/Log.h"
#include "DMCore/DMCore.h"

//-----------------------------------------------------------------------------
// CArrayConcept class - this is an array of concepts class. Note that it
//                       can actually store a heterogeneous array, but we 
//                       recommend using heterogeneour arrays with much care.
//-----------------------------------------------------------------------------

#pragma warning (disable:4100)

//-----------------------------------------------------------------------------
// CArrayConcept: Constructors and Destructors
//-----------------------------------------------------------------------------
// D: default constructor
CArrayConcept::CArrayConcept(string sAName, TConceptSource csAConceptSource): 
	CConcept(sAName, csAConceptSource) {
	// set the concept type
	ctConceptType = ctArray;
	// set it to grounded (array concepts are always grounded)
	bGrounded = true;
	// reset the cardinality to -1
	iCardinality = -1;
	// initialize the grounding model spec for the elements
	sElementGroundingModelSpec = "";
}

// D: destructor
CArrayConcept::~CArrayConcept() {
	// destroy all the elements in the array
	for(unsigned int i = 0; i < ConceptArray.size(); i++) {
		if(ConceptArray[i] != NULL) 
			delete ConceptArray[i];
	}
    ConceptArray.clear();

    // and destroy the history
    if(pPrevConcept != NULL)
        delete pPrevConcept;
	pPrevConcept = NULL;
    // and the grounding model (if any)
	if(pGroundingModel != NULL) {
		// remove any issued grounding requests
		pGroundingManager->RemoveConceptGroundingRequest(this);
		// and delete the grounding model
        delete pGroundingModel;
	}
	pGroundingModel = NULL;

    // remove conveyance notifications from outputmanager
    if(bWaitingConveyance) ClearWaitingConveyance();
    
    // clear the concept notification pointer
    ClearConceptNotificationPointer();
}


//-----------------------------------------------------------------------------
// Overwritten methods for overall concept manipulation
//-----------------------------------------------------------------------------

// D: equality operator
bool CArrayConcept::operator == (CConcept& rAConcept) {

	// the two arrays must have the same size to be equal
	if (ConceptArray.size() != (unsigned)rAConcept.GetSize())
		return false;
	
	// compare all elements
	for(unsigned int i=0; i<ConceptArray.size(); i++)
		if(*ConceptArray[i] != rAConcept[i])
			return false;

	return true;
}

// D: Comparison operator
bool CArrayConcept::operator < (CConcept& rAConcept) {
    Error("Comparison operator < called on array concept.");
    return false;
}

// D: Comparison operator
bool CArrayConcept::operator > (CConcept& rAConcept) {
    Error("Comparison operator > called on array concept.");
    return false;
}

// D: Comparison operator
bool CArrayConcept::operator <= (CConcept& rAConcept) {
    Error("Comparison operator <= called on array concept.");
    return false;
}

// D: Comparison operator
bool CArrayConcept::operator >= (CConcept& rAConcept) {
    Error("Comparison operator >= called on array concept.");
    return false;
}


// D: indexing operator with integer argument
CConcept& CArrayConcept::operator [](int iIndex) {
    // if a negative index into history, deal with that
    if(iIndex < 0) {
        if(pPrevConcept != NULL) {
            // if adressing previous in history, return that
            if(iIndex == -1) {
                return *pPrevConcept;
            } else {
                // o/w go recursively
                return pPrevConcept->operator [](iIndex + 1);
            }
        } else {
            // o/w index if out of history bounds
	        FatalError(FormatString("Indexing operator [%d] on %s "\
                "(array) failed.", iIndex, sName.c_str()));
	        return NULLConcept;	
        }
    } 

    // o/w check if it's within the array bounds
	if(iIndex < (int) ConceptArray.size()) {
		// a concept within the array limits is needed, so return it
		return (*ConceptArray[iIndex]);
	} 
       
	// o/w a concept outside the current array bounds is needed, so
	// we need to extend the array. 

    // First check that we're not in a history concept
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform array extension on "\
            "concept (%s) history.", sName.c_str()));

	// then expand the array
	int iArraySize = ConceptArray.size();
	for(int i=0; i < iIndex - iArraySize + 1; i++) {
		CConcept* pAConcept = CreateElement();	
		pAConcept->SetChangeNotification(bChangeNotification);
		pAConcept->SetName(FormatString("%s.%d", sName.c_str(), 
										i + iArraySize));
        pAConcept->SetOwnerDialogAgent(pOwnerDialogAgent);
        pAConcept->SetOwnerConcept(this);
		ConceptArray.push_back(pAConcept);
	}
	return (*ConceptArray[iIndex]);
}

// D: indexing operator on string
CConcept& CArrayConcept::operator [](string sIndex) {
	if(sIndex == "") {
		// if it's actually not indexed, return the whole array
		return *this;
	} else {
		// in case we might have a complex subitem, we want to split on the 
		// first . (i.e. hotellist.0.name)
		string sBaseConcept, sRest;
		SplitOnFirst(sIndex, ".", sBaseConcept, sRest);

		// now since we're in an array, the baseconcept should be %d
		int iIndex;
		if(sscanf(sBaseConcept.c_str(), "%d", &iIndex) != 1) {
			// if it's not in that form, then we fail
			FatalError(FormatString(
                "Indexing operator on %s.%s failed.", 
				sName.c_str(), sIndex.c_str()));
			return NULLConcept;
		}

		// now that we have the index, return the indexed concept
		return operator[](iIndex).operator [](sRest);
	}
}

// D: Clones the array concept
CConcept* CArrayConcept::Clone(bool bCloneHistory) {

	// start with an empty clone
	CArrayConcept* pConcept = (CArrayConcept *)EmptyClone();

	// set the other members
    pConcept->SetConceptType(ctConceptType);
    pConcept->SetConceptSource(csConceptSource);
    pConcept->sName = sName;
    pConcept->pOwnerDialogAgent = pOwnerDialogAgent;
	pConcept->SetOwnerConcept(pOwnerConcept);
    // a clone (and arrays for that matter) has no grounding model
	pConcept->pGroundingModel = NULL;
	pConcept->sElementGroundingModelSpec = "";
	pConcept->bSealed = bSealed;
	// a clone does not notify of changes
	pConcept->DisableChangeNotification();
	// now clone the contents of the array
	for(unsigned int i = 0; i < ConceptArray.size(); i++) {
		pConcept->ConceptArray.push_back(ConceptArray[i]->Clone());
		// and reassign the owner concept 
		pConcept->ConceptArray.back()->SetOwnerConcept(pConcept);
	}
	// and set the grounded flag
	pConcept->SetGroundedFlag(bGrounded);
    pConcept->SetTurnLastUpdated(iTurnLastUpdated);
    pConcept->cConveyance = cConveyance;
	// a clone does not wait for conveyance
    pConcept->bWaitingConveyance = false;
    pConcept->SetHistoryConcept(bHistoryConcept);
	if(bCloneHistory && (pPrevConcept != NULL)) 
		pConcept->pPrevConcept = pPrevConcept->Clone();
	else
        pConcept->pPrevConcept = NULL;

	// finally, return the clone
    return pConcept;
}

//-----------------------------------------------------------------------------
// Overwritten protected virtual methods implementing various types of updates
//-----------------------------------------------------------------------------

// D: Naive probabilistic update scheme - assign from string
#pragma warning (disable:4189)
void CArrayConcept::Update_NPU_AssignFromString(void* pUpdateData) {

	// check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (AssignFromString) update on concept "\
            "(%s) history.", sName.c_str()));

	// o/w 
    string sString = *(string *)pUpdateData;
    string sWorkingString = TrimLeft(sString);

	// first clear the current contents of the array
    ClearCurrentHypSet();

	// the array concept should start with ":size {"
	string sStart;
	if(!SplitOnFirst(sWorkingString, "{", sStart, sWorkingString)) {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CArrayConcept>"\
			" %s from string (dump below).\n%s",
			sName.c_str(), sString.c_str()));
	}

	// the sStart string should contain ":size"
	unsigned int uiSize;
	if(sscanf(sStart.c_str(), ":%d", &uiSize) != 1) {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CArrayConcept>"\
			" %s from string (dump below).\n%s",
			sName.c_str(), sString.c_str()));
	}

	// find the corresponding closing bracket
	unsigned int uiPos = FindClosingQuoteChar(sWorkingString, 0, '{', '}');

	// skip tabs/spaces/newlines between the concept
	// and the confidence separator
	while((uiPos < sWorkingString.length() - 1)&&
			((sWorkingString[uiPos] == '\n')||
			(sWorkingString[uiPos] == '\t')||
			(sWorkingString[uiPos] == ' '))) {
		uiPos++;	
	}

	// check if there's anything left after iCount (that should be the 
    // confidence score)
	string sConfidence;
	if(uiPos < sWorkingString.length() - 1) {
    	sConfidence = sWorkingString.substr(uiPos + 1, 
			sWorkingString.length() - uiPos - 1);
		float fConfidence = (float)atof(sConfidence.c_str());
		if(errno == ERANGE) {
			// fail the whole process if we can't find a valid number for confidence
			FatalError(FormatString(
                "Cannot perform update to <CArrayConcept> "\
				"%s from string (dump below).\n%s", sName.c_str(),
				sString.c_str()));
		}
	}

	// now go through all the items and push them into the array
	unsigned int uiElementCount = 0;
	string sElementValue;
	sWorkingString = TrimLeft(sWorkingString);
	while((sWorkingString.length() > 0)&&(sWorkingString[0] != '}')) {
		if (sWorkingString[0] != '{') {
			// the element is atomic get the whole line
			string sStart;
			
			// if we cannot find a newline, signal an error
			if(!SplitOnFirst(sWorkingString, "\n", sStart, sWorkingString)) {
				FatalError(FormatString(
                    "Cannot perform conversion to <CArrayConcept>"\
					" %s from string (dump below).\n%s",
					sName.c_str(), sString.c_str()));
			}

			// get the string corresponding to one element
			sElementValue = Trim( sStart);
		
			// and set the rest of the working string
			sWorkingString = TrimLeft(sWorkingString);
		} else {			
			// the element is a structure, it starts with a '{'

			// identify where the corresponding closing brace is 
			unsigned int iCount = 
                FindClosingQuoteChar(sWorkingString, 1, '{', '}');
	    
			// if we ran out of the string, signal an error
			if(iCount >= sWorkingString.length()) {
				FatalError(FormatString(
                    "Cannot perform conversion to <CArrayConcept>"\
					" %s from string (dump below).\n%s",
					sName.c_str(), sString.c_str()));
			}

			// get the string corresponding to one element
			sElementValue = sWorkingString.substr(1, iCount - 2);
		
			// and set the rest of the working string
			sWorkingString = TrimLeft(sWorkingString.substr(iCount + 1, 
                sWorkingString.length() - iCount - 1));
		}

		// create an element of the array
		CConcept* pElement = CreateElement();
		// set the notifications flag
		pElement->SetChangeNotification(bChangeNotification);

        // set its value accordingly 
		if((pElement->GetConceptType() == ctStruct) ||
            (pElement->GetConceptType() == ctFrame)) {
            sElementValue = "{" + sElementValue + "}";
            if(sConfidence != "") 
                sElementValue = sElementValue +
                    VAL_CONF_SEPARATOR + sConfidence;
			pElement->Update(CU_ASSIGN_FROM_STRING, &sElementValue);
		} else {
            if(sConfidence != "") 
                sElementValue = sElementValue + 
                    VAL_CONF_SEPARATOR + sConfidence;
			pElement->Update(CU_ASSIGN_FROM_STRING, &sElementValue);
		}

		// set its name accordingly
		pElement->SetName(FormatString("%s.%d", sName.c_str(), 
            uiElementCount));
        // set its owner dialog agent
        pElement->SetOwnerDialogAgent(pOwnerDialogAgent);
        // set its owner concept
        pElement->SetOwnerConcept(pOwnerConcept);

        // and push it into the array
		ConceptArray.push_back(pElement);

        // finally, increase the element count
		uiElementCount++;
	}

	// and all we have left now should be the '}'
	if(sWorkingString[0] != '}') {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CArrayConcept>"\
			" %s from string (dump below).\n%s",
			sName.c_str(), sString.c_str()));
	}

	// and the number of elements should be equal to size
	if(uiSize != uiElementCount) {
		// fail the conversion, but issue a warning
		FatalError(FormatString(
            "Specified array size (%d) does not match number of"\
			" existing elements (%d) in conversion to <CArrayConcept> "\
			"%s from string (dump below).\n%s",
			uiSize, uiElementCount, sName.c_str(), sString.c_str()));
	}

    // notify the change
    NotifyChange();
}
#pragma warning (default:4189)

// D: Naive probabilistic update scheme - perform update with another concept
void CArrayConcept::Update_NPU_UpdateWithConcept(void* pUpdateData) {
	FatalError(FormatString(
		"UpdateWithConcept call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
}

// D: Update the value of a concept by collapsing it to the mode
void CArrayConcept::Update_NPU_CollapseToMode(void* pUpdateData) {
    // call it for all the elements of the array
    for(int i = 0; i < GetSize(); i++) 
        ConceptArray[i]->Update(CU_COLLAPSE_TO_MODE, pUpdateData);
}

//-----------------------------------------------------------------------------
// Overwritten protected virtual methods implementing various types of updates
// for the calista belief updating scheme
//-----------------------------------------------------------------------------

// D: Naive probabilistic update scheme - perform update with another concept
void CArrayConcept::Update_Calista_UpdateWithConcept(void* pUpdateData) {
	FatalError(FormatString(
		"UpdateWithConcept call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
}

// D: Update the value of a concept by collapsing it to the mode
void CArrayConcept::Update_Calista_CollapseToMode(void* pUpdateData) {
    // call it for all the elements of the array
    for(int i = 0; i < GetSize(); i++) 
        ConceptArray[i]->Update(CU_COLLAPSE_TO_MODE, pUpdateData);
}

//-----------------------------------------------------------------------------
// Overwritten methods implementing various flags (state information) on the 
// concept
//-----------------------------------------------------------------------------

// D: returns true if the array is updated, i.e. its size is > 0
bool CArrayConcept::IsUpdated() {
	return GetSize() > 0;
}

// D: checks if the concept is undergoing grounding
bool CArrayConcept::IsUndergoingGrounding() {
	for(unsigned int i=0; i<ConceptArray.size(); i++) {
		if(ConceptArray[i]->IsUndergoingGrounding()) return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Overwritten methods implementing conversions to string format
//-----------------------------------------------------------------------------

// D: Generate a string representation of the grounded hypothesis (the top
// one, if the concept is grounded)
string CArrayConcept::GroundedHypToString() {
	// the string holding the result
	string sResult = FormatString(":%d\n{\n", ConceptArray.size());

	// go through the items and add them to the string
	for(unsigned int i=0; i < ConceptArray.size(); i++) {
		sResult += ConceptArray[i]->GroundedHypToString();
	}

	// and finally add the closing braces and return 
	sResult += "}\n";
	return sResult;
}

// A: conversion to "mode_value" format
string CArrayConcept::TopHypToString() {
	// the string holding the result
	string sResult = FormatString(":%d\n{\n", ConceptArray.size());

	// go through the items and add them to the string
	for(unsigned int i=0; i < ConceptArray.size(); i++) {
		sResult += ConceptArray[i]->TopHypToString();
	}

	// and finally add the closing braces and return 
	sResult += "}\n";
	return sResult;
}

// D: conversion to value/conf;value/conf... format
string CArrayConcept::HypSetToString() {
	// the string holding the result
	string sResult = FormatString(":%d\n{\n", ConceptArray.size());

	// go through the items and add them to the string
	for(unsigned int i=0; i < ConceptArray.size(); i++) {
		sResult += ConceptArray[i]->HypSetToString();
	}

	// and finally add the closing braces and return 
	sResult += "}\n";
	return sResult;
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to concept name
//-----------------------------------------------------------------------------

// D: set the concept name
void CArrayConcept::SetName(string sAName) {
    // call the inherited
    CConcept::SetName(sAName);
	// then set the name for all subitems
    for(unsigned int i=0; i < ConceptArray.size(); i++) {
        ConceptArray[i]->SetName(FormatString("%s.%d", 
            sName.c_str(), i));
    }
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to the owner dialog agent
//-----------------------------------------------------------------------------
    
// set the owner dialog agent
void CArrayConcept::SetOwnerDialogAgent(CDialogAgent* pADialogAgent) {
	// call the inherited
	CConcept::SetOwnerDialogAgent(pADialogAgent);
	// then set the owner an all subitems
    for(unsigned int i=0; i < ConceptArray.size(); i++) {
        ConceptArray[i]->SetOwnerDialogAgent(pADialogAgent);
    }
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the grounding model
//-----------------------------------------------------------------------------

// D: Create a grounding model
void CArrayConcept::CreateGroundingModel(string sGroundingModelSpec) {
	// partition the spec by commas
	TStringVector vsGMSpecs = PartitionString(sGroundingModelSpec, ",");
	// and trim it
	string sThisSpec = "";
	for(unsigned int i = 0; i < vsGMSpecs.size(); i++) {
		vsGMSpecs[i] = TrimLeft(vsGMSpecs[i]);
		// check if it's for the current item
		if((sThisSpec == "") && (vsGMSpecs[i].find('=') == -1)) {
			sThisSpec = vsGMSpecs[i];
		}
	}

	// create the grounding model for this array
	if(!(pGroundingManager->GetConfiguration().bGroundConcepts) || 
		(sThisSpec == "none") || (sThisSpec == "")) {
        pGroundingModel = NULL;
    } else {
        FatalError(FormatString(
			"Could not create a grounding model for array concept"
			" %s. Array concepts cannot undergo grounding.",
			GetAgentQualifiedName().c_str()));
		return;
    }

	// A: look for a default element grounding model for this frame
	//    if several @default appear, use the last one (this happens
	//    when an array inherits the default grounding model of its 
	//    parent frame/array but the user also defines the child's 
	//    default)
	string sDefaultGroundingModelSpec = "";
	for(unsigned int i = 0; i < vsGMSpecs.size(); i++) {
		string sHead, sRest;
		SplitOnFirst(vsGMSpecs[i], " =.", sHead, sRest);
		if(sHead == "@default") {
			sRest = TrimLeft(sRest);
			if ((sRest.length()>0)&&(sRest[0]=='=')) {
				sDefaultGroundingModelSpec = sRest.substr(1,sRest.length()-1);
			}
		}
	}

	// now go through each of the specs
	string sElementGroundingModelSpec = "";

	// A: propagate the default grounding model to the elements
	if (sDefaultGroundingModelSpec != "") {
		sElementGroundingModelSpec = "@default=" + sDefaultGroundingModelSpec + ", ";
	}

	for(unsigned int i = 0; i < vsGMSpecs.size(); i++) {
		if((vsGMSpecs[i] != "") && (vsGMSpecs[i][0] == '#')) {
			string sReminder = 
				vsGMSpecs[i].substr(1, vsGMSpecs[i].length() - 1);
			if((sReminder[0] == '.') || (sReminder[0] == '=')) {
				sElementGroundingModelSpec += 
					sReminder.substr(1, sReminder.length() - 1) + ", ";
			} else {
				FatalError(FormatString(
					"Cannot create grounding model on concept %s. Specification (%s)"\
					" cannot be parsed.", GetAgentQualifiedName().c_str(), 
					sGroundingModelSpec.c_str()));
			}
		}
	}

	// finally, trim the end of the string
	sElementGroundingModelSpec = TrimRight(sElementGroundingModelSpec, ", ");

	// and create the grounding models on all subelements
	for(unsigned int i = 0; i < ConceptArray.size(); i++)
		ConceptArray[i]->CreateGroundingModel(sElementGroundingModelSpec);
}

// D: Declare the grounding models subsumed by this concept
void CArrayConcept::DeclareGroundingModels(
    TGroundingModelPointersVector& rgmpvModels,
    TGroundingModelPointersSet& rgmpsExclude) {

    // first call the inherited method
    CConcept::DeclareGroundingModels(rgmpvModels, rgmpsExclude);

    // then add the grounding models for the subsumed concepts
    for(int i = 0; i < GetSize(); i++) 
        ConceptArray[i]->DeclareGroundingModels(rgmpvModels, rgmpsExclude);
}

// D: Declare the subsumed concepts
void CArrayConcept::DeclareConcepts(
    TConceptPointersVector& rcpvConcepts, 
    TConceptPointersSet& rcpsExclude) {

    // just add the current concept
	CConcept::DeclareConcepts(rcpvConcepts, rcpsExclude);

    // then go through all the items and have them declare the concepts, too
    for(int i = 0; i < GetSize(); i++) 
        ConceptArray[i]->DeclareConcepts(rcpvConcepts, rcpsExclude);
}

//-----------------------------------------------------------------------------
// Methods related to signaling concept changes
//-----------------------------------------------------------------------------

// D: Set the concept change notifications flag
void CArrayConcept::SetChangeNotification(
	bool bAChangeNotification) {
	bChangeNotification = bAChangeNotification;
	// and set it for all children
	for(int i = 0; i < GetSize(); i++) {
		ConceptArray[i]->SetChangeNotification(
			bAChangeNotification);
	}
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the current hypotheses set and belief 
// manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CArrayConcept::HypFactory() {
	FatalError(FormatString(
		"HypFactory call on array concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
	return NULL;
}

// D: adds a hypothesis to the current set of hypotheses
int CArrayConcept::AddHyp(CHyp* pAHyp) {
	FatalError(FormatString(
		"AddHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
	return -1;
}

// D: adds a new hypothesis to the current set of hypotheses
int CArrayConcept::AddNewHyp() {
	FatalError(FormatString(
		"AddNewHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
	return -1;
}

// D: adds a null hypothesis to the current set of hypotheses
int CArrayConcept::AddNullHyp() {
	FatalError(FormatString(
		"AddNullHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
	return -1;
}

// D: sets a hypothesis into a location
void CArrayConcept::SetHyp(int iIndex, CHyp* pHyp) {
	FatalError(FormatString(
		"SetHyp call on frame concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
}

// D: sets a null hypothesis into a location
void CArrayConcept::SetNullHyp(int iIndex) {
	FatalError(FormatString(
		"SetNullHyp call on frame concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
}

// D: deletes a hypothesis
void CArrayConcept::DeleteHyp(int iIndex) {
	FatalError(FormatString(
		"DeleteHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
}

// D: return the hypothesis at a given index
CHyp* CArrayConcept::GetHyp(int iIndex) {
	FatalError(FormatString(
		"GetHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
    return NULL;
}

// D: finds the index of a given hypothesis
int CArrayConcept::GetHypIndex(CHyp* pHypIndex) {
	FatalError(FormatString(
		"GetHypIndex call on array concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
    return NULL;
}

// D: return the confidence of a certain hypothesis (specified by index)
float CArrayConcept::GetHypConfidence(int iIndex) {
	FatalError(FormatString(
		"GetHypConfidence call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
    return 0;
}

// D: return the top hypothesis
CHyp* CArrayConcept::GetTopHyp() {
	FatalError(FormatString(
		"GetTopHyp call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
    return NULL;
}

// D: return the top hyp index
int CArrayConcept::GetTopHypIndex() {
	FatalError(FormatString(
		"GetTopHypIndex call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
    return -1;
}

// D: return the confidence score of the top hypothesis
float CArrayConcept::GetTopHypConfidence() {
	FatalError(FormatString(
		"GetTopHypConfidence call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
    return 0;
}

// D: check if a hypothesis is valid (confidence score > 0; valus is not 
// NULL)
bool CArrayConcept::IsValidHyp(int iIndex) {
	FatalError(FormatString(
		"IsValidHyp call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
    return false;
}

// D: return the total number of hypotheses for a concept (including 
// NULLs)
int CArrayConcept::GetNumHyps() {
	FatalError(FormatString(
		"GetNumHyps call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
    return -1;
}

// D: return the number of valid hypotheses for a concept
int CArrayConcept::GetNumValidHyps() {
	FatalError(FormatString(
		"GetNumValidHyps call on array concept %s failed (array concepts "
        "do not support hypsets).", sName.c_str()));
    return -1;
}

// D: clear the current set of hypotheses for the concept
void CArrayConcept::ClearCurrentHypSet() {

	// if the array is empty, return
    if(ConceptArray.size() == 0) return;

	// o/w deallocate all the concepts in the array
	for(unsigned int i = 0; i < ConceptArray.size(); i++) {
		if(ConceptArray[i] != NULL) 
			delete ConceptArray[i];
	}

	// clear the array
    ConceptArray.clear();

    // notify the change
    NotifyChange();
}

// D: copies the current set of hypotheses from another concept
void CArrayConcept::CopyCurrentHypSetFrom(CConcept& rAConcept) {
    
	// first clear it
    ClearCurrentHypSet();

    // check that rAConcept is the right type
    if(rAConcept.GetConceptType() != ctArray)
        FatalError(FormatString(
            "CopyCurrentHypSetFrom called with different type (%s) on "\
            "CArrayConcept %s.", 
            ConceptTypeAsString[rAConcept.GetConceptType()].c_str(),
            sName.c_str()));

    // copy the hypsets for the items
    CArrayConcept& rAArrayConcept = (CArrayConcept &)rAConcept;

	// if the other array is empty, return
	if(rAArrayConcept.GetSize() == 0)
		return;

	// o/w go on with the copying
	for(int i = 0; i < rAArrayConcept.GetSize(); i++) {
		// create an element
		CConcept* pAConcept = CreateElement();	
		// set the concept change notifications flag
		pAConcept->SetChangeNotification(bChangeNotification);
		// set the name
		pAConcept->SetName(FormatString("%s.%d", sName.c_str(), i));
		// set the owner dialog agent
        pAConcept->SetOwnerDialogAgent(pOwnerDialogAgent);
		// set the owner concept
        pAConcept->SetOwnerConcept(this);
		// copy the value from the corresponding element in the other
		// array
		pAConcept->CopyCurrentHypSetFrom(rAArrayConcept[i]);
		// push it in the array
		ConceptArray.push_back(pAConcept);
	}

    // notify the change
    NotifyChange();
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to turn last updated information
//-----------------------------------------------------------------------------

// D: Access to turn in which the concept was last updated information
int CArrayConcept::GetTurnLastUpdated() {
	
	// go through each element and check when it was last updated
    int iTurn = -1;
    for(unsigned int i = 0; i < ConceptArray.size(); i++) {
		if (iTurn < ConceptArray[i]->GetTurnLastUpdated()) {
			iTurn = ConceptArray[i]->GetTurnLastUpdated();
		}
	}
	
    return iTurn;
}

//-----------------------------------------------------------------------------
// Overwritten methods for history manipulation
//-----------------------------------------------------------------------------

// D: set the history concept flag
void CArrayConcept::SetHistoryConcept(bool bAHistoryConcept) {
	bHistoryConcept = bAHistoryConcept;
	// and set it for all the subconcepts
	for(unsigned int i = 0; i < ConceptArray.size(); i++)
		ConceptArray[i]->SetHistoryConcept(bAHistoryConcept);
}


//-----------------------------------------------------------------------------
// Overwritten methods that are array-specific
//-----------------------------------------------------------------------------

// D: getsize method: 
int CArrayConcept::GetSize() {
	return ConceptArray.size();
}

// D: DeleteAt method
void CArrayConcept::DeleteAt(unsigned int iIndex) {
    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform DeleteAt on concept (%s) history.",
            sName.c_str()));

	// o/w 
	if(iIndex >= ConceptArray.size()) 
		// an index outside the array limits was provided
		FatalError(FormatString("Index (%d) out of bounds in DeleteAt on concept %s.", 
						iIndex, sName.c_str()));

    // destroy the concept at the index
	delete (ConceptArray[iIndex]);

	// delete the element in the array
    ConceptArray.erase(ConceptArray.begin() + iIndex);

	// fix the name numbering of the rest of the elements in the vector
	for (iIndex++; iIndex < ConceptArray.size(); iIndex++)
		ConceptArray[iIndex]->SetName(FormatString("%s.%d", \
			sName.c_str(), iIndex));

    // notify the change
    NotifyChange();
}

// J: InsertAt() method: inserts the element in the array at a particular 
//    position
void CArrayConcept::InsertAt(unsigned int iIndex, CConcept &rAConcept) {

    // check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform InsertAt on concept (%s) history.",
            sName.c_str()));

	// o/w 
	if(iIndex > ConceptArray.size())
		// an index outside the array limits was provided
		FatalError(FormatString("Index (%d) out of bounds in InsertAt on concept %s.", 
						iIndex, sName.c_str()));

	// create a blank Concept and copy stuff from rAConcept into it
	CConcept* pAConcept = CreateElement();
	// set the change notifications flag
	pAConcept->SetChangeNotification(bChangeNotification);
    *pAConcept = rAConcept;
	pAConcept->SetName(FormatString("%s.%d", sName.c_str(), iIndex));
    pAConcept->SetOwnerDialogAgent(pOwnerDialogAgent);
    pAConcept->SetOwnerConcept(pOwnerConcept);

	// check if we want to push it at the end of the array
	if (iIndex==ConceptArray.size())
		ConceptArray.push_back(pAConcept);
	else // insert the element in the array
		ConceptArray.insert(ConceptArray.begin() + iIndex, pAConcept);

	// fix the name numbering of the rest of the elements in the vector
	for (iIndex++; iIndex < ConceptArray.size(); iIndex++)
		ConceptArray[iIndex]->SetName(FormatString("%s.%d", \
			sName.c_str(), iIndex));

    // notify the change
    NotifyChange();
}

#pragma warning (default:4100)
