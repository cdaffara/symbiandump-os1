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
* Description:  Orientation class of Orientation SSY
*
*/


#include <e32base.h>
#include "OrientationCRListener.h"
#include "OrientationConfiguration.h"
#include "SsyTrace.h"

// ----------------------------------------------------------------------------------
// CSsyOrientationCRListener::CSsyOrientationCRListener
// ----------------------------------------------------------------------------------
//
CSsyOrientationCRListener::CSsyOrientationCRListener( MCenRepCallBack&
                                                      aPrtToCallback ) :
    CActive( CActive::EPriorityStandard ),
    iCallBack( aPrtToCallback )
	{
	SSY_TRACE_IN();
	CActiveScheduler::Add( this );
	SSY_TRACE_OUT();
	}

// ----------------------------------------------------------------------------------
// CSsyOrientationCRListener::~CSsyOrientationCRListener
// ----------------------------------------------------------------------------------
//
CSsyOrientationCRListener::~CSsyOrientationCRListener()
	{
	SSY_TRACE_IN();

    if( IsActive() )
        {
        Cancel();
        }
    iRepository = NULL;
    SSY_TRACE_OUT();
	}

// ----------------------------------------------------------------------------------
// CSsyOrientationCRListener::NewL
// ----------------------------------------------------------------------------------
//
CSsyOrientationCRListener* CSsyOrientationCRListener::NewL(
    MCenRepCallBack& aPrtToCallback,
    CRepository* aRepository )
	{
	SSY_TRACE_IN();
	CSsyOrientationCRListener* self = new ( ELeave )
	    CSsyOrientationCRListener( aPrtToCallback );
	CleanupStack::PushL( self );
	self->ConstructL( aRepository );
	CleanupStack::Pop( self );
	SSY_TRACE_OUT();
	return self;
	}

// ----------------------------------------------------------------------------------
// CSsyOrientationCRListener::ConstructL
// ----------------------------------------------------------------------------------
//
void CSsyOrientationCRListener::ConstructL( CRepository* aRepository )
    {
    SSY_TRACE_IN();
    iRepository = aRepository;
    SetCenRepListener();
    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSsyOrientationCRListener::RunL
// ----------------------------------------------------------------------------------
//
void CSsyOrientationCRListener::RunL()
	{
	SSY_TRACE_IN();
	SSY_TRACE( EExtended, "ORIENTATIONSSY:RunL::Key %d changed", iStatus.Int() );
    iRepository->NotifyRequest( iStatus.Int(), iStatus );

        User::LeaveIfError(iCallBack.CreateConfigurations());

    if( !IsActive() )
        {
        SetActive();
        }
    SSY_TRACE_OUT();
	}

// ----------------------------------------------------------------------------------
// CSsyOrientationCRListener::DoCancel
// ----------------------------------------------------------------------------------
//
void CSsyOrientationCRListener::DoCancel()
    {
    SSY_TRACE_IN();
    iRepository->NotifyCancelAll();
    SSY_TRACE_OUT();
    }


// ----------------------------------------------------------------------------------
// CSsyOrientationCRListener::RunError
// ----------------------------------------------------------------------------------
//
TInt CSsyOrientationCRListener::RunError( TInt aError )
    {
    SSY_TRACE_IN();
    if( aError != KErrNone )
        {
        SSY_TRACE( EExtended, "CSsyOrientationCRListener::RunError aError = %d", aError );
        }

    SSY_TRACE_OUT();
    return KErrNone;
    }

// ----------------------------------------------------------------------------------
// CSsyOrientationCRListener::SetCenRepListener
// ----------------------------------------------------------------------------------
//
void CSsyOrientationCRListener::SetCenRepListener()
    {
    SSY_TRACE_IN();
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayRightUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayRightUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayLeftUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayLeftUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayUpwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayUpwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayDownwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayDownwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayDownKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpToDisplayDownTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayRightUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayRightUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayLeftUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayLeftUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayUpwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayUpwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayDownwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayDownwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownToDisplayUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayRightUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayRightUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayLeftUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayLeftUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayDownwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayDownwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayDownKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayUpwardsToDisplayDownTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayRightUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayRightUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayLeftUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayLeftUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayUpwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayUpwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayDownKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayDownwardsToDisplayDownTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayRightUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayRightUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayUpwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayUpwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayDownwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayDownwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayDownKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayLeftUpToDisplayDownTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayLeftUpKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayLeftUpTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayUpwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayUpwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayDownwardsKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayDownwardsTimerKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayDownKey, iStatus );
    iRepository->NotifyRequest( KOriStateDisplayRightUpToDisplayDownTimerKey, iStatus );
    if( !IsActive() )
        {
        SetActive();
        }
    SSY_TRACE_OUT();
    }

// End of file
