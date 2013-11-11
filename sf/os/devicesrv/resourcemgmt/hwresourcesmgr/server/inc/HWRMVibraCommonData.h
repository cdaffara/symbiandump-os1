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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMVIBRACOMMONDATA_H
#define HWRMVIBRACOMMONDATA_H

//  INCLUDES
#include <e32base.h>
#include "HWRMVibraProfileObserver.h"
#include "HWRMVibra.h"
#include <e32property.h>
#include "HWRMPSChangeObserver.h"
#include "HWRMVibraCommonService.h"



// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMPluginHandler;
class CHWRMReservationHandler;
class CHWRMVibraService;

// CLASS DECLARATION
/**
* Container for vibra data that is common for all sessions.
* Also handles vibra related cenrep and PS data.
* HWRM server upkeeps the instance.
*
*/
class CHWRMVibraCommonData : public CBase,
                             public MHWRMVibraProfileObserver,
                             public MHWRMPSChangeObserver
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler Pointer to plugin handler for vibra
        * @param aReservationHandler Pointer to reservation handler for vibra
        * @param aCommonService Common vibra service instance for all sessions
        */
        static CHWRMVibraCommonData* NewL(CHWRMPluginHandler* aPluginHandler,
                                          CHWRMReservationHandler* aReservationHandler,
                                          CHWRMVibraCommonService& aCommonService);

        /**
        * Destructor.
        */
        virtual ~CHWRMVibraCommonData();
        
    public: // New functions
    
        /**
        * Getter for maximum time of vibra can be on in one go
        * before it is automatically turned off.
        *
        * @return Maximum on time for vibra 
        */
        inline TTimeIntervalMicroSeconds32 MaximumOnTime() const;


        /**
        * Getter for feedback default intensity.
        *
        * @return Feedback default intensity
        */
        inline TInt FeedbackDefaultIntensity() const;

        /**
        * Getter for feedback default duration.
        *
        * @return Feedback default duration
        */
        inline TInt FeedbackDefaultDuration() const;

        /**
        * Getter for pulse feedback support.
        *
        * @return Feedback default duration
        */
        inline TInt FeedbackPulseSupported() const;


        /**
        * Getter for vibra setting flag from profile settings.
        *
        * @return Vibra profile setting state
        */
        CHWRMVibra::TVibraModeState VibraState();
        
        /**
        * Getter for vibra feedback setting flag from profile settings.
        *
        * @return Vibra feedback profile setting state
        */
        CHWRMVibra::TVibraFeedbackModeState VibraFeedbackState();
        

        /**
        * Checks if vibra is blocked from being used or if it is
        * having a cool-out period.
        * 
        * @return EFalse if vibra is available, ETrue if vibra
        *         is cooling or blocked.
        */
        TBool VibraLocked() const;

        /**
        * Checks if vibra is blocked for some reason.
        * 
        * @return EFalse if vibra has no blocks, otherwise 
        *         p&s mask of vibra blocking source.
        */
        TInt VibraBlocked( ) const;

        /**
        * Checks if vibra is blocked from specified reason.
        * 
        * @param aMask Block mask to be checked.
        * @return EFalse if vibra has no blocks for specified
        *         reason, otherwise mask of vibra blocking source.
        */
        TBool VibraBlocked( TInt aMask ) const;      

        /**
        * Locks vibra for minimum lock period.
        */
        void LockVibra();
        
        /**
        * Publish Vibra status
        *
        * @param aStatus New Vibra status to be published.
        * @param aPrivilegedClient Indicates if client is privileged
        */
        void PublishVibraStatus(CHWRMVibra::TVibraStatus aStatus,
                                TBool aPrivilegedClient = EFalse);
        
        /**
        * Checks if supplied SID matches privileged SID
        *
        * @param aClientSid SID to check against privileged SID
        */
        TBool IsPrivilegedSid(const TSecureId& aClientSid);


    public: // Functions from base classes

        // From MHWRMVibraProfileObserver
        void VibraModeChange(CHWRMVibra::TVibraModeState newState);
        void VibraFeedbackModeChange(CHWRMVibra::TVibraFeedbackModeState newState);
        
        // From MHWRMPSChangeObserver
        void PSValueChanged(const TUid& aUid, TUint32 aKey, TInt aValue);

    private:
    
        /**
        * C++ default constructor.
        */        
        CHWRMVibraCommonData(CHWRMVibraCommonService& aCommonService);

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aPluginHandler Pointer to plugin handler for vibra
        * @param aReservationHandler Pointer to reservation handler for vibra
        */
        void ConstructL(CHWRMPluginHandler* aPluginHandler,
                        CHWRMReservationHandler* aReservationHandler);        

    private:
    
        TTimeIntervalMicroSeconds32 iMinimumOffTime;      // Minimum offtime after maximum ontime
        TTimeIntervalMicroSeconds32 iMaximumOnTime;       // Maximum continuous ontime for vibra
        CHWRMVibra::TVibraModeState iVibraSetting;        // Flag to see if vibra is enabled in profile    
        CHWRMVibra::TVibraFeedbackModeState iVibraFeedbackSetting; // Flag to see if feedback vibration is enabled in profile    
        TInt                        iFeedbackIntensity;   // Feedback vibration default intensity 
        TInt                        iFeedbackDuration;    // Feedback vibration default duration 
        CHWRMVibraProfileObserver*  iProfileObserver;     // Vibra profile observer pointer. Owned.
        TTime                       iVibraLockedUntil;    // Vibra cannot be set on before this time.
        RProperty                   iProperty;            // P&S property for publishing vibra status
        CHWRMVibraService*          iVibraService;        // Vibra service used to stop vibra when
                                                          // someone blocks it via HW Resource Blocking API
        CHWRMPSChangeObserver*      iAccessoryBlockObserver;  // Observes accessory blocking of vibra    
        CHWRMPSChangeObserver*      iInternalBlockObserver;   // Observes HWRM internal blocking of vibra    
        TUint32                     iVibraBlockMask;      // Blocking of vibra is indicated by this bitmask
        TUint32                     iConfVibraBlockMask;  // Cenrep configured vibra blocking bitmask
        TSecureId*                  iPrivilegedSid;       // Privileged secure id (client with this SID ignores profile checks)
        TBool                       iPrivilegedVibraOn;   // If privileged client has turned vibra on, this bool is set
        TBool                       iPulseSupported;      // Indication whether tactile feedback pulse command handling supported.
        CHWRMVibra::TVibraStatus    iLastPublishedStatus; // Last successfully published vibra status
        CHWRMVibraCommonService&    iCommonService;       // Vibra common service           
    };

#include "HWRMVibraCommonData.inl"

#endif  // HWRMVIBRACOMMONDATA_H
            
// End of File
