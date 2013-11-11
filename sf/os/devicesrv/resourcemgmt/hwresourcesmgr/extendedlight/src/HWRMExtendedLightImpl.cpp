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
#include <centralrepository.h>
#include "HWRMExtendedLightImpl.h"
#include "HWRMLightClientServer.h"
#include "HWRMtrace.h"
#include "HWRMExtendedLightStatusObserver.h"
#include "HWRMPrivatePSKeys.h"
#include "hwrmuipluginhandler.h"


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
// CHWRMExtendedLightImpl::CHWRMExtendedLightImpl
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMExtendedLightImpl::CHWRMExtendedLightImpl()
    {
    COMPONENT_TRACE1(_L( "HWRM LightClient - CHWRMExtendedLightImpl::CHWRMExtendedLightImpl()" ) );
    COMPONENT_TRACE1(_L( "HWRM LightClient - CHWRMExtendedLightImpl::CHWRMExtendedLightImpl - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::ConstructL(MHWRMExtendedLightObserver* aCallback, TInt aPermittedTargets)
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ConstructL()" ) );
    
    // NULL is acceptable value for aCallback, just means that 
    // client is not interested in callbacks.
    iCallback = aCallback;

    // Connect the session and order notifications
    OpenL(aPermittedTargets);    
  
  	TInt err(KErrNone);    
   	TRAP(err, iUiPluginHandler = CHwrmUiPluginHandler::NewL( ));
   	if ( err == KErrNone )
    	{
    	COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ConstructL() - successfully instantiated UI plugin handler" ) );
    	iUiPluginHandler->AddForegroundObserverL(this);    	
    	}
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ConstructL - return" ) );    
    }


// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMExtendedLightImpl::~CHWRMExtendedLightImpl()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::~CHWRMExtendedLightImpl()" ) );        

    // close HWRM client, also deletes status observers
    Close();

    // iCallback is not owned so it is not cleaned, just set to NULL 
    iCallback = NULL;
    
    // status observer is deleted in close. Set to NULL to avoid pc-lint warning
    iStatusObserver = NULL; 
                    
    delete iUiPluginHandler; 
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::~CHWRMExtendedLightImpl - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::OpenL
// Opens session and orders notifications
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::OpenL(TInt aSupportedTargets)
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::Open()" ));
    
    // Returns either error or target mask
    User::LeaveIfError(iClient.Connect(HWRMResourceTypeLight));

    TPtr8 ptr( reinterpret_cast<TUint8*>(&iSupportedTargets), sizeof(iSupportedTargets), sizeof(iSupportedTargets) );
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMGetSupportedTargets, TIpcArgs(&ptr)));
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::Open - Supported targets: 0x%x" ), iSupportedTargets );

	// When the extended light client has been created from CHWRMLight, only the targets
	// available to SDK users are allowed. The rest of the targets must be filtered out.
	if (aSupportedTargets != -1) // default value (-1): all targets permitted
		{
		iSupportedTargets &= aSupportedTargets;
		}
 
    // Order status notifications
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::Open - Ordering notifications" ) );
    iStatusObserver = CHWRMExtendedLightStatusObserver::NewL(iCallback, iSupportedTargets);
                
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::Open - return" ) );
    
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::Close
// Closes session.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::Close()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::Close()" ));
        
    // Inform session that we are cleaning up now.
    // Ignore errors, just trace them
    if (iClient.Handle())
        {
        iClient.ExecuteOperation(EHWRMCleanupLights, TIpcArgs());
        }
    
    // Abort status notifications
    if ( iStatusObserver )
        {
        COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::Close - canceling Light Status notifications" ) );
        iStatusObserver->Cancel();
        delete iStatusObserver;
        iStatusObserver = NULL;
        }	                
    iClient.Close();
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::Close - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::ReserveLightL
// Reserve light targets
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::ReserveLightL(TInt aTarget)
    {
    ReserveLightL( aTarget, EFalse, EFalse );
    }
    
// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::ReserveLightL
// Reserve light targets
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::ReserveLightL(TInt aTarget, 
                                           TBool aRestoreState, 
                                           TBool aForceNoCCoeEnv)
    {
    COMPONENT_TRACE4(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ReserveLightL(0x%x, 0x%x, 0x%x)" ), aTarget, aRestoreState, aForceNoCCoeEnv);
    
    CheckTargetParameterL( aTarget );
    
    if ( !aForceNoCCoeEnv )
        {        
 		if(iUiPluginHandler)
    		{
    		// Check that we are on foreground (i.e. we have keyboard focus.)
            // Note: There can be windows on top of our application that have explicitly disabled keyboard focus,
            // but then again the foreground/background observer will not kick in in those cases either.
            // -> We can use focus to determine foreground status here.            
    		if(iUiPluginHandler->InForeground() == EFalse)
    			{
    			COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ReserveLightL - Application does not have focus, Reserve denied." ) );
                User::Leave(KErrNotReady);                
    			}
    		} 		        	
        else
        	{
            COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ReserveLightL - No  iUiPluginHandler available" ) );
            User::Leave(KErrBadHandle);        	
        	}
    
        }
    // Do the actual reservation
    DoReserveLightL(aTarget, aRestoreState, aForceNoCCoeEnv);
    
    if ( !aForceNoCCoeEnv )
        {
        // update autoreserve mask
        iAutoReserving |= aTarget;
        COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ReserveLightL - Updated autoreserve mask: 0x%x" ), iAutoReserving );
        }

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ReserveLightL - return" ) );
    
    }
    
// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::ReleaseLight
// Release light targets
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::ReleaseLight(TInt aTarget)
    {
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ReleaseLight(0x%x)" ), aTarget );
    
    DoReleaseLight(aTarget);

    iAutoReserving &= ~aTarget;
    
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ReleaseLight - Updated autoreserve mask: 0x%x" ), iAutoReserving );
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::ReleaseLight - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::DoReserveLightL
// Reserve vibra resource
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::DoReserveLightL(TInt aTarget, TBool aRestoreState, TBool aForceNoCCoeEnv)
    {
    COMPONENT_TRACE4(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoReserveLightL(0x%x, 0x%x, 0x%x)" ), aTarget, aRestoreState, aForceNoCCoeEnv);
    
    // Check if these targets are already reserved by this session. 
    // In that case do nothing.
    if ( (iReserved & aTarget) != aTarget  )
        {
        // Execute reservation
        User::LeaveIfError(iClient.ExecuteOperation(EHWRMReserveLights, TIpcArgs(aTarget, aRestoreState, aForceNoCCoeEnv)));

        // Reserve successful, mark as reserved.
        iReserved |= aTarget;
        }
       
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoReserveLightL - return" ) );
    }
    

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::DoReleaseLight
// Release vibra resource
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::DoReleaseLight(TInt aTarget)
    {
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoReleaseLight(0x%x)" ), aTarget );
    
    // Only release if any of specified targets reserved.
    if ( iReserved & aTarget )
        {
        // Execute release
        TInt err = iClient.ExecuteOperation(EHWRMReleaseLights, TIpcArgs(aTarget));
        
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoReleaseLight - Release execution failed: %d" ), err );
            }
        
        iReserved &= ~aTarget;
        }
        
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoReleaseLight - return" ) );
    }

//------------------------------------------------------------------------------
// CHWRMExtendedLightImpl::HandleGainingForeground     
// Inherited From MCoeForegroundObserver - called when app comes to foreground
//------------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::HandleGainingForeground()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::HandleGainingForeground()" ) );

    // Reserve if any target is autoreserving
    if ( iAutoReserving )
        {
        TRAP_IGNORE(DoReserveLightL(iAutoReserving, ETrue, EFalse));
        }            

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::HandleGainingForeground - return " ) );
    }


//------------------------------------------------------------------------------
// CHWRMExtendedLightImpl::HandleLosingForeground     
// Inherited From MCoeForegroundObserver - called when app goes to background 
//------------------------------------------------------------------------------
void CHWRMExtendedLightImpl::HandleLosingForeground()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::HandleLosingForeground()" ) );

    // Release if any target is autoreserving
    if ( iAutoReserving )
        {
        DoReleaseLight(iAutoReserving);
        }            
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::HandleLosingForeground - return " ) );
    }

	
// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightOnL
// Set light on.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightOnL(TInt aTarget)
    {
    LightOnL(aTarget, KHWRMInfiniteDuration, KHWRMDefaultIntensity, ETrue);
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightOnL
// Set light on for a duration.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightOnL(TInt aTarget, 
                                      TInt aDuration)
    {
    LightOnL(aTarget, aDuration, KHWRMDefaultIntensity, ETrue);
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightOnL
// Set light on for a duration with intensity.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightOnL(TInt aTarget, 
                                      TInt aDuration, 
                                      TInt aIntensity,
		                              TBool aFadeIn)
    {
    COMPONENT_TRACE5(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightOnL(0x%x, 0x%x, 0x%x, 0x%x)" ), aTarget, aDuration, aIntensity, aFadeIn);
        
    CheckCommonParametersL(aTarget, aDuration, aIntensity);
    
    // Create package
    THWRMLightsOnData data;
    data.iTarget = aTarget;
    data.iIntensity = aIntensity;
    data.iDuration = aDuration;
    data.iFadeIn = aFadeIn;
    data.iColor =  0; //No color info
    THWRMLightsOnCmdPackage pckg(data);

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMLightsOn, TIpcArgs(&pckg)));
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightOnL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightBlinkL
// Set light Blinking.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightBlinkL(TInt aTarget)
    {
    LightBlinkL(aTarget, KHWRMInfiniteDuration, KHWRMDefaultCycleTime, KHWRMDefaultCycleTime, KHWRMDefaultIntensity );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightBlinkL
// Set light Blinking for a duration.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightBlinkL(TInt aTarget, 
                                         TInt aDuration)
    {
    LightBlinkL(aTarget, aDuration, KHWRMDefaultCycleTime, KHWRMDefaultCycleTime, KHWRMDefaultIntensity );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightBlinkL
// Set light Blinking for a duration with intensity.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightBlinkL(TInt aTarget, 
                                         TInt aDuration, 
                                         TInt aOnDuration, 
                                         TInt aOffDuration, 
                                         TInt aIntensity)
    {
    COMPONENT_TRACE6(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightBlinkL(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), aTarget, aDuration, aOnDuration, aOffDuration, aIntensity);
        
    CheckCommonParametersL(aTarget, aDuration, aIntensity);
    CheckDurationParameterL(aOnDuration);
    CheckDurationParameterL(aOffDuration);
    
    // If one of the cycle durations is default, both must be
    if ( aOnDuration == KHWRMDefaultCycleTime || aOffDuration == KHWRMDefaultCycleTime )
        {
        if ( aOnDuration != aOffDuration )
            {
            COMPONENT_TRACE3(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightBlinkL - Default cycle duration mismatch (%d,%d)" ), aOnDuration, aOffDuration );
            User::Leave(KErrArgument);
            }
        }
    
    // Create package
    THWRMLightsBlinkData blinkData;
    blinkData.iTarget = aTarget;
    blinkData.iIntensity = aIntensity;
    blinkData.iDuration = aDuration;
    blinkData.iOnCycleDuration = aOnDuration;
    blinkData.iOffCycleDuration = aOffDuration;
    
    THWRMLightsBlinkCmdPackage pckg(blinkData);

    // Call server
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMLightsBlink, TIpcArgs(&pckg)));
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightBlinkL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightOffL
// Set light off.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightOffL(TInt aTarget)
    {
    LightOffL(aTarget, KHWRMInfiniteDuration, ETrue);
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightOffL
// Set light off for a duration.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightOffL(TInt aTarget, 
                               TInt aDuration)
    {
    LightOffL(aTarget, aDuration, ETrue);
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightOffL
// Set light off for a duration with fadeout period
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::LightOffL(TInt aTarget, 
                                       TInt aDuration, 
		                               TBool aFadeOut)
    {
    COMPONENT_TRACE4(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightOffL(0x%x, 0x%x, 0x%x)" ), aTarget, aDuration, aFadeOut);
        
    CheckCommonParametersL(aTarget, aDuration, KHWRMDefaultIntensity);
    
    // Create package
    THWRMLightsOffData data;
    data.iTarget = aTarget;
    data.iDuration = aDuration;
    data.iFadeOut = aFadeOut;
    THWRMLightsOffCmdPackage pckg(data);

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMLightsOff, TIpcArgs(&pckg, aDuration)));
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightOffL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightStatus
// -----------------------------------------------------------------------------
//
CHWRMExtendedLight::TLightStatus CHWRMExtendedLightImpl::LightStatus(TInt aTarget) const
    {
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightStatus(0x%x)" ), aTarget );
    
    CHWRMExtendedLight::TLightStatus status = CHWRMExtendedLight::ELightStatusUnknown;

    if ( iStatusObserver )
        {
        status = iStatusObserver->CurrentStatus(aTarget);
        }
               
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightStatus - return 0x%x" ), status );
    
    return status;    
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::SupportedTargets
// -----------------------------------------------------------------------------
//
TInt CHWRMExtendedLightImpl::SupportedTargets() const
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::SupportedTargets()" ) );
    
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::SupportedTargets - return 0x%x" ), iSupportedTargets );
    
    return iSupportedTargets;    
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::CheckCommonParametersL
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::CheckCommonParametersL(TInt aTarget, TInt aDuration, TInt aIntensity) const
    {
    if ( !iClient.Handle() )
        {
        COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::CheckCommonParametersL - Invalid HWRM client." ) );    
        User::Leave(KErrBadHandle);
        }

    CheckTargetParameterL(aTarget);
        
    CheckDurationParameterL(aDuration);    

    if ( aIntensity != KHWRMDefaultIntensity && (aIntensity < KHWRMLightMinIntensity || aIntensity > KHWRMLightMaxIntensity) )
        {
        COMPONENT_TRACE1( ( _L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::CheckCommonParametersL - Invalid Intensity." ) ) );    
        User::Leave(KErrArgument);
        }
    }
        
// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::CheckDurationParameterL
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::CheckDurationParameterL(TInt aDuration) const
    {
    if ( aDuration < 0 || aDuration > KHWRMLightMaxDuration )
        {
        COMPONENT_TRACE1( ( _L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::CheckCommonParametersL - Invalid Duration." ) ) );    
        User::Leave(KErrArgument);
        }
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::CheckTargetParameterL
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::CheckTargetParameterL(TInt aTarget) const
    {
    // All target combinations up to all targets defined are legal.
    // Empty target is not legal.
    // System target with any other target is not legal.
    if ( (~(iSupportedTargets | CHWRMExtendedLight::ESystemTarget) & aTarget) || aTarget & CHWRMExtendedLight::ENoTarget )
        {
        COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::CheckTargetParameterL - Invalid Target: 0x%x." ), aTarget );    
        User::Leave(KErrNotSupported);
        }
    }

void CHWRMExtendedLightImpl::DoSetLightColorL(TBool aSetColor, TInt aTarget, TUint8 aRed, TUint8 aGreen, TUint8 aBlue)
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoSetLightColorL()" ) );
    
    CheckTargetParameterL(aTarget);
    
    THWRMSetColorData data;
    data.iTarget = aTarget;
    data.iColor  = (aSetColor << KHWRMSetDefaultColorShift) | (aRed << KHWRMRedShift) | (aGreen << KHWRMGreenShift) | (aBlue);

    COMPONENT_TRACE3(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoSetLightColorL() col=0x%x target=0x%x" ), data.iColor, data.iTarget);
    THWRMSetColorCmdPackage pckg(data);

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMSetLightColor, TIpcArgs(&pckg)));

    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoSetLightColorL() return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightOnL
// Set light on for a duration with intensity and color.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::DoLightOnWithColorL(TInt aTarget, 
                                      TInt aDuration, 
                                      TInt aIntensity,
		                              TBool aFadeIn,
		                              const THWRMLightColor& aRGBParam)
    {
    COMPONENT_TRACE8(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::DoLightOnWithColorL(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), 
                    aTarget, aDuration, aIntensity, aFadeIn, aRGBParam.iRed, aRGBParam.iGreen, aRGBParam.iBlue);
        
    CheckCommonParametersL(aTarget, aDuration, aIntensity);
    
    // Create package
    THWRMLightsOnData data;
    data.iTarget = aTarget;
    data.iIntensity = aIntensity;
    data.iDuration = aDuration;
    data.iFadeIn = aFadeIn;
    data.iColor  = (1 << KHWRMSetDefaultColorShift) | (aRGBParam.iRed << KHWRMRedShift) | (aRGBParam.iGreen << KHWRMGreenShift) | (aRGBParam.iBlue);
    THWRMLightsOnCmdPackage pckg(data);

    // Call server 
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMLightsOnWithColor, TIpcArgs(&pckg)));
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightOnL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLightImpl::LightBlinkL
// Set light Blinking for a duration with intensity and color.
// -----------------------------------------------------------------------------
//
void CHWRMExtendedLightImpl::DoLightBlinkWithColorL(TInt aTarget, 
                                         TInt aDuration, 
                                         TInt aOnDuration, 
                                         TInt aOffDuration, 
                                         TInt aIntensity,
                                         const THWRMLightColor& aRGBParam)
    {
    COMPONENT_TRACE9(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightBlinkL(0x%x, 0x%x, 0x%x, 0x%x, 0x%x , 0x%x, 0x%x, 0x%x)" ), 
                     aTarget, aDuration, aOnDuration, aOffDuration, aIntensity, aRGBParam.iRed, aRGBParam.iGreen, aRGBParam.iBlue);
        
    CheckCommonParametersL(aTarget, aDuration, aIntensity);
    CheckDurationParameterL(aOnDuration);
    CheckDurationParameterL(aOffDuration);
    
    // If one of the cycle durations is default, both must be
    if ( aOnDuration == KHWRMDefaultCycleTime || aOffDuration == KHWRMDefaultCycleTime )
        {
        if ( aOnDuration != aOffDuration )
            {
            COMPONENT_TRACE3(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightBlinkL - Default cycle duration mismatch (%d,%d)" ), aOnDuration, aOffDuration );
            User::Leave(KErrArgument);
            }
        }
    
    // Create package
    THWRMLightsBlinkData blinkData;
    blinkData.iTarget = aTarget;
    blinkData.iIntensity = aIntensity;
    blinkData.iDuration = aDuration;
    blinkData.iOnCycleDuration = aOnDuration;
    blinkData.iOffCycleDuration = aOffDuration;
    blinkData.iColor  = (1 << KHWRMSetDefaultColorShift) | (aRGBParam.iRed << KHWRMRedShift) | (aRGBParam.iGreen << KHWRMGreenShift) | (aRGBParam.iBlue);
    
    THWRMLightsBlinkCmdPackage pckg(blinkData);

    // Call server
    User::LeaveIfError(iClient.ExecuteOperation(EHWRMLightsBlinkWithColor, TIpcArgs(&pckg)));
    
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLightImpl::LightBlinkL - return" ) );
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================


            
//  End of File  
