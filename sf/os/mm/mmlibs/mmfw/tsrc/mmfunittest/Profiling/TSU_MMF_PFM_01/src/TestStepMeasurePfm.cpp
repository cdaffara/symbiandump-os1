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
// TestStepMeasurePerformance.cpp
//

// EPOC includes
#include <e32base.h>
#include <ECom\ECom.h>

// Test system includes
#include <TestFramework.h>

// Specific includes for this test suite
#include "TestStepMmfPfm.h"
#include "TestSuiteMmfPfm.h"

// Specific includes for these test steps
#include "TestStepMeasurePfm.h"


// Test steps and Files to be tested:
const TMmfProfileTest KTestDetails[] =
	{
		{ _S("MM-MMF-PFM-001-LP"), _S("adpcm16mono8khz.wav"), ENormal},
		{ _S("MM-MMF-PFM-002-LP"), _S("adpcm16mono8khz.wav"), EWait},
		{ _S("MM-MMF-PFM-003-LP"), _S("gsmMono8khz.wav"), ENormal},
		{ _S("MM-MMF-PFM-004-LP"), _S("gsmMono8khz.wav"), EWait},
		{ _S("MM-MMF-PFM-005-LP"), _S("mulaw8mono44khz.au"), ENormal},
		{ _S("MM-MMF-PFM-006-LP"), _S("mulaw8mono44khz.au"), EWait},
		{ _S("MM-MMF-PFM-007-LP"), _S("pcm16stereo8khz.au"), ENormal},
		{ _S("MM-MMF-PFM-008-LP"), _S("pcm16stereo8khz.au"), EWait},
		{ _S("MM-MMF-PFM-009-LP"), _S("pcm8mono44khz.wav"), ENormal},
		{ _S("MM-MMF-PFM-010-LP"), _S("pcm8mono44khz.wav"), EWait}
	};


/**
  *
  * Test step constructor.
  * Each test step initialises its own name.
  *
  * 
  */
RMmfPfmTest001::RMmfPfmTest001(CTestSuite* aSuite)
	{
	iSuite = aSuite;
	// store the name of this test case
	_LIT( KMmfPfmTest, "MM-MMF-PFM-001-LP");
	iTestStepName = KMmfPfmTest;
	}

/**
  * Open ECom and run every step.
  * @return	"enum TVerdict"
  */

TVerdict RMmfPfmTest001::DoTestStepL( )
	{
	REComSession& ecomSession = REComSession::OpenL();
	CleanupClosePushL(ecomSession);

	const TInt numTestSteps = sizeof(KTestDetails) / sizeof(TMmfProfileTest);
	for(TInt i=0;i<numTestSteps;i++)
		{
		const TMmfProfileTest& testDetails = KTestDetails[i];
		TVerdict verdict =  DoProfileL(testDetails);
		if(verdict > iTestStepResult)
			iTestStepResult = verdict;
		}

	CleanupStack::PopAndDestroy(&ecomSession);

	return iTestStepResult;
	}


/**
  * Do time profiling for a particular file
  * Report the profiling statistics
  *
  * The line "MACRO __AUDIO_PROFILING need to be defined in:
  *		MediaClientAudio.mmp
  *		MmfAudioOutput.mmp
  * and recompile
  *		MmfClientAudioPlayer.cpp
  *		MmfAudioOutput.cpp
  *
  * Profiles:
  * 0 - From start until first buffer is written to DevSound
  * 1 - Around RMMFController::Open()
  * 2 - Around RMMFController::AddDataSource()
  * 3 - Around RMMFController::AddDataSink()
  * 4 - Around RMMFController::Prime()
  * 5 - Around RMMFController::Play()
  */

TVerdict RMmfPfmTest001::DoProfileL(const TMmfProfileTest aProfileTest)
	{
	iTestStepName = aProfileTest.iTestName;
	TBuf<128> outBuf;
	outBuf.Format(_L("\n"));
	Log(outBuf);
	outBuf.Format(_L("---------------------------------------------"));
	Log(outBuf);

	const TFileName fileName = aProfileTest.iTestFileName;
	outBuf.Format(_L("Filename: "));
	outBuf.AppendFormat(fileName);
	Log(outBuf);

	TInt repeatCount = 100;
	if(aProfileTest.iTestType==EWait)
		{
		repeatCount = 10;
		outBuf.Format(_L("Wait for ECom to unload"));
		Log(outBuf);
		}

	TInt err=KErrNone;
	

	for (TInt i=0; i<repeatCount; i++)
		{
		if (aProfileTest.iTestType==EWait)
			User::After(5000000); // wait 5s for ECom to unload

		TRAP(err,OpenAndPlayAudioFileL(fileName));

		if (err!=KErrNone) // stop when we see error
			break;
		}

	if(err)
		{
		outBuf.Format(_L("*** Test Failed... reason %d ***"), err);
		Log(outBuf);
		return EFail;
		}
		
	TProfile profile[6];
	

	outBuf.Format(_L("Profile results"));
	Log(outBuf);
	
	for (TInt index=0; index<6; index++)
		{
		outBuf.Format(iTestStepName);
		outBuf.AppendFormat(_L(" profile(%d) time=%d count=%d"), index, profile[index].iTime, profile[index].iCount);
		Log(outBuf);
		}

	return iTestStepResult;
	}

/**
  * OpenAndPlayAudioFileL
  * Opens the filename into a CMdaAudioPlayerUtility* object
  * This method allows us to measure more precisely
  * the individual times taken for the process.
  */
void RMmfPfmTest001::OpenAndPlayAudioFileL(const TDesC& aFileName)
	{
	CMdaAudioPlayerUtility* player = CMdaAudioPlayerUtility::NewL(*this);
	CleanupStack::PushL(player);

	TFileName fileName = ((CTestSuiteMmfPerformance*)iSuite)->DefaultPath();
	fileName.Append(aFileName);


	player->OpenFileL(fileName);
	CActiveScheduler::Start();
	User::LeaveIfError(iInitError);
	player->Play();
	CActiveScheduler::Start();

	User::LeaveIfError(iPlayError);

	CleanupStack::PopAndDestroy(player);
	}

/**
Init Callback
Save the error code and stop the Scheduler
*/
void RMmfPfmTest001::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
	{
	iInitError = aError;
	iDuration = aDuration;
	CActiveScheduler::Stop();
	}

/**
Play Callback
Save the error code and stop the Scheduler
*/
void RMmfPfmTest001::MapcPlayComplete(TInt aError)
	{
	iPlayError = aError;
	CActiveScheduler::Stop();
	}
