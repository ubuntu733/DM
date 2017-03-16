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
// GAEXPLICITCONFIRM.H   - definition of the explicit confirm grounding 
//                         action class; this grounding action explicitly 
//                         confirms the top value of a concept
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
//   [2007-03-09] (antoine): fixed a _CRequestConfirm so that it takes its
//							 LM- and DTMF-related parameters from the
//							 configuration of its parent agency
//   [2007-01-01] (antoine): changed the _CExplicitConfirm agent into an
//							 agency with one RequestAgent and one InformAgent
//							 (for acknowledgement of the confirmation)
//   [2004-12-30] (antoine): allowed the configuration of the confirmation
//                           language model and the use of DTMF
//   [2004-12-28] (antoine): added constructor with configuration
//   [2003-02-13] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __GAEXPLICITCONFIRM_H__
#define __GAEXPLICITCONFIRM_H__

// A: COMMENTS
//    The explicit_confirm grounding action uses the explicit_confirm prompt 
//    concept to be confirmed. This prompt must be defined. 
// 
//    Configuration parameters for this grounding action (passed through the
//    USE_GROUNDING_ACTION / USE_ALL_GROUNDING_ACTIONS directives):
//    - grounding_model : the name of the grounding model to be used by the 
//                        explicit confirm request agent (if not specified, 
//                        it is assumed to be request_default)
//	  - confirmation_lm : the name of a language model to be used by the yes/no
//                        confirmation request agent.
//    - dtmf_yes/dtmf_no : the names of grammar slots that map to resp. yes 
//                         and no in the confirmation request 
//                         (e.g. dtmf_yes=DTMF_ONE, dtmf_no=DTMF_THREE)

#include "GroundingAction.h"

//-----------------------------------------------------------------------------
// D: The GAExplicitConfirm class
//-----------------------------------------------------------------------------

class CGAExplicitConfirm: public CGroundingAction {

public:
  	//---------------------------------------------------------------------
	// Fundamental, overwritten grounding action methods
	//---------------------------------------------------------------------

	// Constructor with configuration string
	//
	CGAExplicitConfirm(string sNewConfiguration);

    // Overwritten virtual function to obtain the name of the particular 
    // grounding action
    // 
    virtual string GetName();

    // Overwritten virtual function which implements the actual grounding 
    // action (runs the action). 
    //
    virtual void Run(void *pParams);

    // Overwritten virtual function for registering the actual dialog 
    // agency used by this action
    //
    virtual void RegisterDialogAgency();
};

// D: a define for this grounding action
#define EXPLICIT_CONFIRM CGAExplicitConfirm

#endif // __GAEXPLICITCONFIRM_H__