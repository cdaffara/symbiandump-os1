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
//

#include <mmf/server/sounddevice.h>
#include <mmf/common/mmffourcc.h>
#include "formatUtils.h"

const TUint KOneSecondInMicroSeconds = 1000000;

EXPORT_C void GetAudioSettingsFromDevice(CMMFDevSound * aSoundDevice, 
								TUint& aSampleRate,
								TUint& aNumChannels,
								TFourCC& aCodec)
	{
		TMMFCapabilities config = aSoundDevice->Config();

		switch (config.iRate)
			{
			case EMMFSampleRate8000Hz : aSampleRate = 8000; 
				break;
			case EMMFSampleRate11025Hz : aSampleRate = 11025;
				break;
			case EMMFSampleRate12000Hz : aSampleRate = 12000;
				break;
			case EMMFSampleRate16000Hz : aSampleRate = 16000;
				break;
			case EMMFSampleRate22050Hz : aSampleRate = 22050;
				break;
			case EMMFSampleRate24000Hz : aSampleRate = 24000;
				break;
			case EMMFSampleRate32000Hz : aSampleRate = 32000;
				break;
			case EMMFSampleRate44100Hz : aSampleRate = 44100;
				break;
			case EMMFSampleRate48000Hz : aSampleRate = 48000;
				break;
			case EMMFSampleRate64000Hz : aSampleRate = 64000;
				break;
			case EMMFSampleRate88200Hz : aSampleRate = 88200;
				break;
			case EMMFSampleRate96000Hz : aSampleRate = 96000;
				break;
			default:
				// should panic here, as an extra sample rate has been added, or we have an incorrect value
				break;
			}
		switch (config.iChannels)
			{
			case EMMFMono : aNumChannels = 1;
				break;
			case EMMFStereo : aNumChannels = 2;
				break;
			default:
				// should panic
				break;
			}
		switch (config.iEncoding)
			{
			case EMMFSoundEncoding8BitPCM : aCodec = KMMFFourCCCodePCM8;
				break;
			case EMMFSoundEncoding16BitPCM : aCodec = KMMFFourCCCodePCM16;
				break;
			case EMMFSoundEncoding8BitALaw : aCodec = KMMFFourCCCodeALAW;
				break;
			case EMMFSoundEncoding8BitMuLaw : aCodec = KMMFFourCCCodeMuLAW;
				break;
			default:
				// should panic
				break;
			}
	}

EXPORT_C TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels)
	{
	TTimeIntervalMicroSeconds duration(0); //default

	if ((aDataLength) && (aSampleRate) && (aBitsPerSample) && (aChannels))
		{
		TInt64 sampleRate=static_cast<TInt64>(aSampleRate);

		duration = TTimeIntervalMicroSeconds(
			(TInt64(aDataLength) * TInt64(KOneSecondInMicroSeconds) * TInt64(8))
			/ (sampleRate * TInt64(aBitsPerSample) * TInt64(aChannels)));
		}

	return duration;
	}

EXPORT_C TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TUint aAvgBytesPerSecond)
	{
	TTimeIntervalMicroSeconds duration(0); //default

	if (aAvgBytesPerSecond)
		duration = TTimeIntervalMicroSeconds(
			(TInt64(aDataLength) * TInt64(KOneSecondInMicroSeconds)) /
			TInt64(aAvgBytesPerSecond));

	return duration;
	}


EXPORT_C TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TTimeIntervalMicroSeconds aFrameTime,
	TUint aFrameLength)
	{
	TTimeIntervalMicroSeconds duration(0); //default

	if ((aFrameTime.Int64() != 0) && aFrameLength)
		{
		const TInt64 frameTime = aFrameTime.Int64();
		duration = TTimeIntervalMicroSeconds(
			TInt64(aDataLength) / (TInt64(aFrameLength) * frameTime));
		}

	return duration;
	}


EXPORT_C TUint DataPosition(TTimeIntervalMicroSeconds aPosition,
					TUint aSampleRate,
					TUint aBitsPerSample,
					TUint aChannels)
	{
	TInt64 dataPosition64(0);

 	dataPosition64 = 
		(aPosition.Int64() * TInt64(aSampleRate) * 
		TInt64(aBitsPerSample) * TInt64(aChannels)) / 
		(TInt64(KOneSecondInMicroSeconds) * TInt64(8));
	
	return I64INT(dataPosition64);
	}


EXPORT_C TUint DataPosition(TTimeIntervalMicroSeconds aPosition,
					TTimeIntervalMicroSeconds aFrameTime,
					TUint aFrameLength)
	{
	TInt64 dataPosition64(0);

	if(aFrameTime.Int64() != 0)
		dataPosition64 = aPosition.Int64() * aFrameLength / aFrameTime.Int64() ;

	return I64INT(dataPosition64);
	}




EXPORT_C TUint AlignToSample(TUint aNumberToRound, TUint aSampleSize, EMMFRound aUpOrDown)
	{
	return AlignToSample(aNumberToRound, aSampleSize, 0 , KMaxTUint, aUpOrDown);
	}



EXPORT_C TUint AlignToSample(TUint aNumberToRound, TUint aSampleSize, TUint aLowerLimit, TUint aUpperLimit, EMMFRound aUpOrDown, TBool aReentrant)
	{

	//Make sure the upper and lower limits are aligned
	if(!aReentrant)
		{
		aLowerLimit = AlignToSample(aLowerLimit, aSampleSize, aLowerLimit, aUpperLimit, ERoundUp, ETrue);
		aUpperLimit = AlignToSample(aUpperLimit, aSampleSize, aLowerLimit, aUpperLimit, ERoundDown, ETrue);
		}

	
	TUint outBy = aNumberToRound % aSampleSize;
	if(outBy)
		{
		if(aUpOrDown == ERoundDown)
			{
			aNumberToRound = aNumberToRound  - outBy;
			if(aNumberToRound < aLowerLimit)
				{
				aNumberToRound = aNumberToRound + aSampleSize;
				}
			}
		else if (aUpOrDown == ERoundUp)
			{
			aNumberToRound = aNumberToRound  + (aSampleSize - outBy);
			if(aNumberToRound > aUpperLimit)
				{
				aNumberToRound = aNumberToRound - aSampleSize;
				}
			}
		else
			{
			if(outBy > aSampleSize/2) //then round up
				aNumberToRound = AlignToSample(aNumberToRound, aSampleSize, aLowerLimit, aUpperLimit, ERoundUp, true);
			else //round down
				aNumberToRound = AlignToSample(aNumberToRound, aSampleSize, aLowerLimit, aUpperLimit, ERoundDown, true);
			}
		}


	if(aNumberToRound < aLowerLimit)
		return aLowerLimit;
	else if(aNumberToRound > aUpperLimit)
		return aUpperLimit;

	return aNumberToRound;
	}








EXPORT_C TUint FrameSizeCalculation(
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels)
	{
	TUint frameSize = ((aSampleRate * aBitsPerSample * aChannels)/(8 * KMMFFormatFramesPerSecond) + (KMMFFormatDeltaFrameSize-1)) &~ (KMMFFormatDeltaFrameSize-1);

	//clamp buffer to desired limits
	if(frameSize < KMMFFormatMinFrameSize) 
		frameSize = KMMFFormatMinFrameSize;
	else if(frameSize > KMMFFormatMaxFrameSize) 
		frameSize = KMMFFormatMaxFrameSize;

	return(frameSize);	
	}

EXPORT_C TTimeIntervalMicroSeconds FrameTimeIntervalCalculation(
	TUint aFrameSize,
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels)
	{
	TTimeIntervalMicroSeconds frameTimeInterval(0); //default

	if ((aSampleRate) && (aBitsPerSample) && (aChannels))
		{
		frameTimeInterval = 
			TTimeIntervalMicroSeconds(
				(TInt64(aFrameSize) * TInt64(KOneSecondInMicroSeconds) * TInt64(8))
				/ (TInt64(aSampleRate) * TInt64(aBitsPerSample) * TInt64(aChannels)));
		}

	return(frameTimeInterval);
	}

EXPORT_C TTimeIntervalMicroSeconds FrameTimeIntervalCalculation(
	TUint aFrameSize,
	TUint aAvgBytesPerSecond)
	{
	return(TTimeIntervalMicroSeconds(
		TInt64(aFrameSize) * TInt64(KOneSecondInMicroSeconds) /
		TInt64(aAvgBytesPerSecond)));	

	}


