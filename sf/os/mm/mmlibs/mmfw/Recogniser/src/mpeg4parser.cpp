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
#include "constants.h"

//
// Brands.
// Some (most) brands have Release information - e.g. 3gp6, 3gp5 etc.
// Therefore, to match the brand, we only look at the characters that don't
// represent Release information (in the above case, the 4th character).
// The Release character is set to zero.
//
static const TUint32 KMP4Brand = MAKE_INT32('m', 'p', '4', 0);
static const TUint32 K3GPBrand = MAKE_INT32('3', 'g', 'p', 0);
static const TUint32 K3G2Brand = MAKE_INT32('3', 'g', '2', 0);
static const TUint32 K3GSBrand = MAKE_INT32('3', 'g', 's', 0);	// Streaming servers.
static const TUint32 K3GRBrand = MAKE_INT32('3', 'g', 'r', 0);	// Progresive download and MMS.
static const TUint32 KQTBrand  = MAKE_INT32('q', 't', ' ', ' '); // for quicktime
//
// Box identifiers.
//
static const TUint32 KFtyp = MAKE_INT32('f', 't', 'y', 'p');
static const TUint32 KMoov = MAKE_INT32('m', 'o', 'o', 'v');
static const TUint32 KTrak = MAKE_INT32('t', 'r', 'a', 'k');
static const TUint32 KTkhd = MAKE_INT32('t', 'k', 'h', 'd');
static const TUint32 KMdia = MAKE_INT32('m', 'd', 'i', 'a');
static const TUint32 KHdlr = MAKE_INT32('h', 'd', 'l', 'r');
static const TUint32 KVide = MAKE_INT32('v', 'i', 'd', 'e');
static const TUint32 KUuid = MAKE_INT32('u', 'u', 'i', 'd');


//
// This truth table maps the following flags to a confidence level.
//
// A - trak box present
// B - moov box present
//
// A B -> Confidence
// =================
// 0 0 -> EPossible
// 0 1 -> EProbable
// 1 0 -> EProbable
// 1 1 -> ECertain
//
static const TInt KMPEG4FlagsToConfidence[] =
	{
	KConfPossible,
	KConfProbable,
	KConfProbable,
	KConfCertain,
	};


#define KMPEG4ConfidenceMask	0x03	// 00000011
#define KMPEG4BoxTitleLen		4
#define KMPEG4TRAKBit			KBit0	// 00000001
#define KMPEG4MOOVBit			KBit1	// 00000010
#define KMPEG4VideoBit			KBit2	// 00000100

static const TInt KMPEG4BoxIntroLen = 8;
static const TInt KMPEG4Box64BitIntroLen = 16;

//
// In order to find out the type of MPEG4 file it is
// we need to be able to map known extensions, expected
// ftyp expressions with MIME-types.
//
typedef struct
	{
	const TText* iExt;
	TUint32 iBrand;
	const TText8* iAudioMime;
	const TText8* iVideoMime;
	}
TMPEG4File;


//
// A table of ftyp's, extensions and mime-types.
//
// .mp4 - can contain either audio or video.
// .m4a - should contain (unprotected) audio only.
// .m4p - should contain protected audio only.
// .3gp - can contain either audio or video.
//
static const TMPEG4File KMPEG4Files[] =
	{
		{KExtMP4,	KMP4Brand,	KMimeMP4_A,	KMimeMP4_V},
		{KExt3GP,	K3GPBrand,	KMime3GP_A,	KMime3GP_V},
		{KExtM4A,	KMP4Brand,	KMimeMP4_A,	NULL},
		{KExt3G2,	K3G2Brand,	KMime3G2_A,	KMime3G2_V},
		{KExt3GP,	K3GSBrand,	KMime3GP_A,	KMime3GP_V},
		{KExt3GP,	K3GRBrand,	KMime3GP_A,	KMime3GP_V},
		{KExt3GA,	K3GPBrand,	KMime3GA,	NULL},
		{KExtMOV,   KQTBrand,   NULL, KMimeQuickV} // this entry is for .mov files
	};

static const TInt KMPEG4FileTypeCount = sizeof(KMPEG4Files) / sizeof(TMPEG4File);


//
//
//
TMPEG4Parser::TMPEG4Parser(CReader& aReader, TFlags& aFlags)
 :	iBrandIndex(KErrNotFound),
 	iIsFinished(EFalse),
 	iReader(aReader),
 	iFlags(aFlags),
 	iVideoAssumed(EFalse)
	{
	}


//
// Compare a brand with the ones this recogniser knows about.
//
TInt TMPEG4Parser::IsCompatibleBrand(TUint32 aBrand, TInt aStartPos)
	{
	for (TInt i = aStartPos; i < KMPEG4FileTypeCount; i++)
		{
		TUint32 knownBrand = KMPEG4Files[i].iBrand;
		if ((aBrand & knownBrand) == knownBrand)
			{
			return i;
			}
		}

	return KErrNotFound;
	}


//
// Try to determine the mime-type from the extension, and
// if that isn't matched, from the FTYP field if present.
// If none of these are matched, NULL is returned and the
// file isn't a valid MPEG4 file.
//
const TText8* TMPEG4Parser::MatchFileType(const TDesC& aExt)
	{
	TBool videoFound = iFlags.GetBitField(KMPEG4VideoBit);
	TBool video = (videoFound || iVideoAssumed);
	
	// Try to match the extension.
	if (aExt.Length() > 0)
		{
		for (TInt i = 0; i < KMPEG4FileTypeCount; i++)
			{
			TPtrC ext(KMPEG4Files[i].iExt);
			if (aExt.MatchF(ext) != KErrNotFound)
				{
				// Extension match. If the extension is for an audio-only format
				// we must make sure there is no video content in the file. If
				// video content is present then ignore the extension match.
				if (KMPEG4Files[i].iVideoMime == NULL)
					{
					if (videoFound)
						{
						// Try to match another extension.
						continue;
						}
					
					return KMPEG4Files[i].iAudioMime;
					}
				
				return (video ? KMPEG4Files[i].iVideoMime : KMPEG4Files[i].iAudioMime);
				}
			}
		}

	// Unknown or no extension. Try to match the brand
	while (iBrandIndex != KErrNotFound)
		{
		if (KMPEG4Files[iBrandIndex].iVideoMime == NULL)
			{
			if (videoFound)
				{
				// Try to match another brand.
				TUint32 brand = KMPEG4Files[iBrandIndex].iBrand;
				iBrandIndex = TMPEG4Parser::IsCompatibleBrand(brand, iBrandIndex + 1);
				continue;
				}
			
			return KMPEG4Files[iBrandIndex].iAudioMime;
			}
		
		return (video ? KMPEG4Files[iBrandIndex].iVideoMime : KMPEG4Files[iBrandIndex].iAudioMime);
		}

	// If there is no brand and an unknown extension look at the flags.
	// (There are some files that have no ftyp).
	// Only return a potential mime-type if all flag bits have been set.
	if (iFlags.GetBitField(KMPEG4ConfidenceMask) == KMPEG4ConfidenceMask)
		{
		return (video ? KMimeMP4_V : KMimeMP4_A);
		}
		
	return NULL;
	}


//
//
//
void TMPEG4Parser::DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch)
	{
	TFlags flags;
	TMPEG4Parser parser(aReader, flags);
			
	TRAP_IGNORE(parser.ParseL());
	
	// The extension determines the mime-type.
	// The flags say if it's a valid MPEG4 file and if video is present.
	const TText8* extMime = parser.MatchFileType(aFileExt);
	if (extMime != NULL)
		{
		TInt confIndex = flags.GetBitField(KMPEG4ConfidenceMask);
		aMatch.iConfidence = KMPEG4FlagsToConfidence[confIndex];
		if (aMatch.iConfidence != KConfNotRecognised)
			{
			aMatch.iMime = extMime;
			}
		}
	}
	

//
//
//
void TMPEG4Parser::ParseL()
	{
	// If we have only buffer data, we must assume the video
	// content (if any) has been missed. This is because an
	// audio-only file recognised as video should play in a
	// video application, but a video file recognised as audio
	// will not play in a audio application.
	if (iReader.Type() == CReader::EBuffer)
		{
		iVideoAssumed = ETrue;
		}
		
	do
		{		
		ReadBoxHeaderL();
		if (iTitle == KFtyp)
			{
			ReadFileTypeL();
			}
		else if (iTitle == KMoov)
			{
			iFlags.SetBit(KMPEG4MOOVBit);
			ReadMovieL();
			}
		else
			{
			SkipCurrentBoxL();
			}
		}
	while (!iIsFinished);
	}


//
//
//
void TMPEG4Parser::SkipCurrentBoxL()
	{
	// Intro: [size][title] = 8 bytes.
		
	if (iSize == 0)
		{
		// The current box extends to the end of file.
		iIsFinished = ETrue;
		return;
		}
	if(iSizeIn32bit)
	    {
        iReader.SeekL(iSize - KMPEG4BoxIntroLen);
	    }
	else
	    {
        iReader.SeekL(iSize - KMPEG4Box64BitIntroLen);
	    }
	}


//
// Parses the 'moov' box.
// This box contains sub-boxes we're interested in.
//
void TMPEG4Parser::ReadMovieL()
	{
	// This box holds no information.
	// It contains 'trak' boxes, which we want.
	
	TInt64 dataInBox = iSize - KMPEG4BoxIntroLen;
	
	while ((dataInBox > 0) && !iIsFinished)
		{
		ReadBoxHeaderL();
		dataInBox -= iSize;
		
		if (iTitle == KTrak)
			{
			iFlags.SetBit(KMPEG4TRAKBit);
			ReadTrackL();
			}
		else
			{
			SkipCurrentBoxL();
			}
		}
	}
	

//
// Parses the 'trak' box.
// This box contains sub-boxes we're interested in.
//
void TMPEG4Parser::ReadTrackL()
	{
	// This box holds no information.
	// It contains 'tkhd' boxes, which we want.
	
	TInt64 dataInBox = iSize - KMPEG4BoxIntroLen;
	
	while ((dataInBox > 0) && !iIsFinished) 
		{
		ReadBoxHeaderL();
		dataInBox -= iSize;
		
		if (iTitle == KTkhd)
			{
			ReadTrackHeaderL();
			}
		else if (iTitle == KMdia)
			{
			ReadMediaL();
			}
		else
			{
			SkipCurrentBoxL();
			}
		}
	}

//
// Parses a 'mdia' box.
// This box contains sub-boxes we're interested in.
//
void TMPEG4Parser::ReadMediaL()
	{
	TInt64 dataInBox = iSize - KMPEG4BoxIntroLen;
	
	while ((dataInBox > 0) && !iIsFinished)
		{
		ReadBoxHeaderL();
		dataInBox -= iSize;
		
		if (iTitle == KHdlr)
			{
			ReadHandlerL();
			}
		else
			{
			SkipCurrentBoxL();
			}
		}
	}


//
// Parses a 'hdlr' box.
// This is a stand-alone box.
//
void TMPEG4Parser::ReadHandlerL()
	{
	// Intro: [size][title][versionFlags][predefined][handler_type] = 20 bytes.
	const TInt KMPEG4HandlerIntroLen = 20;
	TUint32 versionFlags;
	TUint32 predefined;
	TUint32 handler;

	iReader.Read32L(versionFlags);
	iReader.Read32L(predefined);
	if (predefined != 0)
		{
		User::Leave(KErrCorrupt);
		}
		
	iReader.Read32L(handler);
	if (handler == KVide)
		{
		iFlags.SetBit(KMPEG4VideoBit);
		}
		
	iReader.SeekL(iSize - KMPEG4HandlerIntroLen);
	}


//
//
//
void TMPEG4Parser::ReadTrackHeaderL()
	{
	const TUint8 KMPEG4TrackVersion0 = 0;
	const TUint8 KMPEG4TrackVersion1 = 1;
	const TInt KMPEG4Version0ToVolume = 32; // Distance to volume field from version=0 field.
	const TInt KMPEG4Version1ToVolume = 44; // Distance to volume field from version=1 field.
	const TInt KMPEG4VolumeToWidth = 38;	// Distance to width field from volume field.

	TUint32 versionFlags;
	TUint16 volume;
	TUint32 width;
	TUint32 height;
	
	// This box contains information about a single track.
	iReader.Read32L(versionFlags);
	
	// The highest 8 bits contains the version.
	switch (HIGH_BYTE32(versionFlags))
		{
		case KMPEG4TrackVersion0:
			iReader.SeekL(KMPEG4Version0ToVolume);
			break;
			
		case KMPEG4TrackVersion1:
			iReader.SeekL(KMPEG4Version1ToVolume);
			break;
			
		default:
			User::Leave(KErrCorrupt);
		}
	
	// Volume can not be used to distinguish between audio and video anymore.
	iReader.Read16L(volume);
		
	// We want to seek ahead to read the 'width' and 'height' fields.
	iReader.SeekL(KMPEG4VolumeToWidth);

	iReader.Read32L(width);		// 16.16 fixed-point
	iReader.Read32L(height);	// 16.16 fixed-point
	if ((width != 0) && (height != 0))
		{
		iFlags.SetBit(KMPEG4VideoBit);
		}
	}

	
//
// Parses the 'ftyp' box.
// Records the first recognised brand that helps to
// identify the mime-type.
//
void TMPEG4Parser::ReadFileTypeL()
	{
	// Intro = [size][title][majorBrand] = 12 bytes.
	const TInt KMPEG4FtypIntroLen = 12;
	TUint32 brand;
	
	// If the majorBrand isn't recognised we should also
	// search the compatible brand list.
	TInt64 bytesRemaining = iSize - KMPEG4FtypIntroLen;
	//here there should be bytes remaining. Otherwise we cant read.
	if( bytesRemaining <0 )
	{
	    User::Leave(KErrCorrupt);    
	}
	
	iReader.Read32L(brand);
	iBrandIndex = TMPEG4Parser::IsCompatibleBrand(brand);
	if (iBrandIndex != KErrNotFound)
		{
		// The major brand was recognised.
		// Skip to the end of the ftyp box.
		iReader.SeekL(bytesRemaining);
		return;
		}
		
	// The major brand wasn't recognised.
	// Skip over the version info (32 bit) to the start of the
	// compatible brands list.
	TInt skip = sizeof(TUint32);
	iReader.SeekL(skip);
	bytesRemaining -= skip;
	
	while ((iBrandIndex == KErrNotFound) && (bytesRemaining > 0))
		{
		iReader.Read32L(brand);
		bytesRemaining -= skip;
		iBrandIndex = TMPEG4Parser::IsCompatibleBrand(brand);
		}

	iReader.SeekL(bytesRemaining);
	}


//
// Reads the first few bytes of a box.
// The exact amount read depends on the box.
//
void TMPEG4Parser::ReadBoxHeaderL()
	{
	const TInt KMPEG4ExtendedTypeLen = 16;
	
	TUint32 word1;

	iReader.Read32L(word1);
	iReader.Read32L(iTitle);

	switch (word1)
		{
		case 0:
			// Box extends to the end of file.
			iSize = MAKE_TINT64(0, 0);
			break;
			
		case 1:
			// Size is specified in a 64-bit field.
		    iSizeIn32bit = EFalse;
			iReader.Read64L(iSize);
			break;
			
		default:
			// It's an actual 32-bit size.
		    iSizeIn32bit = ETrue;
			iSize = MAKE_TINT64(0, word1);
		}
		
	if (iTitle == KUuid)
		{
		// Skip the extended type.
		iReader.SeekL(KMPEG4ExtendedTypeLen);
		}
	}


