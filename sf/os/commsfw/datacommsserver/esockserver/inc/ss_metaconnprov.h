// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_SS_METACONNPROV_H
#define SYMBIAN_SS_METACONNPROV_H

#define SYMBIAN_NETWORKING_UPS

#include <comms-infras/ss_commsprov.h>
#include <comms-infras/ss_mmcommsprov.h>
#include <comms-infras/es_availability.h>
#include <comms-infras/ss_tiermanager.h>
#include <comms-infras/ss_nodemessages_mcpr.h>
#include <ss_fact.h>

class CESockIniData;

#ifdef SYMBIAN_NETWORKING_UPS
namespace CommsDat
	{
	class CMDBSession;
	class CCDAccessPointRecord;
	}
#endif

namespace ESock
{

const TInt KMetaConnectionFactoryInterfaceUid = 0x10274546;

//
//RNodeAvailabilityProviderInterface
class RNodeAvailabilityProviderInterface : public Messages::RNodeInterface
	{
public:
	void SetAvailabilityStatus(const TAvailabilityStatus& aAvailabilityStatus)
		{
		iAvailabilityStatus = aAvailabilityStatus;
		}

	const TAvailabilityStatus& AvailabilityStatus() const
		{
		return iAvailabilityStatus;
		}

protected:
    TAvailabilityStatus iAvailabilityStatus;
	};

//
//RMetaServiceProviderInterface
class RMetaServiceProviderInterface : public RNodeAvailabilityProviderInterface
	{
public:
	RMetaServiceProviderInterface(const TProviderInfo& aProviderInfo)
	:	iProviderInfo(aProviderInfo)
		{
		}

	const TProviderInfo& ProviderInfo() const
	    {
	    return iProviderInfo;
	    }

protected:
    TProviderInfo iProviderInfo;
	};


//
//CMetaConnectionProviderBase
class TSelectionPrefs;
class CMetaConnectionProviderFactoryBase;
class CMetaConnectionProviderBase : public CMMCommsProviderBase, public TCFMcpr
/** Base class for a meta connection provider.
*/
	{
#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	friend CMetaConnectionProviderBase* __x86gcc_meta_connection_provider_base_cast(Factories::AFactoryObject* aFactoryObject);
	friend const CMetaConnectionProviderBase* __x86gcc_meta_connection_provider_base_cast(const Factories::AFactoryObject* aFactoryObject);
    friend CMetaConnectionProviderBase* __x86gcc_meta_connection_provider_base_cast(Messages::ANode* aNode);
    friend CMetaConnectionProviderBase& __x86gcc_meta_connection_provider_base_cast(Messages::ANode& aNode);
#elif !defined(__GCCXML__)
	friend CMetaConnectionProviderBase* Messages::mnode_cast<CMetaConnectionProviderBase>(Messages::ANode* aNode);
	friend CMetaConnectionProviderBase& Messages::mnode_cast<CMetaConnectionProviderBase>(Messages::ANode& aNode);
#endif

public:
	IMPORT_C const TProviderInfo& ProviderInfo() const;
	IMPORT_C void SetProviderInfo(const TProviderInfo& aConnectionInfo);
	IMPORT_C void ConstructL();

	IMPORT_C RMetaServiceProviderInterface* FindServiceProvider(TUint aAccessPoint);

	IMPORT_C void SetTierManagerL(CTierManagerBase* aTierManager);

	/**
	  Destruction blocking exists to ensure that a provider will not delete itself while it is still in use.
	  This scenario occurs when FindOrCreate finds a mcpr which is just about to delete itself (because it
	  has lost all clients). If this blocking semaphore didn't exist, it would be possible for a deleted
	  mcpr to be passed to the control client, causing a panic.
	 */
	inline TBool BlockingDestroy();
	/**
	   Increment blocking destroy semaphore. This should occur immediately after the the selector has found the
	   provider, generally in the FindOrCreateProviderL method of the selector. */
	inline void IncrementBlockingDestroy();
	/**
	   Decrement blocking destroy semaphore. Occurs when a client joins the provider. Never decrements past zero.
	*/
	inline void DecrementBlockingDestroy();

	/**
	   Meta providers (unlike other providers) are allowed to have write access to their AccessPointConfig
	   structure provided they know how to do it safely.
	*/
	inline RMetaExtensionContainerC& AccessPointConfig();

	inline CTierManagerBase* GetTierManager() const;
	
protected:
	IMPORT_C CMetaConnectionProviderBase(CMetaConnectionProviderFactoryBase& aFactory,
                                         const TProviderInfo& aProviderInfo,
                                         const MeshMachine::TNodeActivityMap& aActivityMap);
	IMPORT_C virtual Messages::RNodeInterface* NewClientInterfaceL(const Messages::TClientType& aClientType, TAny* aClientInfo = NULL);
	IMPORT_C virtual ~CMetaConnectionProviderBase();

#ifdef SYMBIAN_NETWORKING_UPS
	IMPORT_C virtual void ShowAccessPointRecordL(CommsDat::CMDBSession* aSession, CommsDat::CCDAccessPointRecord* aApRec);
#endif //SYMBIAN_NETWORKING_UPS

	void SetAccessPointConfigL();

protected:
	TProviderInfo iProviderInfo;
	TInt iBlockingDestroy;
	CTierManagerBase* iTierManager;
	};

void CMetaConnectionProviderBase::IncrementBlockingDestroy()
	{
	++iBlockingDestroy;
	}

void CMetaConnectionProviderBase::DecrementBlockingDestroy()
	{
	if (iBlockingDestroy > 0)
		--iBlockingDestroy;
	}

TBool CMetaConnectionProviderBase::BlockingDestroy()
	{
	return (iBlockingDestroy > 0);
	}

RMetaExtensionContainerC& CMetaConnectionProviderBase::AccessPointConfig()
	{
	return CCommsProviderBase::AccessPointConfig();
	}

CTierManagerBase* CMetaConnectionProviderBase::GetTierManager() const
	{
	return iTierManager;
	}

//
class CMetaConnectionFactoryContainer;

//
//Factories - base
class CMetaConnectionProviderFactoryBase : public CCommsFactoryBase
/** Base class for all meta connection provider factories. Specifies the interface
    that all meta connection provider factories must implement. It also provides the
	ECOM framework (via CCommsFactoryBase) to load meta connection provider factories
	from other Dynamic Linked Libraries.
*/
	{
public:
	IMPORT_C virtual ~CMetaConnectionProviderFactoryBase();

protected:
	IMPORT_C CMetaConnectionProviderFactoryBase(TUid aFactoryUid, CMetaConnectionFactoryContainer& aParentContainer);
	/** Override this to create a new instance of CMetaConnectionProviderBase

	@return Pointer to the newly created provider
	@exception Leaves in out of memory conditions */
	IMPORT_C virtual ACommsFactoryNodeId* DoCreateObjectL(TFactoryQueryBase& aQuery);

	IMPORT_C void DoPostCreationL(ACommsFactoryNodeId* aObject,TFactoryQueryBase& aQuery);
	};

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast...

	enum
		{
		EX86GCCMetaConnectionFactoryContainer = 2
		};

	inline CMetaConnectionProviderBase* __x86gcc_meta_connection_provider_base_cast(Messages::ANode* aNode)
    	{
	    return static_cast<CMetaConnectionProviderBase*>(aNode);
    	}

	inline CMetaConnectionProviderBase& __x86gcc_meta_connection_provider_base_cast(Messages::ANode& aNode)
    	{
	    return static_cast<CMetaConnectionProviderBase&>(aNode);
    	}

	inline CMetaConnectionProviderBase* __x86gcc_meta_connection_provider_base_cast(Factories::AFactoryObject* aFactoryObject)
    	{
	    return EX86GCCMetaConnectionFactoryContainer == static_cast<CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<CMetaConnectionProviderBase*>(aFactoryObject) : NULL;
    	}

	inline const CMetaConnectionProviderBase* __x86gcc_meta_connection_provider_base_cast(const Factories::AFactoryObject* aFactoryObject)
    	{
	    return EX86GCCMetaConnectionFactoryContainer == static_cast<CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const CMetaConnectionProviderBase*>(aFactoryObject) : NULL;
    	}
#endif
} //namespace ESock


#ifdef __X86GCC__
	namespace Messages
	{
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template <>
	inline ESock::CMetaConnectionProviderBase* mnode_cast<ESock::CMetaConnectionProviderBase>(Messages::ANode* aNode)
		{
		return ESock::__x86gcc_meta_connection_provider_base_cast(aNode);
		}

	template <>
	inline ESock::CMetaConnectionProviderBase& mnode_cast<ESock::CMetaConnectionProviderBase>(Messages::ANode& aNode)
		{
		return ESock::__x86gcc_meta_connection_provider_base_cast(aNode);
		}
	}
#endif

#endif //SYMBIAN_SS_METACONNPROV_H


