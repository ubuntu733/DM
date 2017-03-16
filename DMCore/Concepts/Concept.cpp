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
// CONCEPT.CPP - implementation of the CConcept base class
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
//	 [2005-11-07] (antoine): added support for partial concept update
//   [2004-06-02] (dbohus):  added definition of pOwnerConcept, concepts now
//                            check with parent if unclear if they are
//                            grounded
//   [2004-05-30] (dbohus):  changed definition of IsGrounded
//   [2004-05-19] (dbohus):  major makeover on concepts, introduced frame 
//                            concepts and structure concepts which support 
//                            grounding
//   [2004-05-15] (dbohus):  cleaned up concepts code, added Update as well as
//                            various types of update methods
//   [2004-04-20] (jsherwan):added InsertAt()
//   [2004-04-25] (dbohus):  added WaitingConveyance flag and functionality
//   [2004-04-18] (dbohus):  renamed MergedHistory to 
//                            CreateMergedHistoryConcept and added a 
//                            MergeHistory function which merges the history
//                            up into the current value for a concept
//   [2004-04-17] (dbohus):  added support for declaring the grounding models
//                            subsumed by the concept
//   [2004-04-16] (dbohus):  removed grounding models from constructors and 
//                            added them as a separate method
//   [2003-04-28] (antoine): fixed == operator for ValConfSet so that two empty
//                            concepts are now equal
//   [2003-04-14] (dbohus):  added support for OwnerDialogAgent
//   [2003-04-10] (dbohus):  added Restore on concepts
//   [2003-04-03] (dbohus):  added support for concept grounding models
//   [2003-04-02] (dbohus):  introduced support for grounding models
//   [2003-04-01] (dbohus):  added support for iTurnLastUpdated
//   [2003-03-19] (dbohus):  eliminated ValueToString, transformed 
//                            ValueToStringWithConf into ValueSetToString
//   [2003-03-13] (antoine): added CConcept::ModeValueToString to return a 
//                            string representing the value of the mode of the
//                            value/confidence distribution of the concept
//   [2003-01-29] (dbohus):  added equality and comparison operators
//   [2002-12-09] (dbohus):  major revamp on concepts: added support for 
//                            reopening concepts and maintaing a history of
//                            previous values 
//   [2002-12-01] (dbohus):  added CValConfSet::GetModeValConf to obtain the 
//                            mode of the distribution
//   [2002-12-01] (dbohus):  added SetValueFromString and UpdateValueFromString
//                            functions on concepts
//   [2002-11-26] (dbohus):  added CValConfSet and changed concepts to work 
//                            with it
//   [2002-11-26] (dbohus):  fixed bug in assign operator when assigning to 
//                            self
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//	 [2002-04-10] (agh)   :  added Conveyance type and CConcept conveyance 
// 							  member
//   [2002-01-08] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#include "Concept.h"
#include "DMCore/Log.h"
#include "DMCore/DMCore.h"
#include "DMCore/Agents/DialogAgents/DialogAgent.h"

#pragma warning (disable:4100)

// NULL concept: this object is used designate invalid concept references
CConcept NULLConcept("NULL");

//-----------------------------------------------------------------------------
// CHyp class - this is the base class for the hierarchy of hypothesis
//              classes. It essentially implements a type and an associated 
//              value + confidence score
//-----------------------------------------------------------------------------

// D: Constructor
CHyp::CHyp() {
	ctHypType = ctUnknown;
	fConfidence = 0.0;
}

// D: Constructor from reference
CHyp::CHyp(CHyp& rAHyp) {
	ctHypType = rAHyp.ctHypType;
	fConfidence = rAHyp.fConfidence;
}

// D: Destructor - does nothing
CHyp::~CHyp() {
}

//-----------------------------------------------------------------------------
// Acess to member variables
//-----------------------------------------------------------------------------

// D: Access method to concept (hypothesis) type
TConceptType CHyp::GetHypType() {
	return ctHypType;
}

// D: Access to confidence value
float CHyp::GetConfidence() {
    return fConfidence;
}

//-----------------------------------------------------------------------------
// Virtual concept type specific functions (to be overwritten by derived 
// classes)
//-----------------------------------------------------------------------------

// D: assignment operator
CHyp& CHyp::operator = (CHyp& rAHyp) {
    // check self assignment
	if(&rAHyp != this) {
    	ctHypType = rAHyp.ctHypType;
	    fConfidence = rAHyp.fConfidence;
    }
	return *this;
}

// D: Set the confidence score
void CHyp::SetConfidence(float fAConfidence) {
    fConfidence = fAConfidence;
}

// D: equality operator - return an error, it should never get called
bool CHyp::operator == (CHyp& rAHyp) {
	FatalError("Equality operator called on abstract CHyp.");
    return false;
}

// D: comparison operator - return an error, it should never get called
bool CHyp::operator < (CHyp& rAHyp) {
	FatalError("Comparison operator < called on abstract CHyp.");
    return false;
}

// D: comparison operator - return an error, it should never get called
bool CHyp::operator > (CHyp& rAHyp) {
	FatalError("Comparison operator > called on abstract CHyp.");
    return false;
}

// D: comparison operator - return an error, it should never get called
bool CHyp::operator <= (CHyp& rAHyp) {
	FatalError("Comparison operator <= called on abstract CHyp.");
    return false;
}

// D: comparison operator - return an error, it should never get called
bool CHyp::operator >= (CHyp& rAHyp) {
	FatalError("Comparison operator >= called on abstract CHyp.");
    return false;
}

// D: Indexing operator - return an error, should never be called
CHyp* CHyp::operator [] (string sItem) {
	FatalError("Indexing operator [] called on abstract CHyp.");
    return NULL;
}

// D: Convert value to string
string CHyp::ValueToString() {
	FatalError("ValueToString called on abstract CHyp.");
	return ABSTRACT_CONCEPT;
}

// D: Convert hypothesis to string
string CHyp::ToString() {
	FatalError("ToString called on abstract CHyp.");
	return ABSTRACT_CONCEPT;
}

// D: Get the hypothesis from a string
void CHyp::FromString(string sString) {
	FatalError("FromString called on abstract CHyp. Call failed.");
}


//-----------------------------------------------------------------------------
//
// CConcept class - this is the base of the hierarchy of concept classes. It 
//                  implements the basic properties and functionality of 
//                  a concept
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CConcept: Constructors and destructors
//-----------------------------------------------------------------------------

// D: default constructor
CConcept::CConcept(string sAName, TConceptSource csAConceptSource, 
                   int iACardinality) {
	ctConceptType = ctUnknown;
	csConceptSource = csAConceptSource;
	sName = sAName;
    pOwnerDialogAgent = NULL;
    pOwnerConcept = NULL;
    pGroundingModel = NULL;
	bGrounded = false;
	bInvalidated = false;
	bRestoredForGrounding = false;
	bSealed = false;
	bChangeNotification = true;
	iNumValidHyps = 0;
    iCardinality = iACardinality;
    iTurnLastUpdated = -1;
	cConveyance = cNotConveyed;
    bWaitingConveyance = false;
    pPrevConcept = NULL;
    bHistoryConcept = false;
    sExplicitlyConfirmedHyp = "";
    sExplicitlyDisconfirmedHyp = "";
}

// D: destructor - clear the history and the associated grounding model 
//    on destruction
CConcept::~CConcept() {    
    // delete the history
	if(pPrevConcept != NULL)
        delete pPrevConcept;
	pPrevConcept = NULL;
    // delete the grounding model
	if(pGroundingModel != NULL) {
		// remove any issued grounding requests
		pGroundingManager->RemoveConceptGroundingRequest(this);
		// and delete the grounding model
        delete pGroundingModel;
	}
	pGroundingModel = NULL;
    // remove the waiting for the conveyance notification
    if(bWaitingConveyance) ClearWaitingConveyance();
    // clear the concept notification pointer
    ClearConceptNotificationPointer();
}

//-----------------------------------------------------------------------------
// CConcept: Methods for overall concept manipulation
//-----------------------------------------------------------------------------

// D: assignment operator
CConcept& CConcept::operator = (CConcept& rAConcept) {
    // check assignment to self
    if(&rAConcept != this) {
		// call on the update from concept
		Update(CU_ASSIGN_FROM_CONCEPT, &rAConcept);
	}
    return *this;
}

// D: assignment operator from integer
CConcept& CConcept::operator = (int iAValue) {
	FatalError(FormatString("Assignment operator from integer to %s (%s type) "\
					   "concept failed.", sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
	return NULLConcept;
}

// D: assignment operator from boolean
CConcept& CConcept::operator = (bool bAValue) {
	FatalError(FormatString("Assignment operator from bool to %s (%s type) concept "\
					   "failed.", sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
	return NULLConcept;
}

// D: assignment operator from float
CConcept& CConcept::operator = (float fAValue) {
	FatalError(FormatString("Assignment operator from float to %s (%s type) concept "\
					   "failed.", sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
	return NULLConcept;
}

// D: assignment operator from double
CConcept& CConcept::operator = (double dAValue) {
	FatalError(FormatString("Assignment operator from float to %s (%s type) concept "\
					   "failed.", sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
	return NULLConcept;
}

// D: assignment operator from string
CConcept& CConcept::operator = (string sAValue) {
    // call upon the AssignFromString update
	Update(CU_ASSIGN_FROM_STRING, &sAValue);
    return *this;
}

// D: assignment operator char *
CConcept& CConcept::operator = (const char * lpszAValue) {
	return operator=((string)lpszAValue);
}

// D: equality operator
bool CConcept::operator == (CConcept& rAConcept) {

    // compare the top hypotheses for the 2 concepts
    CHyp* pTopHyp = GetTopHyp();
    CHyp* pATopHyp = rAConcept.GetTopHyp();

	// if the two top-hyps are NULL (the two concepts didn't receive
	// any value), the sets are equal
	if ((pTopHyp == NULL) && (pATopHyp == NULL)) {
		return true;
	}

	// if only one of them is NULL they are not equal
	else if ((pTopHyp == NULL) || (pATopHyp == NULL)) {
		return false;
	}

	// last, check using the equality operator on the mode
    return *pTopHyp == *pATopHyp;
}

// D: inequality operator
bool CConcept::operator != (CConcept& rAConcept) {
    return !operator==(rAConcept);
}

// D: comparison operator
bool CConcept::operator < (CConcept& rAConcept) {
    // compare the 2 top hypotheses
    CHyp* pTopHyp = GetTopHyp();
    if(pTopHyp == NULL) return false;
    CHyp* pATopHyp = rAConcept.GetTopHyp();
    if(pATopHyp == NULL) return false;
    return *pTopHyp < *pATopHyp;
}

// D: comparison operator
bool CConcept::operator > (CConcept& rAConcept) {
    // compare the 2 top hypotheses
    CHyp* pTopHyp = GetTopHyp();
    if(pTopHyp == NULL) return false;
    CHyp* pATopHyp = rAConcept.GetTopHyp();
    if(pATopHyp == NULL) return false;
    return *pTopHyp > *pATopHyp;
}

// D: comparison operator
bool CConcept::operator <= (CConcept& rAConcept) {
    // compare the 2 top hypotheses
    CHyp* pTopHyp = GetTopHyp();
    if(pTopHyp == NULL) return false;
    CHyp* pATopHyp = rAConcept.GetTopHyp();
    if(pATopHyp == NULL) return false;
    return *pTopHyp <= *pATopHyp;
}

// D: comparison operator
bool CConcept::operator >= (CConcept& rAConcept) {
    // compare the 2 top hypotheses
    CHyp* pTopHyp = GetTopHyp();
    if(pTopHyp == NULL) return false;
    CHyp* pATopHyp = rAConcept.GetTopHyp();
    if(pATopHyp == NULL) return false;
    return *pTopHyp >= *pATopHyp;
}

// D: indexing operator with integer argument
CConcept& CConcept::operator [](int iIndex) {
    // if history-addressing self
    if(iIndex == 0) 
        return *this;
    // check if index is negative, then return the concept in history
    if((iIndex < 0) && (pPrevConcept != NULL)) {
        // if adressing previous in history, return that
        if(iIndex == -1) {
            return *pPrevConcept;
        } else {
            // o/w go recursively
            return pPrevConcept->operator [](iIndex + 1);
        }
    } else {
	    FatalError(FormatString("Indexing operator [%d] on %s (atomic %s) failed.", 
            iIndex, sName.c_str(), ConceptTypeAsString[ctConceptType].c_str()));
	    return NULLConcept;	
    }
}

// D: indexing operator with string argument
CConcept& CConcept::operator [](string sIndex) {
    // if index empty, return self
    if(sIndex.empty()) {
		return *this;
	} else {
        // get the first part of the index , if index is i.i.i
        string sFirstIndex, sFollowUp;
        SplitOnFirst(sIndex, ".", sFirstIndex, sFollowUp);
        // try to convert to an integer
        int iFirstIndex;
        if(sscanf(sFirstIndex.c_str(), "%d", &iFirstIndex) != 1) {
		    FatalError(FormatString("Indexing operator [%s] on %s (atomic %s) failed.", 
			    sIndex.c_str(), sName.c_str(), ConceptTypeAsString[ctConceptType].c_str()));
            return NULLConcept;
        } else {
            // o/w chain the operators
            CConcept *pConcept = &(operator[](iFirstIndex));
            if(sFollowUp.empty()) 
                return *pConcept;
            else 
                return pConcept->operator[](sFollowUp);
        }
	}
}

// D: indexing operator with char * argument
CConcept& CConcept::operator [](const char * lpszIndex) {
	return operator[]((string)lpszIndex);
}

// D: integer cast operator
CConcept::operator int() {
	FatalError(FormatString("Integer cast operator on %s (%s type) concept failed.",
					   sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
	return 0;	
}

// D: float cast operator
CConcept::operator float() {
	FatalError(FormatString("Float cast operator on %s (%s type) concept failed.",
					   sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
	return 0.0;	
}

// D: boolean cast operator
CConcept::operator bool() {	
	FatalError(FormatString("Bool cast operator on %s (%s type) concept failed.",
					   sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
	return false;	
}
	
// D: string cast operator
CConcept::operator string() {
	return Trim(GroundedHypToString());
}

// D: clears the contents of the concept
void CConcept::Clear() {
    // check if it's a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform Clear on concept (%s) history.",
            sName.c_str()));

    // record the initial value of the concept (if the concept has a grounding
    //  model)
    string sInitialValue;
    if(pGroundingModel) 
        sInitialValue = TrimRight(HypSetToString());

	// o/w delete all it's history
    if(pPrevConcept != NULL) {
        delete pPrevConcept;
        pPrevConcept = NULL;
    }

    // and clear the current value (notifies the change)
    ClearCurrentHypSet();

    // now log the update (if the concept has a grounding model
    if(pGroundingModel) 
        Log(CONCEPT_STREAM, FormatString(
            "Concept update [clear] on %s:\nInitial value dumped below:\n%s\n"
            "Updated value dumped below:\n%s", 
            GetAgentQualifiedName().c_str(),
            sInitialValue.c_str(),
            TrimRight(HypSetToString()).c_str()));
            
    // clear the explicitly confirmed hyp
    sExplicitlyConfirmedHyp = "";
    // and the explicitly disconfirmed hyp
    sExplicitlyDisconfirmedHyp = "";
}

// D: clears the current value of the concept
void CConcept::ClearCurrentValue() {
    // check if it's a history concept
    if(bHistoryConcept)
        FatalError(FormatString(
            "Cannot perform ClearCurrentValue on concept (%s) history.",
            sName.c_str()));

    // record the initial value of the concept (if the concept has a grounding
    //  model)
    string sInitialValue;
    if(pGroundingModel) 
        sInitialValue = TrimRight(HypSetToString());

	// o/w clear the current value (this notifies the change)
    ClearCurrentHypSet();

    // now log the update (if the concept has a grounding model
    if(pGroundingModel) 
        Log(CONCEPT_STREAM, FormatString(
            "Concept update [clear_current_value] on %s:\nInitial value dumped below:\n%s\n"
            "Updated value dumped below:\n%s", 
            GetAgentQualifiedName().c_str(),
            sInitialValue.c_str(),
            TrimRight(HypSetToString()).c_str()));
}

// D: clones the concept - essentially produces an exact replica of the concept, 
//     only that it does not notify changes; nor it waits for conveyance; nor it
//     has a grounding model since this is a newly created copy; 
CConcept* CConcept::Clone(bool bCloneHistory) {
    // start with an empty clone
	CConcept* pConcept = EmptyClone();

    // and fill in all the members
    pConcept->SetConceptType(ctConceptType);
    pConcept->SetConceptSource(csConceptSource);
    pConcept->sName = sName;
    pConcept->pOwnerDialogAgent = pOwnerDialogAgent;
    pConcept->SetOwnerConcept(pOwnerConcept);
	// a clone does not have a grounding model
	pConcept->pGroundingModel = NULL;
	pConcept->bSealed = bSealed;
	// a clone does not notify
	pConcept->DisableChangeNotification();
	// now copy the values
    pConcept->CopyCurrentHypSetFrom(*this);
	// and set the grounded state
	pConcept->SetGroundedFlag(bGrounded);
	// set the invalidated flag
	pConcept->SetInvalidatedFlag(bInvalidated);
	// set the restored for grounding
	if(bCloneHistory) 
	    pConcept->SetRestoredForGroundingFlag(bRestoredForGrounding);
	else
	    pConcept->SetRestoredForGroundingFlag(false);
	pConcept->iCardinality = iCardinality;
    pConcept->SetTurnLastUpdated(iTurnLastUpdated);
    pConcept->cConveyance = cConveyance;
	// a clone does not wait for conveyance
    pConcept->bWaitingConveyance = false;
    pConcept->SetHistoryConcept(bHistoryConcept);
	// now clone the history if required
	if(bCloneHistory && (pPrevConcept != NULL)) 
		pConcept->pPrevConcept = pPrevConcept->Clone();
	else
		pConcept->pPrevConcept = NULL;		
    // set the explicitly confirmed and disconfirmed hyps
    pConcept->sExplicitlyConfirmedHyp = sExplicitlyConfirmedHyp;    
    pConcept->sExplicitlyDisconfirmedHyp = sExplicitlyDisconfirmedHyp;    

	// finally, return the clone
    return pConcept;
}

// D: returns an empty clone of the concept (basically just preserving the
//    type, but not the contents
CConcept* CConcept::EmptyClone() {
    return new CConcept;
}

//-----------------------------------------------------------------------------
// Virtual methods implementing various types of updates
//-----------------------------------------------------------------------------

// D: update the concept
void CConcept::Update(string sUpdateType, void* pUpdateData) {

    // record the initial value of the concept (if the concept has a grounding
    //  model
    string sInitialValue;
	string sInitialPartialValue;
    if(pGroundingModel) 
        sInitialValue = TrimRight(HypSetToString());

	// call the appropriate function based on the belief updating model and 
	// on the update type
	string sBeliefUpdatingModelName = "npu";
    //根据android版本修改
		//pGroundingManager->GetBeliefUpdatingModelName();
	if(sBeliefUpdatingModelName == "npu") {
    //cout << "测试1"<<endl;
		if(sUpdateType == CU_ASSIGN_FROM_STRING){
      //cout << "测试"<<endl;
			Update_NPU_AssignFromString(pUpdateData);
    }
		else if(sUpdateType == CU_ASSIGN_FROM_CONCEPT)
    {// cout << "测试2"<<endl;
			Update_NPU_AssignFromConcept(pUpdateData);
    }
		else if(sUpdateType == CU_UPDATE_WITH_CONCEPT) {
      //cout << "进入期待分支1"<<endl;
			Update_NPU_UpdateWithConcept(pUpdateData);
    }
		else if(sUpdateType == CU_COLLAPSE_TO_MODE) 
    { //cout << "测试3"<<endl;
			Update_NPU_CollapseToMode(pUpdateData);
    }
		else if(sUpdateType == CU_PARTIAL_FROM_STRING)
    { //cout << "测试4"<<endl;
			Update_PartialFromString(pUpdateData);
    }
		else FatalError(FormatString(
			"Unknown update type (%s) in updating concept %s.",
			sUpdateType.c_str(), GetAgentQualifiedName().c_str()));

		// if we got a final update, erase the previous partial one
		if (sUpdateType != CU_PARTIAL_FROM_STRING) {
			ClearPartialHypSet();
		}

	} else if(sBeliefUpdatingModelName == "calista") {
		if(sUpdateType == CU_ASSIGN_FROM_STRING)
			Update_Calista_AssignFromString(pUpdateData);
		else if(sUpdateType == CU_ASSIGN_FROM_CONCEPT)
			Update_Calista_AssignFromConcept(pUpdateData);
		else if(sUpdateType == CU_UPDATE_WITH_CONCEPT) 
			Update_Calista_UpdateWithConcept(pUpdateData);
		else if(sUpdateType == CU_COLLAPSE_TO_MODE) 
			Update_Calista_CollapseToMode(pUpdateData);
		else FatalError(FormatString(
			"Unknown update type (%s) in updating concept %s.",
			sUpdateType.c_str(), GetAgentQualifiedName().c_str()));
	}

    // now log the update (if the concept has a grounding model
    if(pGroundingModel) 
        Log(CONCEPT_STREAM, FormatString(
			"Concept update [%s] on %s:\nInitial value dumped below:\n%s\n"
            "Updated value dumped below:\n%s", 
            sUpdateType.c_str(), GetAgentQualifiedName().c_str(),
            sInitialValue.c_str(), 
            TrimRight(HypSetToString()).c_str()));
}

// ----------------------------------------------------------------------------
// D: Update function for the Naive Probabilistic update model
// ----------------------------------------------------------------------------

// D: Naive probabilistic update scheme - assign from string
void CConcept::Update_NPU_AssignFromString(void* pUpdateData) {
    // first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (AssignFromString) update on "
			"concept (%s) history.", sName.c_str()));

	// o/w 
    string sValConf;			            // one hyp pair
	string sRest = *(string *)pUpdateData;	// the rest of the string

	// first clear the current hyp-set of the concept
    ClearCurrentHypSet();

	// then go through each hyp pair and create it from the string
	while(sRest.length() > 0) {
		// grab the first hyp pair
		SplitOnFirst(sRest, HYPS_SEPARATOR, sValConf, sRest);

		// create the new value-confidence (this will notify the change)
		int iIndex = AddNewHyp();

        // acquire it from string
		vhCurrentHypSet[iIndex]->FromString(sValConf);
	}
}

// D: Naive probabilistic update scheme - assign from another concept
void CConcept::Update_NPU_AssignFromConcept(void* pUpdateData) {
    // first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform AssignFromConcept update "
			"on concept (%s) history.", sName.c_str()));

    // copy the set of hypotheses (this will notify the change)
    CopyCurrentHypSetFrom(*((CConcept *)pUpdateData));
}

// D: Naive probabilistic update scheme - perform update with another concept
void CConcept::Update_NPU_UpdateWithConcept(void* pUpdateData) {
    // first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (UpdateWithConcept) update "
			"on concept (%s) history.", sName.c_str()));

	// get a pointer at the concept
	CConcept *pConcept = (CConcept *)pUpdateData;

	// get a pointer to the top-level hypothesis
	CHyp* pOldTopHyp = GetTopHyp();
	string sOldTopHyp = "";
	if(pOldTopHyp) sOldTopHyp = pOldTopHyp->ValueToString();

	// figure out if the concept is undergoing an explicit or an implicit 
	// confirmation
	TSystemActionOnConcept saocAction = 
		pDMCore->GetSystemActionOnConcept(this);
	bool bIsUndergoingExplicitConfirm = 
		(saocAction.sSystemAction == SA_EXPL_CONF);
	bool bIsUndergoingImplicitConfirm = 
		(saocAction.sSystemAction == SA_IMPL_CONF);	

	// if the incoming concept is not empty, then perform a naive probabilistic 
	// update on the concept
	if(pConcept && pConcept->IsUpdated()) {
    //cout << "进入期待分支2" <<endl;
		// set the cardinality to the largest cardinality
		if(iCardinality < pConcept->iCardinality)
			iCardinality = pConcept->iCardinality;

		// hold the confidences in 2 arrays vfConf1, vfConf2
		vector<float, allocator<float> > vfConf1, vfConf2;

		// copy the confidences for the first set and sum them up
		float fConf1Sum = 0;
		for(int i = 0; i < (int)vhCurrentHypSet.size(); i++) {
			if(vhCurrentHypSet[i] != NULL) {
				vfConf1.push_back(vhCurrentHypSet[i]->GetConfidence());
				fConf1Sum += vfConf1[i];
			} else {
				vfConf1.push_back(0);
			}        

		Log(CONCEPT_STREAM, "vfConf1[%d]=%f", i, vfConf1[i]);
		}

		// sum up the confidences for the second set
		float fConf2Sum = 0;
		for(int i = 0; i < (int)pConcept->vhCurrentHypSet.size(); i++)
			if(pConcept->vhCurrentHypSet[i] != NULL) 
				fConf2Sum += pConcept->vhCurrentHypSet[i]->GetConfidence();

		// compute the confidences for the "unknown" values in sets 1 and 2
		float fUnkConf1;
		if(iCardinality == (int)vhCurrentHypSet.size()) 
			fUnkConf1 = 0;
		else 
			fUnkConf1 = (1 - fConf1Sum) / (iCardinality - vhCurrentHypSet.size());

		float fUnkConf2;
		if(iCardinality == (int)pConcept->vhCurrentHypSet.size()) 
			fUnkConf2 = 0;
		else 
			fUnkConf2 = (1 - fConf2Sum) / (iCardinality - pConcept->vhCurrentHypSet.size());

		// update the vfConf2 vector
		for(int i = 0; i < (int)vhCurrentHypSet.size(); i++) 
			vfConf2.push_back(fUnkConf2);

		// now go through the second vector, and add the values one by one, checking
		// if they are already in the set
		unsigned int iOrigSetSize = vhCurrentHypSet.size();
		for(int i = 0; i < (int)pConcept->vhCurrentHypSet.size(); i++) {
      //cout << "进入期待分支3"<<endl;
			bool bFound = false;
			unsigned int j = 0;
			if(pConcept->vhCurrentHypSet[i] != NULL) {
				for(j = 0; j < iOrigSetSize; j++) {
					if(*(pConcept->vhCurrentHypSet[i]) == *(vhCurrentHypSet[j])) {
						bFound = true;
						break;
					}
				}
			}
      //cout << "bFound的值为:" << bFound<<endl;
			// if found in the set
			if(bFound) {
				// just set the appropriate confidence value
				vfConf2[j] = pConcept->vhCurrentHypSet[i]->GetConfidence();
			} else {
        //cout << "进入期待分支4"<<endl;
				// o/w add it to the set
				if(pConcept->vhCurrentHypSet[i] != NULL) {
          //cout << "进入期待分支5"<<endl;
					// add a new hypothesis (this will notify the change)
					int iIndex = AddNewHyp();
					*(vhCurrentHypSet[iIndex]) = *(pConcept->vhCurrentHypSet[i]);
					// and set the confidences right
					vfConf1.push_back(fUnkConf1);
					vfConf2.push_back(pConcept->vhCurrentHypSet[i]->GetConfidence());
				} else {
					AddNullHyp();
					// and set the confidences right
					vfConf1.push_back(fUnkConf1);
					vfConf2.push_back(0);
				}
			}
		}
		
		// finally, multiply the scores in 
		for(int i = 0; i < (int)vhCurrentHypSet.size(); i++) {
			if(vhCurrentHypSet[i] != NULL) 
				// this will also notify the change
				SetHypConfidence(i, vfConf1[i] * vfConf2[i]);
		}
		
		// compute the normalizing factor
		float fNormalizer = 0;
		for(int i = 0; i < (int)vfConf1.size(); i++)
			fNormalizer += vfConf1[i] * vfConf2[i];
		fNormalizer += (iCardinality-vfConf1.size())*fUnkConf1*fUnkConf2;

		// and update the confidences
		for(int i = 0; i < (int)vhCurrentHypSet.size(); i++)
			if(vhCurrentHypSet[i] != NULL)
				// this will also notify the change
				SetHypConfidence(i, (vfConf1[i] * vfConf2[i]) / fNormalizer);

		// now, make sure that at least FREE_PROB_MASS is allocated to the rest
		fNormalizer = 0;
		for(int i = 0; i < (int)vhCurrentHypSet.size(); i++)
			if(vhCurrentHypSet[i] != NULL)
				fNormalizer += vhCurrentHypSet[i]->GetConfidence();
		if(fNormalizer > 1 - FREE_PROB_MASS) {
			// if we're over the limit
			for(int i = 0; i < (int)vhCurrentHypSet.size(); i++)
				if(vhCurrentHypSet[i] != NULL)
					// this will also notify the change
					SetHypConfidence(i, 
						vhCurrentHypSet[i]->GetConfidence() * 
						(1 - FREE_PROB_MASS) / fNormalizer);
		}
	}

	// now, if the concept is undergoing an explicit confirmation, do the 
	// update for explicit confirmations
	if(bIsUndergoingExplicitConfirm) {
		// first check that the top hypothesis has stayed the same
		CHyp* pNewTopHyp = GetTopHyp();
		string sNewTopHyp = "";
		if(pNewTopHyp) sNewTopHyp = pNewTopHyp->ValueToString();
		if((sOldTopHyp == sNewTopHyp) && (sOldTopHyp != "")) {
			// now do the confirmation

			// check if the confirm was bound with a YES or a NO
			string sAgencyName = FormatString("/_ExplicitConfirm[%s]", 
				GetAgentQualifiedName().c_str());
			CDialogAgent *pdaExplConfirmAgency = 
				((CDialogAgent *)AgentsRegistry[sAgencyName]);
			string sRequestAgentName = FormatString("/_ExplicitConfirm[%s]/RequestConfirm", 
				GetAgentQualifiedName().c_str());
			CDialogAgent *pdaRequestConfirmAgent = 
				((CDialogAgent *)AgentsRegistry[sRequestAgentName]);
			CConcept& rConfirmConcept = 
				pdaExplConfirmAgency->C("confirm");
			bool bYes = rConfirmConcept.IsAvailableAndGrounded() &&
				(bool)(rConfirmConcept);
			bool bNo = rConfirmConcept.IsAvailableAndGrounded() &&
				!(bool)(rConfirmConcept);
			bool bTooManyNonUnderstandings = false;
			if (pdaExplConfirmAgency->HasParameter("max_attempts")) {
				bTooManyNonUnderstandings = 
					pdaRequestConfirmAgent->GetTurnsInFocusCounter() >=
					pdaRequestConfirmAgent->GetMaxExecuteCounter()-1;
			}

			// now do the update depending on the confirm
			if(bYes) {
			    // get the top hypothesis number
				int iIndex = GetTopHypIndex();
				// now do the update (this will notify the change)
				SetHypConfidence(iIndex, 1 - FREE_PROB_MASS);
				// do the notify change again, just in case the hyp confidence
				// was 1-FREE_PROB_MASS to begin with
				NotifyChange();
				// set the explicitly confirmed hyp
				SetExplicitlyConfirmedHyp(GetHyp(iIndex));
				// and delete all the other ones before it
				for(int h = 0; h < iIndex; h++) 
					DeleteHyp(0);
				// and after it
				while(GetNumHyps() > 1) DeleteHyp(1);
				// finally, set the concept to grounded
				SetGroundedFlag(true);				
			} else if(bNo || bTooManyNonUnderstandings) {
			    // get the top hypothesis number
				int iIndex = GetTopHypIndex();
				// delete the top hypothesis (this will notify the change)
				DeleteHyp(iIndex);
				// set the explicitly disconfirmed hyp (only if the
				// user actually said "NO", not on non-understandings)
				if (bNo)
					SetExplicitlyDisconfirmedHyp(sOldTopHyp);
				// if we're not left with any valid hyps, set the
				// invalidated flag
				SetInvalidatedFlag(!GetNumValidHyps());
			}
		}
	}

	// finally, if the concept is undergoing an implicit confirmation, do the 
	// update for implicit confirmations; apply the heuristic only if the concept
	// is still sealed, and only if the concept has not been reopened in the 
	// meantime
	if(bIsUndergoingImplicitConfirm && IsSealed() && IsUpdated()) {
	
		// check if the confirm was bound with a YES or a NO
		string sAgencyName = FormatString("/_ImplicitConfirmExpect[%s]", 
			GetAgentQualifiedName().c_str());
		CConcept& rConfirmConcept = 
			((CDialogAgent *)AgentsRegistry[sAgencyName])->C("confirm");
		bool bNo = rConfirmConcept.IsAvailableAndGrounded() &&
			!(bool)(rConfirmConcept);

		int iIndex = GetTopHypIndex();

		// if we heard a No, delete that hypothesis
		if(bNo) {
			// get the top hypothesis number
			DeleteHyp(iIndex);
			// if we're not left with any valid hyps, set the
			// invalidated flag
			SetInvalidatedFlag(!GetNumValidHyps());
		} else {
			// o/w we need to boost the confidence of the top hypothesis
			// to 0.95

			// now do the update (this will notify the change)
			SetHypConfidence(iIndex, 1 - FREE_PROB_MASS);

			// and delete all the other ones before it
			for(int h = 0; h < iIndex; h++) 
				// this will also notify the change
				DeleteHyp(0);

			// and after it
			while(GetNumHyps() > 1) DeleteHyp(1);

			// finally, set the concept to grounded
			SetGroundedFlag(true);
		}
	}		
}

// D: Naive probabilistic update scheme - update the value of a concept by
//    collapsing it to the mode
void CConcept::Update_NPU_CollapseToMode(void* pUpdateData) {

    // first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (CollapseToMode) update "
			"on concept (%s) history.", sName.c_str()));

    // get the top hypothesis number
    int iIndex = GetTopHypIndex();

    // if there is no top hyp, return
    if(iIndex == -1) return;

	// now do the update (this will notify the change)
	SetHypConfidence(iIndex, 1 - FREE_PROB_MASS);

	// and delete all the other ones before it
	for(int h = 0; h < iIndex; h++) 
		// this will also notify the change
		DeleteHyp(0);

	// and after it
	while(GetNumHyps() > 1) DeleteHyp(1);

	// finally, set the concept to grounded
	SetGroundedFlag(true);
}

// A: assigns a partial hypothesis value to the concept
void CConcept::Update_PartialFromString(void* pUpdateData) {
    // first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (PartialFromString) update on "
			"concept (%s) history.", sName.c_str()));

	// o/w 
    string sValConf;			            // one hyp pair
	string sRest = *(string *)pUpdateData;	// the rest of the string

	// reset the partial hyp set
	ClearPartialHypSet();

	Log(CONCEPT_STREAM, "Partial update for concept %s: %s", 
		sName.c_str(), sRest.c_str());

	// then go through each hyp pair and create it from the string
	while(sRest.length() > 0) {
		// grab the first hyp pair
		SplitOnFirst(sRest, HYPS_SEPARATOR, sValConf, sRest);

		// create the new value-confidence (this will notify the change)
	    int iNewIndex = AddNewPartialHyp();

		// acquire it from string
		vhPartialHypSet[iNewIndex]->FromString(sValConf);
	}
}

// ----------------------------------------------------------------------------
// D: Update function for the Calista update model
// ----------------------------------------------------------------------------

// D: Calista belief updating scheme - assign from string
void CConcept::Update_Calista_AssignFromString(void* pUpdateData) {
	// redirect to the NPU update
	Update_NPU_AssignFromString(pUpdateData);
}

// D: Naive probabilistic update scheme - assign from another concept
void CConcept::Update_Calista_AssignFromConcept(void* pUpdateData) {
    // redirect to the NPU update
	Update_NPU_AssignFromConcept(pUpdateData);
}

// D: Calista belief updating scheme - perform update with another concept
//    this is basically the bind update
void CConcept::Update_Calista_UpdateWithConcept(void* pUpdateData) {
    
	// if we don't have a grounding model on this concept, defer to the NPU 
	// type update (Calista only updates concepts that undergo grounding)
	if(!pGroundingModel) {
		Update_NPU_UpdateWithConcept(pUpdateData);
		return;
	}

	// first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (UpdateWithConcept) update "
			"on concept (%s) history.", sName.c_str()));

	CConcept *pConcept = (CConcept *)pUpdateData;

	// identify the model that we want, corresponding to the system action
	TSystemActionOnConcept saocAction = 
		pDMCore->GetSystemActionOnConcept(this);
		
    // now get the model based on the system action
	STRING2FLOATVECTOR s2vfModel = 
		pGroundingManager->GetBeliefUpdatingModelForAction(
			saocAction.sSystemAction);

	// if we are doing a request on this concept or an other type update, 
	// and we don't have a new value, 
	if((!pConcept || !pConcept->IsUpdated()) && 
		((saocAction.sSystemAction == SA_REQUEST) || 
		 (saocAction.sSystemAction == SA_OTHER))) 
		// then simply return
		return;

    // now identify the top hypothesis from history
    int iIndexH_TH = pPrevConcept?pPrevConcept->GetTopHypIndex():-1;
    CHyp* phH_TH = NULL;
	float fConfH_TH = 0;
	if(iIndexH_TH != -1) {
		phH_TH = pPrevConcept->GetHyp(iIndexH_TH);
		fConfH_TH = phH_TH->GetConfidence();
	}        

	// now identify the top hypothesis from the initial concept	
	int iIndexI_TH = GetTopHypIndex();
	CHyp* phI_TH = NULL;
	float fConfI_TH = 0;
	if(iIndexI_TH != -1) {
		phI_TH = GetHyp(iIndexI_TH);
		fConfI_TH = phI_TH->GetConfidence();
	}
	// now store the initial hyp string
	string sInitialTopHyp = phI_TH?phI_TH->ValueToString():"";
	
	// identify the top hypothesis from the new concept
	int iIndexNEW1 = -1;
	if(pConcept)
		iIndexNEW1 = pConcept->GetTopHypIndex();
	CHyp* phNEW1 = NULL;
	float fConfNEW1 = 0;
	if(iIndexNEW1 != -1) { 
		phNEW1 = pConcept->GetHyp(iIndexNEW1);
		fConfNEW1 = phNEW1->GetConfidence();
	}

    // now check if we have an empty_with_history
    bool bEmptyWithHistory = (iIndexI_TH == -1) && (iIndexH_TH != -1);
    
    // now define HOHH as a flag (this indicates that we will need to 
    // use the history hypothesis as the initial hypothesis
    bool bICWithHOHH = bEmptyWithHistory && 
        (saocAction.sSystemAction == SA_IMPL_CONF);
    bool bUICWithH0HH = bEmptyWithHistory && 
        (saocAction.sSystemAction == SA_UNPLANNED_IMPL_CONF);
  
	// now use that belief updating model to construct the confidence
	// scores
	vector <double, allocator <double> > vfConfs;
	vfConfs.resize(3);
	vfConfs[0] = vfConfs[1] = vfConfs[2] = 0;

	// go through each feature of the model and compute the sums
	pGroundingManager->PrecomputeBeliefUpdatingFeatures(
	    this, pConcept, saocAction.sSystemAction);
	STRING2FLOATVECTOR::iterator iPtr;
	string sLogString;
	for(iPtr = s2vfModel.begin(); iPtr != s2vfModel.end(); iPtr++) {
		// get the feature value
		float fFeatureValue = 
			pGroundingManager->GetGroundingFeature(iPtr->first);
		vfConfs[1] += fFeatureValue * (iPtr->second)[0];
		vfConfs[2] += fFeatureValue * (iPtr->second)[1];
		sLogString += FormatString("  %s = %.4f\t%.4f\t%.4f\n", 
		    iPtr->first.c_str(), fFeatureValue, 
		    fFeatureValue * (iPtr->second)[0], 
		    fFeatureValue * (iPtr->second)[1]);
	}
    sLogString += FormatString("  [TOTAL] = 1.0\t%.4f\t%.4f\n", 
        vfConfs[1], vfConfs[2]);
	pGroundingManager->ClearBeliefUpdatingFeatures();

	// exponentiate and normalize
	double fNormalizer = 0;
	for(unsigned int i = 0; i < 3; i++) {
		vfConfs[i] = exp(vfConfs[i]);
		fNormalizer += vfConfs[i];
	}
	for(unsigned int i = 0; i < 3; i++) {
		vfConfs[i] = vfConfs[i] / fNormalizer;
	}

	sLogString = FormatString(
		"Calista belief update [%s] on %s:\n[H:%.4f I:%.4f N:%.4f -> 1:%.4f 2:%.4f O:%.4f]\n%s", 
		saocAction.sSystemAction.c_str(), GetName().c_str(), 
		fConfH_TH, fConfI_TH, fConfNEW1, 
		vfConfs[0], vfConfs[1], vfConfs[2], sLogString.c_str());

	// now do some post-processing
	// if we are doing an explicit confirmation or an implicit confirmation
	// then threshold the top hypothesis by FREE_PROB_MASS
	if((vfConfs[0] < FREE_PROB_MASS) &&
	    ((saocAction.sSystemAction == SA_EXPL_CONF) ||
	     (saocAction.sSystemAction == SA_IMPL_CONF) ||
	     (saocAction.sSystemAction == SA_UNPLANNED_IMPL_CONF))) {
		vfConfs[2] += vfConfs[0];
		vfConfs[0] = 0;
	}
	// if we are doing a request or an other update, threshold the second hyp by
	// the free probability mass
	if((vfConfs[1] < FREE_PROB_MASS) &&
	    ((saocAction.sSystemAction == SA_REQUEST) ||
	     (saocAction.sSystemAction == SA_OTHER))) {
		vfConfs[2] += vfConfs[1];
		vfConfs[1] = 0;
	}	
	// if we have an explicit confirmation and a no-marker, then kill the initial hyp
    int iMarkDisconfirm = atoi(
        pInteractionEventManager->GetValueForExpectation("[mark_disconfirm]").c_str());
	if((saocAction.sSystemAction == SA_EXPL_CONF) && (iMarkDisconfirm > 0)) 
	    vfConfs[0] = 0;

	// log the update
	Log(BELIEFUPDATING_STREAM, sLogString.c_str());    

    // now analyse the results. IF we are doing an implicit confirmation
    // with h0hh then we need to deal with this separately (we need some more
    // careful post-processing)
    if(bICWithHOHH || bUICWithH0HH) {

        // here I'm applying a heuristic for HOHH updates with ImplicitConfirms
        // this heuristic either leaves the concept untouched, or brings it back to 
        // life from a reopen
        
        // store if we have a new top hypothesis
        bool bNewTopHyp = false;
        
        // decide if we need to revive the concept, or simply update the history
        bool bReviveConcept = 
        // we revive the concept in the following conditions
        // 1) there is a new hypothesis which a higher confidence score
            ((iIndexNEW1 != -1) && (vfConfs[1] > vfConfs[0]) && !(*phNEW1 == *phH_TH)) || 
        // 2) the confidence score on the initial hypothesis has dropped
            (vfConfs[0] < fConfH_TH);
        
        // now, if we need to revive the concept
        if(bReviveConcept) {
            // add the first hyp
            if(vfConfs[0] > 0) {
                int iIndex = AddNewHyp();
                *(vhCurrentHypSet[iIndex]) = *phH_TH;
                // and set the confidence
                SetHypConfidence(iIndex, (float)vfConfs[0]);
            }
            // add the second hyp if one exists and is different from the first
            if((iIndexNEW1 != -1) && !(*phNEW1 == *phH_TH) && (vfConfs[1] > 0)) {                
                int iIndex = AddNewHyp();
                *(vhCurrentHypSet[iIndex]) = *phNEW1;
                // and set the confidence
                SetHypConfidence(iIndex, (float)vfConfs[1]);
                // set that we have a new top hyp
                bNewTopHyp = vfConfs[1] > vfConfs[0];
            }
            // now, if the confidence for the first hyp is zero and there is no second
            // hyp, or the second hyp has zero conf, invalidate the concept
            if((vfConfs[0] == 0) && ((iIndexNEW1 == -1) || (vfConfs[1] == 0))) {
                // clear the history
                ClearHistory();
                // set the invalidated flag
                SetInvalidatedFlag(true);
            }
            // finally, set the concept as restored for grounding
            SetRestoredForGroundingFlag();
        }
        
        // now if we have revived the concept (either desealed or invalidated it)
        if(!IsSealed() && !IsInvalidated()) {
            // then schedule it for grounding
            string sAction = pGroundingManager->ScheduleConceptGrounding(this);
            // if the action is implicit confirm or accept, then we have to 
            // kill the concept again and only update the history
            if(((sAction == "IMPL_CONF") || (sAction == "ACCEPT")) && !bNewTopHyp) {
                // signal that grounding has completed for this concept (we 
                // don't want to run IMPL_CONFS and ACCEPTS again on the same
                // top hyp)
                pGroundingManager->ConceptGroundingRequestCompleted(this);
                // then we have to move this to history
                operator[] (-1).CopyCurrentHypSetFrom(*this);
                // clear the current concept
                ClearCurrentHypSet();
                // clear the restored for grounding flag
                SetRestoredForGroundingFlag(false);        
                // and seal it back
                Seal();
            }
        }

    } else {

        // o/w deal with the default case    

	    // now add the new hypothesis 
	    bool bAddedNewHyp = false;
	    if((iIndexNEW1 != -1) &&
		    ((iIndexI_TH == -1) || !(*phNEW1 == *phI_TH))) {
		    int iIndexNEW1 = AddNewHyp();
		    *(vhCurrentHypSet[iIndexNEW1]) = *phNEW1;
		    // and set the confidences right
		    SetHypConfidence(iIndexNEW1, (float)vfConfs[1]);
		    // mark that we've added a new hyp
		    bAddedNewHyp = true;
	    }	

	    // set the confidence score for the initial hypothesis
	    bool bDeletedI_TH = false;
	    if(iIndexI_TH != -1) {
		    if(vfConfs[0] != 0) {
			    SetHypConfidence(iIndexI_TH, (float)vfConfs[0]);
			    // also notify the change since sethypconfidence does
			    // not do it unless the confidence score is different
			    NotifyChange();
		    } else {
		        // o/w delete the initial hyp
			    DeleteHyp(iIndexI_TH);
			    iIndexI_TH = -1;
			    phI_TH = NULL;
			    // remember that we deleted the initial
			    bDeletedI_TH = true;
		    }
	    }

	    // finally clear the hypothesis set of all the rest
	    for(unsigned int i = 0; i < vhCurrentHypSet.size(); i++) {
		    // delete everything that's not the initial or final
		    if(((phI_TH == NULL) || !(*(vhCurrentHypSet[i]) == *phI_TH)) &&
			    ((phNEW1 == NULL) || !(*(vhCurrentHypSet[i]) == *phNEW1))) {
			    DeleteHyp(i);
		    }
		    // delete all the hypotheses that have confidence zero
		    if(vhCurrentHypSet[i]->GetConfidence() == 0)
			    DeleteHyp(i);
	    }
	    
	    // finally, if we deleted the initial hyp and did not add a 
	    // new one, mark the concept as invalidated
        SetInvalidatedFlag(bDeletedI_TH && !bAddedNewHyp);	    
    }
    
    // now if we have an explicit confirmation with a Yes in it, then 
    // set the explicitly confirmed hyp
    if(saocAction.sSystemAction == SA_EXPL_CONF) {
        // get the mark_confirm indicator
        int iMarkConfirm = atoi(
            pInteractionEventManager->GetLastInput()->GetValueForExpectation("[mark_confirm]").c_str());
        if(iMarkConfirm > 0)
            // set the explicitly confirmed hyp
            SetExplicitlyConfirmedHyp(GetTopHyp());    
        // get the mark disconfirm indicator
        int iMarkDisconfirm = atoi(
            pInteractionEventManager->GetLastInput()->GetValueForExpectation("[mark_disconfirm]").c_str());
        if(iMarkDisconfirm > 0)
            // set the explicitly confirmed hyp
            SetExplicitlyDisconfirmedHyp(sInitialTopHyp);        
    }

}

// D: Calista belief updating scheme - update the value of a concept by
//    collapsing it to the mode
void CConcept::Update_Calista_CollapseToMode(void* pUpdateData) {

    // first, check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform (CollapseToMode) update "
			"on concept (%s) history.", sName.c_str()));

    // get the top hypothesis number
    int iIndex = GetTopHypIndex();

    // if there is no top hyp, return
    if(iIndex == -1) return;

	// and delete all the other ones before it
	for(int h = 0; h < iIndex; h++) 
		// this will also notify the change
		DeleteHyp(0);

	// and after it
	while(GetNumHyps() > 1) DeleteHyp(1);

	// finally, set the concept to grounded
	SetGroundedFlag(true);
}

//-----------------------------------------------------------------------------
// Virtual methods implementing various flags (state information) on the 
// concept
//-----------------------------------------------------------------------------

// D: returns true if the concept is updated, i.e. its current hypotheses set 
//    is not empty
bool CConcept::IsUpdated() {
	return GetNumValidHyps() > 0;
}

// D: returns true if the concept is updated and grounded (its current hyp set
//    is not empty, and a hypothesis has been grounded
bool CConcept::IsUpdatedAndGrounded() {
	return IsUpdated() && IsGrounded();
}

// D: returns true if the concept is available, i.e. its current hypotheses 
//    set is not empty (and the concept is not currently undergoing grounding) 
//    or if there's a historical value available
bool CConcept::IsAvailable() {
    if(IsUpdated()) 
        return true;
    if (pPrevConcept)
        return pPrevConcept->IsAvailable();
    else
        return false;
}

// D: returns true if the concept is available and grounded. By default, this 
//    is true if the concept is available, and is not ambiguous, and it's 
//    confidence is above the grounding threshold
bool CConcept::IsAvailableAndGrounded() {
	if(IsUpdatedAndGrounded())
		return true;
	if(pPrevConcept)
		return pPrevConcept->IsAvailableAndGrounded();
	else
		return false;
}

// D: returns true if the concept is grounded
bool CConcept::IsGrounded() {
	// if this is a history concept, it's grounded
	if(bHistoryConcept) return true;

    // if there is no grounding model 
    if(pGroundingModel == NULL) {
        // if there is an owner concept
        if(pOwnerConcept != NULL) 
            // defer to it
            return pOwnerConcept->IsGrounded();
        else 
            // o/w return true
		    return true;
    } else
        // if there is a grounding model
		return bGrounded;
}

// A: returns true if the concept has a partial hypothesis 
//    (for concept that can be updated through partial events, 
//     e.g. partial recognition hypothesis of user utterances)
bool CConcept::HasPartialUpdate() {
	return vhPartialHypSet.size() > 0;
}

// D: returns true if the concept is undergoing grounding (i.e. one of the 
//    actions of its grounding model is on the stack)
bool CConcept::IsUndergoingGrounding() {
    // if it has a grounding model
    if(pGroundingModel) 
        return pGroundingManager->GroundingInProgressOnConcept(this);
    else
        return false;
}

// D: returns true if the concept has been invalidated
bool CConcept::IsInvalidated() {
    return GetInvalidatedFlag();
}

// D: returns true if the concept has been restored for grounding
bool CConcept::IsRestoredForGrounding() {
    return GetRestoredForGroundingFlag();
}

//-----------------------------------------------------------------------------
// Methods implementing conversions to string format
//-----------------------------------------------------------------------------

// D: Generate a string representation of the grounded hypothesis (the top
// one, if the concept is grounded)
string CConcept::GroundedHypToString() {
	if(IsUpdatedAndGrounded()) 
		return TopHypToString();
	else 
		return IsInvalidated()?INVALIDATED_CONCEPT:UNDEFINED_CONCEPT;
}

// A: conversion to "mode_value" format
string CConcept::TopHypToString() {
	if(IsUpdated()) {
		// gets the top hypothesis and returns a string representation
		return GetTopHyp()->ValueToString() + "\n";
	} else {
		return IsInvalidated()?INVALIDATED_CONCEPT:UNDEFINED_CONCEPT;
	}
}

// D: conversion to value/conf;value/conf... format
string CConcept::HypSetToString() {
	// go through the hypset, and convert each one to string
	string sResult;
	if(IsUpdated()) {
		for(int i = 0; i < (int)vhCurrentHypSet.size(); i++) 
			sResult += 
                (vhCurrentHypSet[i]?vhCurrentHypSet[i]->ToString():UNDEFINED_VALUE) + 
				HYPS_SEPARATOR;
		return sResult.substr(0, sResult.length()-1) + "\n";
	} else {
		return IsInvalidated()?INVALIDATED_CONCEPT"\n":UNDEFINED_CONCEPT"\n";
	}

	if (HasPartialHyp()) {
		sResult += " (PARTIAL:";
		for(int i = 0; i < (int)vhPartialHypSet.size(); i++) 
			sResult += 
                (vhPartialHypSet[i]?vhPartialHypSet[i]->ToString():UNDEFINED_VALUE) + 
				HYPS_SEPARATOR;
		sResult = sResult.substr(0, sResult.length()-1) + ")";
	}

	return sResult + "\n";
}

//-----------------------------------------------------------------------------
// Methods providing access to concept type and source
//-----------------------------------------------------------------------------

// D: return the concept type
TConceptType CConcept::GetConceptType() {
	return ctConceptType;
}

// D: set the concept type
void CConcept::SetConceptType(TConceptType ctAConceptType) {
	ctConceptType = ctAConceptType;
}

// D: returns the concept source
TConceptSource CConcept::GetConceptSource() {
	return csConceptSource;
}

// D: set the concept source
void CConcept::SetConceptSource(TConceptSource csAConceptSource) {
	csConceptSource = csAConceptSource;
}

//-----------------------------------------------------------------------------
// Methods providing access to concept name
//-----------------------------------------------------------------------------

// D: set the concept name
void CConcept::SetName(string sAName) {
    if(bHistoryConcept) 
        FatalError(FormatString("Cannot SetName on concept (%s) history.", 
            sName.c_str()));
	sName = sAName;
}

// D: return the concept name
string CConcept::GetName() {
	return sName;
}

// D: return the small concept name
string CConcept::GetSmallName() {
	string sFoo, sSmallName;
	SplitOnLast(sName, ".", sFoo, sSmallName);
	return sSmallName;
}

// D: return the name of the concept, qualified with the name of the owner
//    agent (i.e. AgentName/ConceptName)
string CConcept::GetAgentQualifiedName() {
    if(pOwnerDialogAgent != NULL) {
        return FormatString("%s/%s", pOwnerDialogAgent->GetName().c_str(),
            GetName().c_str());
    } else {
        return GetName();
    }
}

//-----------------------------------------------------------------------------
// Methods providing access to owner dialog agent
//-----------------------------------------------------------------------------

// D: Set the owner dialog agent
void CConcept::SetOwnerDialogAgent(CDialogAgent* pADialogAgent) {
    pOwnerDialogAgent = pADialogAgent;
}

// D: Access to the owner dialog agent
CDialogAgent* CConcept::GetOwnerDialogAgent() {
    return pOwnerDialogAgent;
}

//-----------------------------------------------------------------------------
// Methods providing access to owner concept
//-----------------------------------------------------------------------------

// D: Set the owner concept
void CConcept::SetOwnerConcept(CConcept* pAConcept) {
    pOwnerConcept = pAConcept;
	if(pPrevConcept) 
		pPrevConcept->SetOwnerConcept(pAConcept);
}

// D: Access to the owner concept
CConcept* CConcept::GetOwnerConcept() {
    return pOwnerConcept;
}

//-----------------------------------------------------------------------------
// Methods related to the grounding model
//-----------------------------------------------------------------------------

// D: Create a grounding model
void CConcept::CreateGroundingModel(string sGroundingModelSpec) {

	string sThisSpec = "";
	string sDefaultGroundingModelSpec = "";

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
					"Could not create a grounding model for concept"
					" %s. Ill formed grounding model specification: %s.",
					GetAgentQualifiedName().c_str(), vsGMSpecs[i].c_str()));
			}
		}
	}
	
	// A: no specification given, fall back on the default one
	if (sThisSpec == "") {
		sThisSpec = sDefaultGroundingModelSpec;
	}

	if(!(pGroundingManager->GetConfiguration().bGroundConcepts) ||
		(sThisSpec == "none") || (sThisSpec == "")) {
        pGroundingModel = NULL;
    } else {
        pGroundingModel = (CGMConcept *)
            pGroundingManager->CreateGroundingModel(
			    pGroundingManager->GetConfiguration().sConceptGM,
			    sThisSpec);
        pGroundingModel->Initialize();
        pGroundingModel->SetConcept(this);
    }
}

// D: Return a pointer to the grounding model
CGMConcept* CConcept::GetGroundingModel() {
    if(bHistoryConcept)
        FatalError(FormatString("Trying to access grounding model on history "\
            "concept %s.", sName.c_str()));

	return pGroundingModel;
}

// D: Sets the grounded flag on the concept
void CConcept::SetGroundedFlag(bool bAGrounded) {
	bGrounded = bAGrounded;
	// now if the concept was set to grounded and it was restored for grounding
	if(bGrounded && bRestoredForGrounding) {
        // check that we indeed have a previous concept 
        if(!pPrevConcept)
            FatalError(FormatString("Concept %s was restored for grounding, "
                "now it's grounded, but has no history.", GetName().c_str()));
        // check if the top hypothesis of the previous concept is the same
        // as the top hypothesis of he current one
        CHyp* pHTopHyp = pPrevConcept->GetTopHyp();
        CHyp* pTopHyp = GetTopHyp();
        if(pHTopHyp && pTopHyp && (*pHTopHyp == *pTopHyp)) {
            // if they are equal, then just copy the current hypset in history
            pPrevConcept->CopyCurrentHypSetFrom(*this);
            // and delete the current hypset, but do not notify the change
            if(bChangeNotification) {
                DisableChangeNotification();
                ClearCurrentHypSet();
                EnableChangeNotification();
            } else {
                ClearCurrentHypSet();
            }
        } 
        // set the restored for grounding flag to false
        SetRestoredForGroundingFlag(false);
	}
}

// D: Reads the grounded flag on the concept
bool CConcept::GetGroundedFlag() {
	return bGrounded;
}

// D: Declare the grounding models subsumed by this concept
void CConcept::DeclareGroundingModels(
    TGroundingModelPointersVector& rgmpvModels,
    TGroundingModelPointersSet& rgmpsExclude) {

    // check that this is not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Trying to access grounding models on history "\
            "concept %s.", sName.c_str()));

    // now add the grounding model associated with this concept
    if(pGroundingModel && 
        rgmpsExclude.find(pGroundingModel) == rgmpsExclude.end()) {
        rgmpvModels.push_back(pGroundingModel);
        rgmpsExclude.insert(pGroundingModel);
    }
}

// D: Declare the subsumed concepts
void CConcept::DeclareConcepts(TConceptPointersVector& rcpvConcepts, 
							   TConceptPointersSet& rcpsExclude) {

    // now add this concept if it's not already in the list
	if(rcpsExclude.find(this) == rcpsExclude.end()) {
        rcpvConcepts.push_back(this);
        rcpsExclude.insert(this);
    }
}

//-----------------------------------------------------------------------------
// Methods related to the invalidated flag
//-----------------------------------------------------------------------------

// D: set the invalidated flag
void CConcept::SetInvalidatedFlag(bool bAInvalidated) {
    // set the flag
    bInvalidated = bAInvalidated;
    // if the concept has been restored for grounding, and how has just been
    // invalidated, then invalidate the history value
    if(IsRestoredForGrounding() && pPrevConcept) {
        // set the invalidated flag on the history also
        pPrevConcept->SetInvalidatedFlag(bAInvalidated);
        // then if the concept is set to invalidated, clear the restored for
        // grounding flag
        if(bAInvalidated) 
            SetRestoredForGroundingFlag(false);
    }   
    // and break the seal
    BreakSeal();
}

// D: return the invalidated flag
bool CConcept::GetInvalidatedFlag() {
    return bInvalidated;
}

//-----------------------------------------------------------------------------
// Methods related to the restored for grounding flag
//-----------------------------------------------------------------------------

// D: set the restored for grounding flag
void CConcept::SetRestoredForGroundingFlag(bool bARestoredForGrounding) {
    // set the flag
    if(bARestoredForGrounding) {
        bRestoredForGrounding = true;
    } else {
        bRestoredForGrounding = false;
    }
}

// D: return the restored for grounding flag
bool CConcept::GetRestoredForGroundingFlag() {
    return bRestoredForGrounding;
}

//-----------------------------------------------------------------------------
// Methods related to sealing 
//-----------------------------------------------------------------------------

// D: Seal the concept
void CConcept::Seal() {
	bSealed = true;
}

// D: Set the value of the seal flag to false
void CConcept::BreakSeal() {
	bSealed = false;
}

// D: Check if the concept is sealed
bool CConcept::IsSealed() {
	return bSealed;
}

//-----------------------------------------------------------------------------
// Methods related to signaling concept changes
//-----------------------------------------------------------------------------

// D: Disable concept change notifications
void CConcept::DisableChangeNotification() {
	SetChangeNotification(false);
}

// D: Enable concept change notifications
void CConcept::EnableChangeNotification() {
	SetChangeNotification(true);
}

// D: Set the concept change notifications flag
void CConcept::SetChangeNotification(
	bool bAChangeNotification) {
	bChangeNotification = bAChangeNotification;
}

// D: Processing that happens each time the concept changes
void CConcept::NotifyChange() {
    // set the grounded flag to false
    SetGroundedFlag(false);
    // set the invalidated flag to false
    SetInvalidatedFlag(false);
    // break the seal (if one existed)
    BreakSeal();
    // set the turn last updated information
    MarkTurnLastUpdated();
	// set to not conveyed
	cConveyance = cNotConveyed;
    // clear the conveyance waiting
    ClearWaitingConveyance();
    // and signal the grounding if appropriate
    if(pGroundingModel && bChangeNotification) 
        pGroundingManager->RequestConceptGrounding(this);
}

//-----------------------------------------------------------------------------
// Methods related to the current hypotheses set and belief manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CConcept::HypFactory() {
	FatalError(FormatString(
		"HypFactory call on %s (%s type) concept failed.",
		sName.c_str(), ConceptTypeAsString[ctConceptType].c_str()));
	return NULL;
}

// D: adds a hypothesis to the current set of hypotheses
int CConcept::AddHyp(CHyp* pAHyp) {
    vhCurrentHypSet.push_back(pAHyp);
    iNumValidHyps++;
	// notify the concept change
    NotifyChange();
	return (int)(vhCurrentHypSet.size() - 1);
}

// D: adds a new hypothesis to the current set of hypotheses
int CConcept::AddNewHyp() {
    vhCurrentHypSet.push_back(HypFactory());
    iNumValidHyps++;
	// notify the concept change
    NotifyChange();
	return (int)(vhCurrentHypSet.size() - 1);
}

// D: adds a null hypothesis to the current set of hypotheses
int CConcept::AddNullHyp() {
    vhCurrentHypSet.push_back(NULL);
	// notify the concept change
    NotifyChange();
    return (int)(vhCurrentHypSet.size() - 1);
}

// D: sets a hypothesis into a location
void CConcept::SetHyp(int iIndex, CHyp* pHyp) {
    // first set it to null
    SetNullHyp(iIndex);
    // check if pHyp is null, then return
    if(!pHyp) return;
    // create a new hypothesis
    vhCurrentHypSet[iIndex] = HypFactory();
    // copy the contents
    *(vhCurrentHypSet[iIndex]) = *pHyp;
    iNumValidHyps++;
    // notify the change
    NotifyChange();
}

// D: sets a null hypothesis into a location
void CConcept::SetNullHyp(int iIndex) {
    // if it's already null, return
    if(vhCurrentHypSet[iIndex] == NULL) return;
    // o/w delete it
    delete vhCurrentHypSet[iIndex];
    // and set it to null
    vhCurrentHypSet[iIndex] = NULL;
    iNumValidHyps--;
    // notify the change
    NotifyChange();
}

// D: deletes a hypothesis at a given location
void CConcept::DeleteHyp(int iIndex) {
	if(vhCurrentHypSet[iIndex] != NULL) {
		// if it's not null, destroy it
		delete vhCurrentHypSet[iIndex];
		iNumValidHyps--;
	}
	// then delete it from the array
	vhCurrentHypSet.erase(vhCurrentHypSet.begin() + iIndex);
    // notify the change
    NotifyChange();
}

// D: return the hypothesis at a given index
CHyp* CConcept::GetHyp(int iIndex) {
    if((iIndex < 0) || (iIndex >= (int)vhCurrentHypSet.size())) {
        FatalError(FormatString(
            "Index out of bounds in GetHyp call on %s (%s type) concept",
            sName.c_str(), ConceptTypeAsString[ctConceptType].c_str()).c_str());
    }
    return vhCurrentHypSet[iIndex];
}

// D: return the index of a given hypothesis
int CConcept::GetHypIndex(CHyp* pHyp) {
    for(int i = 0; i < (int)vhCurrentHypSet.size(); i++) 
        if((vhCurrentHypSet[i] != NULL) && 
            (*pHyp == *(vhCurrentHypSet[i])))
            return i;
    // o/w if we reached the end return -1
    return -1;
}

// D: return the confidence of a certain hypothesis (specified by index)
float CConcept::GetHypConfidence(int iIndex) {
    CHyp* pHyp = GetHyp(iIndex);
    return pHyp?pHyp->GetConfidence():0;
}

// D: set the confidence for a certain hypothesis (specified by the index)
void CConcept::SetHypConfidence(int iIndex, float fConfidence) {
	CHyp* pHyp = GetHyp(iIndex);
	if(pHyp) {
		if(pHyp->GetConfidence() != fConfidence) {
			pHyp->SetConfidence(fConfidence);
			// notify the concept change
			NotifyChange();
		}
	} else {
		FatalError(FormatString(
			"Hyp (%d) not found in SetHypConfidence on concept %s.",
			iIndex, GetAgentQualifiedName().c_str()));
	}
}

// D: return the top hypothesis
CHyp* CConcept::GetTopHyp() {
    int iTopHypIndex = GetTopHypIndex();
    if(iTopHypIndex == -1)
        return NULL;
    else
        return GetHyp(iTopHypIndex);
}

// D: return the top hyp index
int CConcept::GetTopHypIndex() {
	// if no valid hyps, return -1
    if(iNumValidHyps == 0) return -1;
    // o/w compute the item with the max confidence
    float fMaxConfidence = 0;
    int iSelected = -1;
    for(int h = 0; h < (int)vhCurrentHypSet.size(); h++) {
		if(vhCurrentHypSet[h] != NULL) {
			if(vhCurrentHypSet[h]->GetConfidence() > fMaxConfidence) {
				fMaxConfidence = vhCurrentHypSet[h]->GetConfidence();
				iSelected = h;
			}
		}
    }
    // return the computed one
    return iSelected;
}

// D: return the second best hyp index
int CConcept::Get2ndHypIndex() {
	// if we don't have at least 2 valid hyps, return -1
    if(iNumValidHyps < 2) return -1;
    
	// o/w get the top hypothesis index
	int iTopHypIndex = GetTopHypIndex();
	
	float fMaxConfidence = 0;
    int iSelected = -1;
    for(int h = 0; h < (int)vhCurrentHypSet.size(); h++) {
		if((h != iTopHypIndex) && vhCurrentHypSet[h] != NULL) {
			if(vhCurrentHypSet[h]->GetConfidence() > fMaxConfidence) {
				fMaxConfidence = vhCurrentHypSet[h]->GetConfidence();
				iSelected = h;
			}
		}
    }
    // return the computed one
    return iSelected;
}

// D: return the confidence score of the top hypothesis
float CConcept::GetTopHypConfidence() {
    CHyp* pTopHyp = GetTopHyp();
    if(pTopHyp == NULL)
        return 0;
    else 
        return pTopHyp->GetConfidence();
}

// D: check if a hypothesis is valid (confidence score > 0; valus is not 
// NULL)
bool CConcept::IsValidHyp(int iIndex) {
    return GetHyp(iIndex) != NULL;
}

// D: return the total number of hypotheses for a concept (including 
// NULLs)
int CConcept::GetNumHyps() {
    return (int)vhCurrentHypSet.size();
}

// D: return the number of valid hypotheses for a concept
int CConcept::GetNumValidHyps() {
    return iNumValidHyps;
}

// D: clear the current set of hypotheses for the concept
void CConcept::ClearCurrentHypSet() {
    // if it's already clear, return
	if(vhCurrentHypSet.size() == 0) return;
    // go through all the valconfs and deallocate them
	for(int h = 0; h < (int)vhCurrentHypSet.size(); h++) {
		if(vhCurrentHypSet[h] != NULL) 
			delete vhCurrentHypSet[h];
	}
    vhCurrentHypSet.clear();
	// finally, reset the number of valid hypotheses
	iNumValidHyps = 0;
    // and notify the change
    NotifyChange();
}

// D: copies the current set of hypotheses from another concept
void CConcept::CopyCurrentHypSetFrom(CConcept& rAConcept) {
    // first clear it
    ClearCurrentHypSet();
    // then go through all the hypotheses from the source concept
    for(int h = 0; h < rAConcept.GetNumHyps(); h++) {
        CHyp* pHyp;
        if((pHyp = rAConcept.GetHyp(h)) != NULL) {
			// this will notify the change
            AddNewHyp();
            *(vhCurrentHypSet[h]) = *pHyp;
        } else {
			// this will notify the change
            AddNullHyp();
        }
    }
    iNumValidHyps = rAConcept.iNumValidHyps;
    // copy the explicitly confirmed and disconfirmed hyps
    sExplicitlyConfirmedHyp = rAConcept.GetExplicitlyConfirmedHypAsString();
    sExplicitlyDisconfirmedHyp = rAConcept.GetExplicitlyDisconfirmedHypAsString();
}

// D: sets the cardinality of the hypset
void CConcept::SetCardinality(int iACardinality) {
    iCardinality = iACardinality;
}

// D: returns the cardinality of the hypset
int CConcept::GetCardinality() {
    return iCardinality;
}

// D: returns the prior for a hypothesis
float CConcept::GetPriorForHyp(CHyp* pHyp) {
	// by default, first look at the information from the grounding manager
	// about priors on a concept
	float fPrior = pGroundingManager->GetPriorForConceptHyp(
		GetSmallName(), pHyp?pHyp->ValueToString():"<UNDEFINED>");
	// if the grounding manager has information about it
	if(fPrior != -1) {
		// then return it
		return fPrior;
	} 
	// otherwise return a uniform prior
	return (float) 1.0 / (iCardinality + 1);
}

// D: returns the confusability for a hypothesis
float CConcept::GetConfusabilityForHyp(CHyp* pHyp) {
	// by default, first look at the information from the grounding manager
	// about confusability on a concept
	float fConfusability = 
		pGroundingManager->GetConfusabilityForConceptHyp(
			GetSmallName(), pHyp?pHyp->ValueToString():"<UNDEFINED>");
	// if the grounding manager has information about it
	if(fConfusability != -1) {
		// then return it
		return fConfusability;
	} 
	// otherwise return a neutral 0.5
	return 0.5;
}

// D: returns the concept type information for a concept
string CConcept::GetConceptTypeInfo() {
	// by default, call on the grounding manager
	return pGroundingManager->GetConceptTypeInfoForConcept(GetSmallName());
}

// D: set the explicitly confirmed hyp
void CConcept::SetExplicitlyConfirmedHyp(CHyp* pHyp) {
    SetExplicitlyConfirmedHyp(pHyp->ValueToString());
}

// D: alternate function for settting the explicitly confirmed hyp
void CConcept::SetExplicitlyConfirmedHyp(string sAExplicitlyConfirmedHyp) {
    sExplicitlyConfirmedHyp = sAExplicitlyConfirmedHyp;
}

// D: set the explicitly disconfirmed hyp
void CConcept::SetExplicitlyDisconfirmedHyp(CHyp* pHyp) {
    SetExplicitlyDisconfirmedHyp(pHyp->ValueToString());
}

// D: alternate function for settting the explicitly disconfirmed hyp
void CConcept::SetExplicitlyDisconfirmedHyp(string sAExplicitlyDisconfirmedHyp) {
    sExplicitlyDisconfirmedHyp = sAExplicitlyDisconfirmedHyp;
}

// D: return the explicitly confirmed hyp
string CConcept::GetExplicitlyConfirmedHypAsString() {
    return sExplicitlyConfirmedHyp;
}

// D: return the explicitly confirmed hyp
string CConcept::GetExplicitlyDisconfirmedHypAsString() {
    return sExplicitlyDisconfirmedHyp;
}

// D: clears the explicitly confirmed hyp
void CConcept::ClearExplicitlyConfirmedHyp() {
    sExplicitlyConfirmedHyp = "";
}

// D: clears the explicitly confirmed hyp
void CConcept::ClearExplicitlyDisconfirmedHyp() {
    sExplicitlyConfirmedHyp = "";
}

//---------------------------------------------------------------------
// Methods providing access to partially updated values
//---------------------------------------------------------------------

// A: adds a partial hypothesis to the current set of partial hypotheses
int CConcept::AddPartialHyp(CHyp* pAHyp) {
    vhPartialHypSet.push_back(pAHyp);
    iNumValidPartialHyps++;
	return (int)(vhPartialHypSet.size() - 1);
}

// A: adds a new partial hypothesis to the current set of partial hypotheses
int CConcept::AddNewPartialHyp() {
    vhPartialHypSet.push_back(HypFactory());
    iNumValidPartialHyps++;
	return (int)(vhPartialHypSet.size() - 1);
}

// A: adds a null hypothesis to the current set of partial hypotheses
int CConcept::AddNullPartialHyp() {
    vhPartialHypSet.push_back(NULL);
    return (int)(vhPartialHypSet.size() - 1);
}

// A: indicates whether a partial hypothesis is currently available 
bool CConcept::HasPartialHyp() {
	return iNumValidPartialHyps > 0;
}

// A: returns one partial hypothesis
CHyp* CConcept::GetPartialHyp(int iIndex) {
    if((iIndex < 0) || (iIndex >= (int)vhPartialHypSet.size())) {
        FatalError(FormatString(
            "Index out of bounds in GetPartialHyp call on %s (%s type) concept",
            sName.c_str(), ConceptTypeAsString[ctConceptType].c_str()).c_str());
    }
    return vhPartialHypSet[iIndex];
}

// return the top partial hypothesis
CHyp* CConcept::GetTopPartialHyp() {
    int iTopPartialHypIndex = GetTopPartialHypIndex();
    if(iTopPartialHypIndex == -1)
        return NULL;
    else
        return GetPartialHyp(iTopPartialHypIndex);
}

// A: return the top partial hyp index
int CConcept::GetTopPartialHypIndex() {
	// if no valid hyps, return -1
    if(iNumValidPartialHyps == 0) return -1;
    // o/w compute the item with the max confidence
    float fMaxConfidence = 0;
    int iSelected = -1;
    for(int h = 0; h < (int)vhPartialHypSet.size(); h++) {
		if(vhPartialHypSet[h] != NULL) {
			if(vhPartialHypSet[h]->GetConfidence() > fMaxConfidence) {
				fMaxConfidence = vhPartialHypSet[h]->GetConfidence();
				iSelected = h;
			}
		}
    }
    // return the computed one
    return iSelected;
}

// A: return the confidence score of the top partial hypothesis
float CConcept::GetTopPartialHypConfidence() {
    CHyp* pTopPartialHyp = GetTopPartialHyp();
    if(pTopPartialHyp == NULL)
        return 0;
    else 
        return pTopPartialHyp->GetConfidence();
}

// A: check if a partial hypothesis is valid (confidence score > 0; 
// valus is not NULL)
bool CConcept::IsValidPartialHyp(int iIndex) {
    return GetPartialHyp(iIndex) != NULL;
}

// A: return the total number of partial hypotheses for a concept 
// (including NULLs)
int CConcept::GetNumPartialHyps() {
    return (int)vhPartialHypSet.size();
}

// A: return the number of valid partial hypotheses for a concept
int CConcept::GetNumValidPartialHyps() {
    return iNumValidPartialHyps;
}
// A: clears the current partial value of the concept
void CConcept::ClearPartialHypSet() {
	// reset the partial hyp set
	// go through all the valconfs and deallocate them
	for(int h = 0; h < (int)vhPartialHypSet.size(); h++) {
		if(vhPartialHypSet[h] != NULL) 
			delete vhPartialHypSet[h];
	}
	vhPartialHypSet.clear();
	// finally, reset the number of valid hypotheses
	iNumValidPartialHyps = 0;
}


//-----------------------------------------------------------------------------
// Methods providing access to turn last updated information
//-----------------------------------------------------------------------------

// D: Set the turn the concept was last updated
void CConcept::SetTurnLastUpdated(int iTurn) {
    iTurnLastUpdated = iTurn;
}

// D: Mark now as the turn in which the concept was last updated
void CConcept::MarkTurnLastUpdated() {
	SetTurnLastUpdated(pDMCore->GetLastInputTurnNumber());
}

// D: Access to turn in which the concept was last updated information
int CConcept::GetTurnLastUpdated() {
    return iTurnLastUpdated;
}

// D: returns the number of turns that have elapsed since the concept was 
//    last updated
int CConcept::GetTurnsSinceLastUpdated() {
    return pDMCore->GetLastInputTurnNumber() - GetTurnLastUpdated();
}

//-----------------------------------------------------------------------------
// Methods providing access to conveyance information
//-----------------------------------------------------------------------------

// D: sets the waiting for conveyance flag
void CConcept::SetWaitingConveyance() {
    bWaitingConveyance = true;
}

// D: clear the waiting for conveyance flag
void CConcept::ClearWaitingConveyance() {
	if(bWaitingConveyance) {
		bWaitingConveyance = false;
		if(pOutputManager) 
			pOutputManager->CancelConceptNotificationRequest(this);
	}
}

// A: set the conveyance information
void CConcept::SetConveyance(TConveyance cAConveyance) {
	cConveyance = cAConveyance;
}

// A: return the conveyance information
TConveyance CConcept::GetConveyance() {
	return cConveyance;
}

// D: clear the concept notification pointer
void CConcept::ClearConceptNotificationPointer() {
    if(pOutputManager)
        pOutputManager->ChangeConceptNotificationPointer(this, NULL);
}

//-----------------------------------------------------------------------------
// Methods for concept history manipulation
//-----------------------------------------------------------------------------

// D: reopens the concept (i.e. moves current value into history, and starts
//    with a clean new value
void CConcept::ReOpen() {

	// first check that it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform ReOpen on concept (%s) history.",
            sName.c_str()));

    // record the initial value of the concept (if the concept has a grounding
    //  model)
    string sInitialValue;
    if(pGroundingModel) 
        sInitialValue = TrimRight(HypSetToString());

	// create a clone of the current concept (without the history)
    CConcept* pConcept = Clone(false);

    // collapse it to the mode (this also sets it to grounded)
    pConcept->Update(CU_COLLAPSE_TO_MODE, NULL);

	// set the flag on it that it's a history concept
    pConcept->SetHistoryConcept(true);

    // links it to the current history
	pConcept->pPrevConcept = pPrevConcept;

    // and push it into the history
    pPrevConcept = pConcept;

    // finally, clear the current value (which notifies the change)
    ClearCurrentHypSet(); 

    // now log the update (if the concept has a grounding model
    if(pGroundingModel) 
        Log(CONCEPT_STREAM, FormatString(
            "Concept update [reopen] on %s:\nInitial value dumped below:\n%s\n"
            "Updated value dumped below:\n%s", 
            GetAgentQualifiedName().c_str(),
            sInitialValue.c_str(),
            TrimRight(HypSetToString()).c_str()));
}

// D: restores the concept (i.e. restores the concept to a previous incarnation
//    from its history
void CConcept::Restore(int iIndex) {

	// first check if it's not a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform Restore on concept (%s) history.",
            sName.c_str()));

    // record the initial value of the concept (if the concept has a grounding
    //  model)
    string sInitialValue;
    if(pGroundingModel) 
        sInitialValue = TrimRight(HypSetToString());

    // check that the index is not zero 
    if(iIndex == 0) return;

    // get the history version that we need to restore to
    CConcept* pConcept = &(operator[](iIndex));

    // restore values from that version (this will notify the change)
    CopyCurrentHypSetFrom(*pConcept);

	// set it to grounded (since it comes from history)
	SetGroundedFlag(true);

    // and clear the history
    ClearHistory();

    // now log the update (if the concept has a grounding model)
    if(pGroundingModel) 
        Log(CONCEPT_STREAM, FormatString(
            "Concept update [restore] on %s:\nInitial value dumped below:\n%s\n"
            "Updated value dumped below:\n%s", 
            GetAgentQualifiedName().c_str(),
            sInitialValue.c_str(),
            TrimRight(HypSetToString()).c_str()));
}

// D: clears the history of the current concept
void CConcept::ClearHistory() {
    // check if it's a history concept
    if(bHistoryConcept)
        FatalError(FormatString("Cannot perform ClearHistory on concept (%s) history.",
            sName.c_str()));

    // o/w merely delete all its history
    if(pPrevConcept != NULL) {
        delete pPrevConcept;
        pPrevConcept = NULL;
    }
}

// D: merges the history on the concept, and returns a new concept containing 
//    that 
CConcept* CConcept::CreateMergedHistoryConcept() {
	
	// check if the concept is updated or invalidated
	if(IsUpdated() || IsInvalidated()) {
	
        // o/w if the concept is updated, create clone of the current value
        // (w/o the history)
        CConcept *pMergedHistory = Clone(false);
	    // set it to not a history concept
        pMergedHistory->SetHistoryConcept(false);
	    // return it
        return pMergedHistory;
        
	} else {
	
        // o/w check for a previous concept
        if(pPrevConcept == NULL) {
            // and if there's no history then just return NULL
            return NULL;
        } else {
            // o/w defer to previous concept in history
            return pPrevConcept->CreateMergedHistoryConcept();
        }        
    } 
}

// D: merges the history of the concept into the current value
void CConcept::MergeHistory() {

    // record the initial value of the concept (if the concept has a grounding
    //  model)
    string sInitialValue;
    if(pGroundingModel) 
        sInitialValue = TrimRight(HypSetToString());

    // if the concept is updated or is invalidated, then we just clear it's history
    if(IsUpdated() || IsInvalidated()) {
        // then simply clear it's history
        ClearHistory();
        // and set the invalidated flag to false
        SetInvalidatedFlag(false);
        // and set the restored for grounding flag to false
        SetRestoredForGroundingFlag(false);    
    } else {

        // o/w first create a merged history concept
        CConcept* pMergedHistoryConcept = CreateMergedHistoryConcept();

        if(pMergedHistoryConcept) {
            // then copy this one from it (this will notify the change)
            CopyCurrentHypSetFrom(*pMergedHistoryConcept);
		    // also set the grounding state according to the merged history
		    // concept
		    SetGroundedFlag(pMergedHistoryConcept->GetGroundedFlag());
            // and finally, delete the auxiliary merged history concept
            delete pMergedHistoryConcept;
        }

	    // clear the history of this concept
        ClearHistory();        
    }

    // now log the update (if the concept has a grounding model)
    if(pGroundingModel) 
        Log(CONCEPT_STREAM, FormatString(
            "Concept update [merge_history] on %s:\nInitial value dumped below:\n%s\n"
            "Updated value dumped below:\n%s", 
            GetAgentQualifiedName().c_str(),
            sInitialValue.c_str(),
            TrimRight(HypSetToString()).c_str()));
}

// D: returns the size of the history on the concept
int CConcept::GetHistorySize() {
    // recursively compute history size
    if(pPrevConcept == NULL)
        return 0;
    else 
        return pPrevConcept->GetHistorySize() + 1;
}

// D: returns a certain historical version of a concept
CConcept& CConcept::GetHistoryVersion(int iIndex) {
    // just redirect through the operator
    return operator [] (iIndex);
}

// D: set the history concept flag
void CConcept::SetHistoryConcept(bool bAHistoryConcept) {
	bHistoryConcept = bAHistoryConcept;
}

// D: get the history concept flag
bool CConcept::IsHistoryConcept() {
	return bHistoryConcept;
}

//-----------------------------------------------------------------------------
// Virtual methods that are array-specific
//-----------------------------------------------------------------------------

// D: getsize method: 
int CConcept::GetSize() {
	FatalError(FormatString("GetSize cannot be called on concept %s (%s type).",
					   sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
	return -1;
}

// D: DeleteAt method
void CConcept::DeleteAt(unsigned int iIndex) {
	FatalError(FormatString("DeleteAt cannot be called on concept %s (%s type).",
					   sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
}

// J: InsertAt method
void CConcept::InsertAt(unsigned int iIndex, CConcept &rAConcept) {
	FatalError(FormatString("InsertAt cannot be called on concept %s (%s type).",
					   sName.c_str(), 
					   ConceptTypeAsString[ctConceptType].c_str()));
}

#pragma warning (default:4100)
