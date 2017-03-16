#pragma once
#ifndef __INTERACTIONEVENTMANAGERAGENT_H__
#define __INTERACTIONEVENTMANAGERAGENT_H__


#include "DMCore/Events/InteractionEvent.h"
#include "Utils/Utils.h"
#include "DMCore/Agents/Agent.h"

#define IET_DIALOG_STATE_CHANGE	"dialog_state_change"
#define IET_USER_UTT_START	"user_utterance_start"
#define IET_USER_UTT_END	"user_utterance_end"
#define IET_PARTIAL_USER_UTT "partial_user_utterance"
#define IET_SYSTEM_UTT_START	"system_utterance_start"
#define IET_SYSTEM_UTT_END	"system_utterance_end"
#define IET_SYSTEM_UTT_CANCELED	"system_utterance_canceled"
#define IET_FLOOR_OWNER_CHANGES "floor_owner_changes"
#define IET_SESSION "session"
#define IET_GUI "gui"

class CInteractionEventManagerAgent: public CAgent {
 private:
  queue <CInteractionEvent*, list<CInteractionEvent*> > qpieEventQueue;

	vector <CInteractionEvent*> vpieEventHistory;

	CInteractionEvent *pieLastEvent;

	// pointer to most recently processed user input
	CInteractionEvent *pieLastInput;

 public:

	//---------------------------------------------------------------------
	// Constructor and destructor
	//---------------------------------------------------------------------
	//
	// Default constructor
	CInteractionEventManagerAgent(string sAName, 
					   string sAConfiguration = "",
					   string sAType = "CAgent:CInteractionEventManagerAgent");

	// Virtual destructor
	virtual ~CInteractionEventManagerAgent();

	// static function for dynamic agent creation
	static CAgent* AgentFactory(string sAName, string sAConfiguration);

	//---------------------------------------------------------------------
	// CAgent Class overwritten methods 
	//---------------------------------------------------------------------
	//
	virtual void Reset();	// overwritten Reset

	virtual void Initialize();	// overwritten Initialize

 public:

	//---------------------------------------------------------------------
	// CInteractionEventManagerAgent class specific public methods
	//---------------------------------------------------------------------

	// Indicates if the queue contains at least one event
	bool HasEvent();

	// Dequeues one event from the unprocessed event queue
	CInteractionEvent *GetNextEvent();

	// Returns a pointer to the last event/user input processed
	CInteractionEvent *GetLastEvent();
	CInteractionEvent *GetLastInput();

	// Checks if the last event matches a given expectation
	bool LastEventMatches(string sGrammarExpectation);

	// Checks if the current input matches a given expectation
	bool LastInputMatches(string sGrammarExpectation);

    // Checks if the current event is a complete or partial event
  bool LastEventIsComplete();

    // Returns the confidence score on the current input
  float GetLastEventConfidence();

	// Returns the string value corresponding to a given expectation from 
	// the current input 
	string GetValueForExpectation(string sGrammarExpectation);

	// Waits for an interaction event to arrive from the Interaction Manager
  // 需要修改的方法
	void WaitForEvent();

	// Used by the Galaxy Bridge to signal that a new event has arrived
//	void SignalInteractionEventArrived();
//	需要添加的方法，根据从命令行输入字符串来解析指令
//	待实现
  STRING2STRING &analysisInput(string inputString);

};

#endif // __INTERACTIONEVENTMANAGERAGENT_H__

