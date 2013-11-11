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
//

/**
 @file
 @internalComponent 
*/

#if !defined(__SS_CONNPROV_H__)
#define __SS_CONNPROV_H__

#include <e32def.h>
#include <elements/metadata.h>
#include <ss_select.h>
#include <elements/factorynotify.h>
#include <es_sock.h> //TNifProgressBuf & co
#include <es_prot.h> //TConnStartType &co
#include <comms-infras/ss_mmcommsprov.h>
#include <comms-infras/ss_metaconnprov.h> //For TProviderInfo which may need to be moved to a different header in case of a circular dep.
#include <comms-infras/ss_nodemessages_cpr.h>
#include <comms-infras/ss_nodemessages_internal.h>

#include <comms-infras/ss_common.h>
#include <comms-infras/ss_commsprov.h>
#include <comms-infras/ss_fact_internal.h>

namespace ESock
{

const TInt KConnInfoPart = 8; //TODO: delete with TConnectionInfo

class CConnection;
class CCommsFactoryBase;
class CConnectionProviderBase;
class CSubConnectionProviderBase;

const TInt KProtocolAny	= 0xffffff1; //cooked-up constant for any protocol
const TInt KConnectionFactoryInterfaceUid = 0x102070EE;

class XConnectionProviderInfoQuery : public Factories::MFactoryQuery
/** Class implementing MCommsFactoryQuery to find a connection provider by
 * using iProviderInfo

@internalTechnology
*/
	{
public:
	explicit XConnectionProviderInfoQuery(const TProviderInfo aProviderInfo)
	:	iProviderInfo(aProviderInfo)
		{
		}
	explicit XConnectionProviderInfoQuery(const TUint32 aAPid)
	    :   iAPid(aAPid)
	        {
	        }

	IMPORT_C virtual TMatchResult Match(Factories::TFactoryObjectInfo& aProviderInfo);

	TProviderInfo iProviderInfo;
	TUint32 iAPid;

protected:
	explicit XConnectionProviderInfoQuery()
	:	iProviderInfo(TProviderInfo::NullProviderInfo())
		{
		}
	};

class CConnectionProviderFactoryBase;
class CConnectionProviderBase;
class CConnectionFactoryContainer : public CCommsFactoryContainer
/** Container for connection provider factories.
Note that there is a 1-1 mapping between aProtocolId and aFactoryId at the moment

@internalTechnology
@released Since 9.1 */
	{
	friend class CConnectionProviderFactoryBase;
#ifdef __X86GCC__
protected:
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend CConnectionProviderBase* __x86gcc_connection_provider_base_cast(Factories::AFactoryObject* aFactoryObject);
    friend const CConnectionProviderBase* __x86gcc_connection_provider_base_cast(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = EConnectionFactoryContainer
        };
#elif defined(__GCCXML__)
public:
    enum
        {
        EId = EConnectionFactoryContainer
        };
#else
protected:
    friend CConnectionProviderBase* Factories::factoryobject_cast<CConnectionProviderBase>(Factories::AFactoryObject* aFactoryObject);
    friend const CConnectionProviderBase* Factories::factoryobject_cast<const CConnectionProviderBase>(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = EConnectionFactoryContainer
        };
#endif

public:
	IMPORT_C static CConnectionFactoryContainer* NewL();

	CConnectionProviderFactoryBase* Factory( TInt aIndex ) const;
    IMPORT_C static CConnectionFactoryContainer& Singleton();
	virtual ~CConnectionFactoryContainer();

protected:
	CConnectionFactoryContainer();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	};

class CConnectionProviderBase;
class CConnectionProviderFactoryBase : public CCommsFactoryBase
/** Base class for all connection provider factories. Specifies the interface
    that all connection provider factories must implement. It also providers the
	ECOM framework (via CCommsFactoryBase) to load connection provider factories
	from other Dynamic Linked Libraries.

@internalTechnology
@released Since 9.1 */
	{
	friend class CConnectionProviderBase;
public:
	IMPORT_C static CConnectionProviderFactoryBase* NewL(const TDesC8& aName, CConnectionFactoryContainer& aParentContainer);
	IMPORT_C virtual ~CConnectionProviderFactoryBase();

	IMPORT_C ACommsFactoryNodeId* CreateL(TFactoryQueryBase& aQuery); //sync

protected:
	IMPORT_C CConnectionProviderFactoryBase(TUid aFactoryId, CConnectionFactoryContainer& aParentContainer);

	IMPORT_C ACommsFactoryNodeId* DoFindOrCreateObjectL(TFactoryQueryBase& aQuery);
	/** Override this to create a new instance of CConnectionProviderBase

	@return Pointer to the newly created provider
	@exception Leaves in out of memory conditions */
	IMPORT_C virtual ACommsFactoryNodeId* DoCreateObjectL(TFactoryQueryBase& aQuery);

protected:
	struct TInternalData
		{
		};

protected:
	TInternalData* iInternalData;
	};

//
// CConnectionProviderBase
class CConnectionSettings;
class CConnectionProviderBase : public CMMCommsProviderBase, public TCFCpr
/**	Base class specifying the interfaces required to implement a connection provider.
	Contains collections of control and data clients and defines operations on them

@internalTechnology
@released Since 9.1 */
	{
	friend class CConnectionProviderFactoryBase;
#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend  CConnectionProviderBase* __x86gcc_connection_provider_base_cast(Messages::ANode* aNode);
    friend  CConnectionProviderBase& __x86gcc_connection_provider_base_cast(Messages::ANode& aNode);
#elif !defined(__GCCXML__)
    friend  CConnectionProviderBase* mcfnode_cast<CConnectionProviderBase>(Messages::ANode* aNode);
    friend  CConnectionProviderBase& mcfnode_cast<CConnectionProviderBase>(Messages::ANode& aNode);
#endif

public:
	inline const TProviderInfo& ProviderInfo() const
		{
		const TProviderInfoExt* providerInfo = static_cast<const TProviderInfoExt*>(AccessPointConfig().FindExtension(
				STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId)));
		ASSERT(providerInfo);
		return providerInfo->iProviderInfo;
		}

    Messages::RNodeInterface* DefaultSubConnectionProvider() const
        {
        return MeshMachine::AMMNodeBase::GetFirstClient<Messages::TDefaultClientMatchPolicy>(Messages::TClientType(TCFClientType::EData,TCFClientType::EDefault));
        }

protected:
	IMPORT_C CConnectionProviderBase(CConnectionProviderFactoryBase& aFactory,
	                                 const MeshMachine::TNodeActivityMap& aActivityMap);
	IMPORT_C virtual Messages::RNodeInterface* NewClientInterfaceL(const Messages::TClientType& aClientType, TAny* aClientInfo = NULL);
	IMPORT_C ~CConnectionProviderBase();
	};

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast...

	inline CConnectionProviderBase* __x86gcc_connection_provider_base_cast(Messages::ANode* aNode)
    	{
    	return static_cast<CConnectionProviderBase*>(aNode);
    	}

	inline CConnectionProviderBase& __x86gcc_connection_provider_base_cast(Messages::ANode& aNode)
    	{
    	return static_cast<CConnectionProviderBase&>(aNode);
    	}

	inline CConnectionProviderBase* __x86gcc_connection_provider_base_cast(Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CConnectionProviderBase*>(aFactoryObject) : NULL;
    	}

	inline const CConnectionProviderBase* __x86gcc_connection_provider_base_cast(const Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CConnectionProviderBase*>(aFactoryObject) : NULL;
    	}
#endif
}//namespace ESock


#ifdef __X86GCC__
	namespace Messages
	{
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template <>
	inline ESock::CConnectionProviderBase* mnode_cast<ESock::CConnectionProviderBase>(Messages::ANode* aNode)
		{
		return ESock::__x86gcc_connection_provider_base_cast(aNode);
		}

	template <>
	inline ESock::CConnectionProviderBase& mnode_cast<ESock::CConnectionProviderBase>(Messages::ANode& aNode)
		{
		return ESock::__x86gcc_connection_provider_base_cast(aNode);
		}
	}
#endif


namespace Factories
{

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template<>
	inline ESock::CConnectionProviderBase* factoryobject_cast<ESock::CConnectionProviderBase>(Factories::AFactoryObject* aFactoryObject)
	    {
		return ESock::__x86gcc_connection_provider_base_cast(aFactoryObject);
	    }

	template<>
	inline const ESock::CConnectionProviderBase* factoryobject_cast<const ESock::CConnectionProviderBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
		return ESock::__x86gcc_connection_provider_base_cast(aFactoryObject);
	    }
#else
	// RVCT does not allow the specialisation of template functions in a different namespace from the original
	// so we declare them in the Factories namespace.
	template<>
	inline ESock::CConnectionProviderBase* factoryobject_cast<ESock::CConnectionProviderBase>(Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CConnectionProviderBase*>(aFactoryObject) : NULL;
	    }

	template<>
	inline const ESock::CConnectionProviderBase* factoryobject_cast<const ESock::CConnectionProviderBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CConnectionProviderBase*>(aFactoryObject) : NULL;
	    }
#endif

}//namespace Factories

#endif	// __SS_CONNPROV_H__



