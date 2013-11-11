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

#include "BidiCompact.h"
#include "BidiCopy.h"
#include <gdi.h>

static const TInt KZeroWidthJoiner = 0x200D;
// This gets round the compiler warning about converting
// EFRightToLeft to unsigned long.
inline TUint FRightToLeft() { return static_cast<TUint>(TRunInfoCompact::EFRightToLeft); }

/**
Constructs a run description without considering optimisations based
on the text itself.
@param aStart Index of the start of the run.
@param aLength Length of the run.
@param aReverse ETrue if the run is right-to-left.
@internalTechnology
*/
TRunInfoCompact::TRunInfoCompact(TInt aStart, TInt aLength,
	TBool aReverse)
	: iStart(aStart), iLengthAndType(aLength)
	{
	if (aReverse)
		iLengthAndType |= FRightToLeft();
	}

/**
Constructs a run description.

@param aStart Index of the start of the run.
@param aLength Length of the run.
@param aReverse ETrue if the run is right-to-left.
@param aText The text that this run refers to (starting at index 0, not
the start of the run). This is required only to determine if optimisations 
to the re-ordering are possible.
@internalTechnology
*/
TRunInfoCompact::TRunInfoCompact(TInt aStart, TInt aLength,
	TBool aReverse, const TText* aText)
	: iStart(aStart), iLengthAndType(aLength)
	{
	ASSERT(0 <= aLength);
	ASSERT(aLength < 0x10000000);
	ASSERT(0 <= aStart);
	if (!aReverse)
		return;
	iLengthAndType |= FRightToLeft();
	TUint32 flags = EFNoPairsNoCombiners | EFNoMirroredCharacters;
	aText += aStart;

	for (const TText* end = aText + aLength; aText < end && flags; ++aText)
		{
		TInt code = *aText;
		if ((code & 0xF800) == 0xD800)
			{
			flags &= ~EFNoPairsNoCombiners;
			if ((code & 0xFC00) == 0xDC00
				&& aText + 1 < end
				&& (aText[1] & 0xFC00) == 0xD800)
				{
				code = (aText[1] << 10) + (code & 0x3FF)
					+ (0x10000 - 0xD800*0x400);
				++aText;
				}
			}
		TChar c = code;
		if (c.GetCombiningClass() != 0)
			flags &= ~EFNoPairsNoCombiners;
		if (BidiCopy::Mirror(code) != code)
			flags &= ~EFNoMirroredCharacters;
		}
	iLengthAndType |= flags;
	}

/**
Attempts to extend a run.

@param aToBeAdded The run to be merged.
@return ETrue if extension succeeded, EFalse if not.
@internalTechnology
*/
TBool TRunInfoCompact::AddRun(const TRunInfoCompact& aToBeAdded)
	{
	TInt length = Length();
	if (length == 0)
		{
		*this = aToBeAdded;
		return ETrue;
		}

	// Are both runs in the same direction?
	if ((iLengthAndType ^ aToBeAdded.iLengthAndType) & FRightToLeft())
		return EFalse;

	TBool rightToLeft = TypeFlags() & EFRightToLeft;
	TInt end = rightToLeft?
		Start() - Length() : Start() + Length();

	if (end != aToBeAdded.Start())
		return EFalse;

	length += aToBeAdded.Length();

	iLengthAndType = length | (TypeFlags() & aToBeAdded.TypeFlags());

	if (rightToLeft)
		iStart -= aToBeAdded.Length();

	return ETrue;
	}

/**
Reorders text described by this run according to aContext. Allow 6 extra
bytes for a truncation.
@param aDestination	Where to write this run of visually-ordered text to.
@param aContext The source of the text to be ordered.
@return	The first byte not written to: in other words, what aDestination
should be updated to.
@internalTechnology
*/
TText* TRunInfoCompact::Reorder(TText* aDestination,
	const TRunInfoCompact::TReorderingContext& aContext) const
	{
	TInt start = Start();
	if (aContext.iEnd < start)
		// does not overlap
		return aDestination;
	TInt end = Start() + Length();
	if (end <= aContext.iStart)
		// does not overlap
		return aDestination;
	TBool startJoins = EFalse;
	if (start <= aContext.iStart)
		{
		start = aContext.iStart;
		startJoins = aContext.iJoinsAtStart;
		}
	TBool truncated = EFalse;
	TBool endJoins = EFalse;
	if (aContext.iEnd <= end)
		{
		if (aContext.iEnd < end
			&& aContext.iTruncation != 0xFFFF)
			truncated = ETrue;
		end = aContext.iEnd;
		endJoins = aContext.iJoinsAtEnd;
		}
	TInt length = end - start;
	if (length == 0 && !truncated)
		return aDestination;
	ASSERT(0 <= length);
	const TText* source = aContext.iSource + start;
	if (TypeFlags() & FRightToLeft())
		{
		// Right-to-left
		if (truncated)
			aDestination = BidiCopy::OutputTChar(aDestination, aContext.iTruncation);
		if (endJoins)
			*(aDestination++) = KZeroWidthJoiner;
		if (TypeFlags() & EFNoPairsNoCombiners)
			{
			// Simple
			aDestination = TypeFlags() & EFNoMirroredCharacters?
				BidiCopy::CopyBackwards(aDestination, source, length)
				: BidiCopy::CopyBackwardsWithMirroring(aDestination, source, length);
			}
		else
			// Respect groups
			aDestination = BidiCopy::CopyGroupsBackwards(aDestination, source, length);
		if (startJoins)
			*aDestination++ = KZeroWidthJoiner;
		return aDestination;
		}
	// Left-to-right
	if (startJoins)
		*aDestination++ = KZeroWidthJoiner;
	Mem::Copy(aDestination, source, length * sizeof(TText));
	aDestination += length;
	if (endJoins)
		*aDestination++ = KZeroWidthJoiner;
	if (truncated)
		aDestination = BidiCopy::OutputTChar(aDestination, aContext.iTruncation);
	return aDestination;
	}

/**
Converts an array of aArraySize TBidirectionalState::TRunInfos into a
compact form.

@param aBuffer Memory to output to, or null just to find out how large the output
array will need to be.
@param aText The text that aRunArray refers to.
@param aRunArray The array to be converted.
@param aArraySize The length of aRunArray.
@return The length of the output array.
@internalTechnology
*/
TInt TRunInfoCompact::Convert(TRunInfoCompact* aBuffer, const TDesC& aText,
	const TBidirectionalState::TRunInfo* aRunArray, TInt aArraySize)
	{
	const TText* text = aText.Ptr();
	TInt outputSize =  0;

	TRunInfoCompact currentRun;
	while (aArraySize)
		{
		TRunInfoCompact newRun(aRunArray->iStart, aRunArray->iLength,
			aRunArray->iDirection, text);
		--aArraySize;
		if (!currentRun.AddRun(newRun))
			{
			if (aBuffer)
				*aBuffer++ = currentRun;
			++outputSize;
			currentRun = newRun;
			}
		++aRunArray; //point to next run
		}
	if (0 < currentRun.Length())
		{
		if (aBuffer)
			*aBuffer++ = currentRun;
		++outputSize;
		}

	return outputSize;
	}

/**
Utility tells whether a character will form a join with the previous
base character.

@param aText The text.
@param aIndex The index into aText of the character to test.
@return ETrue if there is a join before the character.
*/
TBool TRunInfoCompact::JoinBefore(const TText* aText, TInt aIndex)
	{
	TInt charUnderTest = aText[aIndex];
	if (!CFont::CharactersJoin(charUnderTest, KZeroWidthJoiner))
		// Character does not join with anything, so we
		// will not do any more work.
		return EFalse;
	while (aIndex != 0)
		{
		--aIndex;
		TInt c = aText[aIndex];
		// If it is an Arabic point, we will skip it.
		if (0x64B <= c && c < 0x671
			&& !(0x656 <= c && c < 0x670))
			continue;
		return CFont::CharactersJoin(charUnderTest, c);
		}
	return EFalse;
	}
