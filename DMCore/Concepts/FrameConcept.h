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
// FRAMECONCEPT.H - definition of the CFrameConcept class. This class 
//                  implements a frame, i.e. a collection of attribute-values.
//                  In contrast to the CStructConcept class, this class does
//                  not undergo grounding
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
//   [2006-06-15] (antoine): merged Calista belief updating functions from
//                           RavenClaw1
//   [2006-01-01] (antoine): branched for RavenClaw2, added support for
//							 partial hypotheses
//   [2004-06-15] (dbohus):  fixed bug in IsUndergoingGrounding
//   [2004-06-02] (dbohus):  added definition of pOwnerConcept, concepts now
//                            check with parent if unclear if they are
//                            grounded
//   [2004-05-30] (dbohus):  changed definition of IsGrounded
//   [2004-05-19] (dbohus):  major makeover on concepts, introduced frame 
//                            concepts and structure concepts which support 
//                            grounding
//   [2004-05-18] (dbohus):  renamed this to the CFrameConcept class
//   [2004-05-15] (dbohus):  cleaned up concepts code, added Update as well as
//                            various types of update methods
//   [2004-03-29] (dbohus):  fixed some problems in the copy constructor
//   [2004-04-25] (dbohus):  added WaitingConveyance flag and functionality
//   [2004-04-18] (dbohus):  renamed MergedHistory to 
//                            CreateMergedHistoryConcept and added a 
//                            MergeHistory function which merges the history
//                            up into the current value for a concept
//   [2004-04-17] (dbohus):  added support for declaring the grounding models
//                            subsumed by the concept
//   [2004-04-16] (dbohus):  removed grounding models from constructors and 
//                            added them as a separate method
//   [2003-04-28] (antoine): implemented the == operator
//   [2003-04-14] (dbohus):  added support for OwnerDialogAgent
//   [2003-04-12] (dbohus):  fixed bug in Restore
//   [2003-04-10] (dbohus):  added Restore on concepts
//   [2003-04-03] (dbohus):  added support for concept grounding models
//   [2003-04-02] (antoine): overloaded GetTurnLastUpdated and 
//							 GetTurnsSinceUpdated from CConcept
//   [2003-04-01] (dbohus):  added support for iTurnLastUpdated
//   [2003-03-19] (dbohus):  eliminated ValueToString, transformed 
//                            ValueToStringWithConf into ValueSetToString
//   [2003-03-13] (antoine): added ModeValueToString
//   [2003-01-29] (dbohus):  added equality and comparison operators
//   [2002-12-09] (dbohus):  major revamp on concepts: added support for 
//                            reopening concepts and maintaing a history of
//                            previous values 
//   [2002-12-01] (dbohus):  added SetValueFromString and UpdateValueFromString
//                            functions on concepts
//   [2002-11-26] (dbohus):  fixed bug in assign operator when assigning to self
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-02-10] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __FRAMECONCEPT_H__
#define __FRAMECONCEPT_H__

#include "StructConcept.h"
#include "Utils/Utils.h"

//-----------------------------------------------------------------------------
// CFrameConcept class - this is the frame concept class. It holds a 
//                       record of concepts of various types. It serves as a
//                       hierarchy basis for classes which implement 
//                       user-specified frame concepts. Frame concepts do not
//                       undergo grounding, and hypotheses at the frame level
//                       are not defined
//-----------------------------------------------------------------------------

class CFrameConcept: public CStructConcept {

public:
	
	//---------------------------------------------------------------------
	// Constructors and destructors 
	//---------------------------------------------------------------------
    //    
	CFrameConcept(string sAName = "NONAME", 
        TConceptSource csAConceptSource = csUser);
	CFrameConcept(CStructConcept& rAFrameConcept);
	~CFrameConcept();

    //---------------------------------------------------------------------
	// Methods for overall concept manipulation
	//---------------------------------------------------------------------

    // a set of virtual operators on concepts
    //
    virtual bool operator == (CConcept& rAConcept);
    virtual bool operator < (CConcept& rAConcept);
    virtual bool operator > (CConcept& rAConcept);
    virtual bool operator <= (CConcept& rAConcept);
    virtual bool operator >= (CConcept& rAConcept);
	
    // Clear the contents of the concept
    virtual void Clear();

    // Clear the current contents of the concept
    virtual void ClearCurrentValue();

	//---------------------------------------------------------------------
	// Overwritten methods implementing various types of updates in the 
	// naive probabilistic update scheme
	//---------------------------------------------------------------------

    // assign from another string
	virtual void Update_NPU_AssignFromString(void* pUpdateData);

	// update with another concept
    virtual void Update_NPU_UpdateWithConcept(void* pUpdateData);

    // update the value of a concept, so that it collapses to the mode
    // with probability 0.95 (this is used mostly on reopens)
    virtual void Update_NPU_CollapseToMode(void* pUpdateData);

	// update the partial value of a concept (i.e. value obtained through
	// a partial event such as an utterance still in progress)
	virtual void Update_PartialFromString(void* pUpdateDate);

	//---------------------------------------------------------------------
	// Overwritten methods implementing various types of updates in the 
	// calista belief updating scheme
	//---------------------------------------------------------------------

	// update with another concept
    virtual void Update_Calista_UpdateWithConcept(void* pUpdateData);

    // update the value of a concept, so that it collapses to the mode
    virtual void Update_Calista_CollapseToMode(void* pUpdateData);

	//---------------------------------------------------------------------
	// Overwritten methods implementing various flags (state information) 
    // on the concept
	//---------------------------------------------------------------------
    
    // check if the concept is updated (there is at least a current value)
    virtual bool IsUpdated();

    // check if the concept is updated and grounded
    virtual bool IsUpdatedAndGrounded();

	// check if the concept is available (we have a current value or a 
    // history value)
	virtual bool IsAvailable();

    // check that the concept is available and grounded
	virtual bool IsAvailableAndGrounded();

	// check if the concept is currently undergoing grounding
    virtual bool IsUndergoingGrounding();
    

	//---------------------------------------------------------------------
	// Ovewritten methods implementing conversions to string format
	//---------------------------------------------------------------------
    
	// Generate a string representation of the grounded hypothesis (the top
	// one, if the concept is grounded)
	virtual string GroundedHypToString();

	// Generate a string representation of the top hypothesis
	virtual string TopHypToString();

    // Generate a string representation of the set of hypotheses
	virtual string HypSetToString();

	//---------------------------------------------------------------------
    // Overwritten methods related to the grounding model
	//---------------------------------------------------------------------
    
    // create a grounding model for this concept
    virtual void CreateGroundingModel(string sGroundingModelSpec);

    //---------------------------------------------------------------------
    // Methods related to invalidating a concept
	//---------------------------------------------------------------------

	// set the invalidated flag
	virtual void SetInvalidatedFlag(bool bAInvalidated = true);

	// get the invalidated flag
	virtual bool GetInvalidatedFlag();

    //---------------------------------------------------------------------
    // Methods related to restoring a concept for grounding
	//---------------------------------------------------------------------

	// set the restored for grounding flag
	virtual void SetRestoredForGroundingFlag(
	    bool bARestoredForGrounding = true);
	
	// get the restored for grounding flag
	virtual bool GetRestoredForGroundingFlag();

	//---------------------------------------------------------------------
    // Overwritten methods related to the current hypotheses set and belief 
    // manipulation
	//---------------------------------------------------------------------

    // factory method for hypotheses
    virtual CHyp* HypFactory();

    // add a hypothesis to the current set (return the index)
    virtual int AddHyp(CHyp* pAHyp);

	// add a new hypothesis to the current set (return the index)
    virtual int AddNewHyp();

    // add a NULL hypothesis to the current set (return the index)
    virtual int AddNullHyp();

    // sets a hypothesis into a location (location has to contain a 
    // null-hyp and the operation actually copies)
    virtual void SetHyp(int iIndex, CHyp* pHyp);

    // sets a hypothesis into a location
    virtual void SetNullHyp(int iIndex);

	// deletes a hypothesis
	virtual void DeleteHyp(int iIndex);

	// return the hypothesis at a given index
	virtual CHyp* GetHyp(int iIndex);

    // finds the index of a given hypothesis
    virtual int GetHypIndex(CHyp* pHyp);

    // return the confidence of a certain hypothesis (specified by index)
	virtual float GetHypConfidence(int iIndex);

    // return the top hypothesis
    virtual CHyp* GetTopHyp();

    // return the top hyp index
    virtual int GetTopHypIndex();

    // return the confidence score of the top hypothesis
    virtual float GetTopHypConfidence();

    // check if a hypothesis is valid (confidence score > 0; valus is not 
	// NULL)
	virtual bool IsValidHyp(int iIndex);

    // return the total number of hypotheses for a concept (including 
    // NULLs)
	virtual int GetNumHyps();

    // return the number of valid hypotheses for a concept
	virtual int GetNumValidHyps();

    // clear the current set of hypotheses for the concept
    virtual void ClearCurrentHypSet();

    // copies the current hypset from another concept
    virtual void CopyCurrentHypSetFrom(CConcept& rAConcept);
    
    // clears the explicitly confirmed and disconfirmed hyps
    // 
    virtual void ClearExplicitlyConfirmedHyp();    
    virtual void ClearExplicitlyDisconfirmedHyp();    

	//---------------------------------------------------------------------
    // Overwritten methods providing access to turn last updated 
    // information
	//---------------------------------------------------------------------

    // return the turn when the concept was last updated
    virtual int GetTurnLastUpdated();

    //---------------------------------------------------------------------
	// Overwritten methods for concept history manipulation
	//---------------------------------------------------------------------

    // reopen the concept (i.e. push the current value into the history)
    virtual void ReOpen();

    // clear the history of a concept
    virtual void ClearHistory();

    // construct a new concept by merging over the history of this concept
    virtual CConcept* CreateMergedHistoryConcept();

    // merge history of the concept back into the current value
    virtual void MergeHistory();

protected:

	//---------------------------------------------------------------------
	// Protected virtual methods implementing various types of updates
	//---------------------------------------------------------------------

	void updateFromString(void* pUpdateData, string sUpdateType);

};

//-----------------------------------------------------------------------------
// Macros used for defining new frame concepts. The definitions are similar to
// the ones for structures. A sample definition is given below:
//
//   DEFINE_FRAME_CONCEPT_TYPE( CProfileConcept, 
//		ITEM(name, CStringConcept, none)
//		ITEM(age, CIntConcept, none)
//		ITEM(preferred_hotel, CStringConcept, generic)
//   )
// 
// Note that nested frames are allowed, but not recursive ones
//-----------------------------------------------------------------------------


// D: Macro for defining a derived structure concept class
#define DEFINE_FRAME_CONCEPT_TYPE(FrameConceptTypeName, OTHER_CONTENTS) \
	class FrameConceptTypeName: public CFrameConcept {\
	public:\
		FrameConceptTypeName(string aName = "NONAME",\
							 TConceptSource csAConceptSource = csUser):\
			CFrameConcept(aName, csAConceptSource) {\
			CreateStructure();\
		};\
		virtual CConcept* EmptyClone() {\
			return new FrameConceptTypeName;\
		};\
		OTHER_CONTENTS\
	};\


#endif // __FRAMECONCEPT_H__