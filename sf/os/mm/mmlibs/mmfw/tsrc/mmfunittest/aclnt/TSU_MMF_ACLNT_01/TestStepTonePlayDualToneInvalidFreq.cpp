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
// CTestStepTonePlayDualToneInvalidFreq.cpp
// 
//

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

#include <mdaaudiotoneplayer.h>

// Test system includes
#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepTone.h"


#include "mmfclienttoneplayer.h"


CTestStepTonePlayDualToneInvalidFreq* CTestStepTonePlayDualToneInvalidFreq::NewL(const TDesC& aName)
	{
	CTestStepTonePlayDualToneInvalidFreq* self = new(ELeave) CTestStepTonePlayDualToneInvalidFreq(aName);
	return self;
	}


CTestStepTonePlayDualToneInvalidFreq::CTestStepTonePlayDualToneInvalidFreq(const TDesC& aName) : iPrepareError(KErrNone), iPlayError(KErrNone)
	{
	iTestStepName = aName;
	// need a bigger heap size on EKA2 HW
#if !defined __WINS__
	iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2
	}


void CTestStepTonePlayDualToneInvalidFreq::MatoPrepareComplete(TInt aError)
	{
	iPrepareError = aError;
	INFO_PRINTF2(_L("PrepareToPlayDualTone() completed with error %i"), aError);
	CActiveScheduler::Stop();	//To resume DoTestStepL()
	}


void CTestStepTonePlayDualToneInvalidFreq::MatoPlayComplete(TInt aError)
	{
	iPlayError = aError;
	INFO_PRINTF2(_L("Play() completed with error %i"), aError);
	CActiveScheduler::Stop();	//To resume DoTestStepL()
	}


/**
 *
 * Creates the tone utility and active scheduler.
 * 
 */
enum TVerdict CTestStepTonePlayDualToneInvalidFreq::DoTestStepPreambleL(void)
	{
	//install the scheduler
	TVerdict result = CTestStepUnitMMFAudClient::DoTestStepPreambleL();
	if (result != EPass)
		{
		return result;
		}

	INFO_PRINTF1(_L("Creating tone utility..."));
	
	__MM_HEAP_MARK;
	// create the Tone utility
	TRAPD( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone ||
		 iTone->State() != EMdaAudioToneUtilityNotReady	)
		result = EInconclusive;

	return result;
	}


/**
 *
 * Cleans up the tone utility and the active scheduler.
 * 
 */
enum TVerdict CTestStepTonePlayDualToneInvalidFreq::DoTestStepPostambleL(void)
	{
	delete iTone;
	iTone = NULL;

	__MM_HEAP_MARKEND;

	//destroy the scheduler
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

	
/**
 *
 * Prior to the fix the tone utility would stall, this is caught here.
 *
 */	
void CTestStepTonePlayDualToneInvalidFreq::TimeoutExpired()
	{
	iTestStepResult = EFail;	//Original behaviour prior to the introduction of this test had DevSound dropping the play request quietly resulting in the thread timing out.
	INFO_PRINTF1(_L("ERROR: Audio Tone Utility call timed out."));
	CActiveScheduler::Stop();	//To resume DoTestStepL()
	}


/**
 *
 * Static function used by the TCallBack object.
 * @param aPtr A pointer to the test object that is running.
 *
 */
TInt CTestStepTonePlayDualToneInvalidFreq::TimeoutExpiredStaticCallback(TAny* aPtr)	
	{
	CTestStepTonePlayDualToneInvalidFreq* self = static_cast<CTestStepTonePlayDualToneInvalidFreq*>(aPtr);
	self->TimeoutExpired();
	return 0; //This is a one off event, do not want periodic behaviour.
	}


/**
 *
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 */
TVerdict CTestStepTonePlayDualToneInvalidFreq::DoTestStepL()
	{
	iTestStepResult = EInconclusive;
	
	TInt freq1, freq2, duration;
	
	User::LeaveIfError(GetIntFromConfig(iTestStepName, _L("Frequency1"), freq1));
	User::LeaveIfError(GetIntFromConfig(iTestStepName, _L("Frequency2"), freq2));
	User::LeaveIfError(GetIntFromConfig(iTestStepName, _L("Duration"), duration));

	iTestStepResult = EPass;
	
	INFO_PRINTF4(_L("Playing dual tone with frequencies of %d Hz & %d Hz for %d microseconds."), 
			freq1, freq2, duration);

	INFO_PRINTF1(_L("Calling PrepareToPlayDualTone()..."));
	iTone->PrepareToPlayDualTone(freq1, freq2, TTimeIntervalMicroSeconds(duration));
	CActiveScheduler::Start();
	
	if (iPrepareError != KErrNone || iTone->State() != EMdaAudioToneUtilityPrepared	)
		{
		return EFail;
		}
		
	INFO_PRINTF1(_L("Calling Play()..."));	
	iTone->Play();
	
	//The test case was originally introduced as part of DEF125377 where the Tone Utility was hanging and we use a timer here to catch this.
	TCallBack testTimedOutCallback(TimeoutExpiredStaticCallback, this);
	CPeriodic* timeoutTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	CleanupStack::PushL(timeoutTimer);
	timeoutTimer->Start(KTestTimeout, KTestTimeout, testTimedOutCallback);
	
	CActiveScheduler::Start();
	
	timeoutTimer->Cancel();
	CleanupStack::PopAndDestroy(1, timeoutTimer);
	
	if (iPlayError != KErrArgument)
		{
		iTone->CancelPrepare();
		INFO_PRINTF1(_L("ERROR: KErrArgument expected."));
		return EFail;
		}

	// test steps return a result
	return iTestStepResult;
	}
