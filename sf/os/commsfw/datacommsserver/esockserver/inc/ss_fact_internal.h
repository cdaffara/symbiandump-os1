// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#if !defined(__SS_FACT_INTERNAL_H__)
#define __SS_FACT_INTERNAL_H__

#include <elements/factory.h>
#include <elements/sd_objectbroker.h>

namespace ESock
{

class CCommsFactoryBase;
class ACommsFactoryNodeId;

// We need to redeclare this for the CMetaConnectionProviderBase x86gcc cast operators to work.
#ifndef __CCOMMSFACTORYCONTAINER_DECLARED
#define __CCOMMSFACTORYCONTAINER_DECLARED
NONSHARABLE_CLASS(CCommsFactoryContainer) : public Factories::CFactoryContainerNode
/** Comms Factory Factory Container class is used to store comms factorys of a
    specific type. e.g. sub-connection provider factories. It provides basic
	functionality to find, add and remove factories. It has basic functionality to
	create a factory object. If a container wants to do more than just creating a 
	factory object it should specialise the CCommsFactoryContainer

@internalTechnology
@released Since 9.0 */
	{
	friend class CPitBoss;	// CPitBoss enumerates factories without needing (or wanting) derived types

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast.
    friend  CCommsFactoryContainer* __x86gcc_comms_factory_container_cast(Messages::ANode* aNode);
    friend  CCommsFactoryContainer& __x86gcc_comms_factory_container_cast(Messages::ANode& aNode);
#elif !defined(__GCCXML__)
	friend CCommsFactoryContainer* Messages::mnode_cast<CCommsFactoryContainer>(Messages::ANode* aNode);
#endif
	
public:
	IMPORT_C Factories::CFactoryBase* FindOrCreateFactoryL(TUid aUid);	
	IMPORT_C ACommsFactoryNodeId* Find (Factories::MFactoryQuery& aQuery);
protected:
	Factories::CFactoryBase* CreateFactoryL(TUid aUid);	
	ACommsFactoryNodeId* CreateFactoryObjectNodeL(Messages::TSignatureBase& aMessage);
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	
protected:
    //Type information for the planes.
    enum TContaineeType
        {
        EUnknown = 0,
	    ETierManagerFactoryContainer,
	    EMetaConnectionFactoryContainer,
	    EConnectionFactoryContainer,
	    ESubConnectionFactoryContainer,
	    ESubConnectionFlowFactoryContainer,
		EProtocolIntfFactoryContainer,
		EProtocolFamilyFactoryContainer,
        };
	CCommsFactoryContainer(TContaineeType aId = EUnknown);
public:
     const TContaineeType iId;

public:
	static CCommsFactoryContainer* NewL (TContaineeType aId);
	};
#endif // __CCOMMSFACTORYCONTAINER_DECLARED


class CCFFactoryContainerBroker : public Den::CCommonObjectBroker
/**
This object broker is intended to be a per technology singleton that forwards
factory requests to a specified factory container.

Factory containers are added to its client list during the handshaking at worker thread
initialisation.

@internalTechnology
*/
	{
public:
	static CCFFactoryContainerBroker* NewL();

protected:
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	CCFFactoryContainerBroker();
	};

#ifdef __X86GCC__
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast...
 	
	inline CCommsFactoryContainer* __x86gcc_comms_factory_container_cast(Messages::ANode* aNode)
    	{
    	return static_cast<CCommsFactoryContainer*>(aNode);
    	}

	inline CCommsFactoryContainer& __x86gcc_comms_factory_container_cast(Messages::ANode& aNode)
    	{
    	return static_cast<CCommsFactoryContainer&>(aNode);
    	}
#endif
}


#ifdef __X86GCC__
	namespace Messages
	{
 	// gcc-mingw does not support declaring friends from different namespaces so we define proxy
 	// functions to do the cast...
 	
	template <>
	inline ESock::CCommsFactoryContainer* mnode_cast<ESock::CCommsFactoryContainer>(Messages::ANode* aNode)
		{
		return ESock::__x86gcc_comms_factory_container_cast(aNode);
		}

	template <>
	inline ESock::CCommsFactoryContainer& mnode_cast<ESock::CCommsFactoryContainer>(Messages::ANode& aNode)
		{
		return ESock::__x86gcc_comms_factory_container_cast(aNode);
		}
	}
#endif

#endif
// __SS_FACT_INTERNAL_H__
