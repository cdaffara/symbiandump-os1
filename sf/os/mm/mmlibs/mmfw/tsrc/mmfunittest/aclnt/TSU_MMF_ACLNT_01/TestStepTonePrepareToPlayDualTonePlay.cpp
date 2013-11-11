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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
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

/**
 *
 * Static constructor for CTestStepTonePrepareToPlayDualTonePlay.
 *
 *
 * @return	"CTestStepTonePrepareToPlayDualTonePlay*"
 *			The constructed CTestStepTonePrepareToPlayDualTonePlay
 *
 */
CTestStepTonePrepareToPlayDualTonePlay* CTestStepTonePrepareToPlayDualTonePlay::NewL()
	{
	CTestStepTonePrepareToPlayDualTonePlay* self = new(ELeave) CTestStepTonePrepareToPlayDualTonePlay;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 */
CTestStepTonePrepareToPlayDualTonePlay::CTestStepTonePrepareToPlayDualTonePlay() : iError(KErrNone), iHasPlayed(EFalse)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0467-CP");
	// need a bigger heap size on EKA2 HW
#if !defined __WINS__
	iHeapSize = KHeapSizeToneTestEKA2;
#endif // EKA2
	}

/**
 *
 * Test step destructor.
 *
 */
CTestStepTonePrepareToPlayDualTonePlay::~CTestStepTonePrepareToPlayDualTonePlay()
	{
	}

void CTestStepTonePrepareToPlayDualTonePlay::MatoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepTonePrepareToPlayDualTonePlay::MatoPlayComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

/**
 *
 * Test step Preamble.
 * Creates a tone utility
 * 
 */
enum TVerdict CTestStepTonePrepareToPlayDualTonePlay::DoTestStepPreambleL(void)
	{
    TInt err       = KErrNone;
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("this is a test of CMdaAudioToneUtility::PrepareToPlayDualTone()"));
	
	__MM_HEAP_MARK;
	// create the Tone utility
	TRAP( err, (iTone = CMMFMdaAudioToneUtility::NewL(*this)) );
	if ( err != KErrNone ||
		 iTone->State() != EMdaAudioToneUtilityNotReady	)
		verdict = EInconclusive;

	return verdict;

	}

/**
 *
 * Test step Postamble.
 * destroys the tone utility
 * 
 */
enum TVerdict CTestStepTonePrepareToPlayDualTonePlay::DoTestStepPostambleL(void)
	{
	delete iTone;
	iTone = NULL;

	__MM_HEAP_MARKEND;

	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}

class TDualTone
	{
public:
	TInt iFreq1;
	TInt iFreq2;
	};

const TDualTone KDtmfToneTable[] = 
	{
	{941, 1336},//0
	{697, 1209},//1
	{697, 1336},//2
	{697, 1477},//3
	{770, 1209},//4
	{770, 1336},//5
	{770, 1477},//6
	{852, 1209},//7
	{852, 1336},//8
	{852, 1477},//9

	{697, 1633},//A
	{770, 1633},//B
	{852, 1633},//C
	{941, 1633},//D
	{941, 1209},//E or *
	{941, 1477},//F or #
	{0,   0}	// pause
	};


_LIT(KTwinkleDTMFString,"1 1 9 9 # # 9-6 6 3 3 2 2 1-9 9 6 6 3 3 2-9 9 6 6 3 3 2-1 1 9 9 # # 9-6 6 3 3 2 2 1-");

const TInt KQuarterNote = 256000;
const TInt KHalfNote = 512000;
const TInt KEighthNote =  128000;
const TInt KSixteenthNote = 64000;


/**
 * convert a DTMF string into an array of TToneInfo
 */
TInt CTestStepTonePrepareToPlayDualTonePlay::GetTune(const TDesC& aDtmfString, RArray<TToneInfo>& aTones)
	{
	TInt count = aDtmfString.Length();
	for (TInt index=0; index<count; )
		{
		TInt tableIndex;
		TInt c = (aDtmfString)[index++];
		if (c >= '0' && c <= '9')
			tableIndex = c - '0';
		else if (c >= 'A' && c <= 'F')
			tableIndex = c - 'A' + 10;
		else if (c == '*')
			tableIndex = 14;
		else if (c == '#')
			tableIndex = 15;
		else if (c == ',')		// pause ?
			tableIndex = 16;
		else
			return KErrArgument;

		TToneInfo tone;
		tone.iFreq1 = KDtmfToneTable[tableIndex].iFreq1;
		tone.iFreq2 = KDtmfToneTable[tableIndex].iFreq2;

		c = (aDtmfString)[index++];
		if (c == ' ')
			tone.iDuration = KQuarterNote; 
		else if (c == '-')
			tone.iDuration = KHalfNote; 
		else if (c == '+')
			tone.iDuration = KEighthNote; 
		else if (c == '%')
			tone.iDuration = KSixteenthNote; 
		else
			return KErrArgument;
		aTones.Append(tone);
		}
	return KErrNone;
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepTonePrepareToPlayDualTonePlay::DoTestStepL()
	{
	iTestStepResult = EPass;

	RArray<TToneInfo> tones;
	
	// This test plays "twinkle twinkle little star" 
	// by converting a DTMF sequence into an array of dualtones 
	// (i.e. frequency pairs) and then calling PrepareToPlayDualTone() 
	// for each dualtone
	TInt error = GetTune(*(&KTwinkleDTMFString), tones);
	if (error != KErrNone)
		return EInconclusive;

	for (TInt index=0; index<tones.Count(); index++)
		{
		TToneInfo tone = tones[index];
		
		INFO_PRINTF4(_L("playing dual tone of %d Hz & %d Hz for %f seconds"), 
			tone.iFreq1, tone.iFreq2,  ((TReal32)tone.iDuration)/1000000);

		// if the frequency is zero, then pause i.e don't play a note
		if (tone.iFreq1 != 0)
			{
			iTone->PrepareToPlayDualTone(tone.iFreq1, tone.iFreq2, TTimeIntervalMicroSeconds(tone.iDuration));
			CActiveScheduler::Start();
		
			if (iError != KErrNone || iTone->State() != EMdaAudioToneUtilityPrepared	)
				return EInconclusive;
		
			iTone->Play();
			if (iTone->State() == EMdaAudioToneUtilityPlaying)
				iHasPlayed = ETrue;
			CActiveScheduler::Start();
			if (iError != KErrNone || !iHasPlayed)
				return EFail;
			iTone->CancelPrepare();
			}

		User::After(125000);
		}

	tones.Close();

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

