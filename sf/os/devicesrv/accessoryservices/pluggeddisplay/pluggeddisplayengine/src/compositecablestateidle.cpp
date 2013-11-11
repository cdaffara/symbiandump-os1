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
* Description:  Implementation of "Idle" state of Composite Cable Status FSM.
*
*/



#include <e32def.h>
#include <e32cmn.h>


#include "pdeconstants.h"
#include "compositecablestatusfsm.h"
#include "compositecablestateidle.h"
#include "trace.h"




// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CCompositeCableStateIdle* CCompositeCableStateIdle::NewL( 
        MFSMForState& aFSMForState )
    {
    FUNC_LOG;
    CCompositeCableStateIdle* self = new ( ELeave ) CCompositeCableStateIdle(
            aFSMForState );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCompositeCableStateIdle::~CCompositeCableStateIdle()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CCompositeCableStateIdle::Enter( )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
void CCompositeCableStateIdle::Input(
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
            iFSMForState.Transit( ECompositeCableStateConnected ); 
            }
        else if ( EPDEIfAccessoryConnectionEventCableDisconnected == aEvent )
            {
            INFO( "Event: EPDEIfAccessoryConnectionEventCableDisconnected" );        
            // Do nothing already in Idle == Disconnected
            }
        else
            {
            INFO_1( "Unknown Event Id: %i", aEvent );        
            }
        break;
        }
    default:
        {
        INFO_1( "Unknown Interface Id: %i", aInterfaceId );        
        break;
        }
    }

    }


// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CCompositeCableStateIdle::Id()
    {
    FUNC_LOG;
    TFSMState tempStateId( ECompositeCableStateIdle );
    INFO_1( "State's id: %i", tempStateId );
    return tempStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TPtrC CCompositeCableStateIdle::Name()
    {
    FUNC_LOG;
    TPtrC temp( KCompositeCableStateIdle );
    INFO_1( "State's name: %s", temp.Ptr() );
    return temp;
    }

// ---------------------------------------------------------------------------
// From MFSMState.
// ---------------------------------------------------------------------------
//
TFSMState CCompositeCableStateIdle::SubId()
    {
    FUNC_LOG;
    TFSMState tempSubstateId( 0 );
    INFO_1( "Substate's id: %i", tempSubstateId );
    return tempSubstateId;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CCompositeCableStateIdle::ConstructL()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CCompositeCableStateIdle::CCompositeCableStateIdle(
        MFSMForState& aFSMForState )
    : iFSMForState( aFSMForState )
    {
    FUNC_LOG;
    }


// ======== GLOBAL FUNCTIONS ========

