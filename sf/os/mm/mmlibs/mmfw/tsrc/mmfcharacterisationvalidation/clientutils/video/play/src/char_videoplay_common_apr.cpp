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
//

#include "char_videoplay_common_apr.h"


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
//RTestBaseVideoPlayAPR
//

/**
 * Constructor
 */
RTestBaseVideoPlayAPR::RTestBaseVideoPlayAPR(const TDesC& aTestName, const TDesC& aSectName) :
	iAudioPlayer1(NULL),
	iAudioPlayer2(NULL),
	iVideoPlayer(NULL),
	iState(EStateBegin),
	iTimer(NULL),
	iSectName(aSectName)
	{
	iTestStepName = aTestName;
	iHeapSize = 1000000;
	iExpEventCount = 1;
	iActEventCount = 0;
	}

/**
 * MvpuoOpenCompleteTest
 */
void RTestBaseVideoPlayAPR::MvpuoOpenCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
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
void RTestBaseVideoPlayAPR::MvpuoPrepareCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
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
void RTestBaseVideoPlayAPR::MvpuoFrameReadyTest(CFbsBitmap &/*aFrame*/, TInt aError, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
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
void RTestBaseVideoPlayAPR::MvpuoPlayCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
	{
	if (&aVideoPlayerCallbackSupport == iVideoPlayerCallbackSupport1)
		{
		INFO_PRINTF2(_L("iVideoPlayer -> MMdaVideoPlayerCallback Playcomplete (%d)"), aError);
		Fsm(EMvpuoPlayComplete, aError);
		}
	}
/**
 * MvpuoEventTest
 */
void RTestBaseVideoPlayAPR::MvpuoEventTest(const TMMFEvent &aEvent, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
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
void RTestBaseVideoPlayAPR::MapcInitCompleteTest(TInt aError, const TTimeIntervalMicroSeconds &/*aDuration*/, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)
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
void RTestBaseVideoPlayAPR::MapcPlayCompleteTest(TInt aError, CTestAVPlayerAPRCallbackSupport& aMdaVideoPlayerCallbackSupport)
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
void RTestBaseVideoPlayAPR::MarncResourceAvailableTest(TUid aNotificationEventId, const TDesC8& aNotificationData, CTestAVPlayerAPRCallbackSupport& aVideoPlayerCallbackSupport)
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
 * RTestBaseVideoPlayAPR::InitWservL()
 */
void RTestBaseVideoPlayAPR::InitWservL()
    {
    TInt err = iWs.Connect();
    if (err != KErrNone)
        {
        // Access violation if ws is null
        INFO_PRINTF1(_L("Cannot test, no window server available"));
        User::Leave(err);
        }

    iScreenDevice = new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreenDevice->Construct()); // and complete its construction

    RWindowGroup iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

    iWindow = new(ELeave) RWindow(iWs);
    User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
    iWindow->SetExtent(TPoint(0,0), TSize(100,100));
    iWindow->SetVisible(ETrue);
    iWindow->Activate();
    iWs.Flush();
    }

/**
 * RTestBaseVideoPlayAPR::DeInitWserv
 */
void RTestBaseVideoPlayAPR::DeInitWserv()
    {
    if (iWindow)
        {
        iWindow->Close();
        delete iWindow;
        iWindow = NULL;
        }
    iRootWindow.Close();
    delete iScreenDevice;
    iScreenDevice = NULL;
    iWs.Flush();
    iWs.Close();
    }

// start test
void RTestBaseVideoPlayAPR::KickoffTestL()
	{
	// re-initialise data - orphan any pointer, as would relate to
	// previous run's heap
	iVideoPlayer = NULL;
	iAudioPlayer1 = NULL;
	iAudioPlayer2 = NULL;
	iTimer = NULL;
	
	TPtrC filename;
	TRect rect, clipRect;
	
	// Get Video file name
	if(!GetStringFromConfig(iSectName, _L("VideoFile1"), filename))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	GetDriveName(iFilename1);
	iFilename1.Append(filename);

	//Initialise the window server
	InitWservL();
	// Initialise Video Player
	INFO_PRINTF2(_L("Initialise CVideoPlayerUtility : %S, EMdaPriorityNormal"), &iFilename1);
	iVideoPlayerCallbackSupport1 = CTestAVPlayerAPRCallbackSupport::NewL(*this);
	CleanupStack::PushL(iVideoPlayerCallbackSupport1);
	iVideoPlayer = CVideoPlayerUtility::NewL(*iVideoPlayerCallbackSupport1, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality, iWs, *iScreenDevice, *iWindow, rect, clipRect);
	CleanupStack::PushL(iVideoPlayer);
	
	// Get 1st Audio file name
	if(!GetStringFromConfig(iSectName, _L("AudioFile1"), filename))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	GetDriveName(iFilename2);
	iFilename2.Append(filename);
	
	// Initialise 1st Audio Player
	INFO_PRINTF2(_L("Initialise CMdaAudioPlayerUtility1 : %S, EMdaPriorityMax"), &iFilename2);
	iAudioPlayerCallbackSupport1 = CTestAVPlayerAPRCallbackSupport::NewL(*this); 
	CleanupStack::PushL(iAudioPlayerCallbackSupport1);
	iAudioPlayer1 = CMdaAudioPlayerUtility::NewL(*iAudioPlayerCallbackSupport1, EMdaPriorityMax);
	CleanupStack::PushL(iAudioPlayer1);
	
	// Get 2nd Audio file name
	if(!GetStringFromConfig(iSectName, _L("AudioFile2"), filename))
		{
		ERR_PRINTF1(_L("ERROR : Cannot Get value from Config."));
		}
	GetDriveName(iFilename3);
	iFilename3.Append(filename);
	
	// Initialise 2nd Audio Player
	INFO_PRINTF2(_L("Initialise CMdaAudioPlayerUtility2 : %S, EMdaPriorityMin"), &iFilename3);
	iAudioPlayerCallbackSupport2 = CTestAVPlayerAPRCallbackSupport::NewL(*this); 
	CleanupStack::PushL(iAudioPlayerCallbackSupport2);
	iAudioPlayer2 = CMdaAudioPlayerUtility::NewL(*iAudioPlayerCallbackSupport2, EMdaPriorityMin);
	CleanupStack::PushL(iAudioPlayer2);
	
	// Timer class callback 
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	iCallBack = TCallBack(TimerCallback, this);

	// the derived class should implement DoKickoffTestL()
	DoKickoffTestL();

	// pop all the items from the stack in case of no errors.
	CleanupStack::Pop(6);
	}

/**
 * StateInit
 */
void RTestBaseVideoPlayAPR::StateInit()
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
void RTestBaseVideoPlayAPR::StateChange(TAVPlayerAPRState aState, TAVPlayerAPREvents aExpectedEvent, TInt aExpectedError, TInt aEventIndex)
	{
	iCanStateChange = EFalse;
	iExpectedEvent[aEventIndex-1] = aExpectedEvent;
	iExpectedError[aEventIndex-1] = aExpectedError;
	iState = aState;
	iActEventCount = 0;
	}

/**
 *  Validate the events after the callback
 */
TBool RTestBaseVideoPlayAPR::ValidateEvent(TAVPlayerAPREvents aEventCode, TInt aError)
	{
	TBool valid = EFalse;
#ifndef SYMBIAN_VARIABLE_BITRATE_CODEC	
	if((aEventCode == EMvpuoOpenComplete) && (aError == KErrNotSupported))
		{
		INFO_PRINTF1(_L("The AVI Play Controller is not available in this OS version, hence KNOWN FAILURE"));
		StopTest(iError, EKnownFailure);
		}
#endif
		// analyse if we can go to the next state.
		TInt analysisError = 0;
		analysisError = AnalyseInput(aEventCode, aError);
		if (analysisError == -1)
			{
			iError = aError;
			StopTest(iError, EFail);
			}
		else
			{
			valid = ETrue;
			}
	return valid;
	}

/**
 * AnalyseInput
 * returns 1 = it can move to a new state
 * returns 0 = still waiting for more events
 * returns -1 = some error has occured
 */
 TInt RTestBaseVideoPlayAPR::AnalyseInput(TAVPlayerAPREvents aEventCode, TInt aError)
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

	if((iActEventCount == iExpEventCount))
		{
		ERR_PRINTF3(_L("Expectedeventcount= %d, Actualeventcount= %d"), iExpEventCount, iActEventCount);
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


// cleanup at end
void RTestBaseVideoPlayAPR::CloseTest()
	{
	// Delete all the AudioPlayer and VideoPlayer utilities.
	iVideoPlayer->Close();
	delete iVideoPlayer;
	iVideoPlayer=NULL;
	iAudioPlayer1->Close();
	delete iAudioPlayer1;
	iAudioPlayer1=NULL;
	iAudioPlayer2->Close();
	delete iAudioPlayer2;
	iAudioPlayer2=NULL;
	// Delete CTestAVPlayerAPRCallbackSupport
	delete iVideoPlayerCallbackSupport1;
	delete iAudioPlayerCallbackSupport1;
	delete iAudioPlayerCallbackSupport2;
	// Delete the CCallBackTimer.
	delete iTimer;
	iTimer = NULL;
	
	// Destroy Window server objects
    DeInitWserv();
	}


// timer callback
TInt RTestBaseVideoPlayAPR::TimerCallback(TAny* aPtr)
	{
	static_cast<RTestBaseVideoPlay*>(aPtr)->DoTimerCallback();
	return KErrNone;
	}

void RTestBaseVideoPlayAPR::DoTimerCallback()
	{
	// The derived class may provide implementation
	// this is to avoid link errors
	}
	
