// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Mobility Connection Provider implementation
// 
//
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/corecprstates.h>
#include <comms-infras/corecpractivities.h>
#include "mobilitycpr.h"
#include <comms-infras/ss_log.h>
#include <comms-infras/api_ext_list.h>

#include <comms-infras/ss_msgintercept.h>


#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ESockMbCrCPR, "ESockMbCrCPR");
#endif

#ifdef SYMBIAN_TRACE_ENABLE
#define KIPCprTag KESockConnectionTag
// _LIT8(KIPCprSubTag, "mobilitycpr");
#endif // SYMBIAN_TRACE_ENABLE

using namespace ESock;
using namespace NetStateMachine;
using namespace Messages;
using namespace MeshMachine;

//-=========================================================
//
// CMobilityConnectionProvider methods
//
//-=========================================================
EXPORT_C CMobilityConnectionProvider* CMobilityConnectionProvider::NewL(CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap)
    {
    CMobilityConnectionProvider* provider = new (ELeave) CMobilityConnectionProvider(aFactory, aActivityMap);
    CleanupStack::PushL(provider);
    provider->ConstructL();
    CleanupStack::Pop(provider);
	return provider;
    }

EXPORT_C CMobilityConnectionProvider::CMobilityConnectionProvider(CConnectionProviderFactoryBase& aFactory, const MeshMachine::TNodeActivityMap& aActivityMap)
:	CCoreConnectionProvider(aFactory, aActivityMap),
	TIfStaticFetcherNearestInHierarchy(this)
    {
    LOG_NODE_CREATE(KIPCprTag, CMobilityConnectionProvider);
    }

EXPORT_C CMobilityConnectionProvider::~CMobilityConnectionProvider()
    {
    LOG_NODE_DESTROY(KIPCprTag, CMobilityConnectionProvider);
    }

EXPORT_C void CMobilityConnectionProvider::ReceivedL(const TRuntimeCtxId& aSender, const TNodeId& aRecipient, TSignatureBase& aMessage)
	{
	ESOCK_DEBUG_MESSAGE_INTERCEPT(aSender, aMessage, aRecipient);
	CCoreConnectionProvider::ReceivedL(aSender, aRecipient, aMessage);
	}

EXPORT_C void CMobilityConnectionProvider::ReturnInterfacePtrL(AMobilityProtocolReq*& aInterface)
	{
	aInterface = this;
	}

EXPORT_C void CMobilityConnectionProvider::OpenExtensionInterface(TSubSessionUniqueId aClientId, const Messages::TRuntimeCtxId& aControlClient, CCommsApiExtResponder* aResponder)
	{
	__ASSERT_DEBUG(aResponder, User::Panic(KSpecAssert_ESockMbCrCPR, 1)); //Responder must be valid here
	__ASSERT_DEBUG(aClientId != 0, User::Panic(KSpecAssert_ESockMbCrCPR, 2)); //Client id must be valid here.

	if (iMobilityActivity)
		{
		//Only one mobility client is supported by this implementation,
		//and this client has already opened the interface.
		CCommsApiExtResponder::Complete(aResponder,KErrInUse);
		return;
		}

	TCFMobilityProvider::TStartMobility msg(aResponder, aClientId);
	__ASSERT_DEBUG_NO_LEAVE(ReceivedL(aControlClient, Id(), msg)); //ReceivedL can never leave on MM nodes

	if (iMobilityActivity==NULL)
		{
		CCommsApiExtResponder::Complete(aResponder,KErrNoMemory);
		}
	}

EXPORT_C void CMobilityConnectionProvider::CloseExtensionInterface(TSubSessionUniqueId aClientId)
	{
	//Ignore all close requests for any other client than ours
	if (iMobilityActivity==NULL || iMobilityActivity->iClientId!=aClientId)
		{
		return;
		}

	//The activity has no originators, cancel it explicitly
	TNodeNullContext ctx(*this,iMobilityActivity);
	iMobilityActivity->Cancel(ctx);
	iMobilityActivity = NULL; //New mobility interface can be opened now
	}

EXPORT_C void CMobilityConnectionProvider::SubscribeForMessage(TSubSessionUniqueId aClientId, CCommsMobilitySrvResp* aRespExt)
	{
	__ASSERT_DEBUG(aRespExt, User::Panic(KSpecAssert_ESockMbCrCPR, 3)); //Responder must be valid here
	//If the interface was opened successfuly, iMobilityActivity is set.
	//It could only be cleared again if:
	//1) The mobility activity generated or not handled received error message.
	//But the activity is supposed to do that.
	//2) The node received and is currently processing TDestroy before this
	//activity was terminated, which can never happen, as the Control Client
	//would cancel it before leaving or it would be aborted on the Control Client
	//leaving (so before sending TDataClientIdle).
	__ASSERT_DEBUG(iMobilityActivity, User::Panic(KSpecAssert_ESockMbCrCPR, 4));
	__ASSERT_DEBUG(iMobilityActivity->iClientId==aClientId, User::Panic(KSpecAssert_ESockMbCrCPR, 5));
	(void)(iMobilityActivity->iClientId==aClientId);   //keep compiler happy
	iMobilityActivity->SetResponder(*aRespExt);
	}

EXPORT_C void CMobilityConnectionProvider::CancelSubscription(TSubSessionUniqueId aClientId)
	{
	//Ignore all cancel requests for any other client than ours
	if (iMobilityActivity==NULL || iMobilityActivity->iClientId!=aClientId)
		{
		return;
		}

	__ASSERT_DEBUG(iMobilityActivity, User::Panic(KSpecAssert_ESockMbCrCPR, 7));
	CCommsApiExtResponder::Complete(iMobilityActivity->iResponder, KErrCancel);
	}

EXPORT_C void CMobilityConnectionProvider::MigrateToPreferredCarrier(TSubSessionUniqueId aClientId)
	{
	//If the interface was opened successfuly, iMobilityActivity is set.
	//It could only be cleared again if:
	//1) The mobility activity generated or not handled received error message.
	//But the activity is supposed to do that.
	//2) The node received and is currently processing TDestroy before this
	//activity was terminated, which can never happen, as the Control Client
	//would cancel it before leaving or it would be aborted on the Control Client
	//leaving (so before sending TDataClientIdle).
	__ASSERT_DEBUG(iMobilityActivity, User::Panic(KSpecAssert_ESockMbCrCPR, 8));
	__ASSERT_DEBUG(iMobilityActivity->iClientId==aClientId, User::Panic(KSpecAssert_ESockMbCrCPR, 9));
	(void)(iMobilityActivity->iClientId==aClientId); //keep compiler happy
	TCFMobilityProvider::TMigrationRequested msg;
	__ASSERT_DEBUG_NO_LEAVE(ReceivedL(NodeId(), Id(), msg)); //ReceivedL can never leave on MM nodes
	}

EXPORT_C void CMobilityConnectionProvider::IgnorePreferredCarrier(TSubSessionUniqueId aClientId)
	{
	//If the interface was opened successfuly, iMobilityActivity is set.
	//It could only be cleared again if:
	//1) The mobility activity generated or not handled received error message.
	//But the activity is supposed to do that.
	//2) The node received and is currently processing TDestroy before this
	//activity was terminated, which can never happen, as the Control Client
	//would cancel it before leaving or it would be aborted on the Control Client
	//leaving (so before sending TDataClientIdle).
	__ASSERT_DEBUG(iMobilityActivity, User::Panic(KSpecAssert_ESockMbCrCPR, 10));
	__ASSERT_DEBUG(iMobilityActivity->iClientId==aClientId, User::Panic(KSpecAssert_ESockMbCrCPR, 11));
	(void)(iMobilityActivity->iClientId==aClientId); //keep compiler happy
	TCFMobilityProvider::TMigrationRejected msg(KErrNone);
	__ASSERT_DEBUG_NO_LEAVE(ReceivedL(NodeId(), Id(), msg)); //ReceivedL can never leave on MM nodes
	}

EXPORT_C void CMobilityConnectionProvider::NewCarrierAccepted(TSubSessionUniqueId aClientId)
	{
	//If the interface was opened successfuly, iMobilityActivity is set.
	//It could only be cleared again if:
	//1) The mobility activity generated or not handled received error message.
	//But the activity is supposed to do that.
	//2) The node received and is currently processing TDestroy before this
	//activity was terminated, which can never happen, as the Control Client
	//would cancel it before leaving or it would be aborted on the Control Client
	//leaving (so before sending TDataClientIdle).
	__ASSERT_DEBUG(iMobilityActivity, User::Panic(KSpecAssert_ESockMbCrCPR, 12));
	__ASSERT_DEBUG(iMobilityActivity->iClientId==aClientId, User::Panic(KSpecAssert_ESockMbCrCPR, 13));
	(void)(iMobilityActivity->iClientId==aClientId);//keep compiler happy
	TCFMobilityProvider::TMigrationAccepted msg;
	__ASSERT_DEBUG_NO_LEAVE(ReceivedL(NodeId(), Id(), msg)); //ReceivedL can never leave on MM nodes
	}

EXPORT_C void CMobilityConnectionProvider::NewCarrierRejected(TSubSessionUniqueId aClientId)
	{
	//If the interface was opened successfuly, iMobilityActivity is set.
	//It could only be cleared again if:
	//1) The mobility activity generated or not handled received error message.
	//But the activity is supposed to do that.
	//2) The node received and is currently processing TDestroy before this
	//activity was terminated, which can never happen, as the Control Client
	//would cancel it before leaving or it would be aborted on the Control Client
	//leaving (so before sending TDataClientIdle).
	__ASSERT_DEBUG(iMobilityActivity, User::Panic(KSpecAssert_ESockMbCrCPR, 14));
	__ASSERT_DEBUG(iMobilityActivity->iClientId==aClientId, User::Panic(KSpecAssert_ESockMbCrCPR, 15));
	(void)(iMobilityActivity->iClientId==aClientId);//keep compiler happy
	TCFMobilityProvider::TMigrationRejected msg(KErrNone);
	__ASSERT_DEBUG_NO_LEAVE(ReceivedL(NodeId(), Id(), msg)); //ReceivedL can never leave on MM nodes
	}


