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
//

/**
 @file
 @internalTechnology
 @prototype
*/


#include "dummypr_mcprpubsubsubscriber.h"

#include <comms-infras/ss_log.h>
#include <comms-infras/coremcpractivities.h>
#include <comms-infras/ss_nodemessages_availability.h>

// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_DummyPrStopSubsc, "DummyPrStopSubsc");


#define KDummyMCprTag KESockMetaConnectionTag
_LIT8(KDummyMCprSubTag, "dummymcpr");


using namespace Messages;
using namespace ESock;

CMCPrPubSubSubscriber::CMCPrPubSubSubscriber(const ESock::CMetaConnectionProviderBase& aProvBase, TUint aStopCode)
:	CActive(EPriorityNormal),
	iNode(aProvBase),
	iStopCode(aStopCode)
	{
	}


void CMCPrPubSubSubscriber::StartL()
	{
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CMCPrPubSubSubscriber::StartL")));
	CActiveScheduler::Add(this);
	}

TInt CMCPrPubSubSubscriber::RunError(TInt /*aError*/)
	{
	//path uncovered.
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_DummyPrStopSubsc, 0));
	return 0;
	}

void CMCPrPubSubSubscriber::DoCancel()
	{
	iProperty.Cancel();
	}

/*virtual*/ CMCPrPubSubSubscriber::~CMCPrPubSubSubscriber()
	{
	this->Cancel(); // object must be stoppable by destcruction due to cleanup restrictions
	iProperty.Close();
	}

//

CMCPrPubSubAvailability* CMCPrPubSubAvailability::NewL(const ESock::CMetaConnectionProviderBase& aProvBase, TUint aStopCode)
	{
	CMCPrPubSubAvailability* self = new (ELeave) CMCPrPubSubAvailability(aProvBase,aStopCode);
	CleanupStack::PushL(self);
	self->StartL();
	CleanupStack::Pop(self);
	return self;
	}

CMCPrPubSubAvailability::CMCPrPubSubAvailability(const ESock::CMetaConnectionProviderBase& aProvBase, TUint aStopCode)
:   CMCPrPubSubSubscriber(aProvBase,aStopCode)
    {
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CMCPrPubSubAvailability ctor")));
	}

void CMCPrPubSubAvailability::StartAvailabilityMonitoringL(const Messages::TNodeCtxId& aAvailabilityActivity)
    {
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CMCPrPubSubSubscriber::StartAvailabilityMonitoringL")));

    iAvailabilityActivity = aAvailabilityActivity;
    TInt score = TAvailabilityStatus::EUnknownAvailabilityScore;

    TInt result = iProperty.Attach(TUid::Uid(EAvailabilityTestingPubSubUid), iNode.ProviderInfo().APId());
    if(result != KErrNone)
        {
        User::LeaveIfError(iProperty.Define(TUid::Uid(EAvailabilityTestingPubSubUid), iNode.ProviderInfo().APId(), RProperty::EInt));
        User::LeaveIfError(iProperty.Attach(TUid::Uid(EAvailabilityTestingPubSubUid), iNode.ProviderInfo().APId()));
        }

    iProperty.Subscribe(iStatus);
    SetActive();
    iProperty.Get(score); //If Get() is unsuccessful score must not be modified!

    if (score != TAvailabilityStatus::EUnknownAvailabilityScore)
        {
        //Send the intial availability status.
        TAvailabilityStatus status(score);
        RClientInterface::OpenPostMessageClose(NodeId(), iAvailabilityActivity,
                TCFAvailabilityControlClient::TAvailabilityNotification(status).CRef());
        }
    }

void CMCPrPubSubAvailability::CancelAvailabilityMonitoring()
    {
    iAvailabilityActivity = Messages::TNodeCtxId::NullId();
    }

void CMCPrPubSubAvailability::RunL()
	{
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CMCPrPubSubAvailability::RunL")));
	User::LeaveIfError(iStatus.Int());

	// resubscribe immediately
	iProperty.Subscribe(iStatus);
	SetActive();

    // then process
	TInt value;
	User::LeaveIfError(iProperty.Get(value));
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CMCPrPubSubAvailability subscribed value now %d"),value));

    __ASSERT_DEBUG(!iAvailabilityActivity.IsNull(), User::Panic(KSpecAssert_DummyPrStopSubsc, 1)); //maybe a bit defensive, but this is test code after all. The test changes availability when nobody is listenning; Surely test is wrong?
    RClientInterface::OpenPostMessageClose(Id(), iAvailabilityActivity,
        TCFAvailabilityControlClient::TAvailabilityNotification(value).CRef());
	}

//

CMCPrPubSubStopTrigger* CMCPrPubSubStopTrigger::NewL(const ESock::CMetaConnectionProviderBase& aProvBase, TUint aStopCode)
	{
	CMCPrPubSubStopTrigger* self = new (ELeave) CMCPrPubSubStopTrigger(aProvBase,aStopCode);
	CleanupStack::PushL(self);
	self->StartL();
	CleanupStack::Pop(self);
	return self;
	__ASSERT_DEBUG(EFalse, User::Panic(KSpecAssert_DummyPrStopSubsc, 2));

	}

CMCPrPubSubStopTrigger::CMCPrPubSubStopTrigger(const ESock::CMetaConnectionProviderBase& aProvBase, TUint aStopCode)
:   CMCPrPubSubSubscriber(aProvBase,aStopCode),
	iNodeId(aProvBase.Id())
    {
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CMCPrPubSubStopTrigger ctor")));
	}

void
CMCPrPubSubStopTrigger::StartL()
    {
	CMCPrPubSubSubscriber::StartL();
	TInt result = iProperty.Attach(TUid::Uid(EStopTriggeringPubSubUid), 1);
	if(result != KErrNone)
		{
		User::LeaveIfError(iProperty.Define(TUid::Uid(EStopTriggeringPubSubUid), 1, RProperty::EInt));
		User::LeaveIfError(iProperty.Attach(TUid::Uid(EStopTriggeringPubSubUid), 1));
		}
	iProperty.Subscribe(iStatus);
    SetActive();
    }

void CMCPrPubSubStopTrigger::RunL()
	{
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CMCPrPubSubStopTrigger::RunL")));
	User::LeaveIfError(iStatus.Int());

	// resubscribe immediately
	iProperty.Subscribe(iStatus);
	SetActive();

    // then process
	TInt value;
	User::LeaveIfError(iProperty.Get(value));
	__CFLOG_VAR((KDummyMCprTag, KDummyMCprSubTag, _L8("CMCPrPubSubStopTrigger subscribed value now %d"),value));

	CMetaConnectionProviderBase* provider = const_cast<CMetaConnectionProviderBase*>(static_cast<const CMetaConnectionProviderBase*>(&iNode));
	__ASSERT_DEBUG(provider, User::Panic(KSpecAssert_DummyPrStopSubsc, 3));

	if(value==KStart)
		{
		if(iBlocking)
			{
			// Decrement semaphore to stop the provider from being destroyed once all of it's children have been stopped.
			provider->DecrementBlockingDestroy();
			iBlocking = false;
			}

		TCFServiceProvider::TStart msg;
		RClientInterface::OpenPostMessageClose(iNodeId, iNodeId, msg);
		}
	else
	if((value==KBlockingStop || value==KStop) && iNode.ServiceProvider() != NULL)
		{
		if(!iBlocking && value==KBlockingStop)
			{
			// Increment semaphore to stop the provider from being destroyed once all of it's children have been stopped.
			provider->IncrementBlockingDestroy();
			iBlocking = true;
			}

		TCFServiceProvider::TStop msg(iStopCode);
		RClientInterface::OpenPostMessageClose(iNodeId, iNodeId, msg);
		}
	}

