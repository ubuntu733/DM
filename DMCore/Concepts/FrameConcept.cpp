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
// FRAMECONCEPT.CPP - implementation of the CFrameConcept class. This class 
//                    implements a frame, i.e. a collection of attribute-values.
//                    In contrast to the CStructConcept class, this class does
//                    not undergo grounding
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
//   [2006-06-15] (antoine): merged Calista belief updating functions from
//                           RavenClaw1
//   [2006-01-01] (antoine): branched for RavenClaw2, added support for
//							 partial hypotheses
//   [2004-06-15] (dbohus):  fixed bug in IsUndergoingGrounding
//   [2004-06-02] (dbohus):  added definition of pOwnerConcept, concepts now
//                            check with parent if unclear if they are
//                            grounded
//   [2004-05-30] (dbohus):  changed definition of IsGrounded
//   [2004-05-19] (dbohus):  major makeover on concepts, introduced frame 
//                            concepts and structure concepts which support 
//                            grounding
//   [2004-05-18] (dbohus):  renamed this to the CFrameConcept class
//   [2004-05-15] (dbohus):  cleaned up concepts code, added Update as well as
//                            various types of update methods
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
//   [2003-04-28] (antoine): implemented the == operator
//   [2003-04-14] (dbohus):  added support for OwnerDialogAgent
//   [2003-04-12] (dbohus):  fixed bug in Restore
//   [2003-04-10] (dbohus):  added Restore on concepts
//   [2003-04-03] (dbohus):  added support for concept grounding models
//   [2003-04-02] (antoine): overloaded GetTurnLastUpdated and 
//							 GetTurnsSinceUpdated from CConcept
//   [2003-04-01] (dbohus):  added support for iTurnLastUpdated
//   [2003-03-19] (dbohus):  eliminated ValueToString, transformed 
//                            ValueToStringWithConf into ValueSetToString
//   [2003-03-13] (antoine): added ModeValueToString
//   [2003-01-29] (dbohus):  added equality and comparison operators
//   [2002-12-09] (dbohus):  major revamp on concepts: added support for 
//                            reopening concepts and maintaing a history of
//                            previous values 
//   [2002-12-01] (dbohus):  added SetValueFromString and UpdateValueFromString
//                            functions on concepts
//   [2002-11-26] (dbohus):  fixed bug in assign operator when assigning to self
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-02-10] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#include "FrameConcept.h"
#include "DMCore/Log.h"
#include "DMCore/DMCore.h"

#pragma warning (disable:4100)

//-----------------------------------------------------------------------------
// CFrameConcept class - this is the frame concept class. It holds a 
//                       record of concepts of various types. It serves as a
//                       hierarchy basis for classes which implement 
//                       user-specified frame concepts. Frame concepts do not
//                       undergo grounding, and hypotheses at the frame level
//                       are not defined
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CFrameConcept: Constructors and destructors
//-----------------------------------------------------------------------------
// D: constructor
CFrameConcept::CFrameConcept(string sAName, TConceptSource csAConceptSource): 
	CStructConcept(sAName, csAConceptSource) {
	// call create structure to initialize the ItemMap accordingly
	ctConceptType = ctFrame;
	// set it to grounded (frame concepts are always grounded)
	bGrounded = true;
	// reset the cardinality to -1
	iCardinality = -1;
}
	
// D: destructor
CFrameConcept::~CFrameConcept() {
    // if this is not a history concept
    if(!bHistoryConcept) 
        // call destroy structure to destroy the current instantiation
	    DestroyStructure();
	else {
	    // o/w simply clear those lists
	    ItemMap.clear();
	    svItems.clear();
	}
    // and recursively destroy previous instantiations
	if(pPrevConcept != NULL)
        delete pPrevConcept;
	pPrevConcept = NULL;
    // and finally the grounding model (if any)
	if(pGroundingModel != NULL) {
		// remove any issued grounding requests
		pGroundingManager->RemoveConceptGroundingRequest(this);
		// and delete the grounding model
        delete pGroundingModel;
	}
	pGroundingModel = NULL;
    // clear waiting for conveyance
    if(bWaitingConveyance) ClearWaitingConveyance();
    // clear the concept notification pointer
    ClearConceptNotificationPointer();
}

//-----------------------------------------------------------------------------
// CConcept: Methods for overall concept manipulation
//-----------------------------------------------------------------------------

// D: equality operator
bool CFrameConcept::operator == (CConcept& rAConcept) {
    // check equality amongst all items
    TItemMap::iterator iPtr;
	for(iPtr = ItemMap.begin(); iPtr != ItemMap.end(); iPtr++) 
		if(*(iPtr->second) != rAConcept[iPtr->first]) {
			return false;
		}
	return true;
}

// D: Comparison operator
bool CFrameConcept::operator < (CConcept& rAConcept) {
    Error("Comparison operator < called on frame concept.");
    return false;
}

// D: Comparison operator
bool CFrameConcept::operator > (CConcept& rAConcept) {
    Error("Comparison operator > called on frame concept.");
    return false;
}

// D: Comparison operator
bool CFrameConcept::operator <= (CConcept& rAConcept) {
    Error("Comparison operator <= called on frame concept.");
    return false;
}

// D: Comparison operator
bool CFrameConcept::operator >= (CConcept& rAConcept) {
    Error("Comparison operator >= called on frame concept.");
    return false;
}

// D: clears the value of the concept
void CFrameConcept::Clear() {
    // first check that it's not a history copy
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform ClearCurrentValue on concept (%s) "\
            "history.", sName.c_str()));

	// o/w delete all it's history
    if(pPrevConcept != NULL) {
        delete pPrevConcept;
        pPrevConcept = NULL;
    }

	// and clear all the concepts in the ItemMap    
	for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->Clear();

    // don't log the update since frames don't have grounding models

    // notify the change
    NotifyChange();
}

// D: clears the current value of the concept
void CFrameConcept::ClearCurrentValue() {
    // first check that it's not a history copy
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform ClearCurrentValue on concept (%s) "\
            "history.", sName.c_str()));

	// o/w clear all the concepts in the ItemMap    
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->ClearCurrentValue();

    // don't log the update since frames don't have grounding models

    // notify the change
    NotifyChange();
}

//-----------------------------------------------------------------------------
// Overwritten methods implementing various types of updates in the 
// naive probabilistic update scheme
//-----------------------------------------------------------------------------

// D: Naive probabilistic update scheme - assign from string
#pragma warning (disable:4189)
void CFrameConcept::Update_NPU_AssignFromString(void* pUpdateData) {

	// first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (AssignFromString) update on "
			"concept (%s) history.", sName.c_str()));
        
	// o/w 
    string sString = *(string *)pUpdateData;
	string sWorkingString = TrimLeft(sString);

	// the first character has to be "{"
	if(sWorkingString[0] != '{') {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CFrameConcept> %s from string "\
            "(dump below).\n%s", sName.c_str(), sString.c_str()));
	}

	// advance over the "{"
	sWorkingString = TrimLeft(
        sWorkingString.substr(1, sWorkingString.length()-1));

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
                "Cannot perform update to <CFrameConcept> "\
				"%s from string (dump below).\n%s",
				sName.c_str(), sString.c_str()));
		}
	}

	// now go through all the lines and convert the items 
	string sLine;
	while(((sLine = Trim(ExtractFirstLine(sWorkingString))) != "}") && 
		(sLine != "") && (sLine[0] != '}')) {

		// split at the equals sign
		string sItem, sValue;
		if(SplitOnFirst(sLine, CONCEPT_EQUALS_SIGN, sItem, sValue)) {
			// if we successfully made the split
			// check if sValue starts with {. 
			if((sValue[0] == '{') || (sValue[0] == ':')) {
				// in this case, we are dealing with a nested structure (or an 
				// array), so identify where it ends, and correct the sValue;

				// basically go forward counting the matching {}s and
				// terminate when the matching one is found
				sWorkingString = TrimLeft(sValue + sWorkingString);
				unsigned int iCount = FindClosingQuoteChar(sWorkingString, 
					sWorkingString.find('{') + 1, '{', '}');

				// if we ran out of the string, signal an error
				if(iCount >= sWorkingString.length()) {
					FatalError(FormatString(
                        "Cannot perform conversion to "\
						"<CFrameConcept> %s from string (dump below).\n%s",
                        sName.c_str(), sWorkingString.c_str()));
				}

				// set the value to the enclosed string
				sValue = sWorkingString.substr(0, iCount);
				// and the working string to whatever was left
				sWorkingString = TrimLeft(sWorkingString.substr(iCount + 1, 
					sWorkingString.length() - iCount - 1));
			}

			// look for that item in the structure
			TItemMap::iterator iPtr;
			iPtr = ItemMap.find(sItem);
			if(iPtr == ItemMap.end()) {
				FatalError(FormatString(
                    "Item %s not found in structured concept %s"\
					" while converting from string. Ignoring.",
					sItem.c_str(), sName.c_str()));
			} else {
				// and if found, update its value accordingly
				if(sConfidence != "") 
					sValue += VAL_CONF_SEPARATOR + sConfidence;
				iPtr->second->Update(CU_ASSIGN_FROM_STRING, &sValue);
			}
		} else {
			// if no equals sign (split unsuccessful), fail the whole process
			FatalError(FormatString(
                "Cannot perform conversion to <CFrameConcept> "\
				"%s from string (dump below).\n%s",
                sName.c_str(), sString.c_str()));
		} 
	}

	// at this point, we should get out of the loop 
	if(sLine[0] != '}') {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CFrameConcept> from"\
			" %s.", sString.c_str()));
	}

    // notify the change
    NotifyChange();
}
#pragma warning (default:4189)

// D: Naive probabilistic update scheme - perform update with another concept
void CFrameConcept::Update_NPU_UpdateWithConcept(void* pUpdateData) {

	// first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (UpdateWithConcept) update on "
			"concept (%s) history.", sName.c_str()));

	CConcept *pConcept = (CConcept *)pUpdateData;

	// check that pConcept is the right type
    if(pConcept->GetConceptType() != ctFrame) {
        FatalError(FormatString(
            "UpdateWithConcept called with different type (%s) on CFrameConcept.", 
            ConceptTypeAsString[pConcept->GetConceptType()].c_str()));
    }

    // apply the UpdateWithConcept operator on all the items
    CFrameConcept* pFrameConcept = (CFrameConcept *)pConcept;
    for(unsigned int i = 0; i < svItems.size(); i++) {
        if(pFrameConcept->ItemMap.find(svItems[i]) == 
            pFrameConcept->ItemMap.end()) {
            FatalError(FormatString(
                "Item %s not found in concept %s during += operator.",
                svItems[i].c_str(), pFrameConcept->GetName().c_str()));
        } else 
            ItemMap[svItems[i]]->Update(CU_UPDATE_WITH_CONCEPT,
				pFrameConcept->ItemMap[svItems[i]]);
    }

    // notify the change
    NotifyChange();
}

// D: Update the value of a concept by collapsing it to the mode
void CFrameConcept::Update_NPU_CollapseToMode(void* pUpdateData) {
    // basically calls collapse to mode on all its subitems
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->Update(CU_COLLAPSE_TO_MODE, pUpdateData);
}

// A: Update the partial value of a concept
void CFrameConcept::Update_PartialFromString(void* pUpdateData) {
	// traverses the frame and updates each sub-concept
	updateFromString(pUpdateData, CU_PARTIAL_FROM_STRING);
}

#pragma warning (disable:4189)
void CFrameConcept::updateFromString(void* pUpdateData, string sUpdateType) {
	// first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (AssignFromString) update on "
			"concept (%s) history.", sName.c_str()));
        
	// o/w 
    string sString = *(string *)pUpdateData;
	string sWorkingString = TrimLeft(sString);

	// the first character has to be "{"
	if(sWorkingString[0] != '{') {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CFrameConcept> %s from string "\
            "(dump below).\n%s", sName.c_str(), sString.c_str()));
	}

	// advance over the "{"
	sWorkingString = TrimLeft(
        sWorkingString.substr(1, sWorkingString.length()-1));

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
                "Cannot perform update to <CFrameConcept> "\
				"%s from string (dump below).\n%s",
				sName.c_str(), sString.c_str()));
		}
	}

	// now go through all the lines and convert the items 
	string sLine;
	while(((sLine = Trim(ExtractFirstLine(sWorkingString))) != "}") && 
		(sLine != "") && (sLine[0] != '}')) {

		// split at the equals sign
		string sItem, sValue;
		if(SplitOnFirst(sLine, CONCEPT_EQUALS_SIGN, sItem, sValue)) {
			// if we successfully made the split
			// check if sValue starts with {. 
			if((sValue[0] == '{') || (sValue[0] == ':')) {
				// in this case, we are dealing with a nested structure (or an 
				// array), so identify where it ends, and correct the sValue;

				// basically go forward counting the matching {}s and
				// terminate when the matching one is found
				sWorkingString = TrimLeft(sValue + sWorkingString);
				unsigned int iCount = FindClosingQuoteChar(sWorkingString, 
					sWorkingString.find('{') + 1, '{', '}');

				// if we ran out of the string, signal an error
				if(iCount >= sWorkingString.length()) {
					FatalError(FormatString(
                        "Cannot perform conversion to "\
						"<CFrameConcept> %s from string (dump below).\n%s",
                        sName.c_str(), sWorkingString.c_str()));
				}

				// set the value to the enclosed string
				sValue = sWorkingString.substr(0, iCount);
				// and the working string to whatever was left
				sWorkingString = TrimLeft(sWorkingString.substr(iCount + 1, 
					sWorkingString.length() - iCount - 1));
			}

			// look for that item in the structure
			TItemMap::iterator iPtr;
			iPtr = ItemMap.find(sItem);
			if(iPtr == ItemMap.end()) {
				FatalError(FormatString(
                    "Item %s not found in structured concept %s"\
					" while converting from string. Ignoring.",
					sItem.c_str(), sName.c_str()));
			} else {
				// and if found, update its value accordingly
				if(sConfidence != "") 
					sValue += VAL_CONF_SEPARATOR + sConfidence;
				iPtr->second->Update(sUpdateType, &sValue);
			}
		} else {
			// if no equals sign (split unsuccessful), fail the whole process
			FatalError(FormatString(
                "Cannot perform conversion to <CFrameConcept> "\
				"%s from string (dump below).\n%s",
                sName.c_str(), sString.c_str()));
		} 
	}

	// at this point, we should get out of the loop 
	if(sLine[0] != '}') {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CFrameConcept> from"\
			" %s.", sString.c_str()));
	}
}
#pragma warning (default:4189)

//-----------------------------------------------------------------------------
// Overwritten methods implementing various types of updates in the 
// calista belief updating scheme
//-----------------------------------------------------------------------------

// D: Calista belief updating scheme - perform update with another concept
void CFrameConcept::Update_Calista_UpdateWithConcept(void* pUpdateData) {

	// first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (UpdateWithConcept) update on "
			"concept (%s) history.", sName.c_str()));

	CConcept *pConcept = (CConcept *)pUpdateData;

	// check that pConcept is the right type
    if(pConcept->GetConceptType() != ctFrame) {
        FatalError(FormatString(
            "UpdateWithConcept called with different type (%s) on CFrameConcept.", 
            ConceptTypeAsString[pConcept->GetConceptType()].c_str()));
    }

    // apply the UpdateWithConcept operator on all the items
    CFrameConcept* pFrameConcept = (CFrameConcept *)pConcept;
    for(unsigned int i = 0; i < svItems.size(); i++) {
        if(pFrameConcept->ItemMap.find(svItems[i]) == 
            pFrameConcept->ItemMap.end()) {
            FatalError(FormatString(
                "Item %s not found in concept %s during += operator.",
                svItems[i].c_str(), pFrameConcept->GetName().c_str()));
        } else 
            ItemMap[svItems[i]]->Update(CU_UPDATE_WITH_CONCEPT,
				pFrameConcept->ItemMap[svItems[i]]);
    }

    // notify the change
    NotifyChange();
}

// D: Update the value of a concept by collapsing it to the mode
void CFrameConcept::Update_Calista_CollapseToMode(void* pUpdateData) {
    // basically calls collapse to mode on all its subitems
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->Update(CU_COLLAPSE_TO_MODE, pUpdateData);
}

//-----------------------------------------------------------------------------
// Overwritten methods implementing various flags (state information) on the 
// concept
//-----------------------------------------------------------------------------

// D: updatedness: so far a frame concept is updated if a least one of 
//    its items is updated
bool CFrameConcept::IsUpdated() {	
	for(unsigned int i = 0; i < svItems.size(); i++)
		if(ItemMap[svItems[i]]->IsUpdated()) {
			return true;
		}
	return false;
}

// D: returns true if the concept is updated and grounded (at least one of its
//    items is updated and grounded)
bool CFrameConcept::IsUpdatedAndGrounded() {
    // if it's not updated, return false
    if(!IsUpdated()) return false;
    
    // o/w if it's updated, check that all updated items are also grounded
    for(unsigned int i = 0; i < svItems.size(); i++) {
        if(ItemMap[svItems[i]]->IsUpdated() && !ItemMap[svItems[i]]->IsUpdatedAndGrounded())
            return false;
    }

    // if it all checks out, return true
	return true;
}

// D: returns true if the concept is available, (at least one of its items is
//    available)
bool CFrameConcept::IsAvailable() {
	for(unsigned int i = 0; i < svItems.size(); i++)
		if(ItemMap[svItems[i]]->IsAvailable()) {
			return true;
		}
	return false;
}

// D: returns true if the concept is available and grounded. (at least one of its 
//    items is available)
bool CFrameConcept::IsAvailableAndGrounded() {

    // first check that it's not currently undergoing grouding
    if(IsUndergoingGrounding()) return false;

    // then check all subitems
	for(unsigned int i = 0; i < svItems.size(); i++)
		if(ItemMap[svItems[i]]->IsAvailableAndGrounded()) {
			return true;
		}
	return false;
}

// D: checks if the frame concept is undergoing grounding (i.e. one of 
//    its members is undergoing grounding)
bool CFrameConcept::IsUndergoingGrounding() { 	
	for(unsigned int i = 0; i < svItems.size(); i++)
		if(ItemMap[svItems[i]]->IsUndergoingGrounding()) {
			return true;
		}
	return false;
}

//-----------------------------------------------------------------------------
// Overwritten methods implementing conversions to string format
//-----------------------------------------------------------------------------

// D: Generate a string representation of the grounded hypothesis (the top
// one, if the concept is grounded)
string CFrameConcept::GroundedHypToString() {
	// the string holding the result
	string sResult = "{\n";

	// go through the items and add them to the string
	for(unsigned int i = 0; i < svItems.size(); i++)
		if(ItemMap[svItems[i]]->IsUpdatedAndGrounded()) {
			sResult += svItems[i] + "\t" + ItemMap[svItems[i]]->GroundedHypToString();
		}

	// and finally add the closing braces and return 
	sResult += "}\n";
	return sResult;
}

// A: conversion to "mode_value" format
string CFrameConcept::TopHypToString() {
	// the string holding the result
	string sResult = "{\n";

	// go through the items and add them to the string
	for(unsigned int i = 0; i < svItems.size(); i++)
		if(ItemMap[svItems[i]]->IsUpdated()) {
			sResult += svItems[i] + "\t" + ItemMap[svItems[i]]->TopHypToString();
		}

	// and finally add the closing braces and return 
	sResult += "}\n";
	return sResult;
}

// D: conversion to value/conf;value/conf... format
string CFrameConcept::HypSetToString() {
	// the string holding the result
	string sResult = "{\n";

	// go through the items and add them to the string
	for(unsigned int i = 0; i < svItems.size(); i++) {
		if(ItemMap[svItems[i]]->IsUpdated()) {
			sResult += svItems[i] + "\t" + 
					   ItemMap[svItems[i]]->HypSetToString();
		}
    }

	// and finally add the closing braces and return 
	sResult += "}\n";
	return sResult;
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the grounding model
//-----------------------------------------------------------------------------

// D: Create a grounding model
void CFrameConcept::CreateGroundingModel(string sGroundingModelSpec) {
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

	// create the grounding model for this frame
    if(!(pGroundingManager->GetConfiguration().bGroundConcepts) ||
		(sThisSpec == "none") || (sThisSpec == "")) {
        pGroundingModel = NULL;
    } else {
        FatalError(FormatString(
			"Could not create a grounding model for frame concept"
			" %s. Frame concepts cannot undergo grounding.",
			GetAgentQualifiedName().c_str()));
		return;
    }

	// A: look for a default element grounding model for this frame
	//    if several @default appear, use the last one (this happens
	//    when a frame inherits the default grounding model of its 
	//    parent frame but the user also defines the child's default)
	string sDefaultGroundingModelSpec = "";
	for(unsigned int i = 0; i < vsGMSpecs.size(); i++) {
		string sHead, sRest;
		SplitOnFirst(vsGMSpecs[i], " =.", sHead, sRest);
		if(sHead == "@default") {
			sRest = TrimLeft(sRest);
			if (sRest.length()>0) {
				sDefaultGroundingModelSpec = sRest;
			}
		}
	}

	// now go through each of the elements of the structure
	for(unsigned int ni = 0; ni < svItems.size(); ni++) {
		// get the item name
		string sItemName = svItems[ni];

		// A: propagate the default grounding model to the items
		string sItemGroundingModelSpec = "";
		if (sDefaultGroundingModelSpec != "") {
			sItemGroundingModelSpec = "@default=" + sDefaultGroundingModelSpec + ", ";
		}

		// now check it against each of the specs
		// and construct the spec for that item
		for(unsigned int i = 0; i < vsGMSpecs.size(); i++) {
			string sHead, sRest;
			SplitOnFirst(vsGMSpecs[i], " =.", sHead, sRest);
			if(sHead == sItemName) {
				sRest = TrimLeft(sRest);
				if(sRest.length() > 1) { 
					sItemGroundingModelSpec += sRest + ", ";
				}
			}
		}

		// now create the grounding model on the item
		ItemMap[svItems[ni]]->CreateGroundingModel(
			TrimRight(sItemGroundingModelSpec, ", "));
	}
}

//-----------------------------------------------------------------------------
// Methods related to the invalidated flag
//-----------------------------------------------------------------------------

// D: set the invalidated flag
void CFrameConcept::SetInvalidatedFlag(bool bAInvalidated) {
    bInvalidated = bAInvalidated;
    // go through the items and set the flag
	for(unsigned int i = 0; i < svItems.size(); i++) {
		ItemMap[svItems[i]]->SetInvalidatedFlag(bInvalidated);
    }
    // finally, break the seal
    BreakSeal();
}

// D: return the invalidated flag
bool CFrameConcept::GetInvalidatedFlag() {
    // assume it's not invalidated
    bInvalidated = false;
    // go through the items and check the flag
	for(unsigned int i = 0; i < svItems.size(); i++) {
		if(ItemMap[svItems[i]]->IsInvalidated()) {
		    bInvalidated = true;
		    break;
		}
    }
    return bInvalidated;
}

//-----------------------------------------------------------------------------
// Methods related to the restored for grounding flag
//-----------------------------------------------------------------------------

// D: set the restored for grounding flag
void CFrameConcept::SetRestoredForGroundingFlag(bool bARestoredForGrounding) {
    bRestoredForGrounding = bARestoredForGrounding;
    // go through the items and set the flag
	for(unsigned int i = 0; i < svItems.size(); i++) {
		ItemMap[svItems[i]]->SetRestoredForGroundingFlag(
		    bARestoredForGrounding);
    }
}

// D: return the restored for grounding flag
bool CFrameConcept::GetRestoredForGroundingFlag() {
    // assume it's not restored for grounding
    bRestoredForGrounding = false;
    // go through the items and check the flag
	for(unsigned int i = 0; i < svItems.size(); i++) {
		if(ItemMap[svItems[i]]->GetRestoredForGroundingFlag()) {
		    bRestoredForGrounding = true;
		    break;
		}
    }
    return bRestoredForGrounding;
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the current hypotheses set and belief 
// manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CFrameConcept::HypFactory() {
	FatalError(FormatString(
		"HypFactory call on frame concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
	return NULL;
}

// D: adds a hypothesis to the current set of hypotheses
int CFrameConcept::AddHyp(CHyp* pAHyp) {
	FatalError(FormatString(
		"AddHyp call on frame concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
	return -1;
}

// D: adds a new hypothesis to the current set of hypotheses
int CFrameConcept::AddNewHyp() {
	FatalError(FormatString(
		"AddNewHyp call on frame concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
	return -1;
}

// D: adds a null hypothesis to the current set of hypotheses
int CFrameConcept::AddNullHyp() {
	FatalError(FormatString(
		"AddNullHyp call on frame concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
	return -1;
}

// D: sets a hypothesis into a location
void CFrameConcept::SetHyp(int iIndex, CHyp* pHyp) {
	FatalError(FormatString(
		"SetHyp call on frame concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
}

// D: sets a null hypothesis into a location
void CFrameConcept::SetNullHyp(int iIndex) {
	FatalError(FormatString(
		"SetNullHyp call on frame concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
}

// D: deletes a hypothesis
void CFrameConcept::DeleteHyp(int iIndex) {
	FatalError(FormatString(
		"DeleteHyp call on frame concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
}

// D: return the hypothesis at a given index
CHyp* CFrameConcept::GetHyp(int iIndex) {
	FatalError(FormatString(
		"GetHyp call on frame concept %s failed (frame concepts do not "\
        "support hypsets).", sName.c_str()));
    return NULL;
}

// D: finds the index of a given hypothesis
int CFrameConcept::GetHypIndex(CHyp* pHypIndex) {
	FatalError(FormatString(
		"GetHypIndex call on array concept %s failed (array concepts do not "\
        "support hypsets).", sName.c_str()));
    return NULL;
}

// D: return the confidence of a certain hypothesis (specified by index)
float CFrameConcept::GetHypConfidence(int iIndex) {
	FatalError(FormatString(
		"GetHypConfidence call on frame concept %s failed (frame concepts "
        "do not support hypsets).", sName.c_str()));
    return 0;
}

// D: return the top hypothesis
CHyp* CFrameConcept::GetTopHyp() {
	FatalError(FormatString(
		"GetTopHyp call on frame concept %s failed (frame concepts "
        "do not support hypsets).", sName.c_str()));
    return NULL;
}

// D: return the top hyp index
int CFrameConcept::GetTopHypIndex() {
	FatalError(FormatString(
		"GetTopHypIndex call on frame concept %s failed (frame concepts "
        "do not support hypsets).", sName.c_str()));
    return -1;
}

// D: return the confidence score of the top hypothesis
float CFrameConcept::GetTopHypConfidence() {
	FatalError(FormatString(
		"GetTopHypConfidence call on frame concept %s failed (frame concepts "
        "do not support hypsets).", sName.c_str()));
    return 0;
}

// D: check if a hypothesis is valid (confidence score > 0; valus is not 
// NULL)
bool CFrameConcept::IsValidHyp(int iIndex) {
	FatalError(FormatString(
		"IsValidHyp call on frame concept %s failed (frame concepts "
        "do not support hypsets).", sName.c_str()));
    return false;
}

// D: return the total number of hypotheses for a concept (including 
// NULLs)
int CFrameConcept::GetNumHyps() {
	FatalError(FormatString(
		"GetNumHyps call on frame concept %s failed (frame concepts "
        "do not support hypsets).", sName.c_str()));
    return -1;
}

// D: return the number of valid hypotheses for a concept
int CFrameConcept::GetNumValidHyps() {
	FatalError(FormatString(
		"GetNumValidHyps call on frame concept %s failed (frame concepts "
        "do not support hypsets).", sName.c_str()));
    return -1;
}

// D: clear the current set of hypotheses for the concept
void CFrameConcept::ClearCurrentHypSet() {
	// if the concept is empty, simply return 
	if(!IsUpdated()) return;
    // clear the current hypsets for all the subsumed concepts
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->ClearCurrentHypSet();
    // notify the change
    NotifyChange();
}

// D: copies the current set of hypotheses from another concept
void CFrameConcept::CopyCurrentHypSetFrom(CConcept& rAConcept) {
    // first clear it
    ClearCurrentHypSet();

    // check that rAConcept is the right type
    if(rAConcept.GetConceptType() != ctFrame) {
        FatalError(FormatString(
            "CopyCurrentHypSetFrom called with different type (%s) on "\
            "CFrameConcept %s.", 
            ConceptTypeAsString[rAConcept.GetConceptType()].c_str(),
            sName.c_str()));
    }

    // copy the hypsets for the items
    CFrameConcept& rAFrameConcept = (CFrameConcept &)rAConcept;
    for(unsigned int i = 0; i < svItems.size(); i++) {
        if(rAFrameConcept.ItemMap.find(svItems[i]) == 
            rAFrameConcept.ItemMap.end()) {
            FatalError(FormatString(
                "Item %s not found in concept %s during CopyCurrentHypSetFrom.",
                svItems[i].c_str(), rAFrameConcept.GetName().c_str()));
        } else {
            ItemMap[svItems[i]]->CopyCurrentHypSetFrom(
                *rAFrameConcept.ItemMap[svItems[i]]);
        }
    }

    // notify the change
    NotifyChange();
}

// D: clear the explicitly confirmed hyp string
void CFrameConcept::ClearExplicitlyConfirmedHyp() {
    // clear the explicitly confirmed hyp for all subconcepts
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->ClearExplicitlyConfirmedHyp();
}

// D: clear the explicitly disconfirmed hyp string
void CFrameConcept::ClearExplicitlyDisconfirmedHyp() {
    // clear the explicitly confirmed hyp for all subconcepts
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->ClearExplicitlyDisconfirmedHyp();
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to turn last updated information
//-----------------------------------------------------------------------------

// D: Access to turn in which the concept was last updated information
int CFrameConcept::GetTurnLastUpdated() {
	// go through each item and check when it was last updated
	int iTurnLastUpdated = -1;
	for(unsigned int i = 0; i < svItems.size(); i++) {
		if (iTurnLastUpdated < ItemMap[svItems[i]]->GetTurnLastUpdated())
			iTurnLastUpdated = ItemMap[svItems[i]]->GetTurnLastUpdated();
	}

	// finally, return it
    return iTurnLastUpdated;
}

//-----------------------------------------------------------------------------
// Overwritten nethods for concept history manipulation
//-----------------------------------------------------------------------------

// D: reopens the concept (i.e. moves current value into history, and starts
//    with a clean new value
void CFrameConcept::ReOpen() {

	// first check if it's a history concept
	if(bHistoryConcept)
        FatalError(FormatString("Cannot perform ReOpen on concept (%s) history.",
            sName.c_str()));

    // now, reopen all the items
    for(unsigned int i = 0; i < svItems.size(); i++) {
		ItemMap[svItems[i]]->ReOpen();
        // and redirect the owner concept accordingly
        ItemMap[svItems[i]]->operator[](-1).SetOwnerConcept(pPrevConcept);
    }

    // clear the current value (this will notify the change)
    ClearCurrentHypSet();

	// create a clone of the current value of the concept (without the history)
	CFrameConcept* pConcept = (CFrameConcept *)Clone(false);
		
	// now destroy its structure, and simply make it its subconcepts point to
	// the main structure subconcepts
	pConcept->DestroyStructure();
	for(unsigned int i = 0; i < svItems.size(); i++) {
	    pConcept->svItems.push_back(svItems[i]);
	    pConcept->ItemMap.insert(TItemMap::value_type(
	        svItems[i], &(ItemMap[svItems[i]]->operator[] (-1))));
	}
	
	// set the flag that it's a history concept
	pConcept->SetHistoryConcept();

    // link it to the current history
	pConcept->pPrevConcept = pPrevConcept;

	// and push it into the history
    pPrevConcept = pConcept;

    // don't log the update since frames don't have grounding models
}

// D: clears the history of the current concept
void CFrameConcept::ClearHistory() {
    // check if it's a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform ClearHistory on concept (%s) history.",
            sName.c_str()));

	// o/w go through each item and clear its history
	for(unsigned int i = 0; i < svItems.size(); i++)
		ItemMap[svItems[i]]->ClearHistory();

    // and delete all its history
    if(pPrevConcept != NULL) {
        delete pPrevConcept;
        pPrevConcept = NULL;
    }
}

// D: merges the history on the concept, and returns a new concept containing 
//    that 
CConcept* CFrameConcept::CreateMergedHistoryConcept() {

    // create a clone of the current value (w/o the history)
	CFrameConcept *pMergedHistory = (CFrameConcept *)Clone(false);

    // merge all the subitems
	for(unsigned int i = 0; i < pMergedHistory->svItems.size(); i++)
        pMergedHistory->ItemMap[svItems[i]]->MergeHistory();

	// set it to not a history concept
    pMergedHistory->SetHistoryConcept(false);

    // and return it
    return pMergedHistory;
}

// D: merges the history of the concept into the current value
void CFrameConcept::MergeHistory() {

    // merge all the subitems
    for(unsigned int i = 0; i < svItems.size(); i++) 
        ItemMap[svItems[i]]->MergeHistory();

    // set it to not a history concept
    SetHistoryConcept(false);

	// then clear the history
    ClearHistory();

    // don't log the update since frames don't have a grounding model
}


#pragma warning (default:4100)