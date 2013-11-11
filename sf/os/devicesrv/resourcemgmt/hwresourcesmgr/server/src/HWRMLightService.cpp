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
#include "HWRMServer.h"
#include "HWRMService.h"
#include "HWRMLightService.h"
#include "HWRMPluginHandler.h"
#include "HWRMReservationHandler.h"
#include "HWRMtrace.h"

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
_LIT( KPanicCategory, "HWRMLightService" );

const TUint KHWRMDefaultColor = 0;
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
// CHWRMLightService::CHWRMLightService
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMLightService::CHWRMLightService(CHWRMLightCommonData& aLightCommonData)
    : iLightCommonData(aLightCommonData)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::CHWRMLightService()" ));
    
    // Null the target timers to ensure proper cleanup if ConstructL leaves
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        iTargets[i].iTimer = NULL;
        }
    
    iSupportedTargets = iLightCommonData.SupportedTargets();
    iFullSystemTarget = iLightCommonData.FullSystemTarget();
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::CHWRMLightService - return" ));
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMLightService::ConstructL(CHWRMPluginHandler* aPluginHandler,
                                   CHWRMReservationHandler* aReservationHandler)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::ConstructL(0x%x, 0x%x)" ), aPluginHandler, aReservationHandler);

    __ASSERT_ALWAYS(aPluginHandler, User::Panic(KPanicCategory, EPanicBadHandle));
    __ASSERT_ALWAYS(aReservationHandler, User::Panic(KPanicCategory, EPanicBadHandle));
    
    BaseConstructL(aPluginHandler, aReservationHandler);
    
    // initialize target timers (and the rest of the data, just in case)    
    TInt currentTarget(KHWRMLightFirstTarget);
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        // only initialize supported target timers
        if ( currentTarget & iSupportedTargets )
            {
            iTargets[i].iTimer = CHWRMGenericTimer::NewL(*this, KHWRMGenericTimerMaxTime, i);	
            }

        iTargets[i].iTarget = currentTarget;
        iTargets[i].iSuspended = EFalse;	
        iTargets[i].iReserved = EFalse;
        
        iTargets[i].iBaseState.iIntensity = KHWRMDefaultIntensity;	
        iTargets[i].iBaseState.iState = CHWRMLight::ELightStatusUnknown;
        iTargets[i].iBaseState.iFade = EFalse;
        iTargets[i].iBaseState.iOffCycleDuration = KHWRMDefaultCycleTime;
        iTargets[i].iBaseState.iOnCycleDuration = KHWRMDefaultCycleTime;
        iTargets[i].iBaseState.iColor = KHWRMInvalidColorMask;
        
        
        iTargets[i].iLastState.iIntensity = KHWRMDefaultIntensity;	
        iTargets[i].iLastState.iState = CHWRMLight::ELightStatusUnknown;
        iTargets[i].iLastState.iFade = EFalse;
        iTargets[i].iLastState.iOffCycleDuration = KHWRMDefaultCycleTime;
        iTargets[i].iLastState.iOnCycleDuration = KHWRMDefaultCycleTime;
        iTargets[i].iLastState.iColor = KHWRMInvalidColorMask;

        iTargets[i].iFrozenState.iIntensity = KHWRMDefaultIntensity;	
        iTargets[i].iFrozenState.iState = CHWRMLight::ELightStatusUnknown;
        iTargets[i].iFrozenState.iFade = EFalse;
        iTargets[i].iFrozenState.iOffCycleDuration = KHWRMDefaultCycleTime;
        iTargets[i].iFrozenState.iOnCycleDuration = KHWRMDefaultCycleTime;
        iTargets[i].iFrozenState.iColor = KHWRMInvalidColorMask;

        // move to next target
        currentTarget <<= 1;
        }

    iDefaultState.iIntensity = KHWRMDefaultIntensity;	
    iDefaultState.iState = CHWRMLight::ELightStatusUnknown; // This is updated based on inactivity time
    iDefaultState.iFade = ETrue;
    iDefaultState.iOffCycleDuration = KHWRMDefaultCycleTime;
    iDefaultState.iOnCycleDuration = KHWRMDefaultCycleTime;
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMLightService* CHWRMLightService::NewL(CHWRMPluginHandler* aPluginHandler,
                                           CHWRMReservationHandler* aReservationHandler,
                                           CHWRMLightCommonData& aLightCommonData)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::NewL(0x%x)" ), aPluginHandler);

    CHWRMLightService* self = new( ELeave ) CHWRMLightService(aLightCommonData);
    
    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler, aReservationHandler);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::NewL - return 0x%x" ), self );

    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMLightService::~CHWRMLightService()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::~CHWRMLightService()" ) );
    
    CleanupLights();

    // Delete timers
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        delete iTargets[i].iTimer;
        }

    // Base class cleanup
    BaseDestruct();
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::~CHWRMLightService - return" ) );
    }



// -----------------------------------------------------------------------------
// CHWRMLightService::CleanupLights()
// Handles Light service cleanup
// -----------------------------------------------------------------------------
//
void CHWRMLightService::CleanupLights()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::CleanupLights()" ) );
    if (!iCleaningUp)
        {
        iCleaningUp = ETrue;
        
        TInt unreserved(0x0);
        TInt mask(0x0); // mask of targets
        
        // If any targets are still reserved, release them
        for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
            {
            if ( iTargets[i].iReserved )
                {
                COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMCleanupLights - Target index %d needs release." ), i );
                mask |= iTargets[i].iTarget;

                // Clear reserved and suspended flags
                iTargets[i].iReserved = EFalse;
                iTargets[i].iSuspended = EFalse;
                }
            }
        
        if ( mask )
            {
            unreserved = iReservationHandler->Release(this, mask);	
            }
        else
            {
            unreserved = iReservationHandler->GetUnreservedTargets();
            }
            
        mask = 0x0; // reset mask
        
        // Restore unreserved targets that have been affected by this session to default state.
        // Also cancel all timers.
        for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
            {
            if ( iTargets[i].iTimer && iTargets[i].iTimer->IsActive() )
                {
                iTargets[i].iTimer->Cancel();
                }
                
            if ( (unreserved & iTargets[i].iTarget) == iTargets[i].iTarget 
                 && iTargets[i].iLastState.iState != CHWRMLight::ELightStatusUnknown )
                {
                mask |= iTargets[i].iTarget;
                }
            }

        // Restore default states if needed. iTargets[0] is used as dummy.
        // The aMessage pointer is required for completion after transaction.
        if ( mask )
            {
            // Cannot restore both system targets and non-system targets with same command, so do them separately.
            TInt subMask(mask & iFullSystemTarget);

            if ( subMask )
                {
                RestoreState(iTargets[0], EStateChangeDefaultState, EFalse, subMask);
                }

            subMask = mask & ~subMask; // rest of targets
            
            if ( subMask )
                {
                RestoreState(iTargets[0], EStateChangeDefaultState, EFalse, subMask);
                }
            }
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::CleanupLights - Already cleaned up" ) );
        }
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::CleanupLights - return" ) );        
    }


// -----------------------------------------------------------------------------
// CHWRMLightService::ExecuteMessageL
// Handles Light requests. 
// -----------------------------------------------------------------------------
//
TBool CHWRMLightService::ExecuteMessageL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL(0x%x)" ), aMessage.Function());
    
    // If cleaning up or dummy message, something has gone wrong, so leave.
    if ( aMessage.IsNull() || iCleaningUp )
        {
        User::Leave(KErrBadHandle);
        }

    // All messages are by default asynchronous because of the nature of the plugin API
    TBool completeMessage(EFalse);

    switch ( aMessage.Function() )
        {
    	case EHWRMLightsOn:
    	    {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMLightsOn" ) );

            THWRMLightsOnCmdPackage pckg;
            aMessage.ReadL( 0, pckg );

            THWRMLightsOnData* commandData = &(pckg());
            
    	    LightsOnL( commandData->iTarget, 
    	               commandData->iDuration, 
    	               commandData->iIntensity, 
    	               commandData->iFadeIn, 
    	               aMessage );

    		break;
    	    }
    	case EHWRMLightsOff:
    	    {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMLightsOff" ) );

            THWRMLightsOffCmdPackage pckg;
            aMessage.ReadL( 0, pckg );
            
            THWRMLightsOffData* commandData = &(pckg());

    	    LightsOffL( commandData->iTarget,
    	                commandData->iDuration, 
    	                commandData->iFadeOut, 
    	                aMessage );

    		break;
    	    }
    	case EHWRMLightsBlink:
    	    {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMLightsBlink" ) );

            THWRMLightsBlinkCmdPackage pckg;
            aMessage.ReadL( 0, pckg );

            THWRMLightsBlinkData* commandData = &(pckg());

    	    LightsBlinkL( commandData->iTarget,
    	                  commandData->iDuration, 
    	                  commandData->iIntensity, 
    	                  commandData->iOnCycleDuration, 
    	                  commandData->iOffCycleDuration, 
    	                  aMessage );

    		break;
    	    }
    	case EHWRMReserveLights:
    	    {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMReserveLights") );
            
            ReserveLightsL(aMessage);
            
            completeMessage = ETrue; // Since using dummy messages for freeze state restores, need complete always.

    		break;
    	    }
    	case EHWRMReleaseLights:
    	    {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMReleaseLights") );

            // Release specified targets
            TInt targetMask(aMessage.Int0());

            ReleaseLights(targetMask);
            
            
            completeMessage = ETrue; // Since using dummy messages for default state restores, need complete always.

    		break;
    	    }
        case EHWRMGetSupportedTargets:
            {
	        // Get the supported targets and return them in parameter 0
            TPtrC8 value( reinterpret_cast<TUint8*>(&iSupportedTargets), sizeof( iSupportedTargets ) );
            aMessage.WriteL( 0, value );
            
            completeMessage = ETrue;
            
            break;
            }
    	case EHWRMCleanupLights:
    	    {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMCleanupLights" ) );

            CleanupLights();

            // Complete, as dummy messages are used in cleanup
            completeMessage = ETrue;

    		break;
    	    }
        
        case EHWRMSetLightColor:
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMSetLightColor" ) );

            THWRMSetColorCmdPackage pckg;
            aMessage.ReadL( 0, pckg );

            THWRMSetColorData* commandData = &(pckg());
            
    	    SetColorL( commandData->iTarget, 
    	               commandData->iColor, 
    	               aMessage );

    		break;
    	    }

        case EHWRMLightsOnWithColor:
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMLightsOnWithColor" ) );

            
            THWRMLightsOnCmdPackage pckg;
            aMessage.ReadL( 0, pckg );

            THWRMLightsOnData* commandData = &(pckg());

            //Data allocated here will be freed in SetColorL if target is suspended
            //else will be freed in CompleteRequest for SetColorL
            THWRMLightsOnData* lightOnData = new(ELeave) THWRMLightsOnData;
            lightOnData->iTarget    = commandData->iTarget;
            lightOnData->iFadeIn    = commandData->iFadeIn;
            lightOnData->iDuration  = commandData->iDuration;
            lightOnData->iIntensity = commandData->iIntensity;

            CleanupStack::PushL( lightOnData );
            
            // coverity[freed_arg]
            TBool pluginCommandCalled = SetColorL(commandData->iTarget, commandData->iColor, aMessage, 
                                                  EFalse, ETrue, HWRMLightCommand::ELightsOnCmdId, lightOnData);

            //SetColor will store the pointer(lightOnData) as splitcommand data.
            //This will freed after ProcessResponseL.
            CleanupStack::Pop( lightOnData );
            
            if(!pluginCommandCalled)
                {
                //Target is suspended, so call light on.
                //Message is completed to client and last state is stored.
                LightsOnL( commandData->iTarget, 
                                      commandData->iDuration, 
                                      commandData->iIntensity, 
                                      commandData->iFadeIn, 
                                      aMessage );
                }
            
            
            
            break;
            }
        
        case EHWRMLightsBlinkWithColor:
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - EHWRMLightsBlinkWithColor" ) );

            
            THWRMLightsBlinkCmdPackage pckg;
            aMessage.ReadL( 0, pckg );

            THWRMLightsBlinkData* commandData = &(pckg());

            //Data allocated here will be freed in SetColorL if target is suspended
            //else will be freed in CompleteRequest for SetColorL
            THWRMLightsBlinkData* lightBlinkData = new(ELeave) THWRMLightsBlinkData;
            
            lightBlinkData->iTarget           = commandData->iTarget;
            lightBlinkData->iDuration         = commandData->iDuration; 
            lightBlinkData->iIntensity        = commandData->iIntensity; 
            lightBlinkData->iOnCycleDuration  = commandData->iOnCycleDuration; 
            lightBlinkData->iOffCycleDuration = commandData->iOffCycleDuration;

            CleanupStack::PushL( lightBlinkData );

            // coverity[freed_arg]
            TBool pluginCommandCalled = SetColorL(commandData->iTarget, commandData->iColor, aMessage, 
                                                  EFalse, ETrue, HWRMLightCommand::ELightsBlinkCmdId, lightBlinkData);

            //SetColor will store the pointer(lightBlinkData) as splitcommand data.
            //This will freed after ProcessResponseL.
            CleanupStack::Pop( lightBlinkData );
            

            if(!pluginCommandCalled)
                {
                //Target is suspended, so call light Blink.
                //Message is completed to client and last state is stored.
                LightsBlinkL( commandData->iTarget,
                              commandData->iDuration, 
                              commandData->iIntensity, 
                              commandData->iOnCycleDuration, 
                              commandData->iOffCycleDuration, 
                              aMessage );
                }

            
            break;
            }

        // Cannot identify the message, panic the client
        default:
            {
            CHWRMServer::PanicClient(aMessage, EPanicIllegalFunction);
            break;
            }
        }
        
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ExecuteMessageL - return 0x%x" ), completeMessage );

    return completeMessage;
    }


// -----------------------------------------------------------------------------
// CHWRMLightService::ReleaseLights
// -----------------------------------------------------------------------------
//
void CHWRMLightService::ReleaseLights(TInt aTargetMask)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ReleaseLights(0x%x)" ), aTargetMask );

    TInt targetMask = aTargetMask;
    
    // Change mask if releasing system target 
    if ( targetMask & CHWRMLight::ESystemTarget )
        {
        targetMask = iFullSystemTarget;
        }
    else
        {
        // Strip unsupported targets from mask since those cannot be reserved anyway.
        targetMask &= iSupportedTargets;
        }            

    TInt restoreMask(0x0);
    TInt unreserved(iReservationHandler->Release(this, targetMask));	
           
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        // freeze only targets that were actually reserved to this session
        if ( ((targetMask & iTargets[i].iTarget) == iTargets[i].iTarget) && iTargets[i].iReserved )
            {
            // Clear reserved and suspended flags
            iTargets[i].iReserved = EFalse;
            iTargets[i].iSuspended = EFalse;
                    
            // Freeze state
            iTargets[i].iFrozenTime = (iTargets[i].iTimer->Freeze()).Int();
            iTargets[i].iFrozenState.iIntensity = iTargets[i].iLastState.iIntensity;	
            iTargets[i].iFrozenState.iState = iTargets[i].iLastState.iState;
            iTargets[i].iFrozenState.iFade = iTargets[i].iLastState.iFade;
            iTargets[i].iFrozenState.iOffCycleDuration = iTargets[i].iLastState.iOffCycleDuration;
            iTargets[i].iFrozenState.iOnCycleDuration = iTargets[i].iLastState.iOnCycleDuration;
            iTargets[i].iFrozenState.iColor = iTargets[i].iLastState.iColor;

            // Prepare to restore default state for target light if no more reservations for it exist.
            if ( (unreserved & iTargets[i].iTarget) == iTargets[i].iTarget )
                {
                restoreMask |= iTargets[i].iTarget;
                }
            }
        }
                
        // Restore default states if needed. iTargets[0] is used as dummy.
        // Note: Cannot restore both system targets and non-system targets with same command, 
        //       because they have possibly different default states, so do them separately.
        if ( restoreMask )
            {
            TInt subMask(restoreMask & iFullSystemTarget);
                
            if ( subMask )
                {
                RestoreState(iTargets[0], EStateChangeDefaultState, EFalse, subMask );
                }
                
            subMask = restoreMask & ~subMask; // rest of targets
                
            if ( subMask )
                {
                RestoreState(iTargets[0], EStateChangeDefaultState, EFalse, subMask );
                }
            }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ReleaseLights - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::ReserveLightsL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::ReserveLightsL(const RMessage2& aMessage)
    {
    // Reserve one or more targets
    TInt targetMask = aMessage.Int0();

    // Change mask if reserving system target
    if ( targetMask & CHWRMLight::ESystemTarget )
        {
        targetMask = iFullSystemTarget;
        }
    else
        {
        // check supplied mask validity
        CheckTargetParameterL( targetMask );           
        }

    TInt suspendedMask(iReservationHandler->ReserveL(aMessage, aMessage.Int2(), this, targetMask));	
            
    // Go through targets and update datas
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        if ( (targetMask & iTargets[i].iTarget) == iTargets[i].iTarget )
            {
            iTargets[i].iReserved = ETrue;
                    
            if ( (suspendedMask & iTargets[i].iTarget) == iTargets[i].iTarget )
                {
                iTargets[i].iSuspended = ETrue;
                }
                    
            // Restore previously stored state if restore was specified and
            // there is valid frozen state stored.
            if ( aMessage.Int1() && iTargets[i].iFrozenState.iState != CHWRMLight::ELightStatusUnknown)
                {
                RestoreState(iTargets[i], EStateChangeFrozenState, EFalse);
                }
            else if (aMessage.Int1() && iTargets[i].iFrozenState.iColor != KHWRMInvalidColorMask)
                {
                RMessage2 message;
                TRAP_IGNORE(SetColorL(targetMask, iTargets[i].iFrozenState.iColor, message));
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::UpdateDefaultSensitivityL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::UpdateDefaultSensitivityL(TInt aSensitivity)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::UpdateDefaultSensitivityL(%d)" ), aSensitivity );
    
    RMessage2 dummy;
    HWRMLightCommand::TLightsSensorSensitivityCmdPackage pckg(aSensitivity);
    ExecutePluginCommandL(iSupportedTargets, dummy, HWRMLightCommand::ESetLightsSensorSensitivityCmdId, pckg, EFalse);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::UpdateDefaultSensitivityL - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::UpdateDefaultIntensityL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::UpdateDefaultIntensityL(TInt aTarget, TInt aIntensity)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::UpdateDefaultIntensityL(0x%x, %d)" ), aTarget, aIntensity );
    
    RMessage2 dummy;

    HWRMLightCommand::TLightsIntensityData intensityData;
    intensityData.iTarget = aTarget;
    intensityData.iIntensity = aIntensity;
    HWRMLightCommand::TLightsIntensityCmdPackage pckg(intensityData);

    ExecutePluginCommandL(iSupportedTargets, dummy, HWRMLightCommand::ESetLightsIntensityCmdId, pckg, EFalse);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::UpdateDefaultIntensityL - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMLightService::LightsOnL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::LightsOnL(TInt aTarget,
	                   TInt aDuration, 
	                   TInt aIntensity,
	                   TBool aFadeIn,
	                   const RMessage2& aMessage,
	                   TBool aTimerInitiated,
	                   TBool aActivationInitiated)
    {
    COMPONENT_TRACE8(_L( "HWRM Server - CHWRMLightService::LightsOnL(0x%x ,%d, %d, 0x%x, 0x%x, %d, %d )" ), aTarget, 
                                                                                                              aDuration, 
                                                                                                              aIntensity, 
                                                                                                              aFadeIn, 
                                                                                                              aMessage.Handle(), 
                                                                                                              aTimerInitiated,
                                                                                                              aActivationInitiated);

    CheckCommonParametersL( aTarget, aDuration, aIntensity);
    
    TInt modTarget(aTarget);
    
    if ( !aActivationInitiated )
        {
        modTarget = ModifyTargetMaskL(aTarget, EFalse);
        }

    // If there is no target, only complete the message, if any.
    if ( modTarget )
        {
        // If intensity is KHWRMDefaultIntensity, get default intensity from common data or use sensor with sensitivity
        TInt intensensitivity = aIntensity;
        TBool useSensor = EFalse;
        if ( aIntensity == KHWRMDefaultIntensity )
            {
            if ( iLightCommonData.SensorEnabled() )
                {
                intensensitivity = iLightCommonData.DefaultSensitivity();
                useSensor = ETrue;
                }
            else
                {
                // Adaptation will be responsible for setting the correct intensity, do nothing
                }
            }
            
        // Check suspended targets, only call plugin for unsuspended ones
        TInt unsuspendedTargets = GetAndCheckUnsuspendedTargetsL(modTarget);
            
        // Call the plugin if any unsuspended targets
        if ( unsuspendedTargets )
            {
            TInt sensorTargets(0);      
            TInt nonSensorTargets(0);
            TBool splitCommand(EFalse);
            
            if ( useSensor )
                {
                // If sensor is in use, command may need splitting to sensor and non-sensor commands,
                // depending on targets.
                sensorTargets = unsuspendedTargets & iLightCommonData.SensorTargets();
                nonSensorTargets = unsuspendedTargets & ~sensorTargets;
                
                if (sensorTargets && nonSensorTargets )
                    {
                    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::LightsOnL - Splitting command required" ) );
                    splitCommand = ETrue;
                    
                    // NOTE about split commands:
                    // The ultimate completion code for messages related to split commands will be whatever
                    // completion code the split command handled last will have. I.e. if there is an error
                    // on first command but second command completes properly, whole message completes properly.
                    // So some errors may go unnoticed by client. However, it should be extremely rare that
                    // one split command will succeed and one completes with error, if environment is properly 
                    // configured. OOM case is the only possiblity that comes to mind.
                    }
                }
            else
                {
                nonSensorTargets = unsuspendedTargets;
                }
                
            if ( sensorTargets )
                {
                // Only sensor targets, no splitting
                HWRMLightCommand::TLightsOnSensorData pluginCommand;
                pluginCommand.iTarget = sensorTargets;	
                pluginCommand.iSensitivity = intensensitivity;
                pluginCommand.iFadeIn = aFadeIn;	
                HWRMLightCommand::TLightsOnSensorCmdPackage pckg(pluginCommand);
                ExecutePluginCommandL(sensorTargets, aMessage, HWRMLightCommand::ELightsOnSensorCmdId, pckg, splitCommand);
                }

            if ( nonSensorTargets )
                {
                HWRMLightCommand::TLightsOnData pluginCommand;
                pluginCommand.iTarget = nonSensorTargets;	
                
                // If targeting non-sensor targets when sensor use is indicated, use default intensity always.
                if ( useSensor )
                    {
                    pluginCommand.iIntensity = KHWRMDefaultIntensity;
                    }
                else
                    {
                    pluginCommand.iIntensity = intensensitivity;
                    }
                    
                pluginCommand.iFadeIn = aFadeIn;	
                HWRMLightCommand::TLightsOnCmdPackage pckg(pluginCommand);
                ExecutePluginCommandL(nonSensorTargets, aMessage, HWRMLightCommand::ELightsOnCmdId, pckg, splitCommand);
                }
                
            // Publish state already here if cleaning up, since no ProcessResponse will come
            if ( iCleaningUp )
                {
                iLightCommonData.PublishLightStatus(unsuspendedTargets, CHWRMLight::ELightOn);
                }
            }
        else
            {
            // complete message if suspended, as there will not be ProcessResponseL
            if ( aMessage.Handle() )
                {
                aMessage.Complete(KErrNone);
                }        
            }
        
        // Use unmodified target in state update as virtual state for inactive targets needs to be preserved.
        if ( !aActivationInitiated )
            {
            UpdateTimersAndStoreStates(aTimerInitiated, aDuration, aTarget, CHWRMLight::ELightOn, aIntensity, aFadeIn, 0, 0);
            }
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::LightsOnL - Target empty, only complete message" ) );

        // complete message if one is pending, as there will not be ProcessResponseL
        if ( aMessage.Handle() )
            {
            aMessage.Complete(KErrNone);
            }        
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::LightsOnL - return" ) );
    }

  	
// -----------------------------------------------------------------------------
// CHWRMLightService::LightsOffL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::LightsOffL(TInt aTarget,
	                    TInt aDuration, 
	                    TBool aFadeOut,
	                    const RMessage2& aMessage,
	                    TBool aTimerInitiated,
	                    TBool aActivationInitiated)
    {
    COMPONENT_TRACE7(_L( "HWRM Server - CHWRMLightService::LightsOffL(0x%x ,%d, %d, 0x%x, %d, %d)" ), aTarget, 
                                                                                                        aDuration, 
                                                                                                        aFadeOut, 
                                                                                                        aMessage.Handle(),
                                                                                                        aTimerInitiated,
                                                                                                        aActivationInitiated);

    CheckCommonParametersL( aTarget, aDuration, KHWRMDefaultIntensity);

    TInt modTarget(aTarget);
    
    if ( !aActivationInitiated )
        {
        modTarget = ModifyTargetMaskL(aTarget, ETrue);
        }

    // If there is no target, only complete the message, if any.
    if ( modTarget )
        {
        // Check suspended targets, only call plugin for unsuspended ones
        TInt unsuspendedTargets = GetAndCheckUnsuspendedTargetsL(modTarget);

        // Call the plugin if any unsuspended targets
        if ( unsuspendedTargets )
            {
            HWRMLightCommand::TLightsOffData pluginCommand;
            pluginCommand.iTarget = unsuspendedTargets;	
            pluginCommand.iFadeOut = aFadeOut;	
            HWRMLightCommand::TLightsOffCmdPackage pckg(pluginCommand);
            ExecutePluginCommandL(unsuspendedTargets, aMessage, HWRMLightCommand::ELightsOffCmdId, pckg, EFalse);

            // Publish state already here if cleaning up, since no ProcessResponse will come
            if ( iCleaningUp )
                {
                iLightCommonData.PublishLightStatus(unsuspendedTargets, CHWRMLight::ELightOff);
                }
            }
        else
            {
            // complete message if suspended, as there will not be ProcessResponseL
            if ( aMessage.Handle() )
                {
                aMessage.Complete(KErrNone);
                }        
            }
        
        // Use unmodified target in state update as virtual state for inactive targets needs to be preserved.        
        if ( !aActivationInitiated )
            {
            UpdateTimersAndStoreStates(aTimerInitiated, aDuration, aTarget, CHWRMLight::ELightOff, KHWRMDefaultIntensity, aFadeOut, 0, 0);
            }
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::LightsOffL - Target empty, only complete message" ) );

        // complete message if one is pending, as there will not be ProcessResponseL
        if ( aMessage.Handle() )
            {
            aMessage.Complete(KErrNone);
            }        
        }
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::LightsOffL - return" ) );    
    }
  	
// -----------------------------------------------------------------------------
// CHWRMLightService::LightsBlinkL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::LightsBlinkL(TInt aTarget,
	                      TInt aDuration, 
	                      TInt aIntensity,
	                      TInt aOnCycleTime,
	                      TInt aOffCycleTime,
	                      const RMessage2& aMessage,
	                      TBool aTimerInitiated,
	                      TBool aActivationInitiated)
    {
    COMPONENT_TRACE9(_L( "HWRM Server - CHWRMLightService::LightsBlinkL(0x%x ,%d, %d, %d, %d, 0x%x, %d, %d)" ), aTarget, 
                                                                                                                  aDuration, 
                                                                                                                  aIntensity, 
                                                                                                                  aOnCycleTime, 
                                                                                                                  aOffCycleTime, 
                                                                                                                  aMessage.Handle(),
                                                                                                                  aTimerInitiated,
                                                                                                                  aActivationInitiated);

    CheckCommonParametersL( aTarget, aDuration, aIntensity);
    CheckDurationParameterL( aOnCycleTime );
    CheckDurationParameterL( aOffCycleTime );

    // If one of the cycle durations is default, both must be
    if ( aOnCycleTime == KHWRMDefaultCycleTime || aOffCycleTime == KHWRMDefaultCycleTime )
        {
        if ( aOnCycleTime != aOffCycleTime )
            {
            COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::LightsBlinkL - Default cycle duration mismatch (%d,%d)" ), aOnCycleTime, aOffCycleTime);
            User::Leave(KErrArgument);
            }
        }

    TInt modTarget(aTarget);
    
    if ( !aActivationInitiated )
        {
        modTarget = ModifyTargetMaskL(aTarget, EFalse);
        }

    // If there is no target, only complete the message, if any.
    if ( modTarget )
        {
        
        // Check suspended targets, only call plugin for unsuspended ones
        TInt unsuspendedTargets = GetAndCheckUnsuspendedTargetsL(modTarget);
            
        // Call the plugin if any unsuspended targets
        if ( unsuspendedTargets )
            {
            // Call the plugin    
            HWRMLightCommand::TLightsBlinkData pluginCommand;
            pluginCommand.iTarget = unsuspendedTargets;	
            pluginCommand.iIntensity = aIntensity;
            pluginCommand.iOnCycleDuration = aOnCycleTime;
            pluginCommand.iOffCycleDuration = aOffCycleTime;
            HWRMLightCommand::TLightsBlinkCmdPackage pckg(pluginCommand);
            ExecutePluginCommandL(unsuspendedTargets, aMessage, HWRMLightCommand::ELightsBlinkCmdId, pckg, EFalse);

            // Publish state already here if cleaning up, since no ProcessResponse will come
            if ( iCleaningUp )
                {
                iLightCommonData.PublishLightStatus(unsuspendedTargets, CHWRMLight::ELightBlink);
                }
            }
        else
            {
            // complete message if suspended, as there will not be ProcessResponseL
            if ( aMessage.Handle() )
                {
                aMessage.Complete(KErrNone);
                }        
            }
            
        // Use unmodified target in state update as virtual state for inactive targets needs to be preserved.    
        if ( !aActivationInitiated )
            {
            UpdateTimersAndStoreStates(aTimerInitiated, aDuration, aTarget, CHWRMLight::ELightBlink, aIntensity, EFalse, aOnCycleTime, aOffCycleTime);
            }
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::LightsBlinkL - Target empty, only complete message" ) );

        // complete message if one is pending, as there will not be ProcessResponseL
        if ( aMessage.Handle() )
            {
            aMessage.Complete(KErrNone);
            }        
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::LightsBlinkL - return" ) );
    
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::ProcessResponseL
// Handles Light requests responses.
// CommandId not required as all commands have same response value
// -----------------------------------------------------------------------------
//
void CHWRMLightService::ProcessResponseL( TInt aCommandId,  TUint8 aTransId, TDesC8& aData, TBool aTimeout )
    {
    COMPONENT_TRACE4(_L( "HWRM Server - CHWRMLightService::ProcessResponseL(0x%x ,0x%x, <data>, 0x%x)" ), aCommandId, aTransId, aTimeout );

    TInt pluginErr(KErrNone);  // Error came from plugin as data (or timeout). Used to complete RMessage.
    TInt contextErr(KErrNone);  // Error in context, i.e. bad handle or descriptor. Used to leave.

    // Unpack the package. All light messages contain only possible error code
    // in return package.
    if ( !aTimeout && aData.Size() != sizeof(TInt) )
        {
        COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::ProcessResponseL - Data size mismatch, expected: 0x%x, got 0x%x" ), sizeof(TInt), aData.Size() );
        contextErr = KErrBadDescriptor;
        pluginErr = KErrBadDescriptor;
        }      
    else
        {        
        // If response was caused by timeout, set that as the error.
        // Timeout handled this way instead of just returning KErrTimeout in aData because
        // aData format is not known in plugin handler and in theoretically some new commands
        // may also return different response than just error code in future.
        if ( aTimeout )
            {
            pluginErr = KErrTimedOut;            
            }
        else
            {
            HWRMLightCommand::TErrorCodeResponsePackage errPckg;
            errPckg.Copy(aData);
            pluginErr = errPckg();
            }

        if ( pluginErr != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ProcessResponseL - Error: %d" ), pluginErr );        
            }
            
        // Get target of the request
        THWRMLightPluginRequestData* data = static_cast<THWRMLightPluginRequestData*>(iTransactionList->FindTransaction(aTransId, EFalse));
        
        // Update light status PS state value
        if ( data && (pluginErr == KErrNone) )
            {   
            // Do not publish statuses after cleanup has started, as those would incorrectly override statuses set by cleanup
            if ( !iCleaningUp )
                {                
                switch ( aCommandId )
                    {
                    case HWRMLightCommand::ELightsOnCmdId:
                        {
                        iLightCommonData.PublishLightStatus(data->iTarget, CHWRMLight::ELightOn);	    
                        break;
                        }
                    case HWRMLightCommand::ELightsOnSensorCmdId:
                        {
                        iLightCommonData.PublishLightStatus(data->iTarget, CHWRMLight::ELightOn);	    
                        break;
                        }
                    case HWRMLightCommand::ELightsBlinkCmdId:
                        {
                        iLightCommonData.PublishLightStatus(data->iTarget, CHWRMLight::ELightBlink);	    
                        break;
                        }
                    case HWRMLightCommand::ELightsOffCmdId:
                        {
                        iLightCommonData.PublishLightStatus(data->iTarget, CHWRMLight::ELightOff);	    
                        break;
                        }
                    case HWRMLightCommand::ESetLightsIntensityCmdId:
                        {
                        // No need to publish status in this case
                        break;
                        }
                    case HWRMLightCommand::ESetLightsSensorSensitivityCmdId:
                        {
                        // No need to publish status in this case
                        break;
                        }
                    
                    case HWRMLightCommand::ELightsSetColorCmdId:
                        {
                        COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::ProcessResponseL - ELightsSetColorCmdId iSplitCommand=0x%x iSplitCommandId=0x%x"), data->iSplitCommand, data->iSplitCommandId);
                        if(data->iSplitCommand)
                            {
                            switch(data->iSplitCommandId)
                                {
                                case HWRMLightCommand::ELightsOnCmdId:
                                    {
                                    THWRMLightsOnData *lightsOnData = (THWRMLightsOnData*)data->iSplitCommandData;
                                    if(lightsOnData)
                                        {
                                        LightsOnL( lightsOnData->iTarget, 
                                	               lightsOnData->iDuration, 
                                	               lightsOnData->iIntensity, 
                                	               lightsOnData->iFadeIn, 
                                	               data->iRequestMessage );
                                        }
                                    break;
                                    }
                                case HWRMLightCommand::ELightsBlinkCmdId:
                                    {
                                    THWRMLightsBlinkData *lightsBlinkData = (THWRMLightsBlinkData*)data->iSplitCommandData;
                                    if(lightsBlinkData)
                                        {
                                        LightsBlinkL( lightsBlinkData->iTarget,
                                                      lightsBlinkData->iDuration, 
                                                      lightsBlinkData->iIntensity, 
                                                      lightsBlinkData->iOnCycleDuration, 
                                                      lightsBlinkData->iOffCycleDuration, 
                                                      data->iRequestMessage );
                                        }
                                    break;
                                    }
                                default:
                                    {
                                    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ProcessResponseL - ELightsSetColorCmdId Invalid iSplitCommandId=0x%x"), data->iSplitCommandId);
                                    break;
                                    }
                                
                                }
                            }
                        break;
                        }
                    default:
                        {
                        iLightCommonData.PublishLightStatus(data->iTarget, CHWRMLight::ELightStatusUnknown);	    
                        break;
                        }
                    }
                }
            }
        else
            {
            // No data found or error in plugin. 
            TInt target = KHWRMAllTargets;
            
            if ( data )
                {
                target = data->iTarget;    
                }

            if(aCommandId != HWRMLightCommand::ELightsSetColorCmdId)
                {
                COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ProcessResponseL - Set ELightStatusUnknown CommandId=%d"), aCommandId);
                iLightCommonData.PublishLightStatus(target, CHWRMLight::ELightStatusUnknown);	            
                }
            }
        }

    // Complete request and delete transaction
    contextErr = CompleteRequest(aTransId, pluginErr);    

    // Leave if there is error in context    
    User::LeaveIfError(contextErr);


    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ProcessResponseL - return" ) );
    }

   
// -----------------------------------------------------------------------------
// CHWRMLightService::UpdateTimersAndStoreStates
// Stores base states of target data objects as necessary.
// This is called from various lights setting methods as opposed to 
// ProcessResponseL so that base state will be correct even if timer fires
// before plugin call completes. If plugin subsequently calls ProcessResponseL
// with error condition, the stored base state can unfortunately be
// inconsistent with real state, but that should be no problem,
// as the stored state is the last state user has requested.
// -----------------------------------------------------------------------------
//
void CHWRMLightService::UpdateTimersAndStoreStates(TBool aTimerInitiated,
    	                                TInt aDuration,
    	                                TInt aTarget,
    	                                CHWRMLight::TLightStatus aState,
                                        TInt aIntensity,
                                        TBool aFade,
                                        TInt aOnCycleTime,
                                        TInt aOffCycleTime)
    {
    COMPONENT_TRACE9(_L( "HWRM Server - CHWRMLightService::UpdateTimersAndStoreStates(0x%x, %d, 0x%x, 0x%x, %d, 0x%x, %d, %d)" ), aTimerInitiated, aDuration, aTarget, aState, aIntensity, aFade, aOnCycleTime, aOffCycleTime );

    // For system targets, use full system target
    TInt updateTarget(aTarget);
    if ( aTarget & CHWRMLight::ESystemTarget)
        {
        updateTarget = iFullSystemTarget;
        }


    // Start timer or update base state if call is not timer initiated.
    if ( !aTimerInitiated )
        {
        if ( aDuration != KHWRMInfiniteDuration )
            {
            // Start timer(s)
            for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
                {
                if ( (updateTarget & iTargets[i].iTarget) == iTargets[i].iTarget)
                    {
                    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::UpdateTimersAndStoreStates - Starting new timer with duration %d for target index %d" ), aDuration, i );    
                    iTargets[i].iTimer->Set(aDuration*1000);

                    if ( iTargets[i].iBaseState.iState == CHWRMLight::ELightStatusUnknown )
                        {
                        // If duration based call and base state unknown,
                        // set base state to off to avoid endless lighting.
                        iTargets[i].iBaseState.iState = CHWRMLight::ELightOff;
                        }
                    }
                }
            }
        else
            {
            // Since this is not timer related call, update base data and cancel related timers.
            for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
                {
                if ( (updateTarget & iTargets[i].iTarget) == iTargets[i].iTarget )
                    {
                    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::UpdateTimersAndStoreStates - Setting base state (0x%x) for target index %d" ), aState, i );    
                    iTargets[i].iBaseState.iIntensity = aIntensity;	
                    iTargets[i].iBaseState.iState = aState;
                    iTargets[i].iBaseState.iFade = aFade;
                    iTargets[i].iBaseState.iOnCycleDuration = aOnCycleTime;
                    iTargets[i].iBaseState.iOffCycleDuration = aOffCycleTime;
                    iTargets[i].iTimer->Cancel();
                    }
                }
            }
        }
    else
        {
        // no else, this is for timer related calls, so no need to cancel or set timers or base data
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::UpdateTimersAndStoreStates - Timer/resume initiated call, base states not updated" ) );    
        }
        
    // Store last states on every call
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        if ( (updateTarget & iTargets[i].iTarget) == iTargets[i].iTarget )
            {
            iTargets[i].iLastState.iIntensity = aIntensity;	
            iTargets[i].iLastState.iState = aState;
            iTargets[i].iLastState.iFade = aFade;
            iTargets[i].iLastState.iOnCycleDuration = aOnCycleTime;
            iTargets[i].iLastState.iOffCycleDuration = aOffCycleTime;
            }
        }
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::UpdateTimersAndStoreStates - return" ) );    
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::GenericTimerFired
// Handles Light cutoff timer firing. 
// aCutOff is not used with lights.
// -----------------------------------------------------------------------------
//
void CHWRMLightService::GenericTimerFired(TInt aTimerId, TBool /*aCutOff*/)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::GenericTimerFired(0x%x)" ), aTimerId );

    // Return the base state for target
    RestoreState(iTargets[aTimerId], EStateChangeBaseState, ETrue);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::GenericTimerFired - return" ) );    
    }
    

// -----------------------------------------------------------------------------
// CHWRMLightService::RestoreState
// Restores target state. aTargetData is always single target data, except in case of 
// restoring default state. In that case aDefaultTargetMask must contain a mask and
// aTargetData is ignored. It must still be a valid reference.
// aDefaultTargetMask may not contain both system and non-system targets.
// Note: If restored state used default intensity, restored state could potentially
// have different intensity or use sensor, if user has changed the settings
// while timer was active. This is correct behaviour however.
// -----------------------------------------------------------------------------
//
void CHWRMLightService::RestoreState(const THWRMLightTargetData& aTargetData, 
                                     TStateChangeType aStateType,
                                     TBool aTimerInitiated,
    	                             TInt aDefaultTargetMask,
    	                             const RMessage2* aMessage,
	                                 TBool aActivationInitiated )
    {
    COMPONENT_TRACE7(_L( "HWRM Server - CHWRMLightService::RestoreState(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), aTargetData.iTarget, 
                                                                                                                  aStateType, 
                                                                                                                  aTimerInitiated, 
                                                                                                                  aDefaultTargetMask,
                                                                                                                  aMessage ? aMessage->Handle() : 0,
                                                                                                                  aActivationInitiated );

    TInt err(KErrNone);
        
    RMessage2 message;
    
    // If non-null message pointer is provided, use that. Otherwise empty dummy message is used.
    if ( aMessage )
        {
        message = *aMessage;
        }
    
    // Get correct state data
    TInt duration(KHWRMInfiniteDuration);
    TInt targetMask(aTargetData.iTarget);
    const THWRHLightTargetStateData* targetStateData = NULL;
    

    TUint color = KHWRMDefaultColor;
    
    switch ( aStateType )
        {
        case EStateChangeFrozenState: 
            {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightService::RestoreState - EStateChangeFrozenState"));
            targetStateData = &(aTargetData.iFrozenState);
            duration = aTargetData.iFrozenTime;
            color = targetStateData->iColor;
            break;
            }
        case EStateChangeBaseState: 
            {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightService::RestoreState - EStateChangeBaseState"));
            targetStateData = &(aTargetData.iBaseState);
            color = targetStateData->iColor;
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMLightService::RestoreState - EStateChangeBaseState color(0x%x)"), color);
            break;
            }
        case EStateChangeLastState: 
            {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightService::RestoreState - EStateChangeLastState"));
            targetStateData = &(aTargetData.iLastState);
            color = targetStateData->iColor;
            break;
            }
        case EStateChangeDefaultState: 
            {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightService::RestoreState - EStateChangeDefaultState"));
            // Target mask may not contain both system and non-system targets
            targetMask = aDefaultTargetMask;         
            if (targetMask & iFullSystemTarget)
                {
                COMPONENT_TRACE1(_L("HWRM Server - CHWRMLightService::RestoreState - GetData"));
                iDefaultState.iState = iLightCommonData.DefaultState();
                }
            else
                {
                iDefaultState.iState = CHWRMLight::ELightOff;
                }

            //Restore color only if light state is off.
            if(iDefaultState.iState != CHWRMLight::ELightOff)
                {
                color = KHWRMInvalidColorMask;
                }
            
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMLightService::RestoreState - DefaultState=0x%x"), iDefaultState.iState);
            targetStateData = &iDefaultState;  
            break;
            }
        default: 
            {
            // Cannot get here. Panic if it does for some reason.
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::RestoreState - Invalid state: 0x%x - Panicing" ), aStateType );
            User::Panic(KPanicCategory, EPanicBadHandle);
            break;
            }
        }
        
    // If state to be restored is unknown, default state is restored instead.
    // Basically this is issue only when duration based call is used prior to infinite calls.
    //
    // Note: This can result in different targets being restored to different default
    //       states, if default state changes between target restorings. Should be
    //       extremely rare though, considering that most light users will have set
    //       some base state anyway. 
    if ( targetStateData->iState == CHWRMLight::ELightStatusUnknown )
        {
        // Default state of non-system targets is always off
        if (targetMask & iFullSystemTarget)
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::RestoreState - State for state type (0x%x) unknown - Using default state" ), aStateType );
            iDefaultState.iState = iLightCommonData.DefaultState();
            targetStateData = &iDefaultState;
            }
        else
            {
            iDefaultState.iState = CHWRMLight::ELightOff;
            targetStateData = &iDefaultState;
            }
        }
    
    if(color != KHWRMInvalidColorMask)
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::RestoreState - Restore color with 0x%x" ), color);
        TRAP_IGNORE(SetColorL(targetMask, color, message, aActivationInitiated));
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::RestoreState - Invalid Color Mask"));
        }

    // Call appropriate method to restore target state
    switch ( targetStateData->iState )
        {
        case CHWRMLight::ELightOn:
            {
            TRAP(err, LightsOnL(targetMask, duration, targetStateData->iIntensity, targetStateData->iFade, message, aTimerInitiated, aActivationInitiated));
            break;
            }
        case CHWRMLight::ELightOff:
            {
            TRAP(err, LightsOffL(targetMask, duration, targetStateData->iFade, message, aTimerInitiated, aActivationInitiated));
            break;
            }
        case CHWRMLight::ELightBlink:
            {
            TRAP(err, LightsBlinkL(targetMask, duration, targetStateData->iIntensity, targetStateData->iOnCycleDuration, targetStateData->iOffCycleDuration, message, aTimerInitiated, aActivationInitiated));
            break;
            }
        default:
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::RestoreState - Invalid target status: 0x%x" ), targetStateData->iState );
            break;
            }
        }

    if ( err != KErrNone )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::RestoreState - Error in restoring light state: %d" ), err );
        
        // complete message if one is pending, as there will not be ProcessResponseL
		if ( aMessage )
			{
        	if ( aMessage->Handle() )
            	{
            	aMessage->Complete(KErrNone);
            	}
			}
        }
        
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::RestoreState - return" ) );    
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::CheckCommonParametersL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::CheckCommonParametersL(TInt aTarget, 
                                               TInt aDuration, 
                                               TInt aIntensity) const
    {
    CheckTargetParameterL(aTarget);
    
    CheckDurationParameterL(aDuration);    

    if ( aIntensity != KHWRMDefaultIntensity && (aIntensity < KHWRMLightMinIntensity || aIntensity > KHWRMLightMaxIntensity) )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::CheckCommonParametersL - Invalid Intensity: %d" ), aIntensity );    
        User::Leave(KErrArgument);
        }
    }
        
// -----------------------------------------------------------------------------
// CHWRMLightService::CheckDurationParameterL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::CheckDurationParameterL(TInt aDuration) const
    {
    if ( aDuration < 0 || aDuration > KHWRMLightMaxDuration )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::CheckCommonParametersL - Invalid Duration: %d" ), aDuration );    
        User::Leave(KErrArgument);
        }
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::CheckTargetParameterL
// -----------------------------------------------------------------------------
//
void CHWRMLightService::CheckTargetParameterL(TInt aTarget) const
    {
    // All target combinations specified in supported targets mask + system target are legal
    if ( (~(iSupportedTargets | CHWRMLight::ESystemTarget) & aTarget) || aTarget == CHWRMLight::ENoTarget  )
        {
        COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::CheckCommonParametersL - Invalid Target: 0x%x. (Supported: 0x%x)" ), aTarget, iSupportedTargets );    
        User::Leave(KErrNotSupported);
        }
    }
        
// -----------------------------------------------------------------------------
// CHWRMLightService::SuspendSubResource
// Suspends light target
// Suspension doesn't actually change light state, it is responsibility of the
// active reserver of the target to change the state to desirable one.
// -----------------------------------------------------------------------------
//
void CHWRMLightService::SuspendSubResource(TInt aSubResource)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::SuspendSubResource(0x%x)" ), aSubResource );

    iTargets[aSubResource].iSuspended = ETrue;

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::SuspendSubResource - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMLightService::ResumeSubResource
// Resumes suspended light target 
// -----------------------------------------------------------------------------
//
void CHWRMLightService::ResumeSubResource(TInt aSubResource)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ResumeSubResource(0x%x)" ), aSubResource );
    
    iTargets[aSubResource].iSuspended = EFalse;
    
    // Restore last state
    // Set aTimerInitiated to ETrue on RestoreState call, so that base states and timers will 
    // not be affected by this state change.
    RestoreState(iTargets[aSubResource], EStateChangeLastState, ETrue);
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ResumeSubResource - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMLightService::ActivateSubResource
// If subresource is activated, restore its state. 
// If it is deactivated, turn it off
// -----------------------------------------------------------------------------
//
void CHWRMLightService::ActivateSubResource(TInt aSubResource, TBool aActivate)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::ActivateSubResource(0x%x, 0x%x)" ), aSubResource, aActivate );
    
    RMessage2 message; // dummy message

    // Set aActivationIntiated to ETrue on RestoreState/lights off calls, so that target is not recursively modified,
    //  and states/timers will not be affected by this call.
    
    if (aActivate)
        {
        RestoreState(iTargets[aSubResource], EStateChangeLastState, ETrue, CHWRMLight::ENoTarget, &message, ETrue);
        }
    else
        {
        // Just turn target off 
        TRAPD(err, LightsOffL(iTargets[aSubResource].iTarget, KHWRMInfiniteDuration, iDefaultState.iFade, message, ETrue, ETrue));
        
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ActivateSubResource - error turning lights off: %d" ), err );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::GetAndCheckUnsuspendedTargetsL
// Checks which targets of specified aTarget mask are unsuspended and returns
// a new mask of those targets.
// -----------------------------------------------------------------------------
//
TInt CHWRMLightService::GetAndCheckUnsuspendedTargetsL(TInt aTarget) const
    {
    TInt unsuspendedTargets(0);
    
    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        if ( !iTargets[i].iSuspended && (aTarget & iTargets[i].iTarget) )
            {
            unsuspendedTargets |= iTargets[i].iTarget;
            }
        }
   
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::GetAndCheckUnsuspendedTargetsL: 0x%x" ), unsuspendedTargets );
    
    // If there are any unsuspended targets, check reservation status for those.
    if ( unsuspendedTargets )
        {
        // Check reservation.
        if ( iReservationHandler->IsReserved(this, unsuspendedTargets) )
            {
            User::Leave(KErrInUse);
            }
        }
        
    return unsuspendedTargets;
    }

// -----------------------------------------------------------------------------
// CHWRMLightService::ExecutePluginCommandL
// Executes a plugin command and handles related transaction.
// -----------------------------------------------------------------------------
//
void CHWRMLightService::ExecutePluginCommandL(TInt aTarget,
                          const RMessage2& aMessage,
                          TInt aCommandId,
                          TDesC8& aPckg,
                          TBool aSplitCommand,
                          TInt aSplitCommandId , 
                          TAny* aSplitCommandData  )
    {
    // Create new data (TransId is updated later)
    THWRMPluginRequestData* data = new (ELeave) THWRMLightPluginRequestData(aTarget, aMessage, 0, aCommandId, aSplitCommand, aSplitCommandId, aSplitCommandData);
    CleanupStack::PushL( data );
    
    // When cleaning up, do not set callback as ProcessResponse would result in panic
    if ( iCleaningUp )
        {
        data->iTransId = iPluginHandler->ProcessCommandL(aCommandId, aPckg, NULL);
        }
    else
        {        
        data->iTransId = iPluginHandler->ProcessCommandL(aCommandId, aPckg, this);
        }
    
    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );

    // Add data to list
    iTransactionList->AddTransaction( data );
    }
    
// -----------------------------------------------------------------------------
// CHWRMLightService::SystemLightsOn
// Turns all unreserved system lights on.
// Custom lights are assumed to be specialty lights and therefore not grouped
// for this common turn on of lights.
// -----------------------------------------------------------------------------
//
void CHWRMLightService::SystemLightsOn()    
    {   
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::SystemLightsOn()" ) );

    TInt err(KErrNone);

    RMessage2 dummy;
    
    TRAP(err, LightsOnL(CHWRMLight::ESystemTarget, KHWRMInfiniteDuration, KHWRMDefaultIntensity, EFalse, dummy ));

    if ( err != KErrNone )
        {
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::SystemLightsOn - Error setting lights on: %d" ), err );
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::SystemLightsOn - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMLightService::ModifyTargetMaskL
// Modifies target mask depending on current device configuration
// Note: Recursive calls to this method caused by activation callbacks from
//       ReservationHandler must be avoided. Therefore target is only modified
//       initially and activation caused calls will not call this method.
// -----------------------------------------------------------------------------
//
TInt CHWRMLightService::ModifyTargetMaskL(TInt aTarget, TBool aLightsOff)
    {
    TInt modifiedMask(CHWRMLight::ENoTarget);
    TInt target(aTarget);
    
    if ( aTarget & CHWRMLight::ESystemTarget )
        {
        target = iFullSystemTarget;
        }

    // Non-system target modifications are passed directly to plugin
    modifiedMask = iLightCommonData.ModifyTargetL( target );
    
    // Make sure modified mask doesn't contain any targets not in original mask,
    // as this can cause problems with ReservationHandler and is therefore not supported.
    modifiedMask &= target;

    // Update active mask to reservation handler
    iReservationHandler->UpdateActiveMask(target, modifiedMask, this);    

    // If command is lights off, do not return modified mask but the original 
    // or reservation corrected system mask, as lights off is ok for inactive targets.
    if ( aLightsOff )
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::ModifyTargetMaskL - Lights off, target modification only used for activation purposes" ) );
        modifiedMask = target;
        }
        
    if ( aTarget & CHWRMLight::ESystemTarget )
        {
        TInt reservations(CHWRMLight::ENoTarget);
        
        // Get targets reserved for this session
        for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
            {
            if ( iTargets[i].iReserved )
                {
                reservations |= iTargets[i].iTarget;
                }
            }
            
        // Get Unreserved targets
        reservations |= iReservationHandler->GetUnreservedTargets();    
               
        modifiedMask &= reservations;

        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ModifyTargetMaskL - final system mask: 0x%x" ), modifiedMask );
        }
    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::ModifyTargetMaskL - returning: 0x%x" ), modifiedMask );
            
    return modifiedMask;
    }

TBool CHWRMLightService::SetColorL(TInt aTarget, TUint aColor, const RMessage2& aMessage, TBool aActivationInitiated, 
                                  TBool aSplitCommand, TInt aSplitCommandId, TAny* aSplitCommandData)
    {
    COMPONENT_TRACE7(_L( "HWRM Server - CHWRMLightService::SetColorL(0x%x, 0x%x, %d, 0x%x, 0x%x, 0x%x)" ), 
                        aTarget, aColor, aActivationInitiated, aSplitCommand, aSplitCommandId, aSplitCommandData);
    
    TBool pluginCommandCalled = EFalse;
    
    CheckTargetParameterL( aTarget);

    TInt modTarget(aTarget);
    
    if ( !aActivationInitiated )
        {
        modTarget = ModifyTargetMaskL(aTarget, EFalse);
        }

    // Check suspended targets, only call plugin for unsuspended ones
    TInt unsuspendedTargets = GetAndCheckUnsuspendedTargetsL(modTarget);

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::SetColorL unSuspended=0x%x)" ), unsuspendedTargets);
        
    // Call the plugin if any unsuspended targets
    if ( unsuspendedTargets )
        {
        HWRMLightCommand::TLightsColorData pluginCommand;
        Mem::FillZ(&pluginCommand, sizeof(HWRMLightCommand::TLightsColorData));
        
        pluginCommand.iTarget = unsuspendedTargets;
        
        if(aColor & KHWRMSetColorMask)
            {
            
            pluginCommand.iBlue   = ((aColor)                   & 0xFF);
            pluginCommand.iGreen  = ((aColor>>KHWRMGreenShift)  & 0xFF);
            pluginCommand.iRed    = ((aColor>>KHWRMRedShift)    & 0xFF);
            
            pluginCommand.iDefaultColor = EFalse; 
            }
        else
            {
            pluginCommand.iDefaultColor = ETrue;
            }
        
        HWRMLightCommand::TLightsColorCmdPackage pckg(pluginCommand);

        ExecutePluginCommandL(unsuspendedTargets, aMessage, HWRMLightCommand::ELightsSetColorCmdId, pckg, aSplitCommand, aSplitCommandId, aSplitCommandData);

        pluginCommandCalled = ETrue;
        }
    else
        {
        // complete message if suspended, as there will not be ProcessResponseL
        if ( aMessage.Handle() && (!aSplitCommand))
            {
            aMessage.Complete(KErrNone);
            }  
        
        if (aSplitCommand && aSplitCommandData)
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMLightService::SetColorL Deleting allocated aSplitCommandData"));
            //Release the data that is allocated, as there will not be ProcessResponseL  
            //coverity[freed_arg]
            delete aSplitCommandData;
            }
        }

    for( TInt i = 0; i < KHWRMLightMaxTargets; i++ )
        {
        if ( (aTarget & iTargets[i].iTarget) == iTargets[i].iTarget )
            {            
            if(aSplitCommand)
                {
                //Store the base state only if it is duration based call.
                TUint color = iTargets[i].iLastState.iColor;
                if(color == KHWRMInvalidColorMask)
                    {
                    color = KHWRMDefaultColor;
                    }
                COMPONENT_TRACE3(_L( "HWRM Server - CHWRMLightService::SetColorL BaseStateColor(0x%x) for target(0x%x)"), color, i);
                iTargets[i].iBaseState.iColor = color;
                }
            else
                {
                //iTargets[i].iBaseState.iColor = KHWRMInvalidColorMask;
                }
            iTargets[i].iLastState.iColor = aColor;    
            }
        }

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMLightService::SetColorL - return %d" ), pluginCommandCalled );

    return pluginCommandCalled;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
