
// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program is designed the test of the TSU_MMFMIDICLNT.cpp.
// 
//

/**
 @file TSU_MMFMIDICLNT.cpp
*/

#include "TSU_MMFMIDICLNT.h"


/**
 * Timeout function
 */
void CTestMmfMidiClntStep::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
	{
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal() ;
	timer.After(timerStatus,aNumberOfMicroSeconds);

	User::WaitForRequest(aStatus, timerStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		INFO_PRINTF1(_L("CTestMmfMidiClntStep wait timed out")) ;
		}
	timer.Close() ;
	}

/**
 * Time comparison utility function
 *
 * @param	"const TUint aActual"
 *			The actual timer value produced
 * @param	"const TUint aExpected"
 *			Expected timer value
 * @param	"const TUint aDeviation"
 *			Allowed deviation of the expected value
 *			from the actual value.
 * @return	"TBool"
 *			Did actual timed value fall within deviation limits
 */ 
TBool CTestMmfMidiClntStep::TimeComparison(const TUint aActual, const TUint aExpected, const TUint aDeviation)
	{
	// save unnessary conditions
	if(aActual == aExpected)
		return ETrue;	

	// Prevent unsigned wrapping errors 
	TUint difference;
	if(aActual > aExpected)
		difference = aActual - aExpected;
	else
		difference = aExpected - aActual;

	// compare
	if(difference < aDeviation)
		return ETrue;
	return EFalse;
	}

/**
 * Test Preamble routines.
 *
 * Creates our own Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Preamble complete.
 */
TVerdict CTestMmfMidiClntStep::DoTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
    //[ push the scheduler on the stack ]
	CleanupStack::PushL( iActiveScheduler );
	
	//[install the active scheduler ]
	CActiveScheduler::Install(iActiveScheduler);

	// Pop the element from the stack
	CleanupStack::Pop();

	return EPass;
	}

/**
 * Test Postamble routines.
 *
 * Destroys our Active Scheduler.
 *
 * @return	"TVerdict"
 *			Did Postamble complete.
 */
TVerdict CTestMmfMidiClntStep::DoTestStepPostambleL()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	
	delete iScreen;
	delete iWindow;
	iWs.Close();
	

	return EPass;
	}

/**
 * CTestMMFVCLNTStep Implementation
 */
CTestMmfMidiClntStep::CTestMmfMidiClntStep(const TDesC& aTestName, const TTestStepType aTestType)
	:CTestStep(),
	 iActiveScheduler( NULL )
	{
	// store the name of this test step
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iTestType = aTestType;
	}

CTestMmfMidiClntStep::CTestMmfMidiClntStep()
	:iActiveScheduler( NULL )
	// for test step that sets its own name
	{
	iTestType = ETestValid;
	}

CTestMmfMidiClntStep::~CTestMmfMidiClntStep()
	{
	}

/*void CTestMmfMidiClntStep::Close()
	{
	delete iScreen;
	delete iWindow;
	iWs.Close();
	}*/

/**
 * DoTestStepL : same as CTestMidiClientOpenFile::DoTestStepL() but with no file opening
 * routines.
 *
 * Generally, test steps should NOT override this, but put their processing in DoTestL()
 *
 */
TVerdict CTestMmfMidiClntStep::DoTestStepL()
	{
	__MM_HEAP_MARK;
	CMidiClientUtility* player = CMidiClientUtility::NewL(*this, EMdaPriorityNormal, EMdaPriorityPreferenceTimeAndQuality);
	if (!player)
		{
		ERR_PRINTF1(_L("Could not create a CMidiClientUtility"));
		return EInconclusive;
		}
	
	CleanupStack::PushL(player);
 
	TMMFMessageDestinationPckg dummyPckg;
	TInt dummyFunc = 0;		//EDevMidiOff;
	TBuf8<8> dummyBuff;
	player->CustomCommandSyncL(dummyPckg, dummyFunc, dummyBuff, dummyBuff, dummyBuff);

	TVerdict ret = DoTestL(player);
		
	INFO_PRINTF1(_L("CMidiClientUtility: Destroying"));
	CleanupStack::PopAndDestroy(player);

	if(iError != KErrNone)
		ERR_PRINTF2( _L("CMidiClientUtility failed with error %d"),iError );
	
	__MM_HEAP_MARKEND;
	return	ret;
	}

TVerdict CTestMmfMidiClntStep::DoTestL(CMidiClientUtility* /*aMidi*/)
	{
	return EPass;
	}

void CTestMmfMidiClntStep::InitWservL()
	{
	TInt err = iWs.Connect();
	if (err != KErrNone)
		{
		// Access violation if ws is null
		INFO_PRINTF1(_L("Cannot test, no window server available"));
		User::Leave(err);
		}

	iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct()); // and complete its construction

	RWindowGroup rootWindow = RWindowGroup(iWs);
	User::LeaveIfError(rootWindow.Construct((TUint32)this, ETrue));

	iWindow = new(ELeave) RWindow(iWs);
	User::LeaveIfError(((RWindow*)iWindow)->Construct(rootWindow,((TUint32)(this)) + 1));
	iWindow->SetExtent(TPoint(0,0), TSize(100,100));
	iWindow->SetVisible(ETrue);
	iWindow->Activate();
	iWs.Flush();
	}

// MMidiClientUtilityObserver

void CTestMmfMidiClntStep::MmcuoStateChanged(TMidiState aOldState, TMidiState aNewState, 
											  const TTimeIntervalMicroSeconds& /* aTime */, TInt aError)
	{
	INFO_PRINTF3(_L("MmcuoStateChanged callback, oldState = %d, newState = %d"), aOldState, aNewState);

	iError = aError;
	iCurrentState = aNewState;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestMmfMidiClntStep::MmcuoTempoChanged(TInt /* aMicroBeatsPerMinute */)
	{
	INFO_PRINTF1(_L("MmcuoTempoChanged callback"));
	}

void CTestMmfMidiClntStep::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 /*aVolumeInDecibels*/)
	{
	INFO_PRINTF1(_L("MmcuoVolumeChanged callback"));
	}

void CTestMmfMidiClntStep::MmcuoMuteChanged(TInt /* aChannel */, TBool /* aMuted */)
	{
	INFO_PRINTF1(_L("MmcuoMuteChanged callback"));
	}

void CTestMmfMidiClntStep::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /* aMicroSeconds */, TInt64 /* aMicroBeats */)
	{
	INFO_PRINTF1(_L("MmcuoSyncUpdate callback"));
	}

void CTestMmfMidiClntStep::MmcuoMetaDataEntryFound(const TInt /*aMetaDataEntryId*/,const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	INFO_PRINTF1(_L("MmcuoSmfMetaDataEntryFound callback"));
	}

void CTestMmfMidiClntStep::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /* aEntry */)
	{
	INFO_PRINTF1(_L("MmcuoMipMessageReceived callback"));
	}

void CTestMmfMidiClntStep::MmcuoPolyphonyChanged(TInt /* aNewPolyphony */)
	{
	INFO_PRINTF1(_L("MmcuoPolyphonyChanged callback"));
	}

void CTestMmfMidiClntStep::MmcuoInstrumentChanged(TInt /* aChannel */, TInt /* aBankId */, TInt /* aInstrumentId */)
	{
	INFO_PRINTF1(_L("MmcuoInstrumentChanged callback"));
	}

