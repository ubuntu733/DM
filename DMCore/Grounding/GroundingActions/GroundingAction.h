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
// GROUNDINGACTION.H   - definition of the CGroundingAction based class. 
//                       This class defines the basic functionality of a
//                       grounding action
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
//   [2003-10-03] (dbohus): added support for registering the dialog agency 
//                           that a grounding action might use
//   [2003-02-12] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __GROUNDINGACTION_H__
#define __GROUNDINGACTION_H__

#include "Utils/Utils.h"
  
//-----------------------------------------------------------------------------
// CGroundingAction base class
//-----------------------------------------------------------------------------

class CGroundingAction {

protected:
	//---------------------------------------------------------------------
	// Proctected members
	//---------------------------------------------------------------------
	
	// storage for the configuration parameters
	STRING2STRING s2sConfiguration;

public:
	//---------------------------------------------------------------------
	// Constructors and destructors
	//---------------------------------------------------------------------
	// 
    CGroundingAction() {};
    CGroundingAction(string sNewConfiguration);
    virtual ~CGroundingAction() {};

	//---------------------------------------------------------------------
	// Fundamental grounding action methods. These are to be overwritten 
	// by derived grounding action classes
	//---------------------------------------------------------------------

    // Abstract virtual function to obtain the name of the particular 
    // grounding action
    // 
    virtual string GetName() = 0;

	// Sets the configuration string
	//
	void SetConfiguration(string sNewConfiguration);

	// Gets the configuration string
	//
	string GetConfiguration();

    // Abstract virtual function which implements the actual grounding 
    // action (runs the action). The void* passed contains a structure 
    // with parameters for the action
    //
    virtual void Run(void *pParams = NULL) = 0;

    // Abstract virtual function which registers the actual dialog agents/
    // agencies that are used by this action
    //
    virtual void RegisterDialogAgency() = 0;

};

#endif // __GROUNDINGACTION_H__