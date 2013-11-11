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
// SS_METACONNPROV.CPP
// 
//

/**
 @file
*/

#include <comms-infras/ss_log.h>
#include <elements/factory.h>
#include <comms-infras/ss_connselect.h>
#include "ss_flowrequest.h"
#include <ss_glob.h>
#include <comms-infras/ss_thread.h>
#include <comms-infras/ss_roles.h>
#include <in_sock.h> //KAfInet
#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_internal_esock.h>
#include <comms-infras/ss_nodemessages_factory.h>
#include <comms-infras/ss_nodemessages_tiermanagerfactory.h>
#include <comms-infras/ss_tiermanager_internal.h>
#include <comms-infras/ss_tiermanagerutils.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockstrmng, "ESockSSockstrmng");
#endif

using namespace ESock;
using namespace Messages;
using namespace Factories;

//

//CTierManagerBase
EXPORT_C CTierManagerBase::CTierManagerBase(CTierManagerFactoryBase& aFactory,
                                            const MeshMachine::TNodeActivityMap& aActivityMap)
:	ACommsFactoryNodeId(aFactory),
	ACFMMNodeBase(aActivityMap, ANodeId::Id())
/**
Constructor for CTierManagerBase
@param aFactory Parent container for the tier manager
@param aActivityMap Activity map for the tier manager
*/
	{
	LOG(ESockLog::Printf(KESockTierTag, _L("CTierManagerBase 0x%08x:\tcreated, factory Uid 0x%08x"),
		 this, Factory().Uid().iUid));
	}

EXPORT_C CTierManagerBase::~CTierManagerBase()
/**
Destructor for CTierManagerBase.
*/
	{
	iMetaConnectionProviders.Reset();
	}

EXPORT_C void CTierManagerBase::AddMetaConnectionProviderL(const CMetaConnectionProviderBase* aMetaConnectionProvider)
	{
	iMetaConnectionProviders.AppendL(aMetaConnectionProvider);
	}

EXPORT_C void CTierManagerBase::RemoveMetaConnectionProvider(const CMetaConnectionProviderBase* aMetaConnectionProvider)
	{
	TInt index = iMetaConnectionProviders.Find(aMetaConnectionProvider);
	if (index != KErrNotFound)
		{
		iMetaConnectionProviders.Remove(index);
		}
	}

EXPORT_C NetInterfaces::TInterfaceControl* CTierManagerBase::FetchNodeInterfaceControlL(TInt aInterfaceId)
	{
	return ACFMMNodeBase::FetchNodeInterfaceControlL (aInterfaceId);
	}

EXPORT_C TUid CTierManagerBase::TierId() const
	{
	TUid temp = TierImplId();
	return static_cast<CTierManagerFactoryBase&>(Factory()).TierTypeId();
	}

EXPORT_C TUid CTierManagerBase::TierImplId() const
	{
	return Factory().Uid();
	}
//
//Factories - Container
CTierManagerFactoryContainer* CTierManagerFactoryContainer::NewL()
	{
	CTierManagerFactoryContainer* container = new (ELeave) CTierManagerFactoryContainer();
	return container;
	}

void CTierManagerFactoryContainer::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
	{	
	if (aMessage.IsMessage<TCFInternalEsock::TCreateCSR>())
		{
		TCFInternalEsock::TCreateCSR& msg = message_cast<TCFInternalEsock::TCreateCSR>(aMessage);
		TTierTypeIdFactoryQuery tierTypeQuery(msg.iUid);
		ACommsFactoryNodeId* obj = Find(tierTypeQuery);   // match the address family to tierTypeId in factory container
		if (!obj)
	        {
            // object not found, use the commsdat to find the Tier Manager
            CommsDat::CMDBSession* dbs = CommsDat::CMDBSession::NewLC(KCDVersion1_2);
            // Maps Tier to ImplId For ECOM since ImplId is used in Factory
            msg.iUid= TierManagerUtils::MapTierIdtoTierImplIdL(msg.iUid, *dbs);
            CleanupStack::PopAndDestroy(dbs);
            CCommsFactoryBase* factory = static_cast<CCommsFactoryBase*>(FindOrCreateFactoryL(msg.iUid));
            TAlwaysFindFactoryQuery query;  // since there is only one Tier Manager per Tier Manager factory, return the first one
            obj = factory->FindOrCreateObjectL(query);
	        }
		
		CSelectionRequest* sr = CSelectionRequest::NewL(address_cast<TNodeId>(aSender), obj->Id());
		RClientInterface::OpenPostMessageClose(Id(), aSender, TCFInternalEsock::TCSRCreated(sr->Id()).CRef());
		aMessage.ClearMessageId();
		}
	else if (aMessage.IsMessage<TCFFactory::TFindOrCreatePeer>())
		{
		TCFFactory::TFindOrCreatePeer& msg = message_cast<TCFFactory::TFindOrCreatePeer>(aMessage);
		// Maps Tier to ImplId For ECOM since ImplId is used in Factory
		CommsDat::CMDBSession* dbs = CommsDat::CMDBSession::NewLC(KCDVersion1_2);
		msg.iUid= TierManagerUtils::MapTierIdtoTierImplIdL(msg.iUid, *dbs);
		CleanupStack::PopAndDestroy(dbs);
		CCommsFactoryContainer::ReceivedL(aSender, aRecipient, msg);
		}
	else
		{
		NM_LOG_START_BLOCK(KESockTierTag, _L8("CTierManagerFactoryContainer::ReceivedL"));
		NM_LOG((KESockTierTag, _L8("ERROR: KErrNotSupported [this=0x%08x]"), this));
		NM_LOG_MESSAGE_EXT(KESockTierTag, aMessage, _L8("Msg:"));
		NM_LOG_ADDRESS_EXT(KESockTierTag, aSender, _L8("From:"));
		NM_LOG_END_BLOCK(KESockTierTag, KNullDesC8);

		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockstrmng, 1)); //For debug configurations
		User::Leave(KErrNotSupported); //For release configurations
		}

	}


CTierManagerFactoryBase* CTierManagerFactoryContainer::Factory(TInt aIndex) const
	{
	return static_cast<CTierManagerFactoryBase*>(CCommsFactoryContainer::Factory(aIndex));
	}

EXPORT_C CTierManagerFactoryContainer::~CTierManagerFactoryContainer()
/** Tier manager factory container destructor */
	{
	LOG_NODE_DESTROY(KESockTierTag, CTierManagerFactoryContainer);
	}

CTierManagerFactoryContainer::CTierManagerFactoryContainer()
:CCommsFactoryContainer((CCommsFactoryContainer::TContaineeType)CTierManagerFactoryContainer::EId)
/** Tier manager factory container constructor */
	{
	LOG_NODE_CREATE(KESockTierTag, CTierManagerFactoryContainer);
	}



//
//Factories
EXPORT_C CTierManagerFactoryBase::CTierManagerFactoryBase(TUid aTierTypeId, TUid aFactoryUid, CTierManagerFactoryContainer& aParentContainer)
:	CCommsFactoryBase(aFactoryUid,aParentContainer), iTierTypeId(aTierTypeId)
	{
// derived classes must do this for best info	LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("CTierManagerFactoryBase %08x:\tCTierManagerFactoryBase(%08x)"), this, aFactoryUid.iUid));
	}

EXPORT_C CTierManagerFactoryBase::~CTierManagerFactoryBase()
/** Tier manager factory base destructor */
	{
	LOG(ESockLog::Printf(KESockMetaConnectionTag, _L("CTierManagerFactoryBase %08x:\t~CTierManagerFactoryBase()"), this));
	}

EXPORT_C ACommsFactoryNodeId* CTierManagerFactoryBase::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockstrmng, 2));
	User::Leave(KErrNotSupported);
	return NULL;
	}
	
EXPORT_C ACommsFactoryNodeId* CTierManagerFactoryBase::DoFindOrCreateObjectL(TFactoryQueryBase& aQuery)
	{
	ACommsFactoryNodeId* tiermgrobject = static_cast<ACommsFactoryNodeId*>(FindObject(aQuery));
	if (tiermgrobject == NULL)
		{
		tiermgrobject = CreateObjectL (aQuery);
		TlsGlobals::Get().AddPersistentItf(tiermgrobject->Id()); //@TODO: global lookup (TLS)! Remove when factories have proper getter functions.
		}
	return tiermgrobject;
	}

EXPORT_C TUid CTierManagerFactoryBase::TierTypeId() const
	{
	return iTierTypeId;
	}

