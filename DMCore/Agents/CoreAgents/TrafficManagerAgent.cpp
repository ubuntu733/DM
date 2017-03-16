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
// TRAFFICMANAGERAGENT.H - implementation of the CTrafficManagerAgent class. 
//						   This core agent forwards calls to other galaxy / OAA 
//						   modules lying outside the dialog manager, and gets
//						   the results back
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
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2003-03-13] (antoine): modified CTrafficManagerAgent::Call so that it 
//                            only sends the value of the mode of confidence 
//                            distribution of each concept (instead of 
//                            value1;value2;value3...)
//   [2002-12-09] (dbohus):  updated to log bindings on returns from a galaxy
//                            call
//   [2002-12-09] (dbohus):  added support for sending merged history concepts
//                            i.e. name<@concept
//   [2002-06-10] (dbohus):  changed name to traffic manager agent, started 
//							 work on accomodating OAA calls
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-01-02] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

//#include "DMInterfaces/DMInterface.h"
#include "TrafficManagerAgent.h"
#include "DMCore/Agents/Registry.h"
#include "DMCore/Core.h"

#include <sstream>

//-----------------------------------------------------------------------------
//
// Constructors and Destructors
//
//-----------------------------------------------------------------------------

// D: constructor
CTrafficManagerAgent::CTrafficManagerAgent(string sAName, 
										   string sAConfiguration,
										   string sAType):
	CAgent(sAName, sAConfiguration, sAType) {
}

// D: destructor - does nothing
CTrafficManagerAgent::~CTrafficManagerAgent() {
}

//-----------------------------------------------------------------------------
// Static function for dynamic agent creation
//-----------------------------------------------------------------------------
CAgent* CTrafficManagerAgent::AgentFactory(string sAName, 
										   string sAConfiguration) {
	return new CTrafficManagerAgent(sAName, sAConfiguration);
}

//-----------------------------------------------------------------------------
// Traffic Manager agent specific PRIVATE methods
//-----------------------------------------------------------------------------

//#ifdef GALAXY
//// D: implements a call to a galaxy server from a TExternalCallSpec structure
//void CTrafficManagerAgent::galaxyCall(TExternalCallSpec ecsCall) {
	//// construct a TGIGalaxyCall structure from the given specification 
	//TGIGalaxyCall gcGalaxyCall;

	//// set the module.function and non-blocking
	//gcGalaxyCall.sModuleFunction = ecsCall.sModuleFunction;
	//gcGalaxyCall.bBlockingCall = ecsCall.bBlockingCall;

	//// construct the outgoing frame
	//string sCallFrame = "{\n";
	//for(unsigned int i=0; i < ecsCall.vToParams.size(); i++) {
		//sCallFrame += ecsCall.vToParams[i].sSlotName + " " + 
						//ecsCall.vToParams[i].sSlotValue + "\n";
	//}
	//sCallFrame += "}\n";
    //// (tk) fixup frame: Galaxy requires that all frames have names
    //ostringstream fixed;
    //istringstream inotfixed(sCallFrame);
    //string token;
    //while(inotfixed >> token) {
        //if(token == "{") {
            //fixed << "{c inframe \n";
        //} else if(token == "}") {
            //fixed << "} \n";
        //} else {
            //fixed << token << ' ';
            //char cpVal[255];
            //inotfixed.ignore();
            //inotfixed.getline(cpVal, 254);
            //if(cpVal[0] == '{') fixed << "{c inframe \n";
            //else fixed << '"' << cpVal << "\" \n";
        //}
    //};
	//gcGalaxyCall.s2sInputs.insert(STRING2STRING::value_type(":inframe", 
															//fixed.str().c_str()));

	//// and the results frame
	//string sResultsFrame;
	//gcGalaxyCall.s2sOutputs.insert(STRING2STRING::value_type(":outframe", 
															 //sResultsFrame));
	
    //// Now, do the actual call (but log it first)
	//Log(TRAFFICMANAGER_STREAM, "Calling galaxy server %s.", 
		//gcGalaxyCall.sModuleFunction.c_str());
	//Log(TRAFFICMANAGERDUMP_STREAM, "Calling galaxy server %s. "\
		//"Call frame dumped below.\n%s",
		//gcGalaxyCall.sModuleFunction.c_str(), sCallFrame.c_str());

	//// retrieve the current thread id
    //DWORD dwThreadId = GetCurrentThreadId();

    //// send the message to the DMInterfaceThread
    //PostThreadMessage(g_idDMInterfaceThread, WM_GALAXYCALL,
                      //(WPARAM)&gcGalaxyCall, dwThreadId);	
	
	//// and wait for a reply
	//MSG Message;
	//GetMessage(&Message, NULL, WM_ACTIONFINISHED, WM_ACTIONFINISHED);
	//Log(TRAFFICMANAGER_STREAM, "Galaxy server call complete.");

    //// now, if the call was non-blocking, simply return (we don't need
    //// to process any results)
    //if(!gcGalaxyCall.bBlockingCall) return;

	//// get the results
	//sResultsFrame = gcGalaxyCall.s2sOutputs[":outframe"];

	//// and log them
	//Log(TRAFFICMANAGERDUMP_STREAM, "Galaxy server call results dumped below.\n%s",
		//sResultsFrame.c_str());

	////---------------------------------------------------------------------
    //// parse the results coming back from the hub and fill in the concepts 
	////---------------------------------------------------------------------
	//string sWorkingString = Trim(sResultsFrame);

	//// the first character has to be "{"
	//if(sWorkingString[0] != '{') {
		//// fail the whole process
		//Error(FormatString("Cannot parse result from %s. Does not start with {\n", 
			//gcGalaxyCall.sModuleFunction.c_str()));
		//return;	
	//}

	//// advance over the "{"
	//sWorkingString = TrimLeft(sWorkingString.substr(1, sWorkingString.length()-1));

	//// now go through all the lines and convert the items 
	//string sLine;
	//while(((sLine = Trim(ExtractFirstLine(sWorkingString))) != "}") && 
			//(sLine != "")) {

		//// split at the equals sign
		//string sItem, sValue;
		//if(SplitOnFirst(sLine, CONCEPT_EQUALS_SIGN, sItem, sValue)) {
			//// if we successfully made the split
			//// check if sValue starts with {. 
			//if((sValue[0] == '{') || (sValue[0] == ':')) {
				//// in this case, we are dealing with a nested structure (or an 
				//// array), so identify where it ends, and correct the sValue;

				//// basically go forward counting the matching {}s and
				//// terminate when the matching one is found
				//sWorkingString = TrimLeft(sValue + sWorkingString);
				//unsigned int iOpenBraces = 1;
				//unsigned int iCount = sWorkingString.find('{') + 1;
				//while((iOpenBraces > 0) && (iCount < sWorkingString.length())) {
					//if(sWorkingString[iCount] == '{') 
						//iOpenBraces++;
					//else if(sWorkingString[iCount] == '}')
						//iOpenBraces--;
					//iCount++;
				//}

				//// if we ran out of the string, signal an error
				//if(iCount >= sWorkingString.length()) {
					//Error(FormatString("Cannot parse result from %s: unmatched"\
						//" closing bracket }.", gcGalaxyCall.sModuleFunction.c_str()));
					//return;
				//}

				//// set the value to the enclosed string
				//sValue = sWorkingString.substr(0, iCount);
				//// and the working string to whatever was left
				//sWorkingString = sWorkingString.substr(iCount + 1, 
											//sWorkingString.length() - iCount - 1);
			//}

			//// now look for that item in the list of output parameters
			//for(unsigned int i=0; i < ecsCall.vFromParams.size(); i++) {
				//if(ecsCall.vFromParams[i].sSlotName == sItem) {
					//// if found, set the value for that concept accordingly
                    //Log(TRAFFICMANAGERDUMP_STREAM, "Setting value: [%s] -> %s.",
                        //sItem.c_str(),
                        //ecsCall.vFromParams[i].pConcept->GetAgentQualifiedName().c_str());
					//ecsCall.vFromParams[i].pConcept->Update(CU_ASSIGN_FROM_STRING, &sValue);
					//break;
				//}
			//}

		//} else {
			//// if no equals sign (split unsuccessful), fail the whole process
			//Error(FormatString("Cannot parse result from %s.",
				//gcGalaxyCall.sModuleFunction.c_str()));
			//return;	
		//} 
	//}

	//// at this point, we should get out of the loop 
	//if(sLine != "}") {
		//// fail the whole process
		//Error(FormatString("Cannot parse result from %s: unmatched closing "\
			//"bracket }.", gcGalaxyCall.sModuleFunction.c_str()));
			//return;	
	//}
//}
//#endif // GALAXY

//#ifdef OAA
//// D: implements a call to an oaa agent from a TExternalCallSpec structure
//void CTrafficManagerAgent::oaaCall(TExternalCallSpec ecsCall) {
	//// *** ACTUALLY NOT IMPLEMENTED YET/ NOT USED YET
//}
/*#endif OAA*/

//-----------------------------------------------------------------------------
// Traffic Manager agent specific PUBLIC methods
//-----------------------------------------------------------------------------
// D: implements a call to an outside server from a string
void CTrafficManagerAgent::Call(CDialogAgent* pCallerAgent, string sCall) {
	
	Log(TRAFFICMANAGER_STREAM, "Preparing external server call...");

	string sRest = Trim(sCall);	// used to hold the rest of the string 
											// while processing it

	// construct a TExternalCallSpec structure which describes the call
	TExternalCallSpec ecsCall;

	// by default, calls are blocking
	ecsCall.bBlockingCall = true;

	// first, obtain the ModuleFunction reference
	SplitOnFirst(sRest, " ", ecsCall.sModuleFunction, sRest);

    // check that the sModuleFunction is in the module.function format
    string sModule = "";
    string sFunction = "";
    SplitOnFirst(ecsCall.sModuleFunction, ".", sModule, sFunction);
    if((sModule == "") || (sFunction == ""))
        FatalError(
            FormatString("Invalid syntax in call (dumped below): cannot find "\
                         "module.function specifier.\n%s", sCall.c_str()));
            

	// then, parse the arguments, and build the call description structure
	string sSlot;
	while(sRest != "") {
        // split on spaces, but paying attention to quoted values
		SplitOnFirst(sRest, " ", sSlot, sRest, '"');
		sRest = TrimLeft(sRest);

		// we deal with strings in one of these 5 forms:
		//   :flags (i.e. :blocking or :non-blocking). By default, everything
		//           is blocking)
		//   name=value
		//   name<concept_path or name<@concept_path
		//   name>concept_path
		//   concept_path or @concept_path
		string sLeftSide, sRightSide;

		if(SplitOnFirst(sSlot, "=", sLeftSide, sRightSide)) {
			// name = value			
			// on the left we have the slot name, on the right a concept path
			TSlotValuePair scpPair;
			scpPair.sSlotName = Trim(sLeftSide);
			scpPair.sSlotValue = Trim(sRightSide, " \n\t\"");
			ecsCall.vToParams.push_back(scpPair);

		} else if(SplitOnFirst(sSlot, ">", sLeftSide, sRightSide)) {

			// name > concept_path
			// on the left we have the slot name, on the right a concept path
			TSlotConceptPair scpPair;
			scpPair.sSlotName = Trim(sLeftSide);
			scpPair.pConcept = &(pCallerAgent->C(Trim(sRightSide)));
			if(scpPair.sSlotName == "") {
				scpPair.sSlotName = scpPair.pConcept->GetName();
			}
			// add this to the structure describing the output parameters
			ecsCall.vFromParams.push_back(scpPair);

		} else if(SplitOnFirst(sSlot, "<", sLeftSide, sRightSide)) {

			// name < concept_path
			TSlotValuePair svpPair;
			svpPair.sSlotName = Trim(sLeftSide);

            // identify the concept
            sRightSide = Trim(sRightSide);
            CConcept* pConcept;
            bool bFreeConceptAtTheEnd = false;
            if(sRightSide[0] == '@') {
                // then we want a history merged version of the concept               
                sRightSide = sRightSide.substr(1, sRightSide.length() - 1);
                pConcept = pCallerAgent->C(sRightSide).CreateMergedHistoryConcept();
                // need to free the concept at the end since CreateMergedHistoryConcept 
                // creates one on the spot
                bFreeConceptAtTheEnd = true;
            } else {
                pConcept = &(pCallerAgent->C(sRightSide));
            }

			svpPair.sSlotValue = pConcept->GroundedHypToString();
			if(svpPair.sSlotName == "") 
				svpPair.sSlotName = pCallerAgent->C(sRightSide).GetName();
			
			// add this to the structure describing the input parameters
			ecsCall.vToParams.push_back(svpPair);

            // free the concept created by CreateMergedHistoryConcept, if it's the case
            if(bFreeConceptAtTheEnd)
                delete pConcept;
		
		} else if(ToLowerCase(sSlot) == ":blocking") {

			// a blocking call flag
			ecsCall.bBlockingCall = true;
		
		} else if(ToLowerCase(sSlot) == ":non-blocking") {

			// a non-blocking call flag
			ecsCall.bBlockingCall = false;

		}
	}

	// at this point the ecsCall structure is filled, so just 
	// call the helper Galaxy or OAA ServerCall function
 /* #ifdef GALAXY*/
	//galaxyCall(ecsCall);
	//#endif	// GALAXY

	//#ifdef OAA
	//oaaCall(ecsCall);
	/*#endif  // OAA*/
}
