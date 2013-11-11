// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "BidiCopy.h"
#include <e32std.h>
#include "mglyphs.inl"

TInt BidiCopy::Mirror(TInt a)
/**
Find the mirror image of a character.
@internalComponent
*/
	{
	TInt current = MirrorStart(a);
	unsigned long v = mGlyphArray[current];
	TInt key = v >> 16;
	// 1st easy-out: no steps at all for any at this key
	if (key == 0)
		return a;
	// 2nd easy-out: no steps at all but we have found it
	if (key == a)
		return v & 0xFFFF;
	TInt step = MirrorStep(a);
	do	{
		current += step;
		current &= (KMirrorTableSize-1);
		key = mGlyphArray[current] >> 16;
		if (key == a)
			return mGlyphArray[current] & 0xFFFF;
		} while (key != 0);
	return a;
	}

void BidiCopy::ReverseCodes(TText* aText, TInt aLength)
/**
Reverse the contents of aText[0..aLength], so that aText[0] holds the
value previously found in aText[aLength - 1], aText[1] holds the value
previously held in aText[aLength - 2] and so on.
@internalComponent
*/
	{
	TText *text = aText;	// backup

	// reverse int16 by int16 (note: high surrogate + low surrogate will be reverse to low surrogate + high surrogate)
	TText* end = aText + aLength - 1;
	while (aText < end)
		{
		TText t = *aText;
		*aText = *end;
		*end = t;
		++aText;
		--end;
		}

	CorrectSurrogatePairs(text, aLength);
	}

void BidiCopy::DeleteUnreversedSurrogates(TText* aText, TInt aLength)
/**
Replaces all surrogates with 0xFFFF that are not paired up the wrong
way round.
@internalComponent
*/
	{
	if (aLength < 1)
		return;
	TText* end = aText + aLength;
	while (aText < end)
		{
		if ((aText[0] & 0xF800) == 0xD800)
			{
			if ((aText[0] & 0xFC00) == 0xD800
				&& aText + 1 != end
				&& (aText[1] & 0xFC00) == 0xDC00)
				++aText;
			else
				aText[0] = 0xFFFF;
			}
		++aText;
		}
	}

void BidiCopy::CorrectSurrogatePairs(TText* aText, TInt aLength)
/**
Corrects all reversed surrogate pairs. Assumes that no unpaired
surrogates are present.
@internalComponent
*/
	{
	TText* end = aText + aLength;
	while (aText < end)
		{
		if ((aText[0] & 0xF800) == 0xD800)
			{
			// Surrogate pair discovered.
			// Do we need to swap it?
			ASSERT(aText + 1 < end);
			ASSERT((aText[1] & 0xF800) == 0xD800);
			if ((aText[0] & 0xFC00) == 0xDC00)
				{
				ASSERT((aText[1] & 0xFC00) == 0xD800);
				TText t = aText[0];
				aText[0] = aText[1];
				aText[1] = t;
				}
			++aText;
			}
		++aText;
		}
	}

void BidiCopy::CorrectGroups(TText* aText, TInt aLength)
/**
Correct all reversed groups (non-combiner followed by combining
characters). Surrogate pairs may be reversed by this operation.
Assumes that no unpaired surrogates are present.
Leading and trailing 0xFFFFs are left alone.
@internalComponent
*/
	{
	TText *end = aText + aLength;
	// Ignore leading 0xFFFFs. This helps Tagma's
	// RTmTextCache::GetDisplayedText with its sentinels.
	while (aText < end && *aText == 0xFFFF)
		++aText;
	TText* groupStart = aText;
	while (aText < end)
		{
		TText* next = aText + 1;
		TInt c = aText[0];
		if ((c & 0xFC00) == 0xD800)
			{
			ASSERT(aText + 1 < end);
			ASSERT((aText[1] & 0xFC00) == 0xDC00);
			c = (c << 10) + (aText[1] & 0x3FF)
				+ (0x10000 - 0xD800*0x400);
			++next;
			}
		// ignore non-characters
		if ((c & 0xFFFE) != 0xFFFE)
			{
			// non-marks represent the end of groups that may need to
			// be reversed
			TChar ch = c;
			if ((ch.GetCategory() & 0xFFFFFFF0) != TChar::EMarkGroup)
				{
				if (aText != groupStart)
					ReverseCodes(groupStart, aText - groupStart + 1);
				groupStart = next;
				}
			}
		aText = next;
		}
	}

void BidiCopy::SubstituteMirrorImages(TText* aText, TInt aLength)
/**
Reverse all mirror image characters. Will not change characters
in the basic multilingual plane for surrogate pairs. Assumes
that no unpaired surrogates are present.
@internalComponent
*/
	{
	TText *end = aText + aLength;
	while (aText < end)
		{
		TText* next = aText + 1;
		TInt c = aText[0];
		if ((c & 0xFC00) == 0xD800)
			{
			ASSERT(aText + 1 < end);
			ASSERT((aText[1] & 0xFC00) == 0xDC00);
			c = (c << 10) + (aText[1] & 0x3FF)
				+ (0x10000 - 0xD800*0x400);
			++next;
			}
		TInt m = Mirror(c);
		if (m != c)
			{
			if (m <= 0xFFFF)
				{
				aText[0] = static_cast<TText>(m);
				if (0xFFFF < c)
					aText[1] = 0xFFFF;
				}
			else if (0xFFFF < c)
				{
				aText[0] = static_cast<TText>((c >> 10) + 0xD760);
				aText[1] = static_cast<TText>((c & 0x3FF) + 0xDC00);
				}
			}
		aText = next;
		}
	}

TText* BidiCopy::CopyMirror(TText* aDestination, const TText* aSource, TInt length)
/**
Copy some text, substituting mirrored characters.

@return aDestination + number of TText codes output.
@internalComponent
*/
	{
	for (; length; --length, ++aDestination, ++aSource)
		*aDestination = static_cast<TText>(Mirror(*aSource));
	return aDestination;
	}

TText* BidiCopy::OutputTChar(TText* aDestination, TInt aToOutput)
/**
Write out the input character as UTF16.

@return aDestination + number of TText codes output.
@internalComponent
*/
	{
	if (aToOutput > 0xFFFF)
		{
		TInt c = aToOutput;
		aDestination[0] = static_cast<TText>((c >> 10) + 0xD760);
		aDestination[1] = static_cast<TText>((c & 0x3FF) + 0xDC00);
		return aDestination + 2;
		}
	*aDestination = static_cast<TText>(aToOutput);
	return aDestination + 1;
	}

TText* BidiCopy::CopyBackwards(TText* aDestination,
	const TText* aSource, TInt aLength)
/**
Copy the text backwards without substituting mirrored characters,
checking for surrogate pairs or combining characters.

@return aDestination + number of TText codes output.
@internalComponent
*/
	{
	TText *end = aDestination + aLength;
	while (aDestination != end)
		{
		*--end = *aSource++;
		}
	return aDestination + aLength;
	}

TText* BidiCopy::CopyBackwardsWithMirroring(TText* aDestination,
	const TText* aSource, TInt aLength)
/**
Copy the text backwards to the output without checking for surrogate
pairs or combining characters, substituting mirrored characters.

@return aDestination + number of TText codes output.
@internalComponent
*/
	{
	TText *end = aDestination + aLength;
	while (aDestination != end)
		{
		*--end = static_cast<TText>(Mirror(*aSource++));
		}
	return aDestination + aLength;
	}

TText* BidiCopy::CopyGroupsBackwards(TText* aDestination,
	const TText* aSource, TInt aLength)
/**
Copy the text backwards, substituting mirrored characters and correctly
handling all surrogate pairs and combining characters.

@return aDestination + number of TText codes output.
@internalComponent
*/
	{
	TText* retval = CopyBackwards(aDestination, aSource, aLength);
	DeleteUnreversedSurrogates(aDestination, aLength);
	SubstituteMirrorImages(aDestination, aLength);
	CorrectGroups(aDestination, aLength);
	CorrectSurrogatePairs(aDestination, aLength);
	return retval;
	}

TBool BidiCopy::ImplicitDirectionalityIsRightToLeft(
	const TText* aText, TInt aLength, TBool* aFound)
/**
Find out if the implicit directionality of a piece of text is right to
left.

@param aText Text to be examined, or the first portion of text to be examined.
@param aLength Length of the text.
@param aFound If non-null, returns ETrue if the directionality could be
determined from aText. If not, and there is more text beyond, the function 
should be called with the next portion of text until aFound returns ETrue 
or there is no more text.
@return	ETrue if the directionality is right to left. If the directionality
cannot be determined solely from this text, EFalse is returned.	This is the 
implicit directionality of text that is composed entirely of neutrals.
@internalComponent
*/
	{
	if (aFound)
		*aFound = ETrue;
	const TText* end = aText + aLength;
	TInt lastUtf16 = 0;
	while (aText != end)
		{
		TInt code = *aText++;
		if ((code & 0xFFFFFC00) == 0xD800
			&& (lastUtf16 & 0xFFFFFC00) == 0xDC00)
			{
			code = (code << 10) + (lastUtf16 & 0x3FF)
				+ (0x10000 - 0xD800*0x400);
			}
		lastUtf16 = code;
		TChar c = code;
		switch(c.GetBdCategory())
			{
		case TChar::ELeftToRight:
			return EFalse;
		case TChar::ERightToLeft:
		case TChar::ERightToLeftArabic:
			return ETrue;
		default:
			break;
			}
		}
	if (aFound)
		*aFound = EFalse;
	return EFalse;
	}
