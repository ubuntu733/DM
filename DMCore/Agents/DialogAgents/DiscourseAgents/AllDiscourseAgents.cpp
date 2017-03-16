//-----------------------------------------------------------------------------
// 
// ALLDISCOURSEAGENTS.CPP - header which includes all the discourse agents
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
//   [2002-04-16] (dbohus): created this file
// 
//-----------------------------------------------------------------------------

#include "AllDiscourseAgents.h"

// D: this function registers all the discourse agents' types into the
//    registry
void RegisterAllDiscourseAgents() {
	// register the repeat agent
	RegisterRepeatAgent();
	// register the various default help agents
	RegisterHelpAgent();
	// register the default suspend agency
	RegisterSuspendAgency();
}
