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

#include "timeoutwaiter.h"
#include "ssconst.h"


void CTimeoutWaiter::RunL()
	{
	
	iTimed.NotifyTimeout();
	}


/**
 Start the timer. If it is already running, re-start it.
*/
void CTimeoutWaiter::ActuateTimer()
	{
	
	Cancel();
	After( iTimeout );
	}

/**
@param aTimeout The timeout value in milliseconds.
@param aTimed The object to notify in the case of the timer expiring.
*/
CTimeoutWaiter* CTimeoutWaiter::NewL( TInt aTimeout, MTimeoutWaiterNotification& aTimed  )
	{
	CTimeoutWaiter* self = NewLC( aTimeout, aTimed );
	CleanupStack::Pop();

	return self;
	}



CTimeoutWaiter* CTimeoutWaiter::NewLC( TInt aTimeout, MTimeoutWaiterNotification& aTimed  )
	{
	CTimeoutWaiter* self = new(ELeave) CTimeoutWaiter( aTimeout, aTimed );
	CleanupStack::PushL( self );
	self->ConstructL();

	return self;
	}



CTimeoutWaiter::~CTimeoutWaiter()
	{
	Cancel();
	}



CTimeoutWaiter::CTimeoutWaiter( TInt aTimeout, MTimeoutWaiterNotification& aTimed   )
: CTimer( EPriorityStandard ), iTimeout( aTimeout * KSsmStartSafeMilliToMicro ), iTimed( aTimed )
	{
	CActiveScheduler::Add( this );
	}



void CTimeoutWaiter::ConstructL()
	{
	if( iTimeout < 0 )
		{
		User::Leave( KErrArgument );
		}
	
	CTimer::ConstructL();
	}

