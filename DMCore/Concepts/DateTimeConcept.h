//-----------------------------------------------------------------------------
// 
// DATETIMECONCEPT.H - definition of the CDateTimeConcept class
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
//   [2002-11-26] (dbohus): defined this structure
// 
//-----------------------------------------------------------------------------

#ifndef __DATETIMECONCEPT_H__
#define __DATETIMECONCEPT_H__

#include "Concept.h"
#include "StructConcept.h"
#include "Utils/Utils.h"

//-----------------------------------------------------------------------------
// D: Definition of the CDateTime structured concept class; this class follows
//    the return specification of the DateTime4 Hub Galaxy agent
//-----------------------------------------------------------------------------

DEFINE_STRUCT_CONCEPT_TYPE( CDateTimeConcept,
    ITEM(month, CIntConcept)
    ITEM(day, CIntConcept)
    ITEM(year, CIntConcept)
    ITEM(valid_date, CBoolConcept)
    ITEM(start_time, CIntConcept)
    ITEM(end_time, CIntConcept)
    ITEM(valid_time, CBoolConcept)
    ITEM(past_date, CBoolConcept)
    ITEM(day_skip, CIntConcept)
    ITEM(weekday, CIntConcept)
    ITEM(past_time, CBoolConcept)
    ITEM(need_date, CBoolConcept)
    ITEM(date_exists, CBoolConcept)
    ITEM(ambiguous, CBoolConcept)
    ITEM(date_choice, CStringConcept)
)

#endif // __DATETIMECONCEPT_H__