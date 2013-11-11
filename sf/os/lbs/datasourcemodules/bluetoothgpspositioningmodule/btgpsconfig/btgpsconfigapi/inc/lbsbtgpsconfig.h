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
// LBS Bluetooth GPS Configuration API
//



/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef LBS_BTGPSCONFIG_H
#define LBS_BTGPSCONFIG_H

#include <e32cmn.h> 
#include <e32base.h>
#include <bttypes.h>
#include <lbs/lbsbtgpsconfigdatatypes.h>

class CLbsBtGpsConfigImpl;


/** LBS Bluetooth GPS Config observer
 
Mixin class used to provide notifications of changes to LBS Bluetooth GPS configuration.

@see CLbsBtGpsConfig

@publishedPartner
@prototype
*/
class MLbsBtGpsConfigObserver 
	{
public:

	IMPORT_C virtual TVersion Version() const;
	
	/** Callback to indicate that the configuration has been changed by another user of this API

	e.g. The LBS Bluetooth PSY may update the configuration through this API, moving a connected 
	device to the top of the priority list. 

	The updated list can be read using GetDeviceListL().
	
	@publishedPartner
	@prototype
	*/
	virtual void OnDeviceListUpdate() =0;
	
private:

	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	};


/** LBS Bluetooth GPS Configuration API

API used to configure Bluetooth GPS devices for use with LBS, allowing a preferred list of devices 
to be maintined.  When required, the LBS Bluetooth positioning plugin will attempt to connect to each
device in the list in order, until a succesful connection to a GPS device is made.

Through this API an application can read, add to, remove from, and reorder the list of preferred 
Bluetooth GPS devices. Accessing this API requires the LocalServices capability, in addition, calling 
any methods which can modify the configuration requires the WriteDeviceData capability.

@code
class CLbsBtGpsConfigExample : public CActive, public MLbsBtGpsConfigObserver
	{

public:

	CLbsBtGpsConfigExample() : CActive(CActive::EPriorityStandard)
		{
		CActiveScheduler::Add(this);
		
		// Create an instance of the API
		// NOTE: Our class implements the observer interface MLbsBtGpsConfigObserver
		iConfig = CLbsBtGpsConfig::NewL(*this);

		// ...
		  
		}

	~CLbsBtGpsConfigExample()
		{
		delete iConfig;
		iDeviceList.ResetAndDestroy();
		iDeviceList.Close();
		}
	
	void ExampleCode()
		{
		// Add a new device at the start of the list
		TInt err;
		TBTDevAddr deviceAddress;
		TLbsBtGpsEntryKey key;
		err = iConfig->AddDevice(deviceAddress, 0, key);
		// NOTE: key contains the unique device key, used to identify the device

		// Move the device around in the list
		// This will move it into 8th place (index is zero based), or, if there are 
		// currently less than 8 devices in the list, it will move it to the end. 
		iConfig->ReorderDevice(key, 7);

		// Remove the device from the list
		iConfig->RemoveDevice(key);
		}
	
	virtual void OnDeviceListUpdate()
		{
		test.Printf(_L(" --- OnDeviceListUpdate ---\n"));

		// Retrieve the list of devices
		iDeviceList.ResetAndDestroy();
		iConfig->GetDeviceListL(iDeviceList);

		// Loop through the devices
		for (TInt index=0; index<iDeviceList.Count(); ++index)
			{
			// Get at the device information from the list
			const TLbsBtGpsDeviceInfo* deviceInfo = iDeviceList[index];
			
			// Do something (e.g. display on a GUI)

			}

		// Destroy the retrieved copy of the list now we have finished using it
		iDeviceList.ResetAndDestroy();
		}

private:

	CLbsBtGpsConfig* iConfig;
	RPointerArray<TLbsBtGpsDeviceInfo> iDeviceList;

	// ...
	
	};
@endcode

An application must wait for the RunL of the class implementing the MLbsBtGpsConfigObserver interface 
to return before calling any other public methods on this class. 

@publishedPartner
@prototype
*/
NONSHARABLE_CLASS(CLbsBtGpsConfig) : public CBase
	{
public:

	IMPORT_C static CLbsBtGpsConfig* NewL();
	IMPORT_C static CLbsBtGpsConfig* NewL(MLbsBtGpsConfigObserver& aObserver);
	IMPORT_C virtual ~CLbsBtGpsConfig();

public:

	IMPORT_C TInt GetDeviceCount(TInt& aCount);
	IMPORT_C TInt AddDevice(const TBTDevAddr& aAddress, TInt aPosition, TLbsBtGpsEntryKey& aKey);
	IMPORT_C TInt RemoveDevice(TLbsBtGpsEntryKey aKey);
	IMPORT_C TInt EmptyDeviceList();
	IMPORT_C TInt ReorderDevice(TLbsBtGpsEntryKey aKey, TInt aNewPosition);
	IMPORT_C void GetDeviceListL(RPointerArray<TLbsBtGpsDeviceInfo>& aList);
	
protected:

	CLbsBtGpsConfig();
	void ConstructL(MLbsBtGpsConfigObserver* aObserver);

private:

	/** Prohibit copy constructor */
	CLbsBtGpsConfig(const CLbsBtGpsConfig&);
	/** Prohibit assigment operator */
	CLbsBtGpsConfig& operator= (const CLbsBtGpsConfig&);

	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	
protected:

	/** Implementation pointer */
	CLbsBtGpsConfigImpl* iImpl;
	};

#endif //LBS_BTGPSCONFIG_H
