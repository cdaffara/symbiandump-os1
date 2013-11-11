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
//

#include <testframework.h>
#include <mmf/common/mmfvideosubtitlecustomcommands.h>
#include "teststepvidplayersubtitle.h"
#include "TS_Codes.h"


_LIT(KClockFileName, "c:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy");		// file not supporting subtitle
_LIT(KClock2FileName, "c:\\mm\\mmf\\testfiles\\vclnt\\clock2.dummy");	// file with subtitle support

// dummy crp id for testing
const TInt dummyCrpId = 1234;

/*
Subtitle availability test
MM-MMF-VCLNT-U-1300-CP
*/
CTestStepVidPlayerSubtitleAvailable* CTestStepVidPlayerSubtitleAvailable::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleAvailable* self = new(ELeave) CTestStepVidPlayerSubtitleAvailable(aTestName);
	return self;
	}

CTestStepVidPlayerSubtitleAvailable::CTestStepVidPlayerSubtitleAvailable(const TDesC& aTestName)
	{
	iTestStepName = aTestName;
	iFileName.Set(KClock2FileName);
	iVpu2 = ETrue;
	
	TUid uid = TUid::Uid(0x101f72B4); //  Test Custom Commands Uid
	iDestPckg = TMMFMessageDestinationPckg(uid);
	}
	
TVerdict CTestStepVidPlayerSubtitleAvailable::DoTestStepPreambleL()
	{
	TVerdict ret = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (EPass == ret)
		{
		InitWservL();

		__MM_HEAP_MARK;
		}
	
	return ret;
	}

TVerdict CTestStepVidPlayerSubtitleAvailable::DoTestStepPostambleL()
	{
	delete iPlayer2;
	iPlayer2 = NULL;
	
	__MM_HEAP_MARKEND;

	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

TVerdict CTestStepVidPlayerSubtitleAvailable::DoTestStepL()
	{
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TBool subtitleAvail = iPlayer2->SubtitlesAvailable();
	
	if (subtitleAvail && KErrNone == iError)
		{
		iTestStepResult = EPass;
		}
	else
		{
		iTestStepResult = EFail;
		ERR_PRINTF1(_L("SubtitleAvailable return false or iError not KErrNone"));
		}
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;
	}

TInt CTestStepVidPlayerSubtitleAvailable::SetSubtitleDisplayCheck(TMMFSubtitleWindowConfig& aConfig)
	{
	TPckgBuf<TMMFSubtitleWindowConfig> configPckg(aConfig);
	
	TInt err = iPlayer2->CustomCommandSync(iDestPckg, EMMFTestCustomCommandSetSubtitleDisplayCheck, configPckg, KNullDesC8);
	return err;
	}

void CTestStepVidPlayerSubtitleAvailable::CheckSubtitleCallCountL(TMMFTestGetSubtitleCountType aType, TInt aCount)
	{
	TPckgBuf<TMMFTestGetSubtitleCountType> commandPckg(aType);
	TPckgBuf<TInt> retPckg;
	TInt err = iPlayer2->CustomCommandSync(iDestPckg, EMMFTestCustomCommandGetSubtitleCallCount, commandPckg, KNullDesC8, retPckg);
	if (err != KErrNone) 
		{
		ERR_PRINTF3(_L("CheckSubtitleCallCount failed, type=%d, err=%d."), aType, err);
		User::Leave(err);
		}
	else if (aCount != retPckg())
		{
		ERR_PRINTF4(_L("CheckSubtitleCallCount failed, type=%d, retCount=%d, expectedCount=%d."), aType, retPckg(), aCount);
		User::Leave(KErrNotFound);
		}
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerSubtitleAvailable::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidPlayerSubtitleAvailable::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	INFO_PRINTF1(_L("MvpuoFrameReady called."));
	}

void CTestStepVidPlayerSubtitleAvailable::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidPlayerSubtitleAvailable::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

	
void CTestStepVidPlayerSubtitleAvailable::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	INFO_PRINTF1(_L("MvpuoEvent called."));
	}


/*
Subtitle enable and disable test
MM-MMF-VCLNT-U-1301-CP
*/
CTestStepVidPlayerSubtitleEnableDisable* CTestStepVidPlayerSubtitleEnableDisable::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleEnableDisable* self = new(ELeave) CTestStepVidPlayerSubtitleEnableDisable(aTestName);
	return self;
	}
	
CTestStepVidPlayerSubtitleEnableDisable::CTestStepVidPlayerSubtitleEnableDisable(const TDesC& aTestName)
											:CTestStepVidPlayerSubtitleAvailable(aTestName)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleEnableDisable::DoTestStepL()
	{
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive; // postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}
	
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	iPlayer2->DisableSubtitles();

	// enable subtitle again
	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}

	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
	
	TRAP(err, PlayAndStartSchedulerL());
	if (err != KErrNone || iError != KErrNone)
		{
		ERR_PRINTF3(_L("PlayAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}

	// check the return of GetCrpParameter, test controller set crp rect to be same as window clip rect
	TInt count = SubtitleCrpDataCount();
	if (count != 1)
		{
		ERR_PRINTF2(_L("Unexpected iSubtitleUtility.iCrpDataArray.Count(), count=%d."), count);
		return EFail;
		}
	
	if (!CheckSubtitleData(expectedConfig.iWindowId, dummyCrpId, expectedConfig.iWindowClipRect))
		{
		return EFail;
		}

	iPlayer2->DisableSubtitles();

	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 2);
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}

/*
Redraw test
MM-MMF-VCLNT-U-1302-HP
*/
CTestStepVidPlayerSubtitleRedraw* CTestStepVidPlayerSubtitleRedraw::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleRedraw* self = new(ELeave) CTestStepVidPlayerSubtitleRedraw(aTestName);
	return self;
	}
	
CTestStepVidPlayerSubtitleRedraw::CTestStepVidPlayerSubtitleRedraw(const TDesC& aTestName)
											:CTestStepVidPlayerSubtitleAvailable(aTestName)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleRedraw::DoTestStepL()
	{
	const TInt unexpectedWindowId = 1234;

	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;	// postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}
	
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
	
	// test redraw subtitle
	iPlayer2->RedrawSubtitle(*iWindow, expectedConfig.iWindowClipRect);

	// test redraw subtitle with a rect that does not intersect subtitle region
	TRect testRect(expectedConfig.iWindowClipRect.iBr, TSize(50, 50));
	iPlayer2->RedrawSubtitle(*iWindow, testRect);

	// simulate crp ready event with unexpected window handle
	TPckgBuf<TInt> intPckg(unexpectedWindowId);
	err = iPlayer2->CustomCommandSync(iDestPckg, EMMFTestCustomCommandSimulateCrpReadyEvent, intPckg, KNullDesC8);
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("EMMFTestCustomCommandCrpReadyEventCount failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
		
	// play prepare again after simulating unexpected window handle
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
	
	// check the return of GetCrpParameter, test controller set crp rect to be same as window clip rect
	TInt count = SubtitleCrpDataCount();
	if (count != 1)
		{
		ERR_PRINTF2(_L("Unexpected iSubtitleUtility.iCrpDataArray.Count(), count=%d."), count);
		return EFail;
		}
	
	if (!CheckSubtitleData(expectedConfig.iWindowId, dummyCrpId, expectedConfig.iWindowClipRect))
		{
		return EFail;
		}

	iPlayer2->DisableSubtitles();

	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 2);
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}

/*
RedrawSubtitle when CRP ready is not received
MM-MMF-VCLNT-U-1303-HP
*/
CTestStepVidPlayerSubtitleRedrawNotReady* CTestStepVidPlayerSubtitleRedrawNotReady::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleRedrawNotReady* self = new(ELeave) CTestStepVidPlayerSubtitleRedrawNotReady(aTestName);
	return self;
	}

CTestStepVidPlayerSubtitleRedrawNotReady::CTestStepVidPlayerSubtitleRedrawNotReady(const TDesC& aTestName)
										:CTestStepVidPlayerSubtitleAvailable(aTestName)
	{
	}

TVerdict CTestStepVidPlayerSubtitleRedrawNotReady::DoTestStepL()
	{
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;	// postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;
	
	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}
	
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}
	
	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	// redraw subtitle test when CRP ready event is not received
	iPlayer2->RedrawSubtitle(*iWindow, expectedConfig.iWindowClipRect);
	
	// check the crp data array in subtitle utility, 1 count is expected because 1 window was added successfully
	TInt count = SubtitleCrpDataCount();
	if (count != 1)
		{
		ERR_PRINTF2(_L("Unexpected iSubtitleUtility.iCrpDataArray.Count(), count=%d."), count);
		return EFail;
		}
	
	// expecte crp rect to be 0,0,0,0 because crp ready is not received yet
	TRect expectedCrpRect(0, 0, 0, 0);
	if (!CheckSubtitleData(expectedConfig.iWindowId, 0, expectedCrpRect))
		{
		return EFail;
		}
	
	iPlayer2->DisableSubtitles();
	
	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 0);
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}

/*
Enable and disable subtitle with multiple window
MM-MMF-VCLNT-U-1304-HP
*/
CTestStepVidPlayerSubtitleMultiWindow* CTestStepVidPlayerSubtitleMultiWindow::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleMultiWindow* self = new(ELeave) CTestStepVidPlayerSubtitleMultiWindow(aTestName);
	return self;
	}
	
CTestStepVidPlayerSubtitleMultiWindow::CTestStepVidPlayerSubtitleMultiWindow(const TDesC& aTestName)
											:CTestStepVidPlayerSubtitleAvailable(aTestName)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleMultiWindow::DoTestStepPostambleL()
	{
	if (iWindow2)
		{
		iWindow2->Close();
		delete iWindow2;
		iWindow2 = NULL;
		}
	if (iWindow3)
		{
		iWindow3->Close();
		delete iWindow3;
		iWindow3 = NULL;
		}
	iRootWindow2.Close();
	iRootWindow3.Close();
	
	delete iScreen2;
	iScreen2 = NULL;
	delete iScreen3;
	iScreen3 = NULL;
	
	return CTestStepVidPlayerSubtitleAvailable::DoTestStepPostambleL();
	}

// Init window 2 only if aWindowTwoOnly = ETrue
// Init window 2 and 3 if aWindowTwoOnly = EFalse
void CTestStepVidPlayerSubtitleMultiWindow::InitWindowsL(TBool aWindowTwoOnly)
	{
	iScreen2 = new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen2->Construct(1)); // and complete its construction
	iRootWindow2 = RWindowGroup(iWs);
	User::LeaveIfError(iRootWindow2.Construct((TUint32)(this) + 2, ETrue, iScreen2));
	
	iWindow2 = new(ELeave) RWindow(iWs);
	User::LeaveIfError(iWindow2->Construct(iRootWindow2, ((TUint32)(this)) + 3));
	iWindow2->SetRequiredDisplayMode(EColor16MA);
	iWindow2->SetExtent(TPoint(0,0), TSize(400,200));
	iWindow2->SetVisible(ETrue);
	iWindow2->Activate();

	if (!aWindowTwoOnly)
		{
		iScreen3 = new (ELeave) CWsScreenDevice(iWs); // make device for this session
		User::LeaveIfError(iScreen3->Construct(2)); // and complete its construction
		iRootWindow3 = RWindowGroup(iWs);
		User::LeaveIfError(iRootWindow3.Construct((TUint32)(this) + 4, ETrue, iScreen3));
	
		iWindow3 = new(ELeave) RWindow(iWs);
		User::LeaveIfError(iWindow3->Construct(iRootWindow3, ((TUint32)(this)) + 5));
		iWindow3->SetRequiredDisplayMode(EColor16MA);
		iWindow3->SetExtent(TPoint(50,50), TSize(400,200));
		iWindow3->SetVisible(ETrue);
		iWindow3->Activate();
		}
	
	iWs.Flush();
	}

TVerdict CTestStepVidPlayerSubtitleMultiWindow::DoTestStepL()
	{
	InitWindowsL();
	
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive; // postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	// add expected config for window1
	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}

	// add expected config for window2
	TRect window2ClipRect(100, 100, 200, 200);
	expectedConfig.iWindowId = iWindow2->WsHandle();
	expectedConfig.iWindowClipRect = window2ClipRect;
	expectedConfig.iDisplayMode = iWindow2->DisplayMode();
	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}

	// add first window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	// add second window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen2, *iWindow2, TRect(iWindow->Size()), window2ClipRect));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}
	
	// add third window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen3, *iWindow3));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}

	// test redraw subtitle on third window, which doesn't not display subtitle
	iPlayer2->RedrawSubtitle(*iWindow3, expectedConfig.iWindowClipRect);

	// play prepare again
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
	
	// check the return of GetCrpParameter, test controller set crp rect to be same as window clip rect
	TInt count = SubtitleCrpDataCount();
	if (count != 2)
		{
		ERR_PRINTF2(_L("Unexpected iSubtitleUtility.iCrpDataArray.Count(), count=%d."), count);
		return EFail;
		}
	
	if (!CheckSubtitleData(iWindow->WsHandle(), dummyCrpId, TRect(iWindow->Size())))
		{
		return EFail;
		}
	if (!CheckSubtitleData(iWindow2->WsHandle(), dummyCrpId, window2ClipRect))
		{
		return EFail;
		}

	iPlayer2->DisableSubtitles();

	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 3);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 2);

	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}


/*
Enable and disable subtitle with multiple window
MM-MMF-VCLNT-U-1305-HP
*/
CTestStepVidPlayerSubtitleAddRemoveMultiWindow* CTestStepVidPlayerSubtitleAddRemoveMultiWindow::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleAddRemoveMultiWindow* self = new(ELeave) CTestStepVidPlayerSubtitleAddRemoveMultiWindow(aTestName);
	return self;
	}
	
CTestStepVidPlayerSubtitleAddRemoveMultiWindow::CTestStepVidPlayerSubtitleAddRemoveMultiWindow(const TDesC& aTestName)
											:CTestStepVidPlayerSubtitleMultiWindow(aTestName)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleAddRemoveMultiWindow::DoTestStepL()
	{
	InitWindowsL();
	
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive; // postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	// add expected config for window1
	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}

	// add first window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	// add expected config for window2
	TRect window2ClipRect(100, 100, 200, 200);
	expectedConfig.iWindowId = iWindow2->WsHandle();
	expectedConfig.iWindowClipRect = window2ClipRect;
	expectedConfig.iDisplayMode = iWindow2->DisplayMode();
	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}

	// add second window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen2, *iWindow2, TRect(iWindow->Size()), window2ClipRect));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}
	
	// add third window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen3, *iWindow3));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
	
	// redraw on second window is ok
	iPlayer2->RedrawSubtitle(*iWindow2, window2ClipRect);
	
	// test redraw subtitle on third window, which doesn't not display subtitle
	iPlayer2->RedrawSubtitle(*iWindow3, expectedConfig.iWindowClipRect);

	// play prepare again
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
	
	// check crp data array, 2 subtitle data are expected
	TInt count = SubtitleCrpDataCount();
	if (count != 2)
		{
		ERR_PRINTF2(_L("Unexpected iSubtitleUtility.iCrpDataArray.Count(), count=%d."), count);
		return EFail;
		}
	
	if (!CheckSubtitleData(iWindow->WsHandle(), dummyCrpId, TRect(iWindow->Size())))
		{
		return EFail;
		}
	if (!CheckSubtitleData(iWindow2->WsHandle(), dummyCrpId, window2ClipRect))
		{
		return EFail;
		}

	// remove display window
	iPlayer2->RemoveDisplayWindow(*iWindow2);
	iPlayer2->RemoveDisplayWindow(*iWindow3);
	
	// try redraw subtitle on second window after removal, call will be ignored but won't fail or panic
	iPlayer2->RedrawSubtitle(*iWindow2, window2ClipRect);
	
	// check crp data array, 1 subtitle data is expected after remove display window
	count = SubtitleCrpDataCount();
	if (count != 1)
		{
		ERR_PRINTF2(_L("Unexpected iSubtitleUtility.iCrpDataArray.Count(), count=%d."), count);
		return EFail;
		}
	
	if (!CheckSubtitleData(iWindow->WsHandle(), dummyCrpId, TRect(iWindow->Size())))
		{
		return EFail;
		}

	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 0);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 3);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 2);

	iPlayer2->DisableSubtitles();
	
	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 3);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 2);
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}


/*
Enable subtitle with multiple window and change rotation or clip rect setting 
MM-MMF-VCLNT-U-1306-MP, MM-MMF-VCLNT-U-1307-MP
*/
CTestStepVidPlayerSubtitleMultiWindowSettingChange* CTestStepVidPlayerSubtitleMultiWindowSettingChange::NewL(const TDesC& aTestName, TBool aTestRotation)
	{
	CTestStepVidPlayerSubtitleMultiWindowSettingChange* self = new(ELeave) CTestStepVidPlayerSubtitleMultiWindowSettingChange(aTestName, aTestRotation);
	return self;
	}
	
CTestStepVidPlayerSubtitleMultiWindowSettingChange::CTestStepVidPlayerSubtitleMultiWindowSettingChange(const TDesC& aTestName, TBool aTestRotation)
											:CTestStepVidPlayerSubtitleMultiWindow(aTestName), iTestRotation(aTestRotation)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleMultiWindowSettingChange::DoTestStepL()
	{
	InitWindowsL(ETrue);
	
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive; // postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	// add expected config for window1
	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}

	// add first window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	// add expected config for window2
	TRect window2ClipRect(100, 100, 200, 200);
	TMMFSubtitleWindowConfig expectedConfig2;
	expectedConfig2.iWindowId = iWindow2->WsHandle();
	expectedConfig2.iWindowClipRect = window2ClipRect;
	expectedConfig2.iDisplayMode = iWindow2->DisplayMode();
	expectedConfig2.iRotation = EVideoRotationNone;
	err = SetSubtitleDisplayCheck(expectedConfig2);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}

	// add second window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen2, *iWindow2, TRect(iWindow->Size()), window2ClipRect));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}

	if (iTestRotation)
		{
		// MM-MMF-VCLNT-U-1306-MP test rotation
		
		// First test changing rotation for 1 window
		// Update expected config for window1
		expectedConfig.iRotation = EVideoRotationClockwise270;
		err = SetSubtitleDisplayCheck(expectedConfig);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}
		
		iPlayer2->SetRotationL(*iWindow, EVideoRotationClockwise270);
		
		TEST(EVideoRotationClockwise270 == iPlayer2->RotationL(*iWindow));
		TEST(EVideoRotationNone == iPlayer2->RotationL(*iWindow2));
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 1);
		
		// test changing rotation for all windows, but actually only one window is updated because setting 
		// is already changed in the other window
		// Update expected config for window2
		expectedConfig2.iRotation = EVideoRotationClockwise270;
		err = SetSubtitleDisplayCheck(expectedConfig2);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}

		static_cast<CVideoPlayerUtility*>(iPlayer2)->SetRotationL(EVideoRotationClockwise270);
		
		TEST(EVideoRotationClockwise270 == iPlayer2->RotationL(*iWindow));
		TEST(EVideoRotationClockwise270 == iPlayer2->RotationL(*iWindow2));
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 2);
		
		// test changing rotation for all windows
		// Update expected config for window1
		expectedConfig.iRotation = EVideoRotationClockwise180;
		err = SetSubtitleDisplayCheck(expectedConfig);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}
		// update expected config for window2
		expectedConfig2.iRotation = EVideoRotationClockwise180;
		err = SetSubtitleDisplayCheck(expectedConfig2);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}

		static_cast<CVideoPlayerUtility*>(iPlayer2)->SetRotationL(EVideoRotationClockwise180);
		
		TEST(EVideoRotationClockwise180 == iPlayer2->RotationL(*iWindow));
		TEST(EVideoRotationClockwise180 == iPlayer2->RotationL(*iWindow2));
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 4);
		}
	else
		{
		// MM-MMF-VCLNT-U-1307-MP test window clip rect
		
		// Update expected config for window1
		TRect window1ClipRect(50, 50, 150, 150);
		expectedConfig.iWindowClipRect = window1ClipRect;
		err = SetSubtitleDisplayCheck(expectedConfig);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}

		iPlayer2->SetWindowClipRectL(*iWindow, window1ClipRect);
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 1);
		}
	
	// redraw before crp ready is received
	iPlayer2->RedrawSubtitle(*iWindow, expectedConfig.iWindowClipRect);
	
	// play prepare again
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
	
	// redraw on windows are ok
	iPlayer2->RedrawSubtitle(*iWindow, expectedConfig.iWindowClipRect);
	iPlayer2->RedrawSubtitle(*iWindow2, expectedConfig.iWindowClipRect);

	// check crp data array, 2 subtitle data are expected
	TInt count = SubtitleCrpDataCount();
	if (count != 2)
		{
		ERR_PRINTF2(_L("Unexpected iSubtitleUtility.iCrpDataArray.Count(), count=%d."), count);
		return EFail;
		}
	
	if (!CheckSubtitleData(iWindow->WsHandle(), dummyCrpId, expectedConfig.iWindowClipRect))
		{
		return EFail;
		}
	if (!CheckSubtitleData(iWindow2->WsHandle(), dummyCrpId, expectedConfig2.iWindowClipRect))
		{
		return EFail;
		}

	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 0);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 0);
	if (iTestRotation)
		{
		CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 6);
		CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 6);
		}
	else
		{
		CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 3);
		CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 3);
		}

	iPlayer2->DisableSubtitles();
	
	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 2);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 2);
	if (iTestRotation)
		{
		CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 6);
		CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 6);
		}
	else
		{
		CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 3);
		CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 3);
		}
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}

/*
Subtitle language test
MM-MMF-VCLNT-U-1308-HP
*/
CTestStepVidPlayerSubtitleLanguage* CTestStepVidPlayerSubtitleLanguage::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleLanguage* self = new(ELeave) CTestStepVidPlayerSubtitleLanguage(aTestName);
	return self;
	}
	
CTestStepVidPlayerSubtitleLanguage::CTestStepVidPlayerSubtitleLanguage(const TDesC& aTestName)
											:CTestStepVidPlayerSubtitleAvailable(aTestName)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleLanguage::DoTestStepL()
	{
	const TLanguage KExpectedLang0 = ELangEnglish;
	const TLanguage KExpectedLang1 = ELangFrench;
	const TLanguage KExpectedLang2 = ELangSpanish;
	
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;	// postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}

	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}
	
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	TArray<TLanguage> languages = iPlayer2->SupportedSubtitleLanguagesL();
	
	if (languages.Count() != 3)
		{
		ERR_PRINTF2(_L("Unexpected language count, count=%d, expected 3."), languages.Count());
		return EFail;
		}
	TEST(KExpectedLang0 == languages[0]);
	TEST(KExpectedLang1 == languages[1]);
	TEST(KExpectedLang2 == languages[2]);
	
	TLanguage lang = iPlayer2->SubtitleLanguageL();
	TEST(KExpectedLang0 == lang);
	
	iPlayer2->SetSubtitleLanguageL(KExpectedLang2);
	lang = iPlayer2->SubtitleLanguageL();
	TEST(KExpectedLang2 == lang);
		
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;
	}

/*
Enable subtitle with one window and change rotation or clip rect setting 
MM-MMF-VCLNT-U-1309-HP and MM-MMF-VCLNT-U-1310-HP
*/
CTestStepVidPlayerSubtitleWindowSettingChange* CTestStepVidPlayerSubtitleWindowSettingChange::NewL(const TDesC& aTestName, TBool aTestRotation)
	{
	CTestStepVidPlayerSubtitleWindowSettingChange* self = new(ELeave) CTestStepVidPlayerSubtitleWindowSettingChange(aTestName, aTestRotation);
	return self;
	}
	
CTestStepVidPlayerSubtitleWindowSettingChange::CTestStepVidPlayerSubtitleWindowSettingChange(const TDesC& aTestName, TBool aTestRotation)
											:CTestStepVidPlayerSubtitleAvailable(aTestName), iTestRotation(aTestRotation)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleWindowSettingChange::DoTestStepL()
	{
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive; // postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	// add expected config for 
	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}

	// add window
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}

	if (iTestRotation)
		{
		// MM-MMF-VCLNT-U-1309-HP test rotation
		
		// First test changing rotation for 1 window
		// Update expected config for window1
		expectedConfig.iRotation = EVideoRotationClockwise270;
		err = SetSubtitleDisplayCheck(expectedConfig);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}
		
		iPlayer2->SetRotationL(*iWindow, EVideoRotationClockwise270);
		TEST(EVideoRotationClockwise270 == iPlayer2->RotationL(*iWindow));
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 1);
		
		// Setting the same rotation value again does not cause a update
		iPlayer2->SetRotationL(*iWindow, EVideoRotationClockwise270);
		TEST(EVideoRotationClockwise270 == iPlayer2->RotationL(*iWindow));
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 1);

		// test changing rotation for all windows, when only one window is added
		// Update expected config for window
		expectedConfig.iRotation = EVideoRotationClockwise180;
		err = SetSubtitleDisplayCheck(expectedConfig);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}

		static_cast<CVideoPlayerUtility*>(iPlayer2)->SetRotationL(EVideoRotationClockwise180);
		
		TEST(EVideoRotationClockwise180 == iPlayer2->RotationL(*iWindow));
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 2);
		}
	else
		{
		// MM-MMF-VCLNT-U-1310-HP test window clip rect
		
		// Update expected config for window
		TRect windowClipRect(50, 50, 150, 150);
		expectedConfig.iWindowClipRect = windowClipRect;
		err = SetSubtitleDisplayCheck(expectedConfig);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}

		iPlayer2->SetWindowClipRectL(*iWindow, windowClipRect);
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 1);

		// setting the same setting does not cause a update
		iPlayer2->SetWindowClipRectL(*iWindow, windowClipRect);
		CheckSubtitleCallCountL(EMMFTestUpdateSubtitleConfigCallCount, 1);
		}
	
	// redraw before crp ready is received
	iPlayer2->RedrawSubtitle(*iWindow, expectedConfig.iWindowClipRect);
	
	// play prepare again
	TRAP(err, PlayerPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		{
		ERR_PRINTF3(_L("PlayerPrepareAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EFail;
		}
	
	// redraw on windows are ok
	iPlayer2->RedrawSubtitle(*iWindow, expectedConfig.iWindowClipRect);

	// check crp data array, 2 subtitle data are expected
	TInt count = SubtitleCrpDataCount();
	if (count != 1)
		{
		ERR_PRINTF2(_L("Unexpected iSubtitleUtility.iCrpDataArray.Count(), count=%d."), count);
		return EFail;
		}
	
	if (!CheckSubtitleData(iWindow->WsHandle(), dummyCrpId, expectedConfig.iWindowClipRect))
		{
		return EFail;
		}

	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 0);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 0);
	if (iTestRotation)
		{
		CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 3);
		CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 3);
		}
	else
		{
		CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 2);
		CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 2);
		}

	iPlayer2->DisableSubtitles();
	
	CheckSubtitleCallCountL(EMMFTestEnableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestDisableSubtitleCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestAddSubtitleConfigCallCount, 1);
	CheckSubtitleCallCountL(EMMFTestRemoveSubtitleConfigCallCount, 1);
	if (iTestRotation)
		{
		CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 3);
		CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 3);
		}
	else
		{
		CheckSubtitleCallCountL(EMMFTestCrpReadyEventCount, 2);
		CheckSubtitleCallCountL(EMMFTestGetCrpParamCallCount, 2);
		}
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}

/*
Subtitle enable negative tests
MM-MMF-VCLNT-U-1350-CP, MM-MMF-VCLNT-U-1351-CP, MM-MMF-VCLNT-U-1352-MP, MM-MMF-VCLNT-U-1353-CP
*/
CTestStepVidPlayerSubtitleEnableNeg* CTestStepVidPlayerSubtitleEnableNeg::NewL(const TDesC& aTestName, 
																				TBool aTestSubNotAvailble,
																				TBool aTestSourceNotOpen,
																				TBool aTestEnableTwice,
																				TBool aTestNoDisplayAdded,
																				TInt aExpectedErr)
	{
	CTestStepVidPlayerSubtitleEnableNeg* self = new(ELeave) CTestStepVidPlayerSubtitleEnableNeg(aTestName,
																				aTestSubNotAvailble,
																				aTestSourceNotOpen,
																				aTestEnableTwice,
																				aTestNoDisplayAdded,
																				aExpectedErr);
	return self;
	}
	
CTestStepVidPlayerSubtitleEnableNeg::CTestStepVidPlayerSubtitleEnableNeg(const TDesC& aTestName, 
																			TBool aTestSubNotAvailble,
																			TBool aTestSourceNotOpen,
																			TBool aTestEnableTwice,
																			TBool aTestNoDisplayAdded,
																			TInt aExpectedErr)
											:CTestStepVidPlayerSubtitleAvailable(aTestName),
											iTestSubNotAvailable(aTestSubNotAvailble),
											iTestSourceNotOpen(aTestSourceNotOpen),
											iTestEnableTwice(aTestEnableTwice),
											iTestNoDisplayAdded(aTestNoDisplayAdded),
											iExpectedErr(aExpectedErr)
	{
	if (iTestSubNotAvailable)
		{
		iFileName.Set(KClockFileName);
		}
	}
	
TVerdict CTestStepVidPlayerSubtitleEnableNeg::DoTestStepL()
	{
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;	// postamble will clean up iPlayer2
		}

	// test that subtitle is not available 
	TBool subtitleAvail = iPlayer2->SubtitlesAvailable();
	if (subtitleAvail)
		{
		ERR_PRINTF1(_L("SubtitleAvailable return ETrue."));
		return EFail;
		}
	
	TInt err(KErrNone);
	if (!iTestSourceNotOpen)
		{
		TRAP(err, OpenAndStartSchedulerL());
		if (KErrNone != err || iError != KErrNone)
			{
			ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
			return EInconclusive;
			}
		}
	
	if (!iTestSourceNotOpen && !iTestNoDisplayAdded)
		{
		TMMFSubtitleWindowConfig expectedConfig;
		expectedConfig.iWindowId = iWindow->WsHandle();
		expectedConfig.iWindowClipRect = TRect(iWindow->Size());
		expectedConfig.iDisplayMode = iWindow->DisplayMode();
		expectedConfig.iRotation = EVideoRotationNone;
		
		err = SetSubtitleDisplayCheck(expectedConfig);
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
			return  EInconclusive;
			}
		
		TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
			return  EInconclusive;
			}
		}
	
	// test that subtitle availability
	subtitleAvail = iPlayer2->SubtitlesAvailable();
	if (iTestSubNotAvailable || iTestSourceNotOpen)
		{
		if (subtitleAvail)
			{
			ERR_PRINTF1(_L("SubtitleAvailable return ETrue."));
			return EFail;
			}
		}
	else
		{
		if (!subtitleAvail)
			{
			ERR_PRINTF1(_L("SubtitleAvailable return EFalse."));
			return EFail;
			}
		}
	
	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (!iTestEnableTwice)
		{
		if (err != iExpectedErr)
			{
			ERR_PRINTF3(_L("Test failed: err=%d, iExpecteErr=%d"), err, iExpectedErr);
			return EFail;
			}
		}
	else
		{
		TEST(KErrNone == err);
		TRAP(err, iPlayer2->EnableSubtitlesL());
		if (err != iExpectedErr)
			{
			ERR_PRINTF3(_L("Test failed: err=%d, iExpecteErr=%d"), err, iExpectedErr);
			return EFail;
			}
		}
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}

/*
Test Subtitle language with subtitle not enabled
MM-MMF-VCLNT-U-1354-HP, MM-MMF-VCLNT-U-1355-HP, MM-MMF-VCLNT-U-1356-HP
*/
CTestStepVidPlayerSubtitleLangNotEnable* CTestStepVidPlayerSubtitleLangNotEnable::NewL(const TDesC& aTestName,
																				TBool aTestGetSupportedLang,
																				TBool aTestGetLang,
																				TBool aTestSetLang)
	{
	CTestStepVidPlayerSubtitleLangNotEnable* self = new(ELeave) CTestStepVidPlayerSubtitleLangNotEnable(aTestName,
																				aTestGetSupportedLang,
																				aTestGetLang,
																				aTestSetLang);
	return self;
	}
	
CTestStepVidPlayerSubtitleLangNotEnable::CTestStepVidPlayerSubtitleLangNotEnable(const TDesC& aTestName,
																				TBool aTestGetSupportedLang,
																				TBool aTestGetLang,
																				TBool aTestSetLang)
																		:CTestStepVidPlayerSubtitleAvailable(aTestName),
																		iTestGetSupportedLang(aTestGetSupportedLang),
																		iTestGetLang(aTestGetLang),
																		iTestSetLang(aTestSetLang)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleLangNotEnable::DoTestStepL()
	{
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;	// postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}

	if (iTestGetSupportedLang)
		{
		TRAP(err, (void) iPlayer2->SupportedSubtitleLanguagesL()); // expect leave, so ignore return value
		if (KErrNotReady != err)
			{
			ERR_PRINTF2(_L("Test SupportedSubtitleLanguagesL failed, expected KErrNotReady, err=%d."), err);
			return EFail;
			}
		}
	else if (iTestGetLang)
		{
		TRAP(err, (void) iPlayer2->SubtitleLanguageL()); // expect leave, so ignore return value
		if (KErrNotReady != err)
			{
			ERR_PRINTF2(_L("Test SubtitleLanguageL failed, expected KErrNotReady, err=%d."), err);
			return EFail;
			}
		}
	else if (iTestSetLang)
		{
		TRAP(err, iPlayer2->SetSubtitleLanguageL(ELangFrench));
		if (KErrNotReady != err)
			{
			ERR_PRINTF2(_L("Test SetSubtitleLanguageL failed, expected KErrNotReady, err=%d."), err);
			return EFail;
			}
		}
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}

/*
Subtitle language not available test
MM-MMF-VCLNT-U-1357-HP
*/
CTestStepVidPlayerSubtitleLangNotAvail* CTestStepVidPlayerSubtitleLangNotAvail::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleLangNotAvail* self = new(ELeave) CTestStepVidPlayerSubtitleLangNotAvail(aTestName);
	return self;
	}
	
CTestStepVidPlayerSubtitleLangNotAvail::CTestStepVidPlayerSubtitleLangNotAvail(const TDesC& aTestName)
											:CTestStepVidPlayerSubtitleAvailable(aTestName)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleLangNotAvail::DoTestStepL()
	{
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;	// postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}

	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}
	
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	// Send command to controller to make subtitle language not available
	err = iPlayer2->CustomCommandSync(iDestPckg, EMMFTestCustomCommandSetLanguageNotAvail, KNullDesC8, KNullDesC8);
	if (err != KErrNone) 
		{
		ERR_PRINTF2(_L("EMMFTestCustomCommandSetLanguageNotAvail failed, err=%d."), err);
		return EInconclusive;
		}
	
	TRAP(err, iPlayer2->EnableSubtitlesL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("EnableSubtitlesL failed: %d"), err);
		return EFail;
		}
	
	TArray<TLanguage> languages = iPlayer2->SupportedSubtitleLanguagesL();
	
	if (languages.Count() != 0)
		{
		ERR_PRINTF2(_L("Unexpected language count, count=%d, expected 0."), languages.Count());
		return EFail;
		}
	
	TLanguage lang = iPlayer2->SubtitleLanguageL();
	TEST(ELangNone == lang);
	
	TRAP(err, iPlayer2->SetSubtitleLanguageL(ELangFrench));
	if (err != KErrNotSupported)
		{
		ERR_PRINTF2(_L("Unexpected SetSubtitleLanguageL err, err=%d, expected KErrNotSupported."), err);
		return EFail;
		}
	
	lang = iPlayer2->SubtitleLanguageL();
	TEST(ELangNone == lang);
		
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;
	}

/*
Test RedrawSubtitle when subtitle is not enabled
MM-MMF-VCLNT-U-1358-LP
*/
CTestStepVidPlayerSubtitleNotEnabledRedraw* CTestStepVidPlayerSubtitleNotEnabledRedraw::NewL(const TDesC& aTestName)
	{
	CTestStepVidPlayerSubtitleNotEnabledRedraw* self = new(ELeave) CTestStepVidPlayerSubtitleNotEnabledRedraw(aTestName);
	return self;
	}
	
CTestStepVidPlayerSubtitleNotEnabledRedraw::CTestStepVidPlayerSubtitleNotEnabledRedraw(const TDesC& aTestName)
											:CTestStepVidPlayerSubtitleAvailable(aTestName)
	{
	}
	
TVerdict CTestStepVidPlayerSubtitleNotEnabledRedraw::DoTestStepL()
	{
	iPlayer2 = CVideoPlayerUtility2::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
			
	if (!PlayerStateStopped())
		{
		ERR_PRINTF1(_L("CVideoPlayerUtility2 isn't stopped"));
		return EInconclusive;	// postamble will clean up iPlayer2
		}
	
	TRAPD(err, OpenAndStartSchedulerL());
	if (KErrNone != err || iError != KErrNone)
		{
		ERR_PRINTF3(_L("OpenAndStartSchedulerL() failed, err=%d, iError=%d."), err, iError);
		return EInconclusive;
		}
	
	TMMFSubtitleWindowConfig expectedConfig;
	expectedConfig.iWindowId = iWindow->WsHandle();
	expectedConfig.iWindowClipRect = TRect(iWindow->Size());
	expectedConfig.iDisplayMode = iWindow->DisplayMode();
	expectedConfig.iRotation = EVideoRotationNone;

	err = SetSubtitleDisplayCheck(expectedConfig);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("SetSubtitleDisplayCheck failed: %d"), err);
		return  EInconclusive;
		}
	
	TRAP(err, iPlayer2->AddDisplayWindowL(iWs, *iScreen, *iWindow));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("AddDisplayWindow failed: %d"), err);
		return  EInconclusive;
		}

	// test redraw subtitle without enabling subtitle
	iPlayer2->RedrawSubtitle(*iWindow, expectedConfig.iWindowClipRect);
	
	delete iPlayer2;
	iPlayer2 = NULL;
	
	return iTestStepResult;	
	}
