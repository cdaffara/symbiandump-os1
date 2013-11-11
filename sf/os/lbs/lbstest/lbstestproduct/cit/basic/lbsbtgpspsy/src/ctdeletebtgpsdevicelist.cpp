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
// Configuration test step used for deleting devices from the device neighbourhood
//

#include "ctdeletebtgpsdevicelist.h"
#include <bt_sock.h>
#include "btgpssim.h"

/** This test step is used as a configuration test step to delete the BT GPS
devices from the BT GPS neighbourhood. The test gets the list of devices from
the RBTGPSSim class and deletes them all.  It also deletes all of the devices
used in the device notification and clears the device notifier list.

So this config step can be used after running a BT GPS PSY test to delete all of
the devices in the list, to ensure it does not affect the next test.
To run this, add the following entry to the script:

	RUN_TEST_STEP 100 ct_lbsbtgpspsysuite DeleteBTGPSDeviceList
 */

/** Constructor
 */
CT_DeleteBTGPSDeviceList::CT_DeleteBTGPSDeviceList()
	{
	SetTestStepName(KDeleteDeviceConfigList);
	}

/** Destructor
 */
CT_DeleteBTGPSDeviceList::~CT_DeleteBTGPSDeviceList()
	{
	}


/** Actual test step implementation, which deletes all the devices in
 the BT device neighbourhood and the device notifier list

@return - TVerdict code
 */
TVerdict CT_DeleteBTGPSDeviceList::doTestStepL()
	{
	if(TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		TInt masterErr = KErrNone;
		
		//Connect to the SIM
		RBtGpsSim sim;
		err = sim.Connect();
		
		if(err == KErrNone)
			{
			//Get the number of devices currently in the neighbourhood
			TInt numOfDevices = 0;
			sim.DeviceCount(numOfDevices);
			INFO_PRINTF2(_L("Trying to delete %d devices"), numOfDevices);
			
			//Loop through all of the devices in the list deleting them
			for(TInt deviceNum = (numOfDevices-1); deviceNum >= 0; --deviceNum)
				{
				RBtGpsSimDevice simDevice;
				err = simDevice.Open(sim, deviceNum);
				if(err == KErrNone)
					{
					//Try and delete the device
					err = simDevice.Remove();
					if(err != KErrNone)
						{
						//Keep on attempting to delete the other devices in the list, but store this latest error code
						masterErr = err;
						INFO_PRINTF3(_L("Error, Unable to delete device %d, with error: %d"), deviceNum, err);
						}
					}
				else
					{
					//Keep on attempting to delete the other devices in the list, but store this latest error code
					masterErr = err;
					INFO_PRINTF3(_L("Error, failed to find device %d, with error: %d"), deviceNum, err);
					}
				simDevice.Close();
				}
			
			//Delete all of the devices in the device notifier list
			err = sim.ClearSelectionList();
			if(err != KErrNone)
				{
				INFO_PRINTF2(_L("Error, failed to clear device selection list with error %d"), err);
				}
			}
		
		sim.Close();
		
		//Check the result
		if((err != KErrNone) || (masterErr != KErrNone))
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
