// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels)
	{
	TTimeIntervalMicroSeconds duration(0); //default

	if ((aDataLength) && (aSampleRate) && (aBitsPerSample) && (aChannels))
		{
		duration = TTimeIntervalMicroSeconds(
			TInt64(aDataLength) * TInt64(KOneSecondInMicroSeconds) / TInt64(aSampleRate) /
			 (TInt64(aBitsPerSample) * TInt64(aChannels))
			* TInt64(8));
		}
	return duration;
	}

TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TUint aAvgBytesPerSecond)
	{
	return (TTimeIntervalMicroSeconds(
		(TInt64(aDataLength) * TInt64(KOneSecondInMicroSeconds)) /
		TInt64(aAvgBytesPerSecond)));
	}

TUint FrameSizeCalculation(
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

TTimeIntervalMicroSeconds FrameTimeIntervalCalculation(
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
				((TInt64(aFrameSize) * TInt64(KOneSecondInMicroSeconds) / TInt64(aSampleRate)) /
				TInt64(aBitsPerSample*aChannels)) * 
				TInt64(8));
		}

	return(frameTimeInterval);
	}

TTimeIntervalMicroSeconds FrameTimeIntervalCalculation(
	TUint aFrameSize,
	TUint aAvgBytesPerSecond)
	{
	return(TTimeIntervalMicroSeconds(
		TInt64(aFrameSize) * TInt64(KOneSecondInMicroSeconds) /
		TInt64(aAvgBytesPerSecond)));	

	}


