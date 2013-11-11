/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef HWRMFMTXCOMMONDATA_H
#define HWRMFMTXCOMMONDATA_H

//  INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include "HWRMFmTxData.h"
#include "HWRMFmTxAudioPolicyDomainPSKeys.h"
#include "HWRMPSChangeObserver.h"
#include "HWRMFmTxCommands.h"
#include "HWRMPluginHandler.h"
#include "HWRMFmTxPowersaveTimer.h"


// FORWARD DECLARATIONS
class CHWRMFmTxRdsTextConverter;
class CHWRMFmTxStateUtility;

// CLASS DECLARATION

/**
* Container for FM Tx data that is common for all sessions.
* Also handles FM Tx related cenrep and P&S data.
* HWRM server upkeeps the instance.
*/
class CHWRMFmTxCommonData : public CBase, 
                            public MHWRMIndicationHandler
    {
    public:
    
        /**
        * Possible power save reasons
        * 0: power-save off
        * Non-zero: power-save on
        */
        enum TFmTxPowerSaveFlag
            {
            EHWRMFmTxPowerSaveOff        = 0,
            EHWRMFmTxPowerSaveInactivity = 1, // due to inactivity timeout
            EHWRMFmTxPowerSaveAccessory  = 2  // due to mutually exclusive accessory connected
            };

        enum TFmTxStateTransition
            {
            EFmTxStateTransError        = 0, // Unknown error event 
            EFmTxStateTransOff          = 1, // HW disabled event       
            EFmTxStateTransRequestedOff = 2, // User requested disable event                
            EFmTxStateTransOn           = 3, // HW enabled event 
            EFmTxStateTransScanning     = 4  // RSSI scanning in progress
            };
    
        // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler Pointer to plugin handler for FM Tx
        * @param aReservationHandler Pointer to reservation handler for FM Tx
        */
        static CHWRMFmTxCommonData* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CHWRMFmTxCommonData();
        
    public: // From MHWRMIndicationHandler
                
        /**
        * Process indications from from plugin handler
        *
        * @param aId  The indication ID
        * @param aData  Data associated with the indication. 
        *
        */
        virtual void ProcessIndicationL( const TUint32 aId, TDesC8& aData );
        
    public: // New functions

        /**
        * Getter for Tx status
        *
        * @return TFmTxState The current status.
        */
        inline TFmTxState Status() const;       
        
        /**
        * Tests whether FM Tx is available as an audio output, in a given state
        *
        * @param aState The FM Tx state to test
        * @return THWRMFmTxAvailableFlag FM Tx available status
        */
        THWRMFmTxAvailableFlag FmTxAvailableState( TFmTxState aState ) const;

        /**
        * Tests whether FM Tx hardware is currently powered up
        *
        * @return TBool ETrue if FM Tx has been powered up
        */
        inline TBool IsFmTxHardwareOn() const;

        /**
        * Returns whether the FM Tx is considered to be powered up in the given state.
        *
        * @param aState The FM Tx state to be evaluated.
        * @return TBool ETrue if the FM Tx is active in the given state
        */      
        inline TBool IsFmTxHardwareOn( TFmTxState aState ) const;

        /**
        * Reads and updates the power save flag set by Audio Policy (for any reason)
        * Method is needed for getting up-to-date state information, because
        * observer is not run yet in all cases, where status is already changed.
        *
        * @param aState The FM Tx power save state               
        */
        void UpdatePowerSaveState( TFmTxPowerSaveFlag aState );
        
        /**
        * Tests whether the power save flag has been set by Audio Policy (for any reason)
        *
        * @return TBool ETrue if FM Tx power save flag is set
        */
        TBool IsPowerSaveOn();     
        
        /**
        * Tests whether an access the power save flag has been set by Audio Policy, due 
        * to a mutually exclusive accessory connection.
        *
        * @return TBool ETrue if FM Tx power save flag has been set to EHWRMFmTxPowerSaveAccessory
        */
        TBool IsAccessoryPowerSaveOn();        

        /**
        * Tests whether FM Tx is currently an active audio output
        * This is determined by Audio Policy
        *
        * @return TBool ETrue if FM Tx audio routed flag is set
        */
        TBool IsAudioRouted();             

        /**
        * Publish FM Tx status
        *
        * @param aEvent Event which requires status update
        */
        void UpdateStatus(TFmTxStateTransition aEvent);

        /**
        * Query whether frequency range has been updated.
        * Initially CenRep does not contain valid frequency range settings.
        * @return TBool ETrue if frequency range has been updated.
        */
        inline TBool FrequencyRangeUpdated() const;
  
        /**
        * Setter for Tx frequency range.
        * This should be called once (only), before calls to GetFrequencyRange are valid.
        */
        void SetFrequencyRange(TInt aMinFrequency, TInt aMaxFrequency, TInt aStepSize);

        /**
        * Getter for Tx frequency.
        *
        * @return TInt The current frequency.
        */
        inline TInt Frequency() const;

        /**
        * Publish FM Tx status.  Note that frequency values
        * are persistent i.e. stored in CenRep (not P&S)
        *
        * @param aFrequency The new frequency to be published.
        */
        void UpdateFrequency(TInt aFrequency);

        /**
        * Get pointer to Unicode->RDS text converter.
        *
        * @return CHWRMFmTxRdsTextConverter* Pointer to text converter
        */
        inline CHWRMFmTxRdsTextConverter* GetRdsTextConverter();

        /**
        * Getter for current RDS data
        *
        * @return TFmTxRdsData The current RDS data
        */
        inline TFmTxRdsData RdsData() const;

        /**
        * Getter for RDS PS string
        *
        * @param On return, the PS string (8bit RDS format)
        */
        inline const TDesC8& ConvertedRdsPs() const;

        /**
        * Getter for RDS PS string
        *
        * @param On return, the PTYN string (8bit RDS format)
        */
        inline const TDesC8& ConvertedRdsPtyn() const;

        /**
        * Queries whether RDS data has been set, since last boot
        *
        * @return TBool EFalse if RDS has not been set
        */
        inline TBool IsRdsInitialised() const;

        /**
        * Getter for power save timout
        *
        * @return Timeout in seconds
        */
        inline TUint32 PowerSaveTimeout() const;

        /**
        * Setter for RDS PTY data
        *
        * @param TRdsProgrammeType The new PTY
        */
        void UpdateRdsPty(const TRdsProgrammeType aData);
        
        /**
        * Setter for RDS PTYN data
        *
        * @param  The new PTYN
        */
        void UpdateRdsPtyn(const TRdsProgrammeTypeName& aData);

        /**
        * Setter for RDS MS data
        *
        * @param  The new Music/Speech flag
        */                  
        void UpdateRdsMs(const TBool aMusic);
        
        /**
        * Setter for RDS language ID data
        *
        * @param  The new language ID
        */          
        void UpdateRdsLanguageId(const TRdsLanguageIdType aLanguageId);   
            
    private:
    
        /**
        * C++ default constructor.
        */
        CHWRMFmTxCommonData();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();       

        /**
        * Starts notifications and reads values from cenrep.  Used at startup.
        */
        void InitCenrepL();

        /**
        * Cleans up central repository related parameters.
        */
        void CleanCenrep();
        
        /**
        * Updates the common state data, after Audio Policy flag has been set/cleared.
        * Used by CHWRMFmTxStateUtility
        */
        void UpdateAudioStatus( TInt aAudioRoutedFlag );

    private:

        CRepository*           iRepository;              // Cenrep client
        RProperty              iFmTxStatusProperty;      // P&S property for publishing FM Tx status
        RProperty              iFmTxIsAvailableProperty; // P&S property for notifications to Audio Policy
        RProperty              iFmTxPowerSaveProperty;   // P&S property for checking Audio Policy published power save state
        RProperty              iFmTxAudioRoutingProperty;// P&S property for checking Audio Policy published audio routing state
        TFmTxState             iFmTxState;               // Cached FM Tx state
        TInt                   iFmTxFrequency;           // Cached FM Tx frequency
		TBool                  iFrequencyRangeUpdated;   // Flags whether frequency range is valid in CenRep
        
        // Audio Policy publishes these state variables                                                         
        TFmTxPowerSaveFlag     iFmTxPowerSaveFlag;   // Cached power save flag
        TBool                  iFmTxAudioRoutedFlag; // Cached audio routed flag

        TSecureId              iFmTxAudioRoutingSid;  // Configurable SID protection for KHWRMFmTxAudioRoutedFlag
        TUint32                iFmTxPowerSaveTimeout; // Timeout configuration as micro seconds for power save mode.

        // RDS
        TBool                      iRdsInitialised;   // Flag to check whether RDS (particularly PS) has been set
        CHWRMFmTxRdsTextConverter* iRdsTextConverter; // Converts unicode <-> 8bit RDS text. Owned.
        TFmTxRdsData               iRdsData;          // RDS settings
        HWRMFmTxCommand::TRdsPs    iRdsConvertedPs;   // Text converted into 8bit RDS format
        HWRMFmTxCommand::TRdsPtyn  iRdsConvertedPtyn; // Text converted into 8bit RDS format
        
    public: // Friend classes
    
        friend class CHWRMFmTxStateUtility;
    };

#include "HWRMFmTxCommonData.inl"

#endif  // HWRMFMTXCOMMONDATA_H
            
// End of File
