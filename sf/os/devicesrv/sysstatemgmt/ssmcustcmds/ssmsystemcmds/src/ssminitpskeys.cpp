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
* Description:
* Implementation of CSsmInitPsKeys class.
*
*/

#include "ssminitpskeys.h"
#include "ssmmapperutility.h"
#include "ssmmapperutilityinternalpskeys.h"
#include "ssmcustcmdsprivatepskeys.h"
#include "trace.h"

#include <ssm/startupdomainpskeys.h>
#include <ssm/starterdomaincrkeys.h>
#include <ssm/ssmrefcustomcmdcommon.h>

_LIT_SECURITY_POLICY_C1( KReadDeviceDataPolicy, ECapabilityReadDeviceData );
_LIT_SECURITY_POLICY_C1( KWriteDeviceDataPolicy, ECapabilityWriteDeviceData );
_LIT_SECURITY_POLICY_C1( KPowerMgmtPolicy, ECapabilityPowerMgmt );
_LIT_SECURITY_POLICY_PASS( KAlwaysPassPolicy );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmInitPsKeys::NewL
// ---------------------------------------------------------------------------
//
CSsmInitPsKeys* CSsmInitPsKeys::NewL()
	{
    FUNC_LOG;
    return new ( ELeave ) CSsmInitPsKeys();
	}


// ---------------------------------------------------------------------------
// CSsmInitPsKeys::~CSsmInitPsKeys
// ---------------------------------------------------------------------------
//
CSsmInitPsKeys::~CSsmInitPsKeys()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CSsmInitPsKeys::Initialize
// ---------------------------------------------------------------------------
//
TInt CSsmInitPsKeys::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
    FUNC_LOG;
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CSsmInitPsKeys::Execute
// ---------------------------------------------------------------------------
//
void CSsmInitPsKeys::Execute(
    const TDesC8& /*aParams*/,
    TRequestStatus& aRequest )
    {
    FUNC_LOG;
    aRequest = KRequestPending;

    TRAPD( errorCode, InitializeKeysL() );
    ERROR( errorCode, "Failed to initialize P&S keys" );

    TRequestStatus* request = &aRequest;
    User::RequestComplete( request, errorCode );
    }


// ---------------------------------------------------------------------------
// CSsmInitPsKeys::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CSsmInitPsKeys::ExecuteCancel()
    {
    FUNC_LOG;
    // Nothing to do.
    }


// ---------------------------------------------------------------------------
// CSsmInitPsKeys::Close
// ---------------------------------------------------------------------------
//
void CSsmInitPsKeys::Close()
    {
    FUNC_LOG;

    /**
    * Close resources initialized in the previous call to Initialize.
    */
    }


// ---------------------------------------------------------------------------
// CSsmInitPsKeys::Release
// ---------------------------------------------------------------------------
//
void CSsmInitPsKeys::Release()
    {
    FUNC_LOG;

	delete this;
    }

// ---------------------------------------------------------------------------
// CSsmInitPsKeys::InitializeKeysL
// ---------------------------------------------------------------------------
//
void CSsmInitPsKeys::InitializeKeysL()
    {
    FUNC_LOG;

// Startup Security Status API

    INFO( "KPSUidStartup::KStartupSimSecurityStatus" );
    DefineL( KPSUidStartup,
             KStartupSimSecurityStatus,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup,
          KStartupSimSecurityStatus,
          ESimSecurityStatusUninitialized );

    INFO( "KPSUidStartup::KStartupSimLockStatus" );
    DefineL( KPSUidStartup,
             KStartupSimLockStatus,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup,
          KStartupSimLockStatus,
          ESimLockStatusUninitialized );

    INFO( "KPSUidStartup::KStartupSecurityCodeStatus" );
    DefineL( KPSUidStartup,
             KStartupSecurityCodeStatus,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup,
          KStartupSecurityCodeStatus,
          ESecurityCodeStatusUninitialized );

    INFO( "KPSUidStartup::KStartupSecurityCodeQueryStatus" );
    DefineL( KPSUidStartup,
             KStartupSecurityCodeQueryStatus,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup,
          KStartupSecurityCodeQueryStatus,
          ESecurityQueryUninitialized );

// System Application Startup API

    INFO( "KPSUidStartup::KStartupBootIntoOffline" );
    DefineL( KPSUidStartup,
             KStartupBootIntoOffline,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup,
          KStartupBootIntoOffline,
          EBootIntoOnlineModeUninitialized );

    INFO( "KPSUidStartup::KStartupCleanBoot" );
    DefineL( KPSUidStartup,
             KStartupCleanBoot,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup, KStartupCleanBoot, EStartupCleanBootUninitialized );

// SIM Utils API

    INFO( "KPSUidStartup::KPSSimStatus" );
    DefineL( KPSUidStartup,
             KPSSimStatus,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup, KPSSimStatus, ESimStatusUninitialized );

    INFO( "KPSUidStartup::KPSSimOwned" );
    DefineL( KPSUidStartup,
             KPSSimOwned,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup, KPSSimOwned, ESimOwnedUninitialized );

    INFO( "KPSUidStartup::KPSSimChanged" );
    DefineL( KPSUidStartup,
             KPSSimChanged,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup, KPSSimChanged, ESimChangedUninitialized );

// System State API

    INFO( "KPSUidStartup::KPSGlobalSystemState" );
    DefineL( KPSUidStartup,
             KPSGlobalSystemState,
             KAlwaysPassPolicy,
             KPowerMgmtPolicy );
    INFO( "KPSUidStartup::KPSGlobalStartupMode" );
    DefineL( KPSUidStartup,
             KPSGlobalStartupMode,
             KAlwaysPassPolicy,
             KPowerMgmtPolicy );

    INFO( "KPSUidStartup::KPSIdlePhase1Ok" );
    DefineL( KPSUidStartup,
             KPSIdlePhase1Ok,
             KAlwaysPassPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup, KPSIdlePhase1Ok, EIdlePhase1NOK );

    INFO( "KPSUidStartup::KPSPhonePhase1Ok" );
    DefineL( KPSUidStartup,
             KPSPhonePhase1Ok,
             KAlwaysPassPolicy,
             KPowerMgmtPolicy );
    SetL( KPSUidStartup, KPSPhonePhase1Ok, EPhonePhase1NOK );

    INFO( "KPSUidStartup::KPSStartupReason" );
    DefineL( KPSUidStartup,
             KPSStartupReason,
             KAlwaysPassPolicy,
             KPowerMgmtPolicy );

// Wakeup Alarm API
    INFO( "KPSUidStartup::KPSWakeupAlarmStatus" );
    DefineL( KPSUidStartup,
             KPSWakeupAlarmStatus,
             KAlwaysPassPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup, KPSWakeupAlarmStatus, EWakeupAlarmUninitialized );
    
    // Startup Security Phase API
    INFO( "KPSStarterUid::KStarterSecurityPhase" );
    DefineL( KPSStarterUid,
             KStarterSecurityPhase,
             KAlwaysPassPolicy,
             KWriteDeviceDataPolicy );
    
    SetL( KPSStarterUid, KStarterSecurityPhase, EStarterSecurityPhaseUninitialized );
    
// Disk Reserver API
    INFO( "KPSStarterUid::KDiskReserveKey" );
    DefineL( KPSStarterUid,
             KDiskReserveKey,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSStarterUid, KDiskReserveKey, 0 );
    
// RTC validation API
    INFO( "KPSStarterUid::KRTCPropertyKey" );
    DefineL( KPSStarterUid,
             KRTCPropertyKey,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSStarterUid, KRTCPropertyKey, 0 );
    

    INFO( "KPSUidStartup::KIgnoreSecurityEvent" );
    DefineL( KPSUidStartup,
             KIgnoreSecurityEvent,
             KReadDeviceDataPolicy,
             KWriteDeviceDataPolicy );
    SetL( KPSUidStartup,
          KIgnoreSecurityEvent,
          EPSIgnoreSecurityEventUninitialized );
    
// Store FirstBoot Value from cenrep
TInt csVal( 0 );
TInt psVal( 0 );
    CSsmMapperUtility& util = MapperUtilityL(); 
    User::LeaveIfError( util.CrValue( KCRUidStartup, 
                                      KStartupFirstBoot,
                                      csVal ) );

    INFO( "KPSUidStartup::KPSStartupFirstBoot" );
    psVal = ( csVal == EStartupIsFirstBoot )? EPSStartupFirstBoot : EPSStartupNotFirstBoot;
    DefineL( KPSUidStartup,
             KPSStartupFirstBoot,
             KAlwaysPassPolicy,
             KWriteDeviceDataPolicy );
             
    SetL(  KPSUidStartup, KPSStartupFirstBoot, psVal );
    }
