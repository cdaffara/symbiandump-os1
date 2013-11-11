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
 @file SS_SUBCONNPROV.CPP
*/
#include "ss_subconnprov.h"


#include <ss_std.h>
#include <cs_subconevents.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <comms-infras/ss_log.h>
#include <es_ini.h>
#include <comms-infras/ss_connprov.h>
#include <ss_glob.h>
#include <comms-infras/ss_subconnflow.h>

#include <comms-infras/ss_nodemessages_subconn.h>
#include <comms-infras/ss_nodemessages_factory.h>

#include <elements/nm_signatures.h>
#include <comms-infras/ss_nodemessages_legacy.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksbcnpr, "ESockSSocksbcnpr");
#endif

using namespace ESock;
using namespace Messages;
using namespace MeshMachine;
using namespace Factories;


EXPORT_C CSubConnectionFactoryContainer* CSubConnectionFactoryContainer::NewL()
/** Create a new instance of a sub-connection factory container

@exception Leaves in out of memory conditions
@return Pointer to new instance of a sub-connection factory container
*/
	{
	TCFLegacyMessage::RegisterL();
	return new (ELeave) CSubConnectionFactoryContainer();
	}

void CSubConnectionFactoryContainer::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& /* aRecipient */, TSignatureBase& aMessage)
	{
	if (aMessage.IsMessage<TCFFactory::TFindOrCreatePeer>())
		{
		TCFFactory::TFindOrCreatePeer& msg = message_cast<TCFFactory::TFindOrCreatePeer>(aMessage);
		CCommsFactoryBase* factory = static_cast<CCommsFactoryBase*>(FindOrCreateFactoryL(msg.iUid));
	    CSubConnectionProviderBase* provider = static_cast<CSubConnectionProviderBase*>(factory->FindOrCreateObjectL(*(msg.iQuery)));
	    if (provider)
	    	{
     	    CleanupStack::PushL(provider);
	    	// Second parameters is the Flags to be used for this Node:
	    	//	check if a "Default DataClient" was requested
	    	//	and set Flags accordingly     	    
	    	const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(*(msg.iQuery));
	    	TCFClientType::TFlags flags = ((TSubConnOpen::TSubConnType)query.iSCPRType == TSubConnOpen::EAttachToDefault) ? 
	    					      TCFClientType::EDefault : (TCFClientType::TFlags) 0;
 		    provider->AddClientL(address_cast<TNodeId>(aSender), TClientType(TCFClientType::ECtrlProvider, flags));
 	#ifndef __GCCXML__
 		    RClientInterface::OpenPostMessageClose(Id(), aSender, TCFFactory::TPeerFoundOrCreated(provider->Id(), flags).CRef());
 	#endif
     	    CleanupStack::Pop(provider);
	    	}
	    else
            {
			RClientInterface::OpenPostMessageClose(Id(), aSender, TCFFactory::TPeerFoundOrCreated(TNodeId::NullId(), 0).CRef());
            }
		aMessage.ClearMessageId();
		}
	else
		{
		NM_LOG_START_BLOCK(KESockSubConnectionTag, _L8("CSubConnectionFactoryContainer:ReceivedL"));
		NM_LOG((KESockSubConnectionTag, _L8("ERROR: KErrNotSupported [this=0x%08x] "), this));
		NM_LOG_MESSAGE(KESockSubConnectionTag, aMessage);
		NM_LOG_END_BLOCK(KESockSubConnectionTag, _L8("CSubConnectionFactoryContainer:ReceivedL"));

		__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksbcnpr, 1)); //For debug configurations
		User::Leave(KErrNotSupported); //For release configurations
		}
	}


CSubConnectionFactoryContainer::~CSubConnectionFactoryContainer()
/** Empty Sub-connection factory container destructor */
	{
	TCFLegacyMessage::DeRegister();
	LOG_NODE_DESTROY(KESockSubConnectionTag, CSubConnectionFactoryContainer);
	}

CSubConnectionFactoryContainer::CSubConnectionFactoryContainer()
:CCommsFactoryContainer((CCommsFactoryContainer::TContaineeType)CSubConnectionFactoryContainer::EId)
/** Empty Sub-connection factory container constructor */
	{
	LOG_NODE_CREATE(KESockSubConnectionTag, CSubConnectionFactoryContainer);
	}

CSubConnectionProviderFactoryBase* CSubConnectionFactoryContainer::Factory( TInt aIndex ) const
	{
	return static_cast<CSubConnectionProviderFactoryBase*>(CCommsFactoryContainer::Factory(aIndex));
	}

//==========================================================

EXPORT_C CSubConnectionProviderFactoryBase::~CSubConnectionProviderFactoryBase()
/** Empty Sub-connection factory base destructor */
	{
	}

EXPORT_C CSubConnectionProviderFactoryBase::CSubConnectionProviderFactoryBase(TUid aFactoryId, CSubConnectionFactoryContainer& aParentContainer)
/** Sub-connection provider factory constructor

@param aFactoryId Unique Integer Identifier of the sub-connection provider factory
@param aParentContainer Container to add the factory to */
	: CCommsFactoryBase(aFactoryId, aParentContainer)
	{
	}

EXPORT_C ACommsFactoryNodeId* CSubConnectionProviderFactoryBase::DoFindOrCreateObjectL(TFactoryQueryBase& aQuery)
/** Default implementation of the Find or Create functionality

@param aQuery The match/creation criteria for the sub-connection to be returned
@return The sub-connection provider found or created
@exception KErrNoMemory in out of memeory conditions, other system errors based on the specialised factory */
	{
	ACommsFactoryNodeId* subConnProvider = static_cast<ACommsFactoryNodeId*>(FindObject(aQuery));
	if (!subConnProvider)
		{
		const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);
		__ASSERT_DEBUG(!query.iControlProvider.IsNull(), User::Panic(KSpecAssert_ESockSSocksbcnpr, 2));
		subConnProvider = CreateL(aQuery);
		}
	return subConnProvider;
	}

EXPORT_C ACommsFactoryNodeId* CSubConnectionProviderFactoryBase::DoCreateObjectL(TFactoryQueryBase& /*aQuery*/)
	{
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_ESockSSocksbcnpr, 3));
	User::Leave(KErrNotSupported);
	return NULL;
	}

EXPORT_C ACommsFactoryNodeId* CSubConnectionProviderFactoryBase::CreateL(TFactoryQueryBase& aQuery)
/** Create an instance of a sub-connection provider

@param aConnectionProviderBase The connection provider for the sub-connection
@param aSubConnType The sub-connection type to create, e.g. attach to default or create a new one
@return The new sub-connection provider created
@exception KErrNoMemory in out of memeory conditions, other system errors based on the specialised factory */
	{
	CSubConnectionProviderBase* provider = static_cast<CSubConnectionProviderBase*>(DoCreateObjectL(aQuery));
	CleanupStack::PushL(provider);

	const TDefaultSCPRFactoryQuery& query = static_cast<const TDefaultSCPRFactoryQuery&>(aQuery);	

	TFactoryObjectEntry& entry = AddManagedObjectL(*provider);
	entry.iFlag |= query.iSCPRType;

	CleanupStack::Pop(provider);

	return provider;
	}

//=============================================================================
// Downcalls from control client (Implemented via pure virtual methods below)
//=============================================================================

EXPORT_C CSubConnectionProviderBase::CSubConnectionProviderBase(CSubConnectionProviderFactoryBase& aFactory,
                                                                const MeshMachine::TNodeActivityMap& aActivityMap)
: CMMCommsProviderBase(aFactory,aActivityMap)
/** Constructor

@param aFactory he factory that created and owns this object
@param aConnProvider Connection Provider the sub-connection provider is a member of */
	{
	LOG(ESockLog::Printf(KESockCtrlFactTag, _L("CSubConnectionProviderBase %08x:\tcreated [%d], factory Uid %x"),
						 this, sizeof(CSubConnectionProviderBase), Factory().Uid().iUid));
	}


EXPORT_C void CSubConnectionProviderBase::NotifyClientsL(const CSubConNotificationEvent& aEvent)
    {
	LOG(ESockLog::Printf(KESockCtrlFactTag, _L("[%08x] CSubConnectionProviderBase::NotifyClientsL ([%d])"),
					 this, aEvent.Id()));
    TClientIter<TDefaultClientMatchPolicy> iter = GetClientIter<TDefaultClientMatchPolicy>(TClientType(TCFClientType::ECtrl));
    if (iter[0] == NULL)
        {
        delete &aEvent;
        return;
        }

	CRefCountOwnedSubConNotification* eventNode = new (ELeave) CRefCountOwnedSubConNotification(&aEvent);
    RNodeInterface* ctl = NULL;
	while ((ctl = iter++) != NULL)
		{
		ctl->PostMessage(Id(), TCFSubConnControlClient::TSubConnNotification(eventNode).CRef());
		eventNode->Open();
		}
    }

EXPORT_C CSubConnectionProviderBase::~CSubConnectionProviderBase()
/** Destructor */
	{
#ifndef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	if(!iParameterBundle.IsNull())
		{
		iParameterBundle.Close();
		}
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	LOG(ESockLog::Printf(KESockCtrlFactTag, _L("~CSubConnectionProviderBase %08x:\tFactory Uid %x"),
						 this, Factory().Uid().iUid));
	}


