// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Reference_subconparams.h
// Header file for the Reference (example) SubConnection Parameter Extension Set.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __REFERENCE_SUBCONPARAMS_H__
#define __REFERENCE_SUBCONPARAMS_H__

#include <es_sock.h>

//The ECOM plugin identifier for this SubConnection parameter extension.
//See the assisting *.rss file.
const TInt KSubConReferenceParamsUid = 0x102738C4;
//The sub-identifier for CSubConReferenceParamSet extension parameter set.
const TInt KSubConReferenceParamsType = 1;



class CSubConReferenceParamSet : public CSubConExtensionParameterSet
/** 
 An example/reference extension set of SubConnection Parameters.
 
 A typical client application wishing to use this extension set would
 look like this:
 ====================================================================
 void foo()
   {
    ...
    //Instantiate a parameter bundle - a generic container for subconnection
    //parameter sets.
	RSubConParameterBundle subconnParamBundle;
	
    //Since the bundle is about to hold heap-objects, must push onto
    //the cleanup stack.	
	CleanupClosePushL(subconnParams);

    //Instantiate a family - a subcontainer within a bundle. The subcontainer
    //should hold QoS subconnection parameter sets.
	CSubConParameterFamily* family = CSubConParameterFamily::NewL(subconnParamBundle, KSubConQoSFamily);

	//Specific parameter sets can be instantiated in either of the three modes (designating their semantics):
	//	enum TParameterSetType
    //   {
    //   ERequested = 0, //parameters requested by the application
    //   EAcceptable = 1,//parameters the application can accept (minimum)
	//	 EGranted = 2    //parameters granted by the network.
    //   }; (see ES_SOCK.H)
    //Consequently the bundle would usually contain ERequested and EAcceptable (or at least ERequested)
    //parameter sets.
    
    //Instantiating the ERequested Reference Extension:
	CSubConReferenceParamSet* subConRefRequestedParSet = CSubConReferenceParamSet::NewL(*family, CSubConParameterFamily::ERequested);

    //Instantiating the EAccepted Reference Extension:
	CSubConExtensionParameterSet* subConRefAcceptedParSet = CSubConReferenceParamSet::NewL(*family, CSubConParameterFamily::EAccepted);
        
    //At this point subconnParamBundle holds 2 instances (ERequested and EAcceptable) of CSubConReferenceParamSet.
    
   }
 ====================================================================

@publishedAll
@released since v9.2 
*/
{
public:
	inline static CSubConReferenceParamSet* NewL(CSubConParameterFamily& aFamily, CSubConParameterFamily::TParameterSetType aType);
	inline static CSubConReferenceParamSet* NewL();

	inline CSubConReferenceParamSet();

	inline TInt   GetDummyTIntParameter() const;
	inline const TName& GetDummyTNameParameter() const;	
	inline void   SetDummyTIntParameter(TInt aDummyTIntParameter);
	inline void   SetDummyTNameParameter(const TName& iDummyName);	

protected:

	DATA_VTABLE

protected:
	TInt	iDummyTIntParameter;
	TName	iDummyName;
	};





class CReferenceSubConnExtensionParamsFactory : public CBase
/** Factory used to create instances of the Reference SubConnection Parameter Extension Sets.

@internalComponent
@released since v9.2 
*/
	{
public:
	static CSubConExtensionParameterSet* NewL(TAny* aConstructionParameters);
	};

#include <networking/reference_subconparams.inl>

#endif // __REFERENCE_SUBCONPARAMS_H__
