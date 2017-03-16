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
// GAIMPLICITCONFIRM.CPP - implementation of the implicit confirm grounding 
//                         action class
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
//   [2003-04-15] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------


#include "GAImplicitConfirm.h"
#include "DialogTask/DialogTask.h"


//-----------------------------------------------------------------------------
//
// D: Specification/implementation of the implicit confirm agency
//
//-----------------------------------------------------------------------------

// D: The inform part
DEFINE_INFORM_AGENT(_CImplicitConfirmInform,
    IS_NOT_DTS_AGENT()

    private: 
    // D: the concept and value that are implicitly confirmed
    CConcept* pConfirmedConcept;
    CHyp* pConfirmedHyp;

    public: 
    // D: member function for setting the concept that is confirmed
    void SetConfirmedConcept(CConcept* pAConfirmedConcept) {
        pConfirmedConcept = pAConfirmedConcept;
        pConfirmedHyp = pConfirmedConcept->GetTopHyp();
    }

    // D: member function for accessing the concept that is confirmed
    CConcept* GetConfirmedConcept() {
        return pConfirmedConcept;
    }

    // D: overwritten virtual function for concepts localization
    virtual CConcept& LocalC(string sConceptName) {
        if(sConceptName == pConfirmedConcept->GetName())
            return *pConfirmedConcept;            
        else return CMAInform::LocalC(sConceptName);
    }

    // D: the prompt we're requesting it with is 
	virtual string Prompt() {
        return FormatString("{implicit_confirm %s %s=\"%s\"}", 
            pConfirmedConcept->GetAgentQualifiedName().c_str(),
			pConfirmedConcept->GetName().c_str(),
            pConfirmedHyp->ValueToString().c_str());
	}

    // D: on execute, set the state of the grounding request to executing
    virtual TDialogExecuteReturnCode Execute() {
		// signal the core about the explicit confirm
		pDMCore->SignalImplicitConfirmOnConcept(pConfirmedConcept);
        // and call the inherited execute
        return CMAInform::Execute();
    }

    SUCCEEDS_WHEN((iExecuteCounter > 0) || !pConfirmedConcept->IsSealed())

	// D: on completion
	ON_COMPLETION(
        // signal that the concept grounding has completed
        pGroundingManager->ConceptGroundingRequestCompleted(pConfirmedConcept);
	)
)


// D: the expect part (expects to hear a yes or no in the next turn
DEFINE_EXPECT_AGENT(_CImplicitConfirmExpect,
    IS_NOT_DTS_AGENT()

    private: 
    // D: the concept and value that are implicitly confirmed
    CConcept* pConfirmedConcept;
    CHyp* pConfirmedHyp;

	// D: the turn number when it was put on the stack
	int iStartingTurnNumber;

    public: 
    // D: member function for setting the concept that is confirmed
    void SetConfirmedConcept(CConcept* pAConfirmedConcept) {
        pConfirmedConcept = pAConfirmedConcept;
        pConfirmedHyp = pConfirmedConcept->GetTopHyp();
    }

    // D: member function for accessing the concept that is confirmed
    CConcept* GetConfirmedConcept() {
        return pConfirmedConcept;
    }

    // D: the confirm concept holds the response to the confirmation
    DEFINE_CONCEPTS(
        BOOL_USER_CONCEPT(confirm, "none")
    )

    // D: we're expecting the confirm concept
    EXPECT_CONCEPT(confirm)

    // D: and expecting a yes or a no
    GRAMMAR_MAPPING("@[Yes]>true, @[No]>false")

    // D: the success criterion is that we hear a yes or a no, or that 
    //    the concept is updated in the last turn, or that the concept 
    //    is grounded
    SUCCEEDS_WHEN(
		(pDMCore->GetLastInputTurnNumber() > iStartingTurnNumber) ||
		!pConfirmedConcept->IsSealed())

	// D: on initialization set the iStartingTurnNumber
	ON_INITIALIZATION(
		bCompleted = false;
		iStartingTurnNumber = pDMCore->GetLastInputTurnNumber();
	)

)

//-----------------------------------------------------------------------------
//
// D: Implementation of the implicit confirm action
//
//-----------------------------------------------------------------------------

// A: Constructor with configuration
CGAImplicitConfirm::CGAImplicitConfirm(string sNewConfiguration) : 
CGroundingAction(sNewConfiguration) {}

// D: Return the name of the grounding action
string CGAImplicitConfirm::GetName() {
    return "IMPL_CONF";
}

// D: Run the action
void CGAImplicitConfirm::Run(void *pParams) {
    
    // look for the agency for doing the implicit confirm inform
    string sAgencyName = FormatString("/_ImplicitConfirmInform[%s]", 
        ((CConcept *)pParams)->GetAgentQualifiedName().c_str());
    _CImplicitConfirmInform *pImplicitConfirmInformAgency = (_CImplicitConfirmInform *)
        AgentsRegistry[sAgencyName];
    if(!pImplicitConfirmInformAgency) {
        // if not found, create it
        pImplicitConfirmInformAgency = (_CImplicitConfirmInform *)
            AgentsRegistry.CreateAgent("_CImplicitConfirmInform", sAgencyName);
    	// create the grounding model
	    pImplicitConfirmInformAgency->CreateGroundingModel("request_default");
    	// initialize it
	    pImplicitConfirmInformAgency->Initialize();
        // and register it
        pImplicitConfirmInformAgency->Register();
    } else {
        // o/w reset it
        pImplicitConfirmInformAgency->Reset();
    }

    // look for the agency for doing the implicit confirm expect
    sAgencyName = FormatString("/_ImplicitConfirmExpect[%s]", 
        ((CConcept *)pParams)->GetAgentQualifiedName().c_str());
    _CImplicitConfirmExpect *pImplicitConfirmExpectAgency = (_CImplicitConfirmExpect *)
        AgentsRegistry[sAgencyName];
    if(!pImplicitConfirmExpectAgency) {
        // if not found, create it
        pImplicitConfirmExpectAgency = (_CImplicitConfirmExpect *)
            AgentsRegistry.CreateAgent("_CImplicitConfirmExpect", sAgencyName);
    	// create the grounding model
	    pImplicitConfirmExpectAgency->CreateGroundingModel("request_default");
    	// initialize it
	    pImplicitConfirmExpectAgency->Initialize();
        // and register it
        pImplicitConfirmExpectAgency->Register();
    } else {
        // o/w reset it
        pImplicitConfirmExpectAgency->Reset();
    }


	// set the dynamic agent ID to the name of the concept
    pImplicitConfirmInformAgency->SetDynamicAgentID(
        ((CConcept *)pParams)->GetAgentQualifiedName());
    // but not for the expect agent

    // sets the concept
    pImplicitConfirmInformAgency->SetConfirmedConcept((CConcept *)pParams);
	pImplicitConfirmExpectAgency->SetConfirmedConcept((CConcept *)pParams);

    // and add the explicit confirm agency on the stack 
	// first add the expect (so that it remains on the stack after the inform
	pDMCore->ContinueWith(pGroundingManager, pImplicitConfirmExpectAgency);
    pDMCore->ContinueWith(pGroundingManager, pImplicitConfirmInformAgency);

    // set the grounding request to on stack
    pGroundingManager->SetConceptGroundingRequestStatus(
        (CConcept *)pParams, GRS_EXECUTING);

    // also mark the concept as grounded
    ((CConcept *)pParams)->SetGroundedFlag(true);
}

// D: Register the agencies used by this grounding action
void CGAImplicitConfirm::RegisterDialogAgency() {
    if(!AgentsRegistry.IsRegisteredAgentType("_CImplicitConfirmInform"))
        AgentsRegistry.RegisterAgentType("_CImplicitConfirmInform", 
            _CImplicitConfirmInform::AgentFactory);
    if(!AgentsRegistry.IsRegisteredAgentType("_CImplicitConfirmExpect"))
        AgentsRegistry.RegisterAgentType("_CImplicitConfirmExpect", 
            _CImplicitConfirmExpect::AgentFactory);
}