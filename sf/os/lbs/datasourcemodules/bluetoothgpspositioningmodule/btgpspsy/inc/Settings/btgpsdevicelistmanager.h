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



/**
 @file
 @internalTechnology
*/

#ifndef BTGPSDEVICELISTMANAGER_H
#define BTGPSDEVICELISTMANAGER_H

//Includes
#include <e32std.h>
#include <bt_sock.h>
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
#include "lbsbtgpsdevicerecord.h"
#include "lbsbtgpsconfiginternal.h"
#endif

/** LBS BT GPS Device List Manager

This is the interface between the BT PSY and the BT GPS Configuration API
which stores a list of BT Devices. This class stores an internal cache of 
these devices and allows the PSY to retrieve, add and edit the BT devices.

@internalTechnology
*/
class CBTGPSDeviceListManager: public CBase
	{
public:
	static CBTGPSDeviceListManager* NewL();
	~CBTGPSDeviceListManager();
	
	//Functions used to edit the BT GPS Device list. Note, these functions only work
	// if the BT GPS Device list is being used and the macro SYMBIAN_LOCATION_BTGPSCONFIG is defined.
	TInt GetNextDevice(TInt& aDeviceType, TBTSockAddr& aAddress);
	TInt RefreshDeviceList();
	TInt SetDeviceInfo(TInt aDeviceType, const TBTSockAddr& aAddress);
	TInt AddDeviceInfo(const TBTDevAddr& aAddress);
	
private:
	CBTGPSDeviceListManager();
	void ConstructL();
	
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
private:
	//Helper functions
	TInt UpdateDevice(TInt aDeviceType, const TBTSockAddr& aAddress);
	TInt AddDevice(const TBTSockAddr& aAddress);
	
private:
	//Cached List of the BT GPS Devices
	RPointerArray<TLbsBtGpsDeviceRecord> iDeviceList;
		
	//BT GPS Device Configuration Manager
	CLbsBtGpsConfigInternal* iDeviceManager;
		
	//Device position in the cached device array
	TInt iCurrentDevicePosition;
		
	//Current Device Key that the PSY is dealing with
	TLbsBtGpsEntryKey iCurrentDeviceKey;
#endif	//SYMBIAN_LOCATION_BTGPSCONFIG
	};

#endif	//BTGPSDEVICELISTMANAGER_H
