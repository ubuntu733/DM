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
// GAINTERACTIONTIPS.CPP - definition of the InteractionTips grounding action
//                         class; this grounding action instructs the user 
//                         about how to speak to the system
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
//   [2006-01-30] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

// D: COMMENTS
//    When using the give_up grounding action, the following prompt needs
//    to be defined:
//    - inform inform nonunderstanding_interaction_tips
//        i.e. "Okay, I know this conversation isn’t going well. There are 
//              things you can try to help me understand you better. Speak 
//              clearly and naturally; don’t speak too quickly or too slowly. 
//              Give short, concise answers. Calling from a quiet place 
//              helps. If you’d like to start from scratch, you can say 
//              ‘start-over’ at any time."

#pragma once
#ifndef __GAINTERACTIONTIPS_H__
#define __GAINTERACTIONTIPS_H__

#include "GroundingAction.h"

//-----------------------------------------------------------------------------
// D: The GAInteractionTips class
//-----------------------------------------------------------------------------

class CGAInteractionTips: public CGroundingAction {

public:
  	//---------------------------------------------------------------------
	// Fundamental, overwritten grounding action methods
	//---------------------------------------------------------------------

	// Constructor with configuration string
	//
	CGAInteractionTips(string sNewConfiguration);

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
#define INTERACTION_TIPS CGAInteractionTips

#endif // __GAINTERACTIONTIPS_H__