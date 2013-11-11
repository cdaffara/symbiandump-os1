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

#ifndef __FORMAT_UTILS_H__
#define __FORMAT_UTILS_H__

const TUint KMMFFormatMinFrameSize = 0x800; //2K
const TUint KMMFFormatMaxFrameSize = 0x4000; //16K
const TUint KMMFFormatDeltaFrameSize = 0x400;  //2K
const TUint KMMFFormatFramesPerSecond = 4; //suply 4 frames per second to the audio drivers

TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels);
TTimeIntervalMicroSeconds DurationCalculation(
	TUint aDataLength,
	TUint aAvgBytesPerSecond);

TUint FrameSizeCalculation(
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels);

TTimeIntervalMicroSeconds FrameTimeIntervalCalculation(
	TUint aFrameSize,
	TUint aSampleRate,
	TUint aBitsPerSample,
	TUint aChannels);
TTimeIntervalMicroSeconds FrameTimeIntervalCalculation(
	TUint aFrameSize,
	TUint aAvgBytesPerSecond);




#endif // __FORMAT_UTILS_H__

