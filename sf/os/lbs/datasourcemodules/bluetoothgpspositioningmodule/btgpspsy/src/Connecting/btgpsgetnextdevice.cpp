// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EGetNextDevice state in the PSY Connecting State Machine; should return
// the next device from the BT GPS Configuration API
//

#include <e32std.h>
#include <bt_sock.h>
#include "btgpsgetnextdevice.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSConnectManagerExt.h"
#include "BTGPSHandlerFactory.h"
#include "btgpsdevicelistmanager.h"
#include "BTGPSLogging.h"

/** Static constructor
 */
CBTGPSGetNextDevice* CBTGPSGetNextDevice::NewL(MBTGPSConnectManagerExt& aManagerExt)
	{
	CBTGPSGetNextDevice* self = new (ELeave) CBTGPSGetNextDevice(aManagerExt);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor
 */
CBTGPSGetNextDevice::~CBTGPSGetNextDevice()
	{
	if(iIdle)
		{
		iIdle->Cancel();
		delete iIdle;
		}
	}

/** Class constructor
 */
CBTGPSGetNextDevice::CBTGPSGetNextDevice(MBTGPSConnectManagerExt& aManagerExt)
: iManagerExt(aManagerExt),
iError(KErrNone)
	{
	}

/** Second phase constructor
 */
void CBTGPSGetNextDevice::ConstructL()
	{
	TRACESTRING("CBTGPSGetNextDevice::ConstructL start...")
	
	TInt tempDeviceType;
	TBTSockAddr deviceAddress;
	
	//Set the PSY state back to Connecting, as we are now dealing with a new device
	iManagerExt.DeviceManager().SetBTDeviceConnectStatus(EBTDeviceConnecting);
	
	//construct idle object
	iIdle = CIdle::NewL(CActive::EPriorityStandard);
	
	//Get the next valid device from the BT Device List
	iError = iManagerExt.DeviceListManager().GetNextDevice(tempDeviceType, deviceAddress);
	
	if(iError == KErrNone)
		{
		//Valid device in the list, update the device info manager
		TBTDeviceType deviceType = static_cast<TBTDeviceType>(tempDeviceType);
		iManagerExt.DeviceManager().SetDeviceInfo(deviceAddress, deviceType);
		}
	
	iIdle->Start(TCallBack(HandlerCompleteCallback, this));
	
	TRACESTRING2("CBTGPSGetNextDevice::ConstructL end...with err: %d", iError)
	}

/** CIdle Handler Complete Callback. This should be called after the ConstrucL
 function has completed.  This allows the object to finish construction and allows
 the object to continue its operation.
 */
TInt CBTGPSGetNextDevice::HandlerCompleteCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSGetNextDevice*>(aAny)->HandlerCompleteNotify();
        
    return KErrNone;
    }

/** Handler Complete
 */
void CBTGPSGetNextDevice::HandlerCompleteNotify()
    {
    iManagerExt.HandlerComplete(EGetNextDevice, iError);
    }
