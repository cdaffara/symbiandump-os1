// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "TSU_MMF_A2DPBLUETOOTH_SVR_Step.h"
#include "A2dpBTHeadsetAudioIfServer.h"
#include "A2dpBTHeadsetAudioIfServerStart.h"


TInt CTestStep_MMF_A2DPBLUETOOTH_SVR::StartA2dpBluetoothServer()
	{
	// Start BT audio server in its own thread here...
	// Assume that the server's not been started already
	// 
	RThread btServerThread;
	TThreadFunction btServerThreadFunc = CA2dpBTHeadsetAudioIfServer::StartThread;
	TName btServerName(KBTAudioServerName);


	TInt err = iBtServerThread.Create(btServerName, btServerThreadFunc, KBTAudioServerStackSize,
								KBTAudioServerInitHeapSize, KBTAudioServerMaxHeapSize,
								NULL, EOwnerProcess);	// NULL => not passing any params to thread

	if(!err)
		{
		// Synchronise with the server
		TRequestStatus reqStatus;
		iBtServerThread.Rendezvous(reqStatus);

		if (reqStatus != KRequestPending)
			{
			iBtServerThread.Kill(0);
			}
		else 
			{
			// Start the thread
			iBtServerThread.Resume();
			// Server will call the reciprocal static synchronise call
			}
		User::WaitForRequest(reqStatus); // wait for start or death
		}
		
	return err;
	}
	
void CTestStep_MMF_A2DPBLUETOOTH_SVR::StopA2dpBluetoothServer()
	{
	iBtServerThread.Kill(0);
	iBtServerThread.Close();
	}
		
/**
 * Pre-amble for test step
 * Sets __MM_HEAP_MARK for memory leak detection
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR::DoTestStepPreambleL()
	{
	
	 __MM_HEAP_MARK;
	
	//load up the bluetooth comms driver 
	iError =User::LoadPhysicalDevice(BT_COMM_PDD_NAME);
	
	if ((iError) && (iError != KErrAlreadyExists))
		{
		INFO_PRINTF2(_L("error loading bluetooth drivers %d" ), iError);
		return EInconclusive;
		}
	else
		{
		iError = User::LoadLogicalDevice(BT_COMM_LDD_NAME);
		}

	iActiveScheduler = CActiveScheduler::Current();
	if (!iActiveScheduler) iActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(iActiveScheduler);
	iError = StartA2dpBluetoothServer();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("error starting up a2dp bluetooth server %d" ), iError);
	 	return EInconclusive;
		}
	else
		{
		iError =iA2dpBTHeadsetAudioInterface.Connect();
		if (iError != KErrNone)
			{
	 		INFO_PRINTF2(_L("error on RA2dpBTHeadsetAudioInterface connection %d" ), iError);
	 		return EInconclusive;
			}
	 	}
	
	//Before each test the BT link needs resetting to clear any state/config
	//information on the headset from previous tests
	iSocketServer.Connect();
	TRAP(iError,iBtPhysicalLink = CBluetoothPhysicalLinks::NewL(*this,iSocketServer));
	if (iError)
		{
		iA2dpBTHeadsetAudioInterface.Close();
		INFO_PRINTF2(_L("error on CBluetoothPhysicalLinks construction %d" ), iError);
		return EInconclusive;
		}
	TRAP(iError, iTimer = CStopActiveSchedulerTimer::NewL());
	if (iError)
		{
		iA2dpBTHeadsetAudioInterface.Close();
		delete iBtPhysicalLink;
		INFO_PRINTF2(_L("error on CStopActiveSchedulerTimer construction %d" ), iError);
		return EInconclusive;
		}
	TRAP(iError,iAsyncTestStepNotifier = CAsyncTestStepNotifier::NewL(this));
	if (iError)
		{//note we don't use clean up stack for this as we want to exit
		//normally rather than leave
		iA2dpBTHeadsetAudioInterface.Close();
		delete iBtPhysicalLink;
		delete iTimer;
		INFO_PRINTF2(_L("error on CAsyncTestStepNotifier construction %d" ), iError);
		return EInconclusive;
		}
	
	//Get the address of the BT headset from the central repository
	CRepository* centralRepository = NULL;
	TRAP(iError,centralRepository = CRepository::NewL(KUidBTHeadsetRepository));
	TBuf<16> btHeadsetAddressString;
	iError = centralRepository->Get(0x01, btHeadsetAddressString);
	TLex stringConverter(btHeadsetAddressString.Ptr());
	TInt64 btHeadsetAddress = 0;
	iError = stringConverter.Val(btHeadsetAddress,EHex);
	iBTheadsetAddress = new TBTDevAddr(btHeadsetAddress);
	delete centralRepository;
	
	return EPass;
	}
	
	
void CTestStep_MMF_A2DPBLUETOOTH_SVR::HandleDisconnectCompleteL(TInt aErr)
	{
	if (!aErr)
		{
		iBTLinkDisconnected = ETrue;
		}
	}
	
void CTestStep_MMF_A2DPBLUETOOTH_SVR::AsyncNotification(TInt aError) 
	{
	iError = aError;
	iActiveScheduler->Stop();
	}

/**
 * Post-amble for test step
 * Sets __MM_HEAP_MARKEND for memory leak detection
 */
TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR::DoTestStepPostambleL()
	{
		
	iA2dpBTHeadsetAudioInterface.Close();
	StopA2dpBluetoothServer();
	delete iBtPhysicalLink;
	delete iBTheadsetAddress;
	delete iAsyncTestStepNotifier;
	iSocketServer.Close();
	delete iTimer;
	delete iActiveScheduler;
	
	
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
	

TVerdict CTestStep_MMF_A2DPBLUETOOTH_SVR_Initialized::DoTestStepPreambleL()
	{
	TVerdict verdict = CTestStep_MMF_A2DPBLUETOOTH_SVR::DoTestStepPreambleL();
	
	if (verdict == EPass)
		{
		TRequestStatus* status = &(iAsyncTestStepNotifier->RequestStatus());
		iA2dpBTHeadsetAudioInterface.Initialize(*iBTheadsetAddress, *status);
		iAsyncTestStepNotifier->HandleAsyncRequest();
		if (iError != KErrNone)
			{
			INFO_PRINTF2(_L("Initialization error %d" ), iError);
			verdict =  EInconclusive;
			}
		}
	return verdict;
	}

