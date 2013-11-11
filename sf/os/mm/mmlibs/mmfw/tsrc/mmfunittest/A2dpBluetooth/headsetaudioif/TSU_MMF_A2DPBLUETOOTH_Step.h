// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __TSU_MMF_A2DPBLUETOOTH_STEP_H__
#define __TSU_MMF_A2DPBLUETOOTH_STEP_H__

#include <TestFramework.h>
#include <centralrepository.h>
#include "a2dpbtheadsetAudioif.h"
#include "TSU_MMF_A2DPBLUETOOTH_Timer.h"
#include "TSU_MMF_A2DPBLUETOOTH_Async.h"

#if defined (__WINS__)
#define BT_COMM_PDD_NAME _L("ECDRV")
#define BT_COMM_LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define BT_COMM_PDD_NAME _L("EUART1")
#define BT_COMM_LDD_NAME _L("ECOMM")
#endif

class CTestSuite;
class CTestSuite_MMF_A2DPBLUETOOTH;
class CStopActiveSchedulerTimer;

const TUint KApproxInitializationTime = 5000000; //5 seconds
const TUint KTimeToRegisterLinkLost = 30000000; //30 seconds
const TUint KDisableBTLinkAfterTime = 200000; //200mS
const TUint KTimeForEventsToRegister = 500000; //0.5 seconds
const TUint KMaxTimeToPlayOutCachedBuffers = 5000000; //5 seconds
const TUint KAllowTimeForHeadsetToResetItselfBetweenTests = 5000000; //5 seconds
const TUint KInvalidBTAddress = 1234;
const TUid KUidBTHeadsetRepository = {0x1020497A};
const TUint KDefaultTestSampleRate = 32000;


/**
 *
 * CTestStep_MMF_A2DPBLUETOOTH, which is the base class for all 
 * the MMF A2DPBLUETOOTH suite test steps.
 *
 * @lib "TSU_MMF_A2DPBLUETOOTH.lib"
 *
 */
class CTestStep_MMF_A2DPBLUETOOTH : public CTestStep, MBluetoothPhysicalLinksNotifier
	{
public:
	static void CleanUp(TAny *aPtr) ;
	virtual void AsyncNotification(TInt aError);
protected:
	virtual TVerdict DoTestStepPreambleL();
	virtual TVerdict DoTestStepPostambleL();
	
	//MBluetoothPhysicalLinksNotifier
	virtual void HandleCreateConnectionCompleteL(TInt /*aErr*/){};
	virtual void HandleDisconnectCompleteL(TInt aErr);
	virtual void HandleDisconnectAllCompleteL(TInt /*aErr*/){};
protected:
	TInt iError;
	TBTDevAddr* iBTheadsetAddress;
	CActiveScheduler* iActiveScheduler;
	CA2dpBTHeadsetAudioInterface* iA2dpBTHeadsetAudioInterface;
	CBluetoothPhysicalLinks* iBtPhysicalLink;
	CStopActiveSchedulerTimer* iTimer;
	CAsyncTestStepNotifier* iAsyncTestStepNotifier;
	RSocketServ iSocketServer;
	TBool iBTLinkDisconnected;
	};
	
class CTestStep_MMF_A2DPBLUETOOTH_Initialized : public CTestStep_MMF_A2DPBLUETOOTH
	{
protected:
	virtual TVerdict DoTestStepPreambleL();
	};
	
#endif /* __TSU_MMF_A2DPBLUETOOTH_STEP_H__ */
