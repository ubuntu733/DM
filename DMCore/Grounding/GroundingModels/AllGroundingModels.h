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
// ALLGROUNDINGMODELS.H   - header which includes all the grounding models
//                          known to the system
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
//   [2003-03-24] (dbohus): created this file
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __ALLGROUNDINGMODELS_H__
#define __ALLGROUNDINGMODELS_H__

#include "GroundingModel.h"
#include "GMConcept.h"
#include "GMRequestAgent.h"
#include "GMRequestAgent_HandCrafted.h"
#include "GMRequestAgent_NumNonu.h"
#include "GMRequestAgent_Experiment.h"
#include "GMRequestAgent_LR.h"

// D: define the RavenClaw-specific grounding models that are registered
#define ALL_GROUNDING_MODEL_TYPES\
    GROUNDING_MODEL_TYPE(request_default, CGMRequestAgent::GroundingModelFactory)\
    GROUNDING_MODEL_TYPE(request_handcrafted, CGMRequestAgent_HandCrafted::GroundingModelFactory)\
    GROUNDING_MODEL_TYPE(concept_default, CGMConcept::GroundingModelFactory)\
    GROUNDING_MODEL_TYPE(request_lr, CGMRequestAgent_LR::GroundingModelFactory)\
    GROUNDING_MODEL_TYPE(request_numnonu, CGMRequestAgent_NumNonu::GroundingModelFactory)\
    GROUNDING_MODEL_TYPE(request_experiment, CGMRequestAgent_Experiment::GroundingModelFactory)

#endif // __ALLGROUNDINGMODELS_H__