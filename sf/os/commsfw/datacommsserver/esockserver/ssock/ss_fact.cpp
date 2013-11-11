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
 @file SS_FACT.CPP
*/

#include <ss_std.h>
#include <ecom/ecom.h>
#include "ss_fact.h"
#include <comms-infras/ss_fact_internal.h>


#include <ss_glob.h>
#include <comms-infras/ss_log.h>
#include <comms-infras/netmessages.h>
#include <in_sock.h> //KAfInet
#include <elements/nm_messages_peer.h>
#include <elements/factorynotify.h>
#include <comms-infras/ss_nodeinterfaces.h>
#include <comms-infras/ss_msgintercept.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_tiermanagerutils.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocks_fct, "ESockSSocks_fct.");
#endif

using namespace ESock;
using namespace Messages;
using namespace Factories;
using namespace MeshMachine;


CCommsFactoryContainer::CCommsFactoryContainer(TContaineeType aId)
: CFactoryContainerNode(),
iId(aId)
	{
	}

void CCommsFactoryContainer::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /* aRecipient */, TSignatureBase& aMessage)
	{
	if (aMessage.IsMessage<TCFFactory::TFindOrCreatePeer>())
		{
		ACommsFactoryNodeId* factoryObject = CreateFactoryObjectNodeL (aMessage);
		RClientInterface::OpenPostMessageClose(Id(), aSender, TCFFactory::TPeerFoundOrCreated(factoryObject->Id()).CRef());
		aMessage.ClearMessageId();		
		}
	else
		{
		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocks_fct, 1)); //For debug configurations
		User::Leave(KErrNotSupported); //For release configurations			
		}

	}

EXPORT_C CFactoryBase* CCommsFactoryContainer::FindOrCreateFactoryL(TUid aUid)
	{
	CFactoryBase* factory = static_cast<CFactoryBase*>(FindFactory(aUid));
	if (factory == NULL)
		{
		factory = CreateFactoryL(aUid);
		}
	return factory;
	}

CFactoryBase* CCommsFactoryContainer::CreateFactoryL(TUid aUid)
	{
	return static_cast<CFactoryBase*> (CFactoryBase::NewL(aUid, *this));
	}

ACommsFactoryNodeId* CCommsFactoryContainer::CreateFactoryObjectNodeL(Messages::TSignatureBase& aMessage)
	{
	__ASSERT_DEBUG(aMessage.IsMessage<TCFFactory::TFindOrCreatePeer>(), User::Panic(KSpecAssert_ESockSSocks_fct, 2));
	const TCFFactory::TFindOrCreatePeer& msg = message_cast<const TCFFactory::TFindOrCreatePeer>(aMessage);
	CCommsFactoryBase* factory = static_cast<CCommsFactoryBase*>(FindOrCreateFactoryL(msg.iUid));
	return factory->FindOrCreateObjectL(*(msg.iQuery));		
	}

EXPORT_C ACommsFactoryNodeId* CCommsFactoryContainer::Find (Factories::MFactoryQuery& aQuery)
	{
	return static_cast<ACommsFactoryNodeId*>(CFactoryContainer::FindObject(aQuery));
	}

//==================================================================


EXPORT_C ACommsFactoryNodeId* CCommsFactoryBase::FindOrCreateObjectL(TFactoryQueryBase& aQuery)
	{
	return DoFindOrCreateObjectL(aQuery);
	}

EXPORT_C ACommsFactoryNodeId* CCommsFactoryBase::CreateObjectL(TFactoryQueryBase& aQuery)
	{
	ACommsFactoryNodeId* factoryObject = DoCreateObjectL(aQuery);
	
	// The object that was created during DoCreateObjectL execution is derived from both CBase
	// and ACommsFactoryNodeId.
	// It is not correct to use PushL for factoryObject here, because ACommsFactoryNodeId 
	// is not a CBase child.
	// So compiler will use CleanupStack::PushL(TAny *) function instead of 
	// CleanupStack::PushL(CBase *).
	// As a result if something leaves we get following problems:
	// 1. destructor is not called
	// 2. User:42 panic when Leave handler will try to free factoryObject's memory
	TRAPD(err, 
		DoPostCreationL(factoryObject,aQuery); //setting Tiermanager
		AddManagedObjectL(*factoryObject);
		)
	if (err != KErrNone)
		{
		// Use delete for ACommsFactoryNodeId only because it has VIRTUAL destructor in one of it's base classes
		delete factoryObject;
		User::Leave(err);
		}

	return factoryObject;
	}

EXPORT_C ACommsFactoryNodeId* CCommsFactoryBase::Find(Factories::MFactoryQuery& aQuery)
	{
	return static_cast<ACommsFactoryNodeId*>(CFactoryBase::FindObject(aQuery));		
	}

EXPORT_C ACommsFactoryNodeId* CCommsFactoryBase::DoFindOrCreateObjectL(TFactoryQueryBase& aQuery)
/** Default implementation to create a factory object. This may be overridden by a derived class
*/
	{
	ACommsFactoryNodeId* factoryObject = static_cast<ACommsFactoryNodeId*>(FindObject(aQuery));
	if (factoryObject == NULL)
		{
		factoryObject = CreateObjectL(aQuery);
		}
	return factoryObject;		
	}

EXPORT_C CCommsFactoryBase::CCommsFactoryBase(TUid aFactoryUid, CCommsFactoryContainer& aParentContainer)
:	CFactoryBase(aFactoryUid, aParentContainer)
	{
	LOG(ESockLog::Printf(KFactoryTag, _L("Factory(Uid : %d) is created."),aFactoryUid) );
	}

EXPORT_C CCommsFactoryBase::~CCommsFactoryBase()
/** CCommsFactoryBase destructor 
*/
    {
    LOG(ESockLog::Printf(KFactoryTag, _L("Factory (Uid : %d) is destroyed"), this->Uid()));
    }

EXPORT_C TInt CCommsFactoryBase::SendMessage( NetMessages::CMessage& aNetMessage )
/** Used to send an arbitrary message to the factory. The implementation is delegated to the
	derived class, if any
@param aNetMessage NetMessages::CMessage reference specifying the message data and type
@returns KErrNone on success, else KErrNotSupported */
   {
   return DoReceiveMessage(aNetMessage);
   }

EXPORT_C TInt CCommsFactoryBase::DoReceiveMessage( NetMessages::CMessage& /*aNetMessage*/ )
/** Default implementation for CCommsFactoryBase::SendMessage. This may be overridden by a
	derived	class
@see CCommsFactoryBase::SendMessage
@released Since 9.1 */
	{
	return KErrNotSupported;
	}

EXPORT_C void CCommsFactoryBase::DoPostCreationL(ACommsFactoryNodeId* /*aObject*/,TFactoryQueryBase& /*aQuery*/)
	{
// Specially left empty, to be overriden in child classes	
	}

// ACommsFactoryNodeId
//=======================================================================
EXPORT_C ACommsFactoryNodeId::~ACommsFactoryNodeId()
	{
		
	}

EXPORT_C ACommsFactoryNodeId::ACommsFactoryNodeId(CCommsFactoryBase& aFactory)
: AFactoryObject (aFactory)
	{		
	}


// CCFFactoryContainerBroker
//--------------------------

void CCFFactoryContainerBroker::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
	{
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, Id());

	// What have we been sent?
	if(aMessage.IsMessage<TCFFactory::TFindOrCreatePeer>())
		{
		TCFFactory::TFindOrCreatePeer& msg = message_cast<TCFFactory::TFindOrCreatePeer>(aMessage);

		// Find the relevant factory container in our client list
		RNodeInterface* interface = GetFirstClient<TDefaultClientMatchPolicy>(TCFPlayerRole(msg.iFactoryContainerType));
	
		// Forward the 'find or create' message on to it with the originator as the sender
		if(interface)
			{
			interface->PostMessage(aSender, aMessage);
			}
		// Respond with an error if we can't find the relevant factory container
		else
			{
			RNodeInterface::OpenPostMessageClose(
				Id(),
				aSender,
				TEBase::TError(KErrNotFound).CRef());
			}
		}
	else
		{
		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocks_fct, 3)); //For debug configurations
		User::Leave(KErrNotSupported); //For release configurations
		}
	}

CCFFactoryContainerBroker* CCFFactoryContainerBroker::NewL()
	{
	return new(ELeave) CCFFactoryContainerBroker();
	}

CCFFactoryContainerBroker::CCFFactoryContainerBroker()
	{
	LOG_NODE_CREATE(KESockBaseFactTag, CCFFactoryContainerBroker);
	}

