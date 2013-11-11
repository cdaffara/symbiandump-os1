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
// ctlbsbtgpspsyteststepbase.cpp
// Base testexecute test step for the BT GPS PSY Integration tests
//

#if (!defined __CT_LBSBTGPSPSYTESTSTEPBASE_STEP_H__)
#define __CT_LBSBTGPSPSYTESTSTEPBASE_STEP_H__

//Includes
#include <test/TestExecuteStepBase.h>
#include <lbs.h>
#include "ctlbsbtgpspsyserver.h"
#include "ctlbsbtgpssimwatcher.h"
#include "ctlbsbtgpspositioner.h"

struct TBTGPSPsyTestConfig
	{
	//Test Case ID
	TInt iTestCaseId;
	//Number of Location Requests to make
	TInt iNumOfLocRequests;
	//Whether the location request should be received
	TBool iLocationRequestPass;
	//TPositionUpdateOptions - MaxAge
	TInt iMaxAge;
	//TPositionUpdateOptions - Timeout
	TInt iTimeout;
	//TPositionUpdateOptions - Interval
	TInt iInterval;
	//TPositionUpdateOptions - Partial Updates
	TBool iPartialUpdates;
	};


/** Base test step for the BT GPS PSY Integration tests
 */
class CT_LbsBtGpsPsyTestStepBase : public CTestStep,
									public MBtGpsSimEventObserver, 
									public MBtGpsPositionerObserver
	{
public:
	CT_LbsBtGpsPsyTestStepBase();
	~CT_LbsBtGpsPsyTestStepBase();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
public:
	// from MBtGpsPositionerObserver:
	virtual void OnPositionUpdate(const TPosition& aPosition);
	virtual void OnPositionError(TInt aError);
	
	// from MBtGpsSimEventObserver:
	virtual void OnDeviceConnect(const TBTSockAddr& aAddr);
	virtual void OnDeviceDisconnect(const TBTSockAddr& aAddr);
	virtual void OnDeviceError(const TBTSockAddr& aAddr);
	
protected:
	//Enables the Positioner
	void EnablePositionerL();
	
	//Setups the Positioner and sets Update Options
	void SetupPositionerL();

	//Resets event counters
	void ResetEventCounters();
	
	//Reads all the test data from the config ini file and stores it in iTestConfig
	void ReadTestDataConfig();
	
	//Used to make a location request to the PSY
	TInt LocationRequest();
	
	//Used to set the night mode of the PSY
	TInt SetupNightModeL(TBool aNightMode);
	
protected:
	CActiveScheduler* iScheduler;

	CBtGpsSimWatcher* iSimWatcher;
	CBtGpsPositioner* iPositioner;
	
	// Counts the number of occurrences of each event:
	// (use ResetEventCounters to reset)
	TInt iGotPositionUpdate;
	TInt iGotPositionError;
	TInt iGotDeviceConnect;
	TInt iGotDeviceDisconnect;
	TInt iGotDeviceError;
	
	TBTGPSPsyTestConfig iTestConfig;
	};

#endif	//__CT_LBSBTGPSPSYTESTSTEPBASE_STEP_H__
