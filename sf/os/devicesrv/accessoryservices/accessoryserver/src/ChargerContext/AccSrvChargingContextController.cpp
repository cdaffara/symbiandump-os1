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
* Description:  Charging Context Controller for checking desired conditions 
*                and acting according them in order to ease device's usability
*                during charging. 
*
*/


#include <e32def.h>
#include <e32cmn.h>
#include <hwrmpowerstatesdkpskeys.h>


#include "AccSrvChargingContextController.h"
#include "acc_debug.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CAccSrvChargingContextController* CAccSrvChargingContextController::NewL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::NewL()" );
    CAccSrvChargingContextController* self = new ( ELeave ) CAccSrvChargingContextController();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvChargingContextController::NewL - return %p", self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAccSrvChargingContextController::~CAccSrvChargingContextController()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::~CAccSrvChargingContextController()" );
    delete iFSMBody;
    iFSMBody = NULL;
    delete iResetInactivityTimeTimerPtr;
    iResetInactivityTimeTimerPtr = NULL;
    delete iChargerStatusWatchPtr;
    iChargerStatusWatchPtr = NULL;
    delete iChannelFinder;
    iChannelFinder = NULL;
    delete iDisplayOrientationChannelPtr;
    iDisplayOrientationChannelPtr = NULL;
#ifdef __MODULE_TEST__
    delete iModuleTestOrientationWatchPtr;
    delete iModuleTestChargerWatchPtr;
#endif //__MODULE_TEST__ 
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::~CAccSrvChargingContextController - return" );
    }


// *** Non-derived function:




//*** Derived function:


//---------------------------------------------------------------------------
// From MResetActivityTimeTimerObserver.
//---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::InactivityTimerTimeout()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::InactivityTimerTimeout()" );   
    iResetInactivityTimeTimerElapsed = ETrue;
    iFSMBody->Input();
    iResetInactivityTimeTimerElapsed = EFalse;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::InactivityTimerTimeout - return void" );
    return;
    }


// ---------------------------------------------------------------------------
// From MSenssrvDataListener.
// ---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::DataReceived( CSensrvChannel& aChannel, 
                               TInt /* aCount*/, 
                               TInt /*aDataLost*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::DataReceived()" );
    if ( aChannel.GetChannelInfo().iChannelType == TSensrvOrientationData::KDataTypeId )
        {
        GetCurrentOrientation();
        iFSMBody->Input();
        }  
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::DataReceived - return void" );
    return;
    }


// ---------------------------------------------------------------------------
// From MSenssrvDataListener.
// ---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::DataError(
         CSensrvChannel& aChannel, 
         TSensrvErrorSeverity aError )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::DataError()" );
    // This should not happen according to expert(s).
    
    // Just to get rid of a compiler warning "unused argument aError".
    // As you can see actual usage is inside a trace macro of which usage depends
    // on compiler switch (_DEBUG).
    TInt tempError( aError ); 
    if ( aChannel.GetChannelInfo().iChannelType == TSensrvOrientationData::KDataTypeId )
        {
        if ( KErrNone != tempError )
            {
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvChargingContextController::DataError - aError %d", tempError );
            tempError = KErrNone;
            }
        }  
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::DataError return void" );
    return;
    }


// ---------------------------------------------------------------------------
// from base class MAccSrvPSIntPropertyObserver.
// ---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::PSPropertyChanged( 
        TUid    aCategory,
        TInt    aKey,
        TInt    /* aNewStatus */ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::PSPropertyChanged()" );

#ifdef __MODULE_TEST__
    if ( ( KModuleTestUid == aCategory ) && ( KModuleTestKeyCharger == aKey ) )
        {
        GetCurrentChargerStatus();
        }
    else if ( ( KModuleTestUid == aCategory ) && ( KModuleTestKeyOrientation == aKey ) )
        {
        GetCurrentOrientation();        
        }
#endif //__MODULE_TEST__ 

    if ( (  KPSUidHWRMPowerState == aCategory ) && ( KHWRMChargingStatus == aKey ) )
        {
        GetCurrentChargerStatus();
        }
    iFSMBody->Input();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::PSPropertyChanged - return void" );
    return;
    }


// ---------------------------------------------------------------------------
// from base class MAccSrvPSIntPropertyObserver.
// ---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::PSPropertyDeleted(
        TUid    aCategory,
        TInt    aKey)
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::PSPropertyDeleted()" );
    if ( (  KPSUidHWRMPowerState == aCategory ) && ( KHWRMChargingStatus == aKey ) )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::PSPropertyDeleted - charger status deleted!");        
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::PSPropertyDeleted - return void" );
    return;
    }


// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CAccSrvChargingContextController::CAccSrvChargingContextController()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::CAccSrvChargingContextController()" );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::CAccSrvChargingContextController - return" );
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::ConstructL()    
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::ConstructL()" );
    iFSMBody = CChargingContextFSMBody::NewL( this );
    iFSMBody->Start();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::ConstructL - return void" );
    return;
    }


// ---------------------------------------------------------------------------
// GetCurrentOrientation
// ---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::GetCurrentOrientation()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::GetCurrentOrientation()" );
#ifdef __MODULE_TEST__
    TInt value( 0 );
    iModuleTestOrientationWatchPtr->GetCurrentValue( value );
    iCurrentDeviceOrientation = static_cast< TSensrvOrientationData::TSensrvDeviceOrientation >( value );
#else // __MODULE_TEST__
    TSensrvOrientationData data;
    TPckg<TSensrvOrientationData> dataPckg( data );
    TInt error( iDisplayOrientationChannelPtr->GetData( dataPckg ) );
    if( KErrNone == error )
        {
        if ( iCurrentDeviceOrientation != data.iDeviceOrientation )
            {
            // Store current orientation
            iCurrentDeviceOrientation = data.iDeviceOrientation;
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvChargingContextController::GetCurrentOrientation - new orientation %i", iCurrentDeviceOrientation );
            }
        }
    else
        {
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvChargingContextController::GetCurrentOrientation - error %d", error );       
        }
#endif // __MODULE_TEST__
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::GetCurrentOrientation - return void" );
 
    return;
    }


// ---------------------------------------------------------------------------
// GetCurrentOrientation
// ---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::GetCurrentChargerStatus()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::GetCurrentChargerStatus()" );
#ifdef __MODULE_TEST__
    iModuleTestChargerWatchPtr->GetCurrentValue( iCurrentChargerStatus );
#else // __MODULE_TEST__
    iChargerStatusWatchPtr->GetCurrentValue( iCurrentChargerStatus );
#endif // __MODULE_TEST__
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvChargingContextController::GetCurrentChargerStatus - charger status %i", iCurrentChargerStatus );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::GetCurrentChargerStatus - return void" );

    return;
    }

//------------------------------------------------------------------------------
// StartOrientationChannel
//------------------------------------------------------------------------------
//
void CAccSrvChargingContextController::StartOrientationChannel() 
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::StartOrientationChannel" );
    if( !iOrientationChannelStarted )
        {
        // Orientation channel not started, start it
        TRAPD( err, StartOrientationChannelL() );
        if( err == KErrNone )
            {
            iOrientationChannelStarted = ETrue;
            }
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvChargingContextController::StartOrientationChannel - start orientation channel with code %d", err );
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::StartOrientationChannel - return void" );
    }

//------------------------------------------------------------------------------
// StartOrientationChannelL
//------------------------------------------------------------------------------
//
void CAccSrvChargingContextController::StartOrientationChannelL() 
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::StartOrientationChannelL" );
    iDisplayOrientationChannelPtr->OpenChannelL();
    GetCurrentOrientation();
    iDisplayOrientationChannelPtr->StartDataListeningL( this,
        1, //aDesiredCount is one, i.e. each double tapping is notified separately
        1, //aMaximumCount is one, i.e. object count in receiving data buffer is one
        0 ); //buffering period is not used    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::StartOrientationChannelL - return void" );
    }

//------------------------------------------------------------------------------
// StopOrientationChannel
//------------------------------------------------------------------------------
//
void CAccSrvChargingContextController::StopOrientationChannel() 
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::StopOrientationChannel" );
    if( iOrientationChannelStarted )
        {
        iDisplayOrientationChannelPtr->StopConditionListening();
        iDisplayOrientationChannelPtr->CloseChannel();
        iOrientationChannelStarted = EFalse;
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::StopOrientationChannel - return void" );
    }

// ---------------------------------------------------------------------------
// GetDataListenerInterfaceL
// ---------------------------------------------------------------------------
//
void CAccSrvChargingContextController::GetDataListenerInterfaceL( 
		TUid /*aInterfaceUid*/, 
    TAny*& aInterface )
    {
    aInterface = NULL;
    }

void CAccSrvChargingContextController::MaintainActivity()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::MaintainActivity()" );
    User::ResetInactivityTime();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::MaintainActivity - return void" );
    }

// ---------------------------------------------------------------------------
// CurrentStateID
// ---------------------------------------------------------------------------
//
EXPORT_C TFSMState CAccSrvChargingContextController::CurrentStateID()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::CurrentStateID()" );
    TFSMState currentState( iFSMBody->CurrentStateID() );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::CurrentStateID" );
    return currentState;
    }

// ---------------------------------------------------------------------------
// CurrentStateName
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CAccSrvChargingContextController::CurrentStateName()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::CurrentStateName()" );
    TPtrC currentStateName;
    currentStateName.Set( iFSMBody->CurrentStateName() );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvChargingContextController::CurrentStateName" );
    return currentStateName;
    }


// ======== GLOBAL FUNCTIONS ========
