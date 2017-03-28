#include "InteractionEventManagerAgent.h"
#include "DMCore/Core.h"
#include <iostream>
#include "Utils/Utils.h"
//#include "DMCore/Events/GalaxyInteractionEvent.h"
using namespace std;



CInteractionEventManagerAgent::CInteractionEventManagerAgent(string sAName,
                                  string sAConfiguration,
                                  string sAType):CAgent(sAName,sAConfiguration,sAType) {
 
}

CInteractionEventManagerAgent::~CInteractionEventManagerAgent() {}

CAgent* CInteractionEventManagerAgent::AgentFactory(string sAName,
                          string sAConfiguration) {
  return new CInteractionEventManagerAgent(sAName,sAConfiguration);
}

void CInteractionEventManagerAgent::Reset() {}

void CInteractionEventManagerAgent::Initialize() {
  pieLastEvent = NULL;
  pieLastInput = NULL;
}

bool CInteractionEventManagerAgent::HasEvent() {
  return  !qpieEventQueue.empty();
}

CInteractionEvent *CInteractionEventManagerAgent::GetNextEvent() {
  if(!qpieEventQueue.empty()){
    CInteractionEvent* pieNext = qpieEventQueue.front();
    qpieEventQueue.pop();

    pieLastEvent = pieNext;

    if(pieNext->GetType() == IET_USER_UTT_END) {
      pieLastInput = pieNext;
    }
    vpieEventHistory.push_back(pieNext);
    return pieNext;
  } else {
    return NULL;
  }
}
CInteractionEvent *CInteractionEventManagerAgent::GetLastEvent() {
  return pieLastEvent;
}

CInteractionEvent *CInteractionEventManagerAgent::GetLastInput() {
  return pieLastInput;
}

bool CInteractionEventManagerAgent::LastEventMatches(string sGrammarExpectation) {
  return pieLastEvent->Matches(sGrammarExpectation);
}

bool CInteractionEventManagerAgent::LastInputMatches(string sGrammarExpectation) {
  return pieLastInput->Matches(sGrammarExpectation);
}
bool CInteractionEventManagerAgent::LastEventIsComplete() {
  return pieLastEvent->IsComplete();
}

float CInteractionEventManagerAgent::GetLastEventConfidence() {
  return pieLastEvent->GetConfidence();
}
string CInteractionEventManagerAgent::GetValueForExpectation(string sGrammarExpectation) {
  return pieLastEvent->GetValueForExpectation(sGrammarExpectation); 
}

//现阶段简单的输入key-value键值对
STRING2STRING &CInteractionEventManagerAgent::analysisInput(string inputString) {
   STRING2STRING tmp;
   string slot,value;
   while (!inputString.empty()){
     SplitOnFirst(inputString,";",slot,inputString);
     SplitOnFirst(slot," ",slot,value);
     cout << "slot="<<slot<<",value="<<value<<endl;
     tmp.insert(map<string,string>::value_type(slot,value));
   }
   //SplitOnFirst(inputString, " ", slot, value);
   //tmp.insert(map<string,string>::value_type(slot,value));
   return tmp;
}
void CInteractionEventManagerAgent::WaitForEvent() {
  if (qpieEventQueue.empty()) {
    CInteractionEvent *newinput = new CInteractionEvent(IET_USER_UTT_END);
    //cout << newinput->GetType() <<endl;
    //cout<<"请输入您想说的话:"<<endl;
    string slot,value;
    //string command;
    //getline(cin,command);
    string command = *waitMessage(input_queue);
    Log(INPUTMANAGER_STREAM,"wait message form input_queue success");
    Log(INPUTMANAGER_STREAM,command);
    while (!command.empty()){
     SplitOnFirst(command,";",slot,command);
     SplitOnFirst(slot," ",slot,value);
     cout << "slot="<<slot<<",value="<<value<<endl;
     newinput->SetProperty(slot,value);
    }
    newinput->SetComplete();
    /*string slot,value;*/
    //SplitOnFirst(command, " ",slot,value);
    //newinput->SetProperty(slot,value);
    /*newinput->SetComplete();*/
    //cout << newinput->GetType() <<endl;
  /*  STRING2STRING news2s = analysisInput(command);*/
    /*newinput->SetProperties(news2s);*/
    qpieEventQueue.push(newinput);
  }
}
