/*
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface   : Domain, System State API
* This file contains Domain Publish & Subscribe definitions of Startup subsystem.
*
*/

#ifndef __STARTUPDOMAINPSKEYS_H__
#define __STARTUPDOMAINPSKEYS_H__

#include <e32property.h>
#include <ssm/startupreason.h>
#include <ssm/simutils.h>

/*
* @file
* @publishedPartner
* @released
*/

const TInt KStartupEnumerationFirstValue = 100;

static const TUid KPSUidStartup = { 0x101F8766 };


// =============================================================================
// SIM Utils API
// =============================================================================

// Use TUid KPSUidStartup = { 0x101F8766 };

/**
* Note: Refer simutils.h for actual definitions.
*/
//const TUint32 KPSSimStatus = 0x00000031;
//const TUint32 KPSSimOwned = 0x00000032;
//const TUint32 KPSSimChanged = 0x00000033;


// =============================================================================
// System State API
// =============================================================================

// Use TUid KPSUidStartup = { 0x101F8766 };

/**
* The system state shared by Symbian OS and Domestic OS.
*/
const TUint32 KPSGlobalSystemState = 0x00000041;
enum TPSGlobalSystemState
    {
    ESwStateStartingUiServices  = KStartupEnumerationFirstValue,
    ESwStateStartingCriticalApps,
    ESwStateSelfTestOK,

    // States for the security check phase.
    ESwStateSecurityCheck,
    ESwStateCriticalPhaseOK,
    ESwStateEmergencyCallsOnly,

    // Terminal states defined by the boot mode (and some other variables such as offline mode).
    ESwStateTest,
    ESwStateCharging,
    ESwStateAlarm,
    ESwStateNormalRfOn,
    ESwStateNormalRfOff,
    ESwStateNormalBTSap,

    // States for notifying adaptation about a terminal state change.
    ESwStateAlarmToCharging,
    ESwStateChargingToAlarm,
    ESwStateChargingToNormal,
    ESwStateAlarmToNormal,

    // Error states.
    ESwStateFatalStartupError,

    // A state for shutdown
    ESwStateShuttingDown
    };

/**
* The startup mode shared by Symbian OS and Domestic OS.
*/
const TUint32 KPSGlobalStartupMode = 0x00000042;
enum TPSGlobalStartupMode
    {
    EStartupModeNormal = KStartupEnumerationFirstValue,
    EStartupModeAlarm,
    EStartupModeCharging,
    EStartupModeTest
    };

/**
* Idle screen is ready to be transferred into foreground. Startup animation can be shown.
*/
const TUint32 KPSIdlePhase1Ok = 0x00000043;
enum TPSIdlePhase1Ok
    {
    EIdlePhase1NOK = KStartupEnumerationFirstValue,
    EIdlePhase1Ok
    };

/**
* Phone application is ready for MO/MT calls. RF can be switched on.
*/
const TUint32 KPSPhonePhase1Ok = 0x00000044;
enum TPSPhonePhase1Ok
    {
    EPhonePhase1NOK = KStartupEnumerationFirstValue,
    EPhonePhase1Ok
    };

/**
* Used by Starter to publish the additional startup reason (language switch,
* RFS, etc.) for other components to use.
* The correct startup reason is updated to this key early in start-up, and the
* value stays the same until system is shut down.
*
* Contains one of the values of TStartupReason enumeration defined in
* startupreason.h.
*/
const TUint32 KPSStartupReason = 0x000000045;

/**
* Informs completion of a startup UI sequence phase (animation, graphic, text).
*
* After the key is notified with value EStartupUiPhaseAllDone, clients can begin 
* to use UI services without interfering with startup queries or animation(s).
* 
* Clients must not rely on notification of any other value but EStartupUiPhaseAllDone,
* because if an animation or graphic is not shown its completion is not notified either.
*/
const TUint32 KPSStartupUiPhase = 0x00000046;

enum TPSStartupUiPhase
    {
    EStartupUiPhaseUninitialized = KStartupEnumerationFirstValue, // default value after key definition
    EStartupUiPhaseSystemWelcomeDone, // startup animation has been displayed
    EStartupUiPhaseOperatorWelcomeDone, // operator animation / graphic has been displayed
    EStartupUiPhaseUserWelcomeDone, // user welcome text / graphic has been displayed
    EStartupUiPhaseAllDone // all the startup related queries and graphics has been displayed
    };

// =============================================================================
// Startup Security Status API
// =============================================================================

// Use TUid KPSUidStartup = { 0x101F8766 };

/**
* Indicates the SIM security status
*/
const TUint32 KStartupSimSecurityStatus = 0x00000001;
enum TPSSimSecurityStatus
    {
    ESimSecurityStatusUninitialized = KStartupEnumerationFirstValue,
    ESimRejected,   // The PUK code has been entered incorrectly, so the card is rejected.
    ESimUnaccepted,  // The SIM lock is on, so the card is unaccepted.
    ESimInvalid     //The Sim inserted is not same as the one provided by the operator, so card is invalid.
    };


/**
* Indicates the SIM lock status
*/
const TUint32 KStartupSimLockStatus = 0x00000002;
enum TPSSimLockStatus
    {
    ESimLockStatusUninitialized = KStartupEnumerationFirstValue,
    ESimLockActive,
    ESimLockRestrictionPending,
    ESimLockRestrictionOn,
    ESimLockOk
    };

/**
* Indicates the Security code status, i.e. is the code required or not.
*/
const TUint32 KStartupSecurityCodeStatus = 0x00000003;
enum TPSSecurityCodeStatus
    {
    ESecurityCodeStatusUninitialized = KStartupEnumerationFirstValue,
    ESecurityCodeNotRequired,
    ESecurityCodeRequired,
    ESecurityCodeInitWait
    };

/**
* Used by Security Notifier to notify Startup Application whether the security
* code query is active.
* Old Shared Data constant name: KSysCodeQueryStatus.
*/
const TUint32 KStartupSecurityCodeQueryStatus = 0x00000004;
enum TStartupCodeQueryStatus
    {
    ESecurityQueryUninitialized = KStartupEnumerationFirstValue,
    ESecurityQueryNotActive,
    ESecurityQueryActive
    };

// =============================================================================
// System Startup Control API
// =============================================================================

// Use TUid KPSUidStartup = { 0x101F8766 };

/**
* "Boot into Offline Mode" query reply. Startup App shows the query
* and notifies Starter about the user's choice.
* Old Shared Data constant name: KSysApBootIntoOffline
**/
const TUint32 KStartupBootIntoOffline = 0x00000011;
enum TStartupBootIntoOffline
    {
    EBootIntoOnlineModeUninitialized = KStartupEnumerationFirstValue,
    EBootIntoOnlineMode,
    EBootIntoOfflineMode
    };

/**
* To be used by Startup App for notifying Starter
* of the ongoing clean boot.
* Old Shared Data constant name: KSysApCleanBoot
*/
const TUint32 KStartupCleanBoot = 0x00000012;
enum TStartupCleanBoot
    {
    EStartupCleanBootUninitialized = KStartupEnumerationFirstValue,
    EStartupNormalBoot,
    EStartupCleanBoot
    };

/**  
* To be used by Starter to check whether this is the first boot. This Key will 
* hold the cenrep value same as KStartupFirstBoot , which is persistant across the boots.
*/
const TUint32 KPSStartupFirstBoot = 0x00000013;
enum TPSStartupFirstBoot
    {
    EPSStartupFirstBootUninitialized = KStartupEnumerationFirstValue,
    EPSStartupFirstBoot,
    EPSStartupNotFirstBoot
    };

// =============================================================================
// Splash Screen Shutdown API
// =============================================================================

// Use TUid KPSUidStartup = { 0x101F8766 };
const TInt KSplashScreenShutdownAPIBase = 0x00000301;

/**
* To be used by Startup App for notifying SplashScreen that it should shut down.
* Changing the value of this key has no effect unless SplashScreen is running.
*/
const TUint32 KPSSplashShutdown = KSplashScreenShutdownAPIBase;
enum TPSSplashShutdown
    {
    ESplashRunning = KStartupEnumerationFirstValue,
    ESplashShutdown
    };

// =============================================================================
// Security Event SUP
// =============================================================================
/**
* To be used by security event SUP to decide whether an event to be ignored or not
*/

// Use TUid KPSUidStartup = { 0x101F8766 };
const TUint32 KIgnoreSecurityEvent = 0x00000302;
enum TPSIgnoreSecurityEvent
    {
    EPSIgnoreSecurityEventUninitialized = KStartupEnumerationFirstValue,
    EPSIgnoreSecurityEventEPin1Required,
    EPSIgnoreSecurityEventEPhonePasswordRequired
    };

// =============================================================================
// Wakeup Alarm API
// =============================================================================

// Use TUid KPSUidStartup = { 0x101F8766 };
const TInt KWakeupAlarmAPIBase = 0x00000401;

/**
* Starter server sets this value to EWakeupAlarmSet when there are active
* wakeup alarm(s), and ENoWakeupAlarmsSet when there are no active wakeup
* alarms.
* A wakeup alarm is an alarm which wakes up the terminal is it is powered off
* when the alarm occurs.
* An active wakeup alarm is a wakeup alarm which has been set and has not
* started to alert yet.
*/
const TUint32 KPSWakeupAlarmStatus = KWakeupAlarmAPIBase;
enum TPSWakeupAlarmStatus
    {
    EWakeupAlarmUninitialized = KStartupEnumerationFirstValue,
    EWakeupAlarmSet,
    ENoWakeupAlarmsSet
    };

#endif // __STARTUPDOMAINPSKEYS_H__
