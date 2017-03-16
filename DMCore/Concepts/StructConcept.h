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
// STRUCTCONCEPT.H - definition of the CStructConcept class
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
//   [2004-06-02] (dbohus):  added definition of pOwnerConcept, concepts now
//                            check with parent if unclear if they are
//                            grounded
//   [2004-05-30] (dbohus):  changed definition of IsGrounded
//   [2004-05-19] (dbohus):  major makeover on concepts, introduced frame 
//                            concepts and structure concepts which support 
//                            grounding
//   [2004-05-15] (dbohus):  started working on this, starting from the
//                            previous StructConcept class
// 
//-----------------------------------------------------------------------------

#pragma once
#ifndef __STRUCTCONCEPT_H__
#define __STRUCTCONCEPT_H__

#include "Concept.h"
#include "Utils/Utils.h"

//-----------------------------------------------------------------------------
// CStructHyp class - this class keeps references to pairs of 
//   atomic structure instances and their corresponding confidence scores
//-----------------------------------------------------------------------------

// D: forward declaration of CStructConcept class
class CStructConcept;

// type definition for the list of items, kept as a hash for fast access
typedef map<string, CConcept*, less<string>, allocator<CConcept *> > TItemMap;

class CStructHyp : public CHyp {

    // declare the CStructConcept class as a friend
    //
    friend class CStructConcept;

protected:

    //---------------------------------------------------------------------
    // Protected member variables 
	//---------------------------------------------------------------------
	
    // pointer to the ItemMap for the structure
	TItemMap* pItemMap;

	// pointer to the svItems for the structure
	TStringVector* psvItems;

    // an index indicating which hypothesis is captured
	int	iHypIndex;

public:
	
    //---------------------------------------------------------------------
    // Constructors and destructors
	//---------------------------------------------------------------------
	
	CStructHyp(TItemMap* pAItemMap = NULL, TStringVector* psvAItems = NULL, 
		int iAHypIndex = -1, bool bComplete = true);

    CStructHyp(CStructHyp& rAStructHyp);

    //---------------------------------------------------------------------
    // CStructHyp specific methods
	//---------------------------------------------------------------------

    // Set the HypIndex
    void SetHypIndex(int iAHypIndex);

    //---------------------------------------------------------------------
	// Overwritten, CStructHyp specific virtual functions
	//---------------------------------------------------------------------

    // Assignment operator from reference
	virtual CHyp& operator = (CHyp& rAHyp);

    // Set the confidence score
    virtual void SetConfidence(float fAConfidence);

    // Comparison operators
    //
    virtual bool operator == (CHyp& rAHyp);
    virtual bool operator < (CHyp& rAHyp);
    virtual bool operator > (CHyp& rAHyp);
    virtual bool operator <= (CHyp& rAHyp);
    virtual bool operator >= (CHyp& rAHyp);
    
    // Indexing operator
    //
    virtual CHyp* operator [](string sItem);

	// String conversion functions
    //
	virtual string ValueToString();
	virtual string ToString();
	virtual void FromString(string sString);
};

//-----------------------------------------------------------------------------
// CStructConcept class - this is the structured concept class. It holds a 
//                        structure which can undergo grounding and acts as
//                        a single units. The structure is implemented as a 
//                        collection of concepts, and the hypotheses are 
//                        "virtualized", i.e. they are represented by 
//                        corresponding slices in the subsumed concepts. 
//                        The structure always maintains the same number of
//                        hypotheses in the underlying concepts
//-----------------------------------------------------------------------------

// define the cardinality of the struct class of concepts
#define DEFAULT_STRUCT_CARDINALITY 1000

class CStructConcept: public CConcept {

protected:

    //---------------------------------------------------------------------
	// protected members 
    //---------------------------------------------------------------------

	// the list of items (pointers to concepts) held by the structure as 
    // a hash
	TItemMap ItemMap;

	// string vector holding the list of structure elements (this is used
	//  for accessing the order of the elements)
	TStringVector svItems;

public:
	
    //---------------------------------------------------------------------
	// Constructors and destructors
    //---------------------------------------------------------------------
	//
	CStructConcept(string sAName = "NONAME", 
		TConceptSource csAConceptSource = csUser);
    ~CStructConcept();

	//---------------------------------------------------------------------
    // CStructConcept class concept manipulation
	//---------------------------------------------------------------------

    // Create the elements of the structure (to be overwritten by derived
    // classes)
	virtual void CreateStructure();

    // Destroy the elements of the structure
	virtual void DestroyStructure();

    //---------------------------------------------------------------------
    // Overwritten methods for overall concept manipulation 
	//---------------------------------------------------------------------
	
    // overwritten operators
    //
    virtual bool operator < (CConcept& rAConcept);
    virtual bool operator > (CConcept& rAConcept);
    virtual bool operator <= (CConcept& rAConcept);
    virtual bool operator >= (CConcept& rAConcept);
	virtual CConcept& operator [] (string sAItemName);

    // Clone the concept
	virtual CConcept* Clone(bool bCloneHistory = true);

	//---------------------------------------------------------------------
	// Overwritten methods implementing various types of updates in the 
	// naive probabilistic update scheme
	//---------------------------------------------------------------------

	// assign from another string
	virtual void Update_NPU_AssignFromString(void* pUpdateData);

	// update partial from another string
	virtual void Update_PartialFromString(void* pUpdateData);

	//---------------------------------------------------------------------
	// Overwritten methods implementing conversions to string format
	//---------------------------------------------------------------------
    
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

    // clear the current set of hypotheses for the concept
    virtual void ClearCurrentHypSet();

	//-----------------------------------------------------------------------------
	// Overwritten methods for partial hypotheses manipulation
	//-----------------------------------------------------------------------------

	// add a new hypothesis to the set of partial hyps (return the index)
    virtual int AddNewPartialHyp();

	// add a null hypothesis to the set of partial hyps (return the index)
    virtual int AddNullPartialHyp();

    //---------------------------------------------------------------------
	// Overwritten methods for concept history manipulation
	//---------------------------------------------------------------------

	// set the history concept flag
	virtual void SetHistoryConcept(bool bAHistoryConcept = true);

protected:

	//---------------------------------------------------------------------
	// Protected virtual methods implementing various types of updates
	//---------------------------------------------------------------------

	// updates the concept from a string representation
	virtual void updateFromString(void* pUpdateData, string sUpdateType);

};

//-----------------------------------------------------------------------------
// Macros used for defining new struct concepts. A sample definition is given 
// below:
//
//   DEFINE_STRUCT_CONCEPT_TYPE( CProfileConcept, 
//		ITEM(name, CStringConcept, none)
//		ITEM(age, CIntConcept, none)
//		ITEM(preferred_hotel, CStringConcept, expl_impl)
//   )
// 
// Note that nested structs are allowed, but not recursive ones
//-----------------------------------------------------------------------------


// D: Macro for defining a derived structure concept class
#define DEFINE_STRUCT_CONCEPT_TYPE(StructConceptTypeName, OTHER_CONTENTS) \
	class StructConceptTypeName: public CStructConcept {\
	public:\
		StructConceptTypeName(string aName = "NONAME",\
							 TConceptSource csAConceptSource = csUser):\
			CStructConcept(aName, csAConceptSource) {\
			CreateStructure();\
		};\
		virtual CConcept* EmptyClone() {\
			return new StructConceptTypeName;\
		};\
		OTHER_CONTENTS\
	};\

// D: Macro for defining the ITEMS in a structure
#define ITEMS(ITEM_DEFINITIONS) \
		virtual void CreateStructure() {\
			ITEM_DEFINITIONS\
		};\

// D: Macro for defining items in the derived structure class
#define ITEM(Name, ConceptType)\
		ItemMap.insert(TItemMap::value_type(#Name,\
    new ConceptType(sName + "." + #Name, csConceptSource))); \
    ItemMap[#Name]->SetOwnerDialogAgent(pOwnerDialogAgent);\
    ItemMap[#Name]->SetOwnerConcept(this);\
		svItems.push_back(#Name);\


#endif // __STRUCTCONCEPT_H__
