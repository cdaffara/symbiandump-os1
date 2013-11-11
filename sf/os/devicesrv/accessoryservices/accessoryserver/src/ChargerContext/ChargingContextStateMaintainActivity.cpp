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
* Description:  ?Description
*
*/



#include <e32def.h>
#include <e32cmn.h>
#include <hwrmpowerstatesdkpskeys.h>


#include "ChargingContextFSMBody.h"
#include "ChargingContextStateMaintainActivity.h"
#include "acc_debug.h"


_LIT(KChargingContextStateMaintainActivity, "ST_MAINTAINACTIVITY");


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CChargingContextStateMaintainActivity* CChargingContextStateMaintainActivity::NewL( 
        MFSMBody* aFSMBody, 
        CAccSrvChargingContextController* aChargingContextController )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::NewL()" );
    CChargingContextStateMaintainActivity* self = new ( ELeave ) CChargingContextStateMaintainActivity();
    CleanupStack::PushL( self );
    self->ConstructL( aFSMBody, aChargingContextController );
    CleanupStack::Pop( self );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateMaintainActivity::NewL - return %p", self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CChargingContextStateMaintainActivity::~CChargingContextStateMaintainActivity()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::~CChargingContextStateMaintainActivity()" );
    if ( iChargingContextController->iResetInactivityTimeTimerPtr->IsActive() )
        {
        iChargingContextController->iResetInactivityTimeTimerPtr->Cancel();
        }
    iFSMBody = NULL;
    iChargingContextController = NULL;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::~CChargingContextStateMaintainActivity" );
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CChargingContextStateMaintainActivity::Enter( )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::Enter()" );
    if ( !iChargingContextController->iResetInactivityTimeTimerPtr->IsActive() )
        {
        iChargingContextController->iResetInactivityTimeTimerPtr->StartTimer();
        }
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::Enter" );   
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CChargingContextStateMaintainActivity::Input()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::Input()" );
    if (    ( EChargingStatusError == iChargingContextController->iCurrentChargerStatus ) 
          ||( EChargingStatusNotConnected == iChargingContextController->iCurrentChargerStatus ) ) 
        {
        // Charger has been disconnected!
        // Stop RestInactivityTime timer.
        iChargingContextController->iResetInactivityTimeTimerPtr->Cancel();            
        // Next state id ST_WF_CHARGER
        iFSMBody->Transit(  EChargerContextStateWaitForCharger );
        }        
    else if ( iChargingContextController->iResetInactivityTimeTimerElapsed )
        {
        iChargingContextController->MaintainActivity();
        iChargingContextController->iResetInactivityTimeTimerPtr->StartTimer();
        }
    else if ( TSensrvOrientationData::EOrientationDisplayUp != iChargingContextController->iCurrentDeviceOrientation )
        {
        // Stop RestInactivityTime timer.
        iChargingContextController->iResetInactivityTimeTimerPtr->Cancel();            
        // Next state id ST_CHARGING
        iFSMBody->Transit( EChargerContextStateCharging );
        }
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::Input - return void" ); 
    return;
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TFSMState CChargingContextStateMaintainActivity::Id()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::Id()" );
    TFSMState tempStateId( EChargerContextStateMaintainActivity );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateMaintainActivity::Id - return %i", tempStateId );
    return tempStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TPtrC CChargingContextStateMaintainActivity::Name()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::Name()" );
    TPtrC temp( KChargingContextStateMaintainActivity );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateMaintainActivity::Name - return %s", temp.Ptr() );
    return temp;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CChargingContextStateMaintainActivity::ConstructL(
        MFSMBody* aFSMBody, 
        CAccSrvChargingContextController* aChargingContextController )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::ConstructL()" );
    iFSMBody = aFSMBody;
    iChargingContextController = aChargingContextController;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::ConstructL" );
    return;
    }


// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CChargingContextStateMaintainActivity::CChargingContextStateMaintainActivity()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::CChargingContextStateMaintainActivity()" );
    iFSMBody = NULL;
    iChargingContextController = NULL;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateMaintainActivity::CChargingContextStateMaintainActivity" );   
    }


// ======== GLOBAL FUNCTIONS ========

