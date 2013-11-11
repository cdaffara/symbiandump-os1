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
 @file SS_NODE.CPP
*/

#include "ss_mmcommsprov.h"
#include <elements/nm_messages_child.h>
#include <elements/mm_activities.h>
#include <comms-infras/corecpractivities.h> 
#include "ss_nodemessages_internal.h"
#include "ss_internal_activities.h"

#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockSSocksmcmsp, "ESockSSocksmcmsp");
#endif

using namespace ESock;
using namespace Messages;

CMMCommsProviderBase::CMMCommsProviderBase(CCommsFactoryBase& aFactory,
                                    const MeshMachine::TNodeActivityMap& aActivityMap)
:	ACommsFactoryNodeId(aFactory),
	ACFMMNodeBase(aActivityMap, ANodeId::Id()),
	TIfStaticFetcherNearestInHierarchy(this)
    {
    }

CMMCommsProviderBase::~CMMCommsProviderBase()
    {
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
	if(!iParameterBundle.IsNull())
		{
		iParameterBundle.Close();
		}
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
    }


EXPORT_C void CMMCommsProviderBase::SetParametersL(const RCFParameterFamilyBundleC& aParameterBundle)
/** Request from a control client to set the parameters of this mm-comms provider

@param aParameterBundle Bundle of parameters to be applied to this mm-comms node
@exception Leaves with System Error Code dependant on the implementation of the mm-comms provider */
	{
	if (! iParameterBundle.IsNull())
		{
		iParameterBundle.Close();
		}
	iParameterBundle.Open(aParameterBundle);
	}

EXPORT_C RCFParameterFamilyBundleC& CMMCommsProviderBase::GetOrCreateParameterBundleL()
	{
	if( ! iParameterBundle.IsNull())
		{
		return iParameterBundle;
		}
	return CreateParameterBundleL();
	}
	
EXPORT_C RCFParameterFamilyBundleC& CMMCommsProviderBase::CreateParameterBundleL()
{
	__ASSERT_DEBUG(iParameterBundle.IsNull(), User::Panic(KSpecAssert_ESockSSocksmcmsp, 1));

	RCFParameterFamilyBundle newBundle;
	newBundle.CreateL();
	iParameterBundle.Open(newBundle);

	return iParameterBundle;
}

/*
void CMMCommsProviderBase::AddCtrlProviderNJoinAsDataClientL( const Messages::TSignatureBase& aCFMessage, TInt aJoinFlags )
    {
   	AddClientL(aCFMessage.iSender, TClientType(TCFClientType::ECtrlProvider));
#ifndef __GCCXML__
	EJoined(Factory().ParentContainer()(),
			aCFMessage.ActivityId(),
	    	(*this)(),
	    	CFNODE_SET_CLIENT_TYPENFLAGS(RNodeInterface::EData, aJoinFlags)
		).PostTo(aCFMessage.iSender);
#endif
    }
*/
EXPORT_C NetInterfaces::TInterfaceControl* CMMCommsProviderBase::FetchNodeInterfaceControlL(TInt aInterfaceId)
	{
	return ACFMMNodeBase::FetchNodeInterfaceControlL(aInterfaceId);
	}

EXPORT_C const RMetaExtensionContainerC& CMMCommsProviderBase::GetAccessPointConfig() const
	{
	return AccessPointConfig();
	}

EXPORT_C void CMMCommsProviderBase::ReturnInterfacePtrL(MAccessPointConfigApi*& aInterface)
	{
	aInterface = this;
	}

EXPORT_C RNodeInterface* CMMCommsProviderBase::AddClientL(const Messages::TNodeId& aClientId, const Messages::TClientType& aClientType, TAny* aClientInfo)
	{
	RNodeInterface* nodeInterface = ANodeBase::AddClientL(aClientId, aClientType, aClientInfo);

	if(aClientType.Type() == TCFClientType::ECtrl)
		{
		TRAPD(err, nodeInterface->PreAllocL(MeshMachine::KDefaultMaxPreallocatedActivitySize));
		if(err!=KErrNone)
			{
			RemoveClient(nodeInterface->RecipientId());
			User::Leave(err);
			}
		}

	return nodeInterface;
	}

void CMMCommsProviderBase::DestroyOrphanedDataClients()
	{
    // Note: If PRDataClientStopActivity is running, it will call PRStates::TDestroyOrphanedDataClients
    // once it has stopped the data clients, so we don't need to start PRDestroyOrphans activity here.
    // What was happening before was that we were destroying the Default data client before one or more
    // non-Default clients.  This causes problems due to internal references between some non-Default
    // and Default data client types.
	if (CountClients<TDefaultClientMatchPolicy>(
	        TClientType(TCFClientType::EData),
	        TClientType(0, TCFClientType::EActive|TCFClientType::EActivating|TCFClientType::ELeaving|TCFClientType::EStarted|TCFClientType::EStarting))
		&& CountActivities(ECFActivityDestroyOrphans) == 0
		&& CountActivities(ECFActivityDestroy) == 0
	    && CountActivities(ECFActivityStopDataClient) == 0)
		{
		RNodeInterface::OpenPostMessageClose(Id(), Id(), TCFMessage::TDestroyOrphans().CRef());
		}
	}


