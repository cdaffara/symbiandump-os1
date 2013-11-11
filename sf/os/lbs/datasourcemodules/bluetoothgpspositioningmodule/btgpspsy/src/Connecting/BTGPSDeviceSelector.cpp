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
#include <btdevice.h>
#include "BTGPSDeviceManager.h"
#include "BTGPSDeviceSelector.h"
#include "BTGPSConnectManagerExt.h"
#include "BTGPSHandlerFactory.h"
#include "BTGPSLogging.h"
#include "btgpsdevicelistmanager.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
//SerialPort
const TInt KSerialPortServiceClass = 0x1101;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSDeviceSelector::NewL
// -----------------------------------------------------------------------------
CBTGPSDeviceSelector* CBTGPSDeviceSelector::NewL(
        MBTGPSConnectManagerExt& aManagerExt)
    {
    CBTGPSDeviceSelector* self = new (ELeave) CBTGPSDeviceSelector(
        aManagerExt);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceSelector::~CBTGPSDeviceSelector
// -----------------------------------------------------------------------------
CBTGPSDeviceSelector::~CBTGPSDeviceSelector()
    {
    Cancel();
    iNotifier.Close();
    }


// -----------------------------------------------------------------------------
// CBTGPSDeviceSelector::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSDeviceSelector::ConstructL()
    {
    TRACESTRING("CBTGPSDeviceSelector::ConstructL start...")
    //If the device is still coneected, then disconnect it
    iManagerExt.DisconnectBtDevice();
    
    //Set device type as unkown
    iManagerExt.DeviceManager().SetBTDeviceType(EBTDeviceTypeUnknown);
    
    //Connect to notifier server
    User::LeaveIfError(iNotifier.Connect());

    //Request a device selection 
    TBTDeviceSelectionParamsPckg selectionFilter;
    
    //Set UUID
    selectionFilter().SetUUID(KSerialPortServiceClass);

	//Apply No filter: To support BC - Some older Nokia (or other) devices do not specify
	//one or other Class causing filter out if filtered for Positioning
	//Major Service Class, Major Device Class and Minor device class unspecified
	TBTDeviceClass selectionDeviceClass(
        0,
        0,
        0); 

    //Set device class
    selectionFilter().SetDeviceClass(selectionDeviceClass);

    iNotifier.StartNotifierAndGetResponse(
        iStatus, 
        KDeviceSelectionNotifierUid, 
        selectionFilter, 
        iBTSelectionResponse
        );

    SetActive();
    TRACESTRING("CBTGPSDeviceSelector::ConstructL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceSelector::CBTGPSDeviceSelector
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSDeviceSelector::CBTGPSDeviceSelector(
        MBTGPSConnectManagerExt& aManagerExt)
    : CActive(EPriorityStandard),
      iManagerExt(aManagerExt)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceSelector::RunL
// -----------------------------------------------------------------------------
void CBTGPSDeviceSelector::RunL()
    {
    TRACESTRING2("CBTGPSDeviceSelector::RunL start...%d", iStatus.Int())
    //Selection Complete
    if(iStatus == KErrNone)
        {
        iManagerExt.DeviceManager().BtDevice().SetDeviceAddress(
            iBTSelectionResponse().BDAddr());
        iManagerExt.DeviceManager().BtDevice().SetDeviceClass(
            iBTSelectionResponse().DeviceClass());
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
        //Add this device to the list
        iManagerExt.DeviceListManager().AddDeviceInfo(iBTSelectionResponse().BDAddr());
#endif
        
        //Name field is ignored
        }
    iManagerExt.HandlerComplete(EDeviceSelector,iStatus.Int());
    TRACESTRING("CBTGPSDeviceSelector::RunL end")
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceSelector::DoCancel
// -----------------------------------------------------------------------------
void CBTGPSDeviceSelector::DoCancel()
    {
    iNotifier.CancelNotifier(KDeviceSelectionNotifierUid);
    }

// -----------------------------------------------------------------------------
// CBTGPSDeviceSelector::RunError
// -----------------------------------------------------------------------------
TInt CBTGPSDeviceSelector::RunError(TInt /*aError*/)
    {
    //Leaves in RunL. Should not happen
    return KErrNone;
    }

//  End of File



