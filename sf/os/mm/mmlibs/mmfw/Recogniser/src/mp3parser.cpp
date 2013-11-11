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


#include "parsers.h"

//
// Masks used for reading bits.
//
static const TInt KMP3Sync1Mask = 0xff;			// 11111111
static const TInt KMP3Sync2Mask = 0xe0;			// 11100000
static const TInt KMP3VersionMask = 0x18;		// 00011000
static const TInt KMP3LayerMask = 0x06;			// 00000110
static const TInt KMP3BitrateMask = 0xf0;		// 11110000
static const TInt KMP3SampleRateMask = 0x0c;	// 00001100
static const TInt KMP3PaddingMask = 0x02;		// 00000010

static const TInt KBitsPerByte = 8;

//
// Macros for retrieving the values.
//
#define MP3_GET_SYNC1(d)				(d & KMP3Sync1Mask)
#define MP3_GET_SYNC2(d)				((d & KMP3Sync2Mask) >> 5)
#define MP3_GET_VERSION(d)				((d & KMP3VersionMask) >> 3)
#define MP3_GET_LAYER(d)				((d & KMP3LayerMask) >> 1)
#define MP3_GET_BITRATE(d)				((d & KMP3BitrateMask) >> 4)
#define MP3_GET_SAMPLE_RATE(d)			((d & KMP3SampleRateMask) >> 2)
#define MP3_GET_PADDING(d)				((d & KMP3PaddingMask) >> 1)


//
// Macros used for checking various bitfields.
//
#define IS_BAD_MP3_FRAME_SYNC1(s)		((s) != 0xff)
#define IS_BAD_MP3_FRAME_SYNC2(s)		((s) != 0x07)	
#define IS_BAD_MP3_VERSION(v)			((v) <  0x02)
#define IS_BAD_MP3_LAYER(l)				((l) == 0x00) 
#define IS_BAD_MP3_BITRATE(b)			((b) == 0x0f)	
#define IS_BAD_MP3_SAMPLE_RATE(s)		((s) == 0x03)


// 
// Meanings of the 'Version' field.
// --------------------------------
// 00 - MPEG Version 2.5 (Unofficial standard. We don't support it.)
// 01 - Reserved
// 10 - MPEG Version 2
// 11 - MPEG Version 1
//
static const TInt8 KMP3Version2 = 2;
static const TInt8 KMP3Version1 = 3;


// 
// Meanings of the 'Layer' field.
// ------------------------------
// 00 - Reserved
// 01 - Layer III
// 10 - Layer II
// 11 - Layer I
//

static const TUint16 KBad = 0;
static const TUint16 KFree = 0;

//
// MPEG Version 1 bitrates. Measured in kilobits per second.
//
static const TUint16 KBitrateVersion1[4][16] =
	{
	{ KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad}, // Reserved layer
	{KFree,   32,   40,   48,   56,   64,   80,   96,  112,  128,  160,  192,  224,  256,  320, KBad}, // Layer III
	{KFree,   32,   48,   56,   64,   80,   96,  112,  128,  160,  192,  224,  256,  320,  384, KBad}, // Layer II
	{KFree,   32,   64,   96,  128,  160,  192,  224,  256,  288,  320,  352,  384,  416,  448, KBad}  // Layer I
	};

//
// MPEG Version 2 bitrates. Measured in kilobits per second.
//
static const TUint16 KBitrateVersion2[4][16] =
	{
	{ KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad, KBad}, // Reserved layer
	{KFree,    8,   16,   24,   32,   40,   48,   56,   64,   80,   96,  112,  128,  144,  160, KBad}, // Layer III 
	{KFree,    8,   16,   24,   32,   40,   48,   56,   64,   80,   96,  112,  128,  144,  160, KBad}, // Layer II
	{KFree,   32,   48,   56,   64,   80,   96,  112,  128,  144,  160,  176,  192,  224,  256, KBad}  // Layer I
	};

//
// Sample rates for the MPEG versions.
//
static const TUint16 KSampleRate[4][4] = 
	{
		{11025, 12000,  8000, KBad}, // Version 2.5
		{ KBad,  KBad,  KBad, KBad}, // Reserved
		{22050, 24000, 16000, KBad}, // Version 2
		{44100, 48000, 32000, KBad}  // Version 1
	};

//
// Sample rates per frame for the MPEG layers.
//
static const TUint16 KSamplesPerFrame[4][4] = 
	{
		{KBad, KBad, KBad, KBad}, // Reserved layer
		{ 576, KBad,  576, 1152}, // Layer III
		{1152, KBad, 1152, 1152}, // Layer II
		{ 384, KBad,  384,  384}  // Layer I
	};

//
// The various states the recognition process goes through.
//
typedef enum
	{
	ESearchFrame1,
	ESearchFrame2
	}
TMP3State;

//
// This truth table maps the following flags to a confidence level.
// -----------------------------------------------------------------
// A: Frame2 found.
// B: Frame1 found.
// C: Extension recognised.
//
// A B C -> Confidence
// ===================
// 0 0 0 -> ENotRecognized
// 0 0 1 -> EPossible
// 0 1 0 -> ENotRecognized
// 0 1 1 -> ECertain
// 1 0 0 -> EPossible
// 1 0 1 -> EProbable
// 1 1 0 -> EProbable
// 1 1 1 -> ECertain
//
// In the case where two consecutive mp3 frames
// are found, ECertain is automatically returned.
//
static const TInt KMP3FlagsToConfidence[8] = 
	{
	KConfNotRecognised,
	KConfPossible,
	KConfNotRecognised,
	KConfCertain,
	KConfPossible,
	KConfProbable,
	KConfProbable,
	KConfCertain
	};

#define KMP3ConfidenceMask	0x07	// 00000111
#define KMP3Frame1Bit		KBit1
#define KMP3Frame2Bit		KBit2

//
//
//
TMP3Parser::TMP3Parser(CReader& aReader, TFlags& aFlags)
 : 	iReader(aReader),
 	iFlags(aFlags)
	{
	}


//
// MP3 recogition function.
//
void TMP3Parser::DoRecognise(const TDesC& aExt, CReader& aReader, TMatch& aMatch)
	{
	TFlags flags;
	
	// Try to match the extension.
	if (aExt.MatchF(TPtrC(KExtMP3)) != KErrNotFound)
		{
		flags.SetExtensionFlag();
		}

	// Parse the data.
	TMP3Parser parser(aReader, flags);
	TRAP_IGNORE(parser.ParseL());
	
	TInt confIndex = flags.GetBitField(KMP3ConfidenceMask);
	aMatch.iConfidence = KMP3FlagsToConfidence[confIndex];
	if (aMatch.iConfidence != KConfNotRecognised)
		{
		aMatch.iMime = KMimeMP3;
		}
	}


//
// Attempts to parse an mp3 file.
// First of all it checks if there is an ID3 metadata header
// present at the current reader position. Then it checks for up to
// two consecutive mp3 audio frames.
//
void TMP3Parser::ParseL()
	{
	TInt length;
	TMP3State state = ESearchFrame1;

	FOREVER
		{
		TID3Parser::ReadAndSkipID3L(iReader);
		
		TInt err = CheckForFrameHeaderL(length);
		if (err == KErrNotFound)
			{
			return;
			}
			
		switch (state)
			{
			case ESearchFrame1:
				iFlags.SetBit(KMP3Frame1Bit);
				state = ESearchFrame2;
				break;
					
			case ESearchFrame2:
				iFlags.SetBit(KMP3Frame2Bit);
				return;
			}
				
		// Skip over the audio frame.
		// This should be done after flags have been set.
		iReader.SeekL(length - KMP3FrameHeaderSize);
		}
	}
	

//
// Checks for an MP3 frame header at the current reader position.
// If one is not found KErrNotFound is returned and aFrameLength
// remains unchanged. If one is found KErrNone is retured and
// aFrameLength is set to the length of the frame.
//
TInt TMP3Parser::CheckForFrameHeaderL(TInt& aFrameLength)
	{
	TBuf8<KMP3FrameHeaderSize> data;
	TUint8 versionIndex;
	TUint8 layerIndex;
	TUint8 bitrateIndex;
	TUint8 sampleRateIndex;
	TUint8 padding;
	TUint8 sync;
	
	data.SetLength(KMP3FrameHeaderSize);
	iReader.ReadBytesL(data);
	
	do
		{
		sync = MP3_GET_SYNC1(data[0]);
		if (IS_BAD_MP3_FRAME_SYNC1(sync))
			{
			break;
			}
			
		sync = MP3_GET_SYNC2(data[1]);
		if (IS_BAD_MP3_FRAME_SYNC2(sync))
			{
			break;
			}
		
		versionIndex = MP3_GET_VERSION(data[1]);
		if (IS_BAD_MP3_VERSION(versionIndex))
			{
			break;
			}
			
		layerIndex = MP3_GET_LAYER(data[1]);
		if (IS_BAD_MP3_LAYER(layerIndex))
			{
			break;
			}
			
		bitrateIndex = MP3_GET_BITRATE(data[2]);
		if (IS_BAD_MP3_BITRATE(bitrateIndex))
			{
			break;
			}
			
		sampleRateIndex = MP3_GET_SAMPLE_RATE(data[2]);
		if (IS_BAD_MP3_SAMPLE_RATE(sampleRateIndex))
			{
			break;
			}
			
		padding = MP3_GET_PADDING(data[2]);
		
		// All the data is valid.
		// Compute the audio data length.
		TUint32 bitRate = KBad;
		TUint16 sampleRate = KBad;
		TUint16 samplesPerFrame = KBad;
		
		if (versionIndex == KMP3Version1)
			{
			bitRate = KBitrateVersion1[layerIndex][bitrateIndex];
			}
		else if (versionIndex == KMP3Version2)
			{
			bitRate = KBitrateVersion2[layerIndex][bitrateIndex];
			}
		else 
			{
			// Version 2.5 is not supported.
			break;
			}

		sampleRate = KSampleRate[versionIndex][sampleRateIndex];
		samplesPerFrame = KSamplesPerFrame[layerIndex][versionIndex];
		
		// Check we have valid values.
		if ((bitRate == KBad) || (sampleRate == KBad) || (samplesPerFrame == KBad))
			{
			break;
			}

		bitRate *= 1000; // Convert to kilobits.
		aFrameLength = (((samplesPerFrame / KBitsPerByte) * bitRate) / sampleRate) + padding;
		return KErrNone;
		}
	while (EFalse);
	
	// No valid frame header was found.
	aFrameLength = 0;
	iReader.SeekL(-KMP3FrameHeaderSize);
	return KErrNotFound;
	}
	



