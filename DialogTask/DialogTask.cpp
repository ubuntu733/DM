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
DEFINE_AGENCY(CRoot,
	IS_MAIN_TOPIC()
        DEFINE_CONCEPTS(
            STRING_USER_CONCEPT(query,"expl"))
	DEFINE_SUBAGENTS(
		SUBAGENT(RootWelcome, CRootWelcome, "")
                SUBAGENT(RequestQuery,CRequestQuery,"")
		SUBAGENT(FlightRoot, CFlightRoot, "")
                SUBAGENT(MusicRoot,CMusicRoot,"")
		SUBAGENT(RootLogout, CRootLogout, ""))
	SUCCEEDS_WHEN(COMPLETED(FlightRoot) || COMPLETED(MusicRoot))
)
DEFINE_INFORM_AGENT( CRootWelcome,
    PROMPT("inform|您好，欢迎使用对话系统")
)
DEFINE_REQUEST_AGENT( CRequestQuery,
    REQUEST_CONCEPT(query)
 //   PRECONDITION(!IS_TRUE("flight_query.startDate"))
    PROMPT(" request|请问您要使用什么业务？")
    GRAMMAR_MAPPING(" [query]")
//    ON_COMPLETION(std::cout<<C("query").GetHyp(0)->ValueToString()<<std::endl;)
  //  SUCCEEDS_WHEN(IS_TRUE("flight_query.startDate"))
)
DEFINE_AGENCY(CFlightRoot,
        PRECONDITION(C("query")==CString("flight"))
//	IS_MAIN_TOPIC()
        DEFINE_CONCEPTS(                                                                                                                        
                STRING_SYSTEM_CONCEPT(startDate)
                STRING_SYSTEM_CONCEPT(startLoc)
                STRING_SYSTEM_CONCEPT(endLoc))
	DEFINE_SUBAGENTS(
		SUBAGENT(Welcome, CWelcome, "")
                SUBAGENT(RequestDate, CRequestDate, "")
		SUBAGENT(RequestStartLoc, CRequestStartLoc, "")
		SUBAGENT(RequestEndLoc, CRequestEndLoc, "")
		SUBAGENT(InformResults, CInformResults, "")
		SUBAGENT(ExecuteFlight, CExecuteFlight, ""))
        TRIGGERED_BY(UPDATED(startDate) || UPDATED(startLoc) || UPDATED(endLoc))
	SUCCEEDS_WHEN(COMPLETED(ExecuteFlight))
)
DEFINE_INFORM_AGENT( CWelcome,
    PROMPT("inform|您好，欢迎使用航班查询系统")
)
DEFINE_REQUEST_AGENT( CRequestDate,
    REQUEST_CONCEPT(startDate)
 //   PRECONDITION(!IS_TRUE("flight_query.startDate"))
    PROMPT(" request|请问您要查询什么时间的航班？")
    GRAMMAR_MAPPING(" [startDate]")
//    ON_COMPLETION(std::cout<<C("startDate").GetTopHyp()->ValueToString()<<std::endl;)
  //  SUCCEEDS_WHEN(IS_TRUE("flight_query.startDate"))
)
DEFINE_REQUEST_AGENT( CRequestStartLoc,
    REQUEST_CONCEPT(startLoc)
//    PRECONDITION(!IS_TRUE("flight_query.RequestStartLoc"))
    PROMPT("request|请说起始地点。")
    GRAMMAR_MAPPING(" [startLoc]")
//    SUCCEEDS_WHEN(IS_TRUE("flight_query.startLoc"))
)
DEFINE_REQUEST_AGENT( CRequestEndLoc,
    REQUEST_CONCEPT(endLoc)
 //   PRECONDITION(!IS_TRUE("flight_query.RequestEndLoc"))
    PROMPT("request|请说目的地。")
    GRAMMAR_MAPPING(" [endLoc]")
 //   SUCCEEDS_WHEN(IS_TRUE("flight_query.endLoc"))
)
DEFINE_INFORM_AGENT( CInformResults,
    PROMPT("end|您好，您好正在为您查询，请稍等。")
)
DEFINE_EXECUTE_AGENT( CExecuteFlight,
    CALL("inform|对不起，未查到相应的航班")
)


//////////////////////////////////////
DEFINE_AGENCY(CMusicRoot,
        PRECONDITION(C("query")==CString("music"))
        DEFINE_CONCEPTS(
            STRING_SYSTEM_CONCEPT(song)
            STRING_SYSTEM_CONCEPT(singer)
            STRING_SYSTEM_CONCEPT(time))
//	IS_MAIN_TOPIC()
	DEFINE_SUBAGENTS(
		SUBAGENT(MusicWelcome, CMusicWelcome, "")
		SUBAGENT(RequestSinger, CRequestSinger, "")
		SUBAGENT(RequestSong, CRequestSong, "")
//		SUBAGENT(RequestTime, CRequestTime, "")
		SUBAGENT(InformMusic, CInformMusic, "")
		SUBAGENT(ExecuteMusic, CExecuteMusic, ""))
        TRIGGERED_BY(UPDATED(song) || UPDATED(singer))
	SUCCEEDS_WHEN(COMPLETED(ExecuteMusic) && COMPLETED(RequestSinger) && COMPLETED(RequestSong))
)

DEFINE_INFORM_AGENT( CMusicWelcome,
    PROMPT("inform|您好，欢迎使用歌曲搜索系统")
)

DEFINE_REQUEST_AGENT( CRequestSinger,
    REQUEST_CONCEPT(singer)
 //   PRECONDITION(!IS_TRUE("flight_query.startDate"))
    PROMPT(" request|请问您需要查询谁的歌曲？")
    GRAMMAR_MAPPING(" [singer]")
  //  SUCCEEDS_WHEN(IS_TRUE("flight_query.startDate"))
)
DEFINE_REQUEST_AGENT( CRequestSong,
    REQUEST_CONCEPT(song)
//    PRECONDITION(!IS_TRUE("flight_query.RequestStartLoc"))
    PROMPT("request|请说歌名。")
    GRAMMAR_MAPPING(" [song]")
//    SUCCEEDS_WHEN(IS_TRUE("flight_query.startLoc"))
)
/**
DEFINE_REQUEST_AGENT( CRequestTime,
    REQUEST_CONCEPT(time)
 //   PRECONDITION(!IS_TRUE("flight_query.RequestEndLoc"))
    PROMPT("request|请说时间。")
    GRAMMAR_MAPPING(" [time]>time")
 //   SUCCEEDS_WHEN(IS_TRUE("flight_query.endLoc"))
)
**/
DEFINE_INFORM_AGENT( CInformMusic,
    PROMPT("end|您好，您好正在为您查询，请稍等。")
)
DEFINE_EXECUTE_AGENT( CExecuteMusic,
    CALL("inform|对不起，未查到相应的歌曲")
)


DEFINE_INFORM_AGENT( CRootLogout,
    PROMPT("end|感谢您的使用。")
)
//-----------------------------------------------------------------------------
//
// AGENT DECLARATIONS
//
//-----------------------------------------------------------------------------
DECLARE_AGENTS(
	DECLARE_AGENT(CRoot)
                DECLARE_AGENT(CRootWelcome)
                DECLARE_AGENT(CFlightRoot)
                        DECLARE_AGENT(CWelcome)
                        DECLARE_AGENT(CRequestQuery)
                        DECLARE_AGENT(CRequestDate)
                        DECLARE_AGENT(CRequestStartLoc)
                        DECLARE_AGENT(CRequestEndLoc)
                        DECLARE_AGENT(CInformResults)
                        DECLARE_AGENT(CExecuteFlight) 
                DECLARE_AGENT(CMusicRoot)
                        DECLARE_AGENT(CMusicWelcome)
                        DECLARE_AGENT(CRequestSinger)
                        DECLARE_AGENT(CRequestSong)
  //                      DECLARE_AGENT(CRequestTime)
                        DECLARE_AGENT(CInformMusic)
                        DECLARE_AGENT(CExecuteMusic)
                DECLARE_AGENT(CRootLogout)
                
            
)

//-----------------------------------------------------------------------------
// DIALOG TASK ROOT DECLARATION
//-----------------------------------------------------------------------------
DECLARE_DIALOG_TASK_ROOT(Root, CRoot, "")
