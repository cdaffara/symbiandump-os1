/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Implementation for the CEventManagerBase class
*
*/


#include <e32base.h>
#include "doseventmanagerbase.h"
#include "dosevent.h"
#include "doswaitingevent.h"

//
// ---------------------------------------------------------
// CDosEventManagerBase Constructor
// ---------------------------------------------------------
//  

CEventManagerBase::CEventManagerBase() : iOrder(CDosEvent::Compare)
{
}

//
// ---------------------------------------------------------
// CDosEventManagerBase Destructor
// ---------------------------------------------------------
//  

CEventManagerBase::~CEventManagerBase()
{
	iEventArray.ResetAndDestroy();
	iEventArray.Close();
}

//
// ---------------------------------------------------------
// CDosEventManagerBase::RaiseEvent 
//
// This version of the function is used by the derived
// class.
// ---------------------------------------------------------
//  

TInt CEventManagerBase::RaiseEvent(TUint aEvent,TAny* aParameter,TInt aParLength)
{
	CDosEvent* event=NULL;
	TInt err=KErrNone;

	LookForEventInArray(aEvent,event);
	if(event)
	{
		//Create the waiting event

		CDosWaitingEvent* waitingEvent = NULL;
		waitingEvent = new CDosWaitingEvent;
		if(waitingEvent)
		{
			TRAP(err,waitingEvent->SetParameterL(aParameter,aParLength));
			if(!err)
			{
				event->InformListeners(waitingEvent);
				if(!waitingEvent->IsQueued()) //If it hasn't been queued anywhere we delete it here.
				{
					delete waitingEvent;
				}
			}
			else
				{
				delete waitingEvent;
				}	
			return err;
		}
		return KErrNoMemory;
	}
	//If the event is not in the array this call is ignored
	return KErrNotFound;
}

//
// ---------------------------------------------------------
// CDosEventManagerBase::RaiseEvent
//
// This version is intended to be used by calls originated
// by RDosEventSnd.
// ---------------------------------------------------------
//  

TInt CEventManagerBase::RaiseEvent(const RMessage2& aMessage)
{
	CDosEvent* event=NULL;
	TInt err=KErrNone;

	LookForEventInArray((TInt32)aMessage.Ptr0(),event);
	if(event)
	{
		//Create the waiting event

		CDosWaitingEvent* waitingEvent = NULL;
		waitingEvent = new CDosWaitingEvent;
		if(waitingEvent)
		{
			TRAP(err,waitingEvent->SetParameterL(aMessage));
			if(!err)
			{
				event->InformListeners(waitingEvent);
				if(!waitingEvent->IsQueued()) //If it hasn't been queued anywhere we delete it here.
				{
					delete waitingEvent;
				}
			}
			else
				{
				delete waitingEvent;
				}
			return err;
		}
		return KErrNoMemory;
	}
	//If the event is not in the array this call is ignored
	return KErrNotFound;
}

//
// ---------------------------------------------------------
// CDosEventManagerBase::RegisterListenerL
// ---------------------------------------------------------
//  

TInt CEventManagerBase::RegisterListenerL(CDosEventRcvService* aListener,TUint aEvent)
{
	//We look for the Event in the array.
	CDosEvent* event=NULL;

	LookForEventInArray(aEvent,event);
	if(!event) 
	{
		//The Event didn't exist so we create it
		event = CDosEvent::NewL(aEvent);

		//We add it to iEventArray
		iEventArray.InsertInOrder(event,iOrder);
	}

	//We add the listener to the Event's list
	event->AddListener(aListener);

	return KErrNone;
}

//
// ---------------------------------------------------------
// CDosEventManagerBase::LookForEventInArray
// ---------------------------------------------------------
//  

void CEventManagerBase::LookForEventInArray(TUint aEventName,CDosEvent*& aEventPtr)
{
	for(TInt i=iEventArray.Count()-1;i>=0;i--) //it counts backwards for eficiency
	{
		if(iEventArray[i]->Event()==aEventName)
		{
			//Found
			aEventPtr = iEventArray[i];
			break;
		}

		if(iEventArray[i]->Event()<aEventName)
		{
			//Not found
			aEventPtr = NULL;
			break;
		}
	}
}

//
// ---------------------------------------------------------
// CDosEventManagerBase::UnRegisterListener
// ---------------------------------------------------------
//  

TInt CEventManagerBase::UnRegisterListener(CDosEventRcvService* aListener, TUint aEvent)
{
	CDosEvent* event=NULL;

	LookForEventInArray(aEvent,event);
	if(!event)
	{
		//The event wasn't found on the Events array
		return KErrNotFound;
	}

	//Remove the listener from the list
	event->RemoveListener(aListener);

	//Check if the event doesn't have anymore listeners.
	if(event->ListenerCount()==0)
	{
		TInt index;
		//No more Listeners. The event can be removed from the list
		iEventArray.FindInOrder(event,index,iOrder);
		iEventArray.Remove(index);
		delete event;
	}
	return KErrNone;
}


