// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides test functions to generate strings based on types of character sets.
// 
//

/**
 @file
*/

#ifndef _TE_SMSPRT_ALPHABET_H_
#define _TE_SMSPRT_ALPHABET_H_

#include <charconv.h>


/**
 *  Character set class names.
 */
_LIT(KCharacterSetStandard7bitGSM, "Standard7bitGSM");
_LIT(KCharacterSetStandard7bitGSMShift, "Standard7bitGSMShift");
_LIT(KCharacterSetStandard8bitGSM, "Standard8bitGSM");
_LIT(KCharacterSetVariousUCS2, "VariousUCS2");
_LIT(KCharacterSetUnconvertible7bitGSM, "Unconvertible7bitGSM");
_LIT(KCharacterSetDowngrading7bitGSM, "Downgrading7bitGSM");
_LIT(KCharacterSetDowngrading7bitGSMExtended, "Downgrading7bitGSMExtended");
_LIT(KCharacterSetDowngradingTurkish7bitGSM, "DowngradingTurkish7bitGSM");
_LIT(KCharacterSetTurkish7bitGSMLocking, "Turkish7bitGSMLocking");
_LIT(KCharacterSetTurkish7bitGSMShift, "Turkish7bitGSMShift");
_LIT(KCharacterSetNonTurkishDowngrading7bitGSMChars, "NonTurkishDowngrading7bitGSMChars");
_LIT(KCharacterSetDowngradingSpanish7bitGSM, "DowngradingSpanish7bitGSM");
_LIT(KCharacterSetSpanish7bitGSMShift, "Spanish7bitGSMShift");
_LIT(KCharacterSetDowngradingPortuguese7bitGSM, "DowngradingPortuguese7bitGSM");
_LIT(KCharacterSetPortuguese7bitGSMLocking, "Portuguese7bitGSMLocking");
_LIT(KCharacterSetPortuguese7bitGSMShift, "Portuguese7bitGSMShift");
_LIT(KCharacterSetPortuguese7bitGSMExt, "Portuguese7bitGSMExt");
_LIT(KCharacterSetPortuguese7bitGSMShiftAndStd1, "Portuguese7bitGSMShiftAndStd1");
_LIT(KCharacterSetPortuguese7bitGSMShiftAndStd2, "Portuguese7bitGSMShiftAndStd2");

/**
 *  Definition of a mapped character. This is the character that may be sent and the
 *  character that would be expected to be received.
 */
typedef struct
	{
	TChar  iOriginal;
	TChar  iExpected;
	TInt   iOctets;
	} TMappedChar;

	
/**
 *  Provides functions to generate strings based on types of character sets.
 */
class CSmsPrtTestAlphabet : public CBase
{
public:
	static CSmsPrtTestAlphabet* NewL();
	~CSmsPrtTestAlphabet();
	
	TInt GetCharacterSetSizeL(const TDesC& aCharSet);

	TInt GetCharacterSetPositionL(const TDesC& aCharSet);
	void SetCharacterSetPositionL(const TDesC& aCharSet, TInt aPosition);
	void ResetAllCharacterSetPositionsL();

	TBool IsCharListedInCharacterSetL(const TDesC& aCharSet, TChar aChar);
	TBool IsCharDowngradedByCharacterSetL(const TDesC& aCharSet, TChar aChar);
	
	void AppendCharactersL(const TDesC& aCharSet, TInt aCount,
			               TDes& aOriginal, TDes& aExpected,
			               TInt& aOctetsCount, TInt& aDowngradesCount,
			               TInt& aUnconvertibleCount);
	
private:
	CSmsPrtTestAlphabet();
	void ConstructL();
	
	TInt GetCharacterSetIndexL(const TDesC& aCharSet);

	RArray<TPtrC>  iCharSetNames;
	RArray<const TMappedChar*>  iCharSetArrays;
	RArray<TInt>  iCharSetSizes;
	RArray<TInt>  iCharPositions;
};

#endif // _TE_SMSPRT_ALPHABET_H_
