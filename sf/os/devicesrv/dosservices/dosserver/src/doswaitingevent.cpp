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
*    Implementation for the CDosWaitingEvent class
*
*/


#include "doswaitingevent.h"

//
// ---------------------------------------------------------
// CDosWaitingEvent Constructor
// ---------------------------------------------------------
//  

CDosWaitingEvent::CDosWaitingEvent()
{
	iParameter = NULL;
	iQueueCount = 0;
}

//
// ---------------------------------------------------------
// CDosWaitingEvent Destructor
// ---------------------------------------------------------
//  

CDosWaitingEvent::~CDosWaitingEvent()
{
	if(iParameter)
	{
		delete iParameter;
	}
}

//
// ---------------------------------------------------------
// CDosWaitingEvent::AddMeToQueue
// ---------------------------------------------------------
//  
	
void CDosWaitingEvent::AddMeToQueue(TSglQue<CDosWaitingEvent>& aQueue)
{
	aQueue.AddLast(*this);
	iQueueCount++;
}

//
// ---------------------------------------------------------
// CDosWaitingEvent:RemoveMeFromQueue
// ---------------------------------------------------------
//  

void CDosWaitingEvent::RemoveMeFromQueueD(TSglQue<CDosWaitingEvent>& aQueue)
{
	if(iQueueCount>0)
	{
		aQueue.Remove(*this);
		iQueueCount--;
		if(iQueueCount==0)
		{
			delete this;
		}
	}
}

//
// ---------------------------------------------------------
// CDosWaitingEvent::SetParameterL
//
// This version is used when the RDosEventSnd call was used
// to initiate the event.
// ---------------------------------------------------------
//  

void CDosWaitingEvent::SetParameterL(const RMessage2& aMessage)
{
	if(aMessage.Int2()>0)
	{
		iParameter = HBufC8::NewL(aMessage.Int2());
		TPtr8 parPtr(iParameter->Des());
		aMessage.ReadL(1, parPtr);
	}
}

//
// ---------------------------------------------------------
// CDosWaitingEvent::SetParameterL
//
// This version is used when the CDosEventManager class was
// used to raise the event.
// ---------------------------------------------------------
//  

void CDosWaitingEvent::SetParameterL(TAny* aParameter,TInt aParLength)
{
	if(aParLength>0)
	{
		iParameter = HBufC8::NewL(aParLength);
		TPtr8 parPtr(iParameter->Des());
		parPtr.Copy(static_cast<TUint8*>(aParameter),aParLength);
	}
}
