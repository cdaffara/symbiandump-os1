/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Hardware Resource Manager Default Light Target Modifier Plugin 
*                implementation file
*
*/


#include <e32property.h>
#include <featmgr.h>
#include <hwrmlight.h>
#include <hwrmdomainpskeys.h>
#include "hwrmdefaultlighttargetmodifierplugin.h"
#include "Trace.h"

// CONSTANTS

// Value for all defined non-custom targets. Used to determine system target.
const TInt KHWRMNonCustomTargets(CHWRMLight::EPrimaryDisplay |
                                 CHWRMLight::EPrimaryKeyboard |
                                 CHWRMLight::ESecondaryDisplay |
                                 CHWRMLight::ESecondaryKeyboard );  

// Values for switch IO group and switch functions. These values are licensee specific.
const TInt KHWSwitchGroup( 29 );
const TInt KHWSwitchGrip( 2 );

// -----------------------------------------------------------------------------
// CHWRMDefaultLightTargetModifierPlugin::NewL
// -----------------------------------------------------------------------------
//
CHWRMDefaultLightTargetModifierPlugin* CHWRMDefaultLightTargetModifierPlugin::NewL()
    {
    CHWRMDefaultLightTargetModifierPlugin* self = new(ELeave) CHWRMDefaultLightTargetModifierPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMDefaultLightTargetModifierPlugin::~CHWRMDefaultLightTargetModifierPlugin
// -----------------------------------------------------------------------------
//
CHWRMDefaultLightTargetModifierPlugin::~CHWRMDefaultLightTargetModifierPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CHWRMDefaultLightTargetModifierPlugin::CHWRMDefaultLightTargetModifierPlugin
// -----------------------------------------------------------------------------
//
CHWRMDefaultLightTargetModifierPlugin::CHWRMDefaultLightTargetModifierPlugin()
    {
    }

// -----------------------------------------------------------------------------
// CHWRMDefaultLightTargetModifierPlugin::ConstructL
//
// Check relevant features initial states (if any) for target modification.
//
// Customer specific lights related initialization needed at startup 
// can be done in ConstructL of customer implementation.
// -----------------------------------------------------------------------------
//
void CHWRMDefaultLightTargetModifierPlugin::ConstructL()
    {
    FeatureManager::InitializeLibL();
    iCoverDisplaySupported = FeatureManager::FeatureSupported( KFeatureIdCoverDisplay );
    iGripSupported = !(FeatureManager::FeatureSupported( KFeatureIdKeypadNoSlider ));
    FeatureManager::UnInitializeLib();  

    TInt err( KErrNone );
    TInt status;
    
    err = RProperty::Get( KPSUidHWRM, KHWRMGripStatus, status );
    if( err == KErrNone && status == EPSHWRMGripStatusUninitialized )
        {
        // Initialize grip status of HWRM Device Switch API
        err = UserSvr::HalFunction( KHWSwitchGroup, KHWSwitchGrip, &status, 0 );
        if( err == KErrNone)
            {
            RProperty::Set( KPSUidHWRM, KHWRMGripStatus, status ? EPSHWRMGripOpen : EPSHWRMGripClosed );
            }
        COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ConstructL() - grip, err: %d, status: %d" ), 
            err, status ) );
        }
    }

// -----------------------------------------------------------------------------
// CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL
//
// Default implementation provides handling for devices that have one of the 
// following supported light target configurations:
// - Primary display + Primary keyboard, no flip
// - Primary and secondary display + Primary (and optionally secondary) keyboard
//   - Requires KFeatureIdCoverDisplay feature to be defined
// - Either of the above with grip that covers primary keyboard
//   - Requires KFeatureIdKeypadNoSlider feature to not be defined
//
// Default implementation will not leave.
// -----------------------------------------------------------------------------
// 
TInt CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL( TInt aTarget )
    {
    COMPONENT_TRACE((_L("CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL(aTarget: 0x%x)"), aTarget ));

    TInt modifiedTarget(aTarget); 

    // Figure out the active targets
    TInt err(KErrNone);
    TInt gripOpen(EPSHWRMGripOpen);
    
    if ( iGripSupported )
        {
        err = RProperty::Get( KPSUidHWRM, KHWRMGripStatus, gripOpen );
        if ( err != KErrNone )
            {
            gripOpen = EPSHWRMGripOpen;
            COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - KHWRMGripStatus reading failed: %d " ), err ) );
            }
        }
    
    if ( iCoverDisplaySupported )
        {
        TInt flipOpen(EPSHWRMFlipOpen);
        err = RProperty::Get(KPSUidHWRM, KHWRMFlipStatus, flipOpen); // returns 0 or 1, no enums defined
        
        if ( err == KErrNone )
            {
            if ( flipOpen == EPSHWRMFlipOpen )
                {
                if ( gripOpen == EPSHWRMGripOpen )
                    {
                    modifiedTarget &= ~CHWRMLight::ESecondaryDisplayAndKeyboard;
                    COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - Flip and grip open, removed secondary display and keyboard" ) ) );
                    }
                else
                    {
                    modifiedTarget &= ~(CHWRMLight::EPrimaryKeyboard | CHWRMLight::ESecondaryDisplayAndKeyboard);
                    COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - Flip open, grip closed, removed primary keyboard and secondary display and keyboard" ) ) );
                    }
                }
            else
                {
                // If cover UI and flip closed: Remove all primary components
                modifiedTarget &= ~CHWRMLight::EPrimaryDisplayAndKeyboard;
                COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - Flip closed, removed primary display and keyboard" ) ) );
                }
            }
        else
            {
            COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - KHWRMFlipStatus reading failed: %d, no further modification to target " ), err ) );
            }
        }
    else
        {
        if ( gripOpen == EPSHWRMGripOpen )
            {
            TInt flipOpen(EPSHWRMFlipOpen);
            err = RProperty::Get(KPSUidHWRM, KHWRMFlipStatus, flipOpen); // returns 0 or 1, no enums defined
            if ( err == KErrNone )
                {		
                if ( flipOpen == EPSHWRMFlipClosed )
                    {
                    // Removing the primary keyboard from the light target of a device with 
                    // the primary keyboard hidden under a slider which can generate flip events. 
                    modifiedTarget &= ~CHWRMLight::EPrimaryKeyboard;
                    COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - Flip closed, removed primary keyboard" ) ) );
                    }
                else
                    {
                    COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - Grip open, no further modification to target" ) ) );	
                    }	    
                }
            else
                {	
                COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - KHWRMFlipStatus reading failed: %d" ), err ) );
                }
            }
        else
            {
            modifiedTarget &= ~CHWRMLight::EPrimaryKeyboard;
            COMPONENT_TRACE( ( _L( "CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - Grip closed, removed primary keyboard" ) ) );
            }
        }

    COMPONENT_TRACE((_L("CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - Return: 0x%x (Original target: 0x%x)"), modifiedTarget, aTarget));    

    return modifiedTarget;
    }

// -----------------------------------------------------------------------------
// CHWRMDefaultLightTargetModifierPlugin::GetFullSystemTarget
//
// Assume all supported non-custom targets are system targets 
// in default implementation.
// In customer implementations KHWRMNonCustomTargets needs to be replaced
// with product specific mask or logic to determine a mask.
// -----------------------------------------------------------------------------
//
TInt CHWRMDefaultLightTargetModifierPlugin::GetFullSystemTarget()
    {
    return KHWRMNonCustomTargets;    
    }
    

