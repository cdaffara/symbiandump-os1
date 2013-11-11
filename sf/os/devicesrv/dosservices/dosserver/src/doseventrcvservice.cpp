/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    Implementation for the CDosEventRcvService class
*
*/


#include <e32base.h>
#include "doseventrcvservice.h"
#include "doseventmanagerbase.h"
#include "doswaitingevent.h"
#include "dosclientserver.h"
#include "dosserver.h"

//
// ---------------------------------------------------------
// CDosEventRcvService Constructor
// ---------------------------------------------------------
//  


CDosEventRcvService::CDosEventRcvService()
: iEventQueue(_FOFF(CDosWaitingEvent,iLink)) , iQueueIter(iEventQueue)
{
	iRegistered = EFalse;
	iReady = EFalse;
}


//
// ---------------------------------------------------------
// CDosEventRcvService Destructor
// ---------------------------------------------------------
//  

CDosEventRcvService::~CDosEventRcvService()
{
	if(iRegistered) iEventManager->UnRegisterListener(this,iEvent);

	if (!iEventQueue.IsEmpty())
	{
		CDosWaitingEvent* item;
    
	    iQueueIter.SetToFirst(); 
	    while ((item = iQueueIter++) != NULL)
		{
			delete item;
		}
		iEventQueue.Reset();
	}
}


//
// ---------------------------------------------------------
// CDosEventRcvService::NewL
// ---------------------------------------------------------
//  

CDosEventRcvService* CDosEventRcvService::NewL(CEventManagerBase* aEventManager)
{
	CDosEventRcvService* result=new (ELeave) CDosEventRcvService;
	CleanupStack::PushL(result);
	result->ConstructL(aEventManager);
	CleanupStack::Pop();

	return result;
}

//
// ---------------------------------------------------------
// CDosEventRcvService::ConstructL
// ---------------------------------------------------------
//  

void CDosEventRcvService::ConstructL(CEventManagerBase* aEventManager)
{
	iEventManager = aEventManager;
}

//
// ---------------------------------------------------------
// CDosEventRcvService::InformListener
// ---------------------------------------------------------
//  

void CDosEventRcvService::InformListener(CDosWaitingEvent* aEvent)
{
	TInt err = KErrNone;

	if(iReady)
	{
		iReady = EFalse;
		if(aEvent->HasParameter())
		{
			TRAP(err,iMessage.WriteL(1, aEvent->Parameter()));
		}

		iMessage.Complete(err);
	}
	else
	{
		if(iQueue == EQueue) //The event is queued if the event supports queueing
			aEvent->AddMeToQueue(iEventQueue);
		else if(iQueue == EOnlyLast)
		{
			//In the EOnlyLast case the queue only has one member.

			if(!iEventQueue.IsEmpty())
			{
				// If there is an event, we remove it 
				CDosWaitingEvent* event = iEventQueue.First();
				event->RemoveMeFromQueueD(iEventQueue);
			}

			aEvent->AddMeToQueue(iEventQueue); // We add the latest event.
		}
	}
}

//
// ---------------------------------------------------------
// CDosEventRcvService::ExecuteMessageL
// ---------------------------------------------------------
//  

TInt CDosEventRcvService::ExecuteMessageL(const RMessage2& aMessage)
{
	TInt err=KErrNone;

	switch (aMessage.Function())
	{
		case ERegisterEvent:
			iEvent = (TInt32)aMessage.Ptr0();
			iEventManager->RegisterListenerL(this,iEvent);
			iRegistered=ETrue;
			return err;

		case EUnRegisterEvent:
			err = iEventManager->UnRegisterListener(this,iEvent);
			if(!err)
			{
				iRegistered=EFalse;
				iReady=EFalse;
			}
			return err;

		case EWaitEvent:
			iMessage = aMessage;
			iReady = ETrue;
			iQueue = (TQueueType)aMessage.Int0();
			if(iQueue == EQueue || iQueue == EOnlyLast)
			{		
				// If we have a Queue we take the events from it.
				if(!iEventQueue.IsEmpty())
				{
					//There are events in the queue. We execute the first now.
					CDosWaitingEvent* event = iEventQueue.First();
					InformListener(event);
					event->RemoveMeFromQueueD(iEventQueue);
				}
			}
			return KErrNone;

		case ECancelWaitEvent:
			if ( iReady )
				{
				iReady = EFalse;
				iMessage.Complete(KErrCancel);
				}
			return KErrNone;
         
		default:
			PanicClient(aMessage,EPanicIllegalFunction);
			return KErrNone;
	}
}


