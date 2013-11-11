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
#include <e32svr.h>
#include <hwrm/hwrmenhancedlight.h>
#include "HWRMLight.h"
#include "HWRMtrace.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMLight::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMLight* CHWRMLight::NewL()
    {
    CHWRMLight* self = NewLC();
    
    CleanupStack::Pop(self);

    return self;
    }
   
// -----------------------------------------------------------------------------
// CHWRMLight::NewLC
// Two-phased constructor. Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMLight* CHWRMLight::NewLC()
    {
    COMPONENT_TRACE1(_L( "HWRM LightClient - CHWRMLight::NewLC()" ) );

    CHWRMLight* self = new(ELeave) CHWRMLight;
    CleanupStack::PushL(self);
    self->ConstructL(NULL);
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMLight::NewL
// Two-phased constructor for creating a Light client with callbacks.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMLight* CHWRMLight::NewL(MHWRMLightObserver* aCallback)
    {
    CHWRMLight* self = NewLC(aCallback);
    
    CleanupStack::Pop(self);

    return self;
    }
   
// -----------------------------------------------------------------------------
// CHWRMLight::NewLC
// Two-phased constructor for creating a Light client with callbacks.
// Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMLight* CHWRMLight::NewLC(MHWRMLightObserver* aCallback)
    {
    COMPONENT_TRACE2(_L( "HWRM LightClient - CHWRMLight::NewLC(0x%x)" ), aCallback );

    // Check that reference is not NULL
    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }

    CHWRMLight* self = new(ELeave) CHWRMLight;
    CleanupStack::PushL(self);
    self->ConstructL(aCallback);
   
    return self;
    }

CHWRMLight::CHWRMLight()
	{
	}

void CHWRMLight::ConstructL(MHWRMLightObserver* aCallback)
	{
	// publishedAll Light API is restricted to the following light targets:
	TInt permittedTargets( CHWRMLight::EPrimaryDisplay | CHWRMLight::EPrimaryKeyboard |
                           CHWRMLight::ESecondaryDisplay | CHWRMLight::ESecondaryKeyboard |
                           CHWRMLight::ECustomTarget1 | CHWRMLight::ECustomTarget2 |
                           CHWRMLight::ECustomTarget3 | CHWRMLight::ECustomTarget4);
         
    if (aCallback == NULL)
    	{
    	iEnhanced = CHWRMEnhancedLight::NewL(permittedTargets);	
    	}
    else
    	{		
		iEnhanced = CHWRMEnhancedLight::NewL(reinterpret_cast<MHWRMExtendedLightObserver*>(aCallback),
						permittedTargets);
    	}
	}

EXPORT_C CHWRMLight::~CHWRMLight()
	{
	delete iEnhanced;
	}	

//
// API functions forward calls to Extended Light Client
//
void CHWRMLight::ReserveLightL(TInt aTarget)
	{
	iEnhanced->ReserveLightL(aTarget);
	}

void CHWRMLight::ReserveLightL(TInt aTarget, TBool aRestoreState, TBool aForceNoCCoeEnv)
	{
	iEnhanced->ReserveLightL(aTarget,aRestoreState,aForceNoCCoeEnv);
	}
	
void CHWRMLight::ReleaseLight(TInt aTarget)
	{
	iEnhanced->ReleaseLight(aTarget);
	}
	
void CHWRMLight::LightOnL(TInt aTarget)
	{
	iEnhanced->LightOnL(aTarget);
	}
	
void CHWRMLight::LightOnL(TInt aTarget, TInt aDuration)
	{
	iEnhanced->LightOnL(aTarget,aDuration);
	}
	
void CHWRMLight::LightOnL(TInt aTarget, TInt aDuration, TInt aIntensity, TBool aFadeIn)
	{
	iEnhanced->LightOnL(aTarget,aDuration,aIntensity,aFadeIn);
	}
		                      
void CHWRMLight::LightBlinkL(TInt aTarget)
	{
	iEnhanced->LightBlinkL(aTarget);
	}
	
void CHWRMLight::LightBlinkL(TInt aTarget, TInt aDuration)
	{
	iEnhanced->LightBlinkL(aTarget,aDuration);
	}
		                         		                      
void CHWRMLight::LightBlinkL(TInt aTarget, TInt aDuration, TInt aOnDuration, TInt aOffDuration, TInt aIntensity)
	{
	iEnhanced->LightBlinkL(aTarget,aDuration,aOnDuration,aOffDuration,aIntensity);
	}
	
void CHWRMLight::LightOffL(TInt aTarget)
	{
	iEnhanced->LightOffL(aTarget);
	}
	
void CHWRMLight::LightOffL(TInt aTarget, TInt aDuration)
	{
	iEnhanced->LightOffL(aTarget,aDuration);
	}
		                       
void CHWRMLight::LightOffL(TInt aTarget, TInt aDuration, TBool aFadeOut)
	{
	iEnhanced->LightOffL(aTarget,aDuration,aFadeOut);
	}
	
CHWRMLight::TLightStatus CHWRMLight::LightStatus(TInt aTarget) const
	{
	return static_cast<CHWRMLight::TLightStatus> (iEnhanced->LightStatus(aTarget));
	}
	
TInt CHWRMLight::SupportedTargets() const
	{
	return iEnhanced->SupportedTargets();
	}

EXPORT_C void CHWRMLight::SetLightColorL(TInt aTarget, THWRMLightColor& aRGB)
    {
    iEnhanced->SetLightColorL(aTarget, aRGB);
    }

EXPORT_C void CHWRMLight::SetLightDefaultColorL(TInt aTarget)
    {
    iEnhanced->SetLightDefaultColorL(aTarget);
    }

EXPORT_C void CHWRMLight::LightOnL(TInt aTarget, TInt aDuration, const THWRMLightColor& aRGBParam)
    {
    iEnhanced->LightOnL(aTarget, aDuration, aRGBParam);
    }

EXPORT_C void CHWRMLight::LightOnL(TInt aTarget, TInt aDuration, TInt aIntensity, TBool aFadeIn, 
                     const  THWRMLightColor& aRGBParam)
    {
    iEnhanced->LightOnL(aTarget, aDuration, aIntensity, aFadeIn, aRGBParam);
    }

EXPORT_C void CHWRMLight::LightBlinkL(TInt aTarget, TInt aDuration, const THWRMLightColor& aRGBParam)
    {
    iEnhanced->LightBlinkL(aTarget, aDuration, aRGBParam);
    }

EXPORT_C void CHWRMLight::LightBlinkL(TInt aTarget, TInt aDuration, TInt aOnDuration, TInt aOffDuration, 
                         TInt aIntensity, const THWRMLightColor& aRGBParam)
    {
    iEnhanced->LightBlinkL(aTarget, aDuration, aOnDuration, aOffDuration, aIntensity, aRGBParam);
    }



//  End of File  
