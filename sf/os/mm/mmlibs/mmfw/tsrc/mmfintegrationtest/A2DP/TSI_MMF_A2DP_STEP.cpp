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

#include "TSI_MMF_A2DP_STEP.h"

/**
 *
 * CAsyncTestStepNotifier::NewL
 *
 */
CAsyncTestStepNotifier* CAsyncTestStepNotifier::NewL(RTestStepA2dp* aTestStep)
	{
	CAsyncTestStepNotifier* self = new(ELeave) CAsyncTestStepNotifier();
	CleanupStack::PushL(self);
	self->ConstructL(aTestStep);
	CleanupStack::Pop();
	return self;
	}

/**
 *
 * CAsyncTestStepNotifier::CAsyncTestStepNotifier()
 *
 */
CAsyncTestStepNotifier::CAsyncTestStepNotifier() : CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}


/**
 *
 * CAsyncTestStepNotifier::ConstructL
 *
 */
void CAsyncTestStepNotifier::ConstructL(RTestStepA2dp* aTestStep)
	{
	iTestStep = aTestStep;
	}

/**
 *
 * CAsyncTestStepNotifier::~CAsyncTestStepNotifier()
 *
 */
CAsyncTestStepNotifier::~CAsyncTestStepNotifier()
	{	
	}


/**
 * Method to kick off the active scheduler.  The calling test
 * must have a pending request before calling this.
 * 
 */	
void CAsyncTestStepNotifier::HandleAsyncRequest()
	{
	if (iStatus != KRequestPending)
		{
		iStatus = KRequestPending;	
		}
	SetActive();
	CActiveScheduler::Current()->Start();
	}

/**
 *
 * RunL of the Active object
 *
 */
void CAsyncTestStepNotifier::RunL()
	{
	iTestStep->AsyncNotification(iStatus.Int());
	}

/**
 *
 * CAsyncTestStepNotifier::DoCancel()
 *
 */	
void CAsyncTestStepNotifier::DoCancel()
	{
	iTestStep->AsyncNotification(KErrCancel);	
	}
	

/**
 * Class used by the test harness to stop the active scheduler 
 * after a timeout period
 */
CStopActiveSchedulerTimer* CStopActiveSchedulerTimer::NewL()
	{
	CStopActiveSchedulerTimer* self = new(ELeave)CStopActiveSchedulerTimer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	
	return self;
	}

/**
 *
 * CStopActiveSchedulerTimer::ConstructL()
 *
 */	
void CStopActiveSchedulerTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

/**
 *
 * CStopActiveSchedulerTimer::CStopActiveSchedulerTimer()
 *
 */	
CStopActiveSchedulerTimer::CStopActiveSchedulerTimer() : CTimer(EPriorityHigh)
	{
	}

/**
 *
 * CStopActiveSchedulerTimer::StopAfter
 *
 */	
void CStopActiveSchedulerTimer::StopAfter(TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	Cancel();
	After(aTimeInterval);	
	}

/**
 *
 * CStopActiveSchedulerTimer::RunL()
 *
 */	
void CStopActiveSchedulerTimer::RunL()
	{
	if (CActiveScheduler::Current()->StackDepth())
		{
		CActiveScheduler::Stop();
		}
	}



/**
 *
 * RTestStepA2dp
 * Test step constructor
 *
 */
RTestStepA2dp::RTestStepA2dp()
	{ 
	iExpectedValue = KErrNone;
	}

/**
 *
 * ~RTestStepA2dp
 *	 Test step destructor
 *
 */
 
RTestStepA2dp::~RTestStepA2dp()
	{
	if (iMMFDevSound)
		{	
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	
	if (iAS)
		{
		delete iAS;
		iAS = NULL;
		}
	
	if (iBtPhysicalLink)
		{
		delete iBtPhysicalLink;
		iSocketServer.Close();
		}
	
	if (iTimer)
		{
		delete iTimer;
		}
		
	if (iBTheadsetAddress)
		{
		delete iBTheadsetAddress;
		}
	
	if (iAsyncRequestHandler)
		{
		delete iAsyncRequestHandler;	
		}
	}

/**
 *
 * SetTestSuite
 * @param aTestSuite
 *
 */

void RTestStepA2dp::SetTestSuite(const CTestSuiteA2dp* aTestSuite)
	{
	iA2dpTestSuite = aTestSuite;
	}

/**
 *
 * ResetCallbacks
 *
 */
void RTestStepA2dp::ResetCallbacks()
	{
	//Initialize callback array
	iCallbackArray.Reset();
	}


/**
 *
 * DoTestStepPreamble
 * @result TVerdict
 *
 */

TVerdict RTestStepA2dp::DoTestStepPreambleL()
	{	
	INFO_PRINTF1(_L("A2DP test preamble"));
	
	TVerdict PreamRes;
	PreamRes = EPass;
	
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);
	
	iAsyncRequestHandler = CAsyncTestStepNotifier::NewL(this);

	TInt err = KErrNone;
	
	err = User::LoadPhysicalDevice(BT_COMM_PDD_NAME);
	err = User::LoadLogicalDevice(BT_COMM_LDD_NAME);


	INFO_PRINTF1(_L("Creating DevSound Object"));
	TRAP(err, iMMFDevSound = CMMFDevSound::NewL());
	if (err)
		{
		INFO_PRINTF2(_L("error on creating the DevSound object %d" ), err);
		PreamRes = EInconclusive;
		}
	else
		{	
		iSocketServer.Connect();
		TRAPD(err1,iBtPhysicalLink = CBluetoothPhysicalLinks::NewL(*this,iSocketServer));
		if (err1)
			{
			delete iMMFDevSound;
			INFO_PRINTF2(_L("error on CBluetoothPhysicalLinks construction %d" ), err);
			PreamRes = EInconclusive;
			return PreamRes;
			}
		else //Get the address of the BT headset from the central repository
			{
			CRepository* centralRepository = NULL;
			TRAP(err1,centralRepository = CRepository::NewL(KUidBTHeadsetRepository));
			TBuf<16> btHeadsetAddressString;
			err1 = centralRepository->Get(0x01, btHeadsetAddressString);
			TLex stringConverter(btHeadsetAddressString.Ptr());
			TInt64 btHeadsetAddress = 0;
			err1 = stringConverter.Val(btHeadsetAddress,EHex);
			iBTheadsetAddress = new TBTDevAddr(btHeadsetAddress);	
			delete centralRepository;
			}
		if (err1) 
			{
			INFO_PRINTF1(_L("Problem with getting the BT address from the central repository" ));
			INFO_PRINTF1(_L("Cannot physically disconnect the BT link" ));
			PreamRes = EInconclusive;
			return PreamRes;
			}
		TRAP(err1, iTimer = CStopActiveSchedulerTimer::NewL());
		if (err1) 
			{
			INFO_PRINTF1(_L("Problem with creating CTimer" ));
			INFO_PRINTF1(_L("Cannot physically disconnect the BT link" ));
			PreamRes = EInconclusive;
			return PreamRes;
			}
		}
	return PreamRes;
	}	


/**
 *
 * DoTestStepPostambleL
 * @result TVerdict
 *
 */
TVerdict RTestStepA2dp::DoTestStepPostambleL()
	{
	INFO_PRINTF1(_L("A2DP test postamble"));

	TVerdict PostRes;
	PostRes = EPass;

	//Disconnect the BT link
	if (!iBTLinkDisconnected) 
		{
		INFO_PRINTF1(_L("Disconnecting the BT link for the next test"));
		iBtPhysicalLink->Disconnect(*iBTheadsetAddress);

		//the lower level code takes around 30sec to register 
		//that the BT link has gone down
		iTimer->StopAfter(TTimeIntervalMicroSeconds32(KTimeToRegisterLinkLost));
		
		CActiveScheduler::Current()->Start();
		}
	
	if (!iBTLinkDisconnected) 
		{
		INFO_PRINTF1(_L("WARNING: BT link failed to disconnect" ));
		INFO_PRINTF1(_L("WARNING: May cause the rest of the test to fail" ));
		}

	if (iMMFDevSound)
		{	
		delete iMMFDevSound;
		iMMFDevSound = NULL;
		}
	
	if (iAS)
		{
		delete iAS;
		iAS = NULL;
		}
	
	if (iBtPhysicalLink)
		{
		delete iBtPhysicalLink;
		iSocketServer.Close();
		}
	
	if (iTimer)
		{
		delete iTimer;
		}
		
	if (iBTheadsetAddress)
		{
		delete iBTheadsetAddress;
		}
	
	if (iAsyncRequestHandler)
		{
		delete iAsyncRequestHandler;	
		}

	//The BT link may require about 5 sec to complete the disconnection
	//This process is needed for cleaning the status for the next test. 
	User::After(5000000);
	
	return PostRes;
	}



/**
 *
 * InitializeComplete
 * @param aError
 *
 */
void RTestStepA2dp::InitializeComplete (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called InitializeComplete with aError = %d"), aError);
	iCallbackError = aError;
	TRequestStatus* status = &(iAsyncRequestHandler->iStatus);
	User::RequestComplete(status, iCallbackError);
	}


/**
 *
 * BufferToBeFilled
 * @param aBuffer
 *
 */
void RTestStepA2dp::BufferToBeFilled (CMMFBuffer* aBuffer)
	{
	INFO_PRINTF1(_L("DevSound called BufferToBeFilled"));
	iBuffer = aBuffer;
	TRequestStatus* stat = &(iAsyncRequestHandler->iStatus);
	
	if (aBuffer != NULL) 
		{
		User::RequestComplete(stat, KErrNone);
		iCallbackError = KErrNone;
		}
	else 
		{
		User::RequestComplete(stat, KErrNotFound);
		iCallbackError = KErrNotFound;
		}
		
	iCallbackArray[EBuffToFill] ++;
	}


void RTestStepA2dp::PlayError (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called PlayError with aError = %d"), aError);
	TRequestStatus* stat = &(iAsyncRequestHandler->iStatus);	
	User::RequestComplete (stat, aError);
	iCallbackError = aError;

	iCallbackArray[EPlayError] ++;
	}

/**
 *
 * ToneFinished
 * @param aError
 *
 */
void RTestStepA2dp::ToneFinished (TInt aError)
	{
	INFO_PRINTF2(_L("DevSound called ToneFinished with aError = %d"), aError);
	TRequestStatus* stat = &(iAsyncRequestHandler->iStatus);
	User::RequestComplete(stat, aError);
	iCallbackError = aError;
	}


/**
 *
 * MapcInitComplete
 *
 */
void RTestStepA2dp::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds & )
	{
	iError = aError;
	INFO_PRINTF1( _L("MMdaAudioPlayerCallback Init Complete"));
	CActiveScheduler::Stop();
	}


/**
 *
 * MoscoStateChangeEvent
 *
 */
void RTestStepA2dp::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorCode)
	{
	iError = aErrorCode;
	INFO_PRINTF1( _L("CTestStepSDevSoundRecordFile : MMdaObjectStateChangeObserver Callback for CMdaAudioRecorderUtility complete"));
	INFO_PRINTF2( _L("iError %d "), iError);
	CActiveScheduler::Stop();
	}	

/**
 *
 * MatoPrepareComplete
 *
 */
void RTestStepA2dp::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF1( _L("CTestMmfAclntTone::MatoPrepareComplete MMdaAudioToneObserver Callback for CMdaAudioToneUtility complete"));
	INFO_PRINTF2( _L("iError %d"), iError);
	CActiveScheduler::Stop();
	}

/**
 *
 * MaoscOpenComplete
 *
 */
void RTestStepA2dp::MaoscOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

/**
 *
 * MaoscBufferCopied
 *
 */
void RTestStepA2dp::MaoscBufferCopied(TInt aError, const TDesC8& /*aBuffer*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

/**
 *
 * MaoscPlayComplete
 *
 */
void RTestStepA2dp::MaoscPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}


/**
 *
 * HandleDisconnectCompleteL
 * @param aError
 *
 */
void RTestStepA2dp::HandleDisconnectCompleteL(TInt aError)
	{
	if (!aError)
		{
		iBTLinkDisconnected = ETrue;
		}
	}

void RTestStepA2dp::AsyncNotification(TInt aError) 
	{
	iCallbackError = aError;
	iAS->Stop();
	}

