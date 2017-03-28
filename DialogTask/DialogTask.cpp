#include "DialogTask.h"
CORE_CONFIGURATION(// declare the grounding model types to be used
    USE_ALL_GROUNDING_MODEL_TYPES

	// declare the grounding actions to be used
	USE_ALL_GROUNDING_ACTIONS(""))
/**
DEFINE_FRAME_CONCEPT_TYPE( CFlightQuery,
    ITEMS(
        STRING_ITEM(startDate)
        STRING_ITEM(startLoc)
        STRING_ITEM(endLoc)
    )
)
**/
DEFINE_AGENCY(CFlightRoot,
	IS_MAIN_TOPIC()
	DEFINE_SUBAGENTS(
		SUBAGENT(Welcome, CWelcome, "")
		SUBAGENT(Task, CTask, "")
		SUBAGENT(Logout, CLogout, ""))
	SUCCEEDS_WHEN(COMPLETED(Logout))
)
DEFINE_INFORM_AGENT( CWelcome,
    PROMPT("inform|您好，欢迎使用航班查询系统")
)
DEFINE_AGENCY(CTask,
	DEFINE_CONCEPTS(
            STRING_SYSTEM_CONCEPT(startDate)
            STRING_SYSTEM_CONCEPT(startLoc)
            STRING_SYSTEM_CONCEPT(endLoc))
//	    CUSTOM_SYSTEM_CONCEPT(FlightQuery, CFlightQuery))
	DEFINE_SUBAGENTS(
		SUBAGENT(RequestDate, CRequestDate, "")
		SUBAGENT(RequestStartLoc, CRequestStartLoc, "")
		SUBAGENT(RequestEndLoc, CRequestEndLoc, "")
		SUBAGENT(InformResults, CInformResults, "")
		SUBAGENT(ExecuteFlight, CExecuteFlight, ""))
	SUCCEEDS_WHEN(COMPLETED(RequestDate) && 
				  COMPLETED(RequestStartLoc) &&
				  COMPLETED(RequestEndLoc) && 
				  COMPLETED(InformResults) && 
				  COMPLETED(ExecuteFlight))
	)
DEFINE_REQUEST_AGENT( CRequestDate,
    REQUEST_CONCEPT(startDate)
 //   PRECONDITION(!IS_TRUE("flight_query.startDate"))
    PROMPT(" request|请问您要查询什么时间的航班？")
    GRAMMAR_MAPPING(" [startDate]>startDate")
  //  SUCCEEDS_WHEN(IS_TRUE("flight_query.startDate"))
)
DEFINE_REQUEST_AGENT( CRequestStartLoc,
    REQUEST_CONCEPT(startLoc)
//    PRECONDITION(!IS_TRUE("flight_query.RequestStartLoc"))
    PROMPT("request|请说起始地点。")
    GRAMMAR_MAPPING(" [startLoc]>startLoc")
//    SUCCEEDS_WHEN(IS_TRUE("flight_query.startLoc"))
)
DEFINE_REQUEST_AGENT( CRequestEndLoc,
    REQUEST_CONCEPT(endLoc)
 //   PRECONDITION(!IS_TRUE("flight_query.RequestEndLoc"))
    PROMPT("request|请说目的地。")
    GRAMMAR_MAPPING(" [endLoc]>endLoc")
 //   SUCCEEDS_WHEN(IS_TRUE("flight_query.endLoc"))
)
DEFINE_INFORM_AGENT( CInformResults,
    PROMPT("inform|您好，您好正在为您查询，请稍等。")
)
DEFINE_EXECUTE_AGENT( CExecuteFlight,
    CALL("inform|对不起，未查到相应的航班")
)

DEFINE_AGENCY(CLogout,
	IS_MAIN_TOPIC()
	DEFINE_SUBAGENTS(SUBAGENT(InformLogout, CInformLogout, ""))
	SUCCEEDS_WHEN(COMPLETED(InformLogout))
	)
DEFINE_INFORM_AGENT( CInformLogout,
    PROMPT("end|查询结束，感谢您的使用。")
)

//-----------------------------------------------------------------------------
//
// AGENT DECLARATIONS
//
//-----------------------------------------------------------------------------
DECLARE_AGENTS(
	DECLARE_AGENT(CFlightRoot)
        DECLARE_AGENT(CWelcome)               
        DECLARE_AGENT(CTask)
            DECLARE_AGENT(CRequestDate)
            DECLARE_AGENT(CRequestStartLoc)
            DECLARE_AGENT(CRequestEndLoc)
            DECLARE_AGENT(CInformResults)
            DECLARE_AGENT(CExecuteFlight)
        DECLARE_AGENT(CLogout)
            DECLARE_AGENT(CInformLogout)
            
)

//-----------------------------------------------------------------------------
// DIALOG TASK ROOT DECLARATION
//-----------------------------------------------------------------------------
DECLARE_DIALOG_TASK_ROOT(FlightRoot, CFlightRoot, "")
