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
#include "BTGPSInitManager.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSConnectManager.h"
#include "BTGPSSettingManager.h"
#include "BTGPSNokGpsDetector.h"
#include "BTGPSNokGpsInitialiser.h"
#include "BTGPSNormalGpsInitialiser.h"
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
// CBTGPSInitManager::NewL
// -----------------------------------------------------------------------------
CBTGPSInitManager* CBTGPSInitManager::NewL(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSDeviceManager& aDeviceManager,
        CBTGPSSettingManager& aSettingManager)
    {
    CBTGPSInitManager* self = new (ELeave) CBTGPSInitManager(
        aConnectManager,
        aDeviceManager,
        aSettingManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSInitManager::~CBTGPSInitManager
// -----------------------------------------------------------------------------
CBTGPSInitManager::~CBTGPSInitManager()
    {
    iDeviceManager.RemoveListener(*this);
    delete iNokGpsDetector;
    delete iNokGpsInitialiser;
    delete iNormalGpsInitialiser;
    }


// -----------------------------------------------------------------------------
// CBTGPSInitManager::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSInitManager::ConstructL()
    {
    iDeviceManager.AddListenerL(*this);
    iNokGpsDetector = CBTGPSNokGpsDetector::NewL(
        iConnectManager,
        iDeviceManager);
    }

// -----------------------------------------------------------------------------
// CBTGPSInitManager::CBTGPSInitManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSInitManager::CBTGPSInitManager(
        CBTGPSConnectManager& aConnectManager,
        CBTGPSDeviceManager& aDeviceManager,
        CBTGPSSettingManager& aSettingManager)
    : iConnectManager(aConnectManager),
      iDeviceManager(aDeviceManager),
      iSettingManager(aSettingManager)
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSInitManager::BTDeviceStatusChanged
// -----------------------------------------------------------------------------
void CBTGPSInitManager::BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt /*aErr*/)
    {
    TRACESTRING("CBTGPSInitManager::BTDeviceStatusChanged start...")
    //Start initialisation when device is connected but unintialized
    if(aConnectStatus == EBTDeviceConnected)
        {
        switch(aDeviceType)
            {
            case EBTDeviceTypeUnknown:
                iNokGpsDetector->Detect(); //error ignore.
                break;
            case EBTDeviceTypeNokGps:
                {
                
                delete iNokGpsInitialiser;
                iNokGpsInitialiser = NULL;
                
                iNokGpsInitialiser = CBTGPSNokGpsInitialiser::New(
                    iConnectManager,
                    iSettingManager,
                    iNokGpsInitialiser);
                }
                break;
            case EBTDeviceTypeNonNokGps:
                {
                //Detect the if the conected BT device support
                //PNOK compatible message
                iNokGpsDetector->Detect(); //error ignore.
                
                delete iNormalGpsInitialiser;
                iNormalGpsInitialiser = NULL;
                
                iNormalGpsInitialiser = CBTGPSNormalGpsInitialiser::New(
                    iConnectManager,
                    iNormalGpsInitialiser);
                }
                break;
            case EBTDeviceTypeNonGPS:
                //Not GPS device
                break;
            default:
                //other events are ignored
                break;
            }
        }
    else
        {
        //If the device is not in connected status then cancel init operation
        iNokGpsDetector->Cancel();
        
        delete iNokGpsInitialiser;
        iNokGpsInitialiser = NULL;

        delete iNormalGpsInitialiser; 
        iNormalGpsInitialiser = NULL;
        }
    TRACESTRING("CBTGPSInitManager::BTDeviceStatusChanged end")
    }

//  End of File



