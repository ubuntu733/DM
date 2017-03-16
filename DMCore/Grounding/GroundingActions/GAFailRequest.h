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
// GAFAILREQUEST.H   - definition of the FailRequest grounding action class;
//                     this class is used for issuing a "sorry" prompt when 
//                     an agent fails and we're moving on with the task in a
//                     different manner. However, if you want the true 
//                     MOVE_ON action (which forces an agent to fail), then 
//                     check out GAMoveOn
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
//   [2004-08-08] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __GAFAILREQUEST_H__
#define __GAFAILREQUEST_H__


// D: COMMENTS
//    When using the fail request grounding action, the following 
//    prompt needs to be defined:
//    - inform nonunderstanding_failrequest
//        i.e. "Sorry, I didn't catch that"
//    - inform nonunderstanding_subsequent_failrequest
//        i.e. "Sorry, I still didn't catch that"

#include "GroundingAction.h"

//-----------------------------------------------------------------------------
// D: The CGAFailRequest class
//-----------------------------------------------------------------------------

class CGAFailRequest: public CGroundingAction {

public:
  	//---------------------------------------------------------------------
	// Fundamental, overwritten grounding action methods
	//---------------------------------------------------------------------

	// Constructor with configuration string
	//
	CGAFailRequest(string sNewConfiguration);

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
#define FAIL_REQUEST CGAFailRequest

#endif // __GAFAILREQUEST_H__