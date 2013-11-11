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
#include "HWRMVibraClientServer.h"
#include "HWRMService.h"
#include "HWRMVibraService.h"
#include "HWRMVibraCommands.h"
#include "HWRMPluginHandler.h"
#include "HWRMtrace.h"
#include "HWRMVibraCommonData.h"
#include "HWRMVibra.h"
#include "HWRMReservationHandler.h"
#include "HWRMPrivateCRKeys.h"
#include "HWRMDomainPSKeys.h"
#include "HWRMPrivatePSKeys.h"
#include "HWRMVibraCommonService.h"



// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "HWRMVibraService" );

const TInt KHWRMVibraMaxDurationInMicroseconds = KHWRMVibraMaxDuration * 1000;

const TInt KHWRMVibraTimerID = KMaxTInt;
const TInt KHWRMVibraNoTimer = KMinTInt;


// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMVibraService::CHWRMVibraService
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMVibraService::CHWRMVibraService(CHWRMVibraCommonService& aCommonService,
                                     CHWRMVibraCommonData& aVibraCommonData,
                                     TBool aPrivilegedClient)
    : iVibraCommonData(aVibraCommonData),
      iPrivilegedClient(aPrivilegedClient),
      iStateSetAfterReservation(EFalse),
      iCommonService(aCommonService),
      iSid(0),
      iActiveTimerId(KHWRMVibraTimerID)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::CHWRMVibraService()" ) );
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::CHWRMVibraService - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraService::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::ConstructL(CHWRMPluginHandler* aPluginHandler,
                                   CHWRMReservationHandler* aReservationHandler)
    {    
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::ConstructL(0x%x)" ), aPluginHandler );

    BaseConstructL(aPluginHandler, aReservationHandler);
    
    // Create vibra timer with special identifier.
    iVibraTimer = CHWRMGenericTimer::NewL(*this, iVibraCommonData.MaximumOnTime(), KHWRMVibraTimerID);
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::ConstructL - return " ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMVibraService* CHWRMVibraService::NewL(CHWRMPluginHandler* aPluginHandler,
                                           CHWRMReservationHandler* aReservationHandler,
                                           CHWRMVibraCommonService& aCommonService,
                                           CHWRMVibraCommonData& aVibraCommonData,
                                           TBool aPrivilegedClient)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraService::NewL(0x%x, aPrivilegedClient: 0x%x)" ), aPluginHandler, aPrivilegedClient );
    
    CHWRMVibraService* self = new( ELeave ) CHWRMVibraService(aCommonService, aVibraCommonData, aPrivilegedClient);
  
    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler, aReservationHandler);
    CleanupStack::Pop();

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMVibraService::~CHWRMVibraService()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::~CHWRMVibraService()" ) );

    // Cleanup vibra just in case regular cleanup failed
    CleanupVibra();    

    // Delete timer
    if (iVibraTimer)
        {
        iVibraTimer->Cancel();
        delete iVibraTimer;
        }

    ReleasePulseData();

    // Base class cleanup
    BaseDestruct();

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::~CHWRMVibraService - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraService::ReleasePulseData
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::ReleasePulseData()
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::ReleasePulseData(), count %d" ), iPulseData.Count() );

    iPulseData.ResetAndDestroy();
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::ReleasePulseData - return" ) );
    }


// -----------------------------------------------------------------------------
// CHWRMVibraService::CleanupVibra
// Cleans up vibra. 
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::CleanupVibra()
    {    
    if ( !iCleanupDone )
        {        
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::CleanupVibra()" ) );
        TInt unreserved(0x0);
        
        // If still reserved, release
        if ( iReserved )
            {
            unreserved = iReservationHandler->Release(this, KHWRMAllSubResources);	
            iReserved = EFalse;
            iSuspended = EFalse;
            }
        else
            {
            unreserved = iReservationHandler->GetUnreservedTargets();
            }

        COMPONENT_TRACE3(_L("HWRM Server - CHWRMVibraService::CleanupVibra - Vibra status: %d, reservation status: %d"), iLastStatus, unreserved );

        // If this session has set vibra to vibrate and vibra is unreserved, stop it.
        if ( unreserved && iLastStatus == CHWRMVibra::EVibraStatusOn )
            {
            RMessage2 dummy; // Use dummy to avoid adaptation returned error messages failing cleanup            
            TRAPD(err, StopVibraL(dummy, EFalse, ETrue));
            if ( err != KErrNone )
                {
                COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraService::CleanupVibra - Stopping vibra failed: %d"), err );
                }
            else 
                {
                // Since there will be no ProcessResponse for this one, publish the status now
                iVibraCommonData.PublishVibraStatus(CHWRMVibra::EVibraStatusStopped, iPrivilegedClient);	    
                }
            }
            
        // Make sure client requests get cleared if not via StopVibraL
        if( iCommonService.HasData( this ) )
            {
            iCommonService.ResetClientData( this );
            }

        iCleanupDone = ETrue;
        
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::CleanupVibra - return" ) );    
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::CleanupVibra - Already cleaned up" ) );    
        }
    }

// -----------------------------------------------------------------------------
// CHWRMVibraService::ForceVibraStop
// Stops vibra. 
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::ForceVibraStop()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::ForceVibraStop()" ) );
    
    RMessage2 dummy;
    TRAP_IGNORE(StopVibraL(dummy, ETrue));
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::ForceVibraStop - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraService::ExecuteMessageL
// Handles Vibra requests. 
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraService::ExecuteMessageL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::ExecuteMessageL(0x%x)" ), aMessage.Function() );
    
    __ASSERT_ALWAYS(iPluginHandler, User::Panic(KPanicCategory, EPanicBadHandle));
    __ASSERT_ALWAYS(iReservationHandler, User::Panic(KPanicCategory, EPanicBadHandle));
    
    if ( aMessage.IsNull() )
        {
        User::Leave(KErrBadHandle);
        }

    // All messages are by default asynchronous because of the nature of the plugin API
    TBool completeMessage(EFalse);
    TInt function( aMessage.Function() );

	// Retrieve client sid
    iSid = aMessage.SecureId();

    switch ( function )
        {
    	case EHWRMStartVibraDefaultIntensity:
    	    {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMStartVibraDefaultIntensity") );

            StartVibraL( aMessage.Int0() * 1000, KUseDefaultIntensity, aMessage);   

    		break;
    	    }
    	case EHWRMStartVibra:
    	    {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMStartVibra") );

            StartVibraL(aMessage.Int0() * 1000, aMessage.Int1(), aMessage);            

    		break;
    	    }
    	case EHWRMPulseVibraDefault:
    	case EHWRMPulseVibraDefaultIntensity:
    	case EHWRMPulseVibra:
    	    {
            // First of all check whether pulse request allowed by this client.
            if ( !iCommonService.VibraPulseAllowed( aMessage.SecureId() ) )
                {
                User::Leave(KErrAccessDenied);
                }

            TInt duration( iVibraCommonData.FeedbackDefaultDuration() * 1000 );
            TInt intensity( iVibraCommonData.FeedbackDefaultIntensity() );

            if( function == EHWRMPulseVibraDefault )
                {
                COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMPulseVibraDefault") );
                }
            else if( function == EHWRMPulseVibraDefaultIntensity )
                {
                COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMPulseVibraDefaultIntensity") );
                duration = aMessage.Int0() * 1000;
                }
            else
                {
                COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMPulseVibra") );
                duration = aMessage.Int0() * 1000;
                intensity = aMessage.Int1();
                }
            
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - pulse supported %d"), 
                iVibraCommonData.FeedbackPulseSupported() );

            // If there is active reservation and vibration ongoing by a higher priority client, 
            // pulse request is not allowed. In all other circumstances either pulse or start 
            // command is sent to plugin.
            if ( iReservationHandler->IsReserved(this, KHWRMAllSubResources) &&
                 iCommonService.ExecutingPriorityHigher( aMessage.SecureId() ) )
                {
                User::Leave(KErrInUse);
                }

            if( iVibraCommonData.FeedbackPulseSupported() )
                {
                // It is plugin or underlying software responsibility to generate pulse start/stop
                // and continue ongoing vibration if such exists.
                PulseVibraL( duration, intensity, aMessage);
                }
            else
                {
                // Request another vibration by lefting current timer if such exist intact
                // and creating new timer for this pulse request.
                StartVibraL( duration, intensity, aMessage, EFalse, ETrue);
                }
                
            break;    
    	    }
    	case EHWRMStopVibra:
    	    {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMStopVibra") );

            StopVibraL(aMessage);            
    		break;
    	    }
    	case EHWRMReserveVibra:
    	    {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMReserveVibra") );
            
            // Reserve whole vibra. If returned mask is non-zero, reservation was done in suspended mode.
            iSuspended = (iReservationHandler->ReserveL(aMessage, aMessage.Int1(), this, KHWRMAllSubResources));	
                           
            iReserved = ETrue;           
            
            iStateSetAfterReservation = EFalse;    
                           
            // Restore previously stored state if restore was specified.
            // Errors are trapped as reservation itself was successful.
            if ( aMessage.Int0() )
                {
                TInt err(KErrNone);
                RMessage2 dummy; // Use dummy to avoid adaptation returned error messages failing reservation.
                
                if ( iFrozenStatus == CHWRMVibra::EVibraStatusOn )
                    {
                    TRAP(err, StartVibraL(iFrozenTime, iFrozenIntensity, dummy));
                    }
                else
                    {
                    TRAP(err, StopVibraL(dummy));
                    }
                }

            completeMessage = ETrue; // Complete needed, as dummy used.
    		break;
    	    }
    	case EHWRMReleaseVibra:
    	    {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMReleaseVibra") );

			// If a "Leave" is introduced, consideration must be given to the corresponding
			// client side side code which requested this action. In particular, ensure that
			// the client-side caller doesn't set its "iReserved" flag to false without 
			// checking the result of the Release command.
        
            // Release whole vibra. 
            TInt unreserved = iReservationHandler->Release(this, KHWRMAllSubResources);	
            
            iReserved = EFalse;
            
            // Clear suspended flag
            iSuspended = EFalse;
            
            // Freeze state
            iFrozenTime = iVibraTimer->Freeze(); 
            iFrozenStatus = iLastStatus;
            iFrozenIntensity = iLastIntensity;
            
            // Restore base state if no more reservations for vibra.
            // Errors are trapped as release itself was successful.
            if ( unreserved )
                {
                RMessage2 dummy; // Use dummy to avoid adaptation returned error messages failing release.
                TRAP_IGNORE(StopVibraL(dummy));
                }
                
            completeMessage = ETrue; // Complete needed, as dummy used.
    		break;
    	    }
        case EHWRMCleanupVibra:
            {
            COMPONENT_TRACE1(_L("HWRM Server - CHWRMVibraService::ExecuteMessageL - EHWRMCleanupVibra") );

            CleanupVibra();
                
            completeMessage = ETrue; // Complete needed, as dummy used in CleanupVibra.
    		break;
    	    }
        // Cannot identify the message, panic the client
        default:
            {
            CHWRMServer::PanicClient(aMessage, EPanicIllegalFunction);
            break;
            }
        }

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::ExecuteMessageL - return 0x%x" ), completeMessage );

    return completeMessage;
    
   }
// -----------------------------------------------------------------------------
// CHWRMVibraService::StartVibraL
// Starts vibra for duration with specified intensity.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::StartVibraL(const TTimeIntervalMicroSeconds32& aDuration, 
                                    TInt aIntensity, 
                                    const RMessage2& aMessage,
	                                TBool aSetTimer,
                                    TBool aPulse)
	{
    
    COMPONENT_TRACE5(_L( "HWRM Server - CHWRMVibraService::StartVibraL(%d, %d, %d, %d)" ),
    aDuration.Int(), aIntensity, aSetTimer, aPulse );


    __ASSERT_ALWAYS(iPluginHandler, User::Panic(KPanicCategory, EPanicBadHandle));

    // Duration must not be negative.
    if ( aDuration.Int() < 0 || aDuration.Int() > KHWRMVibraMaxDurationInMicroseconds )
        {
        User::Leave(KErrArgument);
        }
        
    // Check vibra/feedback profile settings if not privileged client.
    // If not pulse request, vibra must be on. If pulse request, feedback must be on.
    if ( !iPrivilegedClient && 
        ( ( !aPulse && (iVibraCommonData.VibraState() != CHWRMVibra::EVibraModeON ) ) ||
          ( aPulse && (iVibraCommonData.VibraFeedbackState() != CHWRMVibra::EVibraFeedbackModeON) ) ) )        
    	{
        // Stop vibra if it needs shutdown.
        if ( iLastStatus == CHWRMVibra::EVibraStatusOn )
            {
            StopVibraL(aMessage);
            }
        User::Leave(KErrAccessDenied);
        }
    
    // Check if vibra is cooling off or feedback should be prevented
    if ( iVibraCommonData.VibraLocked() || (aPulse && VibraPulseBlocked()) )
        {
        User::Leave(KErrLocked);
        }
           
	// Check if vibra is blocked (aPulse means no feedback request)
    if ( !aPulse &&
         iVibraCommonData.VibraBlocked() && 
         iVibraCommonData.VibraBlocked( KHWRMInternalSettingsChargerBlockVibra ) )
        {
        User::Leave(KErrLocked);
        }
        

    // Check intensity validity
    if ( aIntensity != KUseDefaultIntensity && (aIntensity < KHWRMVibraMinIntensity || aIntensity > KHWRMVibraMaxIntensity) )
        {
        User::Leave(KErrArgument);
        }
        
    // Check intensity validity for pulse requests
    if ( aPulse && (aIntensity < KHWRMVibraMinPulseIntensity || aIntensity > KHWRMVibraMaxIntensity) )
        {
        User::Leave(KErrArgument);
        }

    // Call plugins
    if ( aIntensity == 0 )
        {
        // Stop vibra, if intensity is zero
        StopVibraL(aMessage);
        }
    else
        {
        // Start timers, if so specified
        // This is because when restoring last state, existing timer should be left intact.
        // Vibra timer is left intact also if pulse requested because new timer is setup.
        if ( aSetTimer && !aPulse )
            {
            iVibraTimer->Set(aDuration);
            iActiveTimerId = KHWRMVibraTimerID;
            }

        if ( aPulse )
            {
            // Create new timer with ascending identifier.
            TInt count( iPulseData.Count() );
            CHWRMGenericTimer* timer = 
                CHWRMGenericTimer::NewL(*this, iVibraCommonData.MaximumOnTime(), count );                                
            iActiveTimerId = count;
            CPulseData* pulseData = CPulseData::NewL(*timer, iLastIntensity, aIntensity, iActiveTimerId );
            CleanupStack::PushL( pulseData );
            iPulseData.AppendL( pulseData );
            CleanupStack::Pop( pulseData );
            iPulseData[count]->Timer().Set(aDuration);
            }

		// complete message if suspended, as there will not be ProcessResponseL
        if ( iSuspended && aMessage.Handle() )
            {
            aMessage.Complete(KErrNone);
            }

        // Call plugin if not suspended
        if ( !iSuspended )
            {
            // Check reservation if not pulse request (pulse is blocked earlier if priority
            // of this client is lower than reserving client).
            if ( !aPulse && iReservationHandler->IsReserved(this, KHWRMAllSubResources) )
                {
                User::Leave(KErrInUse);
                }

            // Create new data (TransId is updated later, commandId is not important)
            THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(aMessage, 0, 0, EFalse); 
            CleanupStack::PushL( data );

            if ( aIntensity == KUseDefaultIntensity )
                {
                // No data, pass empty descriptor
                TBuf8<1> emptyDes;
                data->iTransId = iPluginHandler->ProcessCommandL(HWRMVibraCommand::EStartVibraWithDefaultSettingsCmdId, emptyDes, this);
                }
            else 
                {
                HWRMVibraCommand::TStartVibraCmdDataPackage dataPckg(aIntensity);    
                data->iTransId = iPluginHandler->ProcessCommandL(HWRMVibraCommand::EStartVibraCmdId, dataPckg, this);    
                }

            // data still needed, do not destroy, just pop
            CleanupStack::Pop( data );
            
            // Add data to list
            iTransactionList->AddTransaction( data );
            
            // Assign request data for common vibra service
            TVibraClientData clientData( this, iSid, aIntensity );
            // Control of overlapping pulses/normal vibration requests is in hands of HWRM,
            // when pulse length is configured to be handled by HWRM. That is why
            // client data must be set to common vibra service.
            iCommonService.SetClientData( clientData );
            

            iStateSetAfterReservation = ETrue;
            }
        }
        // Store the latest status in case vibra gets suspended before next plugin call.
        iLastStatus = CHWRMVibra::EVibraStatusOn;
        iLastIntensity = aIntensity;        
        
    	COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::StartVibraL - return" ) );    
    }

// -----------------------------------------------------------------------------
// CHWRMVibraService::VibraPulseBlocked
// Checks whether vibra pulse should be blocked.
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraService::VibraPulseBlocked()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::VibraPulseBlocked" ) );
    TBool blocked( EFalse );
    
    // Check whether vibra cooling off should prevent feedback 
    if ( iVibraCommonData.VibraLocked() && 
         iVibraCommonData.VibraBlocked( KHWRMInternalSettingsCoolOffBlockFeedback ) )
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::VibraPulseBlocked - blocked by cool-off" ));
        blocked = ETrue;
        }

    // Check if vibra is blocked and whether it should prevent feedback
    if ( (iVibraCommonData.VibraBlocked() & KHWRMAccessoryVibraBlocked) && 
         iVibraCommonData.VibraBlocked( KHWRMInternalSettingsAccessoryBlockFeedback ) )
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::VibraPulseBlocked - blocked by accessory" ) );
        blocked = ETrue;
        }
    if ( (iVibraCommonData.VibraBlocked() & KHWRMInternalVibraBlocked) && 
         iVibraCommonData.VibraBlocked( KHWRMInternalSettingsChargerBlockFeedback ) )
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::VibraPulseBlocked - blocked by charger" ) );
        blocked = ETrue;
        }
    
    return blocked;
    }

// -----------------------------------------------------------------------------
// CHWRMVibraService::PulseVibraL
// Requests vibra pulse with specified intensity and duration.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::PulseVibraL(const TTimeIntervalMicroSeconds32& aDuration, 
                                    TInt aIntensity, 
                                    const RMessage2& aMessage,
	                                TBool /* aSetTimer */)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraService::PulseVibraL(%d, %d)" ), aDuration.Int(), aIntensity );

    __ASSERT_ALWAYS(iPluginHandler, User::Panic(KPanicCategory, EPanicBadHandle));

    // Duration must not be negative.
    if ( aDuration.Int() < 0 || aDuration.Int() > KHWRMVibraMaxDurationInMicroseconds )
        {
        User::Leave(KErrArgument);
        }
        
    // Check intensity validity
    if ( aIntensity < KHWRMVibraMinPulseIntensity || aIntensity > KHWRMVibraMaxIntensity )
        {
        User::Leave(KErrArgument);
        }

    if( VibraPulseBlocked() )
        {
        User::Leave(KErrLocked);
        }
    
    // Check vibra profile setting if not privileged client
    if ( !iPrivilegedClient && iVibraCommonData.VibraFeedbackState() != CHWRMVibra::EVibraFeedbackModeON )
        {
        // Stop vibra if it needs shutdown.
        if ( iLastStatus == CHWRMVibra::EVibraStatusOn )
            {
            StopVibraL(aMessage);
            }
        User::Leave(KErrAccessDenied);
        }

    // Finally before sending pulse-command to plugin check that this client
    // has higher priority than last client requested vibration.

    // Call plugins
    if ( aIntensity == 0 )
        {
        // Stop vibra, if intensity is zero
        StopVibraL(aMessage);
        }
    else
        {
        // There is no timer for real pulse requests, because low level 
        // implementation must stop the vibra after specified duration.
        // This will however result in vibra status not being updated.
        iActiveTimerId = KHWRMVibraNoTimer;
        
        // Complete message if suspended, as there will not be ProcessResponseL
        if ( iSuspended && aMessage.Handle() )
            {
            aMessage.Complete(KErrNone);
            }

        // Call plugin if not suspended
        if ( !iSuspended )
            {
            // Create new data (TransId is updated later, commandId is not important)
            THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(aMessage, 0, 0, EFalse); 
            CleanupStack::PushL( data );
            HWRMVibraCommand::TVibraData vibraData;
            vibraData.iIntensity = aIntensity;
            vibraData.iDuration = aDuration.Int() / 1000;
            HWRMVibraCommand::TPulseVibraCmdDataPackage dataPckg(vibraData);
            data->iTransId = iPluginHandler->ProcessCommandL(HWRMVibraCommand::EPulseVibraCmdId, dataPckg, this);

            // data still needed, do not destroy, just pop
            CleanupStack::Pop( data );
            
            // Add data to list
            iTransactionList->AddTransaction( data );
            
            // Control of overlapping pulses/normal vibration requests is not in hands of HWRM,
            // when pulse length is configured to be handled by vibra driver. That is why
            // client data may not be set to common vibra service.
            
            iStateSetAfterReservation = ETrue;
            }
            
        // Store the latest status in case vibra gets suspended before next plugin call.
        iLastStatus = CHWRMVibra::EVibraStatusOn;
        iLastIntensity = aIntensity;
        }
        
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::PulseVibraL - return" ) );
    
    }


// -----------------------------------------------------------------------------
// CHWRMVibraService::StopVibraL
// Stops vibra. No harm calling this just in case 
// even if vibra is already stopped.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::StopVibraL(const RMessage2& aMessage, 
                                   TBool aForceStop,
                                   TBool aFinalStop)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraService::StopVibraL(%d,%d)" ), aForceStop, aFinalStop );

    __ASSERT_ALWAYS(iPluginHandler, User::Panic(KPanicCategory, EPanicBadHandle));

    TBool forceStop(aForceStop);

    // Stop any ongoing timers
    iVibraTimer->Cancel();   
    
     ReleasePulseData();
    // Reset this service's client data.
    // This way any other (if any) service's interrupted vibration is restarted.
    iCommonService.ResetClientData( this );

       

    // Check from active reserver if it is ok for them that we stop vibra. 
    // This is necessary if active reserver is just reserving vibra but hasn't set vibra state explicitly
    // after reservation. Otherwise it is possible that vibra will be left on forever.
    // Note: This will sometimes result in unnecessary stop calls from suspended sessions when active session has not
    //       explicitly set vibra on/off, but this is the only way to ensure that the necessary stop calls go through.
    CHWRMService* activeReserver = iReservationHandler->GetActiveReserver(KHWRMAllSubResources);
    if ( activeReserver )
        {
        if ( (static_cast<CHWRMVibraService*>(activeReserver)) == this )
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::StopVibraL - This session is active reserver, no need to force stop.") );
            }
        else if ( (static_cast<CHWRMVibraService*>(activeReserver))->OkToStopVibra() )
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::StopVibraL - Active reserver has not used vibra since reservation - Force stop vibra.") );
            forceStop = ETrue;
            }
        else
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::StopVibraL - Active reserver has been using vibra after reservation, no need to force stop.") );
            }
        }
    else
        {
        COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::StopVibraL - No active reserver found, no need to force stop.") );
        }

    if ( forceStop || !iSuspended )
        {
        // Check reservation.
        if ( !forceStop && iReservationHandler->IsReserved(this, KHWRMAllSubResources) )
            {
            User::Leave(KErrInUse);
            }

        // create new data (TransId is updated later)
        THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(aMessage, 0, 0, EFalse); // commandId is not important
        CleanupStack::PushL( data );

        // No data, pass empty descriptor
        TBuf8<1> emptyDes;

        // Do not require callback on final stopping of vibra    
        CHWRMService* callback = NULL;
        if ( !aFinalStop )
            {
            callback = this;
            }

        data->iTransId = iPluginHandler->ProcessCommandL(HWRMVibraCommand::EStopVibraCmdId, emptyDes, callback);
    	
        // data still needed, do not destroy, just pop
        CleanupStack::Pop( data );
        
        // Add data to list
        iTransactionList->AddTransaction( data );
        
        iStateSetAfterReservation = ETrue;
        }
    else
        {
        // complete message if suspended, as there will not be ProcessResponseL
        if ( aMessage.Handle() )
            {
            aMessage.Complete(KErrNone);
            }        
        }
        
    // Store the latest status in case vibra gets suspended before next plugin call.
    iLastStatus = CHWRMVibra::EVibraStatusStopped;
        
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::StopVibraL - return" ) );
    
    }
    

// -----------------------------------------------------------------------------
// CHWRMVibraService::ProcessResponseL
// Handles Vibra requests responses.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::ProcessResponseL( TInt aCommandId,  TUint8 aTransId, TDesC8& aData, TBool aTimeout)
    {
    COMPONENT_TRACE4(_L( "HWRM Server - CHWRMVibraService::ProcessResponseL(0x%x, 0x%x, <data>, 0x%x)" ), aCommandId, aTransId, aTimeout );

    TInt pluginErr(KErrNone);  // Error came from plugin as data (or timeout). Used to complete RMessage.
    TInt contextErr(KErrNone);  // Error in context, i.e. bad handle or descriptor. Used to leave.

    // Unpack the package. All vibra messages contain only possible error code
    // in return package.
    if ( !aTimeout && aData.Size() != sizeof(TInt) )
        {
        COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraService::ProcessResponseL - Data size mismatch, expected: 0x%x, got 0x%x" ), sizeof(TInt), aData.Size() );
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
            HWRMVibraCommand::TErrorCodeResponsePackage errPckg;
            errPckg.Copy(aData);
            pluginErr = errPckg();
            }

        if ( pluginErr != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::ProcessResponseL - Error: %d" ), pluginErr );        
            }

        // Update vibra status PS state value 
        if ( !iPrivilegedClient  && 
            ( ( (iActiveTimerId == KHWRMVibraTimerID) && (iVibraCommonData.VibraState() != CHWRMVibra::EVibraModeON ) ) ||
              ( (iActiveTimerId != KHWRMVibraTimerID) && (iVibraCommonData.VibraFeedbackState() != CHWRMVibra::EVibraFeedbackModeON) ) ) )
        	{
            // This can presumably happen if there was ongoing plugin call when 
            // vibra was turned off.
            iVibraCommonData.PublishVibraStatus(CHWRMVibra::EVibraStatusNotAllowed, iPrivilegedClient);	
            
            // Set vibra timer for immediate shutdown if commandid was not stop
            if ( aCommandId != HWRMVibraCommand::EStopVibraCmdId )
                {
                iVibraTimer->Set(1);
                ReleasePulseData();
                }
            }
        else
            {       
            if ( pluginErr != KErrNone )
                {
                iVibraCommonData.PublishVibraStatus(CHWRMVibra::EVibraStatusUnknown, iPrivilegedClient);	
                }
            else
                {
                switch ( aCommandId )
                    {
                    case HWRMVibraCommand::EStartVibraCmdId:
                        {
                        iVibraCommonData.PublishVibraStatus(CHWRMVibra::EVibraStatusOn, iPrivilegedClient);	    
                        break;
                        }
                    case HWRMVibraCommand::EStartVibraWithDefaultSettingsCmdId:
                        {
                        iVibraCommonData.PublishVibraStatus(CHWRMVibra::EVibraStatusOn, iPrivilegedClient);	    
                        break;
                        }
						case HWRMVibraCommand::EPulseVibraCmdId:
                        {
                        // It may cause quite a lot of overhead to publish vibra status for short pulses.
                        // iVibraCommonData.PublishVibraStatus(CHWRMVibra::EVibraStatusOn, iPrivilegedClient);	    
                        break;
                        }

                    case HWRMVibraCommand::EStopVibraCmdId:
                        {
                        iVibraCommonData.PublishVibraStatus(CHWRMVibra::EVibraStatusStopped, iPrivilegedClient);	    
                        break;
                        }
                    default:
                        {
                        iVibraCommonData.PublishVibraStatus(CHWRMVibra::EVibraStatusUnknown, iPrivilegedClient);	    
                        break;
                        }
                    }
                }
            }
        }
        
    // Complete request and remove delete transaction
    contextErr = CompleteRequest(aTransId, pluginErr);    

    // Leave if there is error in context    
    User::LeaveIfError(contextErr);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::ProcessResponseL - return" ) );
    }

    
// -----------------------------------------------------------------------------
// CHWRMVibraService::GenericTimerFired
// Handles vibra cutoff timer firing. Adjust vibra intensity and stops vibra.
// There is only one regular vibra timer active at a time. It is used for vibra start calls.
// If vibra pulse is requested, new timer is setup. If pulse timer expires and vibra or any other 
// pulse timer is still active, vibration intensity must be adjusted accordingly. If all pulse 
// timers and vibra timer have expired, stop vibra.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::GenericTimerFired(TInt aTimerId, TBool aCutOff)
    {
    COMPONENT_TRACE5( _L( "HWRM Server - CHWRMVibraService::GenericTimerFired, iLastIntensity %d, iActiveTimerId %d, aTimerId %d, aCutOff %d" ),
        iLastIntensity, iActiveTimerId, aTimerId, aCutOff );

    // if cutoff because maximum ontime set the lock period
    if ( aCutOff )
        {
        iVibraCommonData.LockVibra();
        }
        
	RMessage2 dummy;
    TInt err(KErrNone);
    TInt count( iPulseData.Count() );
    TBool changeIntensity( EFalse );
    TBool pulseTimersActive( EFalse );
    TBool succeedingFound( EFalse );
    
    if( aTimerId < count )
        {
        // Update last intensity from expired pulse timer to ACTIVE timer succeeding it,
        // because otherwise it has old information about intensity, which must
        // be set when it expires. This works, because timer objects only need to 
        // remember last intensity. If timer expires, but it is not "active" one, then
        // intensity is not changed and intensity if just passed to next one in queue.
        for( TInt i = aTimerId+1; i < count; i++ )
            {
            if( iPulseData[i]->Timer().IsActive() )
                {
                COMPONENT_TRACE5(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - upgrading last intensity from %d (%d) to %d (%d) "),
                    aTimerId, iPulseData[aTimerId]->LastIntensity(), i, iPulseData[i]->LastIntensity() );
                COMPONENT_TRACE5(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - upgrading timer id from %d (%d) to %d (%d) " ), 
                    aTimerId, iPulseData[aTimerId]->LastTimerId(), i, iPulseData[i]->LastTimerId() );
                iPulseData[i]->SetLastIntensity( iPulseData[aTimerId]->LastIntensity() );
                iPulseData[i]->SetLastTimerId( iPulseData[aTimerId]->LastTimerId() );
                succeedingFound = ETrue;
                break;
                }
            }
        }

    // Check if any pulse timers active
    for( TInt i = 0; i < count; i++ )
        {
        COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - pulse timer %d status: %d" ), i, iPulseData[i]->Timer().IsActive() );
        if( iPulseData[i]->Timer().IsActive() )
            {
            pulseTimersActive = ETrue;
            }
        }

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMVibraService::GenericTimerFired - vibra timer status: %d" ), iVibraTimer->IsActive() );

    // If timer expired is not last active timer, we must not change intensity,
    // because intensity has been changed when new timer setup and intensity
    // gets updated only if it expires or new pulse request arrives.
    if( aTimerId == iActiveTimerId )
        {
        changeIntensity = ETrue;
        
        if( (aTimerId == KHWRMVibraTimerID || !succeedingFound) && pulseTimersActive )
            {
            // If active timer expired was vibra timer or succeeding (=active) pulse timer was not found
            // and there are still active pulse timers, update timer id and intensity accordingly.
            // First pulse timer from end is new active.
            for( TInt i = count-1; i >= 0; i-- )
                {
                if( iPulseData[i]->Timer().IsActive() )
                    {
                    iActiveTimerId = i;
                    iLastIntensity = iPulseData[i]->Intensity();
                    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - upgraded active timer id to %d and intensity to %d" ),
                        iActiveTimerId, iLastIntensity );
                    break;
                    }
                }
            }
        else if( aTimerId != KHWRMVibraTimerID && iVibraTimer->IsActive() && !pulseTimersActive )
            {
            // If active timer expired was not vibra timer and it still active and no more pulse 
            // timers, update timer id and intensity accordingly
            iActiveTimerId = KHWRMVibraTimerID;
            if( aTimerId < count )
                {
                iLastIntensity = iPulseData[aTimerId]->LastIntensity();
                }
            COMPONENT_TRACE3(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - upgraded active timer id to %d and intensity to %d" ), 
                iActiveTimerId, iLastIntensity );
            }
        else
            {
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - active timer id and intensity not updated" ) ); 
            }
        }

    if( !pulseTimersActive )
        {
        // Remove expired pulse timers
        ReleasePulseData();
        }
    
    // If cutoff or there are no active timers left, stop vibra
    if( aCutOff || (!pulseTimersActive && !iVibraTimer->IsActive()) )
        {
        TRAP(err, StopVibraL(dummy));
    
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - Error Stopping vibra: %d" ), err );
            }
        }
    else if( changeIntensity )
        {
        TRAP(err, ChangeVibraIntensityL());
    
        if ( err != KErrNone )
            {
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - Error changing intensity: %d" ), err );
            }
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::GenericTimerFired - return" ) );
    
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraService::ChangeVibraIntensityL
// Changes the device vibration intensity to previous active intensity.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::ChangeVibraIntensityL()
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMVibraService::ChangeVibraIntensityL() - new intensity %d" ), iLastIntensity );

	// Ignore Coverity false positive as uninitialized variable.
	// Since no response is sent back to the client, only a empty message is created here. 
	// Message handle of this will be initialized to NULL in the default constructor.
	// coverity[var_decl]
	RMessage2 dummy;
    
    // Create new data (TransId is updated later, commandId is not important)
    THWRMPluginRequestData* data = new (ELeave) THWRMPluginRequestData(dummy, 0, 0, EFalse); 
    CleanupStack::PushL( data );

    if ( iLastIntensity == KUseDefaultIntensity )
        {
        // No data, pass empty descriptor
        TBuf8<1> emptyDes;
        data->iTransId = iPluginHandler->ProcessCommandL(HWRMVibraCommand::EStartVibraWithDefaultSettingsCmdId, emptyDes, this);
        }
    else 
        {
        HWRMVibraCommand::TStartVibraCmdDataPackage dataPckg(iLastIntensity);    
        data->iTransId = iPluginHandler->ProcessCommandL(HWRMVibraCommand::EStartVibraCmdId, dataPckg, this);    
        }

    // data still needed, do not destroy, just pop
    CleanupStack::Pop( data );

    // Add data to list
    iTransactionList->AddTransaction( data );
    }
    

    
// -----------------------------------------------------------------------------
// CHWRMVibraService::SuspendSubResource
// Suspends vibra. Since vibra has no subresources, whole vibra is affected.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::SuspendSubResource(TInt /*aSubResource*/)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::SuspendSubResource()" ) );
    
    if ( iLastStatus == CHWRMVibra::EVibraStatusOn )
        {
        // Resume base state, i.e. stop vibra
        RMessage2 dummy;
        TRAP_IGNORE(StopVibraL(dummy));
            
        // Restore iLastStatus
        iLastStatus = CHWRMVibra::EVibraStatusOn;
        }
    
    iSuspended = ETrue;
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::SuspendSubResource - return" ) );
    }
    
// -----------------------------------------------------------------------------
// CHWRMVibraService::ResumeSubResource
// Resumes vibra. Since vibra has no subresources, whole vibra is affected.
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::ResumeSubResource(TInt /*aSubResource*/)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::ResumeSubResource()" ) );
    
    iSuspended = EFalse;
    
    RMessage2 dummy;
    
    // Restore last state
    TInt err(KErrNone);
    if ( iLastStatus == CHWRMVibra::EVibraStatusOn )
        {
        TRAP(err, StartVibraL(0, iLastIntensity, dummy, EFalse));
        }
    else
        {
        TRAP(err, StopVibraL(dummy));
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMVibraService::ResumeSubResource - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMVibraService::ActivateSubResource
// Vibra doesn't support activation/deactivation based on device state
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::ActivateSubResource(TInt /*aSubResource*/, TBool /*aActivate*/)
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CHWRMVibraService::OkToStopVibra
// Checks if it is ok to stop vibra from another session.
// -----------------------------------------------------------------------------
//
TBool CHWRMVibraService::OkToStopVibra()
    {
    return !iStateSetAfterReservation;
    }


// -----------------------------------------------------------------------------
// CPulseData::CPulseData
// -----------------------------------------------------------------------------
//


CHWRMVibraService::CPulseData::CPulseData( CHWRMGenericTimer& aVibraTimer,
                                           const TInt aLastIntensity, 
                                           const TInt aIntensity,
                                           const TInt aLastTimerId )
    :iVibraTimer(aVibraTimer),
    iLastIntensity(aLastIntensity),
    iIntensity(aIntensity),
    iLastTimerId(aLastTimerId)
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CPulseData::CPulseData()" ) );
    }



// -----------------------------------------------------------------------------
// CPulseData::NewL
// -----------------------------------------------------------------------------
//
CHWRMVibraService::CPulseData* 
CHWRMVibraService::CPulseData::NewL( CHWRMGenericTimer& aVibraTimer,
                              const TInt aLastIntensity, 
                              const TInt aIntensity,
                              const TInt aLastTimerId )
    {
    COMPONENT_TRACE4(_L( "HWRM Server - CPulseData::NewL(0x%x, %d, %d)" ), &aVibraTimer, aLastIntensity, aIntensity);

    CPulseData* self = new( ELeave ) 
        CPulseData(aVibraTimer, aLastIntensity, aIntensity, aLastTimerId);
    
    COMPONENT_TRACE2(_L( "HWRM Server - CPulseData::NewL - return 0x%x" ), self);
    
    return self;
    }

// -----------------------------------------------------------------------------
// CPulseData::~CPulseData
// -----------------------------------------------------------------------------
//
CHWRMVibraService::CPulseData::~CPulseData()
    {
    COMPONENT_TRACE1(_L("HWRM Server - CPulseData::~CPulseData()"));

    iVibraTimer.Cancel();
    }
        
// -----------------------------------------------------------------------------
// CPulseData::Timer
// -----------------------------------------------------------------------------
//
CHWRMGenericTimer& CHWRMVibraService::CPulseData::Timer() const
    {
    return iVibraTimer;
    }
            

// -----------------------------------------------------------------------------
// CPulseData::LastIntensity
// -----------------------------------------------------------------------------
//
TInt CHWRMVibraService::CPulseData::LastIntensity() const
    {
    return iLastIntensity;
    }
            
// -----------------------------------------------------------------------------
// CPulseData::Intensity
// -----------------------------------------------------------------------------
//
TInt CHWRMVibraService::CPulseData::Intensity() const
    {
    return iIntensity;
    }

// -----------------------------------------------------------------------------
// CPulseData::SetLastIntensity
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::CPulseData::SetLastIntensity(TInt aIntensity)
    {
    iLastIntensity = aIntensity;
    }

// -----------------------------------------------------------------------------
// CPulseData::LastTimerId
// -----------------------------------------------------------------------------
//
TInt CHWRMVibraService::CPulseData::LastTimerId() const
    {
    return iLastTimerId;
    }

// -----------------------------------------------------------------------------
// CPulseData::SetLastTimerId
// -----------------------------------------------------------------------------
//
void CHWRMVibraService::CPulseData::SetLastTimerId(TInt aTimerId)
    {
    iLastTimerId = aTimerId;
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
