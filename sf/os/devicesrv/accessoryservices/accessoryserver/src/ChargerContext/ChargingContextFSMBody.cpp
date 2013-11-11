/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Finite State Machine class for Charging Context Controller.
*
*/



#include <e32def.h>
#include <e32cmn.h>
#include <hwrmpowerstatesdkpskeys.h>


#include "ChargingContextFSMBody.h"
#include "ChargingContextStateWFCharger.h"
#include "ChargingContextStateCharging.h"
#include "ChargingContextStateMaintainActivity.h"
#include "acc_debug.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CChargingContextFSMBody* CChargingContextFSMBody::NewL(
        CAccSrvChargingContextController* aChargingContextController )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::NewL()" );

    CChargingContextFSMBody* self = new ( ELeave ) CChargingContextFSMBody();
    CleanupStack::PushL( self );
    self->ConstructL( aChargingContextController );
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AccServer] CChargingContextFSMBody::NewL - return %p", self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CChargingContextFSMBody::~CChargingContextFSMBody()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::~CChargingContextFSMBody()" );
    iCurrentStateId = EChargerContextStateUndefined;
    if ( NULL != iChargingContextController )
        {
        if ( NULL != iChargingContextController->iDisplayOrientationChannelPtr )
            {
            iChargingContextController->iDisplayOrientationChannelPtr->StopDataListening();
            iChargingContextController->iDisplayOrientationChannelPtr->CloseChannel();
            }
        if ( NULL != iChargingContextController->iChargerStatusWatchPtr )
            {
            if ( iChargingContextController->iChargerStatusWatchPtr->IsActive() )
                {
                iChargingContextController->iChargerStatusWatchPtr->Cancel();
                }
            }
        }
    // Delete state objects
    for (TInt i = 1; i < EChargerContextStateMaxValue; i++ )
        {
        delete iStateArray[i];
        iStateArray[i] = NULL;
        }   
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::~CChargingContextFSMBody" );
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CChargingContextFSMBody::Start()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::Start()" );
    // Step to the initial state.
    iCurrentStateId = EChargerContextStateWaitForCharger;
    iStateArray[iCurrentStateId]->Enter();
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::Start" );
    return;
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TBool CChargingContextFSMBody::Transit( TFSMState aNextState )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::Transit()" );
    TBool retVal(EFalse);
    if ( ( EChargerContextStateUndefined < aNextState ) && 
         (  EChargerContextStateMaxValue > aNextState ) )
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
        COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::Transit - incorrect nextstate");
        }   
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextFSMBody::Transit - TBool retVal %i", retVal );
    return retVal;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TFSMState CChargingContextFSMBody::CurrentStateID()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::CurrentStateID()" );
    TFSMState currentState( EChargerContextStateUndefined );
    if ( ( EChargerContextStateUndefined < iCurrentStateId ) && 
            ( EChargerContextStateMaxValue >= iCurrentStateId ))
        {
        currentState = iCurrentStateId;
        }
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextFSMBody::CurrentStateID - CurrentStateId %i", currentState );
    return currentState;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CChargingContextFSMBody::Input()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::Input()" );
    if ( EChargerContextStateUndefined != iCurrentStateId )
        {
        if ( NULL != iStateArray[iCurrentStateId] )
            {
            iStateArray[iCurrentStateId]->Input();
            }
        }  
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::Input" );
    return;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
MFSMState* CChargingContextFSMBody::CurrentState()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::CurrentState()" );
    MFSMState* currentState( NULL );
    if ( ( EChargerContextStateUndefined < iCurrentStateId ) && 
            ( EChargerContextStateMaxValue > iCurrentStateId ))
        {
        currentState = iStateArray[iCurrentStateId];
        }
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::CurrentState" );
    return currentState;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TPtrC CChargingContextFSMBody::CurrentStateName()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::CurrentStateName()" );
    TPtrC currentStateName;
    currentStateName.Set( KNullDesC );
    if ( ( EChargerContextStateUndefined < iCurrentStateId ) && 
         ( EChargerContextStateMaxValue > iCurrentStateId ))
        {
        currentStateName.Set( iStateArray[iCurrentStateId]->Name() );
        }
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::CurrentStateName" );
    return currentStateName;
    }

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CChargingContextFSMBody::CChargingContextFSMBody()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::CChargingContextFSMBody()" );
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::CChargingContextFSMBody - return" );
    }


// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CChargingContextFSMBody::ConstructL(
        CAccSrvChargingContextController* aChargingContextController )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::ConstructL()" );
    iChargingContextController = aChargingContextController;
    // Create state objects here.
    iStateArray[ EChargerContextStateWaitForCharger ] = CChargingContextStateWFCharger::NewL( this, aChargingContextController );
    iStateArray[ EChargerContextStateCharging ] = CChargingContextStateCharging::NewL( this, aChargingContextController );
    iStateArray[ EChargerContextStateMaintainActivity ] = CChargingContextStateMaintainActivity::NewL( this, aChargingContextController );

#ifdef __MODULE_TEST__
    aChargingContextController->iModuleTestOrientationWatchPtr = CAccSrvPSIntPropertyWatch::NewL(
            aChargingContextController, 
            KModuleTestUid, 
            KModuleTestKeyOrientation, 
            EOwnerThread );
    aChargingContextController->iModuleTestOrientationWatchPtr->Watch();
    aChargingContextController->iModuleTestChargerWatchPtr = CAccSrvPSIntPropertyWatch::NewL(
            aChargingContextController, 
            KModuleTestUid, 
            KModuleTestKeyCharger, 
            EOwnerThread );
    aChargingContextController->iModuleTestChargerWatchPtr->Watch();
#endif //__MODULE_TEST__ 

    // Create Charger status watcher.
    aChargingContextController->iChargerStatusWatchPtr = CAccSrvPSIntPropertyWatch::NewL(
            aChargingContextController, 
            KPSUidHWRMPowerState, 
            KHWRMChargingStatus, 
            EOwnerThread );
    aChargingContextController->GetCurrentChargerStatus();
    aChargingContextController->iChargerStatusWatchPtr->Watch();

    // Create ResetInactivityTime timer.    
    aChargingContextController->iResetInactivityTimeTimerPtr = CAccSrvResetInactivityTimeTimer::NewL(
            aChargingContextController );
    // Create channel finder watcher.    
    aChargingContextController->iChannelFinder = CSensrvChannelFinder::NewL();
    
    //List of found channels.
    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL( channelInfoList );

     //Create and fill channel search criteria.
    //Here the device orientation channel is searched.
    TSensrvChannelInfo channelInfo;
    channelInfo.iChannelType = TSensrvOrientationData::KDataTypeId;

    //Find the orientation channel.
    aChargingContextController->iChannelFinder->FindChannelsL( channelInfoList, channelInfo );
    
    if( channelInfoList.Count() != 1 )
        {
        //The device doesn’t support orientation channel or
        //there are several orientation channels.
        COM_TRACE_1( "[AccFW:AccServer] CChargingContextFSMBody::ConstructL - User::Leave( error ) %d", KErrNone );
        User::Leave( KErrNotFound);
        }

    //Open the orientation channel.
    //When the channel object is created the channel info object 
    //must be an object returned by CSensrvChannelFinder::FindChannelsL().
    aChargingContextController->iDisplayOrientationChannelPtr = CSensrvChannel::NewL( channelInfoList[ 0 ] );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextFSMBody::ConstructL - iDisplayOrientationChannelPtr %p", aChargingContextController->iDisplayOrientationChannelPtr );
   
    CleanupStack::PopAndDestroy( &channelInfoList ); //Close() is being called on "channelInfoList"
    
    COM_TRACE_( "[AccFW:AccServer] CChargingContextFSMBody::ConstructL - return void" );
    return;
   }

// ======== GLOBAL FUNCTIONS ========

