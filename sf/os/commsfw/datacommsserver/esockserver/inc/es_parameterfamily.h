// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Generic parameter family bundles:
// Serialisable containers for structured custom data.
// FamilyBundle
// has 0..n
// Parameter Families
// each of which has 0..n
// ParameterSets
// 
//
// Note: Every effort should be made to ensure that the classes forming this API be kept consistent with each other
// When making changes to these classes also see the files es_parameterbundle.h, es_parameterfamily.h,
// ss_parameterfamilybundle.h
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef __ES_PARAMETERFAMILY_H
#define __ES_PARAMETERFAMILY_H

#include <comms-infras/es_parameterset.h>
#include <comms-infras/metacontainer.h>
#include <comms-infras/metatype.h>
#include <es_sock.h>

class CParameterSetContainer;
class CGenericParameterBundle;
class RParameterFamilyBundle;


/**
Wrapper round CParameterSetContainer presenting the following interface:
For each Parameter Type (Requested, Acceptable and Granted) it contains 0..N parameter sets.
@released
*/
class RParameterFamily
	{
	friend class RParameterFamilyBundle;
	friend class RParameterFamilyBundleC;

public:
	enum TParameterSetType
         {
         ERequested = 0,
         EAcceptable = 1,
		 EGranted = 2,
		 ENumValues = 3 // The number of values in this enum
         };

	RParameterFamily()
		: iContainer(NULL)
		{
		}

public:
	// creation & deserialising
	IMPORT_C void CreateL(RParameterFamilyBundle& aBundle, TUint32 aFamilyId);
	IMPORT_C void LoadL(RParameterFamilyBundle& aBundle, TPtrC8& aBuffer);

	// serialising
	IMPORT_C TUint Length() const;
	IMPORT_C TInt Store(TDes8& aDes) const;

	// check if we're pointing to something valid
	inline TBool IsNull() const;

	// destruction
	IMPORT_C void Destroy();

public:
	// adding parameter sets
	IMPORT_C TInt AddParameterSet(XParameterSetBase* aParameterSet, TParameterSetType aType);
	IMPORT_C void AddParameterSetL(XParameterSetBase* aParameterSet, TParameterSetType aType);
 
	// counting parameter sets
	IMPORT_C TInt CountParameterSets(TParameterSetType aType);
 
	// getting/finding parameter sets
	IMPORT_C XParameterSetBase* GetParameterSetAtIndex(TUint aIndex, TParameterSetType aType);
	IMPORT_C XParameterSetBase* FindParameterSet(Meta::STypeId aSetId, TParameterSetType aType);

	// getting/finding parameter sets (const)
	const XParameterSetBase* GetParameterSetAtIndex(TUint aIndex, TParameterSetType aType) const
		{return const_cast<RParameterFamily&>(*this).GetParameterSetAtIndex(aIndex,aType);}
	const XParameterSetBase* FindParameterSet(const Meta::STypeId& aSetId, TParameterSetType aType) const
		{return const_cast<RParameterFamily&>(*this).FindParameterSet(aSetId,aType);}

	// removing parameter set, and relinquish ownership to caller
	IMPORT_C TInt RemoveParameterSet(XParameterSetBase* aSetToRemove, TParameterSetType aType);
	IMPORT_C TInt RemoveAndDestroyParameterSet(XParameterSetBase* aSetToRemove, TParameterSetType aType);

	// ID of the parameter family
	IMPORT_C TUint32 Id() const;

	// clear all parameters of specified type
	IMPORT_C void ClearAllParameters(TParameterSetType aType);

	IMPORT_C TInt CountParameterSets();

	IMPORT_C TInt DeleteParameterSet(TInt aIndex);
	IMPORT_C void DeleteParameterSetL(TInt aIndex);

	IMPORT_C TInt ClearParameterSetPointer(TInt aIndex, TParameterSetType aType);
	
private:
	CParameterSetContainer* iContainer; // not necessarily owned.
	};


/**
Bundle of (i.e. container for) parameter families.
May contain and 0..N parameter families.
@released
*/
class RParameterFamilyBundleC
	{
	friend class RParameterFamily;
	friend class RParameterFamilyBundle;

public:
	// creation & deserialising
	IMPORT_C void CreateL();
	IMPORT_C void LoadL(TDesC8& aDes);
	IMPORT_C TInt Load(const TDesC8& aDes);

	// serialising
	IMPORT_C TUint Length() const;
	IMPORT_C TInt Store(TDes8& aDes) const;

	// check if we're pointing to something valid
	inline TBool IsNull() const;
	
	// destruction
	IMPORT_C void Destroy();

	// get/find families
	IMPORT_C const RParameterFamily GetFamilyAtIndex(TUint aIndex) const;
	IMPORT_C const RParameterFamily FindFamily(TUint32 aFamilyId) const;

	RParameterFamilyBundleC()
		: iBundle(NULL)
		{
		}

private:
	RParameterFamilyBundleC(const RParameterFamilyBundleC& aBundle);
	RParameterFamilyBundleC& operator=(const RParameterFamilyBundleC& aBundle);

private:
	CGenericParameterBundle* iBundle;
	};


/**
Bundle of (i.e. container for) parameter families.
May contain and 0..N parameter families.
@released
*/
class RParameterFamilyBundle : public RParameterFamilyBundleC
	{
public:
	// adding/creating families
	IMPORT_C RParameterFamily CreateFamilyL(TUint32 aFamilyId);
	
	// get/find families (non-const version)
	IMPORT_C RParameterFamily GetFamilyAtIndex(TUint aIndex);
	IMPORT_C RParameterFamily FindFamily(TUint32 aFamilyId);

	// clear all parameters
	IMPORT_C void ClearAllParameters(RParameterFamily::TParameterSetType aType);

	RParameterFamilyBundle()
		: RParameterFamilyBundleC()
		{
		}

	IMPORT_C TInt CountParameterFamilies() const;
	IMPORT_C TInt DeleteFamilyAtIndex(TInt aIndex);
	IMPORT_C void RemoveFamilyAtIndex(TInt aIndex);
	
private:
	RParameterFamilyBundle(const RParameterFamilyBundle& aBundle);
	RParameterFamilyBundle& operator=(const RParameterFamilyBundle& aBundle);
	};


#include <comms-infras/es_parameterfamily.inl>

#endif
// __ES_PARAMETERFAMILY_H




