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
* Description:  Implementation of "Rejected" state of HDMI Cable Status FSM.
*
*/



#include <e32def.h>
#include <e32cmn.h>


#include "pdeconstants.h"
#include "hdmicablestatusfsm.h"
#include "hdmicablestaterejected.h"
#include "tvoutconfigforhdmi.h"
#include "edidhandler.h"
#include "trace.h"




// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CHDMICableStateRejected* CHDMICableStateRejected::NewL( 
        MFSMForState& aFSMForState, 
        CTVOutConfigForHDMI& aTVOutConfigForHDMI,
        CEDIDHandler& aEDIDHandler )
    {
    FUNC_LOG;
    CHDMICableStateRejected* self = new ( ELeave ) CHDMICableStateRejected(
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
CHDMICableStateRejected::~CHDMICableStateRejected()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CHDMICableStateRejected::Enter( )
    {
    FUNC_LOG;
    Name();
    iEDIDHandler.ResetData();
    iTVOutConfigForHDMI.ListenForDisconnection();
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CHDMICableStateRejected::Input(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;    
    switch ( aInterfaceId )
    {
    case EPDEIfTVOutConfig:
        {       
        INFO( "Interface: TPDEIfTVOutConfig" );        
        if ( EIfTVOutConfigEventCableDisconnected == aEvent ||  EIfTVOutConfigEventCableStatusListenFailed == aEvent )
            {
            INFO( "Event: EIfTVOutConfigEventCableDisconnected" );        
            iFSMForState.Transit( EHDMICableStateIdle ); 
            }
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
TFSMState CHDMICableStateRejected::Id()
    {
    FUNC_LOG;
    TFSMState tempStateId( EHDMICableStateRejected );
    INFO_1( "State's id: %i", tempStateId );
    return tempStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TPtrC CHDMICableStateRejected::Name()
    {
    FUNC_LOG;
    TPtrC temp( KHDMICableStateRejected );
    INFO_1( "State's name: %s", temp.Ptr() );
    return temp;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CHDMICableStateRejected::SubId()
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
void CHDMICableStateRejected::ConstructL()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CHDMICableStateRejected::CHDMICableStateRejected(
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

