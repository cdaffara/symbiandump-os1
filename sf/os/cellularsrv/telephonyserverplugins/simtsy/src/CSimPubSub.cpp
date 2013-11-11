// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the CSimPubSub class
// 
//



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimPubSubTraces.h"
#endif

#include "CSimPubSub.h"


/**
Standard two phase construction.

@param aPSSimObserver pointer to event observer
@param aUid uids of Publish&Subscribe uids to subscribe to.
*/
CSimPubSub* CSimPubSub::NewL(MPSSimObserver* aPSSimObserver, const CSimPubSub::TPubSubProperty aProperty)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPUBSUB_NEWL_1, ">>CSimPubSub::NewL");
	CSimPubSub* pubsub=new(ELeave) CSimPubSub(aPSSimObserver,aProperty);
	CleanupStack::PushL(pubsub);
	pubsub->ConstructL();
	CleanupStack::Pop();
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMPUBSUB_NEWL_2, "<<CSimPubSub::NewL");
	return pubsub;
	}

/**
Private constructor used in the first phase of construction.
 
@param aPSSimObserver pointer to event observer
@param aUid uids of Publish&Subscribe uids to subscribe to.
*/
CSimPubSub::CSimPubSub(MPSSimObserver* aPSSimObserver, const CSimPubSub::TPubSubProperty aProperty)
	: CActive(EPriorityStandard),
	iProperty(aProperty),
	iPSSimCallBack(aPSSimObserver)
	{
	}

/**
2nd phase of construction - subscribe to event.
*/
void CSimPubSub::ConstructL()
	{
	User::LeaveIfError(iSubscribe.Attach(iProperty.iCategory,iProperty.iKey));
	CActiveScheduler::Add(this);
	}

/**
Destructor.  Cancels any outstanding request on P&S.
*/
CSimPubSub::~CSimPubSub()
	{
	Cancel();
	iSubscribe.Close();
	}
	
/**
Start the subscription
*/
void CSimPubSub::Start()
	{
	iSubscribe.Subscribe(iStatus);
	SetActive();
	}

/**
Handles completion of our P&S request.
Notifies the event observer.
*/
void CSimPubSub::RunL()
	{
	TInt status = iStatus.Int();
	// As described in documentation resubscribe before processing new value to prevent missing updates.
	if(status != KErrNone)
		User::After(1000000L);	//pause before attempting re-subscribe to avoid a tight-loop.
	Start();
	//pass new state to observer
	if (status==KErrNone)
		{
		TInt state;
		if(iSubscribe.Get(state) == KErrNone)
			{
			iPSSimCallBack->SimPSEvent(iProperty,state);	
			}
		}
	}

/**
Handles a request to cancel the state machine.
Cancels any P&S subscriptions.
*/
void CSimPubSub::DoCancel()
	{
	iSubscribe.Cancel();
	}

/**
Empty constructor
*/
CSimPubSub::TPubSubProperty::TPubSubProperty()
{
}


/**
Overloaded constructor
*/
CSimPubSub::TPubSubProperty::TPubSubProperty(TUid aCategory, TInt aKey,	TInt aType)
: iCategory(aCategory), iKey(aKey), iType(aType)
{
}


/**
Compare for equality.
*/
TBool CSimPubSub::TPubSubProperty::operator==(const CSimPubSub::TPubSubProperty& aProperty) const
	{
	return (iCategory == aProperty.iCategory && iKey == aProperty.iKey	&& iType == aProperty.iType);
	}
