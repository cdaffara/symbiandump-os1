// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32svr.h>
#include "LineBreak.h"
#include "LineBreakImp.h"
#include "GlyphSel.h"

const TText16 KThaiCodePageStart = 0x0E00;
const TText16 KThaiCodePageEnd   = 0x0E5C;
const TUint KNumThaiCharacters = KThaiCodePageEnd - KThaiCodePageStart;

/**
 Ecanpsulates rules for when Thai character sequence line breaking.
@internalComponent
*/
class ThaiLinebreakRules
	{
public:
	enum TCharClassification
		{
		EOutOfRange,

		EConsOAng,
		EConsYoYak,
		EConsHoHip,
		EConsWoWean,
		EConsDigraph,
		EConsOther,

		EPostVowelA,
		EPostVowelAA,
		EPostVowelOther,
		EPreVowel,

		EDepMaiHanAkat,
		EDepSaraI,
		EDepOther,

		// marker for end
		EMaxClassification
		};
	enum
		{
		KOutOfRangeFlag = 1 << EOutOfRange,
		KConsOAngFlag = 1 << EConsOAng,
		KConsYoYakFlag = 1 << EConsYoYak,
		KConsHoHipFlag = 1 << EConsHoHip,
		KConsWoWeanFlag = 1 << EConsWoWean,
		KConsDigraphFlag = 1 << EConsDigraph,
		KConsOtherFlag = 1 << EConsOther,
		KPostVowelAFlag = 1 << EPostVowelA,
		KPostVowelAAFlag = 1 << EPostVowelAA,
		KPostVowelOtherFlag = 1 << EPostVowelOther,
		KPreVowelFlag = 1 << EPreVowel,
		KDepMaiHanAkatFlag = 1 << EDepMaiHanAkat,
		KDepSaraIFlag = 1 << EDepSaraI,
		KDepOtherFlag = 1 << EDepOther,

		KSpecialDepFlags = KDepMaiHanAkatFlag | KDepSaraIFlag,
		KPostVowelFlags = KPostVowelAFlag | KPostVowelAAFlag | KPostVowelOtherFlag,
		KConsFlags = KConsOtherFlag | KConsDigraphFlag | KConsWoWeanFlag
			| KConsHoHipFlag | KConsYoYakFlag | KConsOAngFlag,
		KAllFlags = KOutOfRangeFlag | KConsOAngFlag | KConsYoYakFlag
			| KConsHoHipFlag | KConsWoWeanFlag | KConsDigraphFlag
			| KConsOtherFlag | KPostVowelAFlag | KPostVowelAAFlag
			| KPostVowelOtherFlag | KPreVowelFlag | KDepMaiHanAkatFlag
			| KDepSaraIFlag | KDepOtherFlag
		};

	/** Returns the Thai linebreaking class of the character. */
	static TCharClassification Class(TInt aChar);
	/** Returns true if aChar is a combining character according to aBreaker. */
	static TBool IsCombiningChar(TInt aChar, const MLineBreaker& aBreaker);
	/** Returns the Thai linebreaking class of the character at (*aChar),
	searching backwards for the base character if it is foreign and of type CM.
	@param aChar The position of the character within the string.
	@param aStart The start of the string.
	@param aBreaker The line breaker to query for line breaking class.
	*/
	static TCharClassification DerivedClass(const TText* aChar,
		const TText* aStart, const MLineBreaker& aBreaker);
	/**
	 Gets the line break rule for the previous and current character pair
	@param aPrevClass Class of the previous character.
	@param aClass Class of the current character.
	@return the rule code corresponding to the input pair.
	*/
	static TBool BreakAllowedBetween(
		TCharClassification aPrevClass, TCharClassification aClass);
	/** Returns whether a line break is allowed before the SA character at
	(*aChar).
	@param aChar The position of the character within the string.
	@param aStart The start of the string.
	@param aBreaker The line breaker to query for line breaking class.
	*/
	static TBool BreakAllowedAt(const TText* aChar,
		const TText* aStart, const MLineBreaker& aBreaker);

	/**
	 Classification of each character
	*/
	static const TUint8 KCharClassifications[KNumThaiCharacters];

	/**
	 Rules table of prev to next character
	*/
	static const TUint32 KRules[EMaxClassification];
	};

const TUint8 ThaiLinebreakRules::KCharClassifications[KNumThaiCharacters] = 
	{
	EOutOfRange,	// 0x0E00
	EConsOther,		// 0x0E01
	EConsOther,		// 0x0E02
	EConsOther,		// 0x0E03
	EConsOther,		// 0x0E04
	EConsOther,		// 0x0E05
	EConsOther,		// 0x0E06
	EConsDigraph,	// 0x0E07
	EConsOther,		// 0x0E08
	EConsOther,		// 0x0E09
	EConsOther,		// 0x0E0A
	EConsOther,		// 0x0E0B
	EConsOther,		// 0x0E0C
	EConsOther,		// 0x0E0D
	EConsOther,		// 0x0E0E
	EConsOther,		// 0x0E0F

	EConsOther,		// 0x0E10
	EConsOther,		// 0x0E11
	EConsOther,		// 0x0E12
	EConsOther,		// 0x0E13
	EConsOther,		// 0x0E14
	EConsOther,		// 0x0E15
	EConsOther,		// 0x0E16
	EConsOther,		// 0x0E17
	EConsOther,		// 0x0E18
	EConsDigraph,	// 0x0E19
	EConsOther,		// 0x0E1A
	EConsOther,		// 0x0E1B
	EConsOther,		// 0x0E1C
	EConsOther,		// 0x0E1D
	EConsOther,		// 0x0E1E
	EConsOther,		// 0x0E1F

	EConsOther,		// 0x0E20
	EConsDigraph,	// 0x0E21
	EConsYoYak,		// 0x0E22
	EConsDigraph,	// 0x0E23
	EConsOther,		// 0x0E24
	EConsDigraph,	// 0x0E25
	EConsOther,		// 0x0E26
	EConsWoWean,	// 0x0E27
	EConsOther,		// 0x0E28
	EConsOther,		// 0x0E29
	EConsOther,		// 0x0E2A
	EConsHoHip,		// 0x0E2B
	EConsOther,		// 0x0E2C
	EConsOAng,		// 0x0E2D
	EConsOther,		// 0x0E2E
	EOutOfRange,	// 0x0E2F

	EPostVowelA,	// 0x0E30
	EDepMaiHanAkat,	// 0x0E31
	EPostVowelAA,	// 0x0E32
	EPostVowelOther,// 0x0E33

	EDepSaraI,		// 0x0E34
	EDepOther,		// 0x0E35
	EDepOther,		// 0x0E36
	EDepOther,		// 0x0E37
	EDepOther,		// 0x0E38
	EDepOther,		// 0x0E39
	EDepOther,		// 0x0E3A

	EOutOfRange,	// 0x0E3B
	EOutOfRange,	// 0x0E3C
	EOutOfRange,	// 0x0E3D
	EOutOfRange,	// 0x0E3E
	EOutOfRange,	// 0x0E3F

	EPreVowel,		// 0x0E40
	EPreVowel,		// 0x0E41
	EPreVowel,		// 0x0E42
	EPreVowel,		// 0x0E43
	EPreVowel,		// 0x0E44

	EPostVowelOther,// 0x0E45
	EOutOfRange,	// 0x0E46

	EDepOther,		// 0x0E47
	EDepOther,		// 0x0E48
	EDepOther,		// 0x0E49
	EDepOther,		// 0x0E4A
	EDepOther,		// 0x0E4B
	EDepOther,		// 0x0E4C
	EDepOther,		// 0x0E4D
	EDepOther,		// 0x0E4E
	EOutOfRange,	// 0x0E4F

	EOutOfRange,		// 0x0E50
	EOutOfRange,		// 0x0E51
	EOutOfRange,		// 0x0E52
	EOutOfRange,		// 0x0E53
	EOutOfRange,		// 0x0E54
	EOutOfRange,		// 0x0E55
	EOutOfRange,		// 0x0E56
	EOutOfRange,		// 0x0E57
	EOutOfRange,		// 0x0E58
	EOutOfRange,		// 0x0E59
	EOutOfRange,		// 0x0E5A
	EOutOfRange			// 0x0E5B
	};

const TUint32 KNormalBreaksBeforeCons =
	ThaiLinebreakRules::KPreVowelFlag
	| ThaiLinebreakRules::KConsOtherFlag
	| ThaiLinebreakRules::KConsDigraphFlag
	| ThaiLinebreakRules::KConsHoHipFlag
	| ThaiLinebreakRules::KOutOfRangeFlag;
const TUint32 KNormalBreaksBeforePostVowel =
	ThaiLinebreakRules::KPreVowelFlag
	| ThaiLinebreakRules::KPostVowelFlags
	| ThaiLinebreakRules::KConsFlags
	| ThaiLinebreakRules::KOutOfRangeFlag;
const TUint32 ThaiLinebreakRules::KRules[EMaxClassification] =
	{
	/* Prev Char EOutOfRange */
	KAllFlags - KOutOfRangeFlag,
	/* Prev Char EConsOAng */
	KNormalBreaksBeforeCons,
	/* Prev Char EConsYoYak */
	KNormalBreaksBeforeCons,
	/* Prev Char EConsHoHip */
	KNormalBreaksBeforeCons - KConsDigraphFlag,
	/* Prev Char EConsWoWean */
	KNormalBreaksBeforeCons - KConsDigraphFlag - KConsHoHipFlag,
	/* Prev Char EConsDigraph */
	KNormalBreaksBeforeCons,
	/* Prev Char EConsOther */
	KNormalBreaksBeforeCons,
	/* Prev Char EPostVowelA */
	KNormalBreaksBeforePostVowel,
	/* Prev Char EPostVowelAA */
	KNormalBreaksBeforePostVowel - KPostVowelAFlag,
	/* Prev Char EPostVowelOther */
	KNormalBreaksBeforePostVowel,
	/* Prev Char EPreVowel */
	KPreVowelFlag | KPostVowelFlags | KOutOfRangeFlag,
	/* Prev Char EDepMaiHanAkat */
	KSpecialDepFlags | KPreVowelFlag | KPostVowelAAFlag
		| KPostVowelAFlag | KOutOfRangeFlag,
	/* Prev Char EDepSaraI */
	KSpecialDepFlags | KPreVowelFlag | KPostVowelAAFlag
		| KPostVowelAFlag | KOutOfRangeFlag,
	/* Prev Char EDepOther */
	KSpecialDepFlags | KPreVowelFlag | KPostVowelAAFlag
		| KPostVowelAFlag | KConsOtherFlag | KConsDigraphFlag
		| KConsWoWeanFlag | KConsHoHipFlag | KOutOfRangeFlag
	};

ThaiLinebreakRules::TCharClassification ThaiLinebreakRules::Class(TInt a)
    {
	return static_cast<TCharClassification>(
		(KThaiCodePageStart <= a && a < KThaiCodePageEnd) ?
			KCharClassifications[a - KThaiCodePageStart] :
			EOutOfRange);
    }

TBool ThaiLinebreakRules::IsCombiningChar(TInt aChar,
	const MLineBreaker& aBreaker)
	{
	TUint dummy1, dummy2;
	return aBreaker.LineBreakClass(aChar, dummy1, dummy2) == MLineBreaker::ECmLineBreakClass;
	}

ThaiLinebreakRules::TCharClassification ThaiLinebreakRules::DerivedClass(
	const TText* aChar, const TText* aStart, const MLineBreaker& aBreaker)
	{
	ThaiLinebreakRules::TCharClassification c = Class(*aChar);
	while (c == EOutOfRange && aChar != aStart
		&& IsCombiningChar(*aChar, aBreaker))
		{
		--aChar;
		c = Class(*aChar);
		}
	return c;
	}

TBool ThaiLinebreakRules::BreakAllowedBetween(
	ThaiLinebreakRules::TCharClassification aClass,
	ThaiLinebreakRules::TCharClassification aNextClass)
	{
	return KRules[aClass] & (1 << aNextClass);
	}

TBool ThaiLinebreakRules::BreakAllowedAt(const TText* aChar,
	const TText* aStart, const MLineBreaker& aBreaker)
	{
	__ASSERT_DEBUG(aStart < aChar, User::Invariant());
	TCharClassification c = Class(*aChar);
	if (c == EOutOfRange && IsCombiningChar(*aChar, aBreaker))
		return EFalse;
	return BreakAllowedBetween(DerivedClass(aChar - 1, aStart, aBreaker), c);
	}

/**
 Returns whether a line break is possible within a run of characters all having
 the class ESaLineBreakClass (Complex content). Languages with Unicocde
 characters having such a class include: Thai, Lao, Myanmar and Khmer. This
 default implementation of the GetLineBreakInContext() method only supports the
 Thai script. Breaks are determined in Thai based on a simple understanding of
 syllable boundaries. When characters from the other unsupported Sa class
 languages are found the method exits with EFalse.
@param aText
 The text to be searched, which is a contiguous run of characters of class SA
 (or CM attatched to SA). The break position may be restricted further by
 aMinBreakPos and aMaxBreakPos, but more text is provided for extra context
 should it be needed.
@param aMinBreakPos
 The start of the text to be considered for line breaks.
@param aMaxBreakPos
 The end of the text to be considered for line breaks.
@param aForwards
 ETrue if aBreakPos is to be set with the first legal break position,
 EFalse if aBreakPos is to be set with the last legal break position.
@param aBreakPos
 If break position found on exit its value is >= Min and <= Max 
 positions supplied.
@return TBool
 ETrue if and only if a legal break was found, EFalse otherwise.
@publishedAll
@released
*/
EXPORT_C TBool MLineBreaker::GetLineBreakInContext(const TDesC16& aText,
		TInt aMinBreakPos, TInt aMaxBreakPos, TBool aForwards,
		TInt& aBreakPos) const
	{
	__ASSERT_DEBUG (0 <= aMinBreakPos && aMaxBreakPos <= aText.Length(), 
		Panic(ELineBreakPanic_InvalidInputParam));

	TInt length = aText.Length();

	if (aMinBreakPos < 1)
		aMinBreakPos = 1;
	if (length - 1 < aMaxBreakPos)
		aMaxBreakPos = length - 1;
	if (aMaxBreakPos < aMinBreakPos)
		return EFalse;

	const TText16* text = aText.Ptr();

	if (*text == KZeroWidthSpace)
		{
		aBreakPos = aMinBreakPos;
		return ETrue;
		}
	else if (*(text+length-1) == KZeroWidthSpace)
		return EFalse;
	
	TInt start = aForwards? aMinBreakPos : aMaxBreakPos;
	TInt end = aForwards? aMaxBreakPos + 1 : aMinBreakPos - 1;
	TInt direction = aForwards? 1 : -1;
	for (TInt i = start; i != end; i += direction)
		{
		if (ThaiLinebreakRules::BreakAllowedAt(text + i, text, *this))
			{
			aBreakPos = i;
			return ETrue;
			}
		}
	return EFalse;
	}
