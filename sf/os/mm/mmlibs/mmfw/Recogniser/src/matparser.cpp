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

//
// Matroska 'XML' tags.
//
static const TUint32 KEBML = 0x1A45DFA3;
static const TUint32 KSegment = 0x18538067;
static const TUint32 KTracks = 0x1654AE6B;
static const TUint32 KTrackEntry = 0xAE;
static const TUint32 KTrackType = 0x83;
static const TUint32 KVoid = 0xEC;
static const TUint32 KSeekHead = 0x114D9B74;
static const TUint32 KSegmentInfo = 0x1549A966;
static const TUint32 KCluster = 0x1F43B675;
static const TUint32 KCues = 0x1C53BB6B;
static const TUint32 KAttachments = 0x1941A469;
static const TUint32 KChapters = 0x1043A770;
static const TUint32 KTags = 0x1254C367;

//
// Matroska Track Types.
//
static const TUint8 KTrackTypeVideo = 0x01;

#define KMatroskaConfidenceMask	0x07	// 00000111
#define KMatroskaEBMLBit		KBit1
#define KMatroskaSegmentBit		KBit2
#define KMatroskaVideoBit		KBit3


//
// This truth table maps the following flags to confidence levels.
//
// A: Extension match.
// B: EBML tag found.
// C: Segment tag found.
//
// C B A -> Confidence
// --------------------
// 0 0 0 -> NotRecognised
// 0 0 1 -> EPossible
// 0 1 0 -> EPossible
// 0 1 1 -> EProbable
// 1 0 0 -> ENotRecognised (EBML should be present)
// 1 0 1 -> ENotRecognised (EBML should be present)
// 1 1 0 -> EProbable
// 1 1 1 -> ECertain
//
static const TInt KMatroskaFlagsToConfidence[8] =
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
	
static const TInt KMatExtensionOnlyIndex = 1; // See truth table.


typedef struct
	{
	const TText* iExt;
	const TText8* iMime;
	}
TMatroskaExt;

//
// Known Matroska extensions and their corresponding MIME-types.
//
static const TMatroskaExt KMatroskaExt[] =
	{
		{KExtMAT_A, KMimeMAT_A },
		{KExtMAT_V, KMimeMAT_V }
	};

static const TInt KMatroskaExtCount = sizeof(KMatroskaExt) / sizeof(TMatroskaExt);


//
//
//
TMatroskaParser::TMatroskaParser(CReader& aReader, TFlags& aFlags)
 :	iReader(aReader),
 	iFlags(aFlags)
	{
	}


//
//
//
const TText8* TMatroskaParser::MatchExtension(const TDesC& aExt)
	{
	for (TInt i = 0; i < KMatroskaExtCount; i++)
		{
		if (aExt.MatchF(TPtrC(KMatroskaExt[i].iExt)) != KErrNotFound)
			{
			return KMatroskaExt[i].iMime;
			}
		}
	
	return NULL;
	}


//
// This function calls the parser and turns the results
// into a MIME-type.
//
void TMatroskaParser::DoRecognise(const TDesC& aExt, CReader& aReader, TMatch& aMatch)
	{
	TFlags flags;
	TMatroskaParser parser(aReader, flags);
	
	// Try to match the extension.
	const TText8* extMime = parser.MatchExtension(aExt);
	if (extMime != NULL)
		{
		flags.SetExtensionFlag();
		}

	// Try to parse the content.
	TRAP_IGNORE(parser.ParseL());
	TInt confIndex = flags.GetBitField(KMatroskaConfidenceMask);
	aMatch.iConfidence = KMatroskaFlagsToConfidence[confIndex];
	if (aMatch.iConfidence != KConfNotRecognised)
		{
		// If any header data has been recognised trust that,
		// otherwise go with the extension.
		if (confIndex == KMatExtensionOnlyIndex)
			{
			aMatch.iMime = extMime;
			}
		else
			{
			aMatch.iMime = (flags.GetBitField(KMatroskaVideoBit) ? KMimeMAT_V : KMimeMAT_A);
			}
		}
	}


//
// This function does the parsing.
//
void TMatroskaParser::ParseL()
	{
	TUint64 id;
	TInt64 size;
	TBool haveElement = EFalse;
	
	// Assume there's video content if we only have buffer data.
	if (iReader.Type() == CReader::EBuffer)
		{
		iFlags.SetBit(KMatroskaVideoBit);
		}
		
	FOREVER
		{
		if (!haveElement)
			{
			ReadElementL(id, size);
			}
		
		switch (id)
			{
			case KEBML:
				iFlags.SetBit(KMatroskaEBMLBit);
				break;
				
			case KSegment:
				haveElement = ReadSegmentL(id, size);
				break;
				
			default:
				// Skip it.
				iReader.SeekL(size);
			}
		}
	}


//
// This function returns ETrue if the aNextID and aNextSize parameters
// contain valid values, EFalse otherwise.
//
TBool TMatroskaParser::ReadSegmentL(TUint64& aNextID, TInt64& aNextSize)
	{
	TUint64 id;
	TInt64 size;
	TBool videoContent = EFalse;
	
	aNextID = 0;
	aNextSize = 0;
	
	iFlags.SetBit(KMatroskaSegmentBit);
	
	while (!videoContent)
		{
		ReadElementL(id, size);
		
		switch (id)
			{
			case KTracks:
				videoContent = ReadTrackL(size);
				break;
			
			case KSeekHead:
			case KSegmentInfo:
			case KCluster:
			case KCues:
			case KAttachments: 
			case KChapters:
			case KTags:
				iReader.SeekL(size);
				break;
				
			default:
				// Unrecognised element id. Pass it back to the caller
				aNextID = id;
				aNextSize = size;
				return ETrue;
			}
		}
	
	// Tell the caller they must read the next element themselves.
	return EFalse;
	}


//
// The Track. This lets us know if there's video content present.
//
TBool TMatroskaParser::ReadTrackL(TInt64 aTrackSize)
	{
	TUint64 id;
	TInt64 size;
	TInt startPos = iReader.Position();
	
	while (iReader.Position() - startPos < aTrackSize)
		{
		ReadElementL(id, size);

		switch (id)
			{
			case KTrackEntry:
				break;
				
			case KTrackType:
				TUint8 trackType;
				iReader.ReadByteL(trackType);
				if (trackType == KTrackTypeVideo)
					{
					// We found video content so we can stop parsing.
					iFlags.SetBit(KMatroskaVideoBit);
					return ETrue;
					}
				break;
				
			default:
				iReader.SeekL(size);
			}
		}
		
	return EFalse;
	}


//
//
//
void TMatroskaParser::ReadElementL(TUint64& aElementID, TInt64& aSize)
	{
	do
		{
		aElementID = ReadDataL();
		aSize = ReadDataL(ETrue);

		// Void elements are used for padding and
		// can be ignored.
		if (aElementID == KVoid)
			{
			iReader.SeekL(aSize);
			}
		}
	while (aElementID == KVoid);
	}


//
//
//
TUint64 TMatroskaParser::ReadDataL(TBool aTurnOffHighestSetBit)
	{
	TUint64 retval;
	TUint8 byte;
	TUint8 mask = 0x80; // [1000 0000]. It will be shifted right 1 in each 'i' iteration.
	TUint8 size = 1;	// It will be incremented in each 'i' iteration.
	
	iReader.ReadByteL(byte);
	
	for (TInt i = 0; i < sizeof(TUint64); i++)
		{
		if (byte & mask)
			{
			retval = byte;
			if (aTurnOffHighestSetBit)
				{
				retval &= ~mask; // Turn off the highest set bit.
				}
			
			// Now read the real data.
			// Start from 1 because we've already read a byte.
			for (TInt j = 1; j < size; j++)
				{
				iReader.ReadByteL(byte);
				retval <<= 8;
				retval |= byte;
				}
				
			return retval;
			}
		else
			{
			mask >>= 1;
			size++;
			}
		}

	User::Leave(KErrCorrupt);
	return 0; // Keep the compiler happy.
	}

