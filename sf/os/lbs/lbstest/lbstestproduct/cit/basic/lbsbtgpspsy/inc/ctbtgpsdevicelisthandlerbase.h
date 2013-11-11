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
// Base test step handler for the BT GPS Device List configuration
//

#if (!defined __CT_BTGPS_DEVICE_LISTHANDLER_STEP_H__)
#define __CT_BTGPS_DEVICE_LISTHANDLER_STEP_H__

//Includes
#include <test/TestExecuteStepBase.h>
#include <bt_sock.h>
#include "lbsbtgpstestconfig.h"

/** Data storage for the config ini file past in with the test step
 */
struct TBTGPSDeviceConfigData
	{
	//Num of devices that should be in the list
	TInt iNumOfDevices;
	//List of available devices that should be in the list
	TPtrC iAvailableDevices;
	//List of positions of the available devices
	TPtrC iDevicePositions;
	//List of device types for all the devices in the list
	TPtrC iDeviceTypes;
	//Device ID of the device to use in the standard BT device selection dialog
	TPtrC iDeviceSelection;
	};

/** Class used to configure the BT GPS device list.  It reads in
an ini file that is past into it from the test script and uses
this information to enter the BT GPS devices into the BT GPS
configuration API device list.
 */
class CT_BTGPSDeviceListHandler : public CTestStep
	{
public:
	CT_BTGPSDeviceListHandler();
	~CT_BTGPSDeviceListHandler();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
protected:
	//Function to generate random device address, used for unavailable devices
	TBTDevAddr GenerateUniqueBTDevAddr();
	
	void SetupBTDeviceConfigFileL();
	
	//Loads the BT Devices config file
	void LoadBTDeviceConfigFileL(const TDesC& aFilePath);
	
	//Allows access to the Device List Manager
	CLbsBtGpsConfigInternal* DeviceListManager();
	
	//Reads the number of devices that should be in the device list
	void ReadDeviceListSetupConfigDataL();
	
	//Returns the next device that should be in the list
	TInt GetNextDeviceDetails(TInt aCurrentDeviceNum, TBTSockAddr& aSocketAddress, TInt& aDeviceType);
	
	//Returns the device type of the next device that should be in the list
	TInt GetNextDeviceType();
	
	//Reads the device selection from the ini file and sets the notifier up with the right address
	TInt HandleDeviceSelection();
	
protected:
	//Contains all the config information for the device list generation from the ini file
	TBTGPSDeviceConfigData iDeviceListConfig;
	
private:
	//BT GPS Configuration Device List API
	CLbsBtGpsConfigInternal* iDeviceListManager;
		
	//Unique ID for generating a random BT device address
	TInt iUniqueAddr;
	
	//BT Device List ini file
	CIniData* iBTListConfigFile;
	
	//Active Scheduler
	CActiveScheduler* iScheduler;
	
	//GetNextDeviceType list variables for remembering where we are in the list
	TBool iCheckDeviceType;
	TInt iDeviceTypeBufferLength;
	
	//GetNextDevice list variables for remembering where we are in the list
	TBool iCheckAvailableDevice;
	TInt iAvailableDeviceBufferLength;
	
	};

#endif	//__CT_BTGPS_DEVICE_LISTHANDLER_STEP_H__
