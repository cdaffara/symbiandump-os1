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
 @file SS_CONNPROV.CPP
*/

#include <ss_std.h>
#include <comms-infras/esockmessages.h>
#include <comms-infras/ss_log.h>
#include <es_ini.h>
#include <ss_glob.h>
#include "ss_connprov.h"
#include <comms-infras/ss_nodemessages_factory.h>

#include <elements/nm_signatures.h>
#include <comms-infras/ss_nodemessages_legacy.h>

#include <comms-infras/ss_commsprov.h>
using namespace ESock;

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSockscnprv, "ESockSSockscnprv");
#endif

using namespace Messages;
using namespace Factories;

TConnectionInfo::TConnectionInfo(const TDesC8& /*aConnectionInfo*/ /*as passed to ::Bearer/::NoBearer*/)
	{
	}

TBool TConnectionInfo::IsLocalBearer(const TDesC8& aConnectionInfo)
	{
	__ASSERT_DEBUG(aConnectionInfo.Length() >= KConnInfoPart, User::Panic(KSpecAssert_ESockSSockscnprv, 1));

	TConnectionInfo info(0,0);
	TPckg<TConnectionInfo> infoBuf(info);

	return (infoBuf.Right(KConnInfoPart) == aConnectionInfo.Right(KConnInfoPart));
	}

EXPORT_C MFactoryQuery::TMatchResult XConnectionProviderInfoQuery::Match(TFactoryObjectInfo& aProviderInfo)
	{
	CConnectionProviderBase* prov = static_cast<CConnectionProviderBase*>(aProviderInfo.iInfo.iFactoryObject);
	const TProviderInfo& thePI = static_cast<const TProviderInfoExt&>(prov->AccessPointConfig().FindExtensionL(
	        STypeId::CreateSTypeId(TProviderInfoExt::EUid, TProviderInfoExt::ETypeId))).iProviderInfo;
	TBool isMatch = (thePI.APId() == iAPid);
	LOG(ESockLog::Printf(KESockCtrlFactTag, _L("XConnectionProviderInfoQuery::Match %08x:\tiAPid is = %08x THePI.APId = %08x"),
	                         this, iAPid, thePI.APId()));
	return (isMatch ? MFactoryQuery::EMatch : MFactoryQuery::EContinue);
	}


EXPORT_C CConnectionFactoryContainer* CConnectionFactoryContainer::NewL()
/** Create a new instance of a connection factory container

@exception Leaves in out of memory conditions
@return Pointer to new instance of a connection factory container
*/
	{
	TCFLegacyMessage::RegisterL();
	return new (ELeave) CConnectionFactoryContainer();
	}

CConnectionFactoryContainer::~CConnectionFactoryContainer()
	{
	TCFLegacyMessage::DeRegister();

	LOG_NODE_DESTROY(KESockConnectionTag, CConnectionFactoryContainer);
	}

CConnectionFactoryContainer::CConnectionFactoryContainer()
:CCommsFactoryContainer((CCommsFactoryContainer::TContaineeType)CConnectionFactoryContainer::EId)
/** Empty connection factory container constructor */
	{
	LOG_NODE_CREATE(KESockConnectionTag, CConnectionFactoryContainer);
	}


CConnectionProviderFactoryBase* CConnectionFactoryContainer::Factory( TInt aIndex ) const
	{
	return static_cast<CConnectionProviderFactoryBase*>(CCommsFactoryContainer::Factory(aIndex));
	}


void CConnectionFactoryContainer::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /*aRecipient*/, TSignatureBase& aMessage)
	{

	if (aMessage.IsMessage<TCFFactory::TFindOrCreatePeer>())
		{ //rjl todo: inconsistency in this function across planes.. ensure that's intended
		ACommsFactoryNodeId* factoryObject = CreateFactoryObjectNodeL (aMessage);
  		#ifndef __GCCXML__
   	    	RClientInterface::OpenPostMessageClose(Id(), aSender, TCFFactory::TPeerFoundOrCreated(factoryObject->Id(), 0).CRef());
   		#endif
		}
    else
        {
        NM_LOG_START_BLOCK(KESockMetaConnectionTag, _L8("CConnectionFactoryContainer::ReceivedL"));
        NM_LOG((KESockMetaConnectionTag, _L8("ERROR: KErrNotSupported [this=0x%08x]"), this));
        NM_LOG_MESSAGE(KESockMetaConnectionTag, aMessage);
        NM_LOG_END_BLOCK(KESockMetaConnectionTag, _L8("CConnectionFactoryContainer::ReceivedL"));

		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnprv, 2)); //For debug configurations
		User::Leave(KErrNotSupported); //For release configurations
		}
	}


//==========================================================

EXPORT_C CConnectionProviderFactoryBase::~CConnectionProviderFactoryBase()
/** Empty connection factory base destructor */
	{
	}

EXPORT_C CConnectionProviderFactoryBase::CConnectionProviderFactoryBase(TUid aFactoryId, CConnectionFactoryContainer& aParentContainer)
/** connection provider factory constructor

@param aFactoryId Unique Integer Identifier of the connection provider factory
@param aParentContainer Container to add the factory to */
	: CCommsFactoryBase(aFactoryId, aParentContainer)
	{
	}

EXPORT_C ACommsFactoryNodeId* CConnectionProviderFactoryBase::DoFindOrCreateObjectL(TFactoryQueryBase& aQuery)
	{
	ACommsFactoryNodeId* provider = static_cast<ACommsFactoryNodeId*>(FindObject(aQuery));
	return provider ? provider : CreateL(aQuery);		
	}

LOCAL_C void RemoveClientAndDestroy(TAny* aConnProvider)
    {
    CConnectionProviderBase* connProv = static_cast<CConnectionProviderBase*>(aConnProvider);
    Messages::RNodeInterface* cntrProv = connProv->ControlProvider();
    if(cntrProv != NULL)
        {
        connProv->RemoveClient(cntrProv->RecipientId());
        }
    
    // cast needed because CConnectionProviderBase destructor is protected
    delete static_cast<CCommsProviderBase*>(aConnProvider);
    }

EXPORT_C ACommsFactoryNodeId* CConnectionProviderFactoryBase::CreateL(TFactoryQueryBase& aQuery)
/**	Create a new instance of connection provider

@return Pointer to the created connection provider or NULL if there's a failure */
	{
	CConnectionProviderBase* provider = static_cast<CConnectionProviderBase*>(DoCreateObjectL(aQuery));
	CleanupStack::PushL(TCleanupItem(RemoveClientAndDestroy, provider));
	const TDefaultConnectionFactoryQuery& query = static_cast<const TDefaultConnectionFactoryQuery&>(aQuery);
	provider->AddClientL(address_cast<TNodeId>(query.iMCprId), TClientType(TCFClientType::ECtrlProvider));

	// Add the connectionProvider to it's factory
	AddManagedObjectL(*provider);

	CleanupStack::Pop(provider);	
	return provider;
	}

EXPORT_C ACommsFactoryNodeId* CConnectionProviderFactoryBase::DoCreateObjectL(TFactoryQueryBase& /* aQuery */)
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSockscnprv, 3));
	User::Leave(KErrNotSupported);
	return NULL;
	}

//==========================================================


EXPORT_C CConnectionProviderBase::CConnectionProviderBase(CConnectionProviderFactoryBase& aFactory,
                                                          const MeshMachine::TNodeActivityMap& aActivityMap)
:	CMMCommsProviderBase( aFactory,aActivityMap )
/**	Constructor for CConnectionProviderBase

@param aFactory Parent container for the provider */
	{
	LOG(ESockLog::Printf(KESockCtrlFactTag, _L("CConnectionProviderBase %08x:\tcreated [%d], factory Uid %08x"),
						 this, sizeof(CConnectionProviderBase), Factory().Uid().iUid));
	}

EXPORT_C RNodeInterface* CConnectionProviderBase::NewClientInterfaceL(const TClientType& aClientType, TAny* aClientInfo)
	{
	if (aClientType.Type() & TCFClientType::ECtrl)
		{
		if (aClientInfo == NULL)
			{ // This is not CPR, probably CConnection, use min priority.
			return new (ELeave) RCFNodePriorityInterface(KMaxTUint);
			}
		else
			{
			const TUint* priority = static_cast<const TUint*>(aClientInfo);
			return new (ELeave) RCFNodePriorityInterface(*priority);
			}
		}
	else
		{
		return MeshMachine::AMMNodeBase::NewClientInterfaceL(aClientType, aClientInfo);
		}
	}

EXPORT_C CConnectionProviderBase::~CConnectionProviderBase()
/**	Destructor for CConnectionProviderBase.
*/
	{
	LOG(ESockLog::Printf(KESockCtrlFactTag, _L("~CConnectionProviderBase %08x:\tfactory Uid %08x"),
						 this, Factory().Uid().iUid));
	}





