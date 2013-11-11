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

#include "startandretry.h"
#include <ssm/ssmstartupproperties.h>
#include "rvobserver.h"

void CStartAndRetry::Actuate()
	{
	TInt err = KErrNone;
	const TSsmExecutionBehaviour executionBehaviour = iStartupProperties->ExecutionBehaviour();
	if ( (ESsmWaitForSignal == executionBehaviour) || (ESsmDeferredWaitForSignal == executionBehaviour) )
		{
		//issue the request to start the application with check whether we already running or timeout
		TRAP( err, DoWaitForStartL() );
		}
	else
		{
		err = KErrArgument; 
		}
	if( KErrNone != err )
		{
		// Report the completion code and precipitate the destruction of this object.
		iStartSafeNotify.RendezvousComplete( err, iCallerIndex );
		}
	}

/** 
 From MRvObserverNotification
*/
void CStartAndRetry::NotifyCompletion( TInt aCompletion )
	{
	// Report the completion code and precipitate the destruction of this object.
	iStartSafeNotify.RendezvousComplete( aCompletion, iCallerIndex );
	}

/** 
 From MRvObserverNotification
*/
void CStartAndRetry::NotifyCancellation( TInt aCompletion )
	{
	// Report the cacellation code.
	iStartSafeNotify.CancellationComplete( aCompletion, iCallerIndex );
	}

/**
 The observer was constructed in ConstructL.
 Total number of attempts is 1 + NoOfRetries().
 
 The check for NULL is future-proofing precaution..
*/
void CStartAndRetry::DoWaitForStartL()
	{
	User::LeaveIfNull( iRvObserver );
	iRvObserver->RvAndObserveL( iStartupProperties );
	}

CStartAndRetry::~CStartAndRetry()
	{
	delete iRvObserver;
	delete iStartupProperties;
	}

CStartAndRetry* CStartAndRetry::NewL( MStartAndRetryNotifications& aStartSafeNotify, const CSsmStartupProperties& aStartupProperties, RProcess& aProcess, TInt aCallerIndex, CApaStarter* aApaStarter )
	{
	CStartAndRetry* self = NewLC( aStartSafeNotify, aStartupProperties, aProcess, aCallerIndex, aApaStarter );
	CleanupStack::Pop();	
	
	return self;
	}

CStartAndRetry* CStartAndRetry::NewLC( MStartAndRetryNotifications& aStartSafeNotify, const CSsmStartupProperties& aStartupProperties, RProcess& aProcess, TInt aCallerIndex, CApaStarter* aApaStarter )
	{
	CStartAndRetry* self = new(ELeave) CStartAndRetry( aStartSafeNotify, aProcess, aCallerIndex, aApaStarter );
	CleanupStack::PushL( self );
	self->ConstructL( aStartupProperties );
	
	return self;
	}

CStartAndRetry::CStartAndRetry( MStartAndRetryNotifications& aStartSafeNotify, RProcess& aProcess, TInt aCallerIndex, CApaStarter* aApaStarter )
: iCallerIndex( aCallerIndex ),
  iProcess( aProcess ),
  iStartSafeNotify( aStartSafeNotify ),
  iApaStarter( aApaStarter )
	{
	}

void CStartAndRetry::ConstructL( const CSsmStartupProperties& aStartupProperties )
	{	
	iStartupProperties = CSsmStartupProperties::NewL( aStartupProperties );
	
	iRvObserver = CRvObserver::NewL( iProcess, *this, iApaStarter );
	}






