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
* Description:  Implementation of "Connected" state of Composite Cable Status FSM.
*
*/



#include <e32def.h>
#include <e32cmn.h>


#include "pdeconstants.h"
#include "compositecablestatusfsm.h"
#include "compositecablestateconnected.h"
#include "tvoutconfigforcomposite.h"
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
CCompositeCableStateConnected* CCompositeCableStateConnected::NewL( 
        MFSMForState& aFSMForState, 
        CTVOutConfigForComposite& aTVOutConfigForComposite,
        CCompositeCableStatusFSM& aCompositeCableStatusFSM)
    {
    FUNC_LOG;
    CCompositeCableStateConnected* self = new ( ELeave ) CCompositeCableStateConnected(
            aFSMForState, aTVOutConfigForComposite, aCompositeCableStatusFSM);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCompositeCableStateConnected::~CCompositeCableStateConnected()
    {
    FUNC_LOG;
    if ( iCRWatchForAspectRatio )
        {
        iCRWatchForAspectRatio->Cancel();
        delete iCRWatchForAspectRatio;
        }
    if ( iCRWatchForFlickerFilter )
        {
        iCRWatchForFlickerFilter->Cancel();
        delete iCRWatchForFlickerFilter;
        }
    if ( iCRWatchForTvSystem )
        {
        iCRWatchForTvSystem->Cancel();
        delete iCRWatchForTvSystem;        
        }
    if ( iCRWatchForVOverScan )
        {
        iCRWatchForVOverScan->Cancel();
        delete iCRWatchForVOverScan;        
        }
    if ( iTVOutConfigForSettingChanges ) 
        {
        iTVOutConfigForSettingChanges->Cancel();
        delete iTVOutConfigForSettingChanges;             
        }
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::Enter( )
    {
    FUNC_LOG;    

	iTVOutConfigForComposite.UpdateOverscanValues();
    ClearAvailableTvOutConfig();
    
    iTVOutConfigForSettingChanges->ListenSettingsChanges();
    iSubState = ESubStateStartListenSettingsChanges;    
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::Input(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    if ( ESubStateEnabling == iSubState )
        {
        SubStateEnablingInput( aInterfaceId, aEvent );
        }
    else if ( ESubStateDisabling == iSubState )
        {
        SubStateDisablingInput( aInterfaceId, aEvent );          
        }
    else if ( ESubStateSetting == iSubState )
        {
        SubStateSettingInput( aInterfaceId, aEvent );          
        }
    else if ( ESubStateStartListenSettingsChanges == iSubState )
        {
        SubStateStartListenConfigChangesInput( aInterfaceId, aEvent );
        }
    else if ( ESubStateResettingDisable == iSubState )    
        {
        SubStateResettingDisableInput( aInterfaceId, aEvent );
        }
    else if ( ESubStateResetting == iSubState )
        {
        SubStateResettingInput( aInterfaceId, aEvent );        
        }
    else
        {
        SubStateEnabledInput( aInterfaceId, aEvent );          
        }
        
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CCompositeCableStateConnected::Id()
    {
    FUNC_LOG;
    TFSMState tempStateId( ECompositeCableStateConnected );
    INFO_1( "State's id: %i", tempStateId );
    return tempStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TPtrC CCompositeCableStateConnected::Name()
    {
    FUNC_LOG;
    TPtrC temp( KCompositeCableStateConnected );
    INFO_1( "State's name: %s", temp.Ptr() );
    return temp;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CCompositeCableStateConnected::SubId()
    {
    FUNC_LOG;
    INFO_1( "Substate's id: %i", iSubState );
    return iSubState;
    }

// ---------------------------------------------------------------------------
// From MCentralRepositoryObserver.
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::CentRepKeyChanged(
        TUid    aRepositoryId,
        TUint32 aId )
    {
    FUNC_LOG;
	INFO("Cenrep Value Changed");
    if ( KCRUidTvoutSettings == aRepositoryId )
        {
        if (    ( KSettingsTvAspectRatio == aId ) ||
                ( KSettingsTvoutFlickerFilter == aId  ) ||
                ( KSettingsTvSystemInfo == aId ) )
            {
            Input( EPDEIfCentralRepositoryWatch, EPDEIfCentralRepositoryWatchEventKeyChanged );
            }
        else if( KSettingsTvoutVerticalOverscan == aId )
			{
			if( iTVOutConfigForComposite.UpdateOverscanValues() )
				{
				INFO( "There is a real overscan change" );
				Input( EPDEIfCentralRepositoryWatch, EPDEIfCentralRepositoryWatchEventOverscanKeyChanged );
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
void CCompositeCableStateConnected::CentRepGetKeyValueFailed(
        TUid    aRepositoryId,
        TUint32 aId,
        TInt    aError )
    {
    FUNC_LOG;
    // Make use of aError argument top get rid of compiler warning.
    TInt error( aError );
    if ( ( KCRUidTvoutSettings == aRepositoryId ) && ( KErrNone != error ) )
        {
        if ( KSettingsTvAspectRatio == aId )
            {
            INFO_1(" Getting KSettingsTvAspectRatio from CR failed, aError %d", error );                        
            }
        else if ( KSettingsTvoutFlickerFilter == aId  )
            {
            INFO_1(" Getting KSettingsTvoutFlickerFilter from CR failed, aError %d", error );                                    
            }
        else if ( KSettingsTvSystemInfo == aId )
            {
            INFO_1(" Getting KSettingsTvSystemInfo from CR failed, aError %d", error );                                    
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
// SubStateEnablingInput
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::SubStateEnablingInput( 
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAccessoryConnection:
        {       
        INFO( "Interface: EPDEIfAccessoryConnection" );        
        if ( EPDEIfAccessoryConnectionEventCableConnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableConnected" );           
            INFO( "Doing nothing, since already connected" );        
            }
        else if ( EPDEIfAccessoryConnectionEventCableDisconnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableDisconnected" );           
            // Store indication to be handled after enable completed
            iCableDisconnectedWhileEnabling = ETrue;
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: EPDEIfTVOutConfig" );        
        INFO( "Event: EIfTVOutConfigEventEnabled" );
        if ( EIfTVOutConfigEventEnabled == aEvent )
            {
            if ( !iCableDisconnectedWhileEnabling )
                {
                iCRWatchForFlickerFilter->Watch();
                iCRWatchForAspectRatio->Watch();
                iCRWatchForTvSystem->Watch();
				iCRWatchForVOverScan->Watch();
                iSubState = ESubStateEnabled;
                }
            else
                {
                iCableDisconnectedWhileEnabling = EFalse;
                ClearAvailableTvOutConfig();
                iTVOutConfigForComposite.Disable();
                iSubState = ESubStateDisabling;
                }
            }
        else if ( EIfTVOutConfigEventEnableFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventEnableFailed" );        
            INFO( "Retreating back to <Idle>!" );
            iCableDisconnectedWhileEnabling = EFalse;
            TransitStateIdle();         
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_2( "Event from unexpected interface. if Id, event id: %i", aInterfaceId, aEvent );        
        break;
        }
    }
    return;
    }

// ---------------------------------------------------------------------------
// SubStateEnabledInput
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::SubStateEnabledInput( 
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAccessoryConnection:
        {       
        INFO( "Interface: EPDEIfAccessoryConnection" );        
        if ( EPDEIfAccessoryConnectionEventCableConnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableConnected" );           
            INFO( "Doing nothing, since already connected" );        
            }
        else if ( EPDEIfAccessoryConnectionEventCableDisconnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableDisconnected" );           
            iCRWatchForFlickerFilter->Cancel();
            iCRWatchForAspectRatio->Cancel();
            iCRWatchForTvSystem->Cancel();
			iCRWatchForVOverScan->Cancel();
            ClearAvailableTvOutConfig();
            iTVOutConfigForComposite.Disable();
            iSubState = ESubStateDisabling;
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    case EPDEIfCentralRepositoryWatch:
        {       
        INFO( "Interface: EPDEIfCentralRepositoryWatch" );
        if ( EPDEIfCentralRepositoryWatchEventKeyChanged == aEvent )
            {
            INFO( "Event: EPDEIfCentralRepositoryWatchEventKeyChanged" );
            iCRWatchForFlickerFilter->Cancel();
            iCRWatchForAspectRatio->Cancel();
            iCRWatchForTvSystem->Cancel();
			iCRWatchForVOverScan->Cancel();
            ClearAvailableTvOutConfig();
			iResettingInput = ETrue;
            Enter();
            }
		else if ( EPDEIfCentralRepositoryWatchEventOverscanKeyChanged == aEvent )
			{
			INFO( "Event: EPDEIfCentralRepositoryWatchEventOverscanKeyChanged" );
			iTVOutConfigForComposite.SetConfig( ETrue );
			}
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }       
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: EPDEIfTVOutConfig" );       
		if ( EIfTVOutConfigEventSet == aEvent )
			{
			INFO( "Event: EIfTVOutConfigEventSet" );
			// Stay on the same state
			}
		else if ( EIfTVOutConfigEventSetFailed == aEvent )
			{
			INFO( "Event: EIfTVOutConfigEventSetFailed" );
            iCRWatchForFlickerFilter->Cancel();
            iCRWatchForAspectRatio->Cancel();
            iCRWatchForTvSystem->Cancel();
			iCRWatchForVOverScan->Cancel();
            ClearAvailableTvOutConfig();
            iTVOutConfigForComposite.Disable();
            iSubState = ESubStateDisabling;
			}
		break;
		}
    default:
        {
        INFO_2( "Event from unexpected interface. if Id, event id: %i", aInterfaceId, aEvent );        
        break;
        }
    }
    return;
    }

// ---------------------------------------------------------------------------
// SubStateDisablingInput
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::SubStateDisablingInput( 
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAccessoryConnection:
        {       
        INFO( "Interface: EPDEIfAccessoryConnection" );        
        if ( EPDEIfAccessoryConnectionEventCableConnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableConnected" );           
            // Store to be handled after disable has been completed.
            iCableConnectedWhileDisabling = ETrue;
            }
        else if ( EPDEIfAccessoryConnectionEventCableDisconnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableDisconnected" );           
            INFO( "Doing nothing, since already disconnecting" );        
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: EPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventDisabled == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventDisabled" );
            if ( !iCableConnectedWhileDisabling )
                {
                TransitStateIdle();
                }
            else
                {
                iCableConnectedWhileDisabling = EFalse;
                // Back to the beginning of this state
                Enter();
                }
            }
        else if ( EIfTVOutConfigEventDisableFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventDisableFailed" ); 
            iCableConnectedWhileDisabling = EFalse;
            TransitStateIdle();             
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_2( "Event from unexpected interface. if Id, event id: %i", aInterfaceId, aEvent );        
        break;
        }
    }
    return;
    }

// ---------------------------------------------------------------------------
// SubStateResettingDisableInput
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::SubStateResettingDisableInput(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
        {    
        case EPDEIfAccessoryConnection:
            {       
            INFO( "Interface: EPDEIfAccessoryConnection" );        
            if ( EPDEIfAccessoryConnectionEventCableConnected == aEvent )
                {
                INFO( "Event: EPDEIfAccessoryConnectionEventCableConnected" );           
                // Store to be handled after disable has been completed.
                iCableDisconnectedWhileResettingDisable = ETrue;
                }
            else if ( EPDEIfAccessoryConnectionEventCableDisconnected == aEvent )
                {
                INFO( "Event: EPDEIfAccessoryConnectionEventCableDisconnected" );           
                INFO( "Doing nothing, since already disconnecting" );        
                }
            else
                {
                INFO_1( "Unknown Event Id: %i", aEvent );        
                }
            break;
            }            
        case EPDEIfTVOutConfig:
            {       
            INFO( "Interface: EPDEIfTVOutConfig" );        
            if ( EIfTVOutConfigEventDisabled == aEvent )
                {
                INFO( "Event: EIfTVOutConfigEventDisabled" );
                if ( !iCableDisconnectedWhileResettingDisable )
                    {
                    iSubState = ESubStateEnabling;
                    iTVOutConfigForComposite.Enable();
                    }
                else
                    {
                    iCableDisconnectedWhileResettingDisable = EFalse;
                    // Back to the beginning of this state
                    Enter();
                    }
                }
            else if ( EIfTVOutConfigEventDisableFailed == aEvent )
                {
                INFO( "Event: EIfTVOutConfigEventDisableFailed" );
                iCableDisconnectedWhileResettingDisable = EFalse;
                TransitStateIdle();
                }
            else
                {
                INFO_1( "Unknown Event Id: %i", aEvent );           
                }
            break;
            }
        default:
            {
            INFO_2( "Event from unexpected interface. if Id, event id: %i", aInterfaceId, aEvent );        
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// SubStateStartListenConfigChangesInput
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::SubStateStartListenConfigChangesInput( 
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
        {
        case EPDEIfAccessoryConnection:
                {       
                INFO( "Interface: EPDEIfAccessoryConnection" );        
                if ( EPDEIfAccessoryConnectionEventCableConnected == aEvent )
                    {
                    INFO( "Event: EPDEIfAccessoryConnectionEventCableConnected" );           
                    INFO( "Doing nothing, since already connected" );
                    }
                else if ( EPDEIfAccessoryConnectionEventCableDisconnected == aEvent )
                    {
                    INFO( "Event: EPDEIfAccessoryConnectionEventCableDisconnected" );           
                    // Store to be handled after disable has been completed.
                    iCableDisconnectedWhileListening = ETrue;
                    }
                else
                    {
                    INFO_1( "Unknown Event Id: %i", aEvent );        
                    }
                break;
                }
        case EPDEIfTVOutConfig:
            {
            INFO( "Interface: EPDEIfTVOutConfig" );
            if ( EIfTVOutConfigEventSettingsChangesListenStarted == aEvent )
                {
                INFO( "Event: EIfTVOutConfigEventSettingsChangesListenStarted" );
                if ( !iCableDisconnectedWhileListening )
                    {
                    if(iResettingInput)
                        {
                        iTVOutConfigForComposite.SetConfig( EFalse );
                        iSubState = ESubStateResetting;
                        iResettingInput = EFalse;
                        }
                    else
                        {
                        iTVOutConfigForComposite.SetConfig( ETrue );
                        iSubState = ESubStateSetting;
                        }
                    }
                else
                    {
                    INFO( "Retreating back to <Idle> since cable was disconnected while not listening!" );
                    iTVOutConfigForSettingChanges->Cancel();
                    iCableDisconnectedWhileListening = EFalse;
                    
                    if(iResettingInput)
                        {
                        iCRWatchForFlickerFilter->Cancel();
                        iCRWatchForAspectRatio->Cancel();
                        iCRWatchForTvSystem->Cancel();
						iCRWatchForVOverScan->Cancel();
                        iResettingInput = EFalse;
                        }
                    
                    TransitStateIdle();
                    }
                }
            else if ( EIfTVOutConfigEventSettingsChangesListenFailed == aEvent )
                {
                INFO( "Event: EIfTVOutConfigEventSettingsChangesListenFailed" );        
                INFO( "Going to state <Idle> since TV Out Settings Change listening failed!" );
                iCableDisconnectedWhileListening = EFalse;
                
                if(iResettingInput)
                    {
                    iCRWatchForFlickerFilter->Cancel();
                    iCRWatchForAspectRatio->Cancel();
                    iCRWatchForTvSystem->Cancel();
					iCRWatchForVOverScan->Cancel();
                    iResettingInput = EFalse;
                    }
                
                TransitStateIdle();                         
                }
            else
                {
                INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
                }
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// SubStateSettingInput
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::SubStateSettingInput( 
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAccessoryConnection:
        {       
        INFO( "Interface: EPDEIfAccessoryConnection" );        
        if ( EPDEIfAccessoryConnectionEventCableConnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableConnected" );           
            INFO( "Doing nothing, since already connected" );
            }
        else if ( EPDEIfAccessoryConnectionEventCableDisconnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableDisconnected" );           
            // Store to be handled after disable has been completed.
            iCableDisconnectedWhileSetting = ETrue;
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: EPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventSettingsChanged == aEvent )
            {
            if ( !iCableDisconnectedWhileSetting )
                {
                if ( iTVOutConfigForComposite.GetTvOutConfig()->Enabled() )
                    {
                    ClearAvailableTvOutConfig();
                    iTVOutConfigForComposite.Disable();
                    iSubState = ESubStateResettingDisable;                
                    }
                else
                    {
                    iTVOutConfigForComposite.Enable();
                    iSubState = ESubStateEnabling;
                    }
                }
            else
                {
                ClearAvailableTvOutConfig();
                iTVOutConfigForComposite.Disable();
                iCableDisconnectedWhileSetting = EFalse;
                iSubState = ESubStateDisabling;
                }
            }
        else if ( EIfTVOutConfigEventSetFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSetFailed" );
            iCableDisconnectedWhileSetting = EFalse;
            iClearConfig = EFalse;
            TransitStateIdle();             
            }
        else if ( EIfTVOutConfigEventSettingsChangesListenFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSettingsChangesListenFailed" );
            iCableDisconnectedWhileSetting = EFalse;
            TransitStateIdle();
            }
        else if( EIfTVOutConfigEventSet == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSet" );
            iClearConfig = ETrue;
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_2( "Event from unexpected interface. if Id, event id: %i", aInterfaceId, aEvent );        
        break;
        }
    }
    return;
    }

// ---------------------------------------------------------------------------
// SubStateResettingInput
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::SubStateResettingInput( 
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    switch ( aInterfaceId )
    {
    case EPDEIfAccessoryConnection:
        {       
        INFO( "Interface: EPDEIfAccessoryConnection" );        
        if ( EPDEIfAccessoryConnectionEventCableConnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableConnected" );           
            INFO( "Doing nothing, since already connected" );
            }
        else if ( EPDEIfAccessoryConnectionEventCableDisconnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableDisconnected" );           
            // Store to be handled after disable has been completed.
            iCableDisconnectedWhileResetting = ETrue;
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: EPDEIfTVOutConfig" );
        
        if ( EIfTVOutConfigEventSettingsChanged == aEvent )
            {
            if ( !iCableDisconnectedWhileResetting )
                {
                if ( iTVOutConfigForComposite.GetTvOutConfig()->Enabled() )
                    {
                    iTVOutConfigForComposite.Disable();
                    iSubState = ESubStateResettingDisable;                
                    }
                else
                    {
                    iTVOutConfigForComposite.Enable();
                    iSubState = ESubStateEnabling;
                    }
                }
            else
                {
                iCableDisconnectedWhileResetting = EFalse;
                ClearAvailableTvOutConfig();
                iTVOutConfigForComposite.Disable();
                iSubState = ESubStateDisabling;
                }
            
            iCRWatchForFlickerFilter->Cancel();
            iCRWatchForAspectRatio->Cancel();
            iCRWatchForTvSystem->Cancel();            
			iCRWatchForVOverScan->Cancel();
            }
        else if ( EIfTVOutConfigEventSetFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSetFailed" );
            iCableDisconnectedWhileResetting = EFalse;
            iClearConfig = EFalse;
            // Lets continue with old settings.
            iSubState = ESubStateEnabled;                    
            }
        else if(EIfTVOutConfigEventSettingsChangesListenFailed == aEvent)
            {
            INFO( "Event: EIfTVOutConfigEventSettingsChangesListenFailed" );
            iCableDisconnectedWhileResetting = EFalse;
            // Lets continue with old settings.
            iSubState = ESubStateEnabled;      
            }
        else if( EIfTVOutConfigEventSet == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventSet" );
            iClearConfig = ETrue;
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_2( "Event from unexpected interface. if Id, event id: %i", aInterfaceId, aEvent );        
        break;
        }
    }
    return;
    }

// ---------------------------------------------------------------------------
// ClearAvailableConfig
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::ClearAvailableTvOutConfig()
    {
    FUNC_LOG;
    
    if( iClearConfig )
        {
        if( iTVOutConfigForComposite.GetTvOutConfig() )
            {
            RArray<THdmiDviTimings> HdmiSettingsArray;        
        	RArray<TTvSettings> TvSettingsArray;       
        
            TInt err = iTVOutConfigForComposite.GetTvOutConfig()->SetAvailableTvConfigList( TvSettingsArray, HdmiSettingsArray );
            iClearConfig = EFalse;
            
            HdmiSettingsArray.Close();
        	TvSettingsArray.Close();
            }              
        }
    }

// ---------------------------------------------------------------------------
// TransitStateIdle
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::TransitStateIdle()
    {
    FUNC_LOG;
    
    ClearAvailableTvOutConfig();
    iFSMForState.Transit( ECompositeCableStateIdle );
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CCompositeCableStateConnected::ConstructL()
    {
    FUNC_LOG;
    // Create Central Repository Watchers
    iCRWatchForAspectRatio = CCentralRepositoryWatch::NewL(*this, KCRUidTvoutSettings, KSettingsTvAspectRatio );
    iCRWatchForFlickerFilter = CCentralRepositoryWatch::NewL(*this, KCRUidTvoutSettings, KSettingsTvoutFlickerFilter );
    iCRWatchForTvSystem = CCentralRepositoryWatch::NewL(*this, KCRUidTvoutSettings, KSettingsTvSystemInfo );    
    iCRWatchForVOverScan = CCentralRepositoryWatch::NewL(*this, KCRUidTvoutSettings, KSettingsTvoutVerticalOverscan );
    iTVOutConfigForSettingChanges = CTVOutConfigForComposite::NewL(iCompositeCableStatusFSM);
    iResettingInput = EFalse;
    }


// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CCompositeCableStateConnected::CCompositeCableStateConnected(
        MFSMForState& aFSMForState, 
        CTVOutConfigForComposite& aTVOutConfigForComposite,
        CCompositeCableStatusFSM& aCompositeCableStatusFSM)        
    : iFSMForState( aFSMForState ),
    iTVOutConfigForComposite( aTVOutConfigForComposite ),
    iCompositeCableStatusFSM( aCompositeCableStatusFSM )
    {
    FUNC_LOG;
    }


// ======== GLOBAL FUNCTIONS ========

