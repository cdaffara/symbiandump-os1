// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CPP file: Audio Resource Notification tests for Video Player.
// 
//

/**
 @file testvideoplayer_apr.cpp
*/

#include "testvideoplayer_apr.h"
const TUint KCacheSize = 1000000; //1MB
const TUint KMaxCacheSize = 30000000; //20MB


//------------------------------------------------------------------------------
//******************** CR 0805 - Auto Pause Resume for video client********************
//------------------------------------------------------------------------------

/**
 * Standard static NewL() taking a callback function
 */
CCallBackTimer* CCallBackTimer::NewL(TCallBack aCallBack, TPriority aPriority)
	{
	CCallBackTimer* self = new(ELeave) CCallBackTimer(aCallBack, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/**
 * Private c'tor
 */
CCallBackTimer::CCallBackTimer(TCallBack aCallBack, TPriority aPriority)
: CTimer(aPriority),
iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

/*
 * Callback on timer complete
 */
void CCallBackTimer::RunL()
	{
	iCallBack.CallBack();
	}

//
//CTestAVPlayerAPRCallbackSupport
//

/**
 * NewL
 */
CTestAVPlayerAPRCallbackSupport* CTestAVPlayerAPRCallbackSupport::NewL(MTestAVPlayerAPRCallback& aCallback)
	{
	return new(ELeave) CTestAVPlayerAPRCallbackSupport(aCallback);
	}

/** 
 * MvpuoOpenComplete
 */
 void CTestAVPlayerAPRCallbackSupport::MvpuoOpenComplete(TInt aError)
	{
	iCallback.MvpuoOpenCompleteTest(aError, *this);
	}

/** 
 * MvpuoPrepareComplete
 */
void CTestAVPlayerAPRCallbackSupport::MvpuoPrepareComplete(TInt aError)
	{
	iCallback.MvpuoPrepareCompleteTest(aError, *this);
	}

/* 
 * MvpuoFrameReady
 */
void CTestAVPlayerAPRCallbackSupport::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
	{
	iCallback.MvpuoFrameReadyTest(aFrame, aError, *this);
	}

/* 
 * MvpuoPlayComplete
 */
void CTestAVPlayerAPRCallbackSupport::MvpuoPlayComplete(TInt aError)
	{
	iCallback.MvpuoPlayCompleteTest(aError, *this);
	}

/* 
 * MvpuoEvent
 */
void CTestAVPlayerAPRCallbackSupport::MvpuoEvent(const TMMFEvent &aEvent)
	{
	iCallback.MvpuoEventTest(aEvent, *this);
	}

/* 
 * MapcInitComplete
 */
void CTestAVPlayerAPRCallbackSupport::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds &aDuration)
	{
	iCallback.MapcInitCompleteTest(aError, aDuration, *this);
	}

/* 
 * MapcPlayComplete
 */
void CTestAVPlayerAPRCallbackSupport::MapcPlayComplete(TInt aError)
	{
	iCallback.MapcPlayCompleteTest(aError, *this);
	}
	
/**
 * MarncResourceAvailable
 */
void CTestAVPlayerAPRCallbackSupport::MarncResourceAvailable(TUid aNotificationEventId, const TDesC8& aNotificationData)
	{
	iCallback.MarncResourceAvailableTest(aNotificationEventId, aNotificationData, *this);
	}

/**
 * Constructor
 */
CTestAVPlayerAPRCallbackSupport::CTestAVPlayerAPRCallbackSupport(MTestAVPlayerAPRCallback& aCallback):iCallback(aCallback)
	{

	}

//
//RTestMmfVclntVideoPlayerAPR
//

/**
 * Constructor
 */
RTestMmfVclntVideoPlayerAPR::RTestMmfVclntVideoPlayerAPR(const TDesC& aTestName, const TDesC& aSectName)
		:RTestMmfVclntAviStep(aTestName, aSectName, _L(""), KErrNone)
	{
	iHeapSize = 1000000;
	iExpEventCount = 1;
	iActEventCount = 0;
	}

/**
 * MvpuoOpenCompleteTest
 */
void RTestMmfVclntVideoPlayerAPR::MvpuoOpenCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
	{
	if (&aVideoPlayerCallbackSupport == iVideoPlayerCallbackSupport1)
		{
		INFO_PRINTF2(_L("iVideoPlayer -> MMdaVideoPlayerCallback OpenComplete (%d)"), aError);
		Fsm(EMvpuoOpenComplete, aError);
		}
	}

/**
 * MvpuoPrepareCompleteTest
 */
void RTestMmfVclntVideoPlayerAPR::MvpuoPrepareCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
	{
	if (&aVideoPlayerCallbackSupport == iVideoPlayerCallbackSupport1)
		{
		INFO_PRINTF2(_L("iVideoPlayer -> MMdaVideoPlayerCallback PrepareComplete (%d)"), aError);
		Fsm(EMvpuoPrepareComplete, aError);
		}
	}
/**
 * MvpuoFrameReadyTest
 */
void RTestMmfVclntVideoPlayerAPR::MvpuoFrameReadyTest(CFbsBitmap &/*aFrame*/, TInt aError, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
	{
	if (&aVideoPlayerCallbackSupport == iVideoPlayerCallbackSupport1)
		{
		INFO_PRINTF2(_L("iVideoPlayer -> MMdaVideoPlayerCallback FrameReady (%d)"), aError);
		Fsm(EMvpuoFrameReady, aError);
		}
	}
/**
 * MvpuoPlayCompleteTest
 */
void RTestMmfVclntVideoPlayerAPR::MvpuoPlayCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
	{
	if (&aVideoPlayerCallbackSupport == iVideoPlayerCallbackSupport1)
		{
		Fsm(EMvpuoPlayComplete, aError);
		}
	}
/**
 * MvpuoEventTest
 */
void RTestMmfVclntVideoPlayerAPR::MvpuoEventTest(const TMMFEvent &aEvent, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
	{
	if (&aVideoPlayerCallbackSupport == iVideoPlayerCallbackSupport1)
		{
		INFO_PRINTF2(_L("iVideoPlayer -> MMdaVideoPlayerCallback Event (%d)"), aEvent.iErrorCode);
		Fsm(EMvpuoEvent, aEvent.iErrorCode);
		}
	}

/**
 * MapcInitCompleteTest
 */
void RTestMmfVclntVideoPlayerAPR::MapcInitCompleteTest(TInt aError, const TTimeIntervalMicroSeconds &/*aDuration*/, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)
	{
	if (&aMdaVideoPlayerCallbackSupport == iAudioPlayerCallbackSupport1)
		{
		Fsm(EMapcInitComplete1, aError);
		}
	else if (&aMdaVideoPlayerCallbackSupport == iAudioPlayerCallbackSupport2)
		{
		Fsm(EMapcInitComplete2, aError);
		}	
	}

/**
 * MapcPlayCompleteTest
 */
void RTestMmfVclntVideoPlayerAPR::MapcPlayCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)
	{
	if (&aMdaVideoPlayerCallbackSupport == iAudioPlayerCallbackSupport1)
		{
		Fsm(EMapcPlayComplete1, aError);
		}
	else if (&aMdaVideoPlayerCallbackSupport == iAudioPlayerCallbackSupport2)
		{
		Fsm(EMapcPlayComplete2, aError);
		}	
	}

/**
 * MarncResourceAvailableTest
 */
void RTestMmfVclntVideoPlayerAPR::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
	{
	if (aNotificationEventId == KMMFEventCategoryAudioResourceAvailable)
		{
		//Convert TDesc8 to TInt64
		TMMFTimeIntervalMicroSecondsPckg timeIntvbuf;
		timeIntvbuf.Copy(aNotificationData);
		TTimeIntervalMicroSeconds intPos = timeIntvbuf();
		iPosition = intPos;

		if (&aVideoPlayerCallbackSupport == iVideoPlayerCallbackSupport1)
			{
			Fsm(EMarncResourceAvailable1, KErrNone);
			}
		else if (&aVideoPlayerCallbackSupport == iAudioPlayerCallbackSupport1)
			{
			Fsm(EMarncResourceAvailable2, KErrNone);
			}
		else if (&aVideoPlayerCallbackSupport == iAudioPlayerCallbackSupport2)
			{
			Fsm(EMarncResourceAvailable3, KErrNone);
			}
		}
	else
		{
		INFO_PRINTF1(_L("MMMFAudioResourceNotificationCallback, but NOT KMMFEventCategoryAudioResourceAvailable"));
		}
	}

/**
 * DoTestStepCreateL
 */
TVerdict RTestMmfVclntVideoPlayerAPR::DoTestStepCreateL()
	{
	TPtrC filename;
	TVerdict ret = EPass;
	TRect rect, clipRect;
    //WDP::Increase cache size for paging, else we spend time thrashing
	if(SetCacheSize())
    	{
    	return EInconclusive;
    	}

	// Get Video file name
	if(!GetStringFromConfig(iSectName, _L("VideoFile1"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilename1);
	iFilename1.Append(filename);
	
	// Initialise Video Player
	INFO_PRINTF2(_L("Initialise CVideoPlayerUtility : %S, EMdaPriorityNormal"), &iFilename1);
	iVideoPlayerCallbackSupport1 = CTestAVPlayerAPRCallbackSupport::NewL(*this);
	CleanupStack::PushL(iVideoPlayerCallbackSupport1);
	iTestVideoPlayer = CVideoPlayerUtility::NewL(*iVideoPlayerCallbackSupport1, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality, iWs, *iScreen, *iWindow, rect, clipRect);
	CleanupStack::PushL(iTestVideoPlayer);
	
	// Get 1st Audio file name
	if(!GetStringFromConfig(iSectName, _L("AudioFile1"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilename2);
	iFilename2.Append(filename);
	
	// Initialise 1st Audio Player
	INFO_PRINTF2(_L("Initialise CMdaAudioPlayerUtility1 : %S, EMdaPriorityMax"), &iFilename2);
	iAudioPlayerCallbackSupport1 = CTestAVPlayerAPRCallbackSupport::NewL(*this); 
	CleanupStack::PushL(iAudioPlayerCallbackSupport1);
	iTestAudioPlayer1 = CMdaAudioPlayerUtility::NewL(*iAudioPlayerCallbackSupport1, EMdaPriorityMax);
	CleanupStack::PushL(iTestAudioPlayer1);
	
	// Get 2nd Audio file name
	if(!GetStringFromConfig(iSectName, _L("AudioFile2"), filename))
		{
		return EInconclusive;
		}
	GetDriveName(iFilename3);
	iFilename3.Append(filename);
	
	// Initialise 2nd Audio Player
	INFO_PRINTF2(_L("Initialise CMdaAudioPlayerUtility2 : %S, EMdaPriorityMin"), &iFilename3);
	iAudioPlayerCallbackSupport2 = CTestAVPlayerAPRCallbackSupport::NewL(*this); 
	CleanupStack::PushL(iAudioPlayerCallbackSupport2);
	iTestAudioPlayer2 = CMdaAudioPlayerUtility::NewL(*iAudioPlayerCallbackSupport2, EMdaPriorityMin);
	CleanupStack::PushL(iTestAudioPlayer2);
	
	// Initialise the CCallBackTimer.
	iCallBackTimer = CCallBackTimer::NewL(TCallBack(stopActiveScheduler));
	
	// pop all the items from the stack in case of no errors.#
	CleanupStack::Pop(6);
	return ret;
	}

/**
 * DoTestStepDeleteL
 */
TVerdict RTestMmfVclntVideoPlayerAPR::DoTestStepDeleteL()
	{
	// Delete all the AudioPlayer and VideoPlayer utilities.
	iTestVideoPlayer->Close();
	delete iTestVideoPlayer;
	iTestVideoPlayer=NULL;
	iTestAudioPlayer1->Close();
	delete iTestAudioPlayer1;
	iTestAudioPlayer1=NULL;
	iTestAudioPlayer2->Close();
	delete iTestAudioPlayer2;
	iTestAudioPlayer2=NULL;
	// Delete CTestAVPlayerAPRCallbackSupport
	delete iVideoPlayerCallbackSupport1;
	delete iAudioPlayerCallbackSupport1;
	delete iAudioPlayerCallbackSupport2;
	// Delete the CCallBackTimer.
	delete iCallBackTimer;
	iCallBackTimer=NULL;
	// return Pass
	return EPass;
	}

/**
 * DoTestStepL
 */
TVerdict RTestMmfVclntVideoPlayerAPR::DoTestStepL()
	{
	TVerdict ret = EPass;
	DoTestStepCreateL();
	ret = DoTestL();
	
	TUint defaultCacheSize = 0; 
	//Done with the test. Setting 0 makes the cache size revert back to boot up values
	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)defaultCacheSize,(TAny*)defaultCacheSize);
    if(CheckCacheError(error))
    	{
    	INFO_PRINTF1(_L("Could not revert the cache size to default"));
    	ret=EInconclusive;
    	}
    
	DoTestStepDeleteL();
	return ret;
	}

/**
 * StateInit
 */
void RTestMmfVclntVideoPlayerAPR::StateInit()
	{
	iCanStateChange = EFalse;
	for (TInt i=0; i< KMaxEvents; i++)
		{
		iExpectedEvent[i] = EIdlePlayer;
		iExpectedError[i] = KErrNone;
		}
	iState = EStateBegin;	
	}

/**
 * StateChange
 */
void RTestMmfVclntVideoPlayerAPR::StateChange(TAVPlayerAPRState aState, TAVPlayerAPREvents aExpectedEvent, TInt aExpectedError, TInt aEventIndex)
	{
	iCanStateChange = EFalse;
	iExpectedEvent[aEventIndex-1] = aExpectedEvent;
	iExpectedError[aEventIndex-1] = aExpectedError;
	iState = aState;
	iActEventCount = 0;
	}

/**
 * FsmCheck
 */
TBool RTestMmfVclntVideoPlayerAPR::FsmCheck(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	TBool retFsmCheck = EFalse;
#ifndef SYMBIAN_VARIABLE_BITRATE_CODEC	
	if((aEventCode == EMvpuoOpenComplete) && (aError == KErrNotSupported))
		{
		iTestStepResult = EKnownFailure;
		INFO_PRINTF1(_L("The AVI Play Controller is not available in this OS version, hence KNOWN FAILURE"));
		CActiveScheduler::Stop();
		return retFsmCheck;
		}
#endif
	if (aError == KErrNoMemory)
		{
		// Added to enable Alloc tests with Use Client heap...
		iError = aError;
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}
	else
		{
		// analyse if we can go to the next state.
		TInt analysisError = 0;
		analysisError = AnalyseInput(aEventCode, aError);
		if (analysisError == -1)
			{
			iError = aError;
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}
		else
			{
			retFsmCheck = ETrue;
			}
		}
	return retFsmCheck;
	}

/**
 * AnalyseInput
 * returns 1 = it can move to a new state
 * returns 0 = still waiting for more events
 * returns -1 = some error has occured
 */
 TInt RTestMmfVclntVideoPlayerAPR::AnalyseInput(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	TInt res = 0;
	TBool matchFound = EFalse;
	for(TInt i = 0; i < iExpEventCount; i++)
		{
		//check for client and event.
		if((iExpectedEvent[i] == aEventCode) && (iExpectedError[i] == aError))
			{
			iActEventCount++;
			matchFound = ETrue;
			break;
			}
		}
	
	if(matchFound == EFalse)
		{
		for(TInt i = 0; i < iExpEventCount; i++)
			{
			//check for client and event.
			ERR_PRINTF5(_L("Expectedevent[%d]=%d, Actualevent[%d]=%d"), i, iExpectedEvent[i], i, aEventCode);
			ERR_PRINTF5(_L("Expectederror[%d]=%d, Actualerror[%d]=%d"), i, iExpectedError[i], i, aError);
			}
		return -1;
		}

	if(iActEventCount == iExpEventCount)
		{
		iCanStateChange = ETrue;
		// Reset the event counters, for processing next event.
		iExpEventCount = 1;
		iActEventCount = 0;
		}
	else
		{
		iCanStateChange = EFalse;
		}	
		
	return res;
	}

 TInt RTestMmfVclntVideoPlayerAPR::SetCacheSize()
 	{
 	TInt error=UserSvr::HalFunction(EHalGroupVM,EVMHalSetCacheSize,(TAny*)KCacheSize,(TAny*)KMaxCacheSize);
	TInt ret=CheckCacheError(error);
 		
 	return ret;
 	}

 TInt RTestMmfVclntVideoPlayerAPR::CheckCacheError(TInt aError)
 	{
 	TInt ret=KErrNone;
 	#ifdef __WINSCW__
 	//Winscw does not support getting cache size. Ignoring -5 error
 	if(aError!=KErrNone)
 		{
 		if(aError!=KErrNotSupported)
 			{
 			INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
 			ret=aError;
 			}
 		}
 	#else
 	if(aError!=KErrNone)
 		{//For ARMV5 we stop for all errors
 		INFO_PRINTF2(_L("Could not get the cache size  %d"),aError);
 		ret=aError;
 		}
 	#endif
 	return ret;
 	}


//
//POSITIVE TESTS//
//

//
//RTestVclntAprRegOnce4SameEvent (MM-MMF-VCLNTAVIAPR-I-0001-HP)
//

/**
 * Constructor
 */
RTestVclntAprRegOnce4SameEvent::RTestVclntAprRegOnce4SameEvent(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprRegOnce4SameEvent* RTestVclntAprRegOnce4SameEvent::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegOnce4SameEvent* self = new (ELeave) RTestVclntAprRegOnce4SameEvent(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprRegOnce4SameEvent::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprRegOnce4SameEvent::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((FsmCheck(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				INFO_PRINTF1(_L("iTestVideoPlayer-> Received Notification 1st time"));
				iTestVideoPlayer->Play();
				#ifndef __EABI__
				INFO_PRINTF1(_L("iTestVideoPlayer->Play() - Again"));
				#endif
				iTestAudioPlayer1->Play();
				StateChange(EStateErrInUseVideoPlayer1A, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer1A:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer1A, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1A, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1A:
				{
				// we have received all the events, so pass the test
				INFO_PRINTF1(_L("iTestVideoPlayer-> Received Notification 2nd time"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprRegOnce4SameEvent::PerformTestL()
	{
	INFO_PRINTF1(_L("Register Once for same event"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}
	
//
//RTestVclntAprRegB4Open (MM-MMF-VCLNTAVIAPR-I-0002-HP)
//

/**
 * Constructor
 */
RTestVclntAprRegB4Open::RTestVclntAprRegB4Open(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprRegB4Open* RTestVclntAprRegB4Open::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegB4Open* self = new (ELeave) RTestVclntAprRegB4Open(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprRegB4Open::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprRegB4Open::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((FsmCheck(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}				
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iTestAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}				
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));	
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				// we have received all the events, so pass the test
				INFO_PRINTF1(_L("iTestVideoPlayer->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprRegB4Open::PerformTestL()
	{
	INFO_PRINTF1(_L("Register before opening video player"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}
	
//
//RTestVclntAprRegAftrOpenAndB4Play (MM-MMF-VCLNTAVIAPR-I-0003-HP)
//

/**
 * Constructor
 */
RTestVclntAprRegAftrOpenAndB4Play::RTestVclntAprRegAftrOpenAndB4Play(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{
	 iVidPosition = 0;
	}

/**
 * NewL
 */
RTestVclntAprRegAftrOpenAndB4Play* RTestVclntAprRegAftrOpenAndB4Play::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegAftrOpenAndB4Play* self = new (ELeave) RTestVclntAprRegAftrOpenAndB4Play(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprRegAftrOpenAndB4Play::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprRegAftrOpenAndB4Play::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((FsmCheck(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iTestAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Get position for iTestVideoPlayer
				TRAP(iError, iVidPosition = iTestVideoPlayer->PositionL());
				INFO_PRINTF3(_L("iTestVideoPlayer->Position returned %d, error %d"), I64LOW(iVidPosition.Int64()), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				INFO_PRINTF2(_L("iTestVideoPlayer->Position returned after receiving Notification = %d"), I64LOW(iPosition.Int64()));
				// we have received all the events, so pass the test
				// Check the position
				CActiveScheduler::Stop();
				// check the difference with a time dalay
				if(TimeComparison(I64INT(iPosition.Int64()), I64INT(iVidPosition.Int64()), KExpectedDeviation))
					{
					INFO_PRINTF1(_L("Position matches..."));
					iTestStepResult = EPass;
					}
				else
					{
					INFO_PRINTF1(_L("Position does not match..."));
					iTestStepResult = EFail;
					}
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprRegAftrOpenAndB4Play::PerformTestL()
	{
	INFO_PRINTF1(_L("Register after opening and before playing video player"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
		
	return iTestStepResult;
	}
	
//
//RTestVclntAprRegAftrHpClntCompltPlay (MM-MMF-VCLNTAVIAPR-I-0004-HP)
//

/**
 * Constructor
 */
RTestVclntAprRegAftrHpClntCompltPlay::RTestVclntAprRegAftrHpClntCompltPlay(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprRegAftrHpClntCompltPlay* RTestVclntAprRegAftrHpClntCompltPlay::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegAftrHpClntCompltPlay* self = new (ELeave) RTestVclntAprRegAftrHpClntCompltPlay(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprRegAftrHpClntCompltPlay::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprRegAftrHpClntCompltPlay::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((FsmCheck(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iTestAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}				
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Set ExpectedEvent and change the State
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1);
				break;
				}
			case EStateEndAudioPlay1:
				{
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Play iTestVideoPlayer again
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play() - Again"));
				// Play iTestAudioPlayer1 again
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play() - Again"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer1A, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer1A:
				{
				iCallBackTimer->Cancel(); // in case some other event (apart from Notification is received)
				// Set ExpectedEvent and change the State 
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer1A, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1A, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1A:
				{
				// we have received all the events, so pass the test
				INFO_PRINTF1(_L("iTestVideoPlayer->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprRegAftrHpClntCompltPlay::PerformTestL()
	{
	INFO_PRINTF1(_L("Registration after the the High priority client completes play"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}

//
//RTestVclntAprRegSamePriorityPlayers (MM-MMF-VCLNTAVIAPR-I-0005-HP)
//

/**
 * Constructor
 */
RTestVclntAprRegSamePriorityPlayers::RTestVclntAprRegSamePriorityPlayers(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprRegSamePriorityPlayers* RTestVclntAprRegSamePriorityPlayers::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegSamePriorityPlayers* self = new (ELeave) RTestVclntAprRegSamePriorityPlayers(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprRegSamePriorityPlayers::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprRegSamePriorityPlayers::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((FsmCheck(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		TInt retErr1 = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
   				    TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iTestAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}				
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioPlayer2
				TRAP(iError, iTestAudioPlayer2->OpenFileL(iFilename3));
				INFO_PRINTF2(_L("iTestAudioPlayer2->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}		
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer2, EMapcInitComplete2);
				break;
				}
			case EStateOpenAudioPlayer2:
				{
				// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseAudioPlayer2, EMapcPlayComplete2, KErrInUse);
				break;
				}
			case EStateErrInUseAudioPlayer2:
				{
				// Register iTestAudioPlayer2 for Notification
				retErr1 = iTestAudioPlayer2->RegisterAudioResourceNotification(*iAudioPlayerCallbackSupport2, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer2->RegisterAudioResourceNotification()"));
				if( retErr1 != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer2->RegisterAudioResourceNotification - Failed with Error : %d"), retErr1);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				// we have received all the events, so pass the test
				INFO_PRINTF1(_L("iTestVideoPlayer->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprRegSamePriorityPlayers::PerformTestL()
	{
	INFO_PRINTF1(_L("Receive notification by same priority players in the order of registration"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}

//
//RTestVclntAprRegAtPlayAndCancelNfn (MM-MMF-VCLNTAVIAPR-I-0006-HP)
//

/**
 * Constructor
 */
RTestVclntAprRegAtPlayAndCancelNfn::RTestVclntAprRegAtPlayAndCancelNfn(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprRegAtPlayAndCancelNfn* RTestVclntAprRegAtPlayAndCancelNfn::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegAtPlayAndCancelNfn* self = new (ELeave) RTestVclntAprRegAtPlayAndCancelNfn(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprRegAtPlayAndCancelNfn::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprRegAtPlayAndCancelNfn::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		TInt retErr1 = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iTestAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}				
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				retErr1 = iTestVideoPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->CancelRegisterAudioResourceNotification()"));
				if (retErr1 == KErrNone)
					{
					INFO_PRINTF1(_L("iTestVideoPlayer should not receive Notification as registration is cancelled."));
					}
				else
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->CancelRegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1);
				break;
				}
			case EStateEndAudioPlay1:
				{
				// Timer to check if Notification Event Occurs.
				iCallBackTimer->After(2000000);
				iTestStepResult = EPass;
				// Set ExpectedEvent and change the State
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1);
				break;
				}
			case EStateNotifiedVideoPlayer:
				{
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				// we have received the notification event (unexpected), so fail the test
				INFO_PRINTF1(_L("iTestVideoPlayer->NotificationReceived"));
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprRegAtPlayAndCancelNfn::PerformTestL()
	{
	INFO_PRINTF1(_L("Cancellation of the notification request"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}

//
//RTestVclntAprTestWillResumePlay (MM-MMF-VCLNTAVIAPR-I-0007-HP)
//

/**
 * Constructor
 */
RTestVclntAprTestWillResumePlay::RTestVclntAprTestWillResumePlay(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprTestWillResumePlay* RTestVclntAprTestWillResumePlay::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprTestWillResumePlay* self = new (ELeave) RTestVclntAprTestWillResumePlay(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprTestWillResumePlay::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprTestWillResumePlay::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((FsmCheck(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		TInt retErr1 = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestAudioPlayer2 - Low priority
				TRAP(iError, iTestAudioPlayer2->OpenFileL(iFilename3));
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer2->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer2, EMapcInitComplete2);
				break;
				}
			case EStateOpenAudioPlayer2:
				{
			 	// Play iTestAudioPlayer#2
				iTestAudioPlayer2->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer2->Play()"));
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				INFO_PRINTF2(_L("iTestVideoPlayer->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}	
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseAudioPlayer2, EMapcPlayComplete2, KErrInUse);
				break;
				}
			case EStateErrInUseAudioPlayer2:
				{
				// Register iTestAudioPlayer2 for Notification
				retErr = iTestAudioPlayer2->RegisterAudioResourceNotification(*iAudioPlayerCallbackSupport2, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestAudioPlayer2->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestAudioPlayer2->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iTestAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}				
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Register iTestVideoPlayer for Notification
				retErr1 = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr1 != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr1);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				INFO_PRINTF1(_L("iTestVideoPlayer->NotificationReceived"));
				retErr = iTestVideoPlayer->WillResumePlay();
				INFO_PRINTF1(_L("iTestVideoPlayer->WillResumePlay()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->WillResumePlay() - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// initialise and start the timer (5 Sec), just to check if no other event occurs.
                TRAPD(err, iWillResumePlayTimer = CVclntAviAprWillResumeTimer::NewL(this, 5000000));
                if (err != KErrNone)
                    {
                    ERR_PRINTF2(_L("CVclntAviAprWillResumeTimer::NewL left with error = %d"), err);
                    iTestStepResult = EFail;
					CActiveScheduler::Stop();
                    break;
                    }
                iWillResumePlayTimer->Start();
				// Check if No other client is notified in 5 seconds.
				StateChange(EStateNoOtherPlayerNotified, EIdlePlayer);
				break;
				}
			case EStateNoOtherPlayerNotified:
				{
				INFO_PRINTF1(_L("No other client has been notified in 5 seconds after calling WillResumePlay()"));
				iTestVideoPlayer->Play(); // directly calling play, without setting position.
				iExpEventCount = 2;
				StateChange(EStateNotifiedAudioPlayer2, EMarncResourceAvailable3, KErrNone, 1);
				StateChange(EStateEndVideoPlay, EMvpuoPlayComplete, KErrNone, 2);
				break;
				}
			case EStateEndVideoPlay:
				{
				INFO_PRINTF1(_L("iTestAudioPlayer2->NotificationReceived"));
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprTestWillResumePlay::PerformTestL()
	{
	INFO_PRINTF1(_L("WillResumePlay Implementation with three clients"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}

/**
 * set finished on callback from a timer
 */
void RTestVclntAprTestWillResumePlay::FinishedL()
    {
    // say that no events have occured after calling WillResumePlay()
    // and can proceed to the next step
    Fsm(EIdlePlayer, KErrNone);
    }

//
// CVclntAviAprWillResumeTimer
//

/**
 * NewL
 */
CVclntAviAprWillResumeTimer* CVclntAviAprWillResumeTimer::NewL(RTestVclntAprTestWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    {
    CVclntAviAprWillResumeTimer* self = new (ELeave) CVclntAviAprWillResumeTimer(aParent, aWaitTime);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }
    
/**
 * Constructor
 */
CVclntAviAprWillResumeTimer::CVclntAviAprWillResumeTimer(RTestVclntAprTestWillResumePlay* aParent, TTimeIntervalMicroSeconds32 aWaitTime)
    : CTimer(EPriorityNormal), iParent(aParent), iWaitTime(aWaitTime)
    {    
    CActiveScheduler::Add(this);
    }    
    
/**
 * Start() - Start the timer
 */
void CVclntAviAprWillResumeTimer::Start()
    {
    After(iWaitTime);
    }

/**
 * RunL() - see CActive class for more info
 */
void CVclntAviAprWillResumeTimer::RunL()
    {        
    // (iParent->Finished() should call Stop() or not, as we decide)    
    iParent->FinishedL();
    }

/**
 * RunError() - see CActive class for more info
 */
TInt CVclntAviAprWillResumeTimer::RunError(TInt aError)
    {
    CActiveScheduler::Stop();
    return aError;
    }

//
//RTestVclntAprAllocUsingSharedHeap (MM-MMF-VCLNTAVIAPR-I-0008-HP)
//

/**
 * Constructor
 */
RTestVclntAprAllocUsingSharedHeap::RTestVclntAprAllocUsingSharedHeap(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{
	}

/**
 * NewL
 */
RTestVclntAprAllocUsingSharedHeap* RTestVclntAprAllocUsingSharedHeap::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprAllocUsingSharedHeap* self = new (ELeave) RTestVclntAprAllocUsingSharedHeap(aTestName, aSectName);
	return self;
	}

/**
 * DoTestStepL
 */
TVerdict RTestVclntAprAllocUsingSharedHeap::DoTestStepL()
	{
	TVerdict ret = EPass;
	ret = DoTestL();
	return ret;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprAllocUsingSharedHeap::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */

void RTestVclntAprAllocUsingSharedHeap::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if ((FsmCheck(aEventCode, aError)) && iCanStateChange)
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));		
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer:
				{
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1:
				{
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play() - Again"));
				iTestAudioPlayer1->Play();
				StateChange(EStateErrInUseVideoPlayer1A, EMvpuoPlayComplete, KErrInUse);
				break;
				}
			case EStateErrInUseVideoPlayer1A:
				{
				// Set ExpectedEvent and change the State
				iExpEventCount = 2;
				StateChange(EStateNotifiedVideoPlayer1A, EMarncResourceAvailable1, KErrNone, 1);
				StateChange(EStateEndAudioPlay1A, EMapcPlayComplete1, KErrNone, 2);
				break;
				}
			case EStateEndAudioPlay1A:
				{
				iCallBackTimer->Cancel(); // in case something else stopped the AS
				iTestStepResult = EPass;
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
/**
 *
 * Play video by marking heap.
 *
 */ 
void RTestVclntAprAllocUsingSharedHeap::VideoPlayAllocL()
    {
    __MM_HEAP_MARK;
    // create new instance of video player
    TInt startAllocCount = User::CountAllocCells();
    
	DoTestStepCreateL();
	
	// call the state handler and start the test
    iError = KErrNone;
   	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();

	DoTestStepDeleteL();
	
    // check for errors
    if (iError != KErrNone)
        {
        ERR_PRINTF2(_L("Opening/playing the video file failed, unexpected error : %d."), iError);
        User::Leave(iError);
        }
    
    TInt endAllocCount = User::CountAllocCells();
    __MM_HEAP_MARKEND;
    }

/**
 *
 * PerformTestL()
 *
 */
TVerdict RTestVclntAprAllocUsingSharedHeap::PerformTestL()
    {
    INFO_PRINTF1(_L("<<< APR alloc test using VideoPlayer utility.>>>"));
    iTestStepResult = EFail;
    
    TRAPD(err, VideoPlayAllocL());
	if ((err != KErrNone) || (iTestStepResult != EPass))
        {
        return iTestStepResult;
        }
	
    iTestStepResult = EFail;

    TInt failCount = 1;
    TBool completed = EFalse;
    TBool badResult = EFalse;
    TBool reachedEnd = EFalse;

    for(;;)
        {
        INFO_PRINTF2(_L("ALLOC Test Loop #%d."), failCount);
        __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
        __MM_HEAP_MARK;

        TInt err = KErrNone;
        TRAP(err, VideoPlayAllocL());
        ERR_PRINTF2(_L("Tracing the  error = %d."), err);

        completed = EFalse;
        if (err == KErrNone)
            {
            TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
            if (!testAlloc)
                {
                reachedEnd = ETrue;
                failCount--;
                }
            else
                {
                User::Free(testAlloc);
                testAlloc = NULL;
                }

            if (iTestStepResult != EPass)
                {
                badResult = ETrue;
                }

            completed = reachedEnd || badResult;
            }
        else if (err != KErrNoMemory)
            {
            __UHEAP_SETFAIL(RHeap::ENone, 0);
            ERR_PRINTF2(_L("Bad error code %d"), err);
            completed = ETrue;
            badResult = EFail;
            }

        __MM_HEAP_MARKEND;
        __UHEAP_SETFAIL(RHeap::ENone, 0);

        if (completed)
            {
            break;
            }

        failCount++;
        }

    if (err != KErrNone || badResult)
        {
        if (badResult)
            {
            ERR_PRINTF3(_L("Bad result with %d memory allocations tested. The Error returned is %d."), failCount, err);
            }
        else
            {
            ERR_PRINTF3(_L("Error(%d) with %d memory allocations tested."), failCount, err);
            }

        iTestStepResult = EFail;
        }
    else
        {
        INFO_PRINTF2(_L("Completed OK with %d memory allocations tested.\n"), failCount);
        iTestStepResult = EPass;
        }

    return iTestStepResult;
    }

//
//RTestVclntAprMultRegAtPlay (MM-MMF-VCLNTAVIAPR-I-0101-HP)
//

/**
 * Constructor
 */
RTestVclntAprMultRegAtPlay::RTestVclntAprMultRegAtPlay(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprMultRegAtPlay* RTestVclntAprMultRegAtPlay::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprMultRegAtPlay* self = new (ELeave) RTestVclntAprMultRegAtPlay(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprMultRegAtPlay::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprMultRegAtPlay::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				if( iError != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->OpenFileL - Failed with Error : %d"), iError);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification() - First Regn"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}		
				// Open iTestAudioPlayer1
				TRAP(iError, iTestAudioPlayer1->OpenFileL(iFilename2));
				INFO_PRINTF2(_L("iTestAudioPlayer1->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}				
				// Set ExpectedEvent and change the State
				StateChange(EStateOpenAudioPlayer1, EMapcInitComplete1);
				break;
				}
			case EStateOpenAudioPlayer1:
				{
				// Play iTestAudioPlayer#1
				iTestAudioPlayer1->Play();
				INFO_PRINTF1(_L("iTestAudioPlayer1->Play()"));
				// Set ExpectedEvent and change the State
				StateChange(EStateErrInUseVideoPlayer, EMvpuoPlayComplete, KErrInUse);
				break; 
				}
			case EStateErrInUseVideoPlayer:
				{
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification() - Second Regn"));
				if( retErr == KErrAlreadyExists )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification (2nd Time) - Failed with Error : %d"), retErr);
					iTestStepResult = EPass;
					}
				else if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprMultRegAtPlay::PerformTestL()
	{
	INFO_PRINTF1(_L("Multiple registration at the time of play"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}


//
//RTestVclntAprCancelNoResourceNfn (MM-MMF-VCLNTAVIAPR-I-0102-HP)
//

/**
 * Constructor
 */
RTestVclntAprCancelNoResourceNfn::RTestVclntAprCancelNoResourceNfn(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprCancelNoResourceNfn* RTestVclntAprCancelNoResourceNfn::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprCancelNoResourceNfn* self = new (ELeave) RTestVclntAprCancelNoResourceNfn(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprCancelNoResourceNfn::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprCancelNoResourceNfn::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				INFO_PRINTF2(_L("iTestVideoPlayer->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}	
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				retErr = iTestVideoPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryAudioResourceAvailable);
				if (retErr == KErrCancel)
					{
					INFO_PRINTF1(_L("iTestVideoPlayer->CancelRegisterAudioResourceNotification() returned KErrCancel - Successful"));
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->CancelRegisterAudioResourceNotification - Failed with Error : %d"), retErr);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprCancelNoResourceNfn::PerformTestL()
	{
	INFO_PRINTF1(_L("Cancel when no resource Notification is requested"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}

//
//RTestVclntAprRegUnsupportedEvent (MM-MMF-VCLNTAVIAPR-I-0103-HP)
//

/**
 * Constructor
 */
RTestVclntAprRegUnsupportedEvent::RTestVclntAprRegUnsupportedEvent(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprRegUnsupportedEvent* RTestVclntAprRegUnsupportedEvent::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprRegUnsupportedEvent* self = new (ELeave) RTestVclntAprRegUnsupportedEvent(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprRegUnsupportedEvent::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprRegUnsupportedEvent::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				INFO_PRINTF2(_L("iTestVideoPlayer->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}	
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryVideoPrepareComplete);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr == KErrNotSupported )
					{
					INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification() with an Unsupported Event"));
					iTestStepResult = EPass;
					}
					else
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification with Error : %d"), retErr);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprRegUnsupportedEvent::PerformTestL()
	{
	INFO_PRINTF1(_L("Register with Unsupported event"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
	
	return iTestStepResult;
	}
	
//
//RTestVclntAprCancelUnsupportedEvent (MM-MMF-VCLNTAVIAPR-I-0104-HP)
//

/**
 * Constructor
 */
RTestVclntAprCancelUnsupportedEvent::RTestVclntAprCancelUnsupportedEvent(const TDesC& aTestName, const TDesC& aSectName)
	: RTestMmfVclntVideoPlayerAPR(aTestName, aSectName)
	{}

/**
 * NewL
 */
RTestVclntAprCancelUnsupportedEvent* RTestVclntAprCancelUnsupportedEvent::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	RTestVclntAprCancelUnsupportedEvent* self = new (ELeave) RTestVclntAprCancelUnsupportedEvent(aTestName, aSectName);
	return self;
	}

/**
 * DoTestL
 */
TVerdict RTestVclntAprCancelUnsupportedEvent::DoTestL()
	{
	TVerdict testResult = EFail;
	TRAPD(err, testResult = PerformTestL());
	User::LeaveIfError(err);
	return testResult;
	}

/**
 * FsmL
 */
void RTestVclntAprCancelUnsupportedEvent::Fsm(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	if (FsmCheck(aEventCode, aError))
		{
		TInt retErr = KErrNone;
		TInt retErr1 = KErrNone;
		switch (iState)
			{
			case EStateBegin:
				{
				// Open iTestVideoPlayer
				if (iBinaryCompatibility)
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1, ControllerUid()));
					}
				else
					{
					TRAP(iError, iTestVideoPlayer->OpenFileL(iFilename1));
					}
				
				INFO_PRINTF2(_L("iTestVideoPlayer->OpenFileL(), returned %d"), iError);
				if( iError != KErrNone )
					{
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}	
				StateChange(EStateOpenVideoPlayer, EMvpuoOpenComplete);
				break;
				}
			case EStateOpenVideoPlayer:
				{
				// Prepare iTestVideoPlayer
				iTestVideoPlayer->Prepare();
				INFO_PRINTF1(_L("iTestVideoPlayer->Prepare()"));
				StateChange(EStatePrepareVideoPlayer, EMvpuoPrepareComplete);
				break;
				}
			case EStatePrepareVideoPlayer:
				{
				// Play iTestVideoPlayer
				iTestVideoPlayer->Play();
				INFO_PRINTF1(_L("iTestVideoPlayer->Play()"));
				// Register iTestVideoPlayer for Notification
				retErr = iTestVideoPlayer->RegisterAudioResourceNotification(*iVideoPlayerCallbackSupport1, KMMFEventCategoryAudioResourceAvailable);
				INFO_PRINTF1(_L("iTestVideoPlayer->RegisterAudioResourceNotification()"));
				if( retErr != KErrNone )
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->RegisterAudioResourceNotification - Failed with Error : %d"), retErr1);
					iTestStepResult = EFail;
					CActiveScheduler::Stop();
					break;
					}
				retErr1 = iTestVideoPlayer->CancelRegisterAudioResourceNotification(KMMFEventCategoryVideoRecorderGeneralError);
				if (retErr1 == KErrNotSupported)
					{
					INFO_PRINTF1(_L("iTestVideoPlayer->CancelRegisterAudioResourceNotification - Unsuccessful due to Unsupported event"));
					iTestStepResult = EPass;
					}
				else
					{
					ERR_PRINTF2(_L("iTestVideoPlayer->CancelRegisterAudioResourceNotification - Failed with Error : %d"), retErr1);
					iTestStepResult = EFail;
					}
				CActiveScheduler::Stop();
				break;
				}
			}
		}
	}
		
	
/**
 * PerformTestL
 */
TVerdict RTestVclntAprCancelUnsupportedEvent::PerformTestL()
	{
	INFO_PRINTF1(_L("Cancel registration with unsupported event type"));
	iTestStepResult = EFail;
	
	// Initialise the state variables
	StateInit();
	
	// Begin the process
	Fsm(EIdlePlayer, KErrNone);
	
	// Start the scheduler - Done only once !
	CActiveScheduler::Start();
	
	User::LeaveIfError(iError);
		
	return iTestStepResult;
	}
