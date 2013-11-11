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
* Description:  Implementation of "Connected" state of HDMI Cable Status FSM.
*
*/



#include <e32def.h>
#include <e32cmn.h>


#include "pdeconstants.h"
#include "accessorycontrolif.h"
#include "hdmicablestatusfsm.h"
#include "hdmicablestateconnected.h"
#include "tvoutconfigforhdmi.h"
#include "edidhandler.h"
#include "trace.h"
#include <centralrepository.h>
#include <accessoriescrkeys.h>
#include <tvoutconfig.h>




// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CHDMICableStateConnected* CHDMICableStateConnected::NewL( 
        CTVOutConfigForHDMI& aTVOutConfigForCableConnect,
        CEDIDHandler& aEDIDHandler,
        CAccessoryControlIf& aAccessoryControlIf,
        CHDMICableStatusFSM&  aHDMICableStatusFSM )
    {
    FUNC_LOG;
    CHDMICableStateConnected* self = new ( ELeave ) CHDMICableStateConnected(            
            aTVOutConfigForCableConnect,
            aEDIDHandler, aAccessoryControlIf,
            aHDMICableStatusFSM );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHDMICableStateConnected::~CHDMICableStateConnected()
    {
    FUNC_LOG;
    if ( iTVOutConfigForCopyProtect )
        {
        iTVOutConfigForCopyProtect->Cancel();        
        delete iTVOutConfigForCopyProtect;
        }
    if ( iTVOutConfigForSettingChanges )
        {
        iTVOutConfigForSettingChanges->Cancel();
        delete iTVOutConfigForSettingChanges;
        }
    if ( iCRWatchForVOverScan )
        {
        iCRWatchForVOverScan->Cancel();
        delete iCRWatchForVOverScan;        
        }
    }

// ---------------------------------------------------------------------------
// From MCentralRepositoryObserver.
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::CentRepKeyChanged(
        TUid    aRepositoryId,
        TUint32 aId )
    {
    FUNC_LOG;
    if ( KCRUidTvoutSettings == aRepositoryId )
        {
        if ( KSettingsTvoutVerticalOverscan == aId  )
            {            
			INFO("Cenrep Value Changed");
			if( iEDIDHandler.UpdateOverscanValues() )
				{
				INFO( "There is a real change" );
            	Input( EPDEIfCentralRepositoryWatch, EPDEIfCentralRepositoryWatchEventKeyChanged );
				}
            }
        else
            {
            INFO_1("Unexpected CR key ID, aId 0x%x", aId );            
            }       
        }
    else
        {
        INFO_1("Unexpected Central Repository ID, aRepositoryId 0x%x", aRepositoryId);
        }

    }

// ---------------------------------------------------------------------------
// From MCentralRepositoryObserver.
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::CentRepGetKeyValueFailed(
        TUid    aRepositoryId,
        TUint32 aId,
        TInt    aError )
    {
    FUNC_LOG;
    // Make use of aError argument top get rid of compiler warning.
    TInt error( aError );
    if ( ( KCRUidTvoutSettings == aRepositoryId ) && ( KErrNone != error ) )
        {
        if ( KSettingsTvoutVerticalOverscan == aId  )
            {
            INFO_1(" Getting KSettingsTvoutVerticalOverscan from CR failed, aError %d", error );                                    
            }
        else
            {
            INFO_1("Unexpected CR key ID, aId 0x%x", aId );            
            }       
        }
    else
        {
        INFO_1("Unexpected Central Repository ID, aRepositoryId 0x%x", aRepositoryId);
        }    
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::Enter( )
    {
    FUNC_LOG;
    TInt retVal( KErrNone );
	
	// Get the current overscan value
	iEDIDHandler.UpdateOverscanValues();
	iEDIDHandler.GetCurrentOverscanValue( iHOverscanValue, iVOverscanValue );
	
    iCopyProtectListenFailCounter.iCount = 0;
    iTVOutConfigForCopyProtect->ResetLatestRecordedCopyProtectionStatus();
    retVal = iEDIDHandler.FetchEDIDData();
    if ( KErrNone != retVal )
        {
        INFO( "Going to state <Rejected> since iEDIDHandler.FetchEDIDData() failed!" );        
        iHDMICableStatusFSM.Transit( EHDMICableStateRejected );         
        }
    else
        {
        iSubState = ESubStateFechEdidData;
        }
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::Input(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( iSubState )
    {
    case ESubStateFechEdidData:
        {
        SubStateFetchEdidDataInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateStartListenSettingsChanges:
        {
        CHDMICableStateConnected::SubStateStartListenConfigChangesInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateEnable:
        {
        SubStateEnableInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateResettingEnable:
        {
        SubStateResettingEnableInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateIdlingDisable:
        {
        SubStateIdlingDisableInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateResettingDisable:
        {
        SubStateResettingDisableInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateRejectingDisable:
        {
        SubStateRejectingDisableInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateConnectAccessory:
        {
        SubStateConnectAccessoryInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateIdlingDisconnectAccessory:
        {
        SubStateIdlingDisconnectAccessoryInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateRejectingDisconnectAccessory:
        {
        SubStateRejectingDisconnectAccessoryInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateConnected:
        {
        SubStateConnectedInput( aInterfaceId, aEvent );
        break;
        }
    case ESubStateWaitForSettingsChanged:
        {
        SubStateWaitForSettingsChangedInput( aInterfaceId, aEvent );
        break;
        }
    default:
        {
        INFO_1( "Unexpected Event Id: %i", aEvent );        
        INFO_1( "From interface Id: %i", aInterfaceId );        
        break;
        }
    }
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CHDMICableStateConnected::Id()
    {
    FUNC_LOG;
    TFSMState tempStateId( EHDMICableStateConnected );
    INFO_1( "State's id: %i", tempStateId );
    return tempStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TPtrC CHDMICableStateConnected::Name()
    {
    FUNC_LOG;
    TPtrC temp( KHDMICableStateConnected );
    INFO_1( "State's name: %s", temp.Ptr() );
    return temp;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CHDMICableStateConnected::SubId()
    {
    FUNC_LOG;
    TFSMState tempSubstateId( iSubState );
    INFO_1( "Substate's id: %i", tempSubstateId );
    return tempSubstateId;
    }

// ---------------------------------------------------------------------------
// ListenCopyProtectionStatusChanges.
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::ListenCopyProtectionStatusChanges()
    {
    FUNC_LOG;
    TCopyProtctListenRetValues retStruct = iTVOutConfigForCopyProtect->ListenCopyProtectionIfNoMissedEvents();
    if ( EIfTVOutConfigEventSetCopyProtectStatusChanged == retStruct.iFSMEventId )
        {
        // Copy Protection status changed while not listening
        Input( EPDEIfTVOutConfig, EIfTVOutConfigEventSetCopyProtectStatusChanged );                       
        }
    else if ( EIfTVOutConfigEventCopyProtectionListenFailed == retStruct.iFSMEventId )
        {
        iHDMICableStatusFSM.CopyProtectionListenFailed( retStruct.iError );
        }
    }

// ---------------------------------------------------------------------------
// SubStateFetchEdidDataInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateFetchEdidDataInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfEDIDHandler:
        {
        INFO( "Interface: EPDEIfEDIDHandler" );
        if ( EPDEIfEDIDHandlerEventEdidDataFetched == aEvent )
            {
            INFO( "Event: EPDEIfEDIDHandlerEventEDdidDataFetched" );        
            if ( !iTVOutConfigForHDMI.GetTvOutConfig()->HdmiCableConnected() )
                {
                INFO( "Retreating back to <Idle> since cable was disconnected while not listening!" );        
                iHDMICableStatusFSM.Transit( EHDMICableStateIdle );         
                }
            else
                {
                TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
                if ( EIfTVOutConfigEventCableDisconnected == event )
                    {
                    INFO( "Retreating back to <Idle> since cable was disconnected while not listening!" );        
                    iHDMICableStatusFSM.Transit( EHDMICableStateIdle );         
                    }
                else
                    {
                    iTVOutConfigForSettingChanges->ListenSettingsChanges();
                    iSubState = ESubStateStartListenSettingsChanges;
                    }
                }
            }
        else if ( EPDEIfEDIDHandlerEventEdidDataFetchFailed == aEvent )
            {
            INFO( "Event: EPDEIfEDIDHandlerEventEDdidDataFetchFailed" );        
            INFO( "Going to state <Rejected>" );        
            iHDMICableStatusFSM.Transit( EHDMICableStateRejected );                     
             }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }
    }

// ---------------------------------------------------------------------------
// SubStateStartListenConfigChangesInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateStartListenConfigChangesInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
        case EPDEIfTVOutConfig:
            {       
            INFO( "Interface: TPDEIfTVOutConfig" );        
            if ( EIfTVOutConfigEventSettingsChangesListenStarted == aEvent )
                {
                INFO( "Event: EIfTVOutConfigEventSettingsChangesListenStarted" );        
                if ( !iTVOutConfigForHDMI.GetTvOutConfig()->HdmiCableConnected() )
                    {
                    INFO( "Retreating back to <Idle> since cable was disconnected while not listening!" );        
                    iTVOutConfigForSettingChanges->Cancel();
                    iHDMICableStatusFSM.Transit( EHDMICableStateIdle );         
                    }
                else
                    {
                    TInt retVal = iEDIDHandler.SetVideoParameters();
                    if ( KErrNone != retVal )
                        {
                        INFO( "Going to state <Rejected> since video parameter setting failed!" );        
                        iTVOutConfigForSettingChanges->Cancel();
                        iHDMICableStatusFSM.Transit( EHDMICableStateRejected );         
                        }
                    else
                        {
						// Update the current overscan value
						iEDIDHandler.GetCurrentOverscanValue( iHOverscanValue, iVOverscanValue );
						
                        TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
                        if ( EIfTVOutConfigEventCableDisconnected == event )
                            {
                            INFO( "Retreating back to <Idle> since cable was disconnected while not listening!" );        
                            iTVOutConfigForSettingChanges->Cancel();
                            iHDMICableStatusFSM.Transit( EHDMICableStateIdle );         
                            }
                        else
                            {
                            iSubState = ESubStateWaitForSettingsChanged;
                            }
                        }         
                    }
                }
            else if ( EIfTVOutConfigEventSettingsChangesListenFailed == aEvent )
                {
                INFO( "Event: EIfTVOutConfigEventSettingsChangesListenFailed" );        
                INFO( "Going to state <Rejected> since TV Out Settings Change listening failed!" );        
                iHDMICableStatusFSM.Transit( EHDMICableStateRejected );         
                }
            else
                {
                INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
                }
            break;
            }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }
    }

// ---------------------------------------------------------------------------
// SubStateEnableInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateEnableInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: TPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventEnabled == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventEnabled" );        
            if ( !iTVOutConfigForHDMI.GetTvOutConfig()->HdmiCableConnected() )
                {
                INFO( "Disabling since cable was disconnected while not listening!" );        
                iSubState = ESubStateIdlingDisable;
                iTVOutConfigForHDMI.Disable();            
                }
            else
                {
                TInt retVal( KErrNone );
                TBool copyProtectStatus = iTVOutConfigForCopyProtect->GetTvOutConfig()->CopyProtectionStatus();
                TRAP( retVal, iAccessoryControlIf.ConnectAccessoryL( copyProtectStatus ) );
                if ( KErrNone != retVal )
                    {
                    INFO_1( "Rejecting since CAccessoryControlIf::ConnectAccessoryL() failed, error code %i", retVal );        
                    iSubState = ESubStateRejectingDisable;
                     iTVOutConfigForHDMI.Disable();            
                    }
                else
                    {
                    iSubState = ESubStateConnectAccessory;
                    }
                }
            }
        else if ( EIfTVOutConfigEventEnableFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventEnableFailed" );        
            INFO( "Going to state <Rejected> since TV Out Enabling failed!" );        
            iHDMICableStatusFSM.Transit( EHDMICableStateRejected );         
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }    
    }

// ---------------------------------------------------------------------------
// SubStateResettingEnableInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateResettingEnableInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: TPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventEnabled == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventEnabled" );        
            TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
            if ( EIfTVOutConfigEventCableDisconnected == event )
                {
                INFO( "Disabling since cable was disconnected while not listening!" );        
                iSubState = ESubStateIdlingDisable;
                iTVOutConfigForHDMI.Disable();            
                }
            else
                {
                iSubState = ESubStateConnected;

                TRAPD( err, iHDMICableStatusFSM.SettingsChangedL() );
                if ( KErrNone != err )
                    {
                    INFO_1( "iHDMICableStatusFSM.SettingsChangedL() failed, error code %i", err ); 
                    }
                ListenCopyProtectionStatusChanges();
                iTVOutConfigForSettingChanges->ListenSettingsChanges();
				iCRWatchForVOverScan->Watch();
                }
            }
        else if ( EIfTVOutConfigEventEnableFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventEnableFailed" );        
            INFO( "Going to state <Rejected> since TV Out Enabling failed!" );        
            iSubState = ESubStateRejectingDisconnectAccessory;
            iAccessoryControlIf.DisconnectAccessory();
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }    
    }

// ---------------------------------------------------------------------------
// SubStateIdlingDisableInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateIdlingDisableInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: TPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventDisabled == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventDisabled" );
            iSubState = ESubStateIdlingDisconnectAccessory;
            iAccessoryControlIf.DisconnectAccessory();                    
			ClearAvailableTvOutConfig();
            }
        else if ( EIfTVOutConfigEventDisableFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventDisableFailed" );        
            iSubState = ESubStateIdlingDisconnectAccessory;
            iAccessoryControlIf.DisconnectAccessory();                    
			ClearAvailableTvOutConfig();
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }    
    }

// ---------------------------------------------------------------------------
// SubStateResettingDisableInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateResettingDisableInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: TPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventDisabled == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventDisabled" );
            iSubState = ESubStateResettingEnable;
            iTVOutConfigForHDMI.Enable();                
            }
        else if ( EIfTVOutConfigEventDisableFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventDisableFailed" );        
            iSubState = ESubStateRejectingDisconnectAccessory;
            iAccessoryControlIf.DisconnectAccessory();                    
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }    
    }


// ---------------------------------------------------------------------------
// SubStateRejectingDisableInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateRejectingDisableInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: TPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventDisabled == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventDisabled" );
            iSubState = ESubStateRejectingDisconnectAccessory;
            iAccessoryControlIf.DisconnectAccessory();                    
            }
        else if ( EIfTVOutConfigEventDisableFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventDisableFailed" );        
            iSubState = ESubStateRejectingDisconnectAccessory;
            iAccessoryControlIf.DisconnectAccessory();                    
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }    
    }


// ---------------------------------------------------------------------------
// SubStateConnectAccessoryInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateConnectAccessoryInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAccessoryControl:
        {
        INFO( "Interface: EPDEIfAccessoryControl" );
        if ( EPDEIfAccessoryControlEventConnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryControlEventConnected" );
            TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
            if ( EIfTVOutConfigEventCableDisconnected == event )
                {
                INFO( "Event: EIfTVOutConfigEventCableDisconnected occurred while connecting" );
                iSubState = ESubStateIdlingDisable;
                iTVOutConfigForHDMI.Disable();            
                }
            else
                {
                iSubState = ESubStateConnected;
                ListenCopyProtectionStatusChanges();
                iTVOutConfigForSettingChanges->ListenSettingsChanges();
				iCRWatchForVOverScan->Watch();				
                }
            }
        else if ( EPDEIfAccessoryControlEventConnectFailed == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryControlEventConnectFailed" ); 
            iSubState = ESubStateRejectingDisable;
            iTVOutConfigForHDMI.Disable();        
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }    
    }

// ---------------------------------------------------------------------------
// SubStateIdlingDisconnectAccessoryInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateIdlingDisconnectAccessoryInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAccessoryControl:
        {
        INFO( "Interface: EPDEIfAccessoryControl" );
        if ( EPDEIfAccessoryControlEventDisconnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryControlEventDisconnected" );        
            iHDMICableStatusFSM.Transit( EHDMICableStateIdle );                     
            }
        else if ( EPDEIfAccessoryControlEventDisconnectFailed == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryControlEventDisconnectFailed" );        
            iHDMICableStatusFSM.Transit( EHDMICableStateIdle );                     
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }    
    }

// ---------------------------------------------------------------------------
// SubStateRejectingDisconnectAccessoryInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateRejectingDisconnectAccessoryInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAccessoryControl:
        {
        INFO( "Interface: EPDEIfAccessoryControl" );
        if ( EPDEIfAccessoryControlEventDisconnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryControlEventDisconnected" );        
            iHDMICableStatusFSM.Transit( EHDMICableStateRejected );                     
            }
        else if ( EPDEIfAccessoryControlEventDisconnectFailed == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryControlEventDisconnectFailed" );        
            iHDMICableStatusFSM.Transit( EHDMICableStateRejected );                     
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }    
    }

// ---------------------------------------------------------------------------
// SubStateConnectedInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateConnectedInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAsyCommandHandler:
        {
        INFO( "Interface: EPDEIfAsyCommandHandler" );        
        if ( EIfAsyCommandHandlerEventSetCopyProtectionOn == aEvent )
            {
            INFO( "Event: EIfAsyCommandHandlerEventSetCopyProtectionOn" );
            TInt retVal = iTVOutConfigForCopyProtect->GetTvOutConfig()->CopyProtection( ETrue );
            iHDMICableStatusFSM.CopyProtectionSettingDone( retVal, iTVOutConfigForCopyProtect->GetTvOutConfig()->CopyProtectionStatus() );
            }
        else if ( EIfAsyCommandHandlerEventSetCopyProtectionOff == aEvent )
            {
            INFO( "Event: EIfAsyCommandHandlerEventSetCopyProtectionOff" );
            TInt retVal = iTVOutConfigForCopyProtect->GetTvOutConfig()->CopyProtection( EFalse );
            iHDMICableStatusFSM.CopyProtectionSettingDone( retVal, iTVOutConfigForCopyProtect->GetTvOutConfig()->CopyProtectionStatus() );
            }
        else if ( EIfAsyCommandHandlerEventGetCopyProtectStatus == aEvent )
            {
            INFO( "Event: EIfAsyCommandHandlerEventGetCopyProtectStatus" );
            TBool copyProtectStatus = iTVOutConfigForCopyProtect->GetTvOutConfig()->CopyProtectionStatus();
            iHDMICableStatusFSM.CopyProtectionStatusGot( copyProtectStatus );
            }
        break;
        }
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: TPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventCableConnected == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventCableConnected" );        
            // Re-issue request for connection notification
            TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
            
            if ( EIfTVOutConfigEventCableDisconnected == event )
                {
                INFO( "Event: EIfTVOutConfigEventCableDisconnected occurred while not listening" );
                // Stop listening Copy Protection status
                iTVOutConfigForCopyProtect->Cancel();
                // Stop listen setting changes
                iTVOutConfigForSettingChanges->Cancel();
				iCRWatchForVOverScan->Cancel();
                iSubState = ESubStateIdlingDisable;
                iTVOutConfigForHDMI.Disable();            
                }
             }
        else if ( EIfTVOutConfigEventCableDisconnected == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventCableDisconnected" );
            iSubState = ESubStateIdlingDisable;
            // Stop listening Copy Protection status
            iTVOutConfigForCopyProtect->Cancel();
            // Stop listen setting changes
            iTVOutConfigForSettingChanges->Cancel();
			iCRWatchForVOverScan->Cancel();
            // Disable tv out
            iTVOutConfigForHDMI.Disable();            
            }
        else if ( EIfTVOutConfigEventCableStatusListenFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventCableStatusListenFailed" );
            ++iCableStatusListenFailCounter.iCounter;
            if ( SCableStatusListenFailCounter::EMaxCounterValue <= iCableStatusListenFailCounter.iCounter )
                {
                INFO( "Max number of Cable Status listen failures reached. Retiring to <Rejected>" );
                iSubState = ESubStateRejectingDisable;
                iCableStatusListenFailCounter.iCounter = 0;
                // Stop listening Copy Protection status
                iTVOutConfigForCopyProtect->Cancel();
                // Stop listen setting changes
                iTVOutConfigForSettingChanges->Cancel();
				iCRWatchForVOverScan->Cancel();
                iTVOutConfigForHDMI.Disable();                       
                }
            else
                {
                // Try once again
                TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
                if ( EIfTVOutConfigEventCableDisconnected == event )
                    {
                    INFO( "Event: EIfTVOutConfigEventCableDisconnected occurred while not listening" );
                    // Stop listening Copy Protection status
                    iTVOutConfigForCopyProtect->Cancel();
                    // Stop listen setting changes
                    iTVOutConfigForSettingChanges->Cancel();
					iCRWatchForVOverScan->Cancel();
                    iSubState = ESubStateIdlingDisable;
                    iTVOutConfigForHDMI.Disable();            
                    }
                }
            }
        else if ( EIfTVOutConfigEventSetCopyProtectStatusChanged == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSetCopyProtectStatusChanged" );        
            TBool latestStatus = iTVOutConfigForCopyProtect->GetLatestRecordedCopyProtectionStatus();
            iHDMICableStatusFSM.CopyProtectionStatusChanged( latestStatus );
            ListenCopyProtectionStatusChanges();
            }
        else if ( EIfTVOutConfigEventCopyProtectionListenFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventCopyProtectionListenFailed" );
            ++iCopyProtectListenFailCounter.iCount; 
            if ( SCopyProtectListenFailCounter::ECounterMaxValue < iCopyProtectListenFailCounter.iCount )
                {
                iHDMICableStatusFSM.CopyProtectionListenFailed( iTVOutConfigForCopyProtect->iStatus.Int() );
                // Try again
                ListenCopyProtectionStatusChanges();
                }
            else
                {
                INFO( "Max number of Copy Protection Status listen failures reached. Retiring to <Rejected>" );
                iCopyProtectListenFailCounter.iCount = 0;
                // Stop listening cable status changes while disabling.
                iTVOutConfigForHDMI.Cancel();
                // Stop listening Copy Protection status
                iTVOutConfigForCopyProtect->Cancel();
				iCRWatchForVOverScan->Cancel();
                iSubState = ESubStateRejectingDisable;
                iTVOutConfigForHDMI.Disable();            
                }
            }
        else if ( EIfTVOutConfigEventSettingsChanged == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSettingsChanged" );
            // Stop listening cable connection status
            iTVOutConfigForHDMI.Cancel();
            // Stop listening Copy Protection status
            iTVOutConfigForCopyProtect->Cancel();
			iCRWatchForVOverScan->Cancel();
            if ( iTVOutConfigForHDMI.GetTvOutConfig()->Enabled() )
                {
                iSubState = ESubStateResettingDisable;
                iTVOutConfigForHDMI.Disable();
                }
            else
                {
                iSubState = ESubStateResettingEnable;
                iTVOutConfigForHDMI.Enable();                
                }
            }
        else if ( EIfTVOutConfigEventSettingsChangesListenStarted == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSettingsChangesListenStarted" );
            // Everything is OK. Stay in the same state.
			// Should not miss the cenrep value change
			TInt newOverScanValue = 0;
			iCRWatchForVOverScan->GetCurrentValue(newOverScanValue);
			if( iVOverscanValue != newOverScanValue )
				{
				Input( EPDEIfCentralRepositoryWatch, EPDEIfCentralRepositoryWatchEventKeyChanged );
				}
            }
        else if ( EIfTVOutConfigEventSettingsChangesListenFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSettingsChangesListenFailed" );
            INFO( "Going to state <Rejected> since setting changes listening failed!" );        
            iSubState = ESubStateRejectingDisable;
            // Stop listening cable connection status
            iTVOutConfigForHDMI.Cancel();
            // Stop listening Copy Protection status
            iTVOutConfigForCopyProtect->Cancel();
			iCRWatchForVOverScan->Cancel();
            iTVOutConfigForHDMI.Disable();            
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    case EPDEIfCentralRepositoryWatch:
        {       
        INFO( "Interface: EPDEIfCentralRepositoryWatch" );
        if ( EPDEIfCentralRepositoryWatchEventKeyChanged == aEvent )
            {
            INFO( "Event: EPDEIfCentralRepositoryWatchEventKeyChanged" );
			
			// Get the available config
			THdmiDviTimings curConfig;
			
			iTVOutConfigForHDMI.GetTvOutConfig()->GetConfig( curConfig );

			if( (TTvSettings::EHDMI == curConfig.iConnector) && (!curConfig.iUnderscanEnabled) )
				{									
				
				TInt retVal = iEDIDHandler.SetVideoParameters();
				if ( KErrNone != retVal )
					{
					INFO( "Going to state <Rejected> since video parameter setting failed!" );		  
					iTVOutConfigForSettingChanges->Cancel();
					iCRWatchForVOverScan->Cancel();
					iSubState = ESubStateIdlingDisable;
					iTVOutConfigForHDMI.Disable();			  
					}
				else
					{
					// Update the current overscan value
					iEDIDHandler.GetCurrentOverscanValue( iHOverscanValue, iVOverscanValue );
					
					TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
					if ( EIfTVOutConfigEventCableDisconnected == event )
						{
						INFO( "Retreating back to <Idle> since cable was disconnected while not listening!" );		  
						iTVOutConfigForSettingChanges->Cancel();
						iCRWatchForVOverScan->Cancel();
						iSubState = ESubStateIdlingDisable;
						iTVOutConfigForHDMI.Disable();			  
						}
					}				
				}			
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }       
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }
    }

// ---------------------------------------------------------------------------
// SubStateWaitForSettingsChangedInput
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::SubStateWaitForSettingsChangedInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: TPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventCableConnected == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventCableConnected" );        
            TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
            if ( EIfTVOutConfigEventCableDisconnected == event )
                {
                INFO( "Retreating back to <Idle> since cable was disconnected while WF setting changes!" );        
				
                // Stop listening Copy Protection status
                iTVOutConfigForCopyProtect->Cancel();
                // Stop listen setting changes
                iTVOutConfigForSettingChanges->Cancel();
				iCRWatchForVOverScan->Cancel();

				if( iTVOutConfigForHDMI.GetTvOutConfig()->Enabled() )
					{
					iSubState = ESubStateIdlingDisable;
					iTVOutConfigForHDMI.Disable();			  
					}
				else
					{
					iHDMICableStatusFSM.Transit( EHDMICableStateIdle ); 		
					}
                }            
             }
        else if ( EIfTVOutConfigEventCableDisconnected == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventCableDisconnected" );
            INFO( "Retreating back to <Idle> since cable was disconnected while WF setting changes!" );
			// Stop listening Copy Protection status
			iTVOutConfigForCopyProtect->Cancel();
			// Stop listen setting changes
			iTVOutConfigForSettingChanges->Cancel();
			iCRWatchForVOverScan->Cancel();
			
			if( iTVOutConfigForHDMI.GetTvOutConfig()->Enabled() )
				{
				iSubState = ESubStateIdlingDisable;
				iTVOutConfigForHDMI.Disable();			  
				}
			else
				{
				iHDMICableStatusFSM.Transit( EHDMICableStateIdle ); 		
				}
            }
        else if ( EIfTVOutConfigEventSettingsChanged == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSettingsChanged" );
            iSubState = ESubStateEnable;
            iTVOutConfigForHDMI.Enable();
            }
        else if ( EIfTVOutConfigEventSettingsChangesListenFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSettingsChangesListenFailed" );
            INFO( "Going to state <Rejected> since setting changes listening failed!" );        
			// Stop listening Copy Protection status
			iTVOutConfigForCopyProtect->Cancel();
			// Stop listen setting changes
			iCRWatchForVOverScan->Cancel();
			
			if( iTVOutConfigForHDMI.GetTvOutConfig()->Enabled() )
				{
				iSubState = ESubStateIdlingDisable;
				iTVOutConfigForHDMI.Disable();			  
				}
			else
				{
				iHDMICableStatusFSM.Transit( EHDMICableStateRejected ); 		
				}
            }
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Event received from unknown/unexpected interface Id: %i", aInterfaceId );        
        break;
        }
    }
    }

// ---------------------------------------------------------------------------
// ClearAvailableConfig
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::ClearAvailableTvOutConfig()
    {
    FUNC_LOG;
    
    if( iTVOutConfigForHDMI.GetTvOutConfig() )
        {
        RArray<THdmiDviTimings> HdmiSettingsArray;        
    	RArray<TTvSettings> TvSettingsArray;       
    
        TInt err = iTVOutConfigForHDMI.GetTvOutConfig()->SetAvailableTvConfigList( TvSettingsArray, HdmiSettingsArray );
        
        HdmiSettingsArray.Close();
    	TvSettingsArray.Close();
        }              
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CHDMICableStateConnected::ConstructL()
    {
    FUNC_LOG;
    // Create Central Repository Watchers
    iCRWatchForVOverScan = CCentralRepositoryWatch::NewL(*this, KCRUidTvoutSettings, KSettingsTvoutVerticalOverscan );
	
    iTVOutConfigForCopyProtect = CTVOutConfigForHDMI::NewL( iHDMICableStatusFSM );
    iTVOutConfigForSettingChanges = CTVOutConfigForHDMI::NewL( iHDMICableStatusFSM );	
    }


// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CHDMICableStateConnected::CHDMICableStateConnected(
        CTVOutConfigForHDMI& aTVOutConfigForCableConnect,
        CEDIDHandler& aEDIDHandler,
        CAccessoryControlIf& aAccessoryControlIf,
        CHDMICableStatusFSM&  aHDMICableStatusFSM ):
    iTVOutConfigForHDMI( aTVOutConfigForCableConnect ),
    iEDIDHandler( aEDIDHandler ),
    iAccessoryControlIf( aAccessoryControlIf ),
    iHDMICableStatusFSM( aHDMICableStatusFSM )   
    {
    FUNC_LOG;
    }


// ======== GLOBAL FUNCTIONS ========

