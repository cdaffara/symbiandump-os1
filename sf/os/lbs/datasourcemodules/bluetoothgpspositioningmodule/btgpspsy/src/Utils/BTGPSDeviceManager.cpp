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
#include <btdevice.h>
#include <bt_sock.h>
#include "BTGPSPanic.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSDeviceListener.h"
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
// CBTGPSDeviceManager::NewL
// -----------------------------------------------------------------------------
CBTGPSDeviceManager* CBTGPSDeviceManager::NewL()
    {
    CBTGPSDeviceManager* self = new (ELeave) CBTGPSDeviceManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::~CBTGPSDeviceManager
// -----------------------------------------------------------------------------
CBTGPSDeviceManager::~CBTGPSDeviceManager()
    {
	delete iBtDevice;
    iListenerArray.Close();
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::ConstructL()
    {
    iBtDevice = CBTDevice::NewL();
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::CBTGPSDeviceManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSDeviceManager::CBTGPSDeviceManager()
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::BtDevice
// -----------------------------------------------------------------------------
CBTDevice& CBTGPSDeviceManager::BtDevice()
    {
    return *iBtDevice;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::BtSockAddr
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::BtSockAddr(TBTSockAddr& aAddr) const
    {
    aAddr.SetBTAddr(iBtDevice->BDAddr());
    aAddr.SetPort(iPort);
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::SetDeviceInfo
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::SetDeviceInfo(
		    const TBTSockAddr& aAddr,
		    const TBTDeviceType& aType)
    {
    iBtDevice->SetDeviceAddress(aAddr.BTAddr());
    iPort = aAddr.Port();
    iPaired = EFalse;
    iType = aType;
    StatusChanged();
    }


// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::SetPort
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::SetPort(TInt aPort)
    {
    iPort = aPort;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::Port
// -----------------------------------------------------------------------------
TInt CBTGPSDeviceManager::Port() const
    {
    return iPort;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::IsPaired
// -----------------------------------------------------------------------------
TBool CBTGPSDeviceManager::IsPaired() const
    {
    return iPaired;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::SetPaired
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::SetPaired(TBool aPaired)
    {
    iPaired = aPaired;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::DeviceType
// -----------------------------------------------------------------------------
TBTDeviceType CBTGPSDeviceManager::DeviceType() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::IsGpsConnected
// -----------------------------------------------------------------------------
TBool CBTGPSDeviceManager::IsGpsConnected() const
    {
    return iType==EBTDeviceTypeNonNokGps || iType==EBTDeviceTypeNokGps;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::SetBTDeviceType
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::SetBTDeviceType(TBTDeviceType aType)
    {
    iType = aType;
    StatusChanged();
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::DeviceConnectStatus
// -----------------------------------------------------------------------------
TBTDeviceConnectStatus CBTGPSDeviceManager::DeviceConnectStatus() const
    {
    return iConnectStatus;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::SetBTDeviceConnectStatus
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::SetBTDeviceConnectStatus(
        TBTDeviceConnectStatus aStatus,
        TInt aErr)
    {
    iConnectStatus = aStatus;
    iError = aErr;
    StatusChanged();   
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::AddListenerL
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::AddListenerL(MBTGPSDeviceListener& aListener)
    {
    User::LeaveIfError(iListenerArray.Append(&aListener));
    }


// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::RemoveListener
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::RemoveListener(MBTGPSDeviceListener& aListener)
    {
    TInt index = iListenerArray.Find(&aListener);
    if(index!=KErrNotFound)
        {
        iListenerArray.Remove(index);
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceManager::StatusChanged
// -----------------------------------------------------------------------------
void CBTGPSDeviceManager::StatusChanged()
    {
    TRACESTRING("CBTGPSDeviceManager::StatusChanged start...")
    TRACESTRING2("ConnectStatus:%d", iConnectStatus)
    TRACESTRING2("DeviceType: %d", iType)
    //Broadcast BT device status change to listeners
    TInt count = iListenerArray.Count();
    for(TInt i=count-1; i>=0; i--)
        {
        iListenerArray[i]->BTDeviceStatusChanged(
            iConnectStatus,
            iType,
            iError);
        }
    TRACESTRING("CBTGPSDeviceManager::StatusChanged end")
    }


//  End of File



