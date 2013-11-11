// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__SS_SUBCONNPROV_H__)
#define __SS_SUBCONNPROV_H__

#include <e32def.h>
#include <connpref.h>
#include <elements/factorynotify.h>
#include <es_sock.h>
#include <comms-infras/ss_mmcommsprov.h>
#include <comms-infras/ss_connprov.h>
#include <elements/cftransport.h>
#include <comms-infras/ss_nodemessages_scpr.h>

class TSockAddr;
class CSockManData;
class CConnectionProvdBase;

namespace ESock
{

class CSubConnectionProviderFactoryBase;
class CConnectionProviderBase;
class CSubConnectionProviderBase;

class CSubConnectionFlowBase;


//==========================================================================================

class CSubConnectionFactoryContainer : public CCommsFactoryContainer
/** Container for sub-connection provider factories

@internalTechnology
@released Since 9.0 */
	{
	friend class CSubConnectionProviderFactoryBase;
	friend class CSockManData; //SetFlowFactoryCookieL
#ifdef __X86GCC__
protected:
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend CSubConnectionProviderBase* __x86gcc_subconnection_provider_base_cast(Factories::AFactoryObject* aFactoryObject);
    friend const CSubConnectionProviderBase* __x86gcc_subconnection_provider_base_cast(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = ESubConnectionFactoryContainer
        };
#elif defined(__GCCXML__)
public:
    enum
        {
        EId = ESubConnectionFactoryContainer
        };
#else
protected:
    friend CSubConnectionProviderBase* Factories::factoryobject_cast<CSubConnectionProviderBase>(Factories::AFactoryObject* aFactoryObject);
    friend const CSubConnectionProviderBase* Factories::factoryobject_cast<const CSubConnectionProviderBase>(const Factories::AFactoryObject* aFactoryObject);

    enum
        {
        EId = ESubConnectionFactoryContainer
        };
#endif
public:
	virtual ~CSubConnectionFactoryContainer();
	IMPORT_C static CSubConnectionFactoryContainer* NewL();

	CSubConnectionProviderFactoryBase* Factory( TInt aIndex ) const;

protected:
	CSubConnectionFactoryContainer();
	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	};

class CSubConnectionProviderBase;
class CConnectionProviderFactoryBase;

//==========================================================================================

const TInt KSubConnectionFactoryInterfaceUid = 0x10204301;

class CSubConnectionProviderBase;
class CConnectionProviderBase;
class CConnDataTransfer;

//==========================================================================================

class CSubConnectionProviderFactoryBase : public CCommsFactoryBase
/** Base class for all sub-connection provider factories. Specifies the interface
    that all sub-connection provider factories must implement. It also providers the
	ECOM framework (via CCommsFactoryBase) to load sub-connection provider factories
	from other Dynamic Linked Libraries.

@internalTechnology
@released Since 9.0 */
	{
	friend class CConnectionProviderBase;
public:
	static CSubConnectionProviderFactoryBase* NewL(const TDesC8& aName, CSubConnectionFactoryContainer& aParentContainer);
	IMPORT_C virtual ~CSubConnectionProviderFactoryBase();

	IMPORT_C ACommsFactoryNodeId* CreateL(TFactoryQueryBase& aQuery);

protected:
	IMPORT_C CSubConnectionProviderFactoryBase(TUid aFactoryId, CSubConnectionFactoryContainer& aParentContainer);

	/**	Override this to provide your own implementation for finding/creating a flow, either by creating a new flow or
	returning an existing one, as appropriate for the flow type.
	*/
	IMPORT_C virtual ACommsFactoryNodeId* DoFindOrCreateObjectL(TFactoryQueryBase& aQuery);

	/**	Override this to provide your own implementation for creating a sub-connection provider

	@param aConnectionProviderBase The connection provider for the sub-connection to be created
	@param aSubConnType The creation type */
	IMPORT_C virtual ACommsFactoryNodeId* DoCreateObjectL(TFactoryQueryBase& aQuery);
	};


//==========================================================================================

class CSubConnectionProviderBase : public CMMCommsProviderBase, public TCFScpr
/**	Base class specifying the interfaces required to implement a sub-connection provider.
	Contains collections of sub connection control and data clients and defines operations on them

@internalTechnology
@released Since 9.0 */
	{
	friend class CSubConnectionProviderFactoryBase;

protected:
	IMPORT_C CSubConnectionProviderBase(CSubConnectionProviderFactoryBase& aFactory,
	                                    const MeshMachine::TNodeActivityMap& aActivityMap);
	IMPORT_C virtual ~CSubConnectionProviderBase();

    IMPORT_C void NotifyClientsL(const CSubConNotificationEvent& aEvent);
	};

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	inline CSubConnectionProviderBase* __x86gcc_subconnection_provider_base_cast(Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CSubConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CSubConnectionProviderBase*>(aFactoryObject) : NULL;
    	}

	inline const CSubConnectionProviderBase* __x86gcc_subconnection_provider_base_cast(const Factories::AFactoryObject* aFactoryObject)
    	{
	    return ESock::CSubConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CSubConnectionProviderBase*>(aFactoryObject) : NULL;
    	}
#endif
}//namespace ESock

namespace Factories
{

#ifdef __X86GCC__	
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
	template<>
	inline ESock::CSubConnectionProviderBase* factoryobject_cast<ESock::CSubConnectionProviderBase>(Factories::AFactoryObject* aFactoryObject)
	    {
		return ESock::__x86gcc_subconnection_provider_base_cast(aFactoryObject);
	    }

	template<>
	inline const ESock::CSubConnectionProviderBase* factoryobject_cast<const ESock::CSubConnectionProviderBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
		return ESock::__x86gcc_subconnection_provider_base_cast(aFactoryObject);
	    }
#else
	// RVCT does not allow the specialisation of template functions in a different namespace from the original
	// so we declare them in the Factories namespace.
	template<>
	inline ESock::CSubConnectionProviderBase* factoryobject_cast<ESock::CSubConnectionProviderBase>(Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CSubConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<ESock::CSubConnectionProviderBase*>(aFactoryObject) : NULL;
	    }

	template<>
	inline const ESock::CSubConnectionProviderBase* factoryobject_cast<const ESock::CSubConnectionProviderBase>(const Factories::AFactoryObject* aFactoryObject)
	    {
	    return ESock::CSubConnectionFactoryContainer::EId == static_cast<ESock::CCommsFactoryContainer&>(aFactoryObject->Factory().ParentContainer()).iId?
	    	static_cast<const ESock::CSubConnectionProviderBase*>(aFactoryObject) : NULL;
	    }
#endif
    
}//namespace Factories

#endif	// __SS_SUBCONNPROV

