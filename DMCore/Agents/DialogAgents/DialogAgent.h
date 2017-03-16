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
// DIALOGAGENT.H - definition of the CDialogAgent class. This class defines 
//                 the basic capabilities of a DialogAgent: execution, agenda
//                 definition, etc, etc
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
//   [2005-10-22] (antoine): Added methods RequiresFloor and 
//							 IsConversationSynchronous to regulate turn-taking
//                           and asynchronous agent planning/execution
//   [2005-01-21] (jsherwan): Added input line config functionality
//   [2004-12-23] (antoine): modified constructor, agent factory to handle
//							  configurations
//   [2004-05-24] (dbohus):  fixed bug in ExplainMore, WhatCanISay and Timeout
//                            prompt generation
//   [2004-04-24] (dbohus):  added Create method
//   [2004-04-24] (dbohus):  added indices to the last input and binding 
//                            results
//   [2004-04-17] (dbohus):  added support for declaring the grounding models
//                            subsumed by the concept
//   [2004-04-16] (dbohus):  added grounding models on dialog agents
//   [2004-04-01] (dbohus):  fixed potential buffer overrun problem
//   [2003-10-14] (dbohus):  added dynamic agent ids
//   [2003-07-11] (antoine): added function and macro LanguageModel to allow 
//							 agents to specify a LM for speech recognition
//   [2003-05-09] (dbohus):  fixed bug on default failure criteria
//   [2003-04-15] (dbohus,
//                 antoine): introduced support for *-type expectations
//   [2003-04-09] (dbohus):  created OnCreation and OnInitialization
//   [2003-04-08] (dbohus):  changed so that completion is evaluated on each
//                            call to HasCompleted()
//   [2003-04-03] (dbohus,
//                 antoine): lowercased all grammar expectations declared
//   [2003-02-13] (dbohus):  added dercRestartDialog to return codes for agents
//   [2003-01-27] (dbohus):  split ReOpen into ReOpenTopic and ReOpenConcepts
//   [2003-01-22] (dbohus):  added reset and reopen counters and methods to 
//                            access them   
//   [2002-11-20] (dbohus):  added support for maximum number of attempts on an
//                            agent (after that it will terminate with failure)
//   [2002-11-20] (dbohus):  changed so that an agent can complete with success
//                            or failure
//   [2002-11-20] (dbohus):  added support for customizing binding filters in 
//                            concept expectations / grammar mappings
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-13] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __DIALOG_AGENT_H__
#define __DIALOG_AGENT_H__

#include "DMCore/Agents/Agent.h"
#include "DMCore/Concepts/AllConcepts.h"
#include "DMCore/Grounding/Grounding.h"

//-----------------------------------------------------------------------------
// CDialogAgent Class - 
//   This is the base of the dialog agent classes. It implements the basic 
//   functionality of a dialog agent: execution, agenda definition, etc. 
//   It is an abstract class, which only provides a basis for the hierarchy,
//   it is not to be instantiated directly.
//-----------------------------------------------------------------------------

// D: forward class definition
class CDialogAgent;

//-----------------------------------------------------------------------------
// A few auxiliary type definitions: execution return codes, 
// concept expectation, focus claims, etc
//-----------------------------------------------------------------------------

// D: definition for return codes on dialog agent execution
typedef enum { dercContinueExecution,		// continue the execution
			   dercYieldFloor,				// gives the floor to the user
			   dercTakeFloor,				// takes the floor
			   dercWaitForEvent,			// waits for a real-world event
			   dercFinishDialog,			// terminate the dialog 
			   dercFinishDialogAndCloseSession,			// terminate the dialog 
											// and sends a close session message to the hub
               dercRestartDialog            // restart the dialog
} TDialogExecuteReturnCode;			

// D: definition of things that get bound to a concept
typedef enum { bmSlotValue,       // bind the value of the slot 
               bmExplicitValue,   // bind an explicit value specified in the 
                                  //  expectation 
               bmBindingFilter    // bind the result of applying a custom 
                                  //  binding filter                              
} TBindMethod;

// D: structure describing a concept expectation from a dialog agent
//    Note that this structure is also reflected into Helios, since Helios
//    will use it to figure out expectation level features. Any changes to 
//    this structure should also lead to necessary changes in helios
typedef struct {
	CDialogAgent* pDialogAgent;		// which agent expects this
	string sConceptName;			// the name of the concept that we will
									//  bind to
	TStringVector vsOtherConceptNames;
	                                // vector of other concept names 
	                                //  that are implicitly requested by 
	                                //  this agent (sometimes we do a 
	                                //  request on a full structure for only
	                                //  to get some of the members)
	string sGrammarExpectation;		// the grammar slot that is expected
	TBindMethod bmBindMethod;	    // indicates the binding method to be 
                                    //  used
	string sExplicitValue;			// the value bound to the concept in case
									//  the grammar slot appears in the parse
									//  will be bound to the concept, o/w the
									//  value extracted from the grammar will
									//  be bound
    string sBindingFilterName;      // the name of the registered custom 
                                    //  binding filter
	bool bDisabled;					// indicates that this expectation is 
									//  disabled at this moment
	string sReasonDisabled;			// indicates why the expectation is 
									//  disabled
    string sExpectationType;        // indicates the type of this expectation
                                    //  (i.e. ! or @ or * ... )
} TConceptExpectation;

// D: definition of concept expectation collection
typedef vector<TConceptExpectation, allocator <TConceptExpectation> >
    TConceptExpectationList;

// D: structure describing a focus claim
typedef struct {
	string sAgentName;				// the name of the agent that claims focus
	bool bClaimDuringGrounding;     // indicates whether or not the focus is
	                                //  claimed during grounding
} TFocusClaim;

// D: definition of a list of focus claims
typedef vector<TFocusClaim, allocator <TFocusClaim> > TFocusClaimsList;

// D: enumeration type describing the various methods for adding subagents
typedef enum {
	asamAsLastChild,
	asamAsFirstChild,
	asamAsLeftSibling,
	asamAsRightSibling,
} TAddSubAgentMethod;

// D: definition of vector type for agents
class CDialogAgent;
typedef vector <CDialogAgent*, allocator <CDialogAgent*> > TAgentsVector;

// D: definition for completion types
typedef enum { ctSuccess,           // successful completion
               ctFailed,            // completion by failure
} TCompletionType;

//-----------------------------------------------------------------------------
// D: Defines for binding policies
//-----------------------------------------------------------------------------
// D: the binding policies are described as strings, as they might get 
//	  arbitrarily complex 

// D: Topic-initiative: bind concepts only within the current topic / focus
#define WITHIN_TOPIC_ONLY "bind-this-only"

// D: Mixed-initiative: bind anything
#define MIXED_INITIATIVE "bind-anything"


//-----------------------------------------------------------------------------
//
// The CDialogAgent Class 
//
//-----------------------------------------------------------------------------

class CDialogAgent : public CAgent {

protected: 
	
	//---------------------------------------------------------------------
	// Members: subagents, concepts, parent-link. various status 
	//          information
	//---------------------------------------------------------------------

	// the "small" name of the dialog agent. sName (inherited) will hold
	// the full path to the agent in the tree. Do not use sName directly, 
	// use GetName instead since the name of an agent can change as it 
	// becomes part of a larger tree of agents
	string sDialogAgentName;	

	// the list of concepts that this dialog agent holds
	TConceptPointersVector Concepts;

	// the list of subagents
	TAgentsVector SubAgents;

	// pointer to the parent dialog agent
	CDialogAgent* pdaParent;

	// pointer to the dialog agent which represents the context
	// of this agent in the task tree (mostly for agents that are
	// attached to the tree like grounding agents)
	CDialogAgent* pdaContextAgent;

	// pointer to the grounding model
    CGroundingModel* pGroundingModel;

	// indicates if the agent has completed or not
	bool bCompleted;

    // indicates how the agent completed
    TCompletionType ctCompletionType;

	// indicates if this agent is blocked or not
	bool bBlocked;

    // a boolean indicated if the agent was added to the tree at runtime
    bool bDynamicAgent;

    // a dynamic id for the agent (for dynamically generated agents)
    string sDynamicAgentID;

    // holds the grammar mapping for the commands that trigger then agent
    string sTriggeredByCommands;

    // holds the grounding model spec to be used for the commands that 
    // trigger the agent
    string sTriggerCommandsGroundingModelSpec;

    // indicates how many times the agent was attempted since the last 
    // reset/reopen
	int iExecuteCounter;

    // indicates how many times the agent was reset so far
    int iResetCounter;

    // indicates how many times the agent was reopened since the last reset
    int iReOpenCounter;

    // indicates for how many turns the agent was in focus since the 
    // last reset/reopen
    int iTurnsInFocusCounter;

    // holds an index (for the input manager) to the last input for this
    // agent
    int iLastInputIndex;
	int iLastExecutionInputIndex;

	// holds an index (for the output manager) to the last execution of this
	// agent
    int iLastExecutionIndex;

	// holds an index (for the core agent) to the last binding results for
    // this agent (both for the last event and the last user turn)
    int iLastBindingsIndex;

	// J: hash of configuration slot/values for input line
	STRING2STRING s2sInputLineConfiguration;

	// J: indicates whether parent's input line configuration has been inherited
	bool bInheritedParentInputConfiguration;

public:
	
	//---------------------------------------------------------------------
	// Constructors and destructors
	//---------------------------------------------------------------------

	// Default constructor
	CDialogAgent(string sAName,
				 string sAConfiguration = "",
				 string sAType = "CAgent:CDialogAgent");

	// Virtual destructor
	virtual ~CDialogAgent();

	// Static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);


public:

	//---------------------------------------------------------------------
	// CAgent specific methods (overwritten)
	//---------------------------------------------------------------------
	
	// registers the agent
	virtual void Register();

	// method called upon creation
	virtual void Create();

	// method called when the agent is placed in the DTT
	virtual void Initialize();

	// resets the agent
	virtual void Reset();


public:

	//---------------------------------------------------------------------
	// Fundamental Dialog Agent methods. Most of these are to be 
	// overwritten by derived agent classes
	//---------------------------------------------------------------------

	// Virtual function that creates the concepts for this agency, gets 
    // called by Initialize
	virtual void CreateConcepts();

    // Virtual function which specifies if this agent is executable. 
	// This is a pure method which makes this class abstract. Is to be 
	// overwritten by derived agents.
	virtual bool IsExecutable();

	// Virtual function for execution. This is a pure method which makes
	// this class abstract. Is to be overwritten by derived agents.
	virtual TDialogExecuteReturnCode Execute();

	// Virtual function for declaring the list of concepts that the
	// dialog agent expects at a certain point. For this class, it 
	// merely collects the expectations of the subagents. To be overwritten 
	// by derived classes
	virtual int DeclareExpectations(TConceptExpectationList& 
								    rcelExpectationList); 

    // Virtual function for declaring the list of concepts that the agent
    // subsumes
    virtual void DeclareConcepts(
	    TConceptPointersVector& rcpvConcepts, 
		TConceptPointersSet& rcpsExclude);

	// Create the grounding model for the agent
    virtual void CreateGroundingModel(string sGroundingModelSpec);

    // Obtains the grounding model for the agent
    virtual CGroundingModel* GetGroundingModel();

    // Virtual function for declaring the list of grounding models that 
    // the agent subsumes
    virtual void DeclareGroundingModels(
        TGroundingModelPointersVector& rgmpvModels,
        TGroundingModelPointersSet& rgmpsExclude);

	// Virtual function which implements a condition which indicates if the 
	// expectations declared should be enabled or not. For this base class, it 
	// always returns true
	virtual bool ExpectCondition();

	// Virtual function for declaring the binding policy to be used
	// for this particular agent
	virtual string DeclareBindingPolicy();

	// Virtual functions for declaring a claim for focus made by the
	// dialog agent. For this class, it merely collects the focus claims
	// of the subagents. To be overwritten by derived classes
	virtual int DeclareFocusClaims(TFocusClaimsList& fclFocusClaims);

	// Virtual function implementing the precondition for execution
	// of that agent. For this class, it does nothing (always returns
	// true); is to be overwritten by derived agents
	virtual bool PreconditionsSatisfied();

	// Virtual function indicating when the agent claims focus. For this
	// class it always returns false (the agent never claims focus)
	virtual bool ClaimsFocus();
	
	// Virtual function which indicates if the agent can claim focus 
	// while grounding is in progress
	virtual bool ClaimsFocusDuringGrounding();

	// Virtual function indicating if this agent is to be triggered by
	// a command from the user. The function returns as a string the
	// grammar concept(s) corresponding to that user command
	virtual string TriggeredByCommands();

	// Creates a triggering concept for this agent in case one is needed 
	// (gets called during Initialize)
	void CreateTriggerConcept();

    // Virtual function indicating when an agent completes successfully
    // This basic dialogagent class completes successfully when all the subagents 
	// have completed
	virtual bool SuccessCriteriaSatisfied();

    // Virtual function indicating when an agents completes with a failure
    // This basic dialogagent class completes with a failure when the maximum 
    // number of attempts has been exceeded
	virtual bool FailureCriteriaSatisfied();

    // Virtual function indicating how many times the execution of the agent
    // should be attempted
    virtual int GetMaxExecuteCounter();

    // Virtual function that gets called upon the creation of the dialog 
    // agent
    virtual void OnCreation();
    
    // Virtual function that gets called upon the destruction of the dialog
    // agent
    virtual void OnDestruction();

    // Virtual function that gets called upon the initialization of the 
	// dialogagent. For this basic class, it does nothing; is to be
	// overwritten by derived agents
	virtual void OnInitialization();

	// Virtual function that gets called upon the completion of the 
	// dialogagent. For this basic class, it does nothing
	virtual void OnCompletion();

    // Virtual function that is used to "reopen" an agent for conversation. By
	// default, for this basic class, calls upon ReOpenTopic and ReOpenConcepts
	virtual void ReOpen();

    // Virtual function that "reopens" all the concepts an agent holds for 
    // conversation. 
	virtual void ReOpenConcepts();

    // Virtual function that is used to "reopen" the topic of this agent. By
    // default, it resets completed and adjusts the counters, and calls itself
    // recursively on the subagents
	virtual void ReOpenTopic();

    // Virtual function indicating if the agent implements a main topic
	// of conversation
	virtual bool IsAMainTopic();

	// Virtual function returning the prompt for a dialog agent: 
	// by default it returns nothing
	virtual string Prompt();

	// Virtual function returning the timeout prompt for a dialog agent: 
	// by default it returns nothing
	virtual string TimeoutPrompt();

	// Virtual function returning the explain more prompt for a dialog agent: 
	// by default it returns nothing
	virtual string ExplainMorePrompt();

    // Create a versioned prompt (adds version=xxx)
    virtual string CreateVersionedPrompt(string sVersion);

	// Virtual function returning the establish contenxt prompt for a dialog
	// agent: by default it calls on the parent of the agent
	virtual string EstablishContextPrompt();

	// Virtual function returning the what can i say prompt for a dialog agent: 
	// by default it returns nothing
	virtual string WhatCanISayPrompt();

	// J/AR: Virtual function returning the initialization string for the
	// input line configuration
	virtual string InputLineConfigurationInitString();

    // Virtual function with specifies whether this is a dialog task 
    // specification agent or not (i.e. grounding agent are not DTS agents)
    virtual bool IsDTSAgent();

	// Virtual function which specifies if the execution of this agent
	// has to be synchronized with the actual flow of the conversation
	// or if it can be anticipated (i.e. whether this execution yields
	// side effects for the conversation)
    virtual bool IsConversationSynchronous();

    // Virtual function with specifies whether this agent requires the
	// floor for its execution (by default, agents do not)
    virtual bool RequiresFloor();

	// Virtual function used to cancel the effects of an agent which 
	// was executed within the DM but not realized (i.e. there was an
	// interruption of the normal flow of the dialogue due to a user
	// barge-in or another external event)
	virtual void Undo();

public:

	//----------------------------------------------------------------------
	// Other methods for providing access to private/protected members
	//----------------------------------------------------------------------

	// Access to concepts through relative paths
	//
	virtual CConcept& C(string sConceptPath);
	virtual CConcept& C(const char* lpszConceptPath, ...);
	virtual CConcept& LocalC(string sConceptName);

	// Access to related agents through relative paths
	//
	CDialogAgent& A(string sDialogAgentPath);
	CDialogAgent& A(const char* lpszDialogAgentPath, ...);

	// Methods for adding and deleting subagents
	//
	void AddSubAgent(CDialogAgent* pdaWho, CDialogAgent* pdaWhere, 
					 TAddSubAgentMethod asamMethod);
	void DeleteSubAgent(CDialogAgent* pdaWho);
    void DeleteDynamicSubAgents();

    // Access to parent dialog agent information
	//
	void SetParent(CDialogAgent* pdaAParent);
	CDialogAgent* GetParent();
	void UpdateName();

    // Access to context dialog agent information
	//
	void SetContextAgent(CDialogAgent* pdaAContextAgent);
	CDialogAgent* GetContextAgent();

	// Method for obtaining the main topic for this agent
    //
	CDialogAgent* GetMainTopic();

    // Access to completion status
	//
	bool HasCompleted(); 
	void SetCompleted(TCompletionType ctACompletionType = ctSuccess);
    void ResetCompleted();
    bool HasFailed();
    bool HasSucceeded();

    // Blocking related methods
    //
	bool IsAgentPathBlocked(); 
	virtual bool IsBlocked();
	void Block();
	void UnBlock();

    // Access to dynamic agent id information
    //
    void SetDynamicAgent();
    bool IsDynamicAgent();
    void SetDynamicAgentID(string sADynamicAgentID);
    string GetDynamicAgentID();

    // Access to execute counter
    //
    void IncrementExecuteCounter();
    int GetExecuteCounter();

    // Access to reset and reopen information
    //
    bool WasReset();
    bool WasReOpened();

    // Access to turns in focus counter
    //
    void IncrementTurnsInFocusCounter();
    int GetTurnsInFocusCounter();

    // Access to last input index
    //
    void SetLastInputIndex(int iInputIndex);
    int GetLastInputIndex();

    // Access to the input index at last execution
    //
    void SetLastExecutionInputIndex(int iInputIndex);
    int GetLastExecutionInputIndex();

    // Access to last execution index
    //
    void SetLastExecutionIndex(int iExecutionIndex);
    int GetLastExecutionIndex();

    // Access to last bindings index
    void SetLastBindingsIndex(int iBindingsIndex);
    int GetLastBindingsIndex();

	// J: Access to s2sInputLineConfiguration
	// TODO: Merge this code with the same-named functions in Agent.[cpp|h]
	// Begin copy

	// Sets the configuration from a configuration string
	void SetInputConfiguration( string sConfiguration);

	// Sets an individual configuration parameter
	void SetInputConfigurationParameter( string sSlot, string sValue);

	// Tests if a parameter exists in the configuration
	bool HasInputConfigurationParameter( string sSlot);

	// Gets the value for a given parameter
	string GetInputConfigurationParameterValue( string sSlot);
	// End copy

	// J: Selectively inherits parent's input line configuration
	void InheritParentInputConfiguration();

	// J: Gets the final input line configuration for this agent
	STRING2STRING GetInputLineConfiguration();

protected:

	//----------------------------------------------------------------------
	// Various protected methods for parsing declarative constructs
	//----------------------------------------------------------------------

	// Parse a grammar mapping specification into an expectation list
	void parseGrammarMapping(string sConceptNames, string sGrammarMapping, 
							 TConceptExpectationList& rcelExpectationList);
};

// NULL dialog agent: this object is used designate invalid dialog agent
// references
extern CDialogAgent NULLDialogAgent;

//-----------------------------------------------------------------------------
//
// Specific macros to be used when definining derived dialog agents, in 
//  the dialog task file
//
//-----------------------------------------------------------------------------

// D: macro for defining preconditions for agents
#define PRECONDITION(Condition)\
	public:\
	virtual bool PreconditionsSatisfied() {\
		return (Condition);\
	}\

// D: macro for defining preconditions that are the same as the triggers
#define SAME_AS_TRIGGER \
    ClaimsFocus()\
    
// D: macro which indicates that the agent can claim the focus during grounding
#define CAN_TRIGGER_DURING_GROUNDING \
    public:\
    virtual bool ClaimsFocusDuringGrounding() {\
        return true;\
    }\

// D: macro for defining success criteria (when an agency completes successfully)
#define SUCCEEDS_WHEN(Condition)\
	public:\
	virtual bool SuccessCriteriaSatisfied() {\
		return (Condition);\
	}\

// D: macro for defining failure criteria (when an agency completes with failure)
#define FAILS_WHEN(Condition)\
	public:\
	virtual bool FailureCriteriaSatisfied() {\
		return (Condition);\
	}\

// D: macro for defining the maximum number of attempts for an agent
#define MAX_ATTEMPTS(Count)\
    public:\
    virtual int GetMaxExecuteCounter() {\
        return (Count);\
    }\

// D: macro for defining the concept binding policy
#define CONCEPT_BINDING_POLICY(Policy)\
	public:\
	virtual string DeclareBindingPolicy() {\
		return (Policy);\
	}\

// D: defining the expect condition
#define EXPECT_WHEN(Condition)\
	public:\
	virtual bool ExpectCondition() {\
		return(Condition);\
	};\

// D: macro for specifying code on the creation of each agent
#define ON_CREATION(DO_STUFF)\
	public:\
	virtual void OnCreation() {\
		DO_STUFF;\
	}\

// D: macro for specifying code on the destruction of each agent
#define ON_DESTRUCTION(DO_STUFF)\
	public:\
	virtual void OnDestruction() {\
		DO_STUFF;\
	}\

// D: macro for specifying initialization code for an agent
#define ON_INITIALIZATION(DO_STUFF)\
	public:\
	virtual void OnInitialization() {\
		DO_STUFF;\
	}\

// D: macro for defining an "effect" for the agent, through the OnCompletion
//    virtual function
#define ON_COMPLETION(DO_STUFF)\
	public:\
	virtual void OnCompletion() {\
        DO_STUFF;\
	}\

// D: macro for defining reopening behavior for a dialog agent
#define ON_REOPEN(DO_STUFF)\
	public:\
	virtual void ReOpen() {\
		DO_STUFF;\
	}\


// D: macro to be used to reset an agent on completion
#define RESET {Reset();}
#define BLOCK {Block();}
#define REOPEN {ReOpen();}

// D: macro for defining the conditions under which the agent claims focus
#define TRIGGERED_BY(Condition)\
	public:\
	virtual bool ClaimsFocus() {\
		return (Condition);\
	}\

// D: macro for definiting the user commands which trigger this agent
#define TRIGGERED_BY_COMMANDS(Commands, GroundingModelSpec)\
	public:\
	virtual string TriggeredByCommands() {\
        sTriggeredByCommands = Commands;\
        sTriggerCommandsGroundingModelSpec = GroundingModelSpec;\
        return (Commands);\
	}\

// A: macro for defining agents which require the floor to be executed
#define REQUIRES_FLOOR()\
	public:\
	virtual bool RequiresFloor() {\
		return true;\
	}\
	
// A: macro for defining agents which do not require the floor in order to be executed
#define DOES_NOT_REQUIRE_FLOOR()\
	public:\
	virtual bool RequiresFloor() {\
		return false;\
	}\
	
// D: macro for defining agents which are main topics
#define IS_MAIN_TOPIC()\
	public:\
	virtual bool IsAMainTopic() {\
		return true;\
	}\
	
// D: macro for defining agents which are main topics
#define IS_NOT_DTS_AGENT()\
	public:\
	virtual bool IsDTSAgent() {\
		return false;\
	}\

// D: macro for defining the prompt
#define PROMPT(PROMPT)\
	public:\
	virtual string Prompt() {\
		return (PROMPT);\
	}\

// D: macro for defining the timeout prompt
#define PROMPT_TIMEOUT(PROMPT)\
	public:\
	virtual string TimeoutPrompt() {\
		return (PROMPT);\
	}\

// D: macro for defining the explain more prompt
#define PROMPT_EXPLAINMORE(PROMPT)\
	public:\
	virtual string ExplainMorePrompt() {\
		return (PROMPT);\
	}\

// D: macro for defining the establish context prompt
#define PROMPT_ESTABLISHCONTEXT(PROMPT)\
	public:\
	virtual string EstablishContextPrompt() {\
		return (PROMPT);\
	}\

// D: macro for defining the what can i say prompt
#define PROMPT_WHATCANISAY(PROMPT)\
	public:\
	virtual string WhatCanISayPrompt() {\
		return (PROMPT);\
	}\

// AR: macro for defining the language model required by the agent
#define INPUT_LINE_CONFIGURATION(CONFIG_LINE)\
	public:\
	virtual string InputLineConfigurationInitString() {\
		return (CONFIG_LINE);\
	}\

// A: macro for defining agents which cannot be executed by anticipation
#define IS_CONVERSATION_SYNCHRONOUS()\
	public:\
	virtual bool IsConversationSynchronous() {\
		return true;\
	}\

#endif // __DIALOG_AGENT_H__
