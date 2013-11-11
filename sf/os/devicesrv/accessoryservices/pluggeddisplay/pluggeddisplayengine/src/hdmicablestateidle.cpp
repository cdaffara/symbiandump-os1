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
* Description:  Implementation of "Idle" state of HDMI Cable Status FSM.
*
*/



#include <e32def.h>
#include <e32cmn.h>


#include "pdeconstants.h"
#include "hdmicablestatusfsm.h"
#include "hdmicablestateidle.h"
#include "tvoutconfigforhdmi.h"
#include "edidhandler.h"
#include "trace.h"




// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CHDMICableStateIdle* CHDMICableStateIdle::NewL( 
        MFSMForState& aFSMForState, 
        CTVOutConfigForHDMI& aTVOutConfigForHDMI,
        CEDIDHandler& aEDIDHandler )
    {
    FUNC_LOG;
    CHDMICableStateIdle* self = new ( ELeave ) CHDMICableStateIdle(
            aFSMForState,
            aTVOutConfigForHDMI,
            aEDIDHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHDMICableStateIdle::~CHDMICableStateIdle()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CHDMICableStateIdle::Enter( )
    {
    FUNC_LOG;
    Name();
    iEDIDHandler.ResetData();
    TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
    if ( EIfTVOutConfigEventCableConnected == event )
        {
        INFO( "Retreating back to <Connected> since cable was connected while not listening!" );        
        iFSMForState.Transit( EHDMICableStateConnected );         
        }
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CHDMICableStateIdle::Input(
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
            iFSMForState.Transit( EHDMICableStateConnected ); 
            }
        else if ( EIfTVOutConfigEventCableDisconnected == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventCableDisconnected" );        
            TFSMEventId event = iTVOutConfigForHDMI.ListenHDMICableStatusIfNoMissedEvents();
            if ( EIfTVOutConfigEventCableConnected == event )
                {
                INFO( "Retreating back to <Connected> since cable was connected while not listening!" );        
                iFSMForState.Transit( EHDMICableStateConnected );         
                }
            }
        // Event EIfTVOutConfigEventCableStatusListenFailed is not allowed in Idle state.
        // If listening fails in this state TV Out Config For HDMI object has to try to listen
        // cable connection again after a short timeout (and making retrials forever if necessary).
        else
            {
            INFO_1( "Unknown/unexpected Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Unknown/unexpected Interface Id: %i", aInterfaceId );        
        break;
        }
    }
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CHDMICableStateIdle::Id()
    {
    FUNC_LOG;
    TFSMState tempStateId( EHDMICableStateIdle );
    INFO_1( "State's id: %i", tempStateId );
    return tempStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TPtrC CHDMICableStateIdle::Name()
    {
    FUNC_LOG;
    TPtrC temp( KHDMICableStateIdle );
    INFO_1( "State's name: %s", temp.Ptr() );
    return temp;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CHDMICableStateIdle::SubId()
    {
    FUNC_LOG;
    TFSMState tempSubstateId( iSubState );
    INFO_1( "Substate's id: %i", tempSubstateId );
    return tempSubstateId;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CHDMICableStateIdle::ConstructL()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CHDMICableStateIdle::CHDMICableStateIdle(
        MFSMForState& aFSMForState, 
        CTVOutConfigForHDMI& aTVOutConfigForHDMI,
        CEDIDHandler& aEDIDHandler ):
    iFSMForState( aFSMForState ),
    iTVOutConfigForHDMI( aTVOutConfigForHDMI ),
    iEDIDHandler( aEDIDHandler )        
    {
    FUNC_LOG;
    }


// ======== GLOBAL FUNCTIONS ========

