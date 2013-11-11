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



#include <e32property.h>
#include <featurecontrol.h> 
#include <featureuids.h>
#include <hwrmlight.h>
#include <hwrmdomainpskeys.h>
#include "hwrmdefaultlighttargetmodifierplugin.h"
#include "HWRMtrace.h"

// CONSTANTS

// Value for all defined non-custom targets. Used to determine system target.
const TInt KHWRMNonCustomTargets(CHWRMLight::EPrimaryDisplay |
                                 CHWRMLight::EPrimaryKeyboard |
                                 CHWRMLight::ESecondaryDisplay |
                                 CHWRMLight::ESecondaryKeyboard );  


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
	iCoverDisplaySupported = IsFeatureSupported( NFeature::KCoverDisplay );
	iGripSupported = ! IsFeatureSupported( NFeature::KKeypadNoSlider );
    }

// -----------------------------------------------------------------------------
// CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL
//
// Default implementation simply returns the received target without modifying it, 
// unless the flip P&S key has been set to EPSHWRMFlipOpen, in which case it removes
// the primary display from aTarget.
// This is merely a trivial example of how the physical status of the device can be used to 
// modify the targets. This function will need to be adapted to a product's physical 
// configuration by filtering out the inactive targets according to its status at the moment 
// (e.g., "flip" closed or open, "slider" closed or open, etc.)
//
// Default implementation will not leave.
// -----------------------------------------------------------------------------
// 
TInt CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL( TInt aTarget )
    {
    COMPONENT_TRACE2(_L("CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL(aTarget: 0x%x)"), aTarget );

    TInt modifiedTarget(aTarget);
    
    User::LeaveIfError(KErrNone); // avoid LeaveScan error in dummy default version

	TInt flipStatus(EPSHWRMFlipStatusUninitialized);
   	RProperty::Get(KPSUidHWRM, KHWRMFlipStatus, flipStatus);
   	if (flipStatus == EPSHWRMFlipOpen)
   		{
   		modifiedTarget &= ~CHWRMLight::EPrimaryDisplay;
   		}
   
    COMPONENT_TRACE3(_L("CHWRMDefaultLightTargetModifierPlugin::ModifyTargetL - Return: 0x%x (Original target: 0x%x)"), modifiedTarget, aTarget);    

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
    
TBool CHWRMDefaultLightTargetModifierPlugin::IsFeatureSupported(const TUid aFeatureUid)
    {
    RFeatureControl featureControl;
    
    TInt err = featureControl.Open();

    TBool featureSupported(EFalse);

    if(err == KErrNone)
        {
        featureSupported = featureControl.FeatureSupported(aFeatureUid);
        }
    
    featureControl.Close();

    return featureSupported;
    }

