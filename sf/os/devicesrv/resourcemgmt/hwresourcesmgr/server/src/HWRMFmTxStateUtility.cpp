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
#include "HWRMFmTxStateUtility.h"
#include "HWRMPluginHandler.h"
#include "HWRMReservationHandler.h"
#include "HWRMFmTxInternalCRKeys.h"
#include "HWRMFmTxInternalPSKeys.h"
#include "HWRMFmTxAudioPolicyDomainPSKeys.h"
#include "HWRMFmTxService.h"
#include "HWRMFmTxCommonData.h"
#include "HWRMFmTxRdsTextConverter.h"
#include "HWRMClientServer.h"
#include "HWRMConfiguration.h"
#include "HWRMFmTxCommands.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmTxStateUtility::CHWRMFmTxStateUtility
// C++ constructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxStateUtility::CHWRMFmTxStateUtility() : iPreviousState(EFmTxStateUnknown), iEnableStandby(EFalse)
    {
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxStateUtility::CHWRMFmTxStateUtility()" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStateUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxStateUtility::ConstructL(CHWRMPluginHandler* aPluginHandler,
                                       CHWRMPluginHandler* aWatcherPluginHandler,
                                       CHWRMReservationHandler* aReservationHandler,
                                       CHWRMFmTxCommonData*     aFmTxCommonData)
    {    
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxStateUtility::ConstructL()" ) ) );

    iFmTxCommonData = aFmTxCommonData;

    iFmTxStateObserver   = CHWRMPSChangeObserver::NewL(*this, KPSUidHWRMFmTx, KHWRMFmTxStatus);
    iAudioRoutedObserver = CHWRMPSChangeObserver::NewL(*this, KPSUidHWRMFmTx, KHWRMFmTxAudioRoutedFlag);

    iFmTxService = CHWRMFmTxService::NewL( aPluginHandler, *aWatcherPluginHandler,
        aReservationHandler, *aFmTxCommonData, *(aFmTxCommonData->GetRdsTextConverter()) );

    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxStateUtility::ConstructL - return " ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStateUtility::CancelPowerSaveTimer
// Stops and deletes power save timer.
// -----------------------------------------------------------------------------
//    
void CHWRMFmTxStateUtility::CancelPowerSaveTimer()
    {    
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxStateUtility::CancelPowerSaveTimer()" ) ) );             		
    
    if( iPowerSaveTimer )
        {
        iPowerSaveTimer->Stop();
    	delete iPowerSaveTimer;
        iPowerSaveTimer = NULL;
        }
  	      
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxStateUtility::CancelPowerSaveTimer - return" ) ) );        
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStateUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMFmTxStateUtility* CHWRMFmTxStateUtility::NewL(CHWRMPluginHandler*      aPluginHandler,
                                                   CHWRMPluginHandler*      aWatcherPluginHandler,
                                                   CHWRMReservationHandler* aReservationHandler,
                                                   CHWRMFmTxCommonData*     aFmTxCommonData)
    {
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxStateUtility::NewL()" ) ) );

    CHWRMFmTxStateUtility* self = new( ELeave ) CHWRMFmTxStateUtility();
    
    CleanupStack::PushL( self );
    self->ConstructL(aPluginHandler, aWatcherPluginHandler, aReservationHandler, aFmTxCommonData);
    CleanupStack::Pop( self );

    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxStateUtility::NewL - return 0x%x" ), self );
    
    return self;
    }
   
// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CHWRMFmTxStateUtility::~CHWRMFmTxStateUtility()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxStateUtility::~CHWRMFmTxStateUtility()" ) );

    delete iFmTxService;
    delete iFmTxStateObserver;
    delete iPowerSaveObserver;
    delete iAudioRoutedObserver;
    
    CancelPowerSaveTimer();

    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxStateUtility::~CHWRMFmTxStateUtility - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStateUtility::PSValueChanged
// Act on observed PS value changes
// -----------------------------------------------------------------------------
//
void CHWRMFmTxStateUtility::PSValueChanged(const TUid& aUid, TUint32 aKey, TInt aValue)
    {
    COMPONENT_TRACE4(_L("HWRM Server - CHWRMFmTxStateUtility::PSValueChanged(0x%x, 0x%x, 0x%x)"), aUid, aKey, aValue); 

    if ( aUid == KPSUidHWRMFmTx && aKey == KHWRMFmTxStatus )
        {           
        COMPONENT_TRACE3(_L("HWRM Server - CHWRMFmTxStateUtility::PSValueChanged new state = %d, previous = %d"), aValue, iPreviousState);           
            
        TFmTxState currentState = static_cast<TFmTxState>(aValue);
            
        // Check if the hardware has switched on
        if ( iFmTxCommonData->IsFmTxHardwareOn(currentState) && 
             !(iFmTxCommonData->IsFmTxHardwareOn(iPreviousState)) )
            {
            // FM Tx disable event may occur during an enable request
            if ( iFmTxCommonData->IsPowerSaveOn() )
                {
                COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxStateUtility::PSValueChanged - powering down again")) );           
                iFmTxService->ExecutePluginCommand(HWRMFmTxCommand::ETxOffCmdId);
                }               
            else // Now set RDS defaults
                {
                COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxStateUtility::PSValueChanged - setting PS")) );            
                HWRMFmTxCommand::TRdsPsPackage pckg( iFmTxCommonData->ConvertedRdsPs() ); // package up the request         
                iFmTxService->ExecutePluginCommand(HWRMFmTxCommand::ESetTxRdsPsCmdId, EFalse/*not split*/, pckg);
                if( iFmTxCommonData->IsAudioRouted() || iPreviousState == EFmTxStatePowerSaveInactivity )
                    {
                    iEnableStandby = ETrue;
                    }
                else
                    {
                    iEnableStandby = EFalse;
                    }
                }
            }
		else if ( iFmTxCommonData->IsFmTxHardwareOn(currentState) &&
			      !iFmTxCommonData->IsAudioRouted() )
			{
			iEnableStandby = ETrue;
			}
            
        // Cache state for next time
        iPreviousState = currentState;
        
        // Stop power save timer if Fm Tx state is set off.
        if( currentState == EFmTxStateOff )
            {
            CancelPowerSaveTimer();
            }
        }
        
    else if ( aUid == KPSUidHWRMFmTx && aKey == KHWRMFmTxAudioRoutedFlag )
        {       
        // Update FM Tx status
        iFmTxCommonData->UpdateAudioStatus( aValue );                   
        if( aValue && iFmTxCommonData->Status() == EFmTxStatePowerSaveInactivity )
            {
            COMPONENT_TRACE1( _L("HWRM Server - CHWRMFmTxStateUtility::PSValueChanged - powering up from standby") );
            HWRMFmTxCommand::TSetFrequencyPackage pckg( iFmTxCommonData->Frequency() );
            iFmTxService->ExecutePluginCommand(HWRMFmTxCommand::ETxOnCmdId, EFalse/*not split*/, pckg);
            }
        }
    // Check if KHWRFmTxAudioRoutedFlag is set off and FM TX is still enabled        
    if( !iFmTxCommonData->IsAudioRouted()
        && iFmTxCommonData->Status() == EFmTxStateInactive
        && !iPowerSaveTimer )
        {
        TRAPD( err, iPowerSaveTimer = CHWRMFmTxPowerSaveTimer::NewL( *this ) );
        if( err != KErrNone )
            {
            COMPONENT_TRACE2(_L("HWRM Server - CHWRMFmTxStateUtility::PSValueChanged - Couldn't create power save timer cause %d"), err);
            }
        else
            {
            iPowerSaveTimer->Start(iFmTxCommonData->PowerSaveTimeout());
            }
        }
    else if( iFmTxCommonData->IsAudioRouted() )
        {        
        CancelPowerSaveTimer();
		iEnableStandby = EFalse;
        }
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxStateUtility::PSValueChanged - return") ) ); 
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStateUtility::ProcessIndicationL
// Handles status change indications from HWRM plugin
// -----------------------------------------------------------------------------
//
void CHWRMFmTxStateUtility::ProcessIndicationL( const TUint32 aId, TDesC8&  aData )
    {
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxStateUtility::ProcessIndicationL, aId = %d" ), aId );        

    switch ( aId )
        {
        case HWRMFmTxCommand::ETxWatcherConnIndId:
            {
            HWRMFmTxCommand::TConnStatusPackage pckg;
            pckg.Copy( aData );
            HWRMFmTxCommand::TConnStatusData data( pckg() );
            
            if( data.iConnected )
                {
                // Update common data power save state if necessary
                if ( iFmTxCommonData->IsFmTxHardwareOn() ||
					 iFmTxCommonData->IsPowerSaveOn() )
                    {
                    // Update common data power save state if necessary
                    iFmTxCommonData->UpdatePowerSaveState(
                                            CHWRMFmTxCommonData::EHWRMFmTxPowerSaveAccessory );
                    }
                }
            else
                {
                // Reset power save accessory state, if necessary
                if ( iFmTxCommonData->IsAccessoryPowerSaveOn() )
                    {
                    iFmTxCommonData->UpdatePowerSaveState( 
                                                CHWRMFmTxCommonData::EHWRMFmTxPowerSaveOff );
                    }
                }
			
			// Switch off transmitter. As specified in client interface state
			// transition from EFmTxStatePowerSaveAccessory to EFmTxStateOff must occur.
			iFmTxService->ExecutePluginCommand(HWRMFmTxCommand::ETxOffCmdId);
			// Don't change iFmTxState until power down completes
            }
            break;

        default:
            // do nothing
            break;
        }

    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxStateUtility::ProcessIndicationL - return" ) ) );        
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxStateUtility::TimerExpired
// This method is called by Power Save Timer, after timer expires.
// -----------------------------------------------------------------------------
//    
void CHWRMFmTxStateUtility::TimerExpired()
    {    
    COMPONENT_TRACE1( (_L("HWRM Server - CHWRMFmTxStateUtility::TimerExpired()" ) ) );             		
           
    delete iPowerSaveTimer;
    iPowerSaveTimer = NULL;
    // Power down the transmitter, if necessary
    if ( iFmTxCommonData->IsFmTxHardwareOn() && !iFmTxCommonData->IsAudioRouted() )
        {
        if( iEnableStandby )
            {
            iFmTxCommonData->UpdatePowerSaveState( CHWRMFmTxCommonData::EHWRMFmTxPowerSaveInactivity );
			iEnableStandby = EFalse;
            }
        else
            {
            iFmTxCommonData->UpdatePowerSaveState( CHWRMFmTxCommonData::EHWRMFmTxPowerSaveOff );
            }
        iFmTxService->ExecutePluginCommand(HWRMFmTxCommand::ETxOffCmdId);
        // Don't change iFmTxState until power down completes
        }    	      
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxStateUtility::TimerExpired - return" ) ) );        
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
