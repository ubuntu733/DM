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
// FRAMEOUTPUT.H - definition of the CFrameOutput class. This class is derived 
//				   off COutput and encapsulates a Rosetta-type output for the 
//				   dialog manager.
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
//   [2005-01-11] (antoine): changed ToString so that it includes a slot giving
//							  the number of times the output has been repeated
//   [2004-02-24] (dbohus):  changed outputs so that we no longer clone 
//                            concepts but use them directly
//   [2003-10-08] (dbohus):  fixed the prompt parser so that it can accept 
//                            values in quotes
//   [2003-03-13] (antoine): modified CFrameOutput::Create so that it uses only
//                            the value of the mode of the confidence distr.
//                            of each concept (instead of the list of all 
//                            values)
//   [2003-03-13] (dbohus,
//                 antoine): fixed concept cloning in FrameOutput::Create
//                            (set concept name)
//   [2003-01-15] (dbohus):  added support for sending merged history concepts
//                            i.e. name<@concept
//   [2002-10-10] (dbohus):  rewritten Create function so that it's cleaner
//   [2002-06-25] (dbohus):  unified Create and CreateWithClones in a single
//							  create function which takes a bool parameter
//   [2002-06-17] (dbohus):  deemed preliminary stable version 0.5
//	 [2002-06-14] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __FRAMEOUTPUT_H__
#define __FRAMEOUTPUT_H__

// D: Check that the compiler configuration is correct (the Frame-type outputs
//    are used only in a Galaxy configuration so far)
/*#ifndef GALAXY*/
//#error FrameOutput.h should be compiled only in the galaxy version
//#endif

#include "Output.h"

//-----------------------------------------------------------------------------
// D: The CFrameOutput class.
//-----------------------------------------------------------------------------

class CFrameOutput: public COutput {

protected:
	// protected members for holding the frame output information
	//
	// the slot names
	vector<string> vsSlotNames;

	// the slot values (empty whenever the slot is bound to a concept)
	vector<string> vsValues;
	
public:
	//---------------------------------------------------------------------
	// Constructors and destructors
	//---------------------------------------------------------------------
	// 
	CFrameOutput();
	virtual ~CFrameOutput();

public:
	//---------------------------------------------------------------------
	// Public abstract methods 
	//---------------------------------------------------------------------

	// Overwritten virtual method which creates a certain frame output 
	// from a given string-represented prompt 
	virtual bool Create(string sAGeneratorAgentName, int iAExecutionIndex,
						string sAOutput, TFloorStatus fsAFloor, int iAOutputId);

    // Overwritten virtual method which clones an output
	virtual COutput* Clone(int iNewOutputId);

	// Overwritten virtual method which generates a string representation 
	// for the output that will be sent to the external output component 
	// (in this case a Rosetta-like natural language generator)
	virtual string ToString();
	
};

#endif // __FRAMEOUTPUT_H__
