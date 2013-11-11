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
* Description: Implementation of CSsmStartupPolicy class.
*
*/

#include <centralrepository.h>
#include <e32property.h>
#include <e32uid.h>
#include <featmgr.h>
#include <ssm/ssmdomaindefs.h>
#include <ssm/ssmcmd.hrh>
#include <ssm/ssmcommandlistresourcereader.h>
#include <ssm/ssmstate.h>
#include <ssm/ssmstateawaresession.h>
#include <ssm/ssmstatetransition.h>
#include <ssm/startupdomainpskeys.h>

#include "ssmsubstateext.hrh"
#include "ssmmapperutility.h"
#include "ssmmapperutilitystatic.h"
#include "ssmstartuppolicy.h"
#include "ssmpolicypluginsprivatecrkeys.h"
#include "trace.h"

/**
* Start-up state policy resource file path format :
* "\private\<SID of SSM>\startup\<Value of KSystemStartupModeKey>\"
*/
_LIT( KCommandListPath, "Z:\\private\\2000D75B\\startup\\%d\\" );

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// GetStartupMode
// ---------------------------------------------------------------------------
//
static TInt GetStartupMode()
    {
    FUNC_LOG;

    TInt startupMode( EStartupModeNormal );
    TInt errorCode = RProperty::Get(
        SsmMapperUtility::PsUid( KPSUidStartup ), KPSGlobalStartupMode, startupMode );
    ERROR( errorCode, "Failed to read startup mode from P&S" );
    INFO_1( "Startup mode is %d", startupMode );
    return startupMode;
    }


// ---------------------------------------------------------------------------
// DecideStateByStartupMode
// ---------------------------------------------------------------------------
//
static TSsmStartupSubStateExt DecideStateByStartupMode()
    {
    FUNC_LOG;

    TInt startupMode( GetStartupMode() );
    TSsmStartupSubStateExt nextState( ESsmStateSecurityCheck );
    switch ( startupMode )
        {
        case EStartupModeTest:
            nextState = ESsmStateTest;
            break;
        case EStartupModeAlarm:
            nextState = ESsmStateAlarm;
            break;
        case EStartupModeCharging:
            nextState = ESsmStateCharging;
            break;
        default:
            break;
        }

    return nextState;
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmStartupPolicy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C MSsmStatePolicy* CSsmStartupPolicy::NewL()
    {
    CSsmStartupPolicy* self = new( ELeave ) CSsmStartupPolicy;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::~CSsmStartupPolicy
// ---------------------------------------------------------------------------
//
CSsmStartupPolicy::~CSsmStartupPolicy()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::GetNextState
// ---------------------------------------------------------------------------
//
TBool CSsmStartupPolicy::GetNextState(
    TSsmState aCurrentTransition,
    TInt /*aReason*/,
    TInt aError,
    TInt aSeverity,
    TSsmState& aNextState )
    {
    FUNC_LOG;
    INFO_2( "Current state %04x.%04x",
        aCurrentTransition.MainState(), aCurrentTransition.SubState() );
    ERROR_1( aError, "State transition resulted in an error, severity %d", aSeverity );
	ASSERT_TRACE( aCurrentTransition.MainState() == ESsmStartup );

	TBool moreToCome = EFalse;
	if ( aError == KErrNone || aSeverity < ECmdHighSeverity )
		{
		switch( aCurrentTransition.SubState() )
			{
			case KSsmAnySubState:
			case ESsmStatePreUiServices:
				aNextState = TSsmState( ESsmStartup, ESsmStateStartingUiServices );
				moreToCome = ETrue;
				break;
			case ESsmStateStartingUiServices:
				aNextState = TSsmState( ESsmStartup, ESsmStateStartingCriticalApps );
				moreToCome = ETrue;
				break;
			case ESsmStateStartingCriticalApps:
				aNextState = TSsmState( ESsmStartup, ESsmStateSelfTestOK );
				moreToCome = ETrue;
				break;
			case ESsmStateSelfTestOK:
				aNextState = TSsmState( ESsmStartup, DecideStateByStartupMode() );
				moreToCome = ETrue;
				break;
			case ESsmStateChargingToNormal:
			case ESsmStateAlarmToNormal:
				aNextState = TSsmState( ESsmStartup, ESsmStateSecurityCheck );
				moreToCome = ETrue;
				break;
			case ESsmStateSecurityCheck:
				aNextState = TSsmState( ESsmStartup, SelectStateAfterSecurityCheck() );
				moreToCome = ETrue;
				break;
			case ESsmStateChargingToAlarm:
				aNextState = TSsmState( ESsmStartup, ESsmStateAlarm );
				moreToCome = ETrue;
				break;
			case ESsmStateAlarmToCharging:
				aNextState = TSsmState( ESsmStartup, ESsmStateCharging );
				moreToCome = ETrue;
				break;
			case ESsmStateNonCritical:
			    ClearResetCounter(); // start-up is successful
				aNextState = TSsmState( ESsmNormal, KSsmAnySubState );
				moreToCome = ETrue;
				break;
			case ESsmStateAlarm:
			case ESsmStateCharging:
			case ESsmStateTest:
				moreToCome = EFalse;
				break;
			case ESsmStateEmergencyCallsOnly:
			    ClearResetCounter(); // start-up is successful
				moreToCome = EFalse;
				break;
			default:
        		ASSERT_ALWAYS_TRACE;
				break;
			}
		}
	else
		{
        if ( ResetLimitReached() ) // Updates the reset count
            {
            aNextState = TSsmState( ESsmFail, KSsmAnySubState );
	    	moreToCome = ETrue;
    	    }
    	else
    	    {
            aNextState = TSsmState( ESsmShutdown, ESsmErrorShutdown );
            moreToCome = ETrue;
    	    }
		}

	return moreToCome;
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::GetCommandListPathL
// ---------------------------------------------------------------------------
//
void CSsmStartupPolicy::GetCommandListPathL( TDes& aCmdListPath )
    {
    FUNC_LOG;

	aCmdListPath.Format( KCommandListPath, iHardwareReason );
	iUtil->GetCommandListPath( aCmdListPath );
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::IsAllowedTargetState
// ---------------------------------------------------------------------------
//
TBool CSsmStartupPolicy::IsAllowedTargetState(
    const TSsmStateTransition& aRequest ) const
    {
    FUNC_LOG;

    TSsmState currentState;
    TInt errorCode = GetCurrentState( currentState );
    ERROR( errorCode, "Failed to get current state" );

    if ( errorCode == KErrNone )
        {
        TUint16 reqMainState( aRequest.State().MainState() );
        TUint16 reqSubState( aRequest.State().SubState() );
        INFO_2( "Requested target state = %04x.%04x", reqMainState, reqSubState );
        TUint16 curMainState( currentState.MainState() );
        TUint16 curSubState( currentState.SubState() );
        INFO_2( "Current state = %04x.%04x", curMainState, curSubState );

        if ( reqMainState == ESsmStartup )
            {
            if ( ( curSubState == ESsmStateCharging &&
                   ( reqSubState == ESsmStateChargingToAlarm ||
                     reqSubState == ESsmStateChargingToNormal ) ) ||
                 ( curSubState == ESsmStateAlarm &&
                   ( reqSubState == ESsmStateAlarmToCharging ||
                     reqSubState == ESsmStateAlarmToNormal ) ) )
                {
                return ETrue;
                }
            }
        else if ( reqMainState == ESsmNormal )
            {
            if ( ( curMainState == ESsmStartup &&
                   curSubState == ESsmStateNonCritical ) )
                {
                return ETrue;
                }
            }
        else if ( reqMainState == ESsmFail ||
                  reqMainState == ESsmShutdown )
            {
            return ETrue;
            }
        }

    return EFalse;
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::TargetSubState
// ---------------------------------------------------------------------------
//
TUint16 CSsmStartupPolicy::TargetSubState( const TUint16 aRequestedSubState )
    {
    FUNC_LOG;
    INFO_1( "Requested sub state %04x", aRequestedSubState );

	return ( aRequestedSubState == KSsmAnySubState ) ?
        ESsmStatePreUiServices : aRequestedSubState;
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::CSsmStartupPolicy
// ---------------------------------------------------------------------------
//
CSsmStartupPolicy::CSsmStartupPolicy()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::ConstructL
// ---------------------------------------------------------------------------
//
void CSsmStartupPolicy::ConstructL()
    {
    FUNC_LOG;

	// Read the hardware reason
	User::LeaveIfError( RProperty::Get(
	    KUidSystemCategory, KSystemStartupModeKey, iHardwareReason ) );

    BaseConstructL();
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::IsSimlessOfflineSupported
// ---------------------------------------------------------------------------
//
TBool CSsmStartupPolicy::IsSimlessOfflineSupported()
    {
    FUNC_LOG;
    TBool ret( EFalse );

    ret = ( iUtil->FeatureStatus( TUid::Uid( KFeatureIdOfflineMode ) ) ||
            iUtil->FeatureStatus( TUid::Uid( KFeatureIdFlightMode ) ) ) &&
            iUtil->FeatureStatus( TUid::Uid( KFeatureIdFfSimlessOfflineSupport ) );

    INFO_1( "Simless offline supported = %d", ret );
    return ret;
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::SelectStateAfterSecurityCheck
// ---------------------------------------------------------------------------
//
TUint16 CSsmStartupPolicy::SelectStateAfterSecurityCheck()
    {
    FUNC_LOG;

    TInt value( ESimStatusUninitialized );
    TInt errorCode = RProperty::Get( iUtil->PsUid( KPSUidStartup ), KPSSimStatus, value );
    ERROR( errorCode, "Failed to read KPSUidStartup::KPSSimStatus P&S key" );
    INFO_1( "Current SIM status = %d", value );
    TUint16 retVal( ESsmStateEmergencyCallsOnly );
    if ( errorCode == KErrNone )
        {
        if ( value == ESimUsable || value == ESimNotSupported )
            {
            retVal = ESsmStateNonCritical;
            }
        else if ( value == ESimNotPresent && IsSimlessOfflineSupported() )
            {
            retVal = ESsmStateNonCritical;
            }
        }

    INFO_1( "State selected after security check = %04x", retVal );
    return retVal;
    }


// ---------------------------------------------------------------------------
// CSsmStartupPolicy::ClearResetCounter
// ---------------------------------------------------------------------------
//
void CSsmStartupPolicy::ClearResetCounter()
    {
    FUNC_LOG;

    TRAPD_ERR( errorCode, ClearResetCounterL() );
    ERROR( errorCode, "Failed to clear reset counter" );
    }


// ---------------------------------------------------------------------------
// ClearResetCounterL
// ---------------------------------------------------------------------------
//
void CSsmStartupPolicy::ClearResetCounterL()
    {
    FUNC_LOG;

    CRepository* repository = CRepository::NewLC( iUtil->CrUid( KCRUidSsmStartupPolicy ) );
    User::LeaveIfError( repository->Set( KSsmStartupErrorResetCounter, 0 ) );
    CleanupStack::PopAndDestroy( repository );
    }
