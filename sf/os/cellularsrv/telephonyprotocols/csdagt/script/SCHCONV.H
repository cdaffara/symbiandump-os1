/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Character conversion class header.
* 
*
*/



/**
 @file Schconv.h
 @internalComponent
*/

#ifndef __SCHCONV_H__
#define __SCHCONV_H__

#include <f32file.h>		// need this because bug in charconv
#include <charconv.h>

const TInt KMaxCharacterTypeLength=60;

class CScriptCharacterConverter : public CBase
/**
@internalComponent
*/
	{
public:
	static CScriptCharacterConverter* NewL();
	CScriptCharacterConverter();
	~CScriptCharacterConverter();
//	
	HBufC8* ConvertLC(const TDesC8& aString, const TDesC& aCharSet);
	HBufC8* ConvertLC(const TDesC16& aString, const TDesC& aCharSet);
	HBufC8* ConvertL(const TDesC8& aString, const TDesC& aCharSet);
	HBufC8* ConvertL(const TDesC16& aString, const TDesC& aCharSet);
	void ConvertFromDefaultL(const TDesC8& aSrc,TDes& aTrg);
	void SetDefaultCharSet(const TDesC& aCharSet);
private:	
	void ConstructL();
	void PrepareToConvertL(const TDesC& aNameToMatch);
	void CopyToEightBitDataL(TDes8& aTrg, const TDesC16& aSrc);
	void CopyFromEightBitDataL(TDes16& aTrg, const TDesC8& aSrc);
	void DoConvertFromDefaultToUnicodeL(const TDesC8& aSrc,TDes16& aTrg);
	TBool IsUnicode(const TDesC& aType) const;
private:
	CCnvCharacterSetConverter* iConverter;
	TBuf<KMaxCharacterTypeLength> iDefaultCharSet;
	RFs iFs;
	};

#endif
