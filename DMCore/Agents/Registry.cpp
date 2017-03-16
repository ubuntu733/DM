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
// REGISTRY.CPP - implementation of the CRegistry class. This class registers 
//                the existing agents and agenttypes by name, and provides
//                various related functionality: CallAgent, (UnRegister), 
//                ancestor/descendant, etc.
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
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2001-12-30] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#include "Registry.h"
#include "DMCore/Log.h"

// D: the AgentsRegistry object
CRegistry AgentsRegistry;

//-----------------------------------------------------------------------------
// Constructors and Destructors
//-----------------------------------------------------------------------------

// D: Constructor
CRegistry::CRegistry() {
	// nothing here
}

// D: Initializes the registry, empties everything
void CRegistry::Clear() {
	Log(REGISTRY_STREAM, "Clearing up the registry ...");

    // go through the agents hash and deallocate the remaining registered
    // agents
    Log(REGISTRY_STREAM, "Deallocating remaining registered agents ...");
    while(!AgentsHash.empty()) {
        // deallocate the agents        
        delete AgentsHash.begin()->second;
    }
    Log(REGISTRY_STREAM, "Deallocating remaining registered agents completed.");

    // clear the hashes
    AgentsHash.clear();
	AgentsTypeHash.clear();
}

//-----------------------------------------------------------------------------
//
// Registry specific functions for registered agent names
//
//-----------------------------------------------------------------------------

// D: return true if the agent is already registered
bool CRegistry::IsRegisteredAgent(string sAgentName) {
	return (AgentsHash.find(sAgentName) != AgentsHash.end());
}

// D: register an agent into the registry.
void CRegistry::RegisterAgent(string sAgentName, CAgent* pAgent)
{
	// check that there's no agent already registered under the same name
	if(IsRegisteredAgent(sAgentName)) {
		FatalError("An agent already registered under the same name (" + 
					sAgentName + ") was found.");
	}

	// register the agent
	AgentsHash.insert(TAgentsHash::value_type(sAgentName, pAgent));

	// and log that
	Log(REGISTRY_STREAM, "Agent %s registered successfully.", sAgentName.c_str());
}

// D: unregister an agent 
void CRegistry::UnRegisterAgent(string sAgentName) {
	if(AgentsHash.erase(sAgentName) == 0) {
		FatalError("Could not find agent " + sAgentName + " to unregister.");
	}

	// and log that
	Log(REGISTRY_STREAM, "Agent %s unregistered successfully.", 
						 sAgentName.c_str());
}

// D: return a pointer to an agent, given the agent's name. Returns NULL if
//    the agent is not found
CAgent* CRegistry::operator [](string sAgentName) {
	TAgentsHash::iterator iPtr;
	if((iPtr = AgentsHash.find(sAgentName)) == AgentsHash.end()) {
		// if the agent is not found, return NULL
		return NULL;
	} else {
		// otherwise, return the pointer to the agent
		return iPtr->second;
	}
}

//-----------------------------------------------------------------------------
//
// Registry specific functions for registered agent types
//
//-----------------------------------------------------------------------------

// D: return true if the agent type is already registered
bool CRegistry::IsRegisteredAgentType(string sAgentTypeName) {
	return (AgentsTypeHash.find(sAgentTypeName) != AgentsTypeHash.end());
}

// D: register an agent type into the registry.
void CRegistry::RegisterAgentType(string sAgentTypeName, 
								  FCreateAgent fctCreateAgent)
{
	// check that there's no agent already registered under the same name
	if(IsRegisteredAgentType(sAgentTypeName)) {
		FatalError("An agent type already registered under the same name (" + 
					sAgentTypeName + ") was found.");
	}

	// register the agent type
	AgentsTypeHash.insert(TAgentsTypeHash::value_type(sAgentTypeName, 
			 										  fctCreateAgent));

	// and log that
	Log(REGISTRY_STREAM, "Agent type %s registered successfully.", 
		sAgentTypeName.c_str());
}

// D: unregister an agent type 
void CRegistry::UnRegisterAgentType(string sAgentTypeName) {
	if(AgentsTypeHash.erase(sAgentTypeName) == 0) {
		FatalError("Could not find agent type" + sAgentTypeName + 
				   " to unregister.");
	}

	// and log that
	Log(REGISTRY_STREAM, "Agent type %s unregistered successfully.", 
						 sAgentTypeName.c_str());
}

// D: create a new agent from a given agent type
CAgent* CRegistry::CreateAgent(string sAgentTypeName, string sAgentName, string sAgentConfiguration) {
	
	TAgentsTypeHash::iterator iPtr;
	
	if((iPtr = AgentsTypeHash.find(sAgentTypeName)) == AgentsTypeHash.end()) {
		// if the agent type is not in the registry, we're in bad shape
		FatalError("Could not create agent of type " + sAgentTypeName + 
				   ". Type not found in the registry.");
		return NULL;
	} else {
		// o/w, if we found the agent, call the create function
		CAgent* pNewAgent = (*(iPtr->second))(sAgentName, sAgentConfiguration);
		if(pNewAgent) {
			// call the create function for the agent
			pNewAgent->Create();
			Log(REGISTRY_STREAM, 
				"Agent %s created successfully.",
				sAgentName.c_str());
			// and return a pointer to it
			return pNewAgent;
		} else {
			// if creation failed, trigger a fatal error
			FatalError("Error creating agent of type " + sAgentTypeName + 
					   ".");
			return NULL;
		}
	}
}
