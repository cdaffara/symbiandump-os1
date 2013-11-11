// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



// INCLUDE FILES
#include <e32std.h>
#include "BTGPSNokNightModeHandler.h"
#include "BTGPSConnectManager.h"
#include "BTGPSMessageDef.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
// Timer interval for sending setting night mode message
const TInt KBTGPSTimerIntervalSendingNightModeSetting = 5000000; //5 seconds

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSNokNightModeHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSNokNightModeHandler* CBTGPSNokNightModeHandler::NewL(
        CBTGPSConnectManager& aConnectManager,
        TBTGPSSettingsApi::TNightModeState aNightModeStatus)
    {
    CBTGPSNokNightModeHandler* self = new (ELeave) CBTGPSNokNightModeHandler(
        aConnectManager,
        aNightModeStatus);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSNokNightModeHandler::~CBTGPSNokNightModeHandler
// -----------------------------------------------------------------------------
CBTGPSNokNightModeHandler::~CBTGPSNokNightModeHandler()
    {
    if(iTimer!=NULL)
        {
        iTimer->Cancel();
        delete iTimer;
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSNokNightModeHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSNokNightModeHandler::ConstructL()
    {
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iTimer->Start(
        KBTGPSTimerIntervalSendingNightModeSetting,
        KBTGPSTimerIntervalSendingNightModeSetting,
        TCallBack(TimerCallback, this));
        
    SetNightModeSetting();
    }

// -----------------------------------------------------------------------------
// CBTGPSNokNightModeHandler::CBTGPSNokNightModeHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSNokNightModeHandler::CBTGPSNokNightModeHandler(
        CBTGPSConnectManager& aConnectManager,
        TBTGPSSettingsApi::TNightModeState aNightModeStatus)
    : iConnectManager(aConnectManager),
      iNightModeStatus(aNightModeStatus)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSNokNightModeHandler::TimerTick
// -----------------------------------------------------------------------------
void CBTGPSNokNightModeHandler::TimerTick()
    {
    SetNightModeSetting();
    }

// -----------------------------------------------------------------------------
// CBTGPSNokNightModeHandler::TimerCallback
// -----------------------------------------------------------------------------
TInt CBTGPSNokNightModeHandler::TimerCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSNokNightModeHandler*>(aAny)->TimerTick();
    return 1; //to be called again
    }

// -----------------------------------------------------------------------------
// CBTGPSNokNightModeHandler::SetNightModeSetting
// -----------------------------------------------------------------------------
void CBTGPSNokNightModeHandler::SetNightModeSetting()
    {
    if(iNightModeStatus == TBTGPSSettingsApi::ENightModeOn)
        {
        //Night mdoe on
        iConnectManager.SendMessage(KNMEAPNOKNightModeOn);
        }
    else
        {
        //Night mode off
        iConnectManager.SendMessage(KNMEAPNOKNightModeOff);
        }
    }

//  End of File



