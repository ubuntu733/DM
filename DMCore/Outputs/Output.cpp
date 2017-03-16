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
// OUTPUT.CPP - implementation of the COutput base class. This class represents 
//				an output from the dialog manager, and is used by the 
//				COutputManager core agent. The class is the base of a 
//				hierarchy.
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
//   [2005-10-22] (antoine): added method GetGeneratorAgentName
//   [2005-10-20] (antoine): added a sDialogState field sent along with the
//                           output (this is for the InteractionManager)
//   [2005-01-11] (antoine): added methods for computing and getting a counter
//                            on the number of repeats for a given output
//   [2004-02-25] (dbohus):  added methods for cancelling and changing concept
//                            notification requests
//   [2004-02-24] (dbohus):  changed outputs so that we no longer clone 
//                            concepts but use them directly
//   [2003-10-20] (dbohus):  fixed a bug in clone so that the vector of 
//                            concepts is cloned correctly even when there are
//                            nulls in there
//   [2003-04-10] (dbohus,  
//                 antoine): fixed bug in clone so that the vector of concepts
//                            is indeed cloned
//   [2002-12-03] (dbohus):  fixed bug in FindConceptByName
//   [2002-10-15] (dbohus):  added destructor so that concepts are deleted
//   [2002-10-14] (dbohus):  added GetOutputDevice, GetAct, GetObject, moved 
//                            all members to protected; removed all the Get 
//							  functions put OutputManagerAgent as friend class
//   [2002-10-13] (dbohus):  changed iParams to iFlags, moved flags definitions
//                            here, added CheckFlags function
//   [2002-10-12] (dbohus):  removed FindConcept, since not used so far
//   [2002-06-25] (dbohus):  unified Create and CreateWithClones in a single
//							  create function which takes a bool parameter
//   [2002-06-17] (dbohus):  deemed preliminary stable version 0.5
//	 [2002-06-17] (dbohus):  added method for finding concepts
//	 [2002-06-14] (dbohus):  drafted this class
// 
//-----------------------------------------------------------------------------


#include "Output.h"
#include "DMCore/Log.h"

//-----------------------------------------------------------------------------
// D: Constructors and Destructor
//-----------------------------------------------------------------------------
// A: Constructor - Initializes the repeat counter
COutput::COutput() {
	iRepeatCounter = 0;
}

// D: destructor - does nothing so far
COutput::~COutput() {
}

//-----------------------------------------------------------------------------
// AD: Public methods
//-----------------------------------------------------------------------------

// A: Get name of the agent that generated this output
string COutput::GetGeneratorAgentName() {
	return sGeneratorAgentName;
}

// A: Get execution index corresponding to the generation of this output
void COutput::SetDialogStateIndex(int iAExecutionIndex) {
	iExecutionIndex = iAExecutionIndex;
}

// A: Get execution index corresponding to the generation of this output
int COutput::GetDialogStateIndex() {
	return iExecutionIndex;
}

// A: set dialog state information for this output
void COutput::SetDialogState(string sADialogState) {
	sDialogState = sADialogState;
}

// A: get dialog state information for this output
string COutput::GetDialogState() {
	return sDialogState;
}

// A: set function for conveyance information
void COutput::SetConveyance(TConveyance cAConveyance) {
	cConveyance = cAConveyance;
}

// A: get function for conveyance information
TConveyance COutput::GetConveyance() {
	return cConveyance;
}

// A: Set the dialog act for this output
void COutput::SetAct(string sAAct) {
	sAct = sAAct;
}

// A: Set the dialog act for this output
string COutput::GetAct() {
	return sAct;
}

// A: Set the final floor status for this output
void COutput::SetFinalFloorStatus(TFloorStatus fsAFloor){
	fsFinalFloorStatus = fsAFloor;
}

// A: Get the final floor status for this output
TFloorStatus COutput::GetFinalFloorStatus() {
	return fsFinalFloorStatus;
}

// A: Get a string representation of the final floor status for this output
string COutput::GetFinalFloorStatusLabel() {
	return pDMCore->FloorStatusToString(fsFinalFloorStatus);
}

// D: Checks if certain flags are set for this output
bool COutput::CheckFlag(string sFlag) {
    // go through the list of flags, check if the one we're searching for is
    // in there
    for(unsigned int i=0; i < vsFlags.size(); i++)
        if(vsFlags[i] == sFlag) 
            return true;
    return false;
}

// D: Notifies a concept with conveyance information (used code from 
//    the previous FindConceptByName routine)
void COutput::NotifyConceptConveyance(string sConceptName, 
    TConveyance cAConveyance) {

    // check that the concept looked for is not NULL
    if ( sConceptName == "" ) return;
    
    // partition the concept name on .
	vector<string> vsParts = PartitionString(sConceptName, ".");

    // find base concept
	CConcept *pBaseConcept = NULL;
    unsigned int j = 0;
	for ( unsigned int i = 0; i < vcpConcepts.size(); i++ ) {
		// if the concept should not be notified, continue
		if(!vbNotifyConcept[i]) continue;

        string sPossibleMatch = "";
		for ( j = 0; j < vsParts.size(); j++ ) {
			sPossibleMatch += vsParts[j];
			if ( vcpConcepts[i]->GetName() == sPossibleMatch )
				break;
			sPossibleMatch += ".";
		}
		if ( j < vsParts.size() ) {
			pBaseConcept = vcpConcepts[i];
			break;
		}
	}

    // if no base concept was matching, return 
	if ( pBaseConcept == NULL ) return;

    // then index into it
    string sIndex = "";
    for( unsigned int i = j + 1; i < vsParts.size(); i++ )
        sIndex += vsParts[i] + ".";
    sIndex = TrimRight(sIndex, ".");

    // log the conveyance information
    Log(OUTPUTMANAGER_STREAM, 
        FormatString("Setting conveyance for concept %s -> %s",
        sConceptName.c_str(), ConveyanceAsString[cAConveyance].c_str()));


    pBaseConcept->operator[](sIndex).SetConveyance(cAConveyance);
}

// D: returns a pointer to the concept refered by this output, given
//    the concept name
CConcept* COutput::GetConceptByName(string sConceptName) {

    // use the registry to find the agent name
    CDialogAgent* pGeneratorAgent = 
        (CDialogAgent *)AgentsRegistry[sGeneratorAgentName];
    if(!pGeneratorAgent) return NULL;

    // partition the concept name on .
	vector<string> vsParts = PartitionString(sConceptName, ".");

    // find base concept
	CConcept *pBaseConcept = NULL;
    unsigned int j = 0;
	for ( unsigned int i = 0; i < vcpConcepts.size(); i++ ) {
        string sPossibleMatch = "";
		for ( j = 0; j < vsParts.size(); j++ ) {
			sPossibleMatch += vsParts[j];
			if ( vcpConcepts[i] && (vcpConcepts[i]->GetName() == sPossibleMatch) )
				break;
			sPossibleMatch += ".";
		}
		if ( j < vsParts.size() ) {
			pBaseConcept = vcpConcepts[i];
			break;
		}
	}

    // if no base concept was matching, return NULL
	if ( pBaseConcept == NULL ) return NULL;

    // if a base concept was matching, then index into it
    string sIndex = "";
    for( unsigned int i = j + 1; i < vsParts.size(); i++ )
        sIndex += vsParts[i] + ".";
    sIndex = TrimRight(sIndex, ".");
    
    // and search for that concept through the generator agent
    return &(pGeneratorAgent->C(pBaseConcept->operator[](sIndex).GetName()));
}

// D: Cancels the notification request for one of the concepts
void COutput::CancelConceptNotificationRequest(CConcept* pConcept) {
    for (unsigned int i = 0; i < vcpConcepts.size(); i++) 
        if(vcpConcepts[i] == pConcept) 
            vbNotifyConcept[i] = false;
}

// D: Changes the pointers for one of the concepts (this happens on 
// reopens and other operations which change the concept pointers)
void COutput::ChangeConceptNotificationPointer(CConcept* pOldConcept, 
                                               CConcept* pNewConcept) {
    for (unsigned int i = 0; i < vcpConcepts.size(); i++) 
        if(vcpConcepts[i] == pOldConcept) 
            vcpConcepts[i] = pNewConcept;
}

// Gets the number of times this prompt has been uttered
int COutput::GetRepeatCounter() {
	return iRepeatCounter;
}

// Increments the repeat counter
void COutput::IncrementRepeatCounter() {
	iRepeatCounter++;
}

//-----------------------------------------------------------------------------
// AD: Protected, auxiliary helper methods
//-----------------------------------------------------------------------------

// A: Auxiliary concept function for cloning an output (is to be used by 
//    the overwritten Clone() methods of derived classes
void COutput::clone(COutput * opClone, int iNewOutputId) {
	// copy all the information
	opClone->sGeneratorAgentName = sGeneratorAgentName;
	opClone->iOutputId = iNewOutputId;		
	opClone->sAct = sAct;			
	opClone->sObject = sObject;
	opClone->fsFinalFloorStatus = fsFinalFloorStatus;
    opClone->vcpConcepts = vcpConcepts;
    opClone->vbNotifyConcept = vbNotifyConcept;
	opClone->vsFlags = vsFlags;
	opClone->sOutputDeviceName = sOutputDeviceName;
	opClone->cConveyance = cNotConveyed;
	opClone->iRepeatCounter = iRepeatCounter;
}

