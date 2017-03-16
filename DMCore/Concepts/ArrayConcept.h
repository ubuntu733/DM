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
// ARRAYCONCEPT.H - definition of the CArrayConcept class
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
//   [2004-12-06] (antoine): fixed inconsistencies so that an array is always
//                           considered as an atomic concept when reopened,
//                           tested for availability, etc.
//   [2004-11-11] (jsherwan):added support for insertAt(i, x) to insert at the 
//                            end of an array setting i = n
//   [2004-06-15] (dbohus):  fixed bug in IsUndergoingGrounding
//   [2004-06-02] (dbohus):  added definition of pOwnerConcept, concepts now
//                            check with parent if unclear if they are
//                            grounded
//   [2004-05-30] (dbohus):  changed definition of IsGrounded
//   [2004-05-19] (dbohus):  major makeover on concepts, introduced frame 
//                            concepts and structure concepts which support 
//                            grounding
//   [2004-05-15] (dbohus):  cleaned up concepts code, added Update as well as
//                            various types of update methods
//   [2004-04-20] (jsherwan):added InsertAt()
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
//   [2003-04-28] (antoine): added the == operator
//   [2003-04-14] (dbohus):  added support for OwnerDialogAgent
//   [2003-04-12] (dbohus):  fixed bug in Restore
//   [2003-04-10] (dbohus):  added Restore on concepts
//   [2003-04-03] (dbohus):  added support for concept grounding models
//   [2003-04-01] (dbohus):  added support for iTurnLastUpdated
//   [2003-03-24] (antoine): implemented MergedHistory
//   [2003-03-19] (dbohus):  eliminated ValueToString, transformed 
//                            ValueToStringWithConf into ValueSetToString
//   [2003-03-13] (antoine): modified *ValueToString, SetValueFromString and 
//                            UpdateValueFromString so that when elements are 
//                            atomic, they do not need to be surrounded by 
//                            braces '{' and '}' (it is now one element per 
//                            line in this case)
//   [2003-03-13] (antoine): added ModeValueToString
//   [2003-01-29] (dbohus):  added equality and comparison operators
//   [2002-12-09] (dbohus):  major revamp on concepts: added support for 
//                            reopening concepts and maintaing a history of
//                            previous values 
//   [2002-12-01] (dbohus):  added SetValueFromString and UpdateValueFromString
//                            functions on concepts
//   [2002-11-26] (dbohus):  fixed bug in assign operator when assigning to 
//                            self
//   [2002-05-25] (dbohus):  deemed preliminary stable version 0.5
//   [2002-03-23] (dbohus):  started working on this
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __ARRAYCONCEPT_H__
#define __ARRAYCONCEPT_H__

#include "Concept.h"
#include "Utils/Utils.h"

//-----------------------------------------------------------------------------
// CArrayConcept class - this is an array of concepts class. This is 
//						 actually an abstract class, and particular array
//                       concepts can be implemented by deriving this class.
//                       A macro for easy definition of derived classes is
//                       also provided below.
//-----------------------------------------------------------------------------

class CArrayConcept: public CConcept {

protected:

    //---------------------------------------------------------------------
	// protected members (note that, although inherited, ValConfSet is not 
	// actively used by this class. The CArrayConcept class merely stores
	// a vector of concepts
    //---------------------------------------------------------------------

	// the vector of concepts stored 
	vector <CConcept*, allocator <CConcept*> > ConceptArray;

	// the grounding model specification for each of the elements
	string sElementGroundingModelSpec;

public:
	
	//---------------------------------------------------------------------
	// Constructors and destructors
	//---------------------------------------------------------------------
	//
	CArrayConcept(string sAName = "NONAME",
				  TConceptSource csAConceptSource = csUser);
	virtual ~CArrayConcept();

    //---------------------------------------------------------------------
	// Virtual pure method for creating a new array element. This method
	// is to be overwritten by derived classes. Macros are provided below
	// for easily defining array concepts
    //---------------------------------------------------------------------
	//
	virtual CConcept* CreateElement() = 0;

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
	virtual CConcept& operator [](int iIndex);
	virtual CConcept& operator [](string sIndex);
	
    // Clone the concept
	virtual CConcept* Clone(bool bCloneHistory = true);

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

	//---------------------------------------------------------------------
	// Overwritten methods implementing various types of updates in the 
	// Calista belief updating scheme
	//---------------------------------------------------------------------

	// update with another concept
    virtual void Update_Calista_UpdateWithConcept(void* pUpdateData);

    // update the value of a concept, so that it collapses to the mode
    // with probability 0.95 (this is used mostly on reopens)
    virtual void Update_Calista_CollapseToMode(void* pUpdateData);

	//---------------------------------------------------------------------
	// Overwritten methods implementing various flags (state information) 
    // on the concept
	//---------------------------------------------------------------------
    
	// checks if the array is updated
	virtual bool IsUpdated();

	// check if the concept is currently undergoing grounding
    virtual bool IsUndergoingGrounding();

	//---------------------------------------------------------------------
	// Overwritten methods implementing conversions to string format
	//---------------------------------------------------------------------
    
	// Generate a string representation of the grounded hypothesis (the top
	// one, if the concept is grounded)
	virtual string GroundedHypToString();

	// Generate a string representation of the top hypothesis
	virtual string TopHypToString();

    // Generate a string representation of the set of hypotheses
	virtual string HypSetToString();

	//---------------------------------------------------------------------
    // Overwritten methods providing access to concept name
	//---------------------------------------------------------------------

    // set the concept name
    virtual void SetName(string sAName);

	//---------------------------------------------------------------------
    // Overwritten methods providing access to the owner dialog agent
	//---------------------------------------------------------------------
    
    // set the owner dialog agent
    virtual void SetOwnerDialogAgent(CDialogAgent* pADialogAgent);

	//---------------------------------------------------------------------
    // Overwritten methods related to the grounding model
	//---------------------------------------------------------------------
    
    // create a grounding model for this concept
    virtual void CreateGroundingModel(string sGroundingModelSpec);

    // declare the list of grounding models that this concept subsumes
    virtual void DeclareGroundingModels(
        TGroundingModelPointersVector& rgmpvModels,
        TGroundingModelPointersSet& rgmpsExclude);

    // declare the list of concepts that this concept subsumes
    virtual void DeclareConcepts(
	    TConceptPointersVector& rcpvConcepts, 
		TConceptPointersSet& rcpsExclude);

    //---------------------------------------------------------------------
    // Methods related to signaling the concept change
	//---------------------------------------------------------------------

	// enable / disable concept change notifications
	virtual void SetChangeNotification(
		bool bAChangeNotification = true);

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

	//---------------------------------------------------------------------
    // Overwriten methods providing access to turn last updated information
	//---------------------------------------------------------------------

    // return the turn when the concept was last updated
    virtual int GetTurnLastUpdated();

	//---------------------------------------------------------------------
	// Overwritten methods for concept history manipulation
	//---------------------------------------------------------------------

	// set the history concept flag
	virtual void SetHistoryConcept(bool bAHistoryConcept = true);

	//---------------------------------------------------------------------
	// Overwritten methods that are array-specific 
	//---------------------------------------------------------------------

    // returns the size of the array
    virtual int GetSize();

    // deletes an element at a given index in the array
	virtual void DeleteAt(unsigned int iIndex);   

    // inserts an element at a give index in the array
	virtual void InsertAt(unsigned int iIndex, CConcept &rAConcept);   

};

//-----------------------------------------------------------------------------
// Macro used for defining new array concepts. An sample definition is given 
// below:
//
//   DEFINE_ARRAY_CONCEPT_TYPE(CIntConcept, CIntArrayConcept)
// 
// This defines a new type (CIntArrayConcept) which is an array of integer
//  concepts.
//-----------------------------------------------------------------------------

#define DEFINE_ARRAY_CONCEPT_TYPE(ElementaryConceptType, ArrayConceptTypeName) \
	class ArrayConceptTypeName: public CArrayConcept {\
	public:\
		ArrayConceptTypeName(string aName = "NONAME",\
							 TConceptSource csAConceptSource = csUser): \
            CArrayConcept(aName, csAConceptSource) {\
		}\
		virtual CConcept* CreateElement() {\
            CConcept* pConcept;\
            pConcept = new ElementaryConceptType("NONAME", csConceptSource);\
            pConcept->CreateGroundingModel(sElementGroundingModelSpec);\
            return pConcept;\
		}\
        virtual CConcept* EmptyClone() {\
            return new ArrayConceptTypeName;\
        }\
	};\


#endif // __ARRAYCONCEPT_H__