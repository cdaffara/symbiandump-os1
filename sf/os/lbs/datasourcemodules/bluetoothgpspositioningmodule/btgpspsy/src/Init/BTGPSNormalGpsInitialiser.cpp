// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "BTGPSNormalGpsInitialiser.h"
#include "BTGPSMessageDef.h"
#include "BTGPSConnectManager.h"
#include "BTGPSConstantsManager.h"
#include "BTGPSRequestHandler.h"
#include "BTGPSLogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

/**
*  Normal BT GPS intialisation steps
*/
enum TBTGPSNormalGpsInitStep
    {
    ENormalGpsInitSetFullPower,
    ENormalGpsInitSetWantedMsg
    };
    
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSNormalGpsInitialiser::NewL
// -----------------------------------------------------------------------------
CBTGPSNormalGpsInitialiser* CBTGPSNormalGpsInitialiser::NewL(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSNormalGpsInitialiser*& aOwner)
    {
    CBTGPSNormalGpsInitialiser* self = new (ELeave) CBTGPSNormalGpsInitialiser(
        aConnectManager,
        aOwner);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSNormalGpsInitialiser::New
// -----------------------------------------------------------------------------
CBTGPSNormalGpsInitialiser* CBTGPSNormalGpsInitialiser::New(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSNormalGpsInitialiser*& aOwner)
    {
    CBTGPSNormalGpsInitialiser* self = NULL;
    TInt err(KErrNone);
    TRAP(err, self = CBTGPSNormalGpsInitialiser::NewL(aConnectManager, aOwner));
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSNormalGpsInitialiser::~CBTGPSNormalGpsInitialiser
// -----------------------------------------------------------------------------
CBTGPSNormalGpsInitialiser::~CBTGPSNormalGpsInitialiser()
    {
    if(iTimer!=NULL)
        {
        iTimer->Cancel();
        delete iTimer;
        }
    iOwner = NULL;
    }


// -----------------------------------------------------------------------------
// CBTGPSNormalGpsInitialiser::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSNormalGpsInitialiser::ConstructL()
    {
    TRACESTRING("CBTGPSNormalGpsInitialiser::ConstructL start...")
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iStep = ENormalGpsInitSetFullPower;
    iConnectManager.SendMessage(KNMEAToNormalPower);
    iCount++;
    iTimer->Start(
        CBTGPSRequestHandler::ConstantsManager().iDelayBetweenGpsInitMessages,
        CBTGPSRequestHandler::ConstantsManager().iDelayBetweenGpsInitMessages,
        TCallBack(TimerCallback, this));
    TRACESTRING("CBTGPSNormalGpsInitialiser::ConstructL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSNormalGpsInitialiser::CBTGPSNormalGpsInitialiser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSNormalGpsInitialiser::CBTGPSNormalGpsInitialiser(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSNormalGpsInitialiser*& aOwner)
    : iConnectManager(aConnectManager),
      iOwner(aOwner)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSNormalGpsInitialiser::TimerTick
// -----------------------------------------------------------------------------
void CBTGPSNormalGpsInitialiser::TimerTick()
    {
    if(iStep == ENormalGpsInitSetFullPower &&
        iCount>=
        CBTGPSRequestHandler::ConstantsManager().iNumberOfSetFullPowerMessage)
        {
        iStep++;
        iCount = 0;
        }
    
    switch(iStep)
        {
        case ENormalGpsInitSetFullPower:
            {
            iConnectManager.SendMessage(KNMEAToNormalPower);
            iCount++;
            }
            break;
        case ENormalGpsInitSetWantedMsg:
            iConnectManager.SendMessage(KNMEASelectedMessages);
            iStep++;
            delete this;
            break;
        default:
            //should never be called
            delete this;
            break;
        }
    }
        
// -----------------------------------------------------------------------------
// CBTGPSNormalGpsInitialiser::TimerCallback
// -----------------------------------------------------------------------------
TInt CBTGPSNormalGpsInitialiser::TimerCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSNormalGpsInitialiser*>(aAny)->TimerTick();
    return KErrNone;
    }

//  End of File



