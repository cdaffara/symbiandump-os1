// character_converter.cpp
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


#include "character_converter.h"

const TInt KSlack = 20;


CCharacterConverter* CCharacterConverter::NewL(TInt aMaxBlockSize, RFs& aFs, TUint aCharset)
	{
	CCharacterConverter* self = new(ELeave) CCharacterConverter(aMaxBlockSize, aFs, aCharset);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCharacterConverter::~CCharacterConverter()
	{
	delete iCharSets;
	delete iConverter;
	delete iNarrowBuf;
	delete iWideBuf;
	}

const TDesC& CCharacterConverter::ConvertChunkL(const TDesC8& aNarrowBuf, TChunkType aType)
	{
	ASSERT(aNarrowBuf.Length() <= iMaxBlockSize);

	switch (aType)
		{
		case EFirst:
			{
			iCharConvState = CCnvCharacterSetConverter::KStateDefault;
			iNarrowPtr = aNarrowBuf;
			if (iCharset == 0)
				{
				// Only spend time autodetecting if we haven't been told the charset to use
				const TUint16* p = (const TUint16*)aNarrowBuf.Ptr();
				if ((*p == 0xFEFF) || (*p == 0xFFFE))
					{
					iNarrowPtr.Delete(0, 2);
					iCharset = (*p == 0xFEFF) ? KCharacterSetIdentifierUnicodeLittle : KCharacterSetIdentifierUnicodeBig;
					}
				else
					{
					iCharSets = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL(iFs);
					TInt confidenceLevel;
					CCnvCharacterSetConverter::AutoDetectCharacterSetL(confidenceLevel, iCharset, *iCharSets, iNarrowPtr.Left(32));
					}
				}
			iConverter->PrepareToConvertToOrFromL(iCharset, iFs);
			}	// Deliberate fall through.
		case EMiddle:
			{
			if (aType == EMiddle)
				{
				iNarrowPtr += aNarrowBuf;
				}
			TInt numUnconvertibleChars;
			const TInt ret = iConverter->ConvertToUnicode(iWidePtr,	iNarrowPtr,	iCharConvState, numUnconvertibleChars);
			iNumUnconvertibleChars += numUnconvertibleChars;
			if (ret < 0)
				{
				User::Leave(ret);
				}
			else if (ret > KSlack)
				{
				User::Leave(KErrCorrupt);
				}
			else if (ret > 0)
				{
				iNarrowPtr = iNarrowPtr.Right(ret);
				}
			else
				{
				iNarrowPtr.Zero();
				}
			break;
			}
		case ELast:
			{
			iNarrowPtr += aNarrowBuf;
			TInt numUnconvertibleChars;
			const TInt ret = iConverter->ConvertToUnicode(iWidePtr,	iNarrowPtr,	iCharConvState, numUnconvertibleChars);
			iNumUnconvertibleChars += numUnconvertibleChars;
			if (ret < 0)
				{
				User::Leave(ret);
				}
			else if (ret > 0)
				{
				User::Leave(KErrCorrupt);
				}
			iNarrowPtr.Zero();
			break;
			}
		default:
			{
			ASSERT(EFalse);
			}
		}

	return iWidePtr;
	}

CCharacterConverter::CCharacterConverter(TInt aMaxBlockSize, RFs& aFs, TUint aCharset)
	: iFs(aFs), iMaxBlockSize(aMaxBlockSize), iNarrowPtr(NULL, 0), iWidePtr(NULL, 0), iCharset(aCharset)
	{
	}

void CCharacterConverter::ConstructL()
	{
	iNarrowBuf = HBufC8::NewL(iMaxBlockSize + KSlack);
	iWideBuf = HBufC16::NewL(iMaxBlockSize + KSlack);
	iNarrowPtr.Set(iNarrowBuf->Des());
	iWidePtr.Set(iWideBuf->Des());
	iConverter = CCnvCharacterSetConverter::NewL();
	}
