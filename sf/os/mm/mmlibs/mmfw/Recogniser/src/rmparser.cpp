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

static const TUint32 KRMFileHeaderId = MAKE_INT32('.', 'R', 'M', 'F');
static const TUint32 KRMMediaPropertiesId = MAKE_INT32('M', 'D', 'P', 'R');
static const TInt KRMMaxMimeTypeLen = 255;	// Specified by an 8-bit field

#define KRMExtensionBit			KBit0
#define KRMFileHeaderBit		KBit1
#define KRMMediaPropertiesBit	KBit2
#define KRMVideoBit				KBit3

#define KRMVideoMask			0x08	// 0000 1000
#define KRMConfidenceMask 		0x07	// 0000 0111

//
// This truth table maps the following flags to a confidence level.
// -----------------------------------------------------------------
// A: MediaProperties chunk found
// B: FileHeader signature found
// C: Extension recognised.
//
// A B C -> Confidence
// ===================
// 0 0 0 -> ENotRecognized
// 0 0 1 -> EPossible
// 0 1 0 -> EPossible (EPossible beacuse the header is ASCII and may match plaintext)
// 0 1 1 -> ECertain
// 1 0 0 -> ENotRecognised (Can't have MediaProperties without FileHeader)
// 1 0 1 -> ENotRecognised (Can't have MediaProperties without FileHeader)
// 1 1 0 -> EProbable
// 1 1 1 -> ECertain
//
static const TInt KRMFlagsToConfidence[] =
	{
	KConfNotRecognised,
	KConfPossible,
	KConfPossible,
	KConfCertain,
	KConfNotRecognised,
	KConfNotRecognised,
	KConfProbable,
	KConfCertain
	};


//
// Constructor.
//
TRMParser::TRMParser(CReader& aReader, TFlags& aFlags)
 :	iReader(aReader),
	iFlags(aFlags)
	{
	}


//
// The main RealMedia parsing function.
//
void TRMParser::DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch)
	{
	TFlags flags;
	TRMParser parser(aReader, flags);

	parser.MatchExtension(aFileExt);

	TRAP_IGNORE(parser.ParseL());

	TInt confIndex = flags.GetBitField(KRMConfidenceMask);
	aMatch.iConfidence = KRMFlagsToConfidence[confIndex];

	if (aMatch.iConfidence != KConfNotRecognised)
		{
		aMatch.iMime = (flags.GetBitField(KRMVideoMask) ? KMimeRM_V : KMimeRM_A);
		}
	}


//
// Match the file extension given by AppArc against known
// extensions for this format.
//
void TRMParser::MatchExtension(const TDesC& aFileExt)
	{
	TBool match;

	match = (aFileExt.MatchF(TPtrC(KExtRMF)) != KErrNotFound);
	match |= (aFileExt.MatchF(TPtrC(KExtRM)) != KErrNotFound);

	if (match)
		{
		iFlags.SetExtensionFlag();
		}
	}

//
// RealMedia files are divided into chunks. Chunks begins with a 32-bit identifier
// which is followed by a 32-bit unsigned field that specifies the size of the chunk
// in bytes. Since the size field is unsigned, it must be cast to a TInt64 in order
// for SeekL() to work reliably. The size field also includes the length of the chunk
// identifier and the length of the size field itself, so these must be subtracted
// from any seek operation.
//
void TRMParser::ParseL()
	{
	TUint32 objectId;
	TUint32 size;
	const TInt KChunkHeaderSize = sizeof(TUint32) * 2; // sizeof objectId and size fields.

	// The first chunk must be the FileHeader.
	ReadChunkHeaderL(objectId, size, ETrue);

	// Assume there's video content if we only have buffer data.
	if (iReader.Type() == CReader::EBuffer)
		{
		iFlags.SetBit(KRMVideoBit);
		}

	iReader.SeekL(TInt64(size - KChunkHeaderSize));

	// The other chunks can occur in any order.
	TBool complete = EFalse;
	do
		{
		ReadChunkHeaderL(objectId, size);

		if (objectId == KRMMediaPropertiesId)
			{
			iFlags.SetBit(KRMMediaPropertiesBit);
			if (ReadMediaPropertiesL())
				{
				// Exit early if the video flag has been set by ReadMediaPropertiesL.
				complete = ETrue;
				}
			}
		else
			{
			// Skip over the unneeded chunk.
			iReader.SeekL(TInt64(size - KChunkHeaderSize));
			}
		}
	while (!complete);
	}


//
// Reads the chunk identifier and size.
//
void TRMParser::ReadChunkHeaderL(TUint32& aObjectId, TUint32& aSize, TBool aFirstChunk)
	{
	iReader.Read32L(aObjectId);

	if (aFirstChunk)
		{
		if (aObjectId == KRMFileHeaderId)
			{
			iFlags.SetBit(KRMFileHeaderBit);
			}
		else
			{
			User::Leave(KErrCorrupt);
			}
		}

	iReader.Read32L(aSize);
	if (aSize == 0)
		{
		User::Leave(KErrCorrupt);
		}
	}


//
// The MediaProperties chunk describes a stream in the RM file. There
// may be several MediaProperties chunks in one file. In order to determine
// if video content is present we must look at the MIME-type that may
// be present in each MediaProperties chunk.
// Return ETrue if this function set the video bit.
//
TBool TRMParser::ReadMediaPropertiesL()
	{
	const TInt KBytesToFieldSize = 30;
	
	// ObjectId and size fields have already been read.
	TBool setVideo = EFalse;
	TUint16 objectVersion;

	iReader.Read16L(objectVersion);

	if (objectVersion == 0)
		{
		TUint8 fieldSize;
		TBuf8<KRMMaxMimeTypeLen> mimeType;
		_LIT8(KVideoStar, "video/*");

		// Skip unneeded information.
		iReader.SeekL(KBytesToFieldSize);

		// Read stream_name_size so we can skip over it.
		iReader.ReadByteL(fieldSize);
		iReader.SeekL(fieldSize);

		// Read the mime_type_size. Because it's 8-bit, it has a max length of 256 bytes.
		iReader.ReadByteL(fieldSize);

		mimeType.SetLength(fieldSize);
		iReader.ReadBytesL(mimeType);

		// Now check if the mime-type field indicates video content.
		if (mimeType.MatchF(KVideoStar) != KErrNotFound)
			{
			iFlags.SetBit(KRMVideoBit);
			setVideo = ETrue;
			}

		// Read the type_specific_len so we can skip over it.
		TUint32 typeSpecificLen;
		iReader.Read32L(typeSpecificLen);
		iReader.SeekL(TInt64(typeSpecificLen));
		}

	return setVideo;
	}

