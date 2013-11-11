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
 * CTVOutConfigForHDMI class declaration.
 *
 */

#ifndef TVOUTCONFIGFORHDMI_H
#define TVOUTCONFIGFORHDMI_H

// INCLUDES
#include <e32base.h>
#include "multifinitestatemachine.h"
#include "myasynconeshottimercallback.h"
#include "pdengine.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CTvOutConfig;
class MFSMForBody;
class CMyAsyncOneShotTimer;
struct THdmiDviTimings;
struct TTvSettings;
class TSupportedHdmiDviMode;

/** Returned struct for function CTVOutConfigForHDMI::ListenCopyProtectionIfNoMissedEvents(). */
struct TCopyProtctListenRetValues
    {
    TInt iError;
    TFSMEventId iFSMEventId;
    };

// CLASS DECLARATION
/**
 *  TV Out Configurer for HDMI Cable Status FSM.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CTVOutConfigForHDMI ): public CActive, public MMyAsyncOneShotTimerCallback
    {
    public:  // Constructors and destructor

        /**
         * Symbian two phased constructor.
         * 
         * @since S60 TB9.2
         * @param aFSM A FSM.
         * @return CTVOutConfigForHDMI*
         */
        static CTVOutConfigForHDMI* NewL( MFSMForBody& aFSM );

        /**
        * Destructor.
        */
        virtual ~CTVOutConfigForHDMI();
        
    public: // New methods
        
        /**
         * Start listening HDMI Cable connected/disconnected situations if status
         * haven't changed while not listening.
         * 
         * @since TB9.2
         * @param None.
         * @return TFSMEventId Event that has occured while not listening:
         *          EIfTVOutConfigEventUndefined No event occurred,
         *          EIfTVOutConfigEventCableConnected Connection occurred,
         *          EIfTVOutConfigEventCableDisconnected Disconnection occurred
         *          
         */
        TFSMEventId ListenHDMICableStatusIfNoMissedEvents();

        /**
         * Set list of available Tv-configurations.
         * 
         * @since TB9.2
         * @param  aAnalogConfigs  Available Analog Tv configurations.
         * @param  aHdmiConfigs    Available HDMI/DVI Tv configurations
         * @return Symbian error code.
         */
        TInt SetAvailableTvConfigList(const RArray<TTvSettings>& aAnalogConfigs, const RArray<THdmiDviTimings>& aHdmiConfigs);

        /**
         *  Enables Tv-out. Latest SetConfig() settings are used.
         *
         * @since TB9.2
         * @param  None.
         * @return void.
         */
        void Enable();

        /**
         * Disables Tv-out.
         * 
         * @since TB9.2
         * @param  None.
         * @return void.
         */
        void Disable();
        
        /**
         * Start listening for disconnection of HDMI Cable. Connection has failed and
         * cable has been rejected. Now waiting for end user to disconnect.
         * 
         * @since TB9.2
         * @param None.
         * @return void
         *          
         */
        void ListenForDisconnection();

        /**
         * Get pointer of CTvOutConfig.
         * 
         * @since TB9.2
         * @param  None.
         * @return CTvOutConfig* Pointer to CTvOutConfig.
         */
        CTvOutConfig* GetTvOutConfig();

        /**
         * Start listening Copy Protection status if status
         * haven't changed while not listening.
         * 
         * @since TB9.2
         * @param None.
         * @return TCopyProtctListenRetValues
         *          iError error code, KErrNone if OK
         *          iFSMEventId Event that has occured while not listening:
         *          EIfTVOutConfigEventUndefined No event occurred,
         *          EIfTVOutConfigEventCopyProtectionOn Copy Protection set on,
         *          EIfTVOutConfigEventCableCopyProtectionOff Copy Protection set off
         *          
         */
        TCopyProtctListenRetValues ListenCopyProtectionIfNoMissedEvents();

        /**
         * Get latest recorded Copy Protection status.
         * 
         * @since TB9.2
         * @param  None.
         * @return TBool Copy Protection status.
         *           
         */
        TBool GetLatestRecordedCopyProtectionStatus() const;

        /**
         * Reset latest recorded Copy Protection status.
         * 
         * @since TB9.2
         * @param  None.
         * @return void.
         *           
         */
        void ResetLatestRecordedCopyProtectionStatus();

        /**
         * Listen for setting changes in TV Out Configuration.
         * 
         * @since TB9.2
         * @param  None.
         * @return void.
         *           
         */
        void ListenSettingsChanges();

        /**
         * Get the HW supported HDMI display modes
         * 
         * @since TB9.2
         * @param  asupportedModes   place holder for getting the list of display modes.
         * @return KErrNone if OK. KErrNotFound if the filtered list is empty.
         *           
         */
		TInt GetSupportedHdmiModes( RArray<TSupportedHdmiDviMode>& asupportedModes );

    private:  // Constructors

        /**
        * C++ constructor.
        * @param aFSMForBody A FSM.
        */
        CTVOutConfigForHDMI( MFSMForBody& aFSM );

        /**
        * Symbian 2nd phase constructor.
        * 
        */
        void ConstructL();

        /**
        * Provides Hdmi Dvi Timings
        */
        void ReadHdmiDviTimings( THdmiDviTimings& aSetting );

        /**
         * From MMyAsyncOneShotTimerCallback
         * This is called when one shot asyncronous timer has elapsed.
         * 
         * @since TB9.2
         * @param  None.
         * @return void.
         *           
         */
        virtual void OneShotTimeout();

    private: // Functions from base classes        
        
        enum TOpt
          {
          EOptStartListenHdmiCable,
          EOptListenHdmiCable,
          EOptEnableTv,
          EOptDisableTv,
          EOptIdle,
          EOptListenCopyProtection,
          EOptStartListenForDisconnection,
          EOptListenForDisconnection,
          EOptStartListenForSettingsChanges,
          EOptListenForSettingsChanges
          };

        /**
        * RunL()
        */
        void RunL();

        /**
        * DoCancel()
        */
        void DoCancel();

        /**
        * RunError(aError)
        */
        TInt RunError(TInt aError);
        
    private: // Functions from base classes


    private:  // Data
        
        enum
        {
        EWaitTimeInMsForTvOutConfigBusyServer = 50000
        };
                
        // Handle to TV-out config
        CTvOutConfig*     iTvConfig;

        // Next operation
        TOpt  iNextOpt;
        
        // A FSM Body.
        // Not own.
        MFSMForBody& iFSM;
        
        // Last recorded Copy Protection status value.
        // Read by calling function: 
        // TBool CTvOutConfig::CopyProtectionStatus() = 0
        TBool iLastRecordedCopyProtectEnabledStatus;
        
        // Asynchronous timer 
        CMyAsyncOneShotTimer* iMyAsyncOneShotTimer;
        
        // Time out reason and next opt after timer has 
        // elapsed
        TOpt  iTimeOutReason;
    };

#endif // TVOUTCONFIGFORHDMI_H
