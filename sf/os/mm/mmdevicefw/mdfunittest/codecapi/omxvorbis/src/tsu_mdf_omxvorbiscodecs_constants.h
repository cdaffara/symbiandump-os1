// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef TSU_MDF_OMXVORBISCODECS_CONSTANTS_H
#define TSU_MDF_OMXVORBISCODECS_CONSTANTS_H

#define KUidHwDeviceAudioCodecTestAdapter	0x102737dd
#define KUidHwDeviceAudioAdapter	0x10273791
#define KMaxDataTypeLength			126
#define KUidMdfProcessingUnit 		0x10273789
const TUint KVersionOne = 1;

// heap size - we will be opening some large files.
const TInt KTestHeapSize = 0x200000; // 2 Mb

// files we are going to play from
_LIT(KTestDecoderPlayFile, "c:\\mm\\mmf\\testfiles\\mdf\\ogg-decode-in.ogg");
_LIT(KTestEncoderPlayFile, "c:\\mm\\mmf\\testfiles\\mdf\\ogg-encode-in.wav");

// files we are going to record into, with expected sizes
_LIT(KTestDecoderRecordedFile, "c:\\ogg-decode-out.wav");
const TInt KTestDecoderRecordedFileExpectedSize = 1066284;
_LIT(KTestEncoderRecordedFile, "c:\\ogg-encode-out.ogg");
const TInt KTestEncoderRecordedFileExpectedSize = 54942;

// sound parameters for init / config
// appropriate to our test files
const TInt KTestWavFormatPCMChunkHeaderSize = 36;
const TInt KTestWavFormatPCMSubchunk1Size = 16;
const TInt KTestAudioFormatPCM = 1;
const TInt KTestSampleRate = 44100;
const TInt KTestNumChannels = 2;
const TInt KTestBitsPerSample = 16;
const TInt KTestBitRate = 128000;


const TUint32 KMMFFourCCCodeTestVorbis = 0x32425256;		//('V', 'R', 'B', '2')

#endif // TSU_MDF_OMXVORBISCODECS_CONSTANTS_H
