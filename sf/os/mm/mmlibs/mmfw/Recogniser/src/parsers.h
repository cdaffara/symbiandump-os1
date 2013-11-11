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

#ifndef PARSERS_H
#define PARSERS_H

#include "readers.h"

//
// Utility class for performing bit operations.
//
class TFlags
	{
public:
	TFlags()
	 : iFlags(0)
		{
		}
		
	void SetExtensionFlag(){ iFlags |= KBit0; }
	void SetBit(TUint32 aMask) { iFlags |= aMask; }
	
	TUint32 GetBitField(TUint32 aMask, TUint8 aRightShift = 0)
		{
		return ((iFlags & aMask) >> aRightShift);
		}
		
private:
	TUint32 iFlags;
	};


//
// An ID3 tag parser.
//
class TID3Parser
	{
public:
	static TBool ReadAndSkipID3L(CReader& aReader);
	};
	

//
// A raw-AAC audio file parser.
//
class TAACParser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);

protected:
	TAACParser(CReader& aReader, TFlags& aFlags);
	void ParseL();
	TInt CheckForFrameHeaderL(TInt& aFrameLength);
	
private:
	CReader& iReader;
	TFlags& iFlags;
	};


//
// A raw-MP3 audio file parser.
//
class TMP3Parser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);

protected:
	TMP3Parser(CReader& aReader, TFlags& aFlags);
	void ParseL();
	TInt CheckForFrameHeaderL(TInt& aFrameLength);

private:
	CReader& iReader;
	TFlags& iFlags;
	};


//
// An MPEG4 container file format parser.
//
#define KMPEG4BoxTitleLen		4

class TMPEG4Parser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);

protected:
	TMPEG4Parser(CReader& aReader, TFlags& aFlags);
	const TText8* MatchFileType(const TDesC& aExt);
	static TInt IsCompatibleBrand(TUint32 aBrand, TInt aStartPos = 0);

	// Parsing routines.
	void ParseL();
	void SkipCurrentBoxL();
	void ReadBoxHeaderL();
	void ReadFileTypeL();
	void ReadMovieL();
	void ReadTrackL();
	void ReadTrackHeaderL();
	void ReadMediaL();
	void ReadHandlerL();
			
private:
	// Data members.
	TUint32 iTitle;			// The title of the current box.
	TInt iBrandIndex;
	TBool iIsFinished;
	TInt64 iSize; 			// The current box's size.
	TBool iSizeIn32bit;
	CReader& iReader;
	TFlags& iFlags;
	TBool iVideoAssumed;
	};


//
// An MPEG2 container file format parser.
//
class TMPEG2Parser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);

protected:
	TMPEG2Parser(CReader& aReader, TFlags& aFlags);
	void ParseL();
	TBool NextStartCodeL();
	void ReadPackHeaderL();
	void SkipL();
	const TText8* MatchExtension(const TDesC& aExt);
	
private:
	CReader& iReader;
	TFlags& iFlags;
	};


//
// A Matroska container file format parser.
//
class TMatroskaParser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);

protected:
	TMatroskaParser(CReader& aReader, TFlags& aFlags);
	void ParseL();
	TUint64 ReadDataL(TBool aTurnOffHighestSetBit = EFalse);
	void ReadElementL(TUint64& aElementID, TInt64& aSize);
	TBool ReadSegmentL(TUint64& aNextID, TInt64& aNextSize);
	TBool ReadTrackL(TInt64 aTrackSize);
	const TText8* MatchExtension(const TDesC& aExt);

private:
	CReader& iReader;
	TFlags& iFlags;
	};


//
// A Windows Media ASF file format parser.
//
class TASFParser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);

protected:
	TASFParser(CReader& aReader, TFlags& aFlags);
	void ParseL();
	void ReadObjectL(TDes8& aGUID, TInt64& aSize);
	void ReadGUIDL(TDes8& aGUID);
	const TText8* MatchExtension(const TDesC& aExt, TBool aVideo);
	
private:
	CReader& iReader;
	TFlags& iFlags;
	};
	

//
// A RealMedia file format parser.
//
class TRMParser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);

protected:
	TRMParser(CReader& aReader, TFlags& aFlags);
	void ParseL();
	void ReadChunkHeaderL(TUint32& aObjectId, TUint32& aSize, TBool aFirstChunk = EFalse);
	void MatchExtension(const TDesC& aFileExt);
	TBool ReadMediaPropertiesL();
	
private:
	CReader& iReader;
	TFlags& iFlags;
	};


//
// A RAM file parser.
//
class TRAMParser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);
	};


//
// An SDP file parser.
//
class TSDPParser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);
	};


//
// An XPS file parser.
//
class TXPSParser
	{
public:
	static void DoRecognise(const TDesC& aFileExt, CReader& aReader, TMatch& aMatch);
	};

#endif
