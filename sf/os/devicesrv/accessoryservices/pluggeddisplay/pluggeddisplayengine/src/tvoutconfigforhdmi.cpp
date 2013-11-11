/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * CPDConfigurationForHDMI class implementation.
 *
 */


// INCLUDE FILES
#include <tvoutconfig.h>
#include <e32base.h>
#include "pdeconstants.h"
#include "tvoutconfigforhdmi.h"
#include "multifinitestatemachine.h"
#include "hdmicablestatusfsm.h"
#include "myasynconeshottimer.h"
#include "trace.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS
#define SEND_VIA_PDENGINE(event) \
                        ((CHDMICableStatusFSM*)&iFSM)->GetPDEngine()->Input( \
                        EPDEFSMIdHDMICableStatus, \
                        EPDEIfTVOutConfig, \
                        event )

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// CTVOutConfigListenerForHDMI::NewL
//
// ----------------------------------------------------------------------------
//
CTVOutConfigForHDMI* CTVOutConfigForHDMI::NewL(
        MFSMForBody& aFSM )
    {
    FUNC_LOG;
    CTVOutConfigForHDMI* self = new(ELeave) CTVOutConfigForHDMI( aFSM );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self);

    return self;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::CTVOutConfigForHDMI
//
// ----------------------------------------------------------------------------
//
CTVOutConfigForHDMI::CTVOutConfigForHDMI(
        MFSMForBody& aFSM )
    : CActive( CActive::EPriorityLow ),
    iTvConfig ( NULL ),
    iFSM( aFSM )
    {
    FUNC_LOG;
    // Nothing
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::ListenHDMICableStatusIfNoMissedEvents
//
// ----------------------------------------------------------------------------
//
TFSMEventId CTVOutConfigForHDMI::ListenHDMICableStatusIfNoMissedEvents()
    {
    FUNC_LOG;
    TFSMEventId retEvent( EIfTVOutConfigEventUndefined );
    TFSMState currentStateId = iFSM.CurrentStateID();

    if  ( ( EHDMICableStateIdle == currentStateId ) && ( iTvConfig->HdmiCableConnected() ) )
        {
        INFO("Cable connected while disconnect actions were going on in ASY or is already connected when ASY starts.");
        retEvent = EIfTVOutConfigEventCableConnected;
        }
    else if ( ( EHDMICableStateConnected == currentStateId ) && ( !iTvConfig->HdmiCableConnected() ) )
        {
        INFO("Cable disconnected while connect actions were going on in ASY.");
        retEvent = EIfTVOutConfigEventCableDisconnected;
        }
    else if ( !IsActive() )
        {
        iStatus = KRequestPending;
        iNextOpt = EOptStartListenHdmiCable;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );                  
        }
    INFO_1("Returning event id: %d", retEvent);
    return retEvent;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::SetAvailableTvConfigList
//
// ----------------------------------------------------------------------------
//
TInt CTVOutConfigForHDMI::SetAvailableTvConfigList(
        const RArray<TTvSettings>& aAnalogConfigs, 
        const RArray<THdmiDviTimings>& aHdmiConfigs )
    {
    FUNC_LOG;
    TInt retVal( KErrNotFound );
    if ( iTvConfig )
        {
        retVal = iTvConfig->SetAvailableTvConfigList(
                aAnalogConfigs,
                aHdmiConfigs );
        }
    
    return retVal;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::Enable
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::Enable()
    {
    FUNC_LOG;
    Cancel();
    iStatus = KRequestPending;
    iNextOpt = EOptEnableTv;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );        
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::Disable
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::Disable()
    {
    FUNC_LOG;
    Cancel();
    iStatus = KRequestPending;
    iNextOpt = EOptDisableTv;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );        
    }


// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::Disable
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::ListenForDisconnection()
    {
    Cancel();
    iNextOpt = EOptStartListenForDisconnection;
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );        
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::GetTvOutConfig
//
// ----------------------------------------------------------------------------
//
CTvOutConfig* CTVOutConfigForHDMI::GetTvOutConfig()
    {
    FUNC_LOG;
    return iTvConfig;
    }


// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::ListenCopyProtectionIfNoMissedEvents
//
// ----------------------------------------------------------------------------
//
TCopyProtctListenRetValues CTVOutConfigForHDMI::ListenCopyProtectionIfNoMissedEvents()
    {
    FUNC_LOG;
    TCopyProtctListenRetValues retStruct;
    retStruct.iFSMEventId = EIfTVOutConfigEventUndefined;
    retStruct.iError = KErrNone;
    TBool currentCopyProtectStatus = iTvConfig->CopyProtectionStatus();

    if  ( iLastRecordedCopyProtectEnabledStatus != currentCopyProtectStatus )
        {
        INFO("Copy Protection status changed while not listening.");
        iLastRecordedCopyProtectEnabledStatus = currentCopyProtectStatus;
        retStruct.iFSMEventId = EIfTVOutConfigEventSetCopyProtectStatusChanged;
        }
    else if ( !IsActive() )
        {
        iLastRecordedCopyProtectEnabledStatus = currentCopyProtectStatus;
        iStatus = KRequestPending;
        TInt err = iTvConfig->CopyProtectionStatusListener( iStatus );
        if ( KErrNone == err )
            {
            SetActive();
            iNextOpt = EOptListenCopyProtection;
            }
        else
            {
            iStatus = KErrNone;
            INFO_1("Error status %d", err);
            INFO("Copy Protection Status listening failed.");
            retStruct.iError = err;
            retStruct.iFSMEventId = EIfTVOutConfigEventCopyProtectionListenFailed;            
            }
        }
    return retStruct;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::GetLatestRecordedCopyProtectionStatus
//
// ----------------------------------------------------------------------------
//
TBool CTVOutConfigForHDMI::GetLatestRecordedCopyProtectionStatus() const
    {
    FUNC_LOG;
    return iLastRecordedCopyProtectEnabledStatus;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::ResetLatestRecordedCopyProtectionStatus
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::ResetLatestRecordedCopyProtectionStatus()
    {
    FUNC_LOG;
    iLastRecordedCopyProtectEnabledStatus = EFalse;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::ListenSettingsChanges
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::ListenSettingsChanges()
    {
    FUNC_LOG;
    Cancel();    
    iNextOpt = EOptStartListenForSettingsChanges;
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );        
    
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::GetSupportedHdmiModes
//
// ----------------------------------------------------------------------------
//
TInt CTVOutConfigForHDMI::GetSupportedHdmiModes( RArray<TSupportedHdmiDviMode>& asupportedModes )
    {
    FUNC_LOG;

	TUint count;
	TInt  retVal( KErrNone );
	
	retVal = iTvConfig->GetNumberOfHdmiModes( count );
	ERROR( retVal, "Getting supported hdmi modes failed" );

	if( KErrNone == retVal )
		{
		for( TInt i = 0; (i < count); i++)
			{
			TSupportedHdmiDviMode mode;			
			iTvConfig->GetSupportedHdmiMode( i, mode );
			TInt err = asupportedModes.Append( mode );
			if(KErrNone != err)
			    {
			    ERROR( err, "Failed to append supported modes in array" );
			    retVal = err;
			    break;
			    }
			}
		}

	return retVal;
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::ConstructL
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::ConstructL()
    {
    FUNC_LOG;
     // Tv out config
    iTvConfig = CTvOutConfig::NewL();
    iMyAsyncOneShotTimer = new (ELeave) CMyAsyncOneShotTimer( CActive::EPriorityLow, *this );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::~CTVOutConfigForHDMI
//
// ----------------------------------------------------------------------------
//
CTVOutConfigForHDMI::~CTVOutConfigForHDMI()
    {
    FUNC_LOG;
    if ( iTvConfig )
        {
        Cancel();
        delete iTvConfig;        
        }
    if ( iMyAsyncOneShotTimer )
        {
        if ( iMyAsyncOneShotTimer->IsActive() )
            {
            iMyAsyncOneShotTimer->Cancel();      
            }
        delete iMyAsyncOneShotTimer;
        }
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::DoCancel
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::DoCancel()
    {
    FUNC_LOG;
    TInt err( KErrNone );
    iLastRecordedCopyProtectEnabledStatus = EFalse;
    if (    ( EOptListenHdmiCable == iNextOpt )||
            ( EOptListenForDisconnection == iNextOpt ) )
        {
        err = iTvConfig->HdmiCableListenerCancel();            
        }
    else if ( EOptListenCopyProtection == iNextOpt )
        {
        err = iTvConfig->CopyProtectionStatusCancel();            
        }
    else if ( EOptListenForSettingsChanges == iNextOpt )
        {
        err = iTvConfig->SettingsChangedListenerCancel();
        }
    if ( iMyAsyncOneShotTimer )
        {
        iMyAsyncOneShotTimer->Cancel();
        }
    iNextOpt = EOptIdle;
    if ( KErrNone != err )
        {
        INFO_1("Error status %d", err);
        }
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::RunL
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::RunL()
    {
    FUNC_LOG;

    switch ( iNextOpt )
        {
        case EOptStartListenHdmiCable:
            {
            INFO("EOptStartListenHdmiCable");
            iStatus = KRequestPending;
            TInt err = iTvConfig->HdmiCableListener( iStatus );
            INFO_1("Error status %d", err);
            if(err == KErrNone)
                {
                SetActive();
                iNextOpt = EOptListenHdmiCable;
                }
            else
                {
                iStatus = KErrNone;
                User::LeaveIfError( err );
                }       
            break;
            }
        case EOptListenHdmiCable:
            {
            INFO("EOptListenHdmiCable");
            if ( KErrNone != iStatus.Int())
                {
                if ( EHDMICableStateIdle == iFSM.CurrentStateID() )
                    {
                    // HDMI Cable FSM may not fail in idle state.
                    // Try to listen after a while, and do this again (possibly) forever
                    iTimeOutReason = EOptStartListenHdmiCable;
                    iNextOpt = EOptIdle;
                    iMyAsyncOneShotTimer->Call();
                    }
                else
                    {
                    iFSM.Input( 
                            EPDEIfTVOutConfig, 
                            EIfTVOutConfigEventCableStatusListenFailed );
                    }
                }
            else if ( iTvConfig->HdmiCableConnected() )
                {
                    // Send connected event to FSM via PDEngine where the multiple connection(analog and hdmi) logic is handled.
                    SEND_VIA_PDENGINE(EIfTVOutConfigEventCableConnected);
                }
            else
                {
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventCableDisconnected );
                }
            break;
            }
        case EOptListenCopyProtection:
            {
            INFO("EOptListenCopyProtection");
            iNextOpt = EOptIdle;
            if ( KErrNone != iStatus.Int())
                {
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventCopyProtectionListenFailed );                
                }
            else
                {
                iLastRecordedCopyProtectEnabledStatus = iTvConfig->CopyProtectionStatus();
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSetCopyProtectStatusChanged );                
                }
            break;
            }
         case EOptEnableTv:
            {
            INFO("EOptEnableTv");
            TInt retVal = iTvConfig->Enable();
            INFO_1("iTvConfig->Enable() retVal: %d", retVal);
            switch ( retVal )
                {
                case KErrNone:
                case KErrAlreadyExists:    
                    {
                    iFSM.Input(
                            EPDEIfTVOutConfig,
                            EIfTVOutConfigEventEnabled );                                             
                    }
                    break;
                case KErrServerBusy:
                    {
                    iTimeOutReason = iNextOpt;
                    iNextOpt = EOptIdle;
                    iMyAsyncOneShotTimer->Call();
                    }
                    break;
                case KErrNotSupported:
                case KErrNotReady:
                default:
                    {
                        iFSM.Input(
                                EPDEIfTVOutConfig,
                                EIfTVOutConfigEventEnableFailed );                                             
                    }
                    break;
                }            
            break;
            }
        case EOptDisableTv:
            {
            INFO("EOptDisableTv");
            TInt retVal = iTvConfig->Disable();
            INFO_1("iTvConfig->Disable() retVal: %d", retVal);
            switch ( retVal )
                {
                case KErrNone:
                case KErrAlreadyExists:    
                    {
                    iFSM.Input(
                            EPDEIfTVOutConfig,
                            EIfTVOutConfigEventDisabled );                                             
                    }
                    break;
                case KErrServerBusy:
                    {
                    iTimeOutReason = iNextOpt;
                    iNextOpt = EOptIdle;
                    iMyAsyncOneShotTimer->Call();
                    }
                    break;
                case KErrNotSupported:
                case KErrNotReady:
                default:
                    {
                    iFSM.Input(
                            EPDEIfTVOutConfig,
                            EIfTVOutConfigEventDisableFailed );                                             
                    }
                    break;
                }            
            break;
            }
        case EOptStartListenForDisconnection:
            {
            INFO("EOptStartListenForDisconnection");
            iStatus = KRequestPending;
            TInt err = iTvConfig->HdmiCableListener( iStatus );
            if(err == KErrNone)
                {
                SetActive();
                if(iTvConfig->HdmiCableConnected())
                    {
                    iNextOpt = EOptListenForDisconnection;
                    }
                else
                    {
                    Cancel();
                    iFSM.Input( 
                            EPDEIfTVOutConfig, 
                            EIfTVOutConfigEventCableDisconnected );                    
                    }
                }
            else
                {
                iStatus = KErrNone;
                User::LeaveIfError( err );
                }
            break;
            }
        case EOptListenForDisconnection:
            {
            INFO("EOptListenForDisconnection");
            TInt err( iStatus.Int() );
            if ( KErrNone != err )
                {
                INFO_1("Error status %d", err);
                iFSM.Input(
                            EPDEIfTVOutConfig,
                            EIfTVOutConfigEventCableStatusListenFailed );
                }
            else if ( iTvConfig->HdmiCableConnected() )
                {
                // This should not come at all, because waiting for the first occurred
                // disconnection since entering this state.
                ListenForDisconnection();
                }
            else
                {
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventCableDisconnected );
                }
            break;
            }
        case EOptStartListenForSettingsChanges:
            {
            INFO("EOptStartListenForSettingsChanges");
            iStatus = KRequestPending;
            TInt err = iTvConfig->SettingsChangedListener( iStatus );
            if ( KErrNone != err )
                {
                INFO_1("iTvConfig->SettingsChangedListener( iStatus ) error status %i", err);
                iStatus = KErrNone;
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSettingsChangesListenFailed );
                }
            else
                {
                SetActive();
                iNextOpt = EOptListenForSettingsChanges;
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSettingsChangesListenStarted );
                }
            break;
            }
        case EOptListenForSettingsChanges:
            {
            INFO("EOptListenForSettingsChanges");
            iNextOpt = EOptIdle;
            TInt err( iStatus.Int() );
            if ( KErrNone != err )
                {
                INFO_1("Error status %d", err);
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSettingsChangesListenFailed );
                }
            else
                {
                iFSM.Input(
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventSettingsChanged );
                }
            break;
            }
        default:
            INFO_1("Default case, iNextOpt = %i", iNextOpt);
            break;
        }
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::RunError
//
// ----------------------------------------------------------------------------
//
TInt CTVOutConfigForHDMI::RunError( TInt /*aError*/ )
    {
    FUNC_LOG;

    switch ( iNextOpt )
        {
        case EOptStartListenHdmiCable:
        case EOptStartListenForDisconnection:
            {
            iFSM.Input( 
                        EPDEIfTVOutConfig,
                        EIfTVOutConfigEventCableStatusListenFailed );
            }
            break;
          
        default:
            break;
        }
    
    return KErrNone; 
    }

// ----------------------------------------------------------------------------
// CTVOutConfigForHDMI::OneShotTimeout
//
// ----------------------------------------------------------------------------
//
void CTVOutConfigForHDMI::OneShotTimeout()
    {
    iNextOpt = iTimeOutReason;
    iTimeOutReason = EOptIdle;
    RunL();
    }
