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
#include "ChargingContextStateCharging.h"
#include "acc_debug.h"


_LIT(KChargingContextStateCharging, "ST_CHARGING");


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CChargingContextStateCharging* CChargingContextStateCharging::NewL( 
        MFSMBody* aFSMBody, 
        CAccSrvChargingContextController* aChargingContextController )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::NewL()" );
    CChargingContextStateCharging* self = new ( ELeave ) CChargingContextStateCharging();
    CleanupStack::PushL( self );
    self->ConstructL( aFSMBody, aChargingContextController );
    CleanupStack::Pop( self );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateCharging::NewL - return %p", self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CChargingContextStateCharging::~CChargingContextStateCharging()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::~CChargingContextStateCharging()" );
    iFSMBody = NULL;
    iChargingContextController = NULL;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::~CChargingContextStateCharging" );
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CChargingContextStateCharging::Enter( )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::Enter()" );
    
    // Start orientation channel
    iChargingContextController->StartOrientationChannel();
    
    // Check the initial state and transit to new state is needed
    if  ( TSensrvOrientationData::EOrientationDisplayUp == iChargingContextController->iCurrentDeviceOrientation ) 
        {
        // Display is already up.
        Input();
        }
    
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::Enter" );   
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CChargingContextStateCharging::Input()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::Input()" );
     
    if (    ( EChargingStatusError == iChargingContextController->iCurrentChargerStatus ) 
          ||( EChargingStatusNotConnected == iChargingContextController->iCurrentChargerStatus )  ) 
        {
        // Charger has been disconnected!
        // Next state id ST_WF_CHARGER
        iFSMBody->Transit( EChargerContextStateWaitForCharger );
        }       
    else if ( TSensrvOrientationData::EOrientationDisplayUp == iChargingContextController->iCurrentDeviceOrientation )
        {
        // Next state id ST_MAINTAINACTIVITY
        iFSMBody->Transit( EChargerContextStateMaintainActivity );
        }

    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::Input - return void" );   
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TFSMState CChargingContextStateCharging::Id()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::Id()" );
    TFSMState tempStateId( EChargerContextStateCharging );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateCharging::Id - return %i", tempStateId );
    return tempStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TPtrC CChargingContextStateCharging::Name()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::Name()" );
    TPtrC temp( KChargingContextStateCharging );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateCharging::Name - return %s", temp.Ptr() );
    return temp;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CChargingContextStateCharging::ConstructL(
        MFSMBody* aFSMBody, 
        CAccSrvChargingContextController* aChargingContextController )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::ConstructL()" );
    iFSMBody = aFSMBody;
    iChargingContextController = aChargingContextController;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::ConstructL" );
    return;
    }


// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CChargingContextStateCharging::CChargingContextStateCharging()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::CChargingContextStateCharging()" );
    iFSMBody = NULL;
    iChargingContextController = NULL;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateCharging::CChargingContextStateCharging" );   
    }


// ======== GLOBAL FUNCTIONS ========

