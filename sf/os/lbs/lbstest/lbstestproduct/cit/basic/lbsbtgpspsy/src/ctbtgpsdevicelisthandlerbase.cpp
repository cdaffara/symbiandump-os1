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
// Base test step handler for the BT GPS Device List  configuration
//

#include "ctbtgpsdevicelisthandlerbase.h"
#include <ES_SOCK.H>
#include <iniparser.h>
#include <centralrepository.h>
#include "btgpssim.h"

/** These testexecute steps are used to configure the BT GPS Configuration API, which contains
a list of BT GPS devices that the BT PSY should use to get a location update.  Before a location
request can be made, an application needs to setup this list of BT devices to allow the PSY
to connect to that device.

This framework allows a test to easily setup and view this device list.  This test server can be
loaded into other test servers.  Then the test simply needs to call the test steps contained in
this framework and it will do all of the device list handling.  All the test needs to do is to 
provide an ini file to inform the framework what the device list should look like.

There are currently 3 test steps (which inherit from this base step), which allow:
 - [ctsetupbtgpsdevicelist.cpp] - This allows the  BT GPS device List to be setup 
	 with the devices requested (usually run before a test)
 - [ctcheckbtgpsdevicelist.cpp] - This allows the BT GPS device List to be checked
	 to ensure it contains the expected devices (usually run after a test)
 - [ctdeletebtgpsdevicelist.cpp] - This deletes all of the BT devices from the BT
	 device neighbourhood (usually run after a test)

For information about the exact details of what these test steps do, read the comment header
in the relevant cpp file. Most of the test tests need to provide some ini file data to
the test.  The structure of these ini files is discussed below.

To use either of these test steps in a test:
	1. Ensure this framework is built (should be built in standard integration test fw)
	2. Create the BT Device and Device List Setup ini files (discussed below)
	3. In the test script load the extra test suite:
		LOAD_SUITE ctlbsbtgpspsytestserver
	4. In the test script, add either of the following 
		   RUN_TEST_STEP 100 ct_lbsbtgpspsysuite SetupBTGPSDeviceList c:\testdata\configs\setupdeviceist.ini Test1Pre
		   RUN_TEST_STEP 100 ct_lbsbtgpspsysuite CheckBTGPSDeviceList c:\testdata\configs\setupdeviceist.ini Test1Post
		   RUN_TEST_STEP 100 ct_lbsbtgpspsysuite DeleteBTGPSDeviceList
		Changing them to include the correct ini file and passing in the correct Config Section.  The delete step
		does not require any ini data.
	5. Run the BT test

There are two different types of BT GPS devices that can be added to the list:
	==> Available BT Device - these are devices which are ON and available and which can be connected to.
	==> Unavailable BT Device - these are devices that are not present/on; and cannot be connected to.  
	    Their addresses are made up.

==========================
Device List Setup ini File
==========================

The ini file contains the following structure:
	============================================================
	[BTDevices]
	File = c:\testdata\configs\btgpsdevicelist_real.ini
	CheckDeviceType = 1
	
	[Test1]
	NumOfDevices = 6
	AvailableDevices = BT1, BT2
	Position = 0,4
	DeviceType = 0,1
	DeviceSelection = BT1, BT3
	============================================================

The definition of these entries is:

	[BTDevices] 
	==> For this step to generate the list of BT devices, it needs information about the different BT devices
	(address and port information).
	
	File
	==> This BT device information is put into a separate ini file.  This ini entry defines the location of this
	BT device ini file.  For more information about what needs to go into this ini file, see below.
	
	CheckDeviceType
	==> This defines whether the check device list step should fail if the device type does not match the expected
	device type.  This is turned off for real BT device tests, as both PNOK or standard BT devices could be used so
	failing on the device type is wrong.  For the SIM tests there is greater control on the device type so this can
	be tested.
	1 = Check, and fail if device type is wrong. 0 = do not check device type (although it is logged in the results file)

	[TestX]
	==> Contains the information required to build up the BT device list and the BT Device Selection list.  It is 
	made up of the following sub sections; the only required entry is NumOfDevices (although this will produce a 
	list only with unavailable devices). 

	NumOfDevices
	==> Indicates the total number of BT GPS devices that should be in the list, including available and unavailable devices.

	AvailableDevices
	==> Comma-delimited list which defines what available devices should be added to the list.
	
	Position 
	==> Comma-delimited list which defines the position of the available BT devices.  There should, therefore, be
	a position entry for every single available device in the list.  Unavailable devices fill in the remaining spaces.
	e.g. 6 devices in the list, 2 of which are available.  Their positions are defined as 0 and 4, so the two available
	devices will be at position 0 and 4 in the list.  Unavailable devices will be put in spaces 1,2,3 and 5. 
	
	DeviceType
	==> Comma-delimited list which defines the type of GPS device ((TLbsBtGpsDeviceType) => 0-Unknown, 1=Invalid,
	2=Non-Nok type, 3=Nok type GPS device).  Devices which do not have their device type specified or if the device
	type is outside of the 0-3 range, the device type is set to Unknown (0) for that device.  This field is, therefore,
	optional.
	
	DeviceSelection
	==> Comma-delimited list which defines the device ID of the devices that should be added to the standard BT 
	device selection dialog.  Devices will be selected by the notifier in the order they are added in this list. If this 
	entry is not present or the PSY cannot connect to any devices in this list, the notifier will cancel the PSY request.
	Due to the way the notifier works, it is not possible to add unavailable devices to the device notifier list. If
	the test requires unavailable devices to be added to the list, it should use the simulated device that simulates
	being unavailable.

So the above example will produce two lists:

 1. The device list:
	[ BT1 | A | B | C | BT2 | D ]
	where A,B,C & D are unavailable BT devices
 2. The device selection notifier list:
	[ BT1 | BT3 ]
 
==================
BT Device Ini File
==================
 
 As stated above, the test also needs to provide a list of BT GPS devices along with some information about
each device.  This information is stored in its own ini file, which has the following structure:

	============================================================
	[BT1]
	Name = Holux-M-1000
	Address = 001bc1019591
	Port = 1
	Type = 2

	[BT2]
	Name = Nokia_LD-3W
	Address = 000276c86fe5
	Port = 1
	Type = 3
	============================================================

The definition of these entries is:

	[BTX]
	==> Contains the information about each BT GPS device.  Each device needs to have its own block.  The
	name contained within the block needs to be unique for each BT device and is used in the Device List
	Setup ini file.
	
	Name
	==>  Descriptive name of the BT GPS device.  This is used for logging purposes.
	
	Address
	==> The address of the BT GPS device (TBTDevAddr)
	
	Port
	==> The port/SDP Channel of the BT GPS device
	
	Type
	==>  The type of the BT GPS device (TBtGpsSimDeviceType)
	0 = Real BT GPS Device, 1 = Invalid BT GPS Device, 2 = Standard Device, 3 = PNOK Device, 4...
*/

/** Constructor
 */
CT_BTGPSDeviceListHandler::CT_BTGPSDeviceListHandler()
: iCheckDeviceType(ETrue),
iCheckAvailableDevice(ETrue)
	{
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	}

/** Destructor
 */
CT_BTGPSDeviceListHandler::~CT_BTGPSDeviceListHandler()
	{
	delete iDeviceListManager;
	delete iBTListConfigFile;
	delete iScheduler;
	}

/** Test step pre-amble

@return - TVerdict code
 */
TVerdict CT_BTGPSDeviceListHandler::doTestStepPreambleL()
	{
	//Open the device manager
	iDeviceListManager = CLbsBtGpsConfigInternal::NewL();
	
	if (TestStepResult()!=EPass)
	    return TestStepResult();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/** Actual test step implementation

@return - TVerdict code
 */
TVerdict CT_BTGPSDeviceListHandler::doTestStepL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/** Post Amble Test Step
 
 @return - TVerdict code
 */
TVerdict CT_BTGPSDeviceListHandler::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/** Generates a BT device address for use as an unavailable device

@return BT Device Address
 */
TBTDevAddr CT_BTGPSDeviceListHandler::GenerateUniqueBTDevAddr()
	{
	return TBTDevAddr(MAKE_TINT64(++iUniqueAddr, 0x01234567));
	}

/** Sets up up the BT GPS Device Config ini file
 */
void CT_BTGPSDeviceListHandler::SetupBTDeviceConfigFileL()
	{
	//Get the location of the BT Devices
	TPtrC btDevicesFileLoc;
	if(!GetStringFromConfig(_L("BTDevices"), _L("File"), btDevicesFileLoc))
		{
		//Error, cannot find the list of valid BT devices
		User::Leave(KErrNotFound);
		}
	//Load the config file
	LoadBTDeviceConfigFileL(btDevicesFileLoc);
	}

/** Loads the BT device list config file

@param aFilePath file path for the BT device ini file
 */
void CT_BTGPSDeviceListHandler::LoadBTDeviceConfigFileL(const TDesC& aFilePath)
	{
	if(!aFilePath.Length())
		{
		//Invalid File Path
		User::Leave(KErrNotFound);
		}
	
	TRAPD(err, iBTListConfigFile = CIniData::NewL(aFilePath));
	if(err != KErrNone)
		{
		User::Leave(err);
		}
	}

/** Returns the instance of the Device List Manager, to allow the test steps to
access the BT Device list. 
 */
CLbsBtGpsConfigInternal* CT_BTGPSDeviceListHandler::DeviceListManager()
	{
	return iDeviceListManager;
	}

/** Reads the BT device list ini file to get details of the device list to be created
 */
void CT_BTGPSDeviceListHandler::ReadDeviceListSetupConfigDataL()
	{
	//Read the number of devices that should be in the list
	if(!GetIntFromConfig(ConfigSection(), _L("NumOfDevices"), iDeviceListConfig.iNumOfDevices))
		{
		//Test must specify the number of devices in the list
		User::Leave(KErrNotFound);
		}
	
	//Read the list of available devices that should be in the list
	GetStringFromConfig(ConfigSection(), _L("AvailableDevices"), iDeviceListConfig.iAvailableDevices);
	
	//Check to see if there are available devices
	iAvailableDeviceBufferLength = iDeviceListConfig.iAvailableDevices.Length();
	if(iAvailableDeviceBufferLength == 0)
		{
		iCheckAvailableDevice = EFalse;
		}

	//Read the positions of the available BT Devices
	GetStringFromConfig(ConfigSection(), _L("Position"), iDeviceListConfig.iDevicePositions);
	
	//Read the device types of the BT devices
	GetStringFromConfig(ConfigSection(), _L("DeviceType"), iDeviceListConfig.iDeviceTypes);
	
	//Check if device types are specified
	iDeviceTypeBufferLength = iDeviceListConfig.iDeviceTypes.Length();
	if(iDeviceTypeBufferLength == 0)
		{
		iCheckDeviceType = EFalse;
		}
	
	//Read the device selection information
	GetStringFromConfig(ConfigSection(), _L("DeviceSelection"), iDeviceListConfig.iDeviceSelection);
	}

/** Returns the details for the next BT GPS Device  This is either the next available device
in the list or a randomly generated unavailable device.  The device type returned is the actual
type of device the BT GPS device should be. It may be added to the Configuration API list as a
different device type.

@param aCurrentDeviceNum [IN] Current position in the list
@param aSocketAddress [OUT] Address of the device
@param aDeviceType [OUT] Type of GPS Device (TBtGpsSimDeviceType)

@return KErrNone if the operation succeeded.
		KErrNotFound if the next device should be an available device, but cannot be found in the BT device list
		Any other standard Symbian error code
 */
TInt CT_BTGPSDeviceListHandler::GetNextDeviceDetails(TInt aCurrentDeviceNum, TBTSockAddr& aSocketAddress,
														TInt& aDeviceType)
	{
	TInt err = KErrNone;
	TBuf<2> num;
	TInt port = 0;
	
	num.AppendNum(aCurrentDeviceNum);
	TInt found = iDeviceListConfig.iDevicePositions.Find(num);
	if((found != KErrNotFound) && (iCheckAvailableDevice))
		{
		//Get the id for the next BT device to add to the list
		TPtrC tempdeviceBuffer(iDeviceListConfig.iAvailableDevices.Right(iAvailableDeviceBufferLength));
		TInt endDevicePosition = 0;
		
		//Find the next comma
		endDevicePosition = tempdeviceBuffer.Find(_L(","));
		if(endDevicePosition == KErrNotFound)
			{
			//No more commas in the list, at the end of the file
			endDevicePosition = tempdeviceBuffer.Length();
			}
		
		TPtrC deviceBuf(tempdeviceBuffer.Mid(0, endDevicePosition));
		iAvailableDeviceBufferLength -= (endDevicePosition + 1);
		if(iAvailableDeviceBufferLength <= 0)
			{
			iCheckAvailableDevice = EFalse;
			}
		
		TBuf<4> deviceId(deviceBuf);
		deviceId.TrimAll();
		
		//Find this device in the BT device list
		TPtrC btAddress;
		if(!iBTListConfigFile->FindVar(deviceId, _L("Address"), btAddress))
			{
			err = KErrNotFound;
			}
		TInt64 hwAddrInt(0);
		TLex lex(btAddress);
		err = lex.Val(hwAddrInt, EHex);
		if(err == KErrNone)
			{
			aSocketAddress.SetBTAddr(hwAddrInt);
			}
		
		//Read the device Port information
		if(!iBTListConfigFile->FindVar(deviceId, _L("Port"), port))
			{
			port = 0;
			}
		
		aSocketAddress.SetPort(port);
		
		TInt deviceType;
		if(!iBTListConfigFile->FindVar(deviceId, _L("Type"), deviceType))
			{
			//No device type, device added as standard BT Device
			deviceType = 2;
			}
		aDeviceType = deviceType;
		}
	else
		{
		//Adding an unavailable device to the list
		aSocketAddress.SetBTAddr(GenerateUniqueBTDevAddr());
		aSocketAddress.SetPort(port);
		aDeviceType = -1;
		}
	
	return err;
	}

/** Returns the device type of the current device.  This is different from the
device type returned in GetDeviceDetails.  GetDeviceDetails() returns the actual
device type, this function returns the device type stored in the list (so a real
device can be stored in the list as an invalid device).

@return deviceType type of the current BT GPS device (value between 0-3)
 */
TInt CT_BTGPSDeviceListHandler::GetNextDeviceType()
	{
	//Read off the device type information (if present)
	TInt deviceType = 0;
	if(iCheckDeviceType)
		{
		TPtrC tempBuffer(iDeviceListConfig.iDeviceTypes.Right(iDeviceTypeBufferLength));
		TInt endPosition = 0;
		
		//Find the next comma
		endPosition = tempBuffer.Find(_L(","));
		if(endPosition == KErrNotFound)
			{
			//No more commas in the list, at the end of the file
			endPosition = tempBuffer.Length();
			}
		
		TPtrC deviceTypeBuf(tempBuffer.Mid(0, endPosition));
		iDeviceTypeBufferLength -= (endPosition + 1);
		if(iDeviceTypeBufferLength <= 0)
			{
			iCheckDeviceType = EFalse;
			}
		
		TLex lex(deviceTypeBuf);
		lex.SkipSpace();
		lex.Val(deviceType);
		
		//Check the device type range
		if(deviceType < 0 || deviceType > 3)
			{
			//If the device type is out of the set range, just set it to an unknown device
			deviceType = 0;
			}
		}
	
	return deviceType;
	}

/** Reads the device selection from the ini file and sets the notifier up with the right addresses
and channel/port details.

@return KErrNone if the operation succeeded.
		Any other standard Symbian error code
 */
TInt CT_BTGPSDeviceListHandler::HandleDeviceSelection()
	{
	if(iDeviceListConfig.iDeviceSelection.Length() == 0)
		{
		//Nothing to do, no device specified
		return KErrNone;
		}
	
	TInt err = KErrNone;
	TInt overallErr = KErrNone;
	TInt endPosition = 0;
	TInt listLength = iDeviceListConfig.iDeviceSelection.Length();
	
	//Loop through the devices in the ini file, adding them to the notifier
	while(listLength > 0)
		{
		err = KErrNone;
		
		TPtrC tempDeviceList(iDeviceListConfig.iDeviceSelection.Right(listLength));
		
		endPosition = tempDeviceList.Find(_L(","));
		if(endPosition == KErrNotFound)
			{
			//No more commas in the list, at the end of the file
			endPosition = tempDeviceList.Length();
			}
		
		TPtrC currentDevice(tempDeviceList.Mid(0, endPosition));
		
		TBuf<4> deviceId(currentDevice);
		deviceId.TrimAll();
		
		//Find this device in the BT device list
		TPtrC btAddress;
		if(!iBTListConfigFile->FindVar(deviceId, _L("Address"), btAddress))
			{
			INFO_PRINTF2(_L("ERROR - Cannot find device with id: %S, not adding to Notifier"), &deviceId);
			overallErr = err = KErrNotFound;
			}
		
		//If the device address has been found
		if(err == KErrNone)
			{
			TInt64 hwAddrInt(0);
			TLex lex(btAddress);
			TBTSockAddr socketAddress;
			
			err = lex.Val(hwAddrInt, EHex);
			if(err == KErrNone)
				{
				socketAddress.SetBTAddr(hwAddrInt);
				}
			
			//Get the port information
			TInt port = 0;
			if(!iBTListConfigFile->FindVar(deviceId, _L("Port"), port))
				{
				port = 1;
				}
			socketAddress.SetPort(port);
			
			//Get the device type
			TInt deviceType;
			if(!iBTListConfigFile->FindVar(deviceId, _L("Type"), deviceType))
				{
				//Device type not present, just add as a standard Simulation device
				deviceType = 2;
				}
			
			//Create the device in the neighbourhood
			RBtGpsSim sim;
			TInt err = sim.Connect();
			
			if (err == KErrNone)
				{
				RBtGpsSimDevice simDevice;
				err = simDevice.Open(sim, socketAddress);
				if (err == KErrNotFound)
					{
					// Create a device Entry
					TBtGpsSimDeviceType simType = static_cast<TBtGpsSimDeviceType>(deviceType);
					err = simDevice.Create(sim, simType, socketAddress);
					if(err == KErrNone)
						{
						simDevice.SetParameter(EBtGpsSimAuthorised, ETrue);
						simDevice.SetParameter(EBtGpsSimPin, _L8("0000"));
						}
					}
				
				//Add the device to the notifier list
				if(err == KErrNone)
					{
					err = simDevice.AddToSelectionList();
					}
				
				//Check to see whether the device was successfully added
				if(err != KErrNone)
					{
					INFO_PRINTF3(_L("ERROR %d - Cannot add device with ID %S, to the Notifier"), err, &deviceId);
					overallErr = err;
					}
				
				simDevice.Close();
				}
			sim.Close();
			}
		
		//Get Ready for the next device
		listLength -= endPosition + 1;		
		}
	
	return overallErr;
	}
