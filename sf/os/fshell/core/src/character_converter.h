// character_converter.h
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __CHARACTER_CONVERTER_H__
#define __CHARACTER_CONVERTER_H__

#include <e32std.h>
#include <f32file.h>
#include <charconv.h>


class CCharacterConverter : public CBase
	{
public:
	enum TChunkType
		{
		EFirst,
		EMiddle,
		ELast
		};
public:
	static CCharacterConverter* NewL(TInt aMaxBlockSize, RFs& aFs, TUint aCharset=0); // 0 means autodetect
	~CCharacterConverter();
	const TDesC& ConvertChunkL(const TDesC8& aNarrowBuf, TChunkType aType);
private:
	CCharacterConverter(TInt aMaxBlockSize, RFs& aFs, TUint aCharset);
	void ConstructL();
private:
	RFs& iFs;
	TInt iCharConvState;
	TInt iNumUnconvertibleChars;
	const TInt iMaxBlockSize;
	HBufC8* iNarrowBuf;
	HBufC16* iWideBuf;
	TPtr8 iNarrowPtr;
	TPtr16 iWidePtr;
	CCnvCharacterSetConverter* iConverter;
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* iCharSets;
	TUint iCharset;
	};


#endif // __CHARACTER_CONVERTER_H__
