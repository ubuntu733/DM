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
// CORE.H   - header which includes all the core headers
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
//   [2002-04-11] (dbohus): created this file
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __CORE_H__
#define __CORE_H__


#define WM_DLGBASE			(WM_USER+100)		// the base id for the messages
#define WM_DLGMAX			(WM_DLGBASE+5)		// the max id for the messages

#define WM_WAITINPUT		(WM_DLGBASE)		// wait for input
#define WM_GALAXYCALL		(WM_DLGBASE+1)		// do a call to a galaxy module
#define WM_OAACALL			(WM_DLGBASE+1)		// do a call to an OAA module
#define WM_DIALOGFINISHED	(WM_DLGBASE+2)		// finish the dialog
#define WM_ACTIONFINISHED	(WM_DLGBASE+3)		// notify that action was 
												//  completed
#define WM_WAITINTERACTIONEVENT	(WM_DLGBASE+4)	// wait for an interaction event
#define WM_GALAXYACTIONCALL		(WM_DLGBASE+5)	// do a call to a galaxy module

#define ACTION_SUCCESS 1						// the action was completed 
												//	successfully
#define ACTION_FAILED 0							// the action terminated with a 
				

#define RCP_DMI_VERBOSITY "dmi_verbosity"
#define RCP_LOGGED_STREAMS "logged_streams"
#define RCP_DISPLAYED_STREAMS "displayed_streams"
#define RCP_EXIT_ON_FATAL_ERROR "exit_on_fatal_error"
#define RCP_GROUNDING_POLICIES "grounding_policies"
#define RCP_GROUNDING_POLICIES_FILE "grounding_policies_file"
#define RCP_DIALOG_STATES_FILE "dialog_states_file"
#define RCP_GROUNDING_MANAGER_CONFIGURATION "grounding_manager_configuration"
#define RCP_LOG_DIR "log_dir"
#define RCP_LOG_PREFIX ""
#define RCP_DEFAULT_TIMEOUT "default_timeout"
#define RCP_DEFAULT_NONUNDERSTANDING_THRESHOLD "default_nonunderstanding_threshold"
#define RCP_SERVER_NAME "server_name"
#define RCP_SERVER_PORT "server_port"
// Include all the headers for the core 
#include "DMCore.h"
#include "Log.h"
#define TIMEOUT_ELAPSED "[turn_timeout:timeout]"
#define VOID_INPUT "VoidInput"
#define START_SESSION "StartSession"
#define TERMINATE_SESSION "TerminateSession"
#define NON_UNDERSTANDING "NonUnderstanding"
#endif // __CORE_H__
