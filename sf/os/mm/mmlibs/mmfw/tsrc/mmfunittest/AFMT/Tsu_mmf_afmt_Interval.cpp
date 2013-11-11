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
// EPOC includes
// 
//

#include <e32base.h>

#include <mmf/server/mmffile.h>  // for TMMFFileParams and CMMFFile
#include <mmf/server/mmfformat.h> // For CMMFFormatDecode
#include <mmf/server/mmfdes.h> // for TMMFDescriptorParams and CMMFDescriptor
#include <mmf/server/mmfdatasource.h> 
#include <mmf/plugin/mmfformatimplementationuids.hrh> // for KMmfUidFormatWAVRead UIDs

//#include "mmfwavFormat.h"  //KGSMAvgBytesPerSecond

// Test system includes
#include "TSU_MMF_AFMT.h"
#include "Tsu_mmf_afmt_Interval.h"
#include "TSU_MMF_AFMTSuite.h"
#include "TSU_MMF_AFMT_testdatasink.h"

#include <mda/common/audio.hrh>
#include <mda/common/gsmaudio.hrh>	//for  KUidMdaGsmWavCodecDefine def

// The FrameTimeInterval should now always be approximately 250 millisecs
// but allow a little variation (50ms) as the calculation is approximate
const TUint KIntervalBounds = 50000;	

const TUint KMMFFormatMinFrameSize = 0x800; //2K
const TUint KMMFFormatMaxFrameSize = 0x4000; //16K
const TUint KMMFFormatDeltaFrameSize = 0x400;  //2K
const TUint KMMFFormatFramesPerSecond = 4; //suply 4 frames per second to the audio drivers

/**
 * This function works out the FrameTimeInterval from the supplied parameters.
 * The return value can then be compared to a real FrameTimeInterval, and they 
 * should roughly match (depending on rounding errors etc).
 * @param aType - The type of audio encoding (PCM, ADPCM, GSM) 
 * @param aSampleRate - The sample rate of the audio data
 * @param aBitsPerSample - The bits per sample of the audio data
 * @param aChannels - The number of channels of the audio data.
 * @return TTimeIntervalMicroSeconds - The estimated FrameTimeInterval.
 */
TTimeIntervalMicroSeconds CTestStep_MMF_AFMT_IntervalL::FindFrameTimeInterval(	const EAudioType& aType, 
																				const TInt& aSampleRate, 
																				const TInt& aBitsPerSample , 
																				const TInt& aChannels,
																				const TUint aSuggestedFrameSize)
	{
	TTimeIntervalMicroSeconds comparison (0);

	// The FrameTimeInterval should now always be approximately 250 millisecs
	TUint frameSize;
	
	if (aSuggestedFrameSize == 0)
		frameSize = ((aSampleRate * aBitsPerSample * aChannels)/(8 * KMMFFormatFramesPerSecond) + (KMMFFormatDeltaFrameSize-1)) &~ (KMMFFormatDeltaFrameSize-1);
	else
		frameSize = aSuggestedFrameSize;

	//clamp buffer to desired limits
	if(frameSize < KMMFFormatMinFrameSize) 
		frameSize = KMMFFormatMinFrameSize;
	else if(frameSize > KMMFFormatMaxFrameSize) 
		frameSize = KMMFFormatMaxFrameSize;

  	switch (aType)
  		{
  		case EAudioPCM:
  		case EAudioADPCM:
			comparison = 
				TTimeIntervalMicroSeconds(
					((TInt64(frameSize) * TInt64(KOneSecondInMicroSeconds) / TInt64(aSampleRate)) /
					TInt64(aBitsPerSample*aChannels)) * 
					TInt64(8));
  			break;
  		case EAudioGSM:
			comparison = 
				TTimeIntervalMicroSeconds(
					TInt64(frameSize) * TInt64(KOneSecondInMicroSeconds) /
					TInt64(KGSMAvgBytesPerSecond));	
  			break;
  		default:
  			User::Leave(KErrArgument);
  			break;
  		};

	
	return comparison;
	}




/**
 * This function runs the FrameTimeIntervalTest, though it requires alot of information
 * to make it's own calculations of what the time interval SHOULD be.
 * @param aType - The type of audio encoding (PCM, ADPCM, GSM) 
 * @param aSampleRate - The sample rate of the audio data
 * @param aBitsPerSample - The bits per sample of the audio data
 * @param aChannels - The number of channels of the audio data.
 * @return TVerdict - EPass if the interval is about right, else EFail.
 */
TVerdict CTestStep_MMF_AFMT_IntervalL::RunTest(const EAudioType& aType,
											   const TInt& aSampleRate, 
											   const TInt& aBitsPerSample, 
											   const TInt& aChannels,
											   const TUint aSuggestedFrameSize)
	{
	TVerdict verdict;

	// Work out how long each block will last
	TTimeIntervalMicroSeconds comparison = 
					FindFrameTimeInterval(aType, aSampleRate, aBitsPerSample, aChannels, aSuggestedFrameSize);
	TInt64 comp64 = comparison.Int64();

	// Make the call to the CMmfXXXFormat object to check the frame time interval
	TTimeIntervalMicroSeconds intval;
	if (iDecode) intval = iFormatDec->FrameTimeInterval(KUidMediaTypeAudio);
	else intval = iFormatEnc->FrameTimeInterval(KUidMediaTypeAudio);
	TInt64 i64 = intval.Int64();

	// Check if the returned interval is above the lower and below the upper
	// bounds check values - if so, it's a pass.
	if (i64 > (comp64-KIntervalBounds) &&	// lower bounds check
		i64 < (comp64+KIntervalBounds) )	// upper bounds check
		verdict = EPass;
	else 
		verdict = EFail;

	return verdict;
	}

	
