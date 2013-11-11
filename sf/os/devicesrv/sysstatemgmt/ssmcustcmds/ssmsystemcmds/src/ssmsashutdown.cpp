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
* Implementation of CSsmSaShutdown class.
*
*/

#include "ssmsashutdown.h"
#include "ssmmapperutilitystatic.h"
#include "trace.h"

#include <starterdomaincrkeys.h>
#include <ssm/startupadaptationcommands.h>

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// ReadStartupReasonL
// ---------------------------------------------------------------------------
//
static void ReadStartupReasonL( TInt& aReasonCode )
    {
    FUNC_LOG;

    TInt err = SsmMapperUtility::CrValue( KCRUidStartup, KStartupReason, 
                                          aReasonCode );
    ERROR( err, "Failed to get KStartupReason CenRep key" );
    User::LeaveIfError( err );
    }


// ---------------------------------------------------------------------------
// FromCenRepReasonToAdaptation
// ---------------------------------------------------------------------------
//
static TInt FromCenRepReasonToAdaptation( const TInt aReasonCode )
    {
    FUNC_LOG;
    INFO_1( "Startup reason from CentRep = %d", aReasonCode );

    switch ( aReasonCode )
        {
        case ELanguageSwitchReset:      return StartupAdaptation::ELanguageSwitchReset;
        case ENormalRFSReset:           return StartupAdaptation::ENormalRFSReset;
        case EDeepRFSReset:             return StartupAdaptation::EDeepRFSReset;
        case EOperatorSettingReset:     return StartupAdaptation::EOperatorSettingReset;
        case ENetworkModeChangeReset:   return StartupAdaptation::ENetworkModeChangeReset;
        case ESIMStatusChangeReset:     return StartupAdaptation::ESIMStatusChangeReset;
        case EDRMReset:                 return StartupAdaptation::EDRMReset;
        case EFirmwareUpdate:           return StartupAdaptation::EFirmwareUpdate;
        case EDataRestoreReset:         return StartupAdaptation::EDataRestoreReset;
        case EFieldTestReset:           return StartupAdaptation::EFieldTestReset;
        case EUnknownReset:
        default:
            return StartupAdaptation::EUnknownReset;
        }
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmSaShutdown::NewL
// ---------------------------------------------------------------------------
//
CSsmSaShutdown* CSsmSaShutdown::NewL()
	{
    FUNC_LOG;
	return new ( ELeave ) CSsmSaShutdown();
	}


// ---------------------------------------------------------------------------
// CSsmSaShutdown::~CSsmSaShutdown
// ---------------------------------------------------------------------------
//
CSsmSaShutdown::~CSsmSaShutdown()
    {
    FUNC_LOG;
    
    Cancel();
    Close();
    }


// ---------------------------------------------------------------------------
// CSsmSaShutdown::Initialize
// ---------------------------------------------------------------------------
//
TInt CSsmSaShutdown::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    FUNC_LOG;

    TInt errorCode = iAdaptation.Connect();
    ERROR( errorCode, "Failed to connect to RSsmStateAdaptation" );
    return errorCode;
    }


// ---------------------------------------------------------------------------
// CSsmSaShutdown::Execute
// ---------------------------------------------------------------------------
//
void CSsmSaShutdown::Execute( const TDesC8& /*aParams*/, 
                              TRequestStatus& aRequest )
    {
    FUNC_LOG;

    aRequest = KRequestPending;
    iClientStatus = &aRequest;

    TInt reasonCode( ENormalStartup );
    TRAPD_ERR( errorCode, ReadStartupReasonL( reasonCode ) );
    ERROR( errorCode, "Failed to read startup reason, using default" );

    if ( reasonCode == ENormalStartup )
        {
        iAdaptation.RequestCoopSysPerformShutdownActions( 0, iStatus );
        }
    else
        {
        reasonCode = FromCenRepReasonToAdaptation( reasonCode );
        INFO_1( "Startup Adaptation reset reason = %d", reasonCode );
        iAdaptation.RequestCoopSysPerformRestartActions(
            reasonCode, iStatus );
        }

    SetActive();
    }


// ---------------------------------------------------------------------------
// CSsmSaShutdown::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CSsmSaShutdown::ExecuteCancel()
    {
    FUNC_LOG;

    Cancel();
    }


// ---------------------------------------------------------------------------
// CSsmSaShutdown::Close
// ---------------------------------------------------------------------------
//
void CSsmSaShutdown::Close()
    {
    FUNC_LOG;

    iAdaptation.Close();
    }


// ---------------------------------------------------------------------------
// CSsmSaShutdown::Release
// ---------------------------------------------------------------------------
//
void CSsmSaShutdown::Release()
    {
    FUNC_LOG;

	delete this;
    }

// ---------------------------------------------------------------------------
// CSsmSaShutdown::CSsmSaShutdown
// ---------------------------------------------------------------------------
//
CSsmSaShutdown::CSsmSaShutdown() : CActive( EPriorityNormal )
    {
    FUNC_LOG;

    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CSsmSaShutdown::RunL
// ---------------------------------------------------------------------------
//
void CSsmSaShutdown::RunL()
    {
    FUNC_LOG;
    ERROR( iStatus.Int(), "CSsmSaShutdown::RunL" );

    User::RequestComplete( iClientStatus, iStatus.Int() );
    }


// ---------------------------------------------------------------------------
// CSsmSaShutdown::DoCancel
//
// ---------------------------------------------------------------------------
//
void CSsmSaShutdown::DoCancel()
    {
    FUNC_LOG;

    iAdaptation.RequestCancel();
    User::RequestComplete( iClientStatus, KErrCancel );
    }

