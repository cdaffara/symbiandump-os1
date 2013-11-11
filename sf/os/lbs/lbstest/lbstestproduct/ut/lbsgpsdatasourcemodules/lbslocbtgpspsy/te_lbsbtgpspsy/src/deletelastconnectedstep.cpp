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
// This step removes from the central repository any last known device data that
// may have existed in the BT GPS PSY.
//



/**
 @file deletelastconnectedstep.cpp
 @internalTechnology
*/
#include "deletelastconnectedstep.h"
#include "Te_LbsBtGpsPsyDefs.h"
#include "BtGpsPsyPrivateCRKeys.h"
#include <lbs.h>
#include <bt_sock.h>
#include <centralrepository.h>

#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
#include <lbs/lbsbtgpsconfig.h>
#endif

// CONSTANTS




CDeleteLastConnectedDevice::~CDeleteLastConnectedDevice()
/**
 * Destructor
 */
	{
	}

CDeleteLastConnectedDevice::CDeleteLastConnectedDevice()
/**
 * Constructor
 */
	{
	SetTestStepName(KDeleteLastConnectedDevice);
	}

TVerdict CDeleteLastConnectedDevice::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
		// Bluetooth GPS PSY is using the new configuration API to store HW information
		// simply clear the list
		
		SetTestStepResult(EPass);

		CLbsBtGpsConfig* config = CLbsBtGpsConfig::NewL();
		CleanupStack::PushL(config);
		
		User::LeaveIfError(config->EmptyDeviceList());
		
		CleanupStack::PopAndDestroy(config);
		
	    _LIT(KDeviceRemoved, "Cleared Bluetooth GPS PSY device list" );
		INFO_PRINTF1(KDeviceRemoved); 
#else
		// Bluetooth GPS PSY is using cenrep to store HW information  
		
		// This step removes any last known device information from the BT GPS PSY
		TUid KCRUidBtGpsPsy = { 0x101FE999 };
		SetTestStepResult(EPass);
		CRepository* settings = CRepository::NewL(KCRUidBtGpsPsy);
		
	    TInt err = settings->Set(KBluetoothGpsPsyDeviceAddressValidity, 0);
		
	    TInt isValid;
	    err |= settings->Get(KBluetoothGpsPsyDeviceAddressValidity, isValid);
	   
	    TBTSockAddr dummyAddr;
       
		TBuf<KMaxSockAddrSize> addressBuffer;
		addressBuffer.Copy(dummyAddr);
		err |= settings->Set(KBluetoothGpsPsyDeviceAddress , addressBuffer);
		err |= settings->Set(KDevNotifCenrepHwKey,addressBuffer);
	  
		if(isValid==0)
	    	{
		    _LIT(KDeviceInvalidated, "Device is invalidated" );
			INFO_PRINTF1(KDeviceInvalidated); 
	    	}
	    if((err!=KErrNone)||(isValid))
	    	{
		    _LIT(KErrorOccurred, "Error or device is still valid. Error is %d and isValid is %d");
			INFO_PRINTF3(KErrorOccurred,err,isValid);
	    	SetTestStepResult(EFail); 
	    	}
#endif
		}
	  return TestStepResult();
 	}



// End of File


