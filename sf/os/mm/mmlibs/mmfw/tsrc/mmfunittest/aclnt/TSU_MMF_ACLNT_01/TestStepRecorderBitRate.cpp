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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
// 
//

#include "TestStepRecorderBitRate.h"

// Path where the test files reside
_LIT(KFilePath, "C:\\mm\\mmf\\testfiles\\aclnt\\");

//----- Test for WAV files -----
/**
 *
 * Static constructor for CTestStepRecorderBitRateWav.
 *
 *
 * @return	"CTestStepRecorderBitRateWav*"
 *			The constructed CTestStepRecorderBitRateWav
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRateWav* CTestStepRecorderBitRateWav::NewL()
	{
	CTestStepRecorderBitRateWav* self = new(ELeave) CTestStepRecorderBitRateWav;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRateWav::CTestStepRecorderBitRateWav()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0852-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRateWav::~CTestStepRecorderBitRateWav()
	{
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderBitRateWav::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorCode)
	{
	iError = aErrorCode;
	CActiveScheduler::Stop();
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
TVerdict CTestStepRecorderBitRateWav::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0852-HP"));
	INFO_PRINTF1(_L("This test will read the bit rate of different wav files"));
	
	TInt err = KErrNone;
	TVerdict result = EPass;

	RArray<TPtrC> array;
	array.Append(_L("PCM8BitStereo.wav"));
	array.Append(_L("PCM 8-Bit,8000 Hz,128 Kbps,Stereo"));
	array.Append(_L("PCM8BitMono.wav"));
	array.Append(_L("PCM 8-Bit,8000 Hz,64 Kbps,Mono"));
	array.Append(_L("PCM16BitStereo.wav"));
	array.Append(_L("PCM 16-Bit,8000 Hz,256 Kbps,Stereo"));
	array.Append(_L("PCM16BitMono.wav"));
	array.Append(_L("PCM 16-Bit,8000 Hz,128 Kbps,Mono"));
	
	CMMFMdaAudioRecorderUtility* recorder = CMMFMdaAudioRecorderUtility::NewL(*this);

	if (recorder == NULL ||
		 iError != KErrNone)
		{
		 return EInconclusive;
		}
	
	CleanupStack::PushL(recorder);
	TInt count = array.Count();
	for (TInt iter=0; iter<count; iter+=2)
		{
		TUint bitRate = 0;	
		TBuf<256> fileBuf;
		
		// Append the test file path
		fileBuf.Append(KFilePath);
		// Append the test file name
		fileBuf.Append(array[iter]);
		
		INFO_PRINTF2(_L("Opening file = %S"), &fileBuf);
		INFO_PRINTF2(_L("File info = %S"), &array[iter+1]);
	
		TRAP(err,recorder->OpenFileL(fileBuf));
		// If the opening fails, save the test result as fail and continue looping
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Opening file %S failed"), &fileBuf);
			result = EFail;
			continue;
			}
		CActiveScheduler::Start();
		// Get the bit rate of the file.
		TRAPD(error,bitRate = recorder->SourceBitRateL());
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("Obtaining the bit rate for the file %S failed"), &fileBuf);
			result = EFail;
			}
		else
			{
			INFO_PRINTF2(_L("Bit rate of the file = %d bps"), bitRate);	
			}
		
		// Close the file.
		recorder->Close();
		
		if ( err != KErrNone ||
		 	 iError != KErrNone)
			{
			result = EFail;
			}	
		}
	CleanupStack::PopAndDestroy(recorder);
			 
	INFO_PRINTF1(_L("Finished with this test step"));
	// test steps return a result
	return result;
	}

//----- Test for AU files -----
/**
 *
 * Static constructor for CTestStepRecorderBitRateAu.
 *
 *
 * @return	"CTestStepRecorderBitRateAu*"
 *			The constructed CTestStepRecorderBitRateAu
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRateAu* CTestStepRecorderBitRateAu::NewL()
	{
	CTestStepRecorderBitRateAu* self = new(ELeave) CTestStepRecorderBitRateAu;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRateAu::CTestStepRecorderBitRateAu()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0853-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderBitRateAu::~CTestStepRecorderBitRateAu()
	{
	}

/**
 *
 * Callback Handle.
 *
 * @xxxx
 * 
 */
void CTestStepRecorderBitRateAu::MoscoStateChangeEvent(CBase* /*aObject*/,
														  TInt /*aPreviousState*/,
														  TInt /*aCurrentState*/,
														  TInt aErrorCode)
	{
	iError = aErrorCode;
	CActiveScheduler::Stop();
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
TVerdict CTestStepRecorderBitRateAu::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0853-HP"));
	INFO_PRINTF1(_L("This test will read the bit rate of a different AU files"));
	
	TInt err = KErrNone;
	TVerdict result = EPass;

	RArray<TPtrC> array;
	array.Append(_L("PCM8BitStereo.au"));
	array.Append(_L("PCM 8-Bit,8000 Hz,128 Kbps,Stereo"));
	array.Append(_L("PCM8BitMono.au"));
	array.Append(_L("PCM 8-Bit,8000 Hz,64 Kbps,Mono"));
	array.Append(_L("PCM16BitStereo.au"));
	array.Append(_L("PCM 16-Bit,8000 Hz,256 Kbps,Stereo"));
	array.Append(_L("PCM16BitMono.au"));
	array.Append(_L("PCM 16-Bit,8000 Hz,128 Kbps,Mono"));
	
	CMMFMdaAudioRecorderUtility* recorder = CMMFMdaAudioRecorderUtility::NewL(*this);

	if (recorder == NULL ||
		 iError != KErrNone)
		{
		 return EInconclusive;
		}
	
	CleanupStack::PushL(recorder);
	TInt count = array.Count();
	for (TInt iter=0; iter<count; iter+=2)
		{
		TUint bitRate = 0;	
		TBuf<256> fileBuf;
		
		// Append the test file path
		fileBuf.Append(KFilePath);
		// Append the test file name
		fileBuf.Append(array[iter]);
		
		INFO_PRINTF2(_L("Opening file = %S"), &fileBuf);
		INFO_PRINTF2(_L("File info = %S"), &array[iter+1]);
	
		TRAP(err,recorder->OpenFileL(fileBuf));
		// If the opening fails, save the test result as fail and continue looping
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Opening file %S failed"), &fileBuf);
			result = EFail;
			continue;
			}
		CActiveScheduler::Start();
		// Get the bit rate of the file.
		TRAPD(error, bitRate = recorder->SourceBitRateL());
		if (error != KErrNone)
			{
			INFO_PRINTF2(_L("Obtaining the bit rate for the file %S failed"), &fileBuf);
			result = EFail;
			}
		else
			{
			INFO_PRINTF2(_L("Bit rate of the file = %d bps"), bitRate);	
			}
		// Close the file.
		recorder->Close();
		
		if ( err != KErrNone ||
		 	 iError != KErrNone)
			{
			result = EFail;
			}	
		}
	CleanupStack::PopAndDestroy(recorder);
			 
	INFO_PRINTF1(_L("Finished with this test step"));
	// test steps return a result
	return result;
	}
