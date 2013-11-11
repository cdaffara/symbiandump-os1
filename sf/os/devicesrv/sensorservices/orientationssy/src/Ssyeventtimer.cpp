/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSsyOrientationEventTimer class implementation.
*
*/


#include "Ssyeventtimer.h"
#include "SsyTrace.h"

const TInt KOneMilSecondInMicroSeconds( 1000 );

// ----------------------------------------------------------------------------------
// CSsyOrientationEventTimer::CSsyOrientationEventTimer
// ----------------------------------------------------------------------------------
//
CSsyOrientationEventTimer::CSsyOrientationEventTimer( MSendDataCallBack&
                            aPrtToCallback ) :
    CActive( CActive::EPriorityHigh ),
    iCallBack( aPrtToCallback )
	{
	SSY_TRACE_IN();
	CActiveScheduler::Add( this );
	SSY_TRACE_OUT();
	}

// ----------------------------------------------------------------------------------
// CSsyOrientationEventTimer::~CSsyOrientationEventTimer
// ----------------------------------------------------------------------------------
//
CSsyOrientationEventTimer::~CSsyOrientationEventTimer() // destruct - virtual, so no export
	{
	SSY_TRACE_IN();
	if( IsActive() )
	    {
	    Cancel();
	    }
	iTimer.Close();
	SSY_TRACE_OUT();
	}

// ----------------------------------------------------------------------------------
// CSsyOrientationEventTimer::NewL
// ----------------------------------------------------------------------------------
//
CSsyOrientationEventTimer* CSsyOrientationEventTimer::NewL( TInt aSetTimerToMilSeconds,
     MSendDataCallBack& aPrtToCallback )
	{
	SSY_TRACE_IN();

	CSsyOrientationEventTimer* self = new
	    ( ELeave ) CSsyOrientationEventTimer( aPrtToCallback );
	CleanupStack::PushL( self );
	self->ConstructL( aSetTimerToMilSeconds );
	CleanupStack::Pop( self );
	SSY_TRACE_OUT();
	return self;
    }

// ----------------------------------------------------------------------------------
// CSsyOrientationEventTimer::ConstructL
// ----------------------------------------------------------------------------------
//
void CSsyOrientationEventTimer::ConstructL( TInt aSetTimerToMilSeconds )
	{
	SSY_TRACE_IN();
	User::LeaveIfError( iTimer.CreateLocal() );
	if( aSetTimerToMilSeconds < 0 )
	    {
	    User::Leave( KErrArgument );
	    }
	TTimeIntervalMicroSeconds32 interval( KOneMilSecondInMicroSeconds *
	        aSetTimerToMilSeconds );

    iTimer.After( iStatus, interval );
    SetActive();
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSsyOrientationEventTimer::RunL
// ----------------------------------------------------------------------------------
//
void CSsyOrientationEventTimer::RunL()
	{
	SSY_TRACE_IN();
    iCallBack.SendDataAfterTimer();
    SSY_TRACE_OUT();
	}

// ----------------------------------------------------------------------------------
// CSsyOrientationEventTimer::DoCancel
// ----------------------------------------------------------------------------------
//
void CSsyOrientationEventTimer::DoCancel()
	{
	SSY_TRACE_IN();
	iTimer.Cancel();
	SSY_TRACE_OUT();
	}

// ----------------------------------------------------------------------------------
// CSsyOrientationEventTimer::RunError
// ----------------------------------------------------------------------------------
//
TInt CSsyOrientationEventTimer::RunError( TInt aError )
    {
    SSY_TRACE_IN();
    if( aError != KErrNone )
        {
        SSY_TRACE( EExtended, "CSsyOrientationEventTimer::RunError aError=%d", aError );
        }
    SSY_TRACE_OUT();
    return KErrNone;
    }

// End of file
