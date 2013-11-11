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
#include "BTGPSPowerManager.h"
#include "BTGPSConnectManager.h"
#include "BTGPSPsyConnectionManager.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSConstantsManager.h"
#include "BTGPSRequestHandler.h"
#include "BTGPSLogging.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSPowerManager::NewL
// -----------------------------------------------------------------------------
CBTGPSPowerManager* CBTGPSPowerManager::NewL(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSPsyConnectionManager& aPsyConnectionManager,
        CBTGPSDeviceManager& aDeviceManager,
        CBTGPSConstantsManager& aConstantsManager)
    {
    CBTGPSPowerManager* self = new (ELeave) CBTGPSPowerManager(
        aConnectManager,
        aPsyConnectionManager,
        aDeviceManager,
        aConstantsManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSPowerManager::~CBTGPSPowerManager
// -----------------------------------------------------------------------------
CBTGPSPowerManager::~CBTGPSPowerManager()
    {
    if(iTimer!=NULL)
        {
        iTimer->Cancel();
        delete iTimer;
        }
    iPsyConnectionManager.RemoveListener(*this);
    }

// -----------------------------------------------------------------------------
// CBTGPSPowerManager::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSPowerManager::ConstructL()
    {
    iPsyConnectionManager.AddListenerL(*this);
    
    //Construct timer
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    }

// -----------------------------------------------------------------------------
// CBTGPSPowerManager::CBTGPSPowerManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSPowerManager::CBTGPSPowerManager(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSPsyConnectionManager& aPsyConnectionManager,
        CBTGPSDeviceManager& aDeviceManager,
        CBTGPSConstantsManager& aConstantsManager)
    : iConnectManager(aConnectManager),
      iPsyConnectionManager(aPsyConnectionManager),
      iDeviceManager(aDeviceManager),
      iConstantsManager(aConstantsManager)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSPowerManager::HandlePsyConnectionChange
// -----------------------------------------------------------------------------
void CBTGPSPowerManager::HandlePsyConnectionChange()
    {
    CheckTimer();
    }

// -----------------------------------------------------------------------------
// CBTGPSPowerManager::BTDeviceStatusChanged
// -----------------------------------------------------------------------------
void CBTGPSPowerManager::BTDeviceStatusChanged(
            TInt /*aConnectStatus*/, 
            TInt /*aDeviceType*/,
            TInt /*aErr*/)
    {
    CheckTimer();
    }

// -----------------------------------------------------------------------------
// CBTGPSPowerManager::CheckTimer
// -----------------------------------------------------------------------------
void CBTGPSPowerManager::CheckTimer()
    {
    if( CBTGPSRequestHandler::ConstantsManager().iUtilisePowerSaving &&
        iPsyConnectionManager.LocationRequestCount()==0 &&
        iDeviceManager.DeviceConnectStatus() == EBTDeviceConnected &&
        iDeviceManager.IsGpsConnected() ) 
        {
        if(!iTimer->IsActive())
            {
            TRACESTRING("CBTGPSPowerManager::CheckTimer timer start")
            iTimer->Start(
                iConstantsManager.iInactivityDelay,
                iConstantsManager.iInactivityDelay,
                TCallBack(TimerCallback, this));
            }
        }
    else
        {
        TRACESTRING("CBTGPSPowerManager::CheckTimer timer cancel")
        iTimer->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSPowerManager::TimerTick
// -----------------------------------------------------------------------------
void CBTGPSPowerManager::TimerTick()
    {
    TRACESTRING("CBTGPSPowerManager::TimerTick start...")
    //close connection
    iTimer->Cancel();
    iConnectManager.CancelConnecting();
    TRACESTRING("CBTGPSPowerManager::TimerTick end")
    }
        
// -----------------------------------------------------------------------------
// CBTGPSPowerManager::TimerCallback
// -----------------------------------------------------------------------------
TInt CBTGPSPowerManager::TimerCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSPowerManager*>(aAny)->TimerTick();
    return KErrNone; 
    }

//  End of File



