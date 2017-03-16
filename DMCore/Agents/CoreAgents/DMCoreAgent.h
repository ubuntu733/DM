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
// DMCoreAgent.H - defines the core dialog management agent, that handles
//				   execution and input passes (agenda generation, bindings, 
//				   etc)
//                 ** THIS IS A DEVELOPMENT VERSION THAT NEEDS TO BE MERGED 
//                    WITH DMCOREAGENT WHEN IT'S STABLE **
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
//   [2007-03-05] (antoine): changed Execute so that grounding and dialog agents
//							 are only executed once the floor is free and all 
//							 pending prompt notifications have been received 
//							 (previously, only the floor mattered)
//   [2004-12-29] (antoine): added method GetNumberNonUnderstandings
//   [2004-12-23] (antoine): modified constructor and agent factory to handle
//							  configurations
//   [2004-04-06] (dbohus):  rearranged the broadcast agenda into a new, 
//                            compressed format
//   [2004-03-25] (dbohus):  moved functionality for broadcastign the 
//                            expectation agenda into the state manager where
//                            it actually belong
//   [2004-03-23] (dbohus):  added functionality for broadcasting the 
//                            expectation agenda
//   [2004-03-22] (dbohus):  fixed binding counting and developed bindings
//                            description and history routines
//   [2004-03-03] (dbohus):  added RestartTopic
//   [2004-03-03] (dbohus):  fixed bug in popFromExecutionStack so that indeed
//                            all the children are removed. Added 
//                            PopAgentFromExecutionStack() and
//                            PopTopicFromExecutionStack()
//   [2004-03-03] (dbohus):  added access to binding history datastructure
//   [2004-02-16] (dbohus):  added binding history datastructure (maintains a 
//                            history of features regarding the binding
//                            process)
//   [2003-11-09] (dbohus):  moved the grounding and focus claims phase out of 
//                            the input phase, into the execution cycle; now 
//                            they happen at every clock tick, when the core
//                            agent has been signaled to do them (added the 
//                            SignalFocusClaimsPhase and SignalGroundingPhase
//                            methods)
//   [2003-07-11] (antoine): added support for state-dependent language models
//							  (function updateLanguageModel called in 
//							  doInputPass)
//   [2003-04-29] (dbohus):  fixed small bug in Execute (checking for empty
//                            execution stack after popCompleted)
//   [2003-04-25] (dbohus,
//                 antoine): checking for completion of agents changed to 
//                            before execution; added PopAgentFromExecutionStack
//   [2003-04-15] (dbohus,
//                 antoine): introduced support for *-type expectations
//   [2003-04-13] (dbohus):  added friend class CGroundingManagerAgent
//                           delete counters for bindings in 2nd phase, since
//                            there's no more second phase
//   [2003-04-08] (dbohus):  added support for logging the execution stack
//   [2003-03-24] (dbohus):  changed resolveFocusShifts so that all the 
//                            claiming agents are pushed on the stack
//   [2003-02-13] (dbohus):  added support for dercRestartDialog 
//   [2003-01-15] (dbohus):  changed so that conflicting expectations are not
//                            created when the slots map to the same concept
//                            from different agents
//   [2002-12-01] (dbohus):  added confidence to concept binding
//   [2002-11-20] (dbohus):  added support for and binding through custom 
//                            binding filters
//   [2002-10-19] (dbohus):  added basic skeleton code for non-understanding 
//                            processing
//   [2002-07-15] (dbohus):  fixed bug with binding on multiple expectations on
//							  a given level
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-04-12] (dbohus):  decided to transform it into a core agent, which
//                            therefore took over all the functionality that 
//							  we previously had implemented in DMCore.h (cpp). 
//   [2002-01-26] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __DMCOREAGENT_H__
#define __DMCOREAGENT_H__

#include "Utils/Utils.h"
#include "DMCore/Agents/Agent.h"
#include "DMCore/Agents/DialogAgents/DialogAgent.h"

// D: when ALWAYS_CONFIDENT is defined, the binding on concepts will ignore the
//    confidence scores on the input and will be considered "always confident"
// #define ALWAYS_CONFIDENT

//-----------------------------------------------------------------------------
// D: Auxiliary type definitions for the expectation agenda
//    These definitions are also reflected into Helios, since Helios uses the 
//    agenda to simulate this binding process in advance in order to compute
//    expectation features. Any changes to these structures, need to be 
//    also reflected in Helios
//-----------------------------------------------------------------------------
// D: a compiled concept expectation: implemented as a mapping between a 
//    string (the grammar concept), and a vector of integers (pointers in the 
//	  concept expectation list. This representation is used for faster 
//    checking of grammar concept expectations

typedef map <string, TIntVector, less <string>, allocator <TIntVector> > 
	TMapCE;

typedef struct {
	TMapCE mapCE;					// the hash of compiled expectations
	CDialogAgent* pdaGenerator;		// the agent that represents that level
									//  of expectations
} TCompiledExpectationLevel;

// D: the struct representation for the compiled expectation agenda. It 
//    contains the expectation list gathered from the dialog task tree, 
//    and a "compiled" representation for each of the levels
typedef struct {
	// the full system expectations, as gathered from the dialog task tree
	TConceptExpectationList celSystemExpectations;		

	// an array holding the expectations of different levels (on index 0, 
	// are the expectations of the focused agent, on index 1 those of the
	// immediate upper level agent, etc)
	vector <TCompiledExpectationLevel, allocator <TCompiledExpectationLevel> >
		vCompiledExpectations;
} TExpectationAgenda; 


//-----------------------------------------------------------------------------
// D: Auxiliary type definitions for the execution stack and history
//-----------------------------------------------------------------------------

// D: structure holding an execution stack item
typedef struct {
	CDialogAgent* pdaAgent;			// the agent that is on the stack
	int iEHIndex;					// an index into it's correspondent history
									//   entry
} TExecutionStackItem;

// D: definition of the execution stack type - essentially a list 
typedef list <TExecutionStackItem, allocator<TExecutionStackItem> > 
	TExecutionStack;

// D: structure holding a execution history item
typedef struct {
	string sCurrentAgent;			// the name of the agent that is executed
	string sCurrentAgentType;		// the type of that agent
	string sScheduledBy;			// the agent that scheduled this one for
									//   execution
	bool bScheduled;				// has the history item been scheduled 
									//   for execution?
	bool bExecuted;					// has the history item been executed?
	bool bCommitted;				// has the history item been committed
									//   to history?
	bool bCanceled;					// has the history item been canceled
									//   before being committed?
	timeb timeScheduled;			// the time when the agent was scheduled
									//   for execution
	vector<timeb, allocator<timeb> > 
		vtExecutionTimes;			// the times when the agent was actually
									//   executed
	timeb timeTerminated;			// the time when the agent completed 
									//   execution
	int iStateHistoryIndex;			// the index in the history of dialog
									// states when the agent was executed
} TExecutionHistoryItem;

// A: definition of the execution history class - an extended vector
class CExecutionHistory: public vector<TExecutionHistoryItem, allocator<TExecutionHistoryItem> > {


};

//-----------------------------------------------------------------------------
// D: Auxiliary type definitions for bindings
//-----------------------------------------------------------------------------

// D: structure describing one particular binding
typedef struct {
	bool bBlocked;					// indicates whether the binding was 
									//  blocked or not
	string sGrammarExpectation;		// the expected grammar slot
	string sValue;					// the value in the binding
	float fConfidence;				// the confidence score for the binding
	int iLevel;						// the level in the agenda
	string sAgentName;				// the name of the agent that declared the 
									//  expectation
	string sConceptName;			// the name of the concept that will bind
	string sReasonDisabled;			// if the binding was blocked, the reason
									//  the expectation was disabled
} TBinding;

// D: structure describing a particular forced update
#define FCU_EXPLICIT_CONFIRM	1
#define FCU_IMPLICIT_CONFIRM	2
#define FCU_UNPLANNED_IMPLICIT_CONFIRM  3
typedef struct {
	string sConceptName;			// the name of the concept that had a 
									//  forced update
	int iType;						// the type of the forced update
	bool bUnderstanding;			// the update changed the concept 
									//  enough that the grounding action
									//  on it is different, and therefore
									//  we consider that we have an actual
									//  understanding occuring on that 
									//  concept
} TForcedConceptUpdate;

// D: structure holding a binding history item, describing the bindings in a 
//    turn
typedef struct {
//    int iTurnNumber;                // the input turn number      
	string sEventType;				// the type of event to which this
										// binding corresponds
    bool bNonUnderstanding;         // was the turn a non-understanding?
                                        // i.e. no concepts bound
    int iConceptsBound;             // the number of bound concepts
    int iConceptsBlocked;           // the number of blocked concepts
	int iSlotsMatched;				// the number of slots that matched
	int iSlotsBlocked;				// the number of slots that were blocked
	vector<TBinding> vbBindings;	// the vector of bindings
	vector<TForcedConceptUpdate> vfcuForcedUpdates;
									// the vector of forced updates
} TBindingsDescr;

// D: definition of the binding history - a vector
typedef vector<TBindingsDescr, allocator<TBindingsDescr> >
    TBindingHistory;

// D: definition of function type for performing customized bindings 
//     (binding filter functions)
typedef string (*TBindingFilterFunct)(string sSlotName, string sSlotValue);

// D: auxiliary definition for mapping from binding filter names to the actual
//    functions
typedef map<string, TBindingFilterFunct, 
    less<string>, allocator<TBindingFilterFunct> > STRING2BFF;

// D: structure maintaining a description of the current system action on the
//    various concepts
typedef struct {
	set<CConcept *> setcpRequests;
	set<CConcept *> setcpExplicitConfirms;
	set<CConcept *> setcpImplicitConfirms;
	set<CConcept *> setcpUnplannedImplicitConfirms;
} TSystemAction;

// D: structure describing system actions taken on a particular concept
#define SA_REQUEST	"REQ"
#define SA_EXPL_CONF	"EC"
#define SA_IMPL_CONF	"IC"
#define SA_UNPLANNED_IMPL_CONF  "UIC"
#define SA_OTHER	"OTH"
typedef struct {
	string sSystemAction;
	// *** this might need to be elaborated with other system actions
} TSystemActionOnConcept;

// D: definition for customized start over routine
typedef void (*TCustomStartOverFunct)();

//-----------------------------------------------------------------------------
//
// D: CDMCoreAgent class -
//      implements core dialog management functionalities execution, input 
//      passes, expectation agenda, etc
//
//-----------------------------------------------------------------------------

// A: Floor status constants
typedef enum { fsUnknown,		// floor owner is unknown
			   fsUser,			// floor owner is user
			   fsSystem,		// floor owner is system
			   fsFree,			// floor is not owned by anyone
} TFloorStatus;		

extern vector<string> vsFloorStatusLabels;

class CDMCoreAgent: public CAgent {

private:

    //---------------------------------------------------------------------
	// Private members
	//---------------------------------------------------------------------
	//
	TExecutionStack esExecutionStack;		// the execution stack
	CExecutionHistory ehExecutionHistory;	// the execution history
    TBindingHistory bhBindingHistory;       // the binding history
	TExpectationAgenda eaAgenda;			// the expectation agenda
	TFocusClaimsList fclFocusClaims;		// the list of focus claims
	TSystemAction saSystemAction;			// the current system action
	
    int iTimeoutPeriod;						// the current timeout period
	int iDefaultTimeoutPeriod;				// the default timeout period

    float fNonunderstandingThreshold;       // the current nonunderstanding
                                            //  threshold
    float fDefaultNonunderstandingThreshold;// the default nonunderstanding 
                                            //  threshold                                                 
   
    STRING2BFF s2bffFilters;                // the register of binding 
                                            //  filters

    bool bFocusClaimsPhaseFlag;             // indicates whether we should
                                            //  run focus claims
	bool bAgendaModifiedFlag;				// indicates if the agenda should
											// be recompiled

	TFloorStatus fsFloorStatus;             // indicates who has the floor
	int iTurnNumber;						// stores the current turn number
    TCustomStartOverFunct csoStartOverFunct;// a custom start over function

    //---------------------------------------------------------------------
    // The grounding manager needs access to internals, so it is declared
    // as a friend class
    //---------------------------------------------------------------------
    
    friend class CGroundingManagerAgent;
    friend class CStateManagerAgent;

public:

	//---------------------------------------------------------------------
	// Constructor and destructor
	//---------------------------------------------------------------------
	//
	// Default constructor
	CDMCoreAgent(string sAName, 
				 string sAConfiguration = "",
			     string sAType = "CAgent:CDMCoreAgent");

	// Virtual destructor
	virtual ~CDMCoreAgent();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

	//---------------------------------------------------------------------
	// CAgent Class overwritten methods 
	//---------------------------------------------------------------------
	//
	virtual void Reset();					// overwritten Reset

public:

	//---------------------------------------------------------------------
	//
	// DMCoreAgent class specific public methods
	//
	//---------------------------------------------------------------------

	//---------------------------------------------------------------------
	// Execution
	//---------------------------------------------------------------------

	// Executes the dialog task
	//
	void Execute();

	//---------------------------------------------------------------------
	// Method for performing an input pass (and related)
	//---------------------------------------------------------------------

	// Waits for and processes the next real world event
	void AcquireNextEvent();

	// Registers a customized binding filter
    //
    void RegisterBindingFilter(string sBindingFilterName, 
        TBindingFilterFunct bffFilter);

    //---------------------------------------------------------------------
	// Methods for accessing Interface-specific variables
	//---------------------------------------------------------------------

	int GetIntSessionID();

    //---------------------------------------------------------------------
	// Timeout related methods
	//---------------------------------------------------------------------

	// Sets and gets the timeout for the system 
	//
	void SetTimeoutPeriod(int iATimeoutPeriod);
	int GetTimeoutPeriod();

	// Sets the default timeout period for the system
	//
	void SetDefaultTimeoutPeriod(int iADefaultTimeoutPeriod);
	int GetDefaultTimeoutPeriod();

    //---------------------------------------------------------------------
	// Methods for accessing the nonunderstanding threshold
	//---------------------------------------------------------------------

    // Sets and gets the nonunderstanding threshold for the system
    //
    void SetNonunderstandingThreshold(float fANonunderstandingThreshold);
    float GetNonunderstandingThreshold();

    // Sets the default nonunderstanding threshold for the system
    //
    void SetDefaultNonunderstandingThreshold(float fANonuThreshold);
    float GetDefaultNonunderstandingThreshold();

	//---------------------------------------------------------------------
	// Signaling the need for running a focus claims phase
	//---------------------------------------------------------------------

    void SignalFocusClaimsPhase(bool bAFocusClaimsPhaseFlag = true);

	//---------------------------------------------------------------------
	// Signaling floor changes
	//---------------------------------------------------------------------

    void SetFloorStatus(TFloorStatus fsaFloorStatus);
    TFloorStatus GetFloorStatus();
	void SetFloorStatus(string sAFloorStatus);

	string FloorStatusToString(TFloorStatus fsAFloor);
	TFloorStatus StringToFloorStatus(string sAFloor);

    //---------------------------------------------------------------------
	// Access to various private fields
	//---------------------------------------------------------------------

    // Gets the number of concepts bound in the last input phase
    int LastTurnGetConceptsBound();

    // Returns true if there was a non-understanding in the last input
    // phase
    bool LastTurnNonUnderstanding();

	// Returns the number of consecutive non-understandings so far
    int GetNumberNonUnderstandings();

	// Returns the total number non-understandings so far
	int GetTotalNumberNonUnderstandings();

    //---------------------------------------------------------------------
	// Methods for execution and access to the execution stack, history, 
    // and binding history
	//---------------------------------------------------------------------

    // Plans a new agent for execution (pushes it on the execution stack)
	void ContinueWith(CAgent* paPusher, CDialogAgent* pdaDialogAgent);

    // Restarts an agent 
    void RestartTopic(CDialogAgent* pdaDialogAgent);

	// Registers a customized start over routine
	void RegisterCustomStartOver(TCustomStartOverFunct csoAStartOverFunct);

    // The start over function
	void StartOver();

	// Returns the agent on top of the execution stack (the focused one)
	CDialogAgent* GetAgentInFocus();

    // Returns the dialog task specification agent in focus (closest to the top 
    // of the execution stack
    CDialogAgent* GetDTSAgentInFocus();

    // Returns true if the agent is in focus
    bool AgentIsInFocus(CDialogAgent* pdaDialogAgent);

    // Returns the previously focused agent (previous with respect to the
    // incoming parameter)
	CDialogAgent* GetAgentPreviouslyInFocus(
        CDialogAgent* pdaDialogAgent = NULL);

    // Returns the previously focused dialog task specification agent 
    // (previous with respect to the incoming parameter)
	CDialogAgent* GetDTSAgentPreviouslyInFocus(
        CDialogAgent* pdaDialogAgent = NULL);

    // Returns the current main topic agent
	CDialogAgent* GetCurrentMainTopicAgent();

	// Returns true if the agent is currently active (i.e. it is 
	// somewhere on the execution stack)
	bool AgentIsActive(CDialogAgent* pdaDialogAgent);

    // Eliminates an agent from the execution stack
    void PopAgentFromExecutionStack(CDialogAgent* pdaADialogAgent);

    // Eliminates an agent from the execution stack, together with all
    // the subagents it has planned
    void PopTopicFromExecutionStack(CDialogAgent* pdaADialogAgent);

    // Eliminates all grounding agents from the execution stack
    void PopGroundingAgentsFromExecutionStack();

    // Returns the size of the binding history
    int GetBindingHistorySize();

    // Returns a reference to the binding history item
    const TBindingsDescr& GetBindingResult(int iBindingHistoryIndex);

	// Returns the number of the last input turn
	int GetLastInputTurnNumber();

	// Returns a description of the system action on a particular concept
	TSystemActionOnConcept GetSystemActionOnConcept(CConcept* pConcept);

	// Signal explicit and implicit confirms on a certain concept
	//
	void SignalExplicitConfirmOnConcept(CConcept* pConcept);
	void SignalImplicitConfirmOnConcept(CConcept* pConcept);
	void SignalUnplannedImplicitConfirmOnConcept(int iState, CConcept* pConcept);
	void SignalAcceptOnConcept(CConcept* pConcept);

    //---------------------------------------------------------------------
	// Methods for floor handling
	//---------------------------------------------------------------------

	

private:

	//---------------------------------------------------------------------
	// DMCoreManagerAgent private methods related to the execution stack
	//---------------------------------------------------------------------

	// Pops all the completed agents from the execution stack, and returns
    // the number popped
	int popCompletedFromExecutionStack();

	// Pops a dialog agent from the execution stack
	void popAgentFromExecutionStack(CDialogAgent *pdaADialogAgent,
                                    TStringVector& rvsAgentsEliminated);

    // Pops a dialog agent from the execution stack, together with all the
	// sub-agents (recursively) that it has planned for execution
	void popTopicFromExecutionStack(CDialogAgent *pdaADialogAgent,
                                    TStringVector& rvsAgentsEliminated);

    // Eliminates all grounding agents from the execution stack
	void popGroundingAgentsFromExecutionStack(TStringVector& rvsAgentsEliminated);

	//---------------------------------------------------------------------
	// DMCoreManagerAgent private methods related to the input pass
	//---------------------------------------------------------------------

	// Method for logging the concepts
	void dumpConcepts();

    // Methods for logging the execution stack
	//
    void dumpExecutionStack();
    string executionStackToString();
    string executionStackToString(TExecutionStack es);

	// Methods for computing the current system action 
	// 
	void clearCurrentSystemAction();
	void computeCurrentSystemAction();
	string systemActionToString(TSystemAction saASystemAction);
	string currentSystemActionToString();

	// Methods for assembling, logging, and broadcasting the expectation 
    // agenda
	//
	void assembleExpectationAgenda();
	void compileExpectationAgenda();
	void enforceBindingPolicies();
    void broadcastExpectationAgenda();
	string expectationAgendaToString();
    string expectationAgendaToBroadcastString();
	string expectationAgendaToBroadcastString(TExpectationAgenda eaBAgenda);

	// Method for logging the bindings description
	//
	string bindingsDescrToString(TBindingsDescr& rbdBindings);

	// J:  Updates the speech recognizer's configuration according to 
	//		the execution stack
	void updateInputLineConfiguration();

	// Assembles the list of focus claims
	//
	int assembleFocusClaims();

	// Binds the concepts from the input, according to an agenda
	//
    void bindConcepts(TBindingsDescr& rbdBindings);

	// Helper function which performs an actual binding to a concept
	//
	void performConceptBinding(string sSlotName, string sSlotValue,
							   float fConfidence, int iExpectationIndex,
							   bool bIsComplete);

    // Helper function that performs a binding through a customized
    // binding filter
    //
    void performCustomConceptBinding(int iExpectationIndex);

	// Helper function that performs the forced concept updates (if 
	// there are concepts that are being explicitly or implicitly 
	// confirmed and they are not being updated in the binding phase
	// then force an update on these concepts)
	//
	void performForcedConceptUpdates(TBindingsDescr& rbdBindings);

	// Process non-understandings
	//
	void processNonUnderstanding();

	// Analyze the need for and resolve focus shifts
	//
	void resolveFocusShift();

	// Rolls back to a previous dialog state (e.g. after
	// a user barge-in)
	//
	void rollBackDialogState(int iState);
};

#endif // __DMCOREAGENT_H__
