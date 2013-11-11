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

#include "constants.h"
#include "parsers.h"

// The length of the frame in written directly into the header.
// It is 13 bits in length and spans 3 bytes.

#define AAC_SYNC1_MASK				0xff	// 11111111
#define AAC_SYNC2_MASK				0xf0	// 11110000
#define AAC_FRAME_LENGTH_MASK1		0x03	// 00000011
#define AAC_FRAME_LENGTH_MASK2		0xff	// 11111111
#define AAC_FRAME_LENGTH_MASK3		0xe0	// 11100000
#define AAC_LAYER_MASK				0x06	// 00000110

#define AAC_GET_SYNC1(d)	(d)
#define AAC_GET_SYNC2(d)	((d & AAC_SYNC2_MASK) >> 4)
#define AAC_GET_LAYER(d)	(((d) & AAC_LAYER_MASK) >> 1)
#define AAC_GET_FRAME_LENGTH(a, b, c) \
							(((a) & AAC_FRAME_LENGTH_MASK1) << 11) | \
							(((b) & AAC_FRAME_LENGTH_MASK2) << 03) | \
							(((c) & AAC_FRAME_LENGTH_MASK3) >> 05)
							

#define AAC_IS_BAD_SYNC1(s)			((s) != 0xff)
#define AAC_IS_BAD_SYNC2(s)			((s) != 0x0f)
#define AAC_IS_BAD_LAYER(l)			((l) != 0x00)
#define AAC_IS_BAD_FRAME_LENGTH(l)	((l) < KAACFrameHeaderSize)

//
// The various states the recognition process goes through.
//
typedef enum
	{
	ESearchFrame1,
	ESearchFrame2
	}
TAACState;

//
// This truth table maps the following flags to a confidence level.
//
// A: Extension identified.
// B: Frame1 found.
// C: Frame2 found.
//
// C B A -> Confidence
// ===================
// 0 0 0 -> ENotRecognised
// 0 0 1 -> EPossible
// 0 1 0 -> EProbable
// 0 1 1 -> ECertain
// 1 0 0 -> ENotRecognised (Can't have f2 without f1)
// 1 0 1 -> ENotRecognised (Can't have f2 without f1)
// 1 1 0 -> EProbable
// 1 1 1 -> ECertain
//
static const TInt KAACFlagsToConfidence[8] =
	{
	KConfNotRecognised,
	KConfPossible,
	KConfProbable,
	KConfCertain,
	KConfNotRecognised,
	KConfNotRecognised,
	KConfProbable,
	KConfCertain
	};
	
#define KAACConfidenceMask	0x07	// [0000 0111]
#define KAACFrame1Bit		KBit1
#define KAACFrame2Bit		KBit2



//
// Constructs a TAACParser on the stack.
//
TAACParser::TAACParser(CReader& aReader, TFlags& aFlags)
 :	iReader(aReader),
 	iFlags(aFlags)
	{
	}


//
//
//
void TAACParser::DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch)
	{
	TFlags flags;
	TBool extMatch;
	
	// See if the extension is recognised.
	extMatch = (aFileExt.MatchF(TPtrC(KExtAAC)) != KErrNotFound);
	
	// Try to match a known header.
	if (aReader.Match(TPtrC8(_S8("ADIF*"))))
		{
		aMatch.iConfidence = (extMatch ? KConfCertain : KConfProbable);
		aMatch.iMime = KMimeAAC;
		return;
		}
	
	// No known header so try to parse it.
	// Parsing uses flags to track what has been identified.
	if (extMatch)
		{
		flags.SetExtensionFlag();
		}
	
	TAACParser parser(aReader, flags);
	TRAP_IGNORE(parser.ParseL());
	
	TInt confIndex = flags.GetBitField(KAACConfidenceMask);
	aMatch.iConfidence = KAACFlagsToConfidence[confIndex];
	if (aMatch.iConfidence != KConfNotRecognised)
		{
		aMatch.iMime = KMimeAAC;
		}
	}


//
// Looks for valid AAC frame headers; at the current position
// and immediately after (if one found).
//
void TAACParser::ParseL()
	{
	TInt frameLength;
	TAACState state = ESearchFrame1;
	
	// Check if it's an ADTS (raw) format AAC file.
	// There's no known metadata tag for AAC so the
	// first frame header should be at the start of the buffer.
	
	FOREVER
		{
		TID3Parser::ReadAndSkipID3L(iReader);
		
		TInt err = CheckForFrameHeaderL(frameLength);
		if (err == KErrNotFound)
			{
			return;
			}
			
		switch (state)
			{
			case ESearchFrame1:
				iFlags.SetBit(KAACFrame1Bit);
				state = ESearchFrame2;
				break;
					
			case ESearchFrame2:
				iFlags.SetBit(KAACFrame2Bit);
				return;
			}
		
		// Skip over the audio frame.
		// This should be done after flags have been set.
		iReader.SeekL(frameLength - KAACFrameHeaderSize);
		};
	}


//
// Looks for valid AAC frame header bit patterns.
// If one is not found KErrNotFound is returned and aFrameLength
// remains unchanged. If one is found KErrNone is retured and
// aFrameLength is set to the length of the frame.
//
TInt TAACParser::CheckForFrameHeaderL(TInt& aFrameLength)
	{
	TBuf8<KAACFrameHeaderSize> header;
	
	header.SetLength(KAACFrameHeaderSize);
	iReader.ReadBytesL(header);
	
	do
		{
		if (AAC_IS_BAD_SYNC1(AAC_GET_SYNC1(header[0])))
			{
			break;
			}
			
		if (AAC_IS_BAD_SYNC2(AAC_GET_SYNC2(header[1])))
			{
			break;
			}
			
		if (AAC_IS_BAD_LAYER(AAC_GET_LAYER(header[1])))
			{
			break;
			}
			
		TInt frameLength = AAC_GET_FRAME_LENGTH(header[3], header[4], header[5]);
		if (AAC_IS_BAD_FRAME_LENGTH(frameLength))
			{
			break;
			}
		// All is ok.
		aFrameLength = frameLength;
		return KErrNone;
		}
	while (EFalse);
	
	// No frame header was found.
	aFrameLength = 0;
	iReader.SeekL(-KAACFrameHeaderSize);
	return KErrNotFound;
	}
	

