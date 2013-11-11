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

static const TUint8 KISOEndCode = 0xB9;
static const TUint8 KPackStartCode = 0xBA;
static const TUint8 KVideoElementaryStream = 0xE0;

#define KMPEG1PackHeaderLen		8
#define KMPEG2PackHeaderLen		10

#define KMPEG1PackHeaderID		0x21	// 0010xxx1. See Doc Link 2.
#define KMPEG2PackHeaderID		0x44	// 01xxx1xx. See Doc Link 2.

#define KStartCodeMask			0xffffff00
#define KStartCodeIntro			0x00000100

#define KMPEG2StartCode1Bit	KBit1
#define KMPEG2StartCode2Bit	KBit2
#define KMPEG2VideoBit		KBit3
#define KMPEG2MPEG1Bit		KBit4

//
// Mapping flags to a confidence level.
//
// A: extension match
// B: start-code1
// C: start-code2
//
// C B A -> Confidence
// -------------------
// 0 0 0 -> ENotRecognised
// 0 0 1 -> EPossible
// 0 1 0 -> EPossible
// 0 1 1 -> EProbable
// 1 0 0 -> ENotRecognised
// 1 0 1 -> ENotRecognised
// 1 1 0 -> EProbable
// 1 1 1 -> ECertain
//
static const TInt KMPEG2FlagsToConfidence[] =
	{
	KConfNotRecognised,
	KConfPossible,
	KConfPossible,
	KConfProbable,
	KConfNotRecognised,
	KConfNotRecognised,
	KConfProbable,
	KConfCertain
	};

#define KMPEG2ConfidenceMask	0x07	// 00000111
#define KMPEG2MimeMask	 		0x18	// 00011000
#define KMPEG2MimeShift			0x03	

static const TInt KMPEG2ExtOnlyIndex = 1;
	
//
// The 'ED' bits of the flags member are used as an index into
// this table of possible MIME-types.
//
// E: MPEG1 instead of MPEG2
// D: Video content present
//
// E D -> Mime
// -----------
// 0 0 -> audio/mpeg2
// 0 1 -> video/mpeg2
// 1 0 -> audio/mpeg1
// 1 1 -> video/mpeg1
//
static const TText8* const KMPEG2Mimes[] = 
	{
	KMimeMPEG2_A,
	KMimeMPEG2_V,
	KMimeMPEG1_A,
	KMimeMPEG1_V
	};

//
// A list of known MPEG2 file extensions.
// MPEG1 file extensions are also listed.
//
typedef struct
	{
	const TText* iExt;
	const TText8* iMime;
	}
TMPEG2Types;

static const TMPEG2Types KMPEG2Types[] = 
	{
		{ KExtMPEG_1, KMimeMPEG2_V },
		{ KExtMPEG_2, KMimeMPEG2_V },
		{ KExtMPEG_3, KMimeMPEG2_V },
		{ KExtMPEG_4, KMimeMPEG2_V },
		{ KExtMPEG_5, KMimeMPEG2_V },
		{ KExtMPEG_6, KMimeMPEG2_V },
		{ KExtMPEG_7, KMimeMPEG1_V },
		{ KExtMPEG_8, KMimeMPEG2_A }
	};
	
#define KMPEG2ExtCount sizeof(KMPEG2Types) / sizeof(TMPEG2Types)
	

//
//
//
TMPEG2Parser::TMPEG2Parser(CReader& aReader, TFlags& aFlags)
 :	iReader(aReader),
 	iFlags(aFlags)
	{
	}


//
// Match the file's extension to known MPEG2 file extensions.
//
const TText8* TMPEG2Parser::MatchExtension(const TDesC& aExt)
	{
	if (aExt.Length() > 0)
		{
		for (TInt i = 0; i < KMPEG2ExtCount; i++)
			{
			if (aExt.MatchF(TPtrC(KMPEG2Types[i].iExt)) != KErrNotFound)
				{
				iFlags.SetExtensionFlag();
				return KMPEG2Types[i].iMime;
				}
			}
		}
	
	return NULL;
	}


//
//
//
void TMPEG2Parser::DoRecognise(const TDesC& aExt, CReader& aReader, TMatch& aMatch)
	{
	TFlags flags;
	TMPEG2Parser parser(aReader, flags);
	
	const TText8* extMime = parser.MatchExtension(aExt);
	TRAP_IGNORE(parser.ParseL());
	
	TInt confIndex = flags.GetBitField(KMPEG2ConfidenceMask);
	aMatch.iConfidence = KMPEG2FlagsToConfidence[confIndex];
	if (aMatch.iConfidence != KConfNotRecognised)
		{
		if (confIndex == KMPEG2ExtOnlyIndex)
			{
			// The content is corrupt, but the extension was recognised.
			aMatch.iMime = extMime;
			}
		else
			{
			TInt mimeIndex = flags.GetBitField(KMPEG2MimeMask, KMPEG2MimeShift);
			aMatch.iMime = KMPEG2Mimes[mimeIndex];
			}
		}
	}


//
// Attempts to parse an MPEG2 file by looking for start-codes.
//
void TMPEG2Parser::ParseL()
	{
	TBool finished;
	
	// Assume there's video content if we only have a buffer.
	if (iReader.Type() == CReader::EBuffer)
		{
		iFlags.SetBit(KMPEG2VideoBit);
		}
	
	do
		{
		finished = NextStartCodeL();
		}
	while (!finished);
	}


//
// Skips over the current start-code box.
//
void TMPEG2Parser::SkipL()
	{
	TUint16 size;
	
	iReader.Read16L(size);
	iReader.SeekL((TInt)size);
	}


/*
** Expects an MPEG2 Pack Header at the current location.
** The Pack Header is used to determine between MPEG1 and MPEG2.
*/
void TMPEG2Parser::ReadPackHeaderL()
	{
	TUint8 byte;
	TBuf8<KMPEG2PackHeaderLen> header;	// Size is of whichever is larger.
	
	iReader.ReadByteL(byte);
	
	if ((byte & KMPEG1PackHeaderID) == KMPEG1PackHeaderID)
		{
		iFlags.SetBit(KMPEG2MPEG1Bit);
		header.SetLength(KMPEG1PackHeaderLen - 1);	// We've already read a byte.
		iReader.ReadBytesL(header);
		}
	else if ((byte & KMPEG2PackHeaderID) == KMPEG2PackHeaderID)
		{
		header.SetLength(KMPEG2PackHeaderLen - 1);	// We've already read a byte.
		iReader.ReadBytesL(header);
		
		// The lowest 3 bits of the last byte say how much stuffing is present.
		TInt stuffing = header[8] & 0x07; // 00000111
		if (stuffing)
			{
			iReader.SeekL(stuffing);
			}
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
		
	}

//
// Start codes are bit patterns that do not occur in the video stream.
// The start-code sequence is expected to be at the current CReader position.
//
TBool TMPEG2Parser::NextStartCodeL()
	{
	TUint32 data;

	iReader.Read32L(data);
	
	// Start codes must begin with 0x000001ss, where 'ss' is the start code.
	if ((data & KStartCodeMask) != KStartCodeIntro)
		{
		User::Leave(KErrCorrupt);
		}
		
	if (!iFlags.GetBitField(KMPEG2StartCode1Bit))
		{
		iFlags.SetBit(KMPEG2StartCode1Bit);
		}
	else
		{
		if (!iFlags.GetBitField(KMPEG2StartCode2Bit))
			{
			iFlags.SetBit(KMPEG2StartCode2Bit);
			}
		}
		
	// Try to identify the start code.
	switch (LOW_BYTE(data))
		{
		case KPackStartCode:
			ReadPackHeaderL();
			break;
			
		case KVideoElementaryStream:
			iFlags.SetBit(KMPEG2VideoBit);
			return ETrue;
			
		case KISOEndCode:
			// This code should occur at the end of the file and
			// it cannot be skipped over.
			return ETrue;
			
		default:
			SkipL();
		}
		
	return EFalse;
	}
	
