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
// Configuration test step used for setting up the BT GPS Device List
// and creating the simulation devices if required
//

#include "ctsetupbtgpsdevicelist.h"
#include <bt_sock.h>
#include "btgpssim.h"

/** This test step is used as a configuration test step to setup the BT GPS
device list before running the actual test. The test simply needs to create
and provide an ini file and pass it into this test step, it will then create
the device list.  For more information about what needs to go into the ini
file to make the device list, see the base step: CT_BTGPSDeviceListHandler
(ctbtgpsdevicelisthandler.cpp)

So this config step can be used before running a BT GPS PSY test to setup the
list of BT devices that the PSY should use.  To run this, add the following entry
to the script:

	RUN_TEST_STEP 100 ct_lbsbtgpspsysuite SetupBTGPSDeviceList c:\testdata\configs\setupdeviceist.ini Test1Pre
 */

/** Constructor
 */
CT_SetupBTGPSDeviceList::CT_SetupBTGPSDeviceList()
	{
	SetTestStepName(KSetupDeviceConfigList);
	}

/** Destructor
 */
CT_SetupBTGPSDeviceList::~CT_SetupBTGPSDeviceList()
	{
	}


/** Actual test step implementation, which extracts the required information
 from the testdata .ini file and creates the BT GPS device list.

@return - TVerdict code
 */
TVerdict CT_SetupBTGPSDeviceList::doTestStepL()
	{
	if(TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		
		//Clear the device list
		DeviceListManager()->EmptyDeviceList();
		
		//Get the config data from the device list ini file
		ReadDeviceListSetupConfigDataL();
		
		//Setup the BT Device List ini file
		SetupBTDeviceConfigFileL();
		
		//Loop through the devices that should be added to the list
		for(TInt deviceNum=0; deviceNum < iDeviceListConfig.iNumOfDevices; ++deviceNum)
			{
			TLbsBtGpsDeviceRecord newDevice;
			TBTSockAddr socketAddress;
			TLbsBtGpsEntryKey key;
			TInt listDeviceType = 0;
			TInt actualDeviceType;
			TBtGpsSimDeviceType simType;
			
			//Get the next socket address
			err = GetNextDeviceDetails(deviceNum, socketAddress, actualDeviceType);
			if(err != KErrNone)
				break;
			//Get the device type of this device
			listDeviceType = GetNextDeviceType();
			
			//If the device is an available device type, add the device to the sim fw
			if(actualDeviceType >= 0)
				{
				simType = static_cast<TBtGpsSimDeviceType>(actualDeviceType);
				
				// Add device to the simulation neighbourhood
				RBtGpsSim sim;
				TInt err = sim.Connect();
				if (err == KErrNone)
					{
					RBtGpsSimDevice simDevice;
					err = simDevice.Open(sim, socketAddress);
					if(err == KErrNotFound)
						{
						err = simDevice.Create(sim, simType, socketAddress);
						if (err == KErrNone)
							{
							simDevice.SetParameter(EBtGpsSimAuthorised, ETrue);
							simDevice.SetParameter(EBtGpsSimPin, _L8("0000"));
							}
						}
					simDevice.Close();
					sim.Close();
					}
				}
			
			//Add the new device to the BT list using the BT GPS Configuration API
			err = DeviceListManager()->AddDevice(socketAddress.BTAddr(), deviceNum, key);
			if(err != KErrNone)
				break;
			
			//Create the device to update in the list
			newDevice.SetKey(key);
			newDevice.SetSocketAddress(socketAddress);
			newDevice.SetType(listDeviceType);
			
			//Update the device to contain the extra information: device Type and Port
			err = DeviceListManager()->UpdateDevice(newDevice);
			if(err != KErrNone)
				break;
			
			}
		
		//Check to ensure the devices were added correctly
		TInt actualNumOfDevices = 0;
		DeviceListManager()->GetDeviceCount(actualNumOfDevices);
		if(actualNumOfDevices != iDeviceListConfig.iNumOfDevices)
			{
			err = KErrCorrupt;
			}
		INFO_PRINTF2(_L("%d Devices have been added to the BT GPS Device List:"), actualNumOfDevices);
		
		//Loop through the devices in the list logging their details
		RPointerArray<TLbsBtGpsDeviceRecord> deviceList;
		DeviceListManager()->GetDeviceRecordListL(deviceList);
		for(TInt deviceNum=0; deviceNum<deviceList.Count(); ++deviceNum)
			{
			TBuf<20> actualAddr;
			deviceList[deviceNum]->SocketAddress().BTAddr().GetReadable(actualAddr);
			
			INFO_PRINTF3(_L("[%S] (%d)"), &actualAddr, deviceList[deviceNum]->Type());
			}
		
		if (err == KErrNone)
			{
			//Device Selection
			err = HandleDeviceSelection();
			}
		
		if(err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		else
			{
			SetTestStepResult(EPass);
			}
		}
	return TestStepResult();
	}
