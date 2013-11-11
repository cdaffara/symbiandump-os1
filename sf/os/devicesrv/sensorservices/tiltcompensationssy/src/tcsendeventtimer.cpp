/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CTCSendEventTimer class implementation.
*
*/


#include "tcsendeventtimer.h"
#include "trace.h"
#include "tcstatetiltlistendata.h"

const TInt KOneSecondInMicroSeconds( 1000000 );

// ----------------------------------------------------------------------------------
// CTCSendEventTimer::CTCSendEventTimer
// ----------------------------------------------------------------------------------
//
CTCSendEventTimer::CTCSendEventTimer( MSendDataCallBack& aPrtToCallback ) :
    CActive( CActive::EPriorityStandard ),
    iCallBack( aPrtToCallback )
	{
	CActiveScheduler::Add( this );
	iIsCanceled = EFalse;
	}

// ----------------------------------------------------------------------------------
// CTCSendEventTimer::~CTCSendEventTimer
// ----------------------------------------------------------------------------------
//
CTCSendEventTimer::~CTCSendEventTimer() // destruct - virtual, so no export
	{
	iIsCanceled = ETrue;
	Cancel();
	iTimer.Cancel();
	iTimer.Close();
	}

// ----------------------------------------------------------------------------------
// CTCSendEventTimer::NewL
// ----------------------------------------------------------------------------------
//
CTCSendEventTimer* CTCSendEventTimer::NewL( TInt aSetTimerToHzLevel,
     MSendDataCallBack& aPrtToCallback )
	{
	FUNC_LOG;
	
	CTCSendEventTimer* self = new ( ELeave ) CTCSendEventTimer( aPrtToCallback );
	CleanupStack::PushL( self );
	self->ConstructL( aSetTimerToHzLevel );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------------
// CTCSendEventTimer::ConstructL
// ----------------------------------------------------------------------------------
//
void CTCSendEventTimer::ConstructL( TInt aSetTimerToHzLevel )
	{
	FUNC_LOG;
	
    TInt ret( KErrNone );
	ret = iTimer.CreateLocal();
	User::LeaveIfError( ret );
	iTimerToHzLevel = aSetTimerToHzLevel;
	}
void CTCSendEventTimer::Start(TInt aHrz)
	{
	iTimerToHzLevel = aHrz;
	TTimeIntervalMicroSeconds32 interval( KOneSecondInMicroSeconds / iTimerToHzLevel );
		
	if( !IsActive() )
		{
		iTimer.After( iStatus, interval );
		SetActive();
		}
    }

// ----------------------------------------------------------------------------------
// CTCSendEventTimer::RunL
// ----------------------------------------------------------------------------------
//
void CTCSendEventTimer::RunL()
	{
	FUNC_LOG;
	
    iCallBack.SendDataAfterTimer();
    TTimeIntervalMicroSeconds32 interval( KOneSecondInMicroSeconds / iTimerToHzLevel );
	
    if( !IsActive() && !iIsCanceled )
		{
		iTimer.After( iStatus, interval );
		SetActive();
		}
	}

// ----------------------------------------------------------------------------------
// CTCSendEventTimer::DoCancel
// ----------------------------------------------------------------------------------
//	
void CTCSendEventTimer::DoCancel()
	{
	iIsCanceled = ETrue;
	}

// ----------------------------------------------------------------------------------
// CTCSendEventTimer::RunError
// ----------------------------------------------------------------------------------
//
TInt CTCSendEventTimer::RunError( TInt aError )
    {
    if( aError != KErrNone )
        {
        INFO_1( "Run error in send event timer: %d", aError );
        }
    return KErrNone;
    }

// End of file
