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
// StructConcept.CPP - implementation of the CStructConcept class
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
//   [2004-05-15] (dbohus):  started working on this, starting from the
//                            StructConcept class
// 
//-----------------------------------------------------------------------------

#include "StructConcept.h"
#include "DMCore/Log.h"
#include "DMCore/DMCore.h"

#pragma warning (disable:4100)

//-----------------------------------------------------------------------------
// CStructHyp class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CStructHyp: Constructors and destructors
//-----------------------------------------------------------------------------

// D: default constructor
CStructHyp::CStructHyp(TItemMap* pAItemMap, TStringVector* psvAItems, 
					   int iAHypIndex, bool bComplete) {
	ctHypType = ctStruct;
	pItemMap = pAItemMap;
	psvItems = psvAItems;
	iHypIndex = iAHypIndex;
	
    // now get the confidence from the confidence of the items in the map
    fConfidence = 0.0;
    for(unsigned int i = 0; i < psvItems->size(); i++) {
		CHyp* pItemHyp;
		if (bComplete) {
			pItemHyp = pItemMap->operator [](psvItems->operator [](i))->GetHyp(iAHypIndex);
		} else {
			pItemHyp = pItemMap->operator [](psvItems->operator [](i))->GetPartialHyp(iAHypIndex);
		}
        if(pItemHyp != NULL) {
            if(fConfidence == 0.0)
                fConfidence = pItemHyp->GetConfidence();
            else if(fConfidence != pItemHyp->GetConfidence()) {
                FatalError(FormatString(
                    "Inconsistent confidence scores on structure items "
                    "%s (%.2f) and %s (%.2f) on index %d.",
                    pItemMap->begin()->first.c_str(),
                    fConfidence, 
					psvItems->operator [](i).c_str(),
                    pItemHyp->GetConfidence(),
                    iAHypIndex));
            }
        }
    }
}

// D: copy constructor
CStructHyp::CStructHyp(CStructHyp& rAStructHyp) {
	ctHypType = ctStruct;
	pItemMap = rAStructHyp.pItemMap;
	psvItems = rAStructHyp.psvItems;
	iHypIndex = rAStructHyp.iHypIndex;
	fConfidence = rAStructHyp.fConfidence;
}

//-----------------------------------------------------------------------------
// CStructHyp specific methods
//-----------------------------------------------------------------------------

// D: set the HypIndex
void CStructHyp::SetHypIndex(int iAHypIndex) {
    iHypIndex = iAHypIndex;
}

//-----------------------------------------------------------------------------
// CStructHyp: Overwritten, CStructHyp specific methods
//-----------------------------------------------------------------------------

// D: assignment operator from another hyp
CHyp& CStructHyp::operator = (CHyp& rAHyp) {
    // check against self-assignment
    if(&rAHyp != this) {
	    // check the type
	    if(rAHyp.GetHypType() != ctStruct) {
		    // if it's not an atomic structure, signal an error
		    FatalError("Assignment operator from a different hyp type called on "\
		  	    "structure hyp. Cannot perform conversion.");
		    return *this;
	    }

		// convert it to an atomic structure valconf
	    CStructHyp& rAStructHyp = (CStructHyp&)rAHyp;
	    // now iterate through the map
		for(unsigned int i = 0; i < psvItems->size(); i++) {
            // check that the other one has it
            TItemMap::iterator iPtr2 = 
				rAStructHyp.pItemMap->find(psvItems->operator [](i));
            if(iPtr2 == rAStructHyp.pItemMap->end()) {
    		    // if it's not an atomic structure, signal an error
	    	    FatalError("Assignment operator from a different hyp type called on "\
		      	    "structure hyp. Cannot perform conversion.");
		        return *this;                
            }
			// then copy that hypothesis into the right location
			pItemMap->operator [](psvItems->operator [](i))->SetHyp(iHypIndex, 
                iPtr2->second->GetHyp(rAStructHyp.iHypIndex));
		}

		// finally, set the confidence
	    fConfidence = rAStructHyp.fConfidence;
    }
	return *this;
}

// D: set the confidence score
void CStructHyp::SetConfidence(float fAConfidence) {
    // call the inherited method first
    CHyp::SetConfidence(fAConfidence);
    // then iterate through the map and set the confidence for each item
	for(unsigned int i = 0; i < psvItems->size(); i++) {
		// get this item hyp
		CHyp* pItemHyp = 
			pItemMap->operator [](psvItems->operator [](i))->GetHyp(iHypIndex);
        if(pItemHyp != NULL) {
            pItemHyp->SetConfidence(fAConfidence);
        }
    }
}

// D: equality operator
bool CStructHyp::operator == (CHyp& rAHyp) {
    
	// check the type
	if(rAHyp.GetHypType() != ctStruct) {
		// if it's not an atomic struct, signal an error
		FatalError("Equality operator with a different hyp type called on "\
		  	  "atomic struct hyp. Cannot perform conversion.");
		return false;
	}

	// convert it to a structure hyp
	CStructHyp& rAStructHyp = (CStructHyp&)rAHyp;

	// now iterate through the map
	for(unsigned int i = 0; i < psvItems->size(); i++) {
		// get this item hyp
        CHyp* pItemHyp = 
			pItemMap->operator [](psvItems->operator [](i))->GetHyp(iHypIndex);
        // check that the other one has it
        TItemMap::iterator iPtr2 = 
			rAStructHyp.pItemMap->find(psvItems->operator [](i));
        if(iPtr2 == rAStructHyp.pItemMap->end()) {
    		// if it's not an atomic structure, signal an error
	    	FatalError("Comparison operator from a different hyp type called on "\
		      	"structure hyp. Cannot perform conversion.");
		    return false;                
        }
        // then check it
        if(pItemHyp == NULL) {
            if(iPtr2->second->GetHyp(rAStructHyp.iHypIndex) != NULL) 
                return false;
        } else {
            if(iPtr2->second->GetHyp(rAStructHyp.iHypIndex) == NULL) 
                return false;
            if(!(*pItemHyp == *(iPtr2->second->GetHyp(rAStructHyp.iHypIndex))))
                return false;
        }
	}

	// if we got here, it's all equal, so return true
	return true;
}

// D: comparison operator
bool CStructHyp::operator < (CHyp& rAHyp) {   
    // signal an error, cannot compare bools
	FatalError("Comparison operator < called on CStructHyp.");
	return false;
}

// D: comparison operator
bool CStructHyp::operator > (CHyp& rAHyp) {   
    // signal an error, cannot compare bools
	FatalError("Comparison operator > called on CStructHyp.");
	return false;
}

// D: comparison operator
bool CStructHyp::operator <= (CHyp& rAHyp) {   
    // signal an error, cannot compare bools
	FatalError("Comparison operator <= called on CStructHyp.");
	return false;
}

// D: comparison operator
bool CStructHyp::operator >= (CHyp& rAHyp) {   
    // signal an error, cannot compare bools
	FatalError("Comparison operator >= called on CStructHyp.");
	return false;
}

// D: indexing operator
CHyp* CStructHyp::operator [](string sItem) {
	return pItemMap->operator [](sItem)->GetHyp(iHypIndex);
}

// D: Convert value to string
string CStructHyp::ValueToString() {
	// assemble the result
	string sResult = "{\n";

	// now iterate through the map
	for(unsigned int i = 0; i < psvItems->size(); i++) {
		CHyp* pItemHyp = 
			pItemMap->operator [](psvItems->operator [](i))->GetHyp(iHypIndex);
		// and add the string representation
        if(pItemHyp)
		    sResult += FormatString("%s\t%s\n", 
				psvItems->operator [](i).c_str(), pItemHyp->ValueToString().c_str());
	}

	// close it
	sResult += "}\n";

	// finally, return it
	return sResult;
}

// D: Convert valconf to string
string CStructHyp::ToString() {
	// assemble the result
	string sResult = "{\n";

	// now iterate through the map
	for(unsigned int i = 0; i < psvItems->size(); i++) {
		CHyp* pItemHyp = 
			pItemMap->operator [](psvItems->operator [](i))->GetHyp(iHypIndex);
		// and add the string representation
        if(pItemHyp)
    		sResult += FormatString("%s\t%s\n", 
				psvItems->operator [](i).c_str(), pItemHyp->ToString().c_str());
	}

	// close it
	sResult += "}\n";

	// finally, return it
	return sResult;
}

// D: Get the hyp from a string
void CStructHyp::FromString(string sString) {
	FatalError("FromString called on CStructHyp. Call failed.");
}

//-----------------------------------------------------------------------------
// CStructConcept class - this is the structured concept class. It is 
//   similar to the CStructConcept class, but acts as a single unit and can 
//   undergo grounding
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CStructConcept: Constructors and destructors
//-----------------------------------------------------------------------------
// D: constructor
CStructConcept::CStructConcept(string sAName, TConceptSource csAConceptSource): 
	CConcept(sAName, csAConceptSource, DEFAULT_STRUCT_CARDINALITY) {
	// call create structure to initialize the ItemMap accordingly
	ctConceptType = ctStruct;
	CreateStructure();
}
	
// D: destructor
CStructConcept::~CStructConcept() {
    // call destroy structure to destroy the current instantiation
	DestroyStructure();
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
// CStructConcept class concept manipulation
//-----------------------------------------------------------------------------

// D: create the items of a structure - does nothing, should be overwritten
void CStructConcept::CreateStructure() {
}

// D: destroy the structure
void CStructConcept::DestroyStructure() {
	// go through each item, and delete it
	TItemMap::iterator iPtr;
	for(iPtr = ItemMap.begin(); iPtr != ItemMap.end(); iPtr++) {
		delete iPtr->second;
	}
	// finally, clear the map
	ItemMap.clear();
	// and the items
	svItems.clear();
}

//-----------------------------------------------------------------------------
// CStructConcept: Access to various class members
//-----------------------------------------------------------------------------

// D: Comparison operator
bool CStructConcept::operator < (CConcept& rAConcept) {
    Error("Comparison operator < called on structure concept.");
    return false;
}

// D: Comparison operator
bool CStructConcept::operator > (CConcept& rAConcept) {
    Error("Comparison operator > called on structure concept.");
    return false;
}

// D: Comparison operator
bool CStructConcept::operator <= (CConcept& rAConcept) {
    Error("Comparison operator <= called on structure concept.");
    return false;
}

// D: Comparison operator
bool CStructConcept::operator >= (CConcept& rAConcept) {
    Error("Comparison operator >= called on structure concept.");
    return false;
}

// D: operator for accessing structure members
CConcept& CStructConcept::operator [](string sAItemName) {

	string sItemName;	// the item in this structure we are trying to access
	string sFollowUp;	// something else after that item, in case we have 
						// nested structures i.e. hotel.chain.name

	// if empty itemname, then return this concept
	if(sAItemName.empty()) 
		return *this;

	// separate the item name into the first part and the follow-up part
	SplitOnFirst(sAItemName, ".", sItemName, sFollowUp);

    // convert it to an int
    int iFirstIndex = atoi(sItemName.c_str());

    // check if index is negative and there is an active history, then 
    // return the structured concept in history
    if((iFirstIndex < 0) && (pPrevConcept != NULL)) {
        // if adressing previous in history, return that
        if(iFirstIndex == -1) {
            if(sFollowUp.empty())
                return *pPrevConcept;
            else
                return pPrevConcept->operator [](sFollowUp);
        } else {
            // o/w go recursively
            return pPrevConcept->operator [](
                FormatString("%d.%s", iFirstIndex + 1, sFollowUp.c_str()));
        }
    } else {
        // o/w deal with it as a member access in the current value of the 
        // structure

        // identify the item corresponding to sItemName
	    TItemMap::iterator iPtr;
	    iPtr = ItemMap.find(sItemName);
	    if(iPtr == ItemMap.end()) {
		    FatalError(FormatString(
                "Accessing invalid item in structured concept: %s.%s",
                sName.c_str(), sAItemName.c_str()).c_str());
		    return NULLConcept;	
	    }
	    CConcept* pConcept = iPtr->second;
    	
	    // return the appropriate concept
	    if(sFollowUp.empty()) 
		    return *pConcept;
	    else 
		    return pConcept->operator [](sFollowUp);
    }
}

// D: Clones the struct concept
CConcept* CStructConcept::Clone(bool bCloneHistory) {

	// start with an empty clone
	CStructConcept* pConcept = (CStructConcept *)EmptyClone();

	// now destroy the structure and clone all the items
	pConcept->DestroyStructure();
	for(unsigned int i = 0; i < svItems.size(); i++) {
		// clone a subitem
		CConcept* pConceptToInsert = ItemMap[svItems[i]]->Clone();
		// reassign the owner concept
		pConceptToInsert->SetOwnerConcept(pConcept);
		// insert it
        pConcept->ItemMap.insert(TItemMap::value_type(svItems[i], pConceptToInsert));
		// and add stuff to svItems
		pConcept->svItems.push_back(svItems[i]);
	}

	// now set the other members
    pConcept->SetConceptType(ctConceptType);
    pConcept->SetConceptSource(csConceptSource);
    pConcept->sName = sName;
    pConcept->pOwnerDialogAgent = pOwnerDialogAgent;
	pConcept->SetOwnerConcept(pOwnerConcept);
	// a clone has no grounding model
	pConcept->pGroundingModel = NULL;
	pConcept->bSealed = bSealed;
	// a clone does not notify changes
	pConcept->DisableChangeNotification();
	// reconstruct the current hyp set
    for(unsigned int i = 0; i < (int)vhCurrentHypSet.size(); i++)
		if(vhCurrentHypSet[i] == NULL)
			pConcept->vhCurrentHypSet.push_back(NULL);
		else
			pConcept->vhCurrentHypSet.push_back(
				new CStructHyp(&(pConcept->ItemMap), &(pConcept->svItems), i));
	pConcept->iNumValidHyps = iNumValidHyps;
	pConcept->SetGroundedFlag(bGrounded);
	pConcept->iCardinality = iCardinality;
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
// Overwritten methods implementing various types of updates in the 
// naive probabilistic update scheme
//-----------------------------------------------------------------------------

// D: naive probabilistic update - assign from string method
#pragma warning (disable:4189)
void CStructConcept::Update_NPU_AssignFromString(void* pUpdateData) {
    // first, check that it's not a history concept
    if(bHistoryConcept) 
        FatalError(FormatString("Cannot perform (AssingFromString) update on concept "\
            "(%s) history.", sName.c_str()));

	// o/w 
    string sString = *(string *)pUpdateData;
	string sWorkingString = TrimLeft(sString);

    // clear the current hyp set
    ClearCurrentHypSet();

	// the first character has to be "{"
	if(sWorkingString[0] != '{') {
		// fail the whole process
		FatalError(FormatString("Cannot perform conversion to "\
            "<CStructConcept> %s from string (dump below).\n%s", 
            sName.c_str(), sString.c_str()));
	}

	// advance over the "{"
	sWorkingString = TrimLeft(sWorkingString.substr(1, 
        sWorkingString.length()-1));

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
                "Cannot perform update to <CStructConcept> "\
				"%s from string (dump below).\n%s", sName.c_str(),
				sString.c_str()));
		}
	}

	// now go through all the lines and convert the items 
	string sLine;

	// while keeping track of the ones that were updated and how many hypotheses they
	// had
	set<string> ssUpdated;
	int iUpdatedNumHyps = -1;

	while(((sLine = Trim(ExtractFirstLine(sWorkingString))) != "}") && 
		(sLine != "") && (sLine[0] != '}')) {

		// split at the equals sign
		string sItem, sValue;
		if(SplitOnFirst(sLine, CONCEPT_EQUALS_SIGN, sItem, sValue)) {
			// if we successfully made the split
			// check if sValue starts with {. 
			if((sValue[0] == '{') || (sValue[0] == ':')) {
				// in this case, we are dealing with a nested structure 
				// (or an array), so identify where it ends, and correct 
                // the sValue;

				// basically go forward counting the matching {}s and
				// terminate when the matching one is found
				sWorkingString = TrimLeft(sValue + sWorkingString);
				unsigned int iCount = FindClosingQuoteChar(sWorkingString, 
					sWorkingString.find('{') + 1, '{', '}');

				// if we ran out of the string, signal an error
				if(iCount >= sWorkingString.length()) {
					FatalError(FormatString(
                        "Cannot perform conversion to <CStructConcept> "\
                        "%s from string (dump below).\n%s",
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
					" while converting from string. ",
					sItem.c_str(), sName.c_str()));
			} else {
				// and if found, set its value accordingly
                if(sConfidence != "") 
					sValue += VAL_CONF_SEPARATOR + sConfidence;
				iPtr->second->Update(CU_ASSIGN_FROM_STRING, &sValue);
				// and mark it as updated
				ssUpdated.insert(sItem);
				// set the number of hypotheses
                if(iUpdatedNumHyps == -1) 
					iUpdatedNumHyps = iPtr->second->GetNumHyps();
                else if(iPtr->second->GetNumHyps() != iUpdatedNumHyps) {
                    FatalError(FormatString(
                        "Variable number of hypotheses in item %s while"\
                        " converting concept %s from string.", 
                        sItem.c_str(), sName.c_str()));
                }
			}
		} else {
			// if no equals sign (split unsuccessful), fail the whole process
			FatalError(FormatString(
                "Cannot perform conversion to <CStructConcept> "\
				"%s from string (dump below).\n%s",
				sName.c_str(), sString.c_str()));
		} 
	}

	// at this point, we should get out of the loop 
	if(sLine[0] != '}') {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CStructConcept> from %s.", 
            sString.c_str()));
	}

	// finally, go through all the items again and update all the ones that were 
	//  not updated
	for(unsigned int i = 0; i < svItems.size(); i++) {
		if(ssUpdated.find(svItems[i]) == ssUpdated.end()) {
			// then update this concept with empty hypotheses
			for(int h = 0; h < iUpdatedNumHyps; h++) {
				// this will notify the update
				ItemMap[svItems[i]]->AddNullHyp();
			}
		}
	}

	// finally, update the valconf set
    // first clear it (but without deleting the hypotheses from the 
    //  concepts
    CConcept::ClearCurrentHypSet();
    for(int h = 0; h < iUpdatedNumHyps; h++) {
		// this will notify the change
        AddHyp(new CStructHyp(&ItemMap, &svItems, h));
    }			
    iNumValidHyps = iUpdatedNumHyps;
}
#pragma warning (default:4189)

// A: update partial hypotheses from a string
void CStructConcept::Update_PartialFromString(void* pUpdateData) {
	updateFromString(pUpdateData, CU_PARTIAL_FROM_STRING);
}

// updates the concept from a string representation
#pragma warning (disable:4189)
void CStructConcept::updateFromString(void* pUpdateData, string sUpdateType) {
    // first, check that it's not a history concept
    if(bHistoryConcept) 
        FatalError(FormatString("Cannot perform (AssingFromString) update on concept "\
            "(%s) history.", sName.c_str()));


	// o/w 
    string sString = *(string *)pUpdateData;
	string sWorkingString = TrimLeft(sString);

	Log( CONCEPT_STREAM, "updateFromString: %s", sString.c_str());

    // clear the current hyp set
    ClearCurrentHypSet();

	// the first character has to be "{"
	if(sWorkingString[0] != '{') {
		// fail the whole process
		FatalError(FormatString("Cannot perform conversion to "\
            "<CStructConcept> %s from string (dump below).\n%s", 
            sName.c_str(), sString.c_str()));
	}

	// advance over the "{"
	sWorkingString = TrimLeft(sWorkingString.substr(1, 
        sWorkingString.length()-1));

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
                "Cannot perform update to <CStructConcept> "\
				"%s from string (dump below).\n%s", sName.c_str(),
				sString.c_str()));
		}
	}

	// now go through all the lines and convert the items 
	string sLine;

	// while keeping track of the ones that were updated and how many hypotheses they
	// had
	set<string> ssUpdated;
	int iUpdatedNumHyps = -1;

	while(((sLine = Trim(ExtractFirstLine(sWorkingString))) != "}") && 
		(sLine != "") && (sLine[0] != '}')) {

		// split at the equals sign
		string sItem, sValue;
		if(SplitOnFirst(sLine, CONCEPT_EQUALS_SIGN, sItem, sValue)) {
			// if we successfully made the split
			// check if sValue starts with {. 
			if((sValue[0] == '{') || (sValue[0] == ':')) {
				// in this case, we are dealing with a nested structure 
				// (or an array), so identify where it ends, and correct 
                // the sValue;

				// basically go forward counting the matching {}s and
				// terminate when the matching one is found
				sWorkingString = TrimLeft(sValue + sWorkingString);
				unsigned int iCount = FindClosingQuoteChar(sWorkingString, 
					sWorkingString.find('{') + 1, '{', '}');

				// if we ran out of the string, signal an error
				if(iCount >= sWorkingString.length()) {
					FatalError(FormatString(
                        "Cannot perform conversion to <CStructConcept> "\
                        "%s from string (dump below).\n%s",
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
					" while converting from string. ",
					sItem.c_str(), sName.c_str()));
			} else {
				// and if found, set its value accordingly
                if(sConfidence != "") 
					sValue += VAL_CONF_SEPARATOR + sConfidence;
				iPtr->second->Update(sUpdateType, &sValue);
				// and mark it as updated
				ssUpdated.insert(sItem);
				if (sUpdateType == CU_PARTIAL_FROM_STRING) {
					// set the number of hypotheses
					if(iUpdatedNumHyps == -1) 
						iUpdatedNumHyps = iPtr->second->GetNumPartialHyps();
					else if(iPtr->second->GetNumPartialHyps() != iUpdatedNumHyps) {
						FatalError(FormatString(
							"Variable number of partial hypotheses in item %s while"\
							" converting concept %s from string.", 
							sItem.c_str(), sName.c_str()));
					}
				} else {
					// set the number of hypotheses
					if(iUpdatedNumHyps == -1) 
						iUpdatedNumHyps = iPtr->second->GetNumHyps();
					else if(iPtr->second->GetNumHyps() != iUpdatedNumHyps) {
						FatalError(FormatString(
							"Variable number of hypotheses in item %s while"\
							" converting concept %s from string.", 
							sItem.c_str(), sName.c_str()));
					}
				}
			}
		} else {
			// if no equals sign (split unsuccessful), fail the whole process
			FatalError(FormatString(
                "Cannot perform conversion to <CStructConcept> "\
				"%s from string (dump below).\n%s",
				sName.c_str(), sString.c_str()));
		} 
	}

	// at this point, we should get out of the loop 
	if(sLine[0] != '}') {
		// fail the whole process
		FatalError(FormatString(
            "Cannot perform conversion to <CStructConcept> from %s.", 
            sString.c_str()));
	}

	// finally, go through all the items again and update all the ones that were 
	//  not updated
	for(unsigned int i = 0; i < svItems.size(); i++) {
		if(ssUpdated.find(svItems[i]) == ssUpdated.end()) {
			// then update this concept with empty hypotheses
			for(int h = 0; h < iUpdatedNumHyps; h++) {
				if (sUpdateType == CU_PARTIAL_FROM_STRING) {
					ItemMap[svItems[i]]->AddNullPartialHyp();
				} else {
					// this will notify the update
					ItemMap[svItems[i]]->AddNullHyp();
				}
			}
		}
	}

	// finally, update the valconf set
    // first clear it (but without deleting the hypotheses from the 
    //  concepts
	if (sUpdateType == CU_PARTIAL_FROM_STRING) {
		CConcept::ClearPartialHypSet();
		for(int h = 0; h < iUpdatedNumHyps; h++) {
			// this will notify the change
			AddPartialHyp(new CStructHyp(&ItemMap, &svItems, h, false));
		}
	    iNumValidPartialHyps = iUpdatedNumHyps;
	} else {
		CConcept::ClearCurrentHypSet();
		for(int h = 0; h < iUpdatedNumHyps; h++) {
			// this will notify the change
			AddHyp(new CStructHyp(&ItemMap, &svItems, h));
		}
	    iNumValidHyps = iUpdatedNumHyps;
    }			
}
#pragma warning (default:4189)



//-----------------------------------------------------------------------------
// Overwritten methods implementing string conversion
//-----------------------------------------------------------------------------

// D: conversion to value/conf;value/conf... format
string CStructConcept::HypSetToString() {
	// go through the hypset, and convert each one to string
	string sResult;
	if(IsUpdated()) {
	    sResult = "{\n";
	    // go through the items and add them to the string
		for(unsigned int i = 0; i < svItems.size(); i++) {		
		    if(ItemMap[svItems[i]]->IsUpdated()||
			   ItemMap[svItems[i]]->HasPartialHyp()) {
			    sResult += FormatString("%s\t%s", 
                    svItems[i].c_str(), 
					ItemMap[svItems[i]]->HypSetToString().c_str());
		    }
        }
	    // and finally add the closing braces and return 
	    sResult += "}\n";
	    return sResult;
	} else {
		return IsInvalidated()?INVALIDATED_CONCEPT:UNDEFINED_CONCEPT;
	}
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to concept name
//-----------------------------------------------------------------------------

// D: set the concept name
void CStructConcept::SetName(string sAName) {
    // call the inherited
    CConcept::SetName(sAName);
    // sets the name recursively on each of the items
	for(unsigned int i = 0; i < svItems.size(); i++)
		ItemMap[svItems[i]]->SetName(sName + "." + svItems[i]);
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to the owner dialog agent
//-----------------------------------------------------------------------------
    
// set the owner dialog agent
void CStructConcept::SetOwnerDialogAgent(CDialogAgent* pADialogAgent) {
	// call the inherited
	CConcept::SetOwnerDialogAgent(pADialogAgent);
	// then set the owner an all subitems
	for(unsigned int i = 0; i < svItems.size(); i++) 
		ItemMap[svItems[i]]->SetOwnerDialogAgent(pADialogAgent);
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the grounding model
//-----------------------------------------------------------------------------

// D: create a grounding model for this concept
void CStructConcept::CreateGroundingModel(string sGroundingModelSpec) {

	string sDefaultGroundingModelSpec = "";
	string sThisSpec = "";

	// partition the spec by commas
	TStringVector vsGMSpecs = PartitionString(sGroundingModelSpec, ",");
	// and trim it
	for(unsigned int i = 0; i < vsGMSpecs.size(); i++) {
		vsGMSpecs[i] = TrimLeft(vsGMSpecs[i]);
		// check if it's for the current item
		if((sThisSpec == "") && (vsGMSpecs[i].find('=') == -1)) {
			sThisSpec = vsGMSpecs[i];
		}
		else {
			string sHead, sTail;
			SplitOnFirst(vsGMSpecs[i], "=", sHead, sTail);
			// A: found a default grounding model spec
			if (sHead == "@default") {
				sDefaultGroundingModelSpec = sTail;
			}
			else {
			    FatalError(FormatString(
					"Could not create a grounding model for struct"
					" %s. Ill formed grounding model specification: %s.",
					GetAgentQualifiedName().c_str(), vsGMSpecs[i].c_str()));
			}
		}
	}
	
	// A: no specification given, fall back on the default one
	if (sThisSpec == "") {
		sThisSpec = sDefaultGroundingModelSpec;
	}

	// create the grounding model for this struct
    if(!(pGroundingManager->GetConfiguration().bGroundConcepts) ||
		(sThisSpec == "none") || (sThisSpec == "")) {
        pGroundingModel = NULL;
    } else {
        pGroundingModel = new CGMConcept(sThisSpec);
        pGroundingModel->Initialize();
        pGroundingModel->SetConcept(this);
    }

	// now go through each of the elements of the structure
	for(unsigned int in = 0; in < svItems.size(); in++) {
		// get the item name
		string sItemName = svItems[in];
		// now check it against each of the specs
		// and construct the spec for that item
		string sItemGroundingModelSpec = "";
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
		ItemMap[svItems[in]]->CreateGroundingModel(TrimRight(sItemGroundingModelSpec, ", "));
	}
}

// D: Declare the grounding models subsumed by this concept
void CStructConcept::DeclareGroundingModels(
    TGroundingModelPointersVector& rgmpvModels,
    TGroundingModelPointersSet& rgmpsExclude) {

    // first call the inherited method
    CConcept::DeclareGroundingModels(rgmpvModels, rgmpsExclude);

    // then add the grounding models for the subsumed concepts
	for(unsigned int i = 0; i < svItems.size(); i++)
		ItemMap[svItems[i]]->DeclareGroundingModels(rgmpvModels, rgmpsExclude);
}

// D: Declare the subsumed concepts
void CStructConcept::DeclareConcepts(
    TConceptPointersVector& rcpvConcepts, 
	TConceptPointersSet& rcpsExclude) {

    // just add the current concept
	CConcept::DeclareConcepts(rcpvConcepts, rcpsExclude);

    // then go through all the items and have them declare the concepts, too
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->DeclareConcepts(rcpvConcepts, rcpsExclude);
}

//-----------------------------------------------------------------------------
// Methods related to signaling concept changes
//-----------------------------------------------------------------------------

// D: Set the concept change notifications flag
void CStructConcept::SetChangeNotification(
	bool bAChangeNotification) {
	bChangeNotification = bAChangeNotification;
	// and set it for items
	for(unsigned int i = 0; i < svItems.size(); i++)
		ItemMap[svItems[i]]->SetChangeNotification(bAChangeNotification);
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the current hypotheses set and belief 
// manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CStructConcept::HypFactory() {
	FatalError(FormatString(
		"HypFactory call on structured concept %s concept failed. "\
        "Structured hypotheses should not be generated through the HypFactory "\
        "method.",
		sName.c_str()));
	return NULL;
}

// D: adds a new hypothesis to the current set of hypotheses
int CStructConcept::AddNewHyp() {
    // add a hypothesis in all the members
	for(unsigned int i = 0; i < svItems.size(); i++) 
		ItemMap[svItems[i]]->AddNewHyp();
    // then add the corresponding entry in the hypset (this notifies the 
	// change)
    return AddHyp(new CStructHyp(&ItemMap, &svItems, vhCurrentHypSet.size()));
}

// D: adds a null hypothesis to the current set of hypotheses
int CStructConcept::AddNullHyp() {
    // add a null hypothesis in all the members
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->AddNullHyp();
    // then create the corresponding entry (this will notify the change)
	return CConcept::AddNullHyp();
}

// D: sets a hypothesis into a location
void CStructConcept::SetHyp(int iIndex, CHyp* pHyp) {
    // first set it to null (this potentially notifies the change)
    SetNullHyp(iIndex);
    // check if pHyp is null, then we're done return
    if(!pHyp) return;
    // create a new hypothesis at that location
    vhCurrentHypSet[iIndex] = new CStructHyp(&ItemMap, &svItems, iIndex);
    // copy the contents (which will automatically copy into members)
    *(vhCurrentHypSet[iIndex]) = *pHyp;
    iNumValidHyps++;
    // notify the change
    NotifyChange();
}

// D: deletes a hypothesis 
void CStructConcept::DeleteHyp(int iIndex) {

    // delete the hypothesis in all the members
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->DeleteHyp(iIndex);

	if(vhCurrentHypSet[iIndex] != NULL) {
		// if it's not null, destroy it
		delete vhCurrentHypSet[iIndex];
		iNumValidHyps--;
	}

	// then delete it from the array
	vhCurrentHypSet.erase(vhCurrentHypSet.begin() + iIndex);

	// and reset the iHypIndex for all the hypotheses following
    for(int i = iIndex; i < (int)vhCurrentHypSet.size(); i++) 
        if(vhCurrentHypSet[i] != NULL)
            ((CStructHyp *)vhCurrentHypSet[i])->SetHypIndex(i);

    // notify the change
    NotifyChange();
}

// D: sets a null hypothesis into a location
void CStructConcept::SetNullHyp(int iIndex) {
    // if it's already null, return
    if(vhCurrentHypSet[iIndex] == NULL) return;
    // o/w delete it
    delete vhCurrentHypSet[iIndex];
    // and set it to null
    vhCurrentHypSet[iIndex] = NULL;
    // and call the same on all member items
    // add a null hypothesis in all the members
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->SetNullHyp(iIndex);
    // finally decrease the number of valid hypotheses and return
    iNumValidHyps--;
    // notify the change
    NotifyChange();
}

// D: clear the current set of hypotheses for the concept
void CStructConcept::ClearCurrentHypSet() {
    // call the inherited method (this will notify the change)
    CConcept::ClearCurrentHypSet();
    // clear the current hypsets for all the subsumed concepts
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->ClearCurrentHypSet();
}

//-----------------------------------------------------------------------------
// Overwritten methods for partial hypotheses manipulation
//-----------------------------------------------------------------------------

// A: adds a new partial hypothesis to the current set of partial hypotheses
int CStructConcept::AddNewPartialHyp() {
    // add a hypothesis in all the members
	for(unsigned int i = 0; i < svItems.size(); i++) 
		ItemMap[svItems[i]]->AddNewPartialHyp();

	Log(CONCEPT_STREAM, "index of new hyp: %d", vhPartialHypSet.size());

    // then add the corresponding entry in the hypset
    return AddPartialHyp(new CStructHyp(&ItemMap, &svItems, vhPartialHypSet.size(), false));
}

// A: adds a null hypothesis to the current set of partial hypotheses
int CStructConcept::AddNullPartialHyp() {
    // add a null partial hypothesis in all the members
    for(unsigned int i = 0; i < svItems.size(); i++)
        ItemMap[svItems[i]]->AddNullPartialHyp();
    // then create the corresponding entry
	return CConcept::AddNullPartialHyp();
}

//-----------------------------------------------------------------------------
// Overwritten methods for concept history manipulation
//-----------------------------------------------------------------------------

// D: set the history concept flag
void CStructConcept::SetHistoryConcept(bool bAHistoryConcept) {
	bHistoryConcept = bAHistoryConcept;
	// and set it for all the subconcepts
    for(unsigned int i = 0; i < svItems.size(); i++)
		ItemMap[svItems[i]]->SetHistoryConcept(bAHistoryConcept);
}

#pragma warning (default:4100)