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

#ifndef HWRMLIGHTCOMMONDATA_H
#define HWRMLIGHTCOMMONDATA_H

//  INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include <e32property.h>
#include "cenrepobserver.h"
#include "HWRMLightsTimeoutObserver.h"
#include "HWRMLight.h"
#include "HWRMCmn.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMLightService;
class CHWRMPluginHandler;
class CHWRMReservationHandler;
class CHWRMTargetModifierPlugin;

// CLASS DECLARATION
/**
* Container for Light data that is common for all sessions.
* Also handles Light related cenrep and P&S data.
* HWRM server upkeeps the instance.
*
*/
class CHWRMLightCommonData : public CBase,
                             public MCenRepObserver,
                             public MHWRMLightsTimeoutObserver
    {
    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler Pointer to plugin handler for light
        * @param aReservationHandler Pointer to reservation handler for light
        */
        static CHWRMLightCommonData* NewL(CHWRMPluginHandler* aPluginHandler,
                                          CHWRMReservationHandler* aReservationHandler);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMLightCommonData();
        
    public: // New functions
    
        /**
        * Getter for default sensor sensitivity
        *
        * @return Default sensor sensitivity
        */
        TInt DefaultSensitivity();

        /**
        * Getter for light sensor enable status
        *
        * @return Light sensor enable status
        */
        TBool SensorEnabled();
        
        /**
        * Getter for supported light targets
        *
        * @return Bitmask of supported lights targets.
        */
        inline TInt SupportedTargets() const;

        /**
        * Getter for sensor supporting light targets
        *
        * @return Bitmask of sensor supporting lights targets.
        */
        inline TInt SensorTargets() const;

        /**
        * Getter for full system light target
        *
        * @return Bitmask of full system light targets.
        */
        inline TInt FullSystemTarget() const;

        /**
        * Publish Light status
        *
        * @param aTarget Light target for which the status is published.
        * @param aStatus New Light status to be published.
        */
        void PublishLightStatus(TInt aTarget, CHWRMLight::TLightStatus aStatus);
        
        /**
        * Checks what the default state for lights should be
        * based on inactivity timer.
        *
        * @return CHWRMLight::TLightStatus indicating desired state.
        */
        CHWRMLight::TLightStatus DefaultState() const;
        
        /**
        * Modifies target as defined in CHWRMTargetModifierPlugin
        * @see CHWRMTargetModifierPlugin::ModifyTargetL
        */        
        TInt ModifyTargetL(TInt aTarget);
        
    public: // Functions from base classes

        // From MCenRepObserver
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);
        void HandleNotifyError(TUint32 aId, TInt error, CCenRepListener* aHandler);
        
        // From MHWRMLightsTimeoutObserver
        void LightsTimeoutChange(TInt aTimeout);
    
    private:
    
        /**
        * C++ default constructor.
        */
        CHWRMLightCommonData();

        /**
        * By default Symbian 2nd phase constructor is private.
        *
        * @param aPluginHandler Pointer to plugin handler for light
        * @param aReservationHandler Pointer to reservation handler for light
        */
        void ConstructL(CHWRMPluginHandler* aPluginHandler,
                        CHWRMReservationHandler* aReservationHandler);       
        
        /**
        * Sets iDefaultIntensity within allowed limits.
        * Also updates the value to plugin.
        * @param aValue Value to set as the intensity.
        * @param aConstructCall If ETrue, method is called from ConstructL
        */ 
        void SetDefaultIntensity(TInt aValue, TBool aConstructCall = EFalse);
        
        /**
        * Sets iDefaultSensitivity within allowed limits
        * Also updates the value to plugin.
        * @param aValue Value to set as the sensitivity
        * @param aConstructCall If ETrue, method is called from ConstructL
        */ 
        void SetDefaultSensitivity(TInt aValue, TBool aConstructCall = EFalse);
        
        /**
        * Restarts notifications and re-reads values from cenrep.
        * Used at startup and if there is an error with notifiers.
        */
        void ResetCenrepL();

        /**
        * Cleans up central repository related parameters.
        */
        void CleanCenrep();
        
        /**
        * Finds item from iStatusList by target.
        */
        static TInt FindByTarget( const TInt* aTarget, 
                                  const THWRMStatusInfo& aItem );

    private:
    
        TInt                   iDefaultIntensity;         // Default intensity for lights
        TInt                   iDefaultSensitivity;       // Default sensor sensitivity for lights 
        TBool                  iSensorOn;                 // ETrue if sensor is enabled
        TBool                  iSensorSupported;          // ETrue if sensor is supported by platform
        TInt                   iSupportedTargets;         // Supported targets mask
        CRepository*           iRepository;               // Cenrep client
        CCenRepListener*  iIntensityNotifyHandler;   // Cenrep notifyhandler
        CCenRepListener*  iSensitivityNotifyHandler; // Cenrep notifyhandler
        CCenRepListener*  iSensorNotifyHandler;      // Cenrep notifyhandler
        RProperty              iLightStatusProperty;      // P&S property for publishing light status
        TInt                   iInactivityLimit;          // Cached inactivity limit
        CHWRMLightsTimeoutObserver* iLightsTimeoutObserver; // Lights timeout observer
        CHWRMLightService*     iLightService;             // Light service used to notify plugin about 
                                                          // default intensity and sensitivity changes
        CHWRMTargetModifierPlugin* iTargetModifier;       // Target modifier plugin
        TInt                   iFullSystemTarget;         // Full system target, obtained from TM plugin
        TInt                   iSensorTargets;            // Targets that support sensor
        RLightStatusArray      iStatusList;               // Cached light status

    };

#include "HWRMLightCommonData.inl"

#endif  // HWRMLIGHTCOMMONDATA_H
            
// End of File
