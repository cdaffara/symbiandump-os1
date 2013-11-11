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
// Manages the BT GPS device list for the PSY (from the BT GPS Configuration API).  This class only works
// if the BT Device Config List is being used (i.e. the SYMBIAN_LOCATION_BTGPSCONFIG macro is defined).  If
// it is not, all calls to the functions will return KErrNone and will not do anything.
//

#include "btgpsdevicelistmanager.h"
#include "BTGPSLogging.h"

//Constants
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
//Defines the location of where the device should be moved/added to in the device list (i.e. the top of the list) 
const TInt KTopOfDeviceList = 0;
#endif

/** Static constructor
 */
CBTGPSDeviceListManager* CBTGPSDeviceListManager::NewL()
	{
	CBTGPSDeviceListManager* self = new (ELeave) CBTGPSDeviceListManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/** Destructor
 */
CBTGPSDeviceListManager::~CBTGPSDeviceListManager()
	{
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
	delete iDeviceManager;
	iDeviceList.ResetAndDestroy();
#endif
	}

/** Class constructor
 */
CBTGPSDeviceListManager::CBTGPSDeviceListManager()
	{
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
	iCurrentDevicePosition = -1;
#endif
	}

/** Second phase constructor
 */
void CBTGPSDeviceListManager::ConstructL()
	{
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
	TRACESTRING("CBTGPSDeviceListManager::ConstructL start...")
	iDeviceManager = CLbsBtGpsConfigInternal::NewL();
	TRACESTRING("CBTGPSDeviceListManager::ConstructL end...")
#endif
	}

/** Gets the next valid BT GPS device from the cached BT GPS device list

@param aDeviceType [Out] Contains the GPS Device Type
		aAddress [Out] Contains the BT Address for the device
 
@return KErrNone if the operation succeeded.
		KErrNotFound if there are no more devices in the list
		Any other standard Symbian error code if the next device cannot be extracted
 */
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
TInt CBTGPSDeviceListManager::GetNextDevice(TInt& aDeviceType, TBTSockAddr& aAddress)
	{
	TRACESTRING("CBTGPSDeviceListManager::GetNextDevice start...")
		
	TInt err = KErrNone;
	TInt numOfDevices = iDeviceList.Count();
	TBool valid = EFalse;
	
	for(++iCurrentDevicePosition; iCurrentDevicePosition < numOfDevices; ++iCurrentDevicePosition)
		{
		//Get the next device in the list
		const TLbsBtGpsDeviceRecord* deviceInfo = iDeviceList[iCurrentDevicePosition];
		
		//Check that this is a valid device type
		if(deviceInfo->Type() != TLbsBtGpsDeviceInfo::ELbsBtGpsDeviceNonGps)
			{
			//Store the current device key
			iCurrentDeviceKey = deviceInfo->Key();
			
			//Update the device info
			aAddress = deviceInfo->SocketAddress();
			aDeviceType = deviceInfo->Type();
			valid = ETrue;
			
			TRACESTRING("Valid BT GPS Device found in BT Configuration API List")
			TRACESTRING2("DeviceType = %d", aDeviceType)
			TRACESTRING28("Address = %S", &aAddress)
			break;
			}
		//The device is not a valid device, move onto the next device
		}
	
	//Check whether a valid device was found
	if(!valid)
		{
		//No more devices in the list
		TRACESTRING("No more BT GPS devices left in BT Configuration API List")
		err = KErrNotFound;
		}
	
	TRACESTRING("CBTGPSDeviceListManager::GetNextDevice end...")
	return err;
	}
#else
TInt CBTGPSDeviceListManager::GetNextDevice(TInt& /*aDeviceType*/, TBTSockAddr& /*aAddress*/)
	{
	return KErrNone;
	}
#endif

/** Resets the internal list of BT GPS Devices and gets the new list of devices 
 from the CLbsBtGpsConfig Cenrep file.  This should be called when the PSY
 restarts its connecting process.
 
 @return KErrNone if the operation succeeded.
		 Any other standard Symbian error code if the device list cannot be updated
 */
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
TInt CBTGPSDeviceListManager::RefreshDeviceList()
	{
	TRACESTRING("CBTGPSDeviceListManager::ResetDeviceList start...")
	//Reset the RPointerArray
	iDeviceList.ResetAndDestroy();
	iCurrentDevicePosition = -1;
	//Get the list of devices from the BT GPS Device Configuration API
	TRAPD(err, iDeviceManager->GetDeviceRecordListL(iDeviceList));
	
	//Devices are updated using the device key, so reset key to prevent incorrect updating
	iCurrentDeviceKey = KLbsBtGpsEntryKeyNull;
	
	TRACESTRING2("CBTGPSDeviceListManager::ResetDeviceList end...with err: %d", err)
	return err;
	}
#else
TInt CBTGPSDeviceListManager::RefreshDeviceList()
	{
	return KErrNone;
	}
#endif

/** Updates the device list with the device past into this function.  This function
 is used for updating a device that is already in the list,.  If the device is not in the
 list (client deleted device), this does not change the list
 
 @param aDeviceType [In] Contains the GPS Device Type
		 aAddress [In] BT Device Address
 
 @return KErrNone if the operation succeeded.
		KErrNotFound if the device is no longer in the list
		 Any other standard Symbian error code if the device list cannot be updated
 */
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
TInt CBTGPSDeviceListManager::SetDeviceInfo(TInt aDeviceType, const TBTSockAddr& aAddress)
	{
	TRACESTRING("CBTGPSDeviceListManager::SetDeviceInfo start...")
		
	TInt err = KErrNone;
	
	//Update the current device
	err = UpdateDevice(aDeviceType, aAddress);
	
	TRACESTRING2("CBTGPSDeviceListManager::SetDeviceInfo end...with err: %d", err)
	return err;
	}
#else
TInt CBTGPSDeviceListManager::SetDeviceInfo(TInt /*aDeviceType*/, const TBTSockAddr& /*aAddress*/)
	{
	return KErrNone;
	}
#endif

/** Adds a device to the device list.  If the device is already in the list the
 function does nothing extra - it could potentially remove extra information
 that is already stored about the device.  Device type is set to Unknown.
 
 @param aAddress [In] BT Device Address
 
 @return KErrNone if the operation succeeded.
		 KErrAlreadyExists if the device address is already in the list
		 Any other standard Symbian error code if the device list cannot be updated
 */
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
TInt CBTGPSDeviceListManager::AddDeviceInfo(const TBTDevAddr& aAddress)
	{
	TInt err = KErrNone;
	
	TBTSockAddr socketAddress;
	socketAddress.SetBTAddr(aAddress);
	
	//Add the new device to the list
	err = AddDevice(socketAddress);
	
	return err;
	}
#else
TInt CBTGPSDeviceListManager::AddDeviceInfo(const TBTDevAddr& /*aAddress*/)
	{
	return KErrNone;
	}
#endif


//---------------------------------------------------------------
//Helper functions, which are only used when the macro is defined
//---------------------------------------------------------------
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG

/** Updates a device in the BT Device List with this new information
 */
TInt CBTGPSDeviceListManager::UpdateDevice(TInt aDeviceType, const TBTSockAddr& aAddress)
	{
	TRACESTRING("CBTGPSDeviceListManager::UpdateDevice start...")
	
	TInt err = KErrNone;
	
	//Update the BT Device Type and the BT Address/Socket information
	TLbsBtGpsDeviceRecord device;
	device.SetType(aDeviceType);
	device.SetSocketAddress(aAddress);
	device.SetKey(iCurrentDeviceKey);
	err = iDeviceManager->UpdateDevice(device);
		
	if(err == KErrNone)
		{
		//If the device was updated, move it to the top of the list
		err = iDeviceManager->ReorderDevice(iCurrentDeviceKey, KTopOfDeviceList);
		}
	
	TRACESTRING2("CBTGPSDeviceListManager::UpdateDevice end...with err: %d", err)
	return err;
	}

/** Adds a new device to the device list
 */
TInt CBTGPSDeviceListManager::AddDevice(const TBTSockAddr& aAddress)
	{
	TRACESTRING("CBTGPSDeviceListManager::AddDevice start...")
	
	TInt err = KErrNone;
	
	//Extract the BT Device address
	TBTDevAddr deviceAddress(aAddress.BTAddr());
	
	//Always update the current device key even if the add fails to ensure the previous
	// device is not updated accidently
	iCurrentDeviceKey = KLbsBtGpsEntryKeyNull;
	
	//Add the new device to the list
	err = iDeviceManager->AddDevice(deviceAddress, KTopOfDeviceList, iCurrentDeviceKey);
	
	TRACESTRING2("CBTGPSDeviceListManager::AddDevice end...with err: %d", err)
	return err;
	}
#endif //SYMBIAN_LOCATION_BTGPSCONFIG
