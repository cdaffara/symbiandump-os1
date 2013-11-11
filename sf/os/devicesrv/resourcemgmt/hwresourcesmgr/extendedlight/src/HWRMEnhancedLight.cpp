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
#include "HWRMEnhancedLight.h"
#include "HWRMExtendedLightImpl.h"
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

CHWRMEnhancedLight::CHWRMEnhancedLight():
    iExtendedLightImpl(NULL)
    {
    
    }

CHWRMEnhancedLight::~CHWRMEnhancedLight()
    {
    COMPONENT_TRACE1(_L( "HWRM CHWRMEnhancedLight - Destructor" ) );
    
    delete iExtendedLightImpl;

    COMPONENT_TRACE1(_L( "HWRM CHWRMEnhancedLight - Destructor return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMEnhancedLight::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMEnhancedLight* CHWRMEnhancedLight::NewL()
    {
    CHWRMEnhancedLight* impl = NewLC();
    
    CleanupStack::Pop();

    return impl;
    }
   
// -----------------------------------------------------------------------------
// CHWRMEnhancedLight::NewLC
// Two-phased constructor. Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMEnhancedLight* CHWRMEnhancedLight::NewLC()
    {
    COMPONENT_TRACE1(_L( "HWRM CHWRMEnhancedLight - CHWRMEnhancedLight::NewLC()" ) );

    CHWRMEnhancedLight* enhancedLight = new( ELeave ) CHWRMEnhancedLight();
    
    CleanupStack::PushL( enhancedLight );
    
    enhancedLight->ConstructL(NULL);

    COMPONENT_TRACE2(_L( "HWRM CHWRMEnhancedLight - CHWRMEnhancedLight::NewLC - return 0x%x" ), enhancedLight );

    return enhancedLight;
    }

// -----------------------------------------------------------------------------
// CHWRMEnhancedLight::NewL
// Two-phased constructor for creating a Enhanced Light client with callbacks.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMEnhancedLight* CHWRMEnhancedLight::NewL(MHWRMExtendedLightObserver* aCallback)
    {
    CHWRMEnhancedLight* impl = NewLC(aCallback);
    
    CleanupStack::Pop();

    return impl;
    }
   
// -----------------------------------------------------------------------------
// CHWRMEnhancedLight::NewLC
// Two-phased constructor for creating a Enhanced Light client with callbacks.
// Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMEnhancedLight* CHWRMEnhancedLight::NewLC(MHWRMExtendedLightObserver* aCallback)
    {
    COMPONENT_TRACE2(_L( "HWRM CHWRMEnhancedLight - CHWRMEnhancedLight::NewLC(0x%x)" ), aCallback );

    // Check that reference is not NULL
    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }

    CHWRMEnhancedLight* enhancedLight = new( ELeave ) CHWRMEnhancedLight();
    
    CleanupStack::PushL( enhancedLight );
    
    enhancedLight->ConstructL(aCallback);

    COMPONENT_TRACE2(_L( "HWRM CHWRMEnhancedLight - CHWRMEnhancedLight::NewLC - return 0x%x" ), enhancedLight );

    return enhancedLight;
    }

// -------------------------------------------------
// CHWRMEnhancedLight::NewL
// Two-phased constructor for implementation class. 
// On opening passes permitted targets.
// -------------------------------------------------
//
EXPORT_C CHWRMEnhancedLight* CHWRMEnhancedLight::NewL(TInt aPermittedTargets)
	{		
	CHWRMEnhancedLight* enhancedLight = new(ELeave)CHWRMEnhancedLight;
	CleanupStack::PushL(enhancedLight);
	enhancedLight->ConstructL(NULL,aPermittedTargets);
	CleanupStack::Pop(enhancedLight);
	return enhancedLight;
	}

// -------------------------------------------------
// CHWRMEnhancedLight::NewL
// Two-phased constructor for implementation class. 
// Use this method for creating a Light client with callbacks.
// On opening passes permitted targets.
// -------------------------------------------------
//
EXPORT_C CHWRMEnhancedLight* CHWRMEnhancedLight::NewL(MHWRMExtendedLightObserver* aCallback, TInt aPermittedTargets)
	{	
    // Check that reference is not NULL
    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }
	
	CHWRMEnhancedLight* enhancedLight = new(ELeave)CHWRMEnhancedLight;
	CleanupStack::PushL(enhancedLight);
	enhancedLight->ConstructL(aCallback,aPermittedTargets);
	CleanupStack::Pop(enhancedLight);
	return enhancedLight;
	}

// -----------------------------------------------------------------------------
// CHWRMEnhancedLight::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMEnhancedLight::ConstructL(MHWRMExtendedLightObserver* aCallback, TInt aPermittedTargets)
    {
    COMPONENT_TRACE1(_L( "HWRM CHWRMEnhancedLight - CHWRMEnhancedLight::ConstructL()" ) );

    iExtendedLightImpl = new(ELeave) CHWRMExtendedLightImpl;

    iExtendedLightImpl->ConstructL(aCallback, aPermittedTargets);
    
    COMPONENT_TRACE1(_L( "HWRM CHWRMEnhancedLight - CHWRMEnhancedLight::ConstructL - return" ) );    
    }

//
// API functions forward calls to Extended Light Client
//
void CHWRMEnhancedLight::ReserveLightL(TInt aTarget)
	{
	iExtendedLightImpl->ReserveLightL(aTarget);
	}

void CHWRMEnhancedLight::ReserveLightL(TInt aTarget, TBool aRestoreState, TBool aForceNoCCoeEnv)
	{
	iExtendedLightImpl->ReserveLightL(aTarget,aRestoreState,aForceNoCCoeEnv);
	}
	
void CHWRMEnhancedLight::ReleaseLight(TInt aTarget)
	{
	iExtendedLightImpl->ReleaseLight(aTarget);
	}
	
void CHWRMEnhancedLight::LightOnL(TInt aTarget)
	{
	iExtendedLightImpl->LightOnL(aTarget);
	}
	
void CHWRMEnhancedLight::LightOnL(TInt aTarget, TInt aDuration)
	{
	iExtendedLightImpl->LightOnL(aTarget,aDuration);
	}
	
void CHWRMEnhancedLight::LightOnL(TInt aTarget, TInt aDuration, TInt aIntensity, TBool aFadeIn)
	{
	iExtendedLightImpl->LightOnL(aTarget,aDuration,aIntensity,aFadeIn);
	}
		                      
void CHWRMEnhancedLight::LightBlinkL(TInt aTarget)
	{
	iExtendedLightImpl->LightBlinkL(aTarget);
	}
	
void CHWRMEnhancedLight::LightBlinkL(TInt aTarget, TInt aDuration)
	{
	iExtendedLightImpl->LightBlinkL(aTarget,aDuration);
	}
		                         		                      
void CHWRMEnhancedLight::LightBlinkL(TInt aTarget, TInt aDuration, TInt aOnDuration, TInt aOffDuration, TInt aIntensity)
	{
	iExtendedLightImpl->LightBlinkL(aTarget,aDuration,aOnDuration,aOffDuration,aIntensity);
	}
	
void CHWRMEnhancedLight::LightOffL(TInt aTarget)
	{
	iExtendedLightImpl->LightOffL(aTarget);
	}
	
void CHWRMEnhancedLight::LightOffL(TInt aTarget, TInt aDuration)
	{
	iExtendedLightImpl->LightOffL(aTarget,aDuration);
	}
		                       
void CHWRMEnhancedLight::LightOffL(TInt aTarget, TInt aDuration, TBool aFadeOut)
	{
	iExtendedLightImpl->LightOffL(aTarget,aDuration,aFadeOut);
	}
	
CHWRMExtendedLight::TLightStatus CHWRMEnhancedLight::LightStatus(TInt aTarget) const
	{
	return iExtendedLightImpl->LightStatus(aTarget);
	}
	
TInt CHWRMEnhancedLight::SupportedTargets() const
	{
	return iExtendedLightImpl->SupportedTargets();
	}

EXPORT_C void CHWRMEnhancedLight::SetLightColorL(TInt aTarget, THWRMLightColor& aRGB)
    {
    iExtendedLightImpl->DoSetLightColorL(ETrue, aTarget, aRGB.iRed, aRGB.iGreen, aRGB.iBlue);
    }

EXPORT_C void CHWRMEnhancedLight::SetLightDefaultColorL(TInt aTarget)
    {
    iExtendedLightImpl->DoSetLightColorL(EFalse, aTarget);
    }

EXPORT_C void CHWRMEnhancedLight::LightOnL(TInt aTarget, TInt aDuration, const THWRMLightColor& aRGBParam)
    {
    this->LightOnL(aTarget, aDuration, KHWRMDefaultIntensity, ETrue, aRGBParam);
    }

EXPORT_C void CHWRMEnhancedLight::LightOnL(TInt aTarget, TInt aDuration, TInt aIntensity, TBool aFadeIn, 
                     const  THWRMLightColor& aRGBParam)
    {
    iExtendedLightImpl->DoLightOnWithColorL(aTarget, aDuration, aIntensity, aFadeIn, aRGBParam);
    }

EXPORT_C void CHWRMEnhancedLight::LightBlinkL(TInt aTarget, TInt aDuration, const THWRMLightColor& aRGBParam)
    {
    this->LightBlinkL(aTarget, aDuration, KHWRMDefaultCycleTime, 
                      KHWRMDefaultCycleTime, KHWRMDefaultIntensity, aRGBParam);
    }

EXPORT_C void CHWRMEnhancedLight::LightBlinkL(TInt aTarget, TInt aDuration, TInt aOnDuration, TInt aOffDuration, 
                         TInt aIntensity, const THWRMLightColor& aRGBParam)
    {
    iExtendedLightImpl->DoLightBlinkWithColorL(aTarget, aDuration, aOnDuration, aOffDuration, aIntensity, aRGBParam);
    }




//  End of File  
