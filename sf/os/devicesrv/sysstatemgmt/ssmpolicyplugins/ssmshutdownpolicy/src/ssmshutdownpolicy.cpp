/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CSsmShutdownPolicy class.
*
*/

#include <centralrepository.h>
#include <ssm/ssmclayer.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/starterclient.h>
#include <ssm/starterdomaincrkeys.h>
#include <ssm/ssmuiproviderdll.h>

#include "ssmsubstateext.hrh"
#include "ssmmapperutility.h"
#include "ssmshutdownpolicy.h"
#include "trace.h"

/**
* Shutdown state policy resource file path format :
* "\private\<SID of SSM>\shutdown\"
*/
_LIT( KCommandListPath, "Z:\\private\\2000D75B\\shutdown\\" );

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// FromStarterReasonToCenRep
// ---------------------------------------------------------------------------
//
static TInt FromStarterReasonToCenRep( const TInt aReasonCode )
    {
    FUNC_LOG;
    INFO_1( "Startup reason from RStarterSession %d", aReasonCode );

    switch ( aReasonCode )
        {
        case KSsmCLayerPowerOffReason:                   return ENormalStartup;
        case RStarterSession::ELanguageSwitchReset:      return ELanguageSwitchReset;
        case RStarterSession::ENormalRFSReset:           return ENormalRFSReset;
        case RStarterSession::EDeepRFSReset:             return EDeepRFSReset;
        case RStarterSession::EOperatorSettingReset:     return EOperatorSettingReset;
        case RStarterSession::ENetworkModeChangeReset:   return ENetworkModeChangeReset;
        case RStarterSession::ESIMStatusChangeReset:     return ESIMStatusChangeReset;
        case RStarterSession::EDRMReset:                 return EDRMReset;
        case RStarterSession::EFirmwareUpdate:           return EFirmwareUpdate;
        case RStarterSession::EDataRestoreReset:         return EDataRestoreReset;
        case RStarterSession::EFieldTestReset:           return EFieldTestReset;
        case RStarterSession::EUnknownReset:
        default:
            return EUnknownReset;
        }
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmShutdownPolicy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmStatePolicy* CSsmShutdownPolicy::NewL()
    {
    CSsmShutdownPolicy* self = new( ELeave ) CSsmShutdownPolicy;
    CleanupStack::PushL( self );
    self->BaseConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSsmShutdownPolicy::~CSsmShutdownPolicy
// ---------------------------------------------------------------------------
//
CSsmShutdownPolicy::~CSsmShutdownPolicy()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmShutdownPolicy::GetNextState
// ---------------------------------------------------------------------------
//
TBool CSsmShutdownPolicy::GetNextState(
    TSsmState aCurrentTransition,
    TInt aReason,
    TInt ERROR_PARAM( aError ),
    TInt ERROR_PARAM( aSeverity ),
    TSsmState& aNextState )
    {
    FUNC_LOG;
    INFO_3( "Current state %04x.%04x, reason %d",
        aCurrentTransition.MainState(), aCurrentTransition.SubState(), aReason );
    ERROR_1( aError, "State transition resulted in an error, severity %d", aSeverity );
	ASSERT_TRACE( aCurrentTransition.MainState() == ESsmShutdown );

	TBool moreToCome = EFalse;
    switch( aCurrentTransition.SubState() )
	    {
        case ESsmErrorShutdown:
        case KSsmAnySubState:
        case ESsmNormalShutdown: // Fall through
            aNextState = TSsmState( ESsmShutdown, ESsmShutdownSubStateCritical );
			moreToCome = ETrue;
			break;
		case ESsmShutdownSubStateCritical:
            aNextState = TSsmState( ESsmShutdown, ESsmShutdownSubStateNonCritical );
			moreToCome = ETrue;
			break;
        case ESsmShutdownSubStateNonCritical:
            aNextState = TSsmState( ESsmShutdown, ESsmShutdownSubStatePowerOff );
			moreToCome = ETrue;
			break;
        default:
		    break;
		}

    if ( aCurrentTransition.SubState() == ESsmErrorShutdown )
        {
        TRAPD_ERR( errorCode, StoreStartupReasonL( EUnknownReset ) );
        ERROR( errorCode, "Failed to store startup reason" );
        }
    else if ( aCurrentTransition.SubState() == KSsmAnySubState ||
              aCurrentTransition.SubState() == ESsmNormalShutdown )
        {
        // This needs to be done before Startup adaptation shutdown command
        TRAPD_ERR( errorCode, StoreStartupReasonL(
            FromStarterReasonToCenRep( aReason ) ) );
        ERROR( errorCode, "Failed to store startup reason" );
        }

	return moreToCome;
    }


// ---------------------------------------------------------------------------
// CSsmShutdownPolicy::CSsmShutdownPolicy
// ---------------------------------------------------------------------------
//
CSsmShutdownPolicy::CSsmShutdownPolicy()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmShutdownPolicy::GetCommandListPathL
// ---------------------------------------------------------------------------
//
void CSsmShutdownPolicy::GetCommandListPathL( TDes& aCmdListPath )
    {
    FUNC_LOG;

	aCmdListPath.Format( KCommandListPath );
	iUtil->GetCommandListPath( aCmdListPath );
    }


// ---------------------------------------------------------------------------
// CSsmShutdownPolicy::IsAllowedTargetState
// ---------------------------------------------------------------------------
//
TBool CSsmShutdownPolicy::IsAllowedTargetState(
    const TSsmStateTransition& INFO_PARAM(aRequest) ) const
    {
    FUNC_LOG;

    INFO_2( "Requested target state = %04x.%04x", 
        aRequest.State().MainState(), aRequest.State().SubState() );

    return EFalse;
    }


// ---------------------------------------------------------------------------
// CSsmShutdownPolicy::TargetSubState
// ---------------------------------------------------------------------------
//
TUint16 CSsmShutdownPolicy::TargetSubState( const TUint16 aRequestedSubState )
    {
    FUNC_LOG;
    INFO_1( "Requested sub state %04x", aRequestedSubState );

	TUint16 subState = ( aRequestedSubState == KSsmAnySubState ) ?
        ESsmNormalShutdown : aRequestedSubState;

    INFO_1( "Target sub state %04x", subState );
    return subState;
    }


// ---------------------------------------------------------------------------
// CSsmShutdownPolicy::StoreStartupReasonL
// ---------------------------------------------------------------------------
//
void CSsmShutdownPolicy::StoreStartupReasonL( const TInt aReasonCode )
    {
    FUNC_LOG;
    INFO_1( "Storing startup reason %d", aReasonCode );

    CRepository* repository =
        CRepository::NewLC( iUtil->CrUid( KCRUidStartup ) );
    TInt errorCode = repository->Set( KStartupReason, aReasonCode );
    ERROR( errorCode, "Failed to set KStartupReason CenRep key" );

	// In case of disk full, we will not try to set the device lock query status cenrep key here.
	// Rather it is set after disk space is recovered.
	if ((EUnknownReset !=  aReasonCode) && (KErrDiskFull != errorCode))
		{
		// Set the lock code query success as EStartupDevLockNotSucess in case the shutdown is not due to the the unknown reset.  
		// This is because if the device crashes before resetting the key during boot time.
		errorCode = repository->Set(KStartupDevLockStatus, EStartupDevLockNotSucess);
		INFO_1("Setting KStartupDevLockStatus CenRep key with EStartupDevLockNotSucess completed with %d", errorCode);
		}

	if( KErrDiskFull == errorCode )
		{
		//Need not to put on CleanupStack, it's not calling any leaving function and
		//it's handled by reference count
		CSsmUiSpecific* ssmUiSpecific = CSsmUiSpecific::InstanceL();
		//Free complete reserve space
		errorCode = ssmUiSpecific->FreeReservedPhoneMemorySpace( 0 );
		if( KErrNone == errorCode )
			{
			errorCode = repository->Set( KStartupReason, aReasonCode );
			ERROR( errorCode, "Failed to set KStartupReason CenRep key after freeing the memory" );

			if (EUnknownReset !=  aReasonCode)
				{
				// Set the lock code query success as EStartupDevLockNotSucess in case the shutdown is not due to the the unknown reset.  
				// This is because if the device crashes before resetting the key during boot time.
				errorCode = repository->Set(KStartupDevLockStatus, EStartupDevLockNotSucess);
				ERROR( errorCode, "Failed to set KStartupDevLockStatus CenRep key after freeing the memory" );
				}
			}
		CSsmUiSpecific::Release();
		}
    
    CleanupStack::PopAndDestroy( repository );
    User::LeaveIfError( errorCode );
    }
