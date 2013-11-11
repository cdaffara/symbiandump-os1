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

#define KASFAudioMedia				"\xF8\x69\x9E\x40\x5B\x4D\x11\xCF\xA8\xFD\x00\x80\x5F\x5C\x44\x2B"
#define KASFVideoMedia				"\xBC\x19\xEF\xC0\x5B\x4D\x11\xCF\xA8\xFD\x00\x80\x5F\x5C\x44\x2B"
#define KASFHeaderObject 			"\x75\xB2\x26\x30\x66\x8E\x11\xCF\xA6\xD9\x00\xAA\x00\x62\xCE\x6C"
#define KASFStreamPropertiesObject	"\xB7\xDC\x07\x91\xA9\xB7\x11\xCF\x8E\xE6\x00\xC0\x0C\x20\x53\x65"
#define KASFCodecListObject			"\x86\xD1\x52\x40\x31\x1D\x11\xD0\xA3\xA4\x00\xA0\xC9\x03\x48\xF6"


static const TInt KGUIDLen = 16; 				// 128-bit
static const TInt KObjectLen = KGUIDLen + 8; 	// GUID followed by 64-bit size.
static const TInt KMinObjectLen = 30; 			// From documentation


typedef struct
	{
	const TText* iExt;
	const TText8* iVideoMime;
	const TText8* iAudioMime;
	}
TASFType;


//
// Various ASF container MIME-types.
//
static const TASFType KASFTypes[] =
	{
		{KExtWMA,	KMimeWMA,	KMimeWMA},
		{KExtWMV,	KMimeWMV,	KMimeWMV},
		{KExtASF,	KMimeASF_V,	KMimeASF_A}
	};

#define KASFTypesCount	sizeof(KASFTypes) / sizeof(TASFType)


#define KASFHeaderObjectBit		KBit1	// 00000001
#define KASFStreamHeaderBit		KBit2	// 00000010
#define KASFVideoBit			KBit3	// 00000100
#define KASFConfidenceMask		0x07	// 00000111

//
// Flags mapped to confidence levels.
//
// A: Extension identified.
// B: HeaderObject GUID
// C: StreamProperties GUID
//
// C B A -> Confidence
// -------------------
// 0 0 0 -> ENotRecognised
// 0 0 1 -> EPossible
// 0 1 0 -> EPossible
// 0 1 1 -> EProbable
// 1 0 0 -> ENotRecognised (StreamProperties occurs within HeaderObject)
// 1 0 1 -> ENotRecognised (StreamProperties occurs within HeaderObject)
// 1 1 0 -> EProbable
// 1 1 1 -> ECertain
//
static const TInt KASFFlagsToConfidence[8] = 
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


//
//
//
TASFParser::TASFParser(CReader& aReader, TFlags& aFlags)
 :	iReader(aReader),
 	iFlags(aFlags)
	{
	}


//
// Sets the mime-type the file extension implies.
//
const TText8* TASFParser::MatchExtension(const TDesC& aExt, TBool aVideo)
	{
	for (TInt i = 0; i < KASFTypesCount; i++)
		{
		if (aExt.MatchF(TPtrC(KASFTypes[i].iExt)) != KErrNotFound)
			{
			return (aVideo ? KASFTypes[i].iVideoMime : KASFTypes[i].iAudioMime);
			}
		}
		
	return NULL;
	}


//
//
//
void TASFParser::DoRecognise(const TDesC& aExt, CReader& aReader, TMatch& aMatch)
	{
	TFlags flags;
	TASFParser parser(aReader, flags);
	
	// We need to parse first to determine if there's video content present.
	TRAPD(err, parser.ParseL());
	if (err == KErrCorrupt)
		{
		// Unrecognised content. However the extension may allow
		// correct identification so assume there's video content.
		flags.SetBit(KASFVideoBit);
		}
		
	const TText8* extMime = parser.MatchExtension(aExt, flags.GetBitField(KASFVideoBit));
	if (extMime != NULL)
		{
		// The extension was recognised.
		flags.SetExtensionFlag();
		}
		
	TInt confIndex = flags.GetBitField(KASFConfidenceMask);
	aMatch.iConfidence = KASFFlagsToConfidence[confIndex];
	if (aMatch.iConfidence != KConfNotRecognised)
		{
		// Trust the mime-type the extension maps to.
		// If the extension wasn't recognised, but the content was
		// then return the generic ASF mime type. ASF format files
		// can't be identified from their content; just whether they
		// contain video or not.
		aMatch.iMime = extMime;
		if (aMatch.iMime == NULL)
			{
			aMatch.iMime = (flags.GetBitField(KASFVideoBit) ? KMimeASF_V : KMimeASF_A);
			}
		}
	}


//
//
//
void TASFParser::ParseL()
	{
	// ASF files are logically composed of three types of top-level objects:
	// the Header Object, the Data Object, and the Index Object(s).
	// The Header Object is mandatory and must be placed at the beginning of every
	// ASF file. The Data Object is also mandatory and must follow the Header Object.
	// The Index Object(s) are optional, but they are useful in providing time-based
	// random access into ASF files. When present, the Index Object(s) must be the
	// last object in the ASF file. 
	
	TBuf8<KGUIDLen> guid;
	TInt64 size;
	const TBool useLittleEndian = ETrue;
	
	// Assume there's video content present if we only have buffer data.
	if (iReader.Type() == CReader::EBuffer)
		{
		iFlags.SetBit(KASFVideoBit);
		}
		
	ReadObjectL(guid, size);
	if (guid == MAKE_TPtrC8(KASFHeaderObject))
		{
		TUint32 objectCount;
		
		if (size < KMinObjectLen)
			{
			User::Leave(KErrCorrupt);
			}
			
		iFlags.SetBit(KASFHeaderObjectBit);
		// We need to find out how many objects there are.
		iReader.Read32L(objectCount, useLittleEndian);
		iReader.SeekL(2); // Ignore reserved values (two bytes).
		
		const TDesC8& streamPropertiesGUID = MAKE_TPtrC8(KASFStreamPropertiesObject);
		const TDesC8& videoMediaGUID = MAKE_TPtrC8(KASFVideoMedia);
		
		for (TInt i = 0; i < objectCount; i++)
			{
			ReadObjectL(guid, size);
			
			// We want the stream properties object.
			if (guid == streamPropertiesGUID)
				{
				// There may be more than one present.
				iFlags.SetBit(KASFStreamHeaderBit);
				
				ReadGUIDL(guid);
				if (guid == videoMediaGUID)
					{
					iFlags.SetBit(KASFVideoBit);
					}
				iReader.SeekL(size - KObjectLen - KGUIDLen);
				}
			else
				{
				iReader.SeekL(size - KObjectLen);
				}
			}
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}
	
//
//
//
void TASFParser::ReadObjectL(TDes8& aGUID, TInt64& aSize)
	{
	//The base unit of organization for ASF files is called the ASF object.
	//It consists of a 128-bit GUID for the object, a 64-bit integer object size,
	//and the variable-length object data. The value of the object size field is
	//the sum of 24 bytes plus the size of the object data in bytes.
	
	TUint32 word1;
	TUint32 word2;
	const TBool useLittleEndian = ETrue;
	
	aGUID.SetLength(KGUIDLen);
	ReadGUIDL(aGUID);
	
	iReader.Read32L(word2, useLittleEndian);
	iReader.Read32L(word1, useLittleEndian);
	
	aSize = MAKE_TINT64(word1, word2);
	}


//
//
//
void TASFParser::ReadGUIDL(TDes8& aGUID)
	{
	TUint8 byte;
	
	if (aGUID.Length() != KGUIDLen)
		{
		User::Leave(KErrUnderflow);
		}
	
	// Parts of the GUID are stored in big-endian order.
	// They're converted to little-endian order here.
	iReader.ReadByteL(byte);	aGUID[3] = byte;
	iReader.ReadByteL(byte);	aGUID[2] = byte;
	iReader.ReadByteL(byte);	aGUID[1] = byte;
	iReader.ReadByteL(byte);	aGUID[0] = byte;
	
	iReader.ReadByteL(byte);	aGUID[5] = byte;
	iReader.ReadByteL(byte);	aGUID[4] = byte;
	
	iReader.ReadByteL(byte);	aGUID[7] = byte;
	iReader.ReadByteL(byte);	aGUID[6] = byte;
	
	for (TInt i = 8; i < KGUIDLen; i++)
		{
		iReader.ReadByteL(byte);
		aGUID[i] = byte; 
		}
	}

