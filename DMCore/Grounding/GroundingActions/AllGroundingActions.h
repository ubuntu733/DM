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
// ALLGROUNDINGACTIONS.H   - header which includes all the grounding actions
//                           known to the system
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
//   [2003-02-12] (dbohus): created this file
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __ALLGROUNDINGACTIONS_H__
#define __ALLGROUNDINGACTIONS_H__

#include "GroundingAction.h"
#include "GANoAction.h"
#include "GAAccept.h"
#include "GAExplicitConfirm.h"
#include "GAImplicitConfirm.h"
#include "GAAskRepeat.h"
#include "GAAskRephrase.h"
#include "GAFailRequest.h"
#include "GARepeatPrompt.h"
#include "GAExplainMore.h"
#include "GAFullHelp.h"
#include "GAWhatCanISay.h"
#include "GANotifyNonunderstanding.h"
#include "GAYieldTurn.h"
#include "GAAskStartOver.h"
#include "GASpeakLessLoudAndReprompt.h"
#include "GAAskShortAnswerAndReprompt.h"
#include "GAAskShortAnswerAndWhatCanISay.h"
#include "GAInteractionTips.h"
#include "GAGiveUp.h"
#include "GAMoveOn.h"

// D: a define for all the grounding actions
#define ALL_GROUNDING_ACTIONS(Configuration)\
    GROUNDING_ACTION(NO_ACTION, NO_ACTION, Configuration)\
	GROUNDING_ACTION(ACCEPT, ACCEPT, Configuration)\
    GROUNDING_ACTION(EXPL_CONF, EXPLICIT_CONFIRM, Configuration)\
    GROUNDING_ACTION(IMPL_CONF, IMPLICIT_CONFIRM, Configuration)\
    GROUNDING_ACTION(ASK_REPEAT, ASK_REPEAT, Configuration)\
    GROUNDING_ACTION(NOTIFY_AND_ASK_REPEAT, ASK_REPEAT, Configuration + ", notify=true")\
    GROUNDING_ACTION(ASK_REPHRASE, ASK_REPHRASE, Configuration)\
    GROUNDING_ACTION(NOTIFY_AND_ASK_REPHRASE, ASK_REPHRASE, Configuration + ", notify=true")\
    GROUNDING_ACTION(REPEAT_PROMPT, REPEAT_PROMPT, Configuration)\
	GROUNDING_ACTION(NOTIFY_AND_REPEAT_PROMPT, REPEAT_PROMPT, Configuration + ", notify=true")\
    GROUNDING_ACTION(EXPLAIN_MORE, EXPLAIN_MORE, Configuration)\
	GROUNDING_ACTION(NOTIFY_AND_EXPLAIN_MORE, EXPLAIN_MORE, Configuration + ", notify=true")\
    GROUNDING_ACTION(FULL_HELP, FULL_HELP, Configuration)\
	GROUNDING_ACTION(NOTIFY_AND_FULL_HELP, FULL_HELP, Configuration + ", notify=true")\
    GROUNDING_ACTION(WHAT_CAN_I_SAY, WHAT_CAN_I_SAY, Configuration)\
    GROUNDING_ACTION(TERSE_WHAT_CAN_I_SAY, WHAT_CAN_I_SAY, Configuration + ", terse=true")\
	GROUNDING_ACTION(NOTIFY_AND_WHAT_CAN_I_SAY, WHAT_CAN_I_SAY, Configuration + ", notify=true")\
	GROUNDING_ACTION(NOTIFY_AND_TERSE_WHAT_CAN_I_SAY, WHAT_CAN_I_SAY, Configuration + ", terse=true, notify=true")\
	GROUNDING_ACTION(TIPS_AND_WHAT_CAN_I_SAY, WHAT_CAN_I_SAY, Configuration + ", tips=true")\
    GROUNDING_ACTION(NOTIFY_NONUNDERSTANDING, NOTIFY_NONUNDERSTANDING, Configuration)\
    GROUNDING_ACTION(YIELD_TURN, YIELD_TURN, Configuration)\
    GROUNDING_ACTION(FAIL_REQUEST, FAIL_REQUEST, Configuration)\
    GROUNDING_ACTION(ASK_STARTOVER, ASK_STARTOVER, Configuration)\
    GROUNDING_ACTION(SPEAK_LESS_LOUD_AND_REPROMPT, SPEAK_LESS_LOUD_AND_REPROMPT, Configuration)\
    GROUNDING_ACTION(ASK_SHORT_ANSWER_AND_REPROMPT, ASK_SHORT_ANSWER_AND_REPROMPT, Configuration)\
    GROUNDING_ACTION(ASK_SHORT_ANSWER_AND_WHAT_CAN_I_SAY, ASK_SHORT_ANSWER_AND_WHAT_CAN_I_SAY, Configuration)\
    GROUNDING_ACTION(INTERACTION_TIPS, INTERACTION_TIPS, Configuration)\
    GROUNDING_ACTION(GIVE_UP, GIVE_UP, Configuration)\
    GROUNDING_ACTION(MOVE_ON, MOVE_ON, Configuration)

#endif // __ALLGROUNDINGACTIONS_H__