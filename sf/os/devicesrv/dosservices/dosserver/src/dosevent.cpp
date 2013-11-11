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
*    Implementation for the CDosEvent class
*
*/


#include <e32base.h>
#include "dosevent.h"
#include "doswaitingevent.h"
#include "dos_debug.h"

#define SECOND_GREATER_THAN_FIRST -1
#define FIRST_GREATER_THAN_SECOND 1 

//
// ---------------------------------------------------------
// CDosEvent::NewL
// ---------------------------------------------------------
//  

CDosEvent* CDosEvent::NewL(TUint aEvent)
{
	CDosEvent* result = new (ELeave) CDosEvent;
	CleanupStack::PushL(result);
	result->ConstructL(aEvent);
	CleanupStack::Pop();

	return result;
}

//
// ---------------------------------------------------------
// CDosEvent::ConstructL
// ---------------------------------------------------------
//  

void CDosEvent::ConstructL(TUint aEvent)
{
	iEventName = aEvent;
}

//
// ---------------------------------------------------------
// CDosEvent Destructor
// ---------------------------------------------------------
//  

CDosEvent::~CDosEvent()
{
	//Destroys the list of Listeners
	if(!iListenerList.IsEmpty())
	{
		iListenerList.Reset();
	}
}

//
// ---------------------------------------------------------
// CDosEvent::Compare
// ---------------------------------------------------------
//  

TInt CDosEvent::Compare(const CDosEvent& aFirst,const CDosEvent& aSecond)
{
	if(aFirst.iEventName < aSecond.iEventName)
	{
		return SECOND_GREATER_THAN_FIRST;
	}
	else if (aFirst.iEventName > aSecond.iEventName)
	{
		return FIRST_GREATER_THAN_SECOND;
	}
	return 0;
}

//
// ---------------------------------------------------------
// CDosEvent::AddListener
// ---------------------------------------------------------
//  

void CDosEvent::AddListener(CDosEventRcvService* aListener)
{
	// The same listener can't be registered twice for the same event
	if(!ListenerRegistered(aListener)) 
	{
		iListenerCount++;
		iListenerList.AddFirst(*aListener);
	}
}

//
// ---------------------------------------------------------
// CDosEvent::ListenerRegistered
// ---------------------------------------------------------
//  

TBool CDosEvent::ListenerRegistered(CDosEventRcvService* aListener)
{
	CDosEventRcvService* item;

	iListenerIter.SetToFirst();
	while((item = iListenerIter++) != NULL)
	{
		if(item==aListener)
		{
			return ETrue;
		}
	}
	return EFalse;
}

//
// ---------------------------------------------------------
// CDosEvent::RemoveListener
// ---------------------------------------------------------
//  

void CDosEvent::RemoveListener(CDosEventRcvService* aListener)
{
	iListenerCount--;
	iListenerList.Remove(*aListener);
}

//
// ---------------------------------------------------------
// CDosEvent::InformListeners
// ---------------------------------------------------------
//  

void CDosEvent::InformListeners(CDosWaitingEvent* aEvent)
{
    COM_TRACE_( "[DOSSERVER] CDosEvent::InformListeners()" );

	CDosEventRcvService* item;
	TInt err(KErrNone);
	
	//Inform all the listeners in the list about the event
	iListenerIter.SetToFirst();
	while((item = iListenerIter++) != NULL)
	{
        // create a new instance of CDosWaitingEvent & copy the parameters
        // we have to create another instance because the very same 
        // object cannot reside on more than one queue.
        CDosWaitingEvent* copyEvent = new CDosWaitingEvent;
        if( copyEvent )
            {
            if( aEvent->HasParameter() )
                {
                TRAP( err, copyEvent->SetParameterL( (TAny*)aEvent->Parameter().Ptr(),
                                                     aEvent->Parameter().Length() ) );
                }
            if( KErrNone == err )
                {
	            item->InformListener(copyEvent);
        
                // if it hasn't queued, delete it
                if( !copyEvent->IsQueued() )
                    {
                    delete copyEvent;
                    copyEvent = NULL;
                    }
                }
            else
                {
                //SetParameterL() leaves
                COM_TRACE_( "[DOSSERVER] CDosEvent::InformListeners - no memory for parameters - no events sent" );
                delete copyEvent;
                copyEvent = NULL;
                err = KErrNone;
                }
            }
        else
            {
            //In no memory case no events are sent
            COM_TRACE_( "[DOSSERVER] CDosEvent::InformListeners - no memory for event - no events sent" );
            }   
	}
	
	COM_TRACE_( "[DOSSERVER] CDosEvent::InformListeners - return void" );
	
}
