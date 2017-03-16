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
// GROUNDINGMODEL.CPP - implementation of the CGroundingModel base class for
//                      grounding models
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
//   [2006-01-31] (dbohus): added support for dynamically registering grounding
//                          model types
//   [2004-02-24] (dbohus): addeded support for full state and collapsed state
//   [2004-02-09] (dbohus): changed model data to "policy"
//   [2003-09-24] (dbohus): fixed bug in running action (index was not mapped)
//   [2003-02-12] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------


#include "GroundingModel.h"
#include "DMCore/Core.h"

//-----------------------------------------------------------------------------
//
// D: CExternalPolicyInterface class
//
//-----------------------------------------------------------------------------

// D: default constructor
#pragma warning (disable:4706)
CExternalPolicyInterface::CExternalPolicyInterface(string sAHost) : sSocket(1){
    //全部注释
    /*// log the action*/
    //Log(GROUNDINGMANAGER_STREAM, "Creating external grounding policy interface",
        //" to %s ...", sAHost.c_str());

    //Log(GROUNDINGMANAGER_STREAM, "Opening socket connection to %s ...", 
        //sAHost.c_str());

    //// get the host name and the port
    //string sHostName;
    //int iPort;
    //SplitOnFirst(sAHost, ":", sHostName, sAHost);
    //iPort = atoi(sAHost.c_str());

    //// initialize the sockets library
    //WORD wVersion = MAKEWORD(2,0);
    //WSADATA wsaData;

    //if (WSAStartup (wVersion, &wsaData) != 0)
        //FatalError("Error (on WSAStartup) in creating the external policy interface");

    //// get host information
    //struct hostent *phHost;
    //if (!(phHost = gethostbyname(sHostName.c_str()))) {
        //WSACleanup();
			//FatalError("Error in gethostbyname.");
    //}
    
    //// open a TCP socket
    //int iTries;
    //for(iTries = 0; iTries < 5; iTries++) {
			//if ((sSocket = (unsigned long int) 
                //socket (AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            //WSACleanup();
            //FatalError("Error creating socket");
        //}
      
			//// connect to the server
        //struct sockaddr_in saAddress;
			//memset ((char *) &saAddress, sizeof(saAddress), 0);
			//saAddress.sin_family = AF_INET;
			//memcpy (&saAddress.sin_addr, phHost->h_addr, phHost->h_length);
			//saAddress.sin_port = htons((u_short) iPort);	
        //if (connect ((SOCKET)sSocket, (const struct sockaddr *)&saAddress, sizeof(saAddress)) == 0)
					//break;        
			//closesocket((SOCKET)sSocket);

			//Warning(FormatString(
            //"Could not connect to external grounding policy provider at %s:%d. "
            //"Reattempting in 1 second.", 
            //sHostName.c_str(), iPort));
	
      //Sleep(200);
    //}

    //// if we exhausted the 5 tries, give a fatal error
    //if(iTries == 5) {
        //FatalError(FormatString(
            //"Error connecting to external grounding policy provider at %s:%d",
            //sHostName.c_str(), iPort));
    //}
    
    //// o/w if everything is okay

    //// set socket to unbuffered mode (ie, execute sends immediately)
    //unsigned long int iFlag = 1;
    //if (setsockopt((SOCKET)sSocket, IPPROTO_TCP, TCP_NODELAY, 
        //(char *)(&iFlag), sizeof(iFlag)) == SOCKET_ERROR) {
        //closesocket((SOCKET)sSocket);
        //FatalError("Error in setsockopt.");
    //}
    
    //// set socket to non-blocking mode
    //iFlag = 1;
    //if (ioctlsocket((SOCKET)sSocket, FIONBIO, &iFlag) == SOCKET_ERROR) {
        //closesocket((SOCKET)sSocket);
        //FatalError("Error in ioctlsocket.");
    //}

    //// log the success
    //Log(GROUNDINGMANAGER_STREAM, "Socket connection to %s:%d established ",
        //"successfully.", sHostName.c_str(), iPort);
    //Log(GROUNDINGMANAGER_STREAM, 
        //"External grounding policy creation completed.");

}
#pragma warning (default:4706)

// D: destructor
CExternalPolicyInterface::~CExternalPolicyInterface() {
    // now close the socket
    //closesocket((SOCKET)sSocket);
}

// D: compute the suggested action index
#pragma warning (disable:4127)
int CExternalPolicyInterface::ComputeSuggestedActionIndex(CState& rState) {
    return -1;
    // log the action
   /* Log(GROUNDINGMANAGER_STREAM, "Polling external grounding policy ...");*/

    //// construct the buffer with the state information to be send
    //string sStateAsString = rState.ToString();
    //char *lpszBuffer = (char *)malloc(sizeof(char)*(sStateAsString.length()+1));
    //strcpy(lpszBuffer, sStateAsString.c_str());
    //char *lpszBufPointer = lpszBuffer;

    //// and send it over the socket
    //unsigned long int uilRemaining = strlen(lpszBufPointer) + 1;
    //unsigned long int uilSent = 0;
    //while(uilRemaining > 0) {
        //if ((uilSent = send (sSocket, lpszBufPointer, uilRemaining, 0)) == 
            //SOCKET_ERROR) {
            //FatalError("Error sending over socket.");
        //} 
        //uilRemaining -= uilSent;
        //lpszBufPointer += uilSent;
    //}
    //// at the end, free the buffer
    //free(lpszBuffer);

    //// now listen for the action, in blocking mode (use another buffer for
    //// receiving)
    //lpszBuffer = (char *)malloc(sizeof(char)*256);
    //fd_set fdsRead;
    
    //FD_ZERO(&fdsRead);
    //FD_SET(sSocket, &fdsRead);
    //if(select(sSocket + 1, &fdsRead, NULL, NULL, NULL) == SOCKET_ERROR) {
			//FatalError("Error on select.");
    //}
    //if (recv(sSocket, lpszBuffer, STRING_MAX, 0) == SOCKET_ERROR) {
        //FatalError("Error on recv.");
    //}

   
    //// log the completion of the action
    //Log(GROUNDINGMANAGER_STREAM, FormatString(
        //"External grounding policy poll complete. Suggested action: %s.", 
        //lpszBuffer));

    //// find out the suggested action index
    //int iSuggestedActionIndex = 
        //pGroundingManager->GroundingActionNameToIndex(string(lpszBuffer));

    //// now free that buffer too
    //free(lpszBuffer);

    //// and return the suggested action index
    /*return iSuggestedActionIndex;*/
}
#pragma warning (default:4127)

//-----------------------------------------------------------------------------
//
// D: CGroundingModel class
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// D: Constructors and Destructors
//-----------------------------------------------------------------------------
// D: default constructor
CGroundingModel::CGroundingModel(string sAModelPolicy, string sAName) {
    sModelPolicy = sAModelPolicy;
    sName = sAName;
    bExternalPolicy = false;
    sExternalPolicyHost = "localhost:0";
    pepiExternalPolicy = NULL;
    sExplorationMode = "epsilon-greedy";
    fExplorationParameter = (float)0.2;
    iSuggestedActionIndex = -1;    
}

// D: Constructor from reference
CGroundingModel::CGroundingModel(CGroundingModel& rAGroundingModel) {
    sModelPolicy = rAGroundingModel.sModelPolicy;
    sName = rAGroundingModel.sName;
    bExternalPolicy = rAGroundingModel.bExternalPolicy;
    sExternalPolicyHost = rAGroundingModel.sExternalPolicyHost;
    pepiExternalPolicy = rAGroundingModel.pepiExternalPolicy;
    pPolicy = rAGroundingModel.pPolicy;
    viActionMappings = rAGroundingModel.viActionMappings;
    sExplorationMode = rAGroundingModel.sExplorationMode;
    fExplorationParameter = rAGroundingModel.fExplorationParameter;
    bdBeliefState = rAGroundingModel.bdBeliefState;
    stFullState = rAGroundingModel.stFullState;
    iSuggestedActionIndex = rAGroundingModel.iSuggestedActionIndex;
}

// D: virtual destructor - does nothing
CGroundingModel::~CGroundingModel() {
}

//-----------------------------------------------------------------------------
// D: Grounding model factory method
//-----------------------------------------------------------------------------

CGroundingModel* CGroundingModel::GroundingModelFactory(string sModelPolicy) {
    FatalError("Could not create abstract grounding model class.");
    return NULL;
}

//-----------------------------------------------------------------------------
// D: Member access methods
//-----------------------------------------------------------------------------

// D: return the type of the grounding model
string CGroundingModel::GetType() {
    return "generic";
}

// D: return the string description of the grounding model
string CGroundingModel::GetModelPolicy() {
    return sModelPolicy;
}

// D: return the name of the grounding model
string CGroundingModel::GetName() {
    return sName;
}

// D: sets the name of the grounding model
void CGroundingModel::SetName(string sAName) {
    sName = sAName;
}

//-----------------------------------------------------------------------------
// D: Grounding model specific public methods
//-----------------------------------------------------------------------------

// D: Initializes the model
void CGroundingModel::Initialize() {
    if(!LoadPolicy())
        FatalError(FormatString("Invalid policy for grounding model %s.", 
            sModelPolicy.c_str()));
}

// D: Loads the model policy (from the grounding manager agent)
bool CGroundingModel::LoadPolicy() {
    // get the string data
    string sData = pGroundingManager->GetPolicy(sModelPolicy);

    // parse it - first break it into lines
    TStringVector vsLines = PartitionString(sData, "\n");
    bool bExplorationModeLine = true;
    bool bExplorationParameterLine = false;
    bool bActionsLine = false;
    bool bExternalPolicyHostLine = true;
    unsigned int i = 0;
    while(i < vsLines.size()) {
        // check that it's not a commentary or an empty line
        if(vsLines.empty() || (vsLines[i][0] == '#') || 
            (vsLines[i][0] == '/') || (vsLines[i][0] == '%')) {
            i++;
            continue;
        }

        // o/w, if we expect the exploration mode line
        if(bExplorationModeLine || bExternalPolicyHostLine) {
            // read a line in the format EXPLORATION_MODE=...
            // or external_policy_host=...
            string sTemp1, sTemp2;
            SplitOnFirst(vsLines[i], "=", sTemp1, sTemp2);
            if(ToLowerCase(Trim(sTemp1)) == "exploration_mode") {
                // set the external policy flag to false
                bExternalPolicy = false;
                // set the exploration mode
                sExplorationMode = ToLowerCase(Trim(sTemp2));
                // and next, set the expectation for the exploration parameter line
                bExplorationModeLine = false;
                bExplorationParameterLine = true;
                bExternalPolicyHostLine = false;
            } else if(ToLowerCase(Trim(sTemp1)) == "external_policy_host") {
                // set the external policy flag to true
                bExternalPolicy = true;
                // set the host
                sExternalPolicyHost = ToLowerCase(Trim(sTemp2));
                bExplorationModeLine = false;
                bExplorationParameterLine = false;
                bExternalPolicyHostLine = false;
                bActionsLine = true;
            } else return false;
        } else if(bExplorationParameterLine) {
            // read a line in the format EXPLORATION_PARAMETER=...
            string sTemp1, sTemp2;
            SplitOnFirst(vsLines[i], "=", sTemp1, sTemp2);
            if(ToLowerCase(Trim(sTemp1)) != "exploration_parameter")
                return false;
            // set the exploration parameter
            fExplorationParameter = (float)atof(Trim(sTemp2).c_str());
            // and next, set the expectation for the exploration parameter line
            bExplorationParameterLine = false;
            bActionsLine = true;
        } else if(bActionsLine) {
            // obtain the list of actions
            TStringVector vsActions = PartitionString(vsLines[i], " \t");
            // check that there are some actions in the model
            if(vsActions.size() < 1) return false;
            // construct the action index vector
			for(unsigned int a = 0; a < vsActions.size(); a++)
                viActionMappings.push_back(
                    pGroundingManager->GroundingActionNameToIndex(vsActions[a]));
            // set actions line to false
            bActionsLine = false;        
        } else if(!bExternalPolicy) {
            // if it's not the first line, then it will be a line containing a 
            // state and the values
            TStringVector vsValues = PartitionString(vsLines[i], " \t");
            // check that there's enough values
            if(vsValues.size() != viActionMappings.size() + 1) 
                return false;
            
            // construct the state-action-utility datastructure
            TStateActionsValues savData;
            savData.sStateName = vsValues[0];
            for(unsigned int a = 1; a < vsValues.size(); a++) {
				if(vsValues[a] == "-") {
                    // check if we have an unavailable action
                    savData.i2fActionsValues[a-1] = 
                        (float)UNAVAILABLE_ACTION;
                } else {
                    // o/w get the utility
                    savData.i2fActionsValues[a-1] = 
                        (float)atof(vsValues[a].c_str());
                }
            }

            // push it in the policy
            pPolicy.push_back(savData);
 			
       }
        i++;
    }

    // resize the bdActionValues vector accordingly
	bdActionValues.Resize(viActionMappings.size());
	
    // now if we have an external policy interface, create it
    if(bExternalPolicy)
        pepiExternalPolicy = 
            pGroundingManager->CreateExternalPolicyInterface(sExternalPolicyHost);

    return true;
}

// D: Compute the state of the model
void CGroundingModel::ComputeState() {
    // first compute the full state
    computeFullState();
    // then based on that, derive the belief state
    computeBeliefState();
    // and invalidate the suggested action (not computed yet for this state)
    iSuggestedActionIndex = -1;
}

// D: Computes the expected values of the various actions and returns a 
//    corresponding probability distribution over the actions
void CGroundingModel::ComputeActionValuesDistribution() {

    // for each action
    for(unsigned int a = 0; a < viActionMappings.size(); a++) {

        // compute its expected utility by summing over the states
        bdActionValues[a] = 0;
        bool bUnavailableAction = true;
        for(unsigned int s = 0; s < pPolicy.size(); s++) {
            float fStateActionValue = pPolicy[s].i2fActionsValues[a];
            // if the action is available from that state, 
            if(fStateActionValue != UNAVAILABLE_ACTION) {
                if(bdBeliefState[s] != 0) {
                    // update the utility
                    bdActionValues[a] += 
                        fStateActionValue*bdBeliefState[s];
                    bUnavailableAction = false;
                } 
            }
        }
        if(bUnavailableAction)
            bdActionValues[a] = UNAVAILABLE_ACTION;
    }
}

// D: Compute the suggested action index - be default, the action that 
//    maximizes the expected utility
int CGroundingModel::ComputeSuggestedActionIndex() {

    // first check if we already computed a suggested action index
    if(iSuggestedActionIndex != -1)
        return iSuggestedActionIndex;

    // now check if we need to use an external policy
    if(bExternalPolicy) {
        iSuggestedActionIndex = 
            pepiExternalPolicy->ComputeSuggestedActionIndex(stFullState);
        return iSuggestedActionIndex;
    }

    // compute the values for actions from this state
    ComputeActionValuesDistribution();

    // now, depending on the type of exploration mode
    if(sExplorationMode == "greedy") {
        // if greedy, choose the optimal action
        iSuggestedActionIndex = bdActionValues.GetModeEvent();
    } else if(sExplorationMode == "stochastic") {
        // if stochastic, just choose an action randomly from the value distr
        iSuggestedActionIndex = bdActionValues.GetRandomlyDrawnEvent();
    } else if(sExplorationMode == "epsilon-greedy") {
        // if epsilon-greedy, 
        iSuggestedActionIndex = 
            bdActionValues.GetEpsilonGreedyEvent(fExplorationParameter);
    } else if(sExplorationMode == "soft-max") {
        // if soft-max
        iSuggestedActionIndex =
            bdActionValues.GetSoftMaxEvent(fExplorationParameter);
    } else {
        // o/w there's an error
        FatalError(FormatString("Unknown exploration mode: %s.", 
            sExplorationMode.c_str()));
    }

    // apply the mapping
    iSuggestedActionIndex = viActionMappings[iSuggestedActionIndex];

    // and return the value
    return iSuggestedActionIndex;
}

// D: Run the grounding model 
void CGroundingModel::Run() {
    // compute the action index
    ComputeSuggestedActionIndex();
    // run that action
    RunAction(iSuggestedActionIndex);
}

// D: Run a particular action 
void CGroundingModel::RunAction(int iActionIndex) {
    // obtains a pointer to the action from the grounding manager
    // and runs it with no parameters
    pGroundingManager->operator [](iActionIndex)->Run();
}

// D: Log the state and the suggested action of the model
void CGroundingModel::LogStateAction() {
    // log these computations
    Log(GROUNDINGMODELX_STREAM, 
        "Grounding model %s [TYPE=%s;POLICY=%s;EM=%s;EP=%.2f]:\n"
        "Full state: \n%sBelief state: %s\nAction values (dumped below):\n%s\n"
        "Suggested action: %s\n",
        GetName().c_str(), GetType().c_str(), GetModelPolicy().c_str(), 
        sExplorationMode.c_str(), fExplorationParameter, 
        stFullState.ToString().c_str(), beliefStateToString().c_str(),
        actionValuesToString().c_str(), 
        pGroundingManager->
        GroundingActionIndexToName(iSuggestedActionIndex).c_str());
}

//-----------------------------------------------------------------------------
// D: Grounding model specific private methods
//-----------------------------------------------------------------------------

// D: Convert the belief state to a string representation
string CGroundingModel::beliefStateToString() {
    string sResult;
    for(unsigned int i = 0; i < pPolicy.size(); i++) {
        sResult += FormatString("%s:%.2f  ", 
            pPolicy[i].sStateName.c_str(), bdBeliefState[i]);
    }
    return sResult;
}

// D: Convert the action values to a string representation
string CGroundingModel::actionValuesToString() {
    string sResult;
    for(unsigned int i = 0; i < viActionMappings.size(); i++) {
        if(bdActionValues[i] == UNAVAILABLE_ACTION) {
            sResult += FormatString("  %s: -\n", 
                pGroundingManager->GroundingActionIndexToName(
                    viActionMappings[i]).c_str());
        } else {
			if(bdActionValues.LowBound(i) == INVALID_EVENT) {
				sResult += FormatString("  %s:%.4f\n", 
					pGroundingManager->GroundingActionIndexToName(
						viActionMappings[i]).c_str(),
					bdActionValues[i]);
			} else {
				sResult += FormatString("  %s:%.4f [%.4f-%.4f]\n", 
					pGroundingManager->GroundingActionIndexToName(
						viActionMappings[i]).c_str(),
					bdActionValues[i], 
					bdActionValues.LowBound(i), 
					bdActionValues.HiBound(i));
			}
        }
    }
    return sResult;
}
