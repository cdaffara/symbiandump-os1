/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Interface   : Domain, System State Control API and Extended Startup API
* Declaration of RStarterSession interface
*
*/



#ifndef STARTERCLIENT_H
#define STARTERCLIENT_H

#include <e32std.h>
#include <ssm/starter.hrh>


/*
 *   @publishedPartner
 *   @released
 */

/**
*  Client API for Starter server.
*
*  ?more_complete_description
*
*  lib: StarterClient.lib
*  @since S60 1.0
*/
class RStarterSession : public RSessionBase
    {

public:

    /**
     * Connect to Starter server
     * @return EPOC error code
     */
    IMPORT_C TInt Connect();

public: // System State Control API

    // Parameters for SetState.
    enum TGlobalState
        {
        ENormal = 100,  // Possible in alarm, charging, BT SAP and
                        // offline states, but not in simless offline.
        EAlarm,         // Only possible in charging state.
        ECharging,      // Only possible in alarm state.
        EOffline,       // Possible in normal RF on and BT SAP states.
        EBTSap          // Possible in normal RF on and normal RF off
                        // states, but not in simless offline.
        };

    /**
    * Change global system state.
    * @param aState The state to change to.
    * @return KErrArgument if the state change is illegal (from current state
    * it is not possible to change to the requested state), KErrNone if the
    * global state change has been successfully initiated.
    * If state change from a state to the same state is requested, returns
    * KErrNone but does nothing.
    */
    IMPORT_C TInt SetState( const TGlobalState aState );

    // Parameters for Reset.
    enum TResetReason
        {
        ELanguageSwitchReset = 100,     // A reset due to display language switch.
        ENormalRFSReset = 101,          // A reset due to restoring factory
                                        // settings.
        EDeepRFSReset = 102,            // A reset due to restoring factory
                                        // settings (deep).
        EUnknownReset = 104,            // A reset due to an error.
        EOperatorSettingReset = 105,    // A reset due to removing operator
                                        // settings.
        ENetworkModeChangeReset = 106,  // A reset due to network mode change.
        ESIMStatusChangeReset = 107,    // A reset due to removing/inserting SIM card.
        EDRMReset = 108,                // A reset due to a DRM -related problem.
        EFirmwareUpdate = 109,          // A reset due to firmware update.
        EDataRestoreReset = 110,        // A reset after restoring backed-up data.
        EFieldTestReset = 111           // A reset required by Field Test software.
        };

    /**
    * Execute system reset.
    * @param aReason Indicates the reason for the reset.
    */
    IMPORT_C void Reset( const TResetReason aReason );

    /**
    * Execute system shutdown.
    */
    IMPORT_C void Shutdown();

    /**
    * Change state to offline and back to online.
    *
    * @return
    * - KErrServerTerminated if system is shutting down or reseting,
    * - KErrMemory if the state change could not be completed because of lack of memory,
    * - KErrArgument if global state is not normal RF on,
    * - KErrNone if a network reset is already going on.
    */
    IMPORT_C TInt ResetNetwork();

    /**
    * Check whether RTC time is valid.
    * This function should not be called before critical startup phase is
    * over, and if it is, it will return EFalse.
    * @return ETrue if RTC time is valid, EFalse otherwise.
    */
    IMPORT_C TBool IsRTCTimeValid();

    /**
    * Activate RF for making an emergency call. May be called in all
    * global states, but 'normal RF on' state the call has no effect since
    * RF is already activated.
    * After the emergency call has finished, DeactivateRfAfterEmergencyCall
    * should be called.
    * @return KErrNone on success, one of Symbian error codes otherwise.
    */
    IMPORT_C TInt ActivateRfForEmergencyCall();

    /**
    * Deactivate RF after making an emergency call. May only be called
    * after calling ActivateRfForEmergencyCall.
    * @return KErrNone on success, one of Symbian error codes otherwise.
    */
    IMPORT_C TInt DeactivateRfAfterEmergencyCall();

    /**
    * If Splash Screen is active, send it a signal to exit.
    * @return KErrNone on success, one of Symbian error codes otherwise.
    * Returns an error code only if the client-server call fails.
    */
    IMPORT_C TInt EndSplashScreen();
    };

#endif // STARTERCLIENT_H
