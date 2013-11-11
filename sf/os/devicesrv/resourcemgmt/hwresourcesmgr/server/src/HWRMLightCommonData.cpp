// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include "HWRMtrace.h"
#include "HWRMLightDomainCRKeys.h"
#include "HWRMLightCommonData.h"
#include "HWRMClientServer.h"
#include "HWRMPrivatePSKeys.h"
#include "HWRMConfiguration.h"
#include "HWRMLightClientServer.h"
#include "HWRMLightService.h"
#include "HWRMLightCommands.h"
#include "HWRMFeatureMgr.h"
#include <featureuids.h>


#include "HWRMTargetModifierPlugin.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
/**
* Minimum allowed sensor sensitivity setting for Light
*/
const TInt KHWRMLightMinSensitivity(1);

/**
* Maximum allowed sensor sensitivity setting for Light
*/
const TInt KHWRMLightMaxSensitivity(100);

/**
* Maximum inactivity time for lights on in seconds
*/
const TInt KHWRMLightMaxInactivity(60);

/**
* Minimum inactivity time for lights on in seconds
*/
const TInt KHWRMLightMinInactivity(5);

/**
* Default inactivity time for lights on in seconds.
* Used if observer cannot be set up.
*/
const TInt KHWRMLightDefaultInactivity(15);

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::CHWRMLightCommonData
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMLightCommonData::CHWRMLightCommonData()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightCommonData::CHWRMLightCommonData()" ) );
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightCommonData::CHWRMLightCommonData - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMLightCommonData::ConstructL(CHWRMPluginHandler* aPluginHandler,
                                      CHWRMReservationHandler* aReservationHandler)
    {    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightCommonData::ConstructL()" ) );
    
    //The feature UID NFeature::KAmbientLightSensor is defined in featureuids.h
    iSensorSupported = HWRMFeatureMgr::IsFeatureSupported( NFeature::KAmbientLightSensor );
    // Load target modifier plugin, if such a thing exists.
    // Plugin is not mandatory for HWRM functionality,
    // but the light control logic will be broken if the plugin is dropped from the image.
    TRAPD( err, iTargetModifier = CHWRMTargetModifierPlugin::NewL( KHWRMLightApiEComMatch ) );
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::ConstructL - Target modifier plugin not loaded; cause: %d" ), err );
        }  
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::ConstructL - Sensor support: 0x%x" ), iSensorSupported );
      
    ResetCenrepL();
    
    // Initialize full system target. Ensure that only supported targets are included.
    if ( iTargetModifier )
        {        
        iFullSystemTarget = iTargetModifier->GetFullSystemTarget() & iSupportedTargets;
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::ConstructL - Full System Target: 0x%x" ), iFullSystemTarget );
        }
        
    // Set up status P&S values
    User::LeaveIfError(RProperty::Define(KPSUidHWResourceNotification, KHWRMLightStatus, RProperty::EByteArray, KNoCapability, KHWRMSidOnlyPolicy));
    User::LeaveIfError(iLightStatusProperty.Attach(KPSUidHWResourceNotification, KHWRMLightStatus));
    
    TInt currentTarget( KHWRMLightFirstTarget );
    
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        THWRMStatusInfo info;
        info.iTarget = currentTarget;
        info.iStatus = CHWRMExtendedLight::ELightStatusUnknown;
        iStatusList.AppendL( info );
        // shift the mask and target ready for next loop
        currentTarget <<= 1;
        }
        
    TInt arraySize = ( sizeof( THWRMStatusInfo ) ) * KHWRMLightMaxTargets;
    TPtr8 arrayPtr( ( TUint8* )&iStatusList[0], arraySize, arraySize );
      
    User::LeaveIfError( iLightStatusProperty.Set( arrayPtr ) );
    
    // Construct light service last so that the common data is in order to be used
    // in service construction.
    iLightService = CHWRMLightService::NewL(aPluginHandler, aReservationHandler, *this);
    
    // Reset intensity and sensitivity to update values to plugin, 
    // as lightservice is not yet available when ResetCenrepL was called earlier.
    SetDefaultIntensity(iDefaultIntensity, ETrue);

    if ( iSensorSupported )
        {
        SetDefaultSensitivity(iDefaultSensitivity, ETrue);	
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightCommonData::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMLightCommonData* CHWRMLightCommonData::NewL(CHWRMPluginHandler* aPluginHandler,
                                                 CHWRMReservationHandler* aReservationHandler)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightCommonData::NewL()" ) );

    CHWRMLightCommonData* self = new( ELeave ) CHWRMLightCommonData();
    
    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler, aReservationHandler);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMLightCommonData::~CHWRMLightCommonData()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightCommonData::~CHWRMLightCommonData()" ) );

    delete iLightService;
    iLightService = NULL;
    
    // Destroy target modifier plugin
    delete iTargetModifier;
    iTargetModifier = NULL;
    
    // Clean up central repository related data
    CleanCenrep();

    iLightStatusProperty.Close();

    // NULL these references so that pc-lint will not complain. 
    // They are cleaned in CleanCenrep();
    iIntensityNotifyHandler = NULL;
    iSensitivityNotifyHandler = NULL;
    iSensorNotifyHandler = NULL;
    iRepository = NULL;     
    iLightsTimeoutObserver = NULL;   

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightCommonData::~CHWRMLightCommonData - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::PublishLightStatus
// -----------------------------------------------------------------------------
//
void CHWRMLightCommonData::PublishLightStatus(TInt aTarget, CHWRMLight::TLightStatus aStatus)
    {
    COMPONENT_TRACE3(_L("HWRM Server - CHWRMLightCommonData::PublishLightStatus(0x%x, 0x%x)"), aTarget, aStatus); 

    // Publish Light status to P&S. If publishing fails, something is very wrong
    // with P&S, so ignore it. If it is memory issue, hopefully it will work 
    // next time. If it is not, we can't do anything about it anyway.

    TInt err(KErrNone);
    
    // Loop through target and update correct indexes of iStatusList
    TInt currentTarget(KHWRMLightFirstTarget);
    TInt currentStatus = aStatus;
            
    for ( int i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        if ( aTarget & currentTarget )
            {
            THWRMStatusInfo info;
            info.iTarget = currentTarget;
            info.iStatus = currentStatus;
            const TInt& target( info.iTarget );
            TInt j = iStatusList.FindInOrder( target, FindByTarget ); 
            
            if ( j >= 0 && j < KHWRMLightMaxTargets )
                {
                iStatusList[j].iStatus = currentStatus;   
            }
            }    
        // shift the mask and target ready for next loop
        currentTarget <<= 1;
        }
    
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMLightCommonData::PublishLightStatus - Setting light status property.") ) ); 
    
    TInt arraySize = ( sizeof( THWRMStatusInfo ) ) * KHWRMLightMaxTargets;
    TPtr8 arrayPtr( ( TUint8* )&iStatusList[0], arraySize, arraySize );
      
    err = iLightStatusProperty.Set( arrayPtr );
    
    if ( err != KErrNone )
        {
        COMPONENT_TRACE2(_L("HWRM Server - CHWRMLightCommonData::PublishLightStatus - Property set failed: %d"), err );     
        }
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightCommonData::PublishLightStatus - return")); 
    }

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::DefaultSensitivity
// -----------------------------------------------------------------------------
//
TInt CHWRMLightCommonData::DefaultSensitivity() 
    {
    if ( !iRepository )
        {
        TRAP_IGNORE(ResetCenrepL());
        }
        
    return iDefaultSensitivity;
    }

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::SensorEnabled
// -----------------------------------------------------------------------------
//
TBool CHWRMLightCommonData::SensorEnabled() 
    {
    TBool retval(EFalse);
    
    if ( iSensorSupported )
        {
        if ( !iRepository )
            {
            TRAP_IGNORE(ResetCenrepL());
            }
        
        retval = iSensorOn;
        }
        
    return retval;
    }
    
// -----------------------------------------------------------------------------
// CHWRMLightCommonData::DefaultState
// -----------------------------------------------------------------------------
//
CHWRMLight::TLightStatus CHWRMLightCommonData::DefaultState() const
    {   
    CHWRMLight::TLightStatus retval(CHWRMLight::ELightOn);
    
    if ( (User::InactivityTime()).Int() >= iInactivityLimit )
        {
        retval = CHWRMLight::ELightOff;
        }
        
    COMPONENT_TRACE4(_L("HWRM Server - CHWRMLightCommonData::DefaultState - Inactivitity time: %d, Limit: %d, State: %d"), (User::InactivityTime()).Int(), iInactivityLimit, retval);                 
        
    return retval;
    }
    
    
// -----------------------------------------------------------------------------
// CHWRMLightCommonData::HandleNotifyInt
// -----------------------------------------------------------------------------
//
void CHWRMLightCommonData::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {
    COMPONENT_TRACE3(_L("HWRM Server - CHWRMLightCommonData::HandleNotifyInt(0x%x, 0x%x)"), aId, aNewValue) ; 

    switch(aId)
        {
        case KLightIntensity:
            {
            SetDefaultIntensity(aNewValue);
            break;
            }
        case KLightSensorSensitivity:
            {
            SetDefaultSensitivity(aNewValue);
            break;
            }
        case KLightSensorInUse:
            {
            iSensorOn = (aNewValue != 0);
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMLightCommonData::HandleNotifyInt - Sensor set: %d"), iSensorOn); 
            
            // Set lights on unless sensor is not supported or inactivity time passed
            if ( iLightService && 
                 iSensorSupported &&
                 DefaultState() == CHWRMLight::ELightOn )
                {        
                iLightService->SystemLightsOn();
                }
            
            break;
            }
        default:
            {
            // Do nothing, as key does not concern us. 
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMLightCommonData::HandleNotifyInt - Invalid key notified: 0x%x"), aId ); 
            break;
            }
        }

    COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightCommonData::HandleNotifyInt - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::HandleNotifyError
// -----------------------------------------------------------------------------
//
#if  defined(COMPONENT_TRACE_FLAG) && defined(_DEBUG)
void CHWRMLightCommonData::HandleNotifyError(TUint32 aId, TInt error, CCenRepListener* aHandler)
#else
void CHWRMLightCommonData::HandleNotifyError(TUint32 /*aId*/, TInt /*error*/, CCenRepListener* /*aHandler*/)
#endif
    {
    COMPONENT_TRACE4(_L("HWRM Server - CHWRMLightCommonData::HandleNotifyError(0x%x, 0x%x, 0x%x)"), aId, error, aHandler); 
    
    // Try to restart cenrep related functionality. If it fails, clean up and try to restart when values are asked next.
    TRAPD(err, ResetCenrepL());
    
    if (err != KErrNone)
        {
        CleanCenrep();
        COMPONENT_TRACE2(_L("HWRM Server - CHWRMLightCommonData::HandleNotifyError - Cenrep reset failed: %d"), err );
        }

    COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightCommonData::HandleNotifyError - return") ); 
    }

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::SetDefaultIntensity
// -----------------------------------------------------------------------------
//
void CHWRMLightCommonData::SetDefaultIntensity(TInt aValue, TBool aConstructCall)
    {
    iDefaultIntensity = aValue;
    
    if ( iDefaultIntensity < KHWRMLightMinIntensity )
        {
        iDefaultIntensity = KHWRMLightMinIntensity;
        }
    else if ( iDefaultIntensity > KHWRMLightMaxIntensity)
        {
        iDefaultIntensity = KHWRMLightMaxIntensity;
        }
    else
        {
        // no else
        }
        
    // Update value to plugin, if iLightService is initialized
    if ( iLightService )
        {        
        TInt target(iSupportedTargets);
        
        if(aConstructCall)
         {
             target = iFullSystemTarget; //During bootup, intensity of system targets supported in the platform are changed.
         }
                
         TRAPD( err, iLightService->UpdateDefaultIntensityL(target, iDefaultIntensity) );
        
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::SetDefaultIntensity - Error updating plugin: %d" ), err );
            }
        else
            {
            // Intensity change successful. 
            // Set lights on unless sensor control is enabled or inactivity time passed or this is called from constructor
            if ( !aConstructCall &&
                 !(iSensorSupported && iSensorOn) && 
                 DefaultState() == CHWRMLight::ELightOn )
                {
                iLightService->SystemLightsOn();
                }
            }
        }
        
        
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::SetDefaultIntensity: %d" ), iDefaultIntensity );
    }
        
// -----------------------------------------------------------------------------
// CHWRMLightCommonData::SetDefaultSensitivity
// -----------------------------------------------------------------------------
//
void CHWRMLightCommonData::SetDefaultSensitivity(TInt aValue, TBool aConstructCall)
    {
    iDefaultSensitivity = aValue;
    
    if ( iDefaultSensitivity < KHWRMLightMinSensitivity )
        {
        iDefaultSensitivity = KHWRMLightMinSensitivity;
        }
    else if ( iDefaultSensitivity > KHWRMLightMaxSensitivity)
        {
        iDefaultSensitivity = KHWRMLightMaxSensitivity;
        }
    else
        {
        // no else
        }
        
    // Update value to plugin, if iLightService is initialized
    if ( iLightService && iSensorSupported)
        {        
        TRAPD( err, iLightService->UpdateDefaultSensitivityL(iDefaultSensitivity) );
        
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::SetDefaultSensitivity - Error updating plugin: %d" ), err );
            }
        else
            {
            // Sensitivity change successful.
            // Set lights on unless sensor control is not enabled or inactivity time passed or this is called from constructor
            if ( !aConstructCall &&
                 iSensorOn && 
                 DefaultState() == CHWRMLight::ELightOn )
                {
                iLightService->SystemLightsOn();
                }
            }
        }
        
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::SetDefaultSensitivity: %d" ), iDefaultSensitivity );
    }
    

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::LightsTimeoutChange
// -----------------------------------------------------------------------------
//
void CHWRMLightCommonData::LightsTimeoutChange(TInt aTimeout)
    {
    if ( aTimeout < KHWRMLightMinInactivity )
        {
        iInactivityLimit = KHWRMLightMinInactivity;
        }
    else if ( aTimeout > KHWRMLightMaxInactivity )
        {
        iInactivityLimit = KHWRMLightMaxInactivity;
        }
    else
        {
        iInactivityLimit = aTimeout;
        }

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::LightsTimeoutChange: %d" ), iInactivityLimit );
    }

// -----------------------------------------------------------------------------
// CHWRMLightCommonData::CleanCenrep
// Cleans the central repository related functionality. 
// -----------------------------------------------------------------------------
//
void CHWRMLightCommonData::CleanCenrep()
    {
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightCommonData::CleanCenrep()") ); 

    if ( iIntensityNotifyHandler )
        {
        iIntensityNotifyHandler->StopListening();        
        }
    if ( iSensitivityNotifyHandler )
        {
        iSensitivityNotifyHandler->StopListening();        
        }
    if ( iSensorNotifyHandler )
        {
        iSensorNotifyHandler->StopListening();        
        }
    
    delete iIntensityNotifyHandler;
    delete iSensitivityNotifyHandler;
    delete iSensorNotifyHandler;
    delete iRepository;   
    delete iLightsTimeoutObserver; 
    
    iIntensityNotifyHandler = NULL;
    iSensitivityNotifyHandler = NULL;
    iSensorNotifyHandler = NULL;
    iRepository = NULL;        
    iLightsTimeoutObserver = NULL;

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightCommonData::CleanCenrep - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMLightCommonData::ResetCenrepL
// (Re)starts the central repository related functionality. 
// -----------------------------------------------------------------------------
//
void CHWRMLightCommonData::ResetCenrepL()
    {
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightCommonData::ResetCenrep()") ); 

    CleanCenrep();

    // Get read only values from cenrep
    iRepository = CRepository::NewL(KCRUidLightSettings);

    TInt value(0);
    User::LeaveIfError(iRepository->Get(KLightIntensity, value));
    SetDefaultIntensity(value);
    
    User::LeaveIfError(iRepository->Get(KLightTargetMask, iSupportedTargets));        
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::ConstructL - iSupportedTargets: 0x%x" ), iSupportedTargets );
    
    // All target combinations up to all targets defined are legal, so snip the rest off
    iSupportedTargets &= KHWRMAllTargets;       
    
    if ( iSensorSupported )
        {        
        User::LeaveIfError(iRepository->Get(KLightSensorSensitivity, value));
        SetDefaultSensitivity(value);

        User::LeaveIfError(iRepository->Get(KLightSensorInUse, value));
        iSensorOn = (value != 0);
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::ConstructL - iSensorOn: %d" ), iSensorOn );
        
        TInt err = iRepository->Get(KLightSensorMask, iSensorTargets);
        
        if ( err == KErrNone )
            {
            // Sensor targets can only contain supported targets.
            iSensorTargets &= iSupportedTargets; 
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::ConstructL - iSensorTargets: 0x%x" ), iSensorTargets );
            }
        else
            {
            iSensorTargets = iSupportedTargets;
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightCommonData::ConstructL - Error getting sensor targets: %d, assuming all targets are sensor targets" ), err );
            }
        }
        
    // Set up cenrep notify handlers for each key 
    // (separate handlers to ensure that notifications correctly also in repository resets)
    iIntensityNotifyHandler = CCenRepListener::NewL(*this, 
                                                *iRepository, 
                                                KLightIntensity);
    iIntensityNotifyHandler->StartListeningL();

    if ( iSensorSupported )
        {        
        iSensitivityNotifyHandler = CCenRepListener::NewL(*this, 
                                                    *iRepository, 
                                                    KLightSensorSensitivity);
        iSensorNotifyHandler = CCenRepListener::NewL(*this, 
                                                    *iRepository, 
                                                    KLightSensorInUse);
        iSensitivityNotifyHandler->StartListeningL();
        iSensorNotifyHandler->StartListeningL();
        }


    TRAPD( err, iLightsTimeoutObserver = CHWRMLightsTimeoutObserver::NewL(this));
    
    if ( err != KErrNone )
        {
        // Use default value if timeout observer cannot be created
        iInactivityLimit = KHWRMLightDefaultInactivity;
        COMPONENT_TRACE2(_L("HWRM Server - CHWRMLightCommonData::ResetCenrep - Using default lights timeout value: %d "),iInactivityLimit ); 
        }
    
    COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightCommonData::ResetCenrep - return") ); 
    }
    
// -----------------------------------------------------------------------------
// CHWRMLightCommonData::ModifyTargetL
// Passes the call to target modifier plugin
// -----------------------------------------------------------------------------
//
TInt CHWRMLightCommonData::ModifyTargetL(TInt aTarget)
    {
    // If target modifier plugin doesn't exist, just return the target
    if ( iTargetModifier )
        {
        return iTargetModifier->ModifyTargetL( aTarget);
        }
    else
        {
        COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightCommonData::ModifyTargetL - Target modifier plugin NULL, no modification") ); 
        return aTarget;
        }
    }
    
// -----------------------------------------------------------------------------
// CHWRMLightCommonData::FindByTarget
// Finds item from iStatusList by target.
// -----------------------------------------------------------------------------
//
TInt CHWRMLightCommonData::FindByTarget( const TInt* aTarget, 
    const THWRMStatusInfo& aItem )
    {
    if ( *aTarget < aItem.iTarget )
        {
        return -1;
        }
    else if ( *aTarget > aItem.iTarget )
        {
        return 1;
        }

    return 0;
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
