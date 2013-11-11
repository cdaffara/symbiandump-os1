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

#include "TestStepPlayerBitRate.h"

// Path where the test files reside
_LIT(KFilePath, "C:\\mm\\mmf\\testfiles\\aclnt\\");

//----- Test for WAV files -----

/**
 *
 * Static constructor for CTestStepPlayerBitRateWav.
 *
 *
 * @return	"CTestStepPlayerBitRateWav*"
 *			The constructed CTestStepPlayerBitRateWav
 *
 * @xxxx
 * 
 */
CTestStepPlayerBitRateWav* CTestStepPlayerBitRateWav::NewL()
	{
	CTestStepPlayerBitRateWav* self = new(ELeave) CTestStepPlayerBitRateWav;
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
CTestStepPlayerBitRateWav::CTestStepPlayerBitRateWav()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0850-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerBitRateWav::~CTestStepPlayerBitRateWav()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerBitRateWav::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerBitRateWav::MapcPlayComplete(TInt aError)
	{
	iError = aError;
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
TVerdict CTestStepPlayerBitRateWav::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0850-HP"));
	INFO_PRINTF1(_L("This test will read the bit rate of different wav files."));
	
	TInt err = KErrNone;
	TInt state = 0;
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
	array.Append(_L("uLawStereo.wav"));
	array.Append(_L("u-Law 8000 Hz,128 Kbps,Stereo"));
	array.Append(_L("uLawMono.wav"));
	array.Append(_L("u-Law 8000 Hz,64 Kbps,Mono"));
	array.Append(_L("ALawStereo.wav"));
	array.Append(_L("A-Law 8000 Hz,128 Kbps,Stereo"));
	array.Append(_L("ALawMono.wav"));
	array.Append(_L("A-Law 8000 Hz,64 Kbps,Mono"));
	
	CMMFMdaAudioPlayerUtility* player = CMMFMdaAudioPlayerUtility::NewL(*this);

	if (player == NULL ||
		 iError != KErrNone)
		{
		 return EInconclusive;
		}
	
	// check if player openned the file successfully
	err = GetAudPlayerState(player, state);
	if ( state != CMMFMdaAudioPlayerUtility::EStopped )
		{
		return EInconclusive;
		}
		
	CleanupStack::PushL(player);
	TInt count = array.Count();
	for (TInt iter=0; iter<count; iter+=2)
		{
		TUint bitRate = 0;	
				
		TBuf<256> fileBuf;
		fileBuf.Append(KFilePath);
		// Append the file name
		fileBuf.Append(array[iter]);
		
		INFO_PRINTF2(_L("Opening file = %S"), &fileBuf);
		INFO_PRINTF2(_L("File info = %S"), &array[iter+1]);
	
		TRAP(err,player->OpenFileL(fileBuf));
		// If the opening fails, save the test result as fail and continue looping
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Opening file %S failed"), &fileBuf);
			result = EFail;
			continue;
			}
		CActiveScheduler::Start();
		// Get the bit rate of the file being played.
		err = player->GetBitRate(bitRate);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Obtaining bit rate for the file %S failed"), &fileBuf);
			result = EFail;
			}
		else
			{
			INFO_PRINTF2(_L("Bit rate of the file = %d bps"), bitRate);		
			}
		
		// Close the file before opening the second file
		player->Close();
		
		err = GetAudPlayerState(player, state);
		if ( err != KErrNone ||
			 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 	 iError != KErrNone)
			{
			result = EFail;
			}	
		}
	CleanupStack::PopAndDestroy(player);
	
	INFO_PRINTF1(_L("Finished with this test step"));
	// test steps return a result
	return result;
	}

//----- Test for AU files -----
	
/**
 *
 * Static constructor for CTestStepPlayerBitRateAU.
 *
 *
 * @return	"CTestStepPlayerBitRateAU*"
 *			The constructed CTestStepPlayerBitRateAU
 *
 * @xxxx
 * 
 */
CTestStepPlayerBitRateAU* CTestStepPlayerBitRateAU::NewL()
	{
	CTestStepPlayerBitRateAU* self = new(ELeave) CTestStepPlayerBitRateAU;
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
CTestStepPlayerBitRateAU::CTestStepPlayerBitRateAU()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0851-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepPlayerBitRateAU::~CTestStepPlayerBitRateAU()
	{
	}
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepPlayerBitRateAU::MapcInitComplete(TInt aError, 
								const TTimeIntervalMicroSeconds &/*aDuration*/)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepPlayerBitRateAU::MapcPlayComplete(TInt aError)
	{
	iError = aError;
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
TVerdict CTestStepPlayerBitRateAU::DoTestStepL()
	{
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0851-HP"));
	INFO_PRINTF1(_L("This test will read the bit rate of different AU files."));
	
	TInt err = KErrNone;
	TInt state = 0;
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
	array.Append(_L("uLawStereo.au"));
	array.Append(_L("u-Law 8000 Hz,128 Kbps,Stereo"));
	array.Append(_L("uLawMono.au"));
	array.Append(_L("u-Law 8000 Hz,64 Kbps,Mono"));
	array.Append(_L("ALawStereo.au"));
	array.Append(_L("A-Law 8000 Hz,128 Kbps,Stereo"));
	array.Append(_L("ALawMono.au"));
	array.Append(_L("A-Law 8000 Hz,64 Kbps,Mono"));
	
	CMMFMdaAudioPlayerUtility* player = CMMFMdaAudioPlayerUtility::NewL(*this);

	if (player == NULL ||
		 iError != KErrNone)
		{
		 return EInconclusive;
		}
	
	err = GetAudPlayerState(player, state);
	if ( state != CMMFMdaAudioPlayerUtility::EStopped )
		{
		return EInconclusive;
		}
		
	CleanupStack::PushL(player);
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
	
		TRAP(err,player->OpenFileL(fileBuf));
		// If the opening fails, save the test result as fail and continue looping
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Opening file %S failed"), &fileBuf);
			result = EFail;
			continue;
			}
		CActiveScheduler::Start();
		// Get the bit rate of the file.
		err = player->GetBitRate(bitRate);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Obtaining the bit rate for the file %S failed"), &fileBuf);
			result = EFail;
			}
		else
			{
			INFO_PRINTF2(_L("Bit rate of the file = %d bps"), bitRate);	
			}
		
		// Close the file.
		player->Close();
		
		err = GetAudPlayerState(player, state);
		if ( err != KErrNone ||
			 state != CMMFMdaAudioPlayerUtility::EStopped ||
		 	 iError != KErrNone)
			{
			result = EFail;
			}	
		}
	CleanupStack::PopAndDestroy(player);

	INFO_PRINTF1(_L("Finished with this test step"));
	// test steps return a result
	return result;
	}

