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
// Configuration test step used for checking the BT GPS Device List
//

#include "ctcheckbtgpsdevicelist.h"
#include <bt_sock.h>

/** This test step is used as a configuration test step to check the BT GPS
device list after running the actual test. The test simply needs to create
and provide an ini file and pass it into this test step, it will then check
to see whether the device list matches the expected device list.  For more 
information about what needs to go into the ini file to make the device list, 
see the base step: CT_BTGPSDeviceListHandler (ctbtgpsdevicelisthandler.cpp)

So this config step can be used after running a BT GPS PSY test to check the
list of BT devices that the PSY has created to see whether it is what was expected.
To run this, add the following entry to the script:

	RUN_TEST_STEP 100 ct_lbsbtgpspsysuite CheckBTGPSDeviceList c:\testdata\configs\setupdeviceist.ini Test1Post
 */

/** Constructor
 */
CT_CheckBTGPSDeviceList::CT_CheckBTGPSDeviceList()
	{
	SetTestStepName(KCheckDeviceConfigList);
	}

/** Destructor
 */
CT_CheckBTGPSDeviceList::~CT_CheckBTGPSDeviceList()
	{
	}


/** Actual test step implementation, which extracts the required information
 from the testdata .ini file and checks it against the BT GPS device list.

@return - TVerdict code
 */
TVerdict CT_CheckBTGPSDeviceList::doTestStepL()
	{
	if(TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		TInt deviceListError = KErrNone;
		TInt deviceTypeError = KErrNone;
		TInt checkDeviceType = 0;
		
		//Get the config data from the device list ini file
		ReadDeviceListSetupConfigDataL();
		
		//Setup the BT Device List ini file
		SetupBTDeviceConfigFileL();
		
		//Check to see whether the device type should be checked
		if(!GetIntFromConfig(_L("BTDevices"), _L("CheckDeviceType"), checkDeviceType))
			{
			checkDeviceType = 0;
			}
		
		//Get the list of BT devices from the Configuration API
		RPointerArray<TLbsBtGpsDeviceRecord> actualDeviceList;
		TRAPD(error, DeviceListManager()->GetDeviceRecordListL(actualDeviceList));
		if(error != KErrNone)
			{
			//Could not get the list of actual devices, fail the test
			INFO_PRINTF2(_L("ERROR - Could not get the list of BT devices from the Configuration API List: %d"), error);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		
		//First, check to ensure the correct number of devices are in the list
		TInt actualNumOfDevices = 0;
		DeviceListManager()->GetDeviceCount(actualNumOfDevices);
		if(actualNumOfDevices != iDeviceListConfig.iNumOfDevices)
			{
			INFO_PRINTF3(_L("ERROR - Expected number of devices (%d) does not match actual number (%d)"), 
					iDeviceListConfig.iNumOfDevices , actualNumOfDevices);
			SetTestStepResult(EFail);
			}
		
		INFO_PRINTF1(_L("Actual Device List vs Expected Device List [Address] (Device Type):"));
		//This loops through both the expected and the actual BT GPS device lists.  Even if the number of devices in 
		// the two lists do not match, it will still print the whole contents of both lists.
		for(TInt deviceNum=0; ((deviceNum < iDeviceListConfig.iNumOfDevices) || (deviceNum < actualNumOfDevices)); ++deviceNum)
			{
			TBTSockAddr actualSocketAddress;
			TBTSockAddr expectedSocketAddress;
			TInt actualDeviceType = -1;
			TInt expectedDeviceType = -2;
			TBuf<20> actualAddr(_L("None"));
			TBuf<20> expectedAddr(_L("None"));
			
			//Get the actual BT Address details
			if(deviceNum < iDeviceListConfig.iNumOfDevices)
				{
				TInt deviceTypeTemp;
				err = GetNextDeviceDetails(deviceNum, actualSocketAddress, deviceTypeTemp);
				if(err == KErrNone)
					{
					//Get the device type of this device
					actualDeviceType = GetNextDeviceType();
				
					actualSocketAddress.BTAddr().GetReadable(expectedAddr);
					}
				}
			
			//Get the expected BT Address Details
			if(deviceNum < actualNumOfDevices)
				{
				expectedSocketAddress = actualDeviceList[deviceNum]->SocketAddress();
				actualDeviceList[deviceNum]->SocketAddress().BTAddr().GetReadable(actualAddr);
				expectedDeviceType = actualDeviceList[deviceNum]->Type();
				}
			
			//Log the address that is in the list, to allow comparison in the logs
			INFO_PRINTF5(_L("[%S] (%d) - [%S] (%d)"), &actualAddr, expectedDeviceType, &expectedAddr, actualDeviceType);

			//Check to see whether the real and expected devices match
			if(actualSocketAddress.BTAddr() != expectedSocketAddress.BTAddr())
				{
				deviceListError = KErrArgument;
				}
			if(actualDeviceType != expectedDeviceType)
				{
				deviceTypeError = KErrArgument;
				}
			}
		
		//Check to see whether the lists are the same
		if(deviceListError != KErrNone)
			{
			INFO_PRINTF1(_L("FAIL - Expected List of devices does not match actual list of devices"));
			SetTestStepResult(EFail);
			}
		else if((deviceTypeError != KErrNone) && (checkDeviceType == 1))
			{
			INFO_PRINTF1(_L("FAIL - Expected Device Type does not match the actual device types"));
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF1(_L("SUCCESS - Expected List of devices matches actual list of devices"));
			SetTestStepResult(EPass);
			}
		}
	return TestStepResult();
	}
