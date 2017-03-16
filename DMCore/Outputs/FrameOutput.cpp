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
// FRAMEOUTPUT.CPP - implementation of the CFrameOutput class. This class is 
//					 derived off COutput and encapsulates a Rosetta-type output 
//					 for the dialog manager.
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
//   [2005-06-02] (antoine): added the possibility to have a "prompt_header" and
//                           "prompt_ending" parameters in the OutputManager's 
//                           configuration, the values of which get appended 
//                           after resp. before and after each prompt, before
//                           they get parsed by the Create method
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

#include "FrameOutput.h"

// D: Check that the compiler configuration is correct (the Frame-type outputs
//    are used only in a Galaxy configuration so far)
/*#ifndef GALAXY*/
//#error FrameOutput.cpp should be compiled only in the galaxy version
//#endif

#include "DMCore/Log.h"
#include "DMCore/Agents/CoreAgents/OutputManagerAgent.h"

// D: a pointer to the actual Output Manager agent
extern COutputManagerAgent* pOutputManager;

//-----------------------------------------------------------------------------
// D: Constructors and Destructor
//-----------------------------------------------------------------------------
// D: Constructor - does nothing so far
CFrameOutput::CFrameOutput() {
}

// D: Virtual destructor - does nothing so far
CFrameOutput::~CFrameOutput() {  
}

//-----------------------------------------------------------------------------
// D: COutput-Specific public methods
//-----------------------------------------------------------------------------
// D: virtual function for cloning an output 
COutput* CFrameOutput::Clone(int iNewOutputId) {
	// create the new output
	CFrameOutput* pfoClone = new CFrameOutput;
	// copy the slot-value information
	pfoClone->vsSlotNames = vsSlotNames;
	pfoClone->vsValues = vsValues;
	// copy the rest of the stuff using the auxiliary COutput::clone() method
	clone(pfoClone, iNewOutputId);
	// finally return the cloned output
	return pfoClone;
}

// D: creates the internal representation starting from a string description
//    of the prompt. Returns false if an output cannot be created
bool CFrameOutput::Create(string sAGeneratorAgentName, 
						  int iAExecutionIndex, 
						  string sAOutput, TFloorStatus fsAFloor,
						  int iAOutputId) {


	// set the prompt id and caller agent name
	iOutputId = iAOutputId;
	sGeneratorAgentName = sAGeneratorAgentName;
	fsFinalFloorStatus = fsAFloor;

	// Set the state index and string representation
	iExecutionIndex= iAExecutionIndex;
	sDialogState = pStateManager->GetStateAsString(pStateManager->operator[](iExecutionIndex));

	// gets a reference to the sending agent
    CDialogAgent *pdaGenerator = 
			(CDialogAgent *)AgentsRegistry[sGeneratorAgentName];
	if(pdaGenerator == NULL) {
		// if we can't locate the generator agent
		Warning(FormatString("Could not get a pointer to generator agent %s "\
								"for frame-output (dump below). Output could not"\
								" be created.\n%s", 
								sGeneratorAgentName.c_str(), 
								sAOutput.c_str()));
		return false;					                 
	}

	// at starters, the output has not yet been conveyed
	cConveyance = cNotConveyed;

	// work on another copy sToParse
	string sToParse = sAOutput;

	// normalize the syntax 
	sToParse = Trim(sToParse, " \n\t}{");

	// check that the prompt is not empty
	if(sToParse == "") return false;

	// (antoine 06/02/05) If there is a systematic prompt header
	// in the OutputManager's configuration, append it to the prompt
	if (pOutputManager->HasParameter("prompt_header")) {
		sToParse = pOutputManager->GetParameterValue("prompt_header") 
			+ " " + sToParse;
	}

	// (antoine 06/02/05) If there is a systematic prompt ending
	// in the OutputManager's configuration, append it to the prompt
	if (pOutputManager->HasParameter("prompt_ending")) {
		sToParse += " " + pOutputManager->GetParameterValue("prompt_ending");
	}

	// and a token to be used to read in the prompt bit by bit
	string sToken;

	// start splitting the string, but skipping over sequences contained
    // within quotes
	SplitOnFirst(sToParse, " ", sToken, sToParse);
	sToParse = TrimLeft(sToParse);

	// at the beginning look for flags
	while(sToken[0] == ':') {
		vsFlags.push_back(sToken);
		SplitOnFirst(sToParse, " ", sToken, sToParse);
		sToParse = TrimLeft(sToParse);
	}

	// next we should have the act
	if(sToken == "") {
		Warning("Cannot create frame-output: missing act (dump below).\n" + 
				sAOutput);
		return false;
	} else {
		// it could be just the act, or something like act = value
		string sLeftSide, sRightSide;
		if(SplitOnFirst(sToken, "=", sLeftSide, sRightSide)) {
			if(sLeftSide == "act") 
				sAct = Trim(sRightSide);
			else {
				Warning("Cannot create frame-output: missing act (dump below).\n" + 
						sAOutput);
				return false;
			}
		} else {
			sAct = Trim(sToken);
		}
	}

	// continue splitting the string
	SplitOnFirst(sToParse, " ", sToken, sToParse);
	sToParse = TrimLeft(sToParse);

	// next we should have the object
	if(sToken == "") {
		Warning("Cannot create frame-output: missing object (dump below).\n" + 
				sAOutput);
		return false;
	} else {
		// it could be just the act, or something like act = value
		string sLeftSide, sRightSide;
		if(SplitOnFirst(sToken, "=", sLeftSide, sRightSide, '"')) {
			if(sLeftSide == "object") 
				sObject = Trim(sRightSide);
			else {
				Warning("Cannot create frame-output: missing object (dump below).\n" + 
						sAOutput);
				return false;
			}
		} else {
			sObject = Trim(sToken);
		}
	}

	//  now, fill in the rest of the frame, 
	while(sToParse != "") {
        // continue splitting, this time taking quotes into account, as some
        // of the values might be quoted strings
		SplitOnFirst(sToParse, " ", sToken, sToParse, '"');
		sToParse = TrimLeft(sToParse);
		
		string sLeftSide, sRightSide;
		if(SplitOnFirst(sToken, "<", sLeftSide, sRightSide, '"')) {
			// check for form slot<concept_name

			// then look for the concept; start by identifying getting a pointer to
			// the generator agent
			string sConceptName = Trim(sRightSide);

			// now identify the concept relative to the generator agent
            CConcept *pConcept;
            if(sConceptName[0] == '@') {
                // then we want a history merged version of the concept               
                sConceptName = sConceptName.substr(1, sConceptName.length() - 1);
                pConcept = pdaGenerator->C(sConceptName).CreateMergedHistoryConcept();
            } else {
                // o/w simply take a pointer to the concept
                pConcept = &(pdaGenerator->C(sConceptName));
            }

			if ( pConcept != &NULLConcept ) {
				vcpConcepts.push_back(pConcept);
                pConcept->SetWaitingConveyance();
                vbNotifyConcept.push_back(true);
			} else {
			    FatalError(FormatString("Could not find concept %s relative to "\
					                 "agent %s for frame-output (dump below).\n%s", 
					                 sConceptName.c_str(), sGeneratorAgentName.c_str(), 
					                 sAOutput.c_str()));
			}

			// and obtain it's value
			vsValues.push_back(pConcept->GroundedHypToString());

			// also set the slot name
			string sSlotName = Trim(sLeftSide);
			if(sSlotName != "") {
				// put the slot name
				vsSlotNames.push_back(Trim(sLeftSide));
			} else {
				// o/w we have only <concept, so we should use the concept name
				vsSlotNames.push_back(pConcept->GetName());
			}

		} else if(SplitOnFirst(sToken, "=", sLeftSide, sRightSide, '"')) {
			// check for form slot = value
			vsSlotNames.push_back(Trim(sLeftSide));
			vcpConcepts.push_back(NULL);
            vbNotifyConcept.push_back(false);
            // trim the quotes also off the value (in case there are any)
			vsValues.push_back(Trim(sRightSide, " \n\t\"") + "\n");
		}

	}

	// set output device (look through the flags for an output device specifier
	sOutputDeviceName = "";
	for ( unsigned int j = 0; j < vsFlags.size(); j++ ) {
		//TK: should stip leading ':' from flag
		string flag = !vsFlags[j].empty() && vsFlags[j][0] == ':'? 
			vsFlags[j].substr(1): vsFlags[j];
		if ( pOutputManager->GetOutputDevice(flag) != NULL ) {
			sOutputDeviceName = flag;
			break;
		}
	}

	// if it was not specified in the modifiers, set the output agent to
	// be the default one
	if ( sOutputDeviceName == "" )
		sOutputDeviceName = pOutputManager->GetDefaultOutputDeviceName();
			
	return true;
}

// D: converts the frame-output into an appropriate string representation 
//    to be sent to a Rossetta-type output external agent
string CFrameOutput::ToString() {

	string sOutputFrame = "start\n{\n";
	
	// generate act type
	sOutputFrame += "act\t" + sAct + "\n";
	
	// generate object 
	sOutputFrame += "object\t" + sObject + "\n";

	// adds default indicators
	sOutputFrame += "_repeat_counter\t" + IntToString(iRepeatCounter) + "\n";

	// no flags are generated at this point
	// and now go through the frame and generate all the rest, 
	for ( unsigned int i = 0; i < vsSlotNames.size(); i++ ) {
		sOutputFrame += vsSlotNames[i];
		sOutputFrame += "\t";
		sOutputFrame += vsValues[i];
	}

	// terminate
	sOutputFrame += "}\nend\n";

	// and return
	return sOutputFrame;
}
