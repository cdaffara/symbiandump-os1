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
 * CPDEngine class implementation.
 *
 */

// SYSTEM INCLUDES
#include <AsyCmdTypes.h>

// USER INCLUDES
#include "pdengine.h"
#include "compositecablestatusfsm.h"
#include "hdmicablestatusfsm.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CPDEngine* CPDEngine::NewL( RAccessoryServer& aAccessoryServer )
    {
    FUNC_LOG;

    CPDEngine* self = CPDEngine::NewLC( aAccessoryServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// Leaves pointer in the cleanup stack.
// ---------------------------------------------------------------------------
//
EXPORT_C CPDEngine* CPDEngine::NewLC( RAccessoryServer& aAccessoryServer )
    {
    FUNC_LOG;

    CPDEngine* self = new ( ELeave ) CPDEngine();
    CleanupStack::PushL( self );
    self->ConstructL( aAccessoryServer );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CPDEngine::~CPDEngine()
    {
    FUNC_LOG;
    // Delete FSM objects
    for (TInt i = 0; i < EPDEFSMIdFirstUnused; i++ )
        {
        delete iFSMPtr[i];
        } 
    }

//------------------------------------------------------------------------------
// CPDEngine::ProcessCommandL
//------------------------------------------------------------------------------
//
EXPORT_C void CPDEngine::ProcessCommandL( const TProcessCmdId aCommand,
    const TASYCmdParams& aCmdParams,
    MPDAsyCmdHandler& aAsyCmdHandler )
    {
    FUNC_LOG;
    for (TInt i = 0; i < EPDEFSMIdFirstUnused; i++ )
        {
        TBool commandHandled = iFSMPtr[i]->ProcessCommandL( 
                aCommand,
                aCmdParams,
                aAsyCmdHandler );
        if ( commandHandled )
            {
            break;
            }
        } 
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TFSMState CPDEngine::CurrentStateID( TFSMId aFSMId )
    {
    FUNC_LOG;
    TFSMState currentStateId( 0 );
    if ( (  0 <= aFSMId ) && 
            ( EPDEFSMIdFirstUnused > aFSMId ))
        {
        if (iFSMPtr[ aFSMId ])
            {
            currentStateId = iFSMPtr[ aFSMId ]->CurrentStateID();
            }
        }
    INFO_1( "currentStateId %i", currentStateId );
    return currentStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CPDEngine::Input(
        TFSMId aFSMId,
        TFSMInterfaceId aInterfaceId,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    if ( (  0 <= aFSMId ) && 
            ( EPDEFSMIdFirstUnused > aFSMId ) )
        {
        if(EPDEFSMIdHDMICableStatus == aFSMId)
            {            
            if( iFSMPtr[EPDEFSMIdCompositeCableStatus]->CurrentStateID() != ECompositeCableStateIdle )
                {
                // Forcefully transit CompositeCable FSM to Idle state
                ((CCompositeCableStatusFSM*)iFSMPtr[EPDEFSMIdCompositeCableStatus])->Input(EPDEIfAccessoryConnection, EPDEIfAccessoryConnectionEventCableDisconnected);
                }
            iFSMPtr[ aFSMId ]->Input( aInterfaceId, aEvent );            
            }
        else if( ( EPDEFSMIdCompositeCableStatus == aFSMId )
                && ( iFSMPtr[EPDEFSMIdHDMICableStatus]->CurrentStateID() != EHDMICableStateConnected ) )
            {
            iFSMPtr[ aFSMId ]->Input( aInterfaceId, aEvent );
            }
        }
    return;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
MFSMState* CPDEngine::CurrentState( TFSMId aFSMId )
    {
    FUNC_LOG;
    MFSMState* currentState( NULL );
    if ( (  0 <= aFSMId ) && 
            ( EPDEFSMIdFirstUnused > aFSMId ))
        {
        if (iFSMPtr[ aFSMId ])
            {
            currentState = iFSMPtr[ aFSMId ]->CurrentState();
            }
        }
    return currentState;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
MFSMForBody*  CPDEngine::GetFSM( TFSMId aFSMId )
    {
    FUNC_LOG;
    MFSMForBody* fsmPtr( NULL );
    if ( (  0 <= aFSMId ) && 
            ( EPDEFSMIdFirstUnused > aFSMId ))
        {
        fsmPtr = iFSMPtr[ aFSMId ];
        }
    return fsmPtr;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TFSMMainAndSubState CPDEngine::CurrentMainAndSubstateIDs( TFSMId aFSMId )
    {
    FUNC_LOG;
    TFSMMainAndSubState states;
     if ( (  0 <= aFSMId ) && 
            ( EPDEFSMIdFirstUnused > aFSMId ))
        {
        states = iFSMPtr[ aFSMId ]->CurrentMainAndSubstateIDs();
        }
    return states;
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TPtrC CPDEngine::CurrentStateName( TFSMId aFSMId )
    {
    FUNC_LOG;
    TPtrC currentStateName;
    currentStateName.Set( KNullDesC );
    if (    ( 0 <= aFSMId ) && 
            ( EPDEFSMIdFirstUnused > aFSMId ))
        {
        if (iFSMPtr[ aFSMId ])
            {
            currentStateName.Set(  iFSMPtr[ aFSMId ]->CurrentStateName() );
            }
        }
    return currentStateName;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CPDEngine::Start()
    {
    FUNC_LOG;
    // Start FSMs
    for (TInt i = 0; i < EPDEFSMIdFirstUnused; i++ )
        {
        if (iFSMPtr[i])
            {
            iFSMPtr[i]->Start();
            }
        }   
    return;
    }

// ---------------------------------------------------------------------------
// CPDEngine::GetUniqueID
// ---------------------------------------------------------------------------
//
TInt CPDEngine::GetAccPolGenericID( 
        TFSMId aFSMId,
        TAccPolGenericID& aAccPolGenericID )
    {
    FUNC_LOG;
    TInt retVal( KErrNotFound );
    if (    ( 0 <= aFSMId ) && 
            ( EPDEFSMIdFirstUnused > aFSMId ))
        {
        retVal = iFSMPtr[ EPDEFSMIdHDMICableStatus ]->GetAccPolGenericID( aAccPolGenericID );
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CPDEngine::CPDEngine()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CPDEngine::ConstructL( RAccessoryServer& aAccessoryServer )
    {
    FUNC_LOG;
    // Create FSM objects here.
    iFSMPtr[ EPDEFSMIdHDMICableStatus ] = CHDMICableStatusFSM::NewL( aAccessoryServer, this );
    iFSMPtr[ EPDEFSMIdCompositeCableStatus ] = CCompositeCableStatusFSM::NewL( aAccessoryServer, this );
    Start();
    }

// End of file
