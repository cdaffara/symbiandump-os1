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

#ifndef __FORMAT_UTILS_H__
#define __FORMAT_UTILS_H__

const TUint KMMFFormatMinFrameSize = 0x800; //2K
const TUint KMMFFormatMaxFrameSize = 0x4000; //16K
const TUint KMMFFormatDeltaFrameSize = 0x400;  //2K
const TUint KMMFFormatFramesPerSecond = 4; //suply 4 frames per second to the audio drivers

class CMMFDevSound;

IMPORT_C void GetAudioSettingsFromDevice(CMMFDevSound * aSounddevice, 
								TUint& aSampleRate,
								TUint& aNumChannels,
								TFourCC& aCodec);
/*
 * DurationCalculation
 *
 * These methods will provide the duration of the sample
 *
 */

IMPORT_C TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels);
IMPORT_C TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TUint aAvgBytesPerSecond);

IMPORT_C TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TTimeIntervalMicroSeconds aFrameTime,
	TUint aFrameLength);


/*
 * DataPosition
 *
 * These methods will provide the position within the data corresponding to
 * the required time position
 *
 */

IMPORT_C TUint DataPosition(TTimeIntervalMicroSeconds aPosition,
					TUint aSampleRate,
					TUint aBitsPerSample,
					TUint aChannels);


IMPORT_C TUint DataPosition(TTimeIntervalMicroSeconds aPosition,
					TTimeIntervalMicroSeconds aFrameTime,
					TUint aFrameLength);



/*
 * AlignToSample
 *
 * This method provides a utility for aligning positions based upon sample sizes.
 *
 * Outer bounds should be provided to ensure the returned value is within a valid range;
 *
 * The position may be rounded up/down or to nearest sample based upon EMMFRound 
 *
 */

enum EMMFRound
	{
	ERound,
	ERoundUp,
	ERoundDown
	};

IMPORT_C TUint AlignToSample(TUint aNumberToRound, TUint aSampleSize, TUint aLowerLimit, TUint aUpperLimit, EMMFRound aUpOrDown = ERound, TBool aReentrant = false);

IMPORT_C TUint AlignToSample(TUint aNumberToRound, TUint aSampleSize, EMMFRound aUpOrDown = ERound);


/*
 * FrameSizeCalculation
 *
 * These methods provide the playtime that a frame of data contains
 *
 */

IMPORT_C TUint FrameSizeCalculation(
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels);

IMPORT_C TTimeIntervalMicroSeconds FrameTimeIntervalCalculation(
	TUint aFrameSize,
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels);

IMPORT_C TTimeIntervalMicroSeconds FrameTimeIntervalCalculation(
	TUint aFrameSize,
	TUint aAvgBytesPerSecond);




#endif // __FORMAT_UTILS_H__

