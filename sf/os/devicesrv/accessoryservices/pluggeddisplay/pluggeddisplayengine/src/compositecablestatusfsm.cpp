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
* Description:  Finite State Machine class for Composite Cable Status.
*
*/


#include "pdeconstants.h"
#include "accessoryconnectionif.h"
#include "compositecablestateidle.h"
#include "compositecablestateconnected.h"
#include "compositecablestatusfsm.h"
#include "trace.h"
#include <centralrepository.h>
#include <accessoriescrkeys.h>
#include <tvoutconfig.h>


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CCompositeCableStatusFSM* CCompositeCableStatusFSM::NewL(
        RAccessoryServer& aAccessoryServer, CPDEngine *aPdEngine )
    {
    FUNC_LOG;
    CCompositeCableStatusFSM* self = new ( ELeave ) CCompositeCableStatusFSM();
    CleanupStack::PushL( self );
    self->ConstructL( aAccessoryServer, aPdEngine );
    CleanupStack::Pop( self );    
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCompositeCableStatusFSM::~CCompositeCableStatusFSM()
    {
    FUNC_LOG;
    iCurrentStateId = ECompositeCableStateUndefined;
    if ( iAccessoryConnectionIf )
        {
        iAccessoryConnectionIf->Cancel();
        delete iAccessoryConnectionIf;
        }
    if ( iTVOutConfigForComposite )
        {
        iTVOutConfigForComposite->Cancel();
        delete iTVOutConfigForComposite;
        }
    // Delete state objects
    for (TInt i = 0; i < ECompositeCableStateMaxValue; i++ )
        {
        delete iStateArray[i];
        }   
    }


// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
void CCompositeCableStatusFSM::Start()
    {
    FUNC_LOG;
    // Step to the initial state.
    iCurrentStateId = ECompositeCableStateIdle;
    iStateArray[iCurrentStateId]->Enter();
    }


// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
TFSMId CCompositeCableStatusFSM::FSMID()
    {
    FUNC_LOG;
    TFSMId aFSMID( EPDEFSMIdCompositeCableStatus );
    return aFSMID;    
    }

// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
TFSMState CCompositeCableStatusFSM::CurrentStateID()
    {
    FUNC_LOG;
    TFSMState currentState( ECompositeCableStateUndefined );
    if ( ( 0 <= iCurrentStateId ) && 
            ( ECompositeCableStateMaxValue > iCurrentStateId ))
        {
        currentState = iCurrentStateId;
        }
    INFO_1( "iCurrentStateId: %i", iCurrentStateId );
    return currentState;
    }


//------------------------------------------------------------------------------
// From MPdeFSM.
//------------------------------------------------------------------------------
//
TBool CCompositeCableStatusFSM::ProcessCommandL( const TProcessCmdId /*aCommand*/,
    const TASYCmdParams& /* aCmdParams*/,
    MPDAsyCmdHandler& /*aAsyCmdHandler*/ )
    {
    FUNC_LOG;
    TBool commandHandled( EFalse );
    // Current implementation does not receive/accept any ASY commands.
    return commandHandled;
    }

//------------------------------------------------------------------------------
//  From MPdeFSM.
//------------------------------------------------------------------------------
//
TInt CCompositeCableStatusFSM::GetAccPolGenericID( TAccPolGenericID& aAccPolGenericID )
    {
    FUNC_LOG;
    return iAccessoryConnectionIf->GetAccPolGenericID( aAccPolGenericID );
    }


// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
void CCompositeCableStatusFSM::Input(
        TFSMInterfaceId aInterfaceId ,
        TFSMEventId aEvent )
    {
    FUNC_LOG;
    if ( ECompositeCableStateUndefined != iCurrentStateId )
        {
        if ( NULL != iStateArray[iCurrentStateId] )
            {
            iStateArray[iCurrentStateId]->Input( aInterfaceId, aEvent );
            }
        }  
    }

// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
MFSMState* CCompositeCableStatusFSM::CurrentState()
    {
    FUNC_LOG;
    MFSMState* currentState( NULL );
    if (    ( 0 <= iCurrentStateId ) && 
            ( ECompositeCableStateMaxValue > iCurrentStateId ))
        {
        currentState = iStateArray[iCurrentStateId];
        }
    return currentState;
    }

// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
TPtrC CCompositeCableStatusFSM::CurrentStateName()
    {
    FUNC_LOG;
    TPtrC currentStateName;
    currentStateName.Set( KNullDesC );
    if ( ( 0 <= iCurrentStateId ) && 
         ( ECompositeCableStateMaxValue > iCurrentStateId ))
        {
        if ( NULL != iStateArray[iCurrentStateId] )
             {
             currentStateName.Set( iStateArray[iCurrentStateId]->Name() );
             }
        }
    return currentStateName;
    }

// ---------------------------------------------------------------------------
// From MFSMForBody.
// ---------------------------------------------------------------------------
//
TFSMMainAndSubState CCompositeCableStatusFSM::CurrentMainAndSubstateIDs()
    {
    FUNC_LOG;
    TFSMMainAndSubState state;
    state.iMainState = ECompositeCableStateUndefined;
    state.iSubstate = 0;
    if ( ( 0 <= iCurrentStateId ) && 
            ( ECompositeCableStateMaxValue > iCurrentStateId ))
        {
        state.iMainState = iCurrentStateId;
        state.iSubstate = iStateArray[iCurrentStateId]->SubId();
        }
    INFO_2( "Main state id: %i, substate id: %i", state.iMainState, state.iSubstate );
    return state;
    }

// ---------------------------------------------------------------------------
// From MFSMForState.
// ---------------------------------------------------------------------------
//
TBool CCompositeCableStatusFSM::Transit( TFSMState aNextState )
    {
    FUNC_LOG;
    TBool retVal(EFalse);
    if ( (  0 <= aNextState ) && 
         (  ECompositeCableStateMaxValue > aNextState ) )
        {
        iCurrentStateId = aNextState;
        if ( NULL != iStateArray[iCurrentStateId])
            {
            retVal = ETrue;
            iStateArray[iCurrentStateId]->Enter();
            }
        }
    if ( EFalse == retVal )
        {
        //Something is wrong
        INFO("Incorrect nextstate");
        }   
    INFO_1("TBool retVal %i", retVal);
    return retVal;
    }

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CCompositeCableStatusFSM::CCompositeCableStatusFSM()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CCompositeCableStatusFSM::ConstructL(
        RAccessoryServer& aAccessoryServer, CPDEngine *aPdEngine )
    {
    FUNC_LOG;
    
    iPdEngine = aPdEngine;    
    
    // Create interface objecs
    iAccessoryConnectionIf = CAccessoryConnectionIf::NewL( *this, aAccessoryServer );
    iTVOutConfigForComposite = CTVOutConfigForComposite::NewL( *this );

    // Create state objects here.
    iStateArray[ ECompositeCableStateIdle ] = CCompositeCableStateIdle::NewL( *this );
    iStateArray[ ECompositeCableStateConnected ] = CCompositeCableStateConnected::NewL( *this, *iTVOutConfigForComposite, *this );
    }

// ======== GLOBAL FUNCTIONS ========

