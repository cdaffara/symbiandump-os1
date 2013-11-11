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



#include <hwrmpowerstatesdkpskeys.h>


#include "ChargingContextFSMBody.h"
#include "ChargingContextStateWFCharger.h"
#include "acc_debug.h"


_LIT(KChargingContextStateWFCharger, "ST_WF_CHARGER");


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CChargingContextStateWFCharger* CChargingContextStateWFCharger::NewL( 
        MFSMBody* aFSMBody, 
        CAccSrvChargingContextController* aChargingContextController )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::NewL()" );
    CChargingContextStateWFCharger* self = new ( ELeave ) CChargingContextStateWFCharger();
    CleanupStack::PushL( self );
    self->ConstructL( aFSMBody, aChargingContextController );
    CleanupStack::Pop( self );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateWFCharger::NewL - return %p", self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CChargingContextStateWFCharger::~CChargingContextStateWFCharger()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::~CChargingContextStateWFCharger()" );
    iFSMBody = NULL;
    iChargingContextController = NULL;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::~CChargingContextStateWFCharger" );
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CChargingContextStateWFCharger::Enter( )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::Enter()" );
    // Call Input() once to check if exit criterias are already met.
    iChargingContextController->StopOrientationChannel();
    Input();
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::Enter" );   
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
void CChargingContextStateWFCharger::Input()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::Input()" );
    if (( EChargingStatusError != iChargingContextController->iCurrentChargerStatus ) 
            && ( EChargingStatusNotConnected != iChargingContextController->iCurrentChargerStatus ) )
        {
        // Next state ST_CHARGING
        iFSMBody->Transit(  EChargerContextStateCharging );
        }
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::Input - return void" );   
    }


// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TFSMState CChargingContextStateWFCharger::Id()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::Id()" );
    TFSMState tempStateId( EChargerContextStateWaitForCharger );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateWFCharger::Id - return %i", tempStateId );
    return tempStateId;
    }

// ---------------------------------------------------------------------------
// From MFSMBody.
// ---------------------------------------------------------------------------
//
TPtrC CChargingContextStateWFCharger::Name()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::Name()" );
    TPtrC temp( KChargingContextStateWFCharger );
    COM_TRACE_1( "[AccFW:AccServer] CChargingContextStateWFCharger::Name - return %s", temp.Ptr() );
    return temp;
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CChargingContextStateWFCharger::ConstructL(
        MFSMBody* aFSMBody, 
        CAccSrvChargingContextController* aChargingContextController )
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::ConstructL()" );
    iFSMBody = aFSMBody;
    iChargingContextController = aChargingContextController;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::ConstructL" );
    return;
    }


// ---------------------------------------------------------------------------
//  C++ constructor
// ---------------------------------------------------------------------------
//
CChargingContextStateWFCharger::CChargingContextStateWFCharger()
    {
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::CChargingContextStateWFCharger()" );
    iFSMBody = NULL;
    iChargingContextController = NULL;
    COM_TRACE_( "[AccFW:AccServer] CChargingContextStateWFCharger::CChargingContextStateWFCharger" );   
    }


// ======== GLOBAL FUNCTIONS ========

