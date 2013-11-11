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
#include <bt_sock.h>
#include "BTGPSInitHandler.h"
#include "BTGPSConnectManagerExt.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSHandlerFactory.h"
#include "BTGPSLogging.h"
#include "btgpsdevicelistmanager.h"

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
// CBTGPSInitHandler::NewL
// -----------------------------------------------------------------------------
CBTGPSInitHandler* CBTGPSInitHandler::NewL(
        MBTGPSConnectManagerExt& aManagerExt)
    {
    CBTGPSInitHandler* self = new (ELeave) CBTGPSInitHandler(
        aManagerExt);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSInitHandler::~CBTGPSInitHandler
// -----------------------------------------------------------------------------
CBTGPSInitHandler::~CBTGPSInitHandler()
    {
    iManagerExt.DeviceManager().RemoveListener(*this);
    }


// -----------------------------------------------------------------------------
// CBTGPSInitHandler::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSInitHandler::ConstructL()
    {
    TRACESTRING("CBTGPSInitHandler::ConstructL start...")
    iManagerExt.DeviceManager().AddListenerL(*this);
    }

// -----------------------------------------------------------------------------
// CBTGPSInitHandler::CBTGPSInitHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSInitHandler::CBTGPSInitHandler(
        MBTGPSConnectManagerExt& aManagerExt)
    : iManagerExt(aManagerExt)
    {
    }


// -----------------------------------------------------------------------------
// CBTGPSInitHandler::BTDeviceStatusChanged
// -----------------------------------------------------------------------------
void CBTGPSInitHandler::BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt /*aErr*/)
    {
    TRACESTRING("CBTGPSInitHandler::BTDeviceStatusChanged start...")
    TInt err = KErrNone;
    
    if(aConnectStatus == EBTDeviceConnected &&
        aDeviceType == EBTDeviceTypeNonGPS)
        {
        //Device is initialized and not a GPS device
        err = KErrNotSupported;
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
        //Update the device type to set the device as invalid
        TBTSockAddr deviceAddress;
        iManagerExt.DeviceManager().BtSockAddr(deviceAddress);
        iManagerExt.DeviceListManager().SetDeviceInfo(aDeviceType, deviceAddress);
#endif
        }
    else if(aConnectStatus == EBTDeviceConnected && 
        ( aDeviceType == EBTDeviceTypeNonNokGps || 
        aDeviceType == EBTDeviceTypeNokGps ))
        {
        //BT device is GPS device
        err = KErrNone;
        }
    else if(aConnectStatus != EBTDeviceConnected)
        {
        //connection lost
        err = KErrCancel;
        }
    else
        {
        //do nothing
        return;
        }
    iManagerExt.HandlerComplete(EInitHandler, err);
    TRACESTRING("CBTGPSInitHandler::BTDeviceStatusChanged end")
    }
        
    


//  End of File



