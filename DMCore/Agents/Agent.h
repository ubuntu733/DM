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
// AGENT.H - definition of the CAgent base class
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
//   [2004-12-23] (antoine): added configuration methods, modified constructor 
//							 and factory method to handle configurations
//   [2004-04-24] (dbohus): added create method
//   [2002-05-25] (dbohus): deemed preliminary stable version 0.5
//   [2001-12-30] (dbohus): started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __AGENT_H__
#define __AGENT_H__

#include "Utils/Utils.h"
#include "Registry.h"

//-----------------------------------------------------------------------------
// CAgent Class - 
//   This is the base of the agent classes. It implements the basic 
//   functionality of an agent: name, type, registering, dispatch routine call
//-----------------------------------------------------------------------------

class CAgent {

protected:
	//---------------------------------------------------------------------
	// Name and Type class members
	//---------------------------------------------------------------------
	//
	string sName;		// name of agent
	string sType;		// type of agent
	STRING2STRING s2sConfiguration;		// hash of parameters

public:
	//---------------------------------------------------------------------
	// Constructors and destructors
	//---------------------------------------------------------------------
	// 
	CAgent(string sAName, 
		   string sAConfiguration = "",
		   string sAType = "CAgent");
	virtual ~CAgent();

	// Static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

	//---------------------------------------------------------------------
	// Methods for access to private and protected members
	//---------------------------------------------------------------------
	
	string GetName();
	string GetType();

	// Sets the configuration from a configuration string or from a hash
	// 
	void SetConfiguration(string sConfiguration);
	void SetConfiguration(STRING2STRING s2sAConfiguration);	

	// Sets an individual configuration parameter
	void SetParameter(string sParam, string sValue);

	// Tests if a parameter exists in the configuration
	bool HasParameter(string sParam);

	// Gets the value for a given parameter
	string GetParameterValue(string sParam);

	//---------------------------------------------------------------------
	// CAgent specific methods
	//---------------------------------------------------------------------
	
	// registering and unregistering the agent
	//
	virtual void Register();
	virtual void UnRegister();

	// This method is called immediately after an agent is constructed 
	// by the AgentsRegistry.CreateAgent function. 
	// 
	virtual void Create();

	// This method is called to initialize an agent (usually after it's 
	// mounted in the dialog task tree)
	//
	virtual void Initialize();

	// resets the agent (brings it to the same state as after construction
	// and Initialize
	//
	virtual void Reset();

};

#endif // __AGENT_H__