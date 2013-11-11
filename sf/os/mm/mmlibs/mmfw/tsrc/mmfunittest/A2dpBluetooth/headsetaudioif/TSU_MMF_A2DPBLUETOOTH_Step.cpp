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

// Test system includes
#include "TSU_MMF_A2DPBLUETOOTH_Step.h"


/**
 * Pre-amble for test step
 * Sets __MM_HEAP_MARK for memory leak detection
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH::DoTestStepPreambleL()
	{
	
	 __MM_HEAP_MARK;
	
	//load up the bluetooth comms driver 
	TInt err = KErrNone;
	err = User::LoadPhysicalDevice(BT_COMM_PDD_NAME);
	if ((err == KErrNone) || (err == KErrAlreadyExists))
		{
		err = User::LoadLogicalDevice(BT_COMM_LDD_NAME);
		}
	if (err && (err != KErrAlreadyExists))
		{
		INFO_PRINTF2(_L("problem loading bluetooth comms driver %d" ), err);
	 	return EInconclusive;
		}

	iActiveScheduler = CActiveScheduler::Current();
	if (!iActiveScheduler) iActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);

	 TRAP(err,iA2dpBTHeadsetAudioInterface = CA2dpBTHeadsetAudioInterface::NewL())
	 if (err != KErrNone)
	 	{
	 	INFO_PRINTF2(_L("error on CA2dpBTHeadsetAudioInterface construction %d" ), err);
	 	return EInconclusive;
	 	}
	
	iSocketServer.Connect();
	TRAP(err,iBtPhysicalLink = CBluetoothPhysicalLinks::NewL(*this,iSocketServer));
	if (err)
		{
		delete iA2dpBTHeadsetAudioInterface;
		INFO_PRINTF2(_L("error on CBluetoothPhysicalLinks construction %d" ), err);
		return EInconclusive;
		}
	TRAP(err, iTimer = CStopActiveSchedulerTimer::NewL());
	if (err)
		{
		delete iA2dpBTHeadsetAudioInterface;
		delete iBtPhysicalLink;
		INFO_PRINTF2(_L("error on CStopActiveSchedulerTimer construction %d" ), err);
		return EInconclusive;
		}
	TRAP(err,iAsyncTestStepNotifier = CAsyncTestStepNotifier::NewL(this));
	if (err)
		{//note we don't use clean up stack for this as we want to exit
		//normally rather than leave
		delete iA2dpBTHeadsetAudioInterface;
		delete iBtPhysicalLink;
		delete iTimer;
		INFO_PRINTF2(_L("error on CAsyncTestStepNotifier construction %d" ), err);
		return EInconclusive;
		}
	
	//Get the address of the BT headset from the central repository
	CRepository* centralRepository = NULL;
	TRAP(err,centralRepository = CRepository::NewL(KUidBTHeadsetRepository));
	TBuf<16> btHeadsetAddressString;
	err = centralRepository->Get(0x01, btHeadsetAddressString);
	TLex stringConverter(btHeadsetAddressString.Ptr());
	TInt64 btHeadsetAddress = 0;
	err = stringConverter.Val(btHeadsetAddress,EHex);
	iBTheadsetAddress = new TBTDevAddr(btHeadsetAddress);
	delete centralRepository;
	
	return EPass;
	}
	
	
void CTestStep_MMF_A2DPBLUETOOTH::HandleDisconnectCompleteL(TInt aErr)
	{
	if (!aErr)
		{
		iBTLinkDisconnected = ETrue;
		}
	}
	
void CTestStep_MMF_A2DPBLUETOOTH::AsyncNotification(TInt aError) 
	{
	iError = aError;
	if (CActiveScheduler::Current()->StackDepth())
		{
		iActiveScheduler->Stop();
		}
	}

/**
 * Post-amble for test step
 * Sets __MM_HEAP_MARKEND for memory leak detection
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH::DoTestStepPostambleL()
	{
	delete iA2dpBTHeadsetAudioInterface;
	delete iActiveScheduler;
	delete iBtPhysicalLink;
	delete iTimer;
	delete iBTheadsetAddress;
	delete iAsyncTestStepNotifier;
	iSocketServer.Close();
	
	TRequestStatus status;
	RTimer timer;
	TInt err = timer.CreateLocal();
	if (!err)
		{
		timer.After(status, TTimeIntervalMicroSeconds32(KAllowTimeForHeadsetToResetItselfBetweenTests));
		User::WaitForRequest(status);
		timer.Close();
		}
	else
		{
		INFO_PRINTF2(_L("Timout problem subsequent test may be affected %d" ), err);
		}
		
	__MM_HEAP_MARKEND;

	return EPass;
	}
	

TVerdict CTestStep_MMF_A2DPBLUETOOTH_Initialized::DoTestStepPreambleL()
	{
	TVerdict verdict = CTestStep_MMF_A2DPBLUETOOTH::DoTestStepPreambleL();
	
	if (verdict == EPass)
		{
		TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
		iA2dpBTHeadsetAudioInterface->Initialize(*iBTheadsetAddress, *status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Initialization error %d" ), iError);
			verdict =  EInconclusive;
			}
		}
	return verdict;
	}

