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
* Description: Implementation of CSsmPowerSup class.
*
*/

#include <ssm/ssmclayer.h>
#include <ssm/ssmstatemanager.h>
#include <ssm/starterclient.h>

#include "ssmpowersup.h"
#include "trace.h"

// ======== LOCAL FUNCTIONS ========

static TInt ShutdownWithReason( const TInt aReason )
    {
    FUNC_LOG;

    RSsmStateManager session;
    TInt errorCode = session.Connect();
    ERROR( errorCode, "Failed to connect to RSsmStateManager" );

    if ( errorCode == KErrNone )
        {
        TUint16 mainstate = ESsmShutdown;
        TUint16 substate = KSsmAnySubState;
        TInt reason = aReason;
        TSsmStateTransition stateinfo( mainstate, substate, reason );

        // Make request and ignore return value
        session.RequestStateTransition( stateinfo );
        session.Close();
        }

    return errorCode;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmPowerSup::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmUtility* CSsmPowerSup::NewL()
	{
    FUNC_LOG;

	CSsmPowerSup* self = new ( ELeave ) CSsmPowerSup;
	return self;
	}


// ---------------------------------------------------------------------------
// CSsmPowerSup::~CSsmPowerSup
// ---------------------------------------------------------------------------
//
CSsmPowerSup::~CSsmPowerSup()
    {
    FUNC_LOG;

    Cancel();
    iAdaptation.Close();
    }


// ---------------------------------------------------------------------------
// CSsmPowerSup::RunL
// ---------------------------------------------------------------------------
//
void CSsmPowerSup::RunL()
	{
    FUNC_LOG;

    TInt errorCode = iStatus.Int();
    ERROR( errorCode, "RSsmStateAdaptation::NotifyCoopSysEvent completed with error" );

    if ( errorCode == KErrNone )
        {
        if ( iEventBuf() == ESsmShutdownDevice )
            {
            INFO( "DOS-originated shutdown" );
            errorCode = ShutdownWithReason( KSsmCLayerPowerOffReason );
            }
        else if ( iEventBuf() == ESsmRestartDevice ||
                  iEventBuf() == ESsmFatalCoopSysError )
            {
            INFO( "DOS-originated reset" );
            errorCode = ShutdownWithReason( RStarterSession::EUnknownReset );
            }
        }
    else if ( errorCode != KErrServerTerminated )
        {
        Activate();
        }
	}


// ---------------------------------------------------------------------------
// CSsmPowerSup::DoCancel()
// ---------------------------------------------------------------------------
//
void CSsmPowerSup::DoCancel()
    {
    FUNC_LOG;

    iAdaptation.NotifyCancel();
    }


// ---------------------------------------------------------------------------
// CSsmPowerSup::InitializeL()
// ---------------------------------------------------------------------------
//
void CSsmPowerSup::InitializeL()
    {
    FUNC_LOG;

    User::LeaveIfError( iAdaptation.Connect() );
    }


// ---------------------------------------------------------------------------
// CSsmPowerSup::StartL()
// ---------------------------------------------------------------------------
//
void CSsmPowerSup::StartL()
    {
    FUNC_LOG;

    Activate();
    }


// ---------------------------------------------------------------------------
// CSsmPowerSup::Release()
// ---------------------------------------------------------------------------
//
void CSsmPowerSup::Release()
    {
    FUNC_LOG;

	delete this;
    }


// ---------------------------------------------------------------------------
// CSsmPowerSup::CSsmPowerSup
// ---------------------------------------------------------------------------
//
CSsmPowerSup::CSsmPowerSup() : CActive( EPriorityNormal )
    {
    FUNC_LOG;

	CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CSsmPowerSup::Activate
// ---------------------------------------------------------------------------
//
void CSsmPowerSup::Activate()
    {
    FUNC_LOG;

    ASSERT_TRACE( !IsActive() );

    iAdaptation.NotifyCoopSysEvent( iEventBuf, iStatus );
    SetActive();
    }
