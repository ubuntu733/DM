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
// GAACCEPT.CPP - implementation of the Accept grounding action class
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
//   [2004-12-28] (antoine): added constructor with configuration
//   [2003-02-14] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------


#include "GAAccept.h"
#include "DialogTask/DialogTask.h"


// A: Constructor with configuration
CGAAccept::CGAAccept(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the grounding action
string CGAAccept::GetName() {
    return "ACCEPT";
}

// D: Run the action - in this case the concept is marked as grounded
void CGAAccept::Run(void *pParams) {
    // mark the concept as grounded (if it's updated)
    if(((CConcept *)pParams)->IsUpdated()) {
        ((CConcept *)pParams)->SetGroundedFlag(true);
    }

    // set the grounding request to executing
    pGroundingManager->SetConceptGroundingRequestStatus(
        (CConcept *)pParams, GRS_EXECUTING);

	// signal the core that we are doing an accept on a concept
	pDMCore->SignalAcceptOnConcept((CConcept *)pParams);

    // and then set it to completed
    pGroundingManager->ConceptGroundingRequestCompleted((CConcept *)pParams);
}

// D: Register the agency used (does nothing)
void CGAAccept::RegisterDialogAgency() {
}