// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_SS_TIER_MANAGER_H
#define SYMBIAN_SS_TIER_MANAGER_H

#include <comms-infras/ss_nodemessages_tiermanager.h>
#include <comms-infras/ss_mmnode.h>
#include <ss_fact.h>

namespace ESock
{

class CTierManagerFactoryContainer;

//
//CTierManagerBase
class CTierManagerFactoryBase;
class CTierManagerBase : public CBase, public ACommsFactoryNodeId, public ACFMMNodeBase,
						 public TCFTierManager
/** Base class for a meta connection provider.
*/
	{
	friend class CTierManagerFactoryBase;
public:
	/**
	@return the uid for the tier
	*/
	IMPORT_C TUid TierId() const;
	IMPORT_C TUid TierImplId()const;
	const RPointerArray<CMetaConnectionProviderBase>&  GetMetaConnectionProviders() const {return iMetaConnectionProviders;}
	IMPORT_C void AddMetaConnectionProviderL(const CMetaConnectionProviderBase* aMetaConnectionProvider);
	IMPORT_C void RemoveMetaConnectionProvider(const CMetaConnectionProviderBase* aMetaConnectionProvider);

public:
	virtual TBool HandleContentionL(CMetaConnectionProviderBase* aMcpr, Messages::TNodeId& aPendingCprId, TUint aPriority) = 0;
	virtual TBool IsUnavailableDueToContention(const CMetaConnectionProviderBase* aMetaConnectionProvider) const = 0;

	const Messages::TNodeId& Id () const
		{
		return ANodeId::Id ();	
		}

protected:
	IMPORT_C CTierManagerBase(CTierManagerFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap);
	IMPORT_C virtual ~CTierManagerBase();
	IMPORT_C virtual NetInterfaces::TInterfaceControl* FetchNodeInterfaceControlL(TInt aInterfaceId);
protected:
	RPointerArray<CMetaConnectionProviderBase> iMetaConnectionProviders;
	};

//
//Factories - base
class CTierManagerFactoryBase : public CCommsFactoryBase
/** Base class for selection resolver factories. Specifies the interface
    that selection resolver factories must implement.
*/
	{
	friend class CTierManagerFactoryContainer;
public:
	/**
	Destructor
	*/
	IMPORT_C virtual ~CTierManagerFactoryBase();
	IMPORT_C TUid TierTypeId() const;

protected:
	/**
	@param aFactoryUid Uid of the factory being created
	@param aParentContainer Factory container within which the factory is being created
	*/
	IMPORT_C CTierManagerFactoryBase(TUid aTierTypeId, TUid aFactoryUid, CTierManagerFactoryContainer& aParentContainer);

	/**
	Create object
	@param aQuery Parameters with which to create object
	@return The node id of the created object
	*/
	IMPORT_C virtual ACommsFactoryNodeId* DoCreateObjectL(TFactoryQueryBase& aQuery);

	/**
	Find or create object
	@param aQuery Parameters with which to find or create the object
	@return The node id of the found or created object
	*/
	IMPORT_C virtual ACommsFactoryNodeId* DoFindOrCreateObjectL(TFactoryQueryBase& aQuery);	
protected:
	TUid iTierTypeId;
	};
	
} //namespace ESock



#endif //SYMBIAN_SS_TIER_MANAGER_H


