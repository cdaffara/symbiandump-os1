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
// ECheckDevicePairing state in the PSY Connecting State Machine; checks to
// see whether the current device is paired with
//

#include <e32std.h>
#include <bt_sock.h>
#include "btgpscheckdevicepairing.h"
#include "BTGPSDeviceManager.h"
#include "BTGPSConnectManagerExt.h"
#include "BTGPSHandlerFactory.h"
#include "BTGPSLogging.h"

//Constants
const TInt KBTGPSDeviceArrayGranularity = 1;

/** Static constructor
 */
CBTGPSCheckDevicePairing* CBTGPSCheckDevicePairing::NewL(MBTGPSConnectManagerExt& aManagerExt)
	{
	CBTGPSCheckDevicePairing* self = new (ELeave) CBTGPSCheckDevicePairing(aManagerExt);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor
 */
CBTGPSCheckDevicePairing::~CBTGPSCheckDevicePairing()
	{
	if(iIdle)
		{
		iIdle->Cancel();
		delete iIdle;
		}
	if(iDeviceArray)
		{
        iDeviceArray->ResetAndDestroy();
        delete iDeviceArray;
        }
	delete iBtDevMan;
	}

/** Class constructor
 */
CBTGPSCheckDevicePairing::CBTGPSCheckDevicePairing(MBTGPSConnectManagerExt& aManagerExt)
: iManagerExt(aManagerExt),
iError(KErrNone)
	{
	}

/** Second phase constructor
 */
void CBTGPSCheckDevicePairing::ConstructL()
	{
	TRACESTRING("CBTGPSCheckDevicePairing::ConstructL start...")
	
	//construct idle object
	iIdle = CIdle::NewL(CActive::EPriorityStandard);
	
	TBTDeviceType deviceType = iManagerExt.DeviceManager().DeviceType();
	
	if(deviceType == EBTDeviceTypeNonNokGps || deviceType == EBTDeviceTypeNokGps)
		{
		//Check to see whether a pairing exists
		TRACESTRING("Checking device pairing...")
		
		//construct BT device manager
		iBtDevMan = CBTGPSDevMan::NewL(this);
		iDeviceArray = new (ELeave) RBTDeviceArray(KBTGPSDeviceArrayGranularity);
		
		//Get paired devices
		TBTRegistrySearch searchPattern;
		searchPattern.FindBonded();
		iBtDevMan->GetDevices(searchPattern, iDeviceArray);
		}
	else
		{
		//Device type is unknown, so the PSY has not connected to this device before,
		// so do not check the device pairing, always go through the pairing process
		TRACESTRING("Unknown device, starting pairing process...")
		iError = KErrNotFound;
		iIdle->Start(TCallBack(HandlerCompleteCallback, this));
		}
	
	TRACESTRING("CBTGPSCheckDevicePairing::ConstructL end...")
	}

/** Callback function which is called when a list of the currently paired devices has
 been compiled.  This function loops through the array past into the function to see if
 the device we are currently using is in this list of paired devices.  If it is, the
 device is paired with.
 */
void CBTGPSCheckDevicePairing::HandleGetDevicesComplete(TInt aErr, RBTDeviceArray* aDeviceArray)
	{
	TRACESTRING2("CBTGPSCheckDevicePairing::HandleGetDevicesComplete start...%d", aErr)
	TInt err = KErrNotFound;
	
	if((aErr == KErrNone) && (aDeviceArray))
		{
		TInt count = aDeviceArray->Count();
		TBTSockAddr deviceAddr;
		iManagerExt.DeviceManager().BtSockAddr(deviceAddr);
		
		//Loop through the devices to see if the current device is paired
		for (TInt i = 0; i < count; i++)
			{
			const TBTDevAddr& dev = (*aDeviceArray)[i]->BDAddr();
			if(dev == deviceAddr.BTAddr())
				{
				//BT Device is paired, return with no error
				err = KErrNone;
				iManagerExt.DeviceManager().SetPaired(ETrue);
				
				break;
				}
			}
		}
	
	TRACESTRING2("CBTGPSCheckDevicePairing::HandleGetDevicesComplete end...%d", err)
	
	if(err != KErrNone)
		{
		//Device pairing does not exist for this device, so set device type back to unknown
		iManagerExt.DeviceManager().SetBTDeviceType(EBTDeviceTypeUnknown);
		}
	
	iError = err;
	iIdle->Start(TCallBack(HandlerCompleteCallback, this));
	}


void CBTGPSCheckDevicePairing::HandleAddDeviceComplete(TInt /*aErr*/)
	{
	//Do Nothing
	}


void CBTGPSCheckDevicePairing::HandleDeleteDevicesComplete(TInt /*aErr*/)
	{
	//Do Nothing
	}

/** CIdle Handler Complete Callback
 */
TInt CBTGPSCheckDevicePairing::HandlerCompleteCallback(TAny* aAny)
    {
    reinterpret_cast<CBTGPSCheckDevicePairing*>(aAny)->HandlerCompleteNotify();
        
    return KErrNone;
    }

/** Handler Complete
 */
void CBTGPSCheckDevicePairing::HandlerCompleteNotify()
    {
    iManagerExt.HandlerComplete(ECheckDevicePairing, iError);
    }

