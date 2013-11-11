// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Bidirectional text reordering; based on the Unicode Bidirectional Reordering Algorithm.
// 
//

#include <bidi.h>
#include "BidiCopy.h"
#include <s32std.h>

const TInt KBidirectionalStateOverrideStreamValueNone = 0;
const TInt KBidirectionalStateOverrideStreamValueLeftToRight = 1;
const TInt KBidirectionalStateOverrideStreamValueRightToLeft = 2;

inline TBool IsSupplementary(TUint aChar)
/**
@param aChar The 32-bit code point value of a Unicode character.

@return True, if aChar is supplementary character; false, otherwise.
*/
	{
	return (aChar > 0xFFFF);
	}

inline TBool IsHighSurrogate(TText16 aInt16)
/**
@return True, if aText16 is high surrogate; false, otherwise.
*/
	{
	return (aInt16 & 0xFC00) == 0xD800;
	}

inline TBool IsLowSurrogate(TText16 aInt16)
/**
@return True, if aText16 is low surrogate; false, otherwise.
*/
	{
	return (aInt16 & 0xFC00) == 0xDC00;
	}

inline TUint JoinSurrogate(TText16 aHighSurrogate, TText16 aLowSurrogate)
/**
Combine a high surrogate and a low surrogate into a supplementary character.

@return The 32-bit code point value of the generated Unicode supplementary
        character.
*/
	{
	return ((aHighSurrogate - 0xD7F7) << 10) + aLowSurrogate;
	}

TBool TextDefaultsToRightToLeft(const TDesC& aText, TBool* aFound);

TBidirectionalState::TCategory TBidirectionalState::CharToBdCat(TChar::TBdCategory aCat)
	{
	return static_cast<TBidirectionalState::TCategory>(
		1 << static_cast<TInt>(aCat));
	}

TBidirectionalState::TCategory TBidirectionalState::UintToBdCat(TUint aCat)
	{
	return static_cast<TBidirectionalState::TCategory>(1 << aCat);
	}

void TBidirectionalState::TReorderContext::SetNextCategory(
	TChar::TBdCategory aCat)
	{
	iNextCategory = CharToBdCat(aCat);
	}

void TBidirectionalState::TReorderContext::SetNextStrongCategory(
	TChar::TBdCategory aCat)
	{
	iNextStrongCategory = CharToBdCat(aCat);
	}


EXPORT_C void TBidirectionalState::ReverseGroups(TText* aStart,TInt aLength)
/** A utility to reverse text apart from combining characters, which remains after 
their base characters. This is what is needed when drawing right-to-left text.

@param aStart Start position of text to be reversed.
@param aLength Length of text to be reversed. */
	{
	BidiCopy::ReverseCodes(aStart, aLength);
	BidiCopy::DeleteUnreversedSurrogates(aStart, aLength);
	BidiCopy::SubstituteMirrorImages(aStart, aLength);
	BidiCopy::CorrectGroups(aStart, aLength);
	BidiCopy::CorrectSurrogatePairs(aStart, aLength);
	}


// A local helper function. Get the next character from a buffer. This
// function won't check buffer length.
//
// @param aText The text buffer to read character from.
// @param aCharacterIndex Count of characters to skip in aText.
// @return The character.
TUint GetOneCharacter(const TText16 *aText, TInt aCharacterIndex)
	{
	const TText16 *p = aText;
	TUint c = 0xFFFF;
	while (aCharacterIndex >= 0)
		{
		c = *p++;
		ASSERT(!IsLowSurrogate(c));
		if (IsHighSurrogate(c))
			{
			ASSERT(IsLowSurrogate(*p));
			c = JoinSurrogate(c, *p++);
			}
		--aCharacterIndex;
		}
	return c;
	}


TInt TBidirectionalState::GenerateBdRunArray(const TText* aText, TInt aLength,
	TBidirectionalState::TRunInfo* aRun, TInt aMaxRuns)
/** Analyse the input text for runs of characters that share the same
bidirectional class. Categories TChar::EEuropeanNumberSeparator and
TChar::ECommonNumberSeparator are kept as singletons due to a limitation in
the reordering logic.
@param aText The text to be analysed.
@param aLength The length of the text to be analysed.
@param aRun	Output buffer for the runs after analysis. May be null if there 
is to be no output.
@param aMaxRuns The size of the aRun array. No more than this number of runs 
will be	output.
@return The number of runs that are required for the full results of the
analysis.
@internalTechnology */	
    {
	if (aLength == 0)
		{
		if (aRun && 0 < aMaxRuns)
			{
			aRun[0].iCategory = TChar::EOtherNeutral;
			aRun[0].iStart = 0;
			aRun[0].iLength = 0;
			}
		return 1;
		}
	int runs = 0;
	int run_start = 0;
	int run_end = 1;
	const TText* p = aText;
	const TText* q = p + aLength;
	
	// get the character pointed by 'p', then move 'p' to next character, and adjust 'run_end' if need
	TChar pc = ::GetOneCharacter(p, 0);
	TChar::TBdCategory cur_cat = pc.GetBdCategory();
	++p;
	if (IsSupplementary(pc))
		{
		++p;
		run_end = 2;	// run_end points to "end" of current character
		}
	
	while (p < q)
		{
		// get the character pointed by 'p'
		pc = ::GetOneCharacter(p, 0);
		
		TChar::TBdCategory new_cat = pc.GetBdCategory();
		if (new_cat != cur_cat)
			{
			if (new_cat == TChar::ENonSpacingMark &&
				cur_cat != TChar::ELeftToRightEmbedding &&
				cur_cat != TChar::ELeftToRightOverride &&
				cur_cat != TChar::ERightToLeftEmbedding &&
				cur_cat != TChar::ERightToLeftOverride &&
				cur_cat != TChar::EPopDirectionalFormat)
				new_cat = cur_cat;
			else if (p < q - 1 &&
					 (new_cat == TChar::EWhitespace ||
					  new_cat == TChar::EEuropeanNumberSeparator ||
					  new_cat == TChar::ECommonNumberSeparator))
				{
				TChar nextChar = ::GetOneCharacter(p, 1);
				TChar::TBdCategory next_cat = nextChar.GetBdCategory();
				if (new_cat == TChar::EWhitespace)
					{
					if ((cur_cat == TChar::ELeftToRight ||
						 cur_cat == TChar::ERightToLeft ||
						 cur_cat == TChar::ERightToLeftArabic) && cur_cat == next_cat)
						new_cat = cur_cat;
					}
				else if (cur_cat == TChar::EEuropeanNumber && next_cat == TChar::EEuropeanNumber)
					new_cat = TChar::EEuropeanNumber;
				}
			}

		if (new_cat != cur_cat ||
			cur_cat == TChar::EEuropeanNumberSeparator ||
			cur_cat == TChar::ECommonNumberSeparator)
			{
			if (aRun && runs < aMaxRuns)
				{
				aRun[runs].iCategory = cur_cat;
				aRun[runs].iStart = run_start;
				aRun[runs].iLength = run_end - run_start;
				}
			
			runs++;
			run_start = run_end;
			}

		p++;
		run_end++;

		// adjust 'p' and 'run_end'
		if (IsSupplementary(pc))
			{
			p++;
			run_end++;
			}

		cur_cat = new_cat;
		}

	if (aRun && runs < aMaxRuns)
		{
		aRun[runs].iCategory = cur_cat;
		aRun[runs].iStart = run_start;
		aRun[runs].iLength = run_end - run_start;
		}

	return runs + 1;
	}


EXPORT_C TInt TBidirectionalState::ReorderText(const TText* aText,TInt aLength,TBool aParRightToLeft,
											   TText*& aNewText)
/** Reorders text according to the Unicode Bidirectional Reordering algorithm.

Reorders the input text from logical order (which may be bidirectional) to 
display order (strictly left to right).

@param aText The input text in logical order.
@param aLength The length of the input text.
@param aParRightToLeft ETrue if the default directionality of the text to be 
re-ordered is right-to-left.
@param aNewText Returns the re-ordered text. If the text did not need re-ordering, 
or if there was an error, aText will be returned. Otherwise, ownership of 
a newly allocated buffer will be returned to the caller. This buffer must 
be deleted with delete[] (or CleanupArrayDeletePushL()) and not delete (or 
CleanupStack::PushL()).
@return A system-wide error value if there has been an error; KErrNone if there 
has not. */
	{
	aNewText = const_cast<TText*>(aText);
	if (aLength < 2)
		return KErrNone;

	int error = KErrNone;
	TBidirectionalState::TRunInfo run_info;
	run_info.iDirection = 0;
	run_info.iIndex = 0;
	run_info.iStart = 0;
	run_info.iLength = aLength;
	TBidirectionalState::TRunInfo* run_info_array = &run_info;
	TBidirectionalState::TRunInfo* allocated_run_info_array = 0;
	int runs = GenerateBdRunArray(aText, aLength, run_info_array, 1);
	if (runs > 1)
		{
		allocated_run_info_array = new TBidirectionalState::TRunInfo[runs];
		if (allocated_run_info_array)
			{
			run_info_array = allocated_run_info_array;
			GenerateBdRunArray(aText, aLength, run_info_array, runs);
			}
		else
			{
			// the run cannot be allocated: stick with our single l-to-r run
			error = KErrNoMemory;
			runs = 1;
			}
		}
	if (error == KErrNone)
		{
		TBidirectionalState state;
		state.ReorderLine(run_info_array, runs, ETrue, ETrue, aParRightToLeft,
			TChar::EOtherNeutral, TChar::EOtherNeutral);
		}

	// If there was only one run and it's left-to-right, we've finished.
	if (!allocated_run_info_array && run_info.iDirection == 0)
		return error;

	// Reorder the text into a new buffer.
	TText* buffer = new TText[aLength];
	if (!buffer)
		{
		delete [] allocated_run_info_array;
		return KErrNoMemory;
		}
	const TBidirectionalState::TRunInfo* r = run_info_array;
	TText* dest = buffer;
	for (int i = 0; i < runs; i++, r++)
		{
		const TText* source = &aText[r->iStart];
		int length = r->iLength;
		Mem::Copy(dest,source,length * sizeof(TText));
		if (r->iDirection)
			ReverseGroups(dest,length);
		dest += length;
		}

	delete [] allocated_run_info_array;
	aNewText = buffer;
	return KErrNone;
	}


EXPORT_C TBidirectionalState::TBidirectionalState()
/** Standard constructor. */
	{
	Reset();
	}


/** Reorders a line of text and updates the bidirectional state for the next line.

@param aRunInfo An array of objects representing runs of characters with the 
same bidirectional category. Any number of characters can be combined into 
a run if they have the same category, except for the categories TChar::EEuropeanNumberSeparator 
and TChar::ECommonNumberSeparator, which should be put into single-character 
runs because the reordering logic depends on this.
@param aRuns Number of 'run info' objects.
@param aParStart Tells the function whether the line is the first line of a 
paragraph.
@param aParEnd Tells the function whether the line is the last line of a paragraph.
@param aParRightToLeft ETrue if the default directionality of the text to be 
re-ordered is right-to-left.
@param aNextCategory The category of the character immediately after the end 
of the line. This is ignored if aParEnd is ETrue.
@param aNextStrongCategory The category of the first strong character (one 
of the categories ELeftToRight, ELeftToRightEmbedding, ELeftToRightOverride, 
ERightToLeft, ERightToLeftArabic, ERightToLeftEmbedding or ERightToLeftOverride) 
after the end of the line. This is ignored if aParEnd is ETrue.
@param aVisualEndIsAmbiguous EFalse if the logical end of this line is at the
visual end and the logical beginning of the next line is at the visual beginning.
*/
EXPORT_C void TBidirectionalState::ReorderLine(TRunInfo* aRunInfo, TInt aRuns,
	TBool aParStart, TBool aParEnd, TBool aParRightToLeft,
	TChar::TBdCategory aNextCategory, TChar::TBdCategory aNextStrongCategory,
	TBool& aVisualEndIsAmbiguous)
	{
	ReorderLine(aRunInfo, aRuns, aParStart, aParEnd, aParRightToLeft,
		aNextCategory, aNextStrongCategory);
	if (iStackLevel  != 0)
		{
		aVisualEndIsAmbiguous = ETrue;
		return;
		}
	TCategory nextCat = CharToBdCat(aNextCategory);
	TCategory nextStrong = CharToBdCat(aNextStrongCategory);
	const TUint KAllStrongLeftToRight =
		ELeftToRight | ELeftToRightEmbedding | ELeftToRightOverride;
	const TUint KAllStrongRightToLeft =
		ERightToLeft | ERightToLeftArabic | ERightToLeftEmbedding | ERightToLeftOverride;
	if (aParRightToLeft)
		{
		// Ambiguous if any of the surrounding categories are strongly left-to-right
		aVisualEndIsAmbiguous =
			(iPreviousStrongCategory | iPreviousCategory | nextCat | nextStrong)
			& KAllStrongLeftToRight;
		}
	else
		{
		// Ambiguous if any of the surrounding categories are strongly right-to-left
		aVisualEndIsAmbiguous =
			(iPreviousStrongCategory | iPreviousCategory | nextCat | nextStrong)
			& KAllStrongRightToLeft;
		}
	}
/** Reorders a line of text and updates the bidirectional state for the next line.

@param aRunInfo An array of objects representing runs of characters with the 
same bidirectional category. Any number of characters can be combined into 
a run if they have the same category, except for the categories TChar::EEuropeanNumberSeparator 
and TChar::ECommonNumberSeparator, which should be put into single-character 
runs because the reordering logic depends on this.
@param aRuns Number of 'run info' objects.
@param aParStart Tells the function whether the line is the first line of a 
paragraph.
@param aParEnd Tells the function whether the line is the last line of a paragraph.
@param aParRightToLeft ETrue if the default directionality of the text to be 
re-ordered is right-to-left.
@param aNextCategory The category of the character immediately after the end 
of the line. This is ignored if aParEnd is ETrue.
@param aNextStrongCategory The category of the first strong character (one 
of the categories ELeftToRight, ELeftToRightEmbedding, ELeftToRightOverride, 
ERightToLeft, ERightToLeftArabic, ERightToLeftEmbedding or ERightToLeftOverride) 
after the end of the line. This is ignored if aParEnd is ETrue. */
EXPORT_C void TBidirectionalState::ReorderLine(TRunInfo* aRunInfo, TInt aRuns,
	TBool aParStart, TBool aParEnd, TBool aParRightToLeft,
	TChar::TBdCategory aNextCategory, TChar::TBdCategory aNextStrongCategory)
	{
	// Reset if this is a new paragraph.
	if (aParStart)
		{
		Reset();
		iPreviousCategory = EOtherNeutral;
		if (aParRightToLeft)
			{
			iStack[0].iEmbeddingLevel = 1;
			iPreviousStrongCategory = ERightToLeft;
			}
		}

	// Initialise the context object.
	TReorderContext context;
	context.iRunInfo = aRunInfo;
	context.iRuns = aRuns;
	context.iLastStrongCategory = iPreviousStrongCategory;
	if (aParEnd)
		context.iNextCategory = context.iNextStrongCategory = EOtherNeutral;
	else
		{
		context.iNextCategory = CharToBdCat(aNextCategory);
		context.iNextStrongCategory = CharToBdCat(aNextStrongCategory);
		}

	// Initialise output data and find out what categories are present so that unnecessary steps can be skipped.
	context.iCategories = iPreviousCategory | context.iNextCategory | context.iNextStrongCategory;
	for (TInt i = 0; i != aRuns; ++i)
		{
		aRunInfo[i].iEmbeddingLevel = iStack[0].iEmbeddingLevel;
		aRunInfo[i].iDirection = 0;
		aRunInfo[i].iIndex = i;
		aRunInfo[i].iCategory = UintToBdCat(aRunInfo[i].iCategory);
		context.iCategories |= aRunInfo[i].iCategory;
		}

	// Do nothing if no right-to-left material is present.
	if (aRuns == 0 ||
		(iStackLevel == 0 && iStack[0].iEmbeddingLevel == 0 &&
		 !(context.iCategories & (ERightToLeftGroup | EBdControlsGroup))))
		return;

	// Perform the bidirectional algorithm.
	if ((context.iCategories & EBdControlsGroup) ||
		State().iOverrideState != ENoOverrideState)
		HandleBdControls(context);
	ResolveWeakTypesW1W2W3(context);
	ResolveWeakTypesW4W5W6(context);
	ResolveWeakTypesW7(context);
	if (context.iCategories & EOtherNeutral)
		ResolveNeutralTypes(context);
	ResolveImplicitLevels(context);
	PrepareForNextLine(context);
	ReorderRuns(context);
	}


void TBidirectionalState::PrepareForNextLine(const TReorderContext& aContext)
/**
Fold context information back into TBidirectionalState.
@internalComponent
*/	
   {
	if (aContext.iRuns != 0)
		{
		iPreviousCategory = static_cast<TCategory>(
			aContext.iRunInfo[aContext.iRuns - 1].iCategory);
		iPreviousStrongCategory = aContext.iLastStrongCategory;
		}
	}


void TBidirectionalState::HandleBdControls(TReorderContext& aContext)
/**
Handle LRO, RLO, LRE, RLE and PDF. After this phase, these categories will no
longer be found.

This corresponds to Unicode(3.2) Bidirectional Algorithm phases X1-X7.
Phase X8 is not required as the run is assumed to be all in one paragraph.
Phases X9-X10 are implicit in other functions.

@internalComponent
*/	
   {
	aContext.iCategories = iPreviousCategory | aContext.iNextCategory;
	for (TInt i = 0; i != aContext.iRuns; ++i)
		{
		TRunInfo* r = aContext.iRunInfo + i;
		TCategory nextCatInLine = i < aContext.iRuns - 1?
			(TCategory)(r[1].iCategory) : ENoCategory;

		TBool was_pdf = FALSE;
		if (r->iCategory & EBdControlsGroup)
			{
			if (r->iCategory == EPopDirectionalFormat)
				{
				if (iStackLevel > 0)
					{
					was_pdf = TRUE;
					r->iEmbeddingLevel = State().iEmbeddingLevel;
					if (nextCatInLine == State().iStartCategory)
						// Ignore POP-PUSH pair with nothing between.
						// This is surely wrong? Perhaps it is a hack to
						// help other parts of the algorithm. Must investigate.
						// TPB.
						r->iCategory = r[1].iCategory = EBoundaryNeutral;
					else
						{
						r->iCategory = Pop();
						}
					}
				else
					r->iCategory = EBoundaryNeutral;
				}
			else
				{
				// Category is LRE, RLE, LRO or RLO.
				if (nextCatInLine == EPopDirectionalFormat)
					// Ignore PUSH-POP pair with nothing between.
					r->iCategory = r[1].iCategory = EBoundaryNeutral;
				else
					r->iCategory = Push(static_cast<TCategory>(r->iCategory));
				}
			}

		if (!was_pdf)
			{
			switch (State().iOverrideState)
				{
				case ELeftToRightOverrideState:
					r->iCategory = ELeftToRight;
					break;
				case ERightToLeftOverrideState:
					r->iCategory = ERightToLeft;
					break;
				default:
					break;
				}
			r->iEmbeddingLevel = State().iEmbeddingLevel;
			}
		if (r->iCategory & EStrongGroup)
			aContext.iLastStrongCategory = static_cast<TCategory>(r->iCategory);
		aContext.iCategories |= r->iCategory;
		}
	}


void TBidirectionalState::ResolveWeakTypesW1W2W3(TReorderContext& aContext)
/**
Unicode(3.2) Bidirectional Algorithm phases W1, W2 and W3.
@internalComponent
*/	
    {
	if (!(aContext.iCategories
		& (ENonSpacingMark | ERightToLeftArabic | EEuropeanNumber)))
		return;

	TRunInfo* endOfRuns = aContext.iRunInfo + aContext.iRuns;
	TCategory prev_cat = iPreviousCategory;
	TBool european_to_arabic_number = iPreviousStrongCategory == ERightToLeftArabic;

	aContext.iCategories = iPreviousCategory | aContext.iNextCategory;
	for (TRunInfo* r = aContext.iRunInfo; r != endOfRuns; r++)
		{
		switch (r->iCategory)
			{
			case ENonSpacingMark:					// non-spacing marks change to the previous category
				r->iCategory = prev_cat;
				break;
			case ELeftToRight:
				european_to_arabic_number = EFalse;
				break;
			case ERightToLeft:
				european_to_arabic_number = EFalse;
				break;
			case ERightToLeftArabic:				// Arabic letters change to R
				european_to_arabic_number = ETrue;
				r->iCategory = ERightToLeft;
				break;
			case EEuropeanNumber:				    // European numbers change to Arabic if last strong category was R
				if (european_to_arabic_number)
					r->iCategory = EArabicNumber;
				break;
			default:
				break;
			}
		aContext.iCategories |= r->iCategory;
		prev_cat = static_cast<TCategory>(r->iCategory);
		}
	}
/**
This phase removes categories NSM, AL, ES, ET, CS, BS, S, WS and BN, leaving
only L, R, EN, AN and ON.
@internalComponent
*/
void TBidirectionalState::ResolveWeakTypesW4W5W6(TReorderContext& aContext)
	{
	int i;
	TRunInfo* r;
	TCategory prev_cat = iPreviousCategory;
	TCategory next_cat = EOtherNeutral;

	// Phase P0b.
	prev_cat = iPreviousCategory;
	if (aContext.iCategories & EBoundaryNeutral)
		{
		for (i = 0, aContext.iCategories = iPreviousCategory | aContext.iNextCategory, r = aContext.iRunInfo;
			 i < aContext.iRuns;
			 i++, aContext.iCategories |= r->iCategory, r++)
			{
			if (r->iCategory == EBoundaryNeutral)		// runs of boundary neutrals change to EN, ET or AN if adjacent to
				{										// one of these, otherwise to ON
				int end = i + 1;
				while (end < aContext.iRuns && aContext.iRunInfo[end].iCategory == EBoundaryNeutral)
					end++;
				next_cat = end < aContext.iRuns ? (TCategory)(aContext.iRunInfo[end].iCategory) : aContext.iNextCategory;
				TCategory c = EOtherNeutral;
				if (prev_cat == EEuropeanNumber || next_cat == EEuropeanNumber)
					c = EEuropeanNumber;
				else if (prev_cat == EEuropeanNumberTerminator || next_cat == EEuropeanNumberTerminator)
					c = EEuropeanNumberTerminator;
				else if (prev_cat == EArabicNumber || next_cat == EArabicNumber)
					c = EArabicNumber;
				for (int j = i; j < end; j++)
					aContext.iRunInfo[j].iCategory = c;
				i = end - 1;
				r = &aContext.iRunInfo[i];
				}
			prev_cat = (TCategory)r->iCategory;
			}
		}

	// Phase P1.
	prev_cat = iPreviousCategory;
	if (aContext.iCategories & (EEuropeanNumberSeparator | ECommonNumberSeparator))
		{
		for (i = 0, aContext.iCategories = iPreviousCategory | aContext.iNextCategory, r = aContext.iRunInfo;
			 i < aContext.iRuns;
			 i++, aContext.iCategories |= r->iCategory, r++)
			{
			next_cat = i < aContext.iRuns - 1 ? (TCategory)(r[1].iCategory) : aContext.iNextCategory;
			switch (r->iCategory)
				{
				case EEuropeanNumberSeparator:			// European separators change to EN if between two ENs, else to ON
					if (prev_cat == EEuropeanNumber && next_cat == EEuropeanNumber)
						r->iCategory = EEuropeanNumber;
					else
						r->iCategory = EOtherNeutral;
					break;
				case ECommonNumberSeparator:			// CSs change to EN or AN if between two of the same, else to ON
					if (prev_cat == EEuropeanNumber && next_cat == EEuropeanNumber)
						r->iCategory = EEuropeanNumber;
					else if (prev_cat == EArabicNumber && next_cat == EArabicNumber)
						r->iCategory = EArabicNumber;
					else
						r->iCategory = EOtherNeutral;
					break;
				default:
					break;
				}
			prev_cat = (TCategory)r->iCategory;
			}
		}

	/*
	Phase L1: tabs, whitespace before tabs, and trailing whitespace, is set to the base embedding level.
	We ought to do this just before the final reordering, but the whitespace and segment separator
	categories have disappeared by then so we use the sentinel value 255 which tells
	ResolveImplicitLevels what to do.
	*/
	TBool demote_whitespace = TRUE;
	for (i = aContext.iRuns - 1, r = &aContext.iRunInfo[i]; i >= 0; i--, r--)
		{
		switch (r->iCategory)
			{
			case EWhitespace:
				break;
			case ESegmentSeparator:
				demote_whitespace = TRUE;
				break;
			default:
				demote_whitespace = FALSE;
				break;
			}
		if (demote_whitespace)
			r->iEmbeddingLevel = 255;
		}

	// Phases P2 and P3.
	prev_cat = iPreviousCategory;
	if (aContext.iCategories & (EEuropeanNumberTerminator | ESegmentSeparator | EWhitespace))
		{
		for (i = 0, aContext.iCategories = iPreviousCategory | aContext.iNextCategory, r = aContext.iRunInfo;
			 i < aContext.iRuns;
			 i++, aContext.iCategories |= r->iCategory, r++)
			{
			next_cat = i < aContext.iRuns - 1 ? (TCategory)(r[1].iCategory) : aContext.iNextCategory;
			switch (r->iCategory)
				{
				case EEuropeanNumberTerminator:			// runs of ETs change to ENs if next to an EN, else to ON
					{
					int end = i + 1;
					while (end < aContext.iRuns && aContext.iRunInfo[end].iCategory == EEuropeanNumberTerminator)
						end++;
					next_cat = end < aContext.iRuns ? (TCategory)(aContext.iRunInfo[end].iCategory) : aContext.iNextCategory;
					TCategory c = EOtherNeutral;
					if (prev_cat == EEuropeanNumber || next_cat == EEuropeanNumber)
						c = EEuropeanNumber;
					for (int j = i; j < end; j++)
						aContext.iRunInfo[j].iCategory = c;
					i = end - 1;
					r = &aContext.iRunInfo[i];
					}
					break;
				case ESegmentSeparator:					// S and WS change to ON
				case EWhitespace:
					r->iCategory = EOtherNeutral;
					break;
				default:
					break;
				}
			prev_cat = (TCategory)r->iCategory;
			}
		}
	}

void TBidirectionalState::ResolveWeakTypesW7(TReorderContext& aContext)
	{
	if (!(aContext.iCategories & EEuropeanNumber))
		return;

	TCategory prev_strong_cat = iPreviousStrongCategory;

	aContext.iCategories = iPreviousCategory | aContext.iNextCategory;
	TRunInfo* endOfRuns = aContext.iRunInfo + aContext.iRuns;
	for (TRunInfo* r = aContext.iRunInfo; r != endOfRuns; r++)
		{
		switch (r->iCategory)
			{
			case ELeftToRight:
				prev_strong_cat = ELeftToRight;
				break;
			case ERightToLeft:
				prev_strong_cat = ERightToLeft;
				break;
			case EEuropeanNumber: 
				if (prev_strong_cat == ELeftToRight)
					r->iCategory = ELeftToRight;
				break;
			default:
				break;
			}
		aContext.iCategories |= r->iCategory;
		}
	}



void TBidirectionalState::DeneutralizeRuns(TRunInfo* aStart, TRunInfo* aEnd,
	TCategory aStartCategory, TCategory aEndCategory)
/**
Turn all ON (Other Neutral) into non-neutrals according to the rules N1 and N2.
@param aStart The start of the run array to be altered.
@param aEnd One past the end of the run array to be altered.
@param aStartCategory
	The last non-neutral before the run, must be ELeftToRight or ERightToLeft.
@param aEndCategory
	The first non-neutral after the run, must be ELeftToRight or ERightToLeft.
@internalComponent
*/	{
	// if sandwiched by the same category, neutrals become that.
	if (aStartCategory == aEndCategory)
		{
		for (; aStart != aEnd; ++aStart)
			aStart->iCategory = aStartCategory;
		return;
		}
	// otherwise look at the embedding level in each case
	for (; aStart != aEnd; ++aStart)
		{
		aStart->iCategory = aStart->iEmbeddingLevel & 1?
			ERightToLeft : ELeftToRight;
		}
	}


void TBidirectionalState::ResolveNeutralTypes(TReorderContext& aContext)
	/**
This phase removes the ON (Other Neutral) category, leaving only L, R, EN, and
AN; no need to update aContext.iCategories.
@internalComponent
*/
    {
	// Really we should find if any number intervenes, but this would require
	// a BC break.
	TCategory prevNonNeutral = iPreviousStrongCategory;
	if (prevNonNeutral & ELeftToRightGroup)
		prevNonNeutral = ELeftToRight;
	else if (prevNonNeutral & ERightToLeftGroup)
		prevNonNeutral = ERightToLeft;
	TRunInfo *prevNonNeutralRun = aContext.iRunInfo;	// one past the last non-neutral found
	TRunInfo *endOfRuns = aContext.iRunInfo + aContext.iRuns;

	// All neutrals have now been changed to ON; change them to L or R depending on context.
	for (TRunInfo *p = aContext.iRunInfo; p != endOfRuns; ++p)
		{
		TCategory nonNeutral = EOtherNeutral;
		switch (p->iCategory)
			{
			case ELeftToRight:
				nonNeutral = ELeftToRight;
				break;
			case ERightToLeft:
				nonNeutral = ERightToLeft;
				break;
			case EArabicNumber:
			case EEuropeanNumber: 
				nonNeutral = ERightToLeft;
				break;
			default:
				break;
			}
		if (nonNeutral != EOtherNeutral)
			{
			if (p != prevNonNeutralRun)
				DeneutralizeRuns(prevNonNeutralRun, p,
					prevNonNeutral, nonNeutral);
			prevNonNeutral = nonNeutral;
			prevNonNeutralRun = p + 1;
			}
		}
	DeneutralizeRuns(prevNonNeutralRun, endOfRuns, prevNonNeutral,
		aContext.iNextStrongCategory);
	}


void TBidirectionalState::ResolveImplicitLevels(TReorderContext& aContext)
/**
Phases I1 and I2.
@internalComponent
*/	{
	int i;
	TRunInfo* r;
	for (i = 0, r = aContext.iRunInfo; i < aContext.iRuns; i++, r++)
		{
		if (r->iEmbeddingLevel == 255) // sentinel indicating this is a tab or segment-final whitespace
			r->iEmbeddingLevel = iStack[0].iEmbeddingLevel;
		else switch (r->iCategory)
			{
			case ELeftToRight:
				if (r->iEmbeddingLevel & 1)
					r->iEmbeddingLevel++;
				break;
			case ERightToLeft:
				if (!(r->iEmbeddingLevel & 1))
					r->iEmbeddingLevel++;
				break;
			case EEuropeanNumber: case EArabicNumber:
				if (r->iEmbeddingLevel & 1)
					r->iEmbeddingLevel++;
				else
					r->iEmbeddingLevel += 2;
				break;
			default:
				break;
			}
		}
	}


void TBidirectionalState::ReorderRuns(TReorderContext& aContext)
/**
Phase L2.
@internalComponent
*/	{
	// Find the highest level and lowest odd level.
	int i;
	TRunInfo* r;
	int highest = 0;
	int lowest_odd = EMaxLevel;
	int level = 0;
	for (i = 0, r = aContext.iRunInfo; i < aContext.iRuns; i++, r++)
		{
		level = r->iEmbeddingLevel;
		if (level > highest)
			highest = level;
		if ((level & 1) && level < lowest_odd)
			lowest_odd = level;
		}

	// From the highest level to the lowest odd level, reverse any run at that level or higher.
	for (level = highest; level >= lowest_odd; level--)
		{
		int run_start = 0;
		r = aContext.iRunInfo;
		while (run_start < aContext.iRuns)
			{
			while (run_start < aContext.iRuns && r->iEmbeddingLevel < level)
				{
				run_start++;
				r++;
				}
			int run_end = run_start;
			while (run_end < aContext.iRuns && r->iEmbeddingLevel >= level)
				{
				r->iDirection = !r->iDirection;
				run_end++;
				r++;
				}
			TRunInfo* p = &aContext.iRunInfo[run_start];
			TRunInfo* q = &aContext.iRunInfo[run_end - 1];
			while (p < q)
				{
				TRunInfo temp = *p;
				*p = *q;
				*q = temp;
				p++;
				q--;
				}
			run_start = run_end;
			}
		}
	}


TBidirectionalState::TCategory TBidirectionalState::Push(TCategory aStartCategory)
/** @internalComponent */
	{
	TInt rightToLeftFlag = (static_cast<TInt>(aStartCategory)
		& ERightToLeftGroup)? 1 : 0;
	TInt oldLevel = State().iEmbeddingLevel;
	TInt newLevel = oldLevel + 1;
	// And add an extra one if the bottom bit is not correct.
	newLevel += (newLevel & 1) ^ rightToLeftFlag;

	if (EMaxExplicitLevel < newLevel)
		{
		if (oldLevel == 60)
			++iPushesBeyond60;
		else
			++iPushesBeyond61;
		return EBoundaryNeutral;
		}

	++iStackLevel;
	TStackItem& state = iStack[iStackLevel];
	state.iEmbeddingLevel = static_cast<TUint8>(newLevel);
	state.iOverrideState = static_cast<TOverrideState>(aStartCategory
		& (ELeftToRightOverride | ERightToLeftOverride));
	state.iStartCategory = aStartCategory;

	return rightToLeftFlag? ERightToLeft : ELeftToRight;
	}


TBidirectionalState::TCategory TBidirectionalState::Pop()
/** @internalComponent */	
   {
	__ASSERT_DEBUG(0 < iStackLevel, User::Invariant());
	TInt level = State().iEmbeddingLevel;
	if (level < 60)
		--iStackLevel;
	else if (iPushesBeyond61 != 0)
		--iPushesBeyond61;
	else if (level == 61)
		--iStackLevel;
	else if (iPushesBeyond60)
		--iPushesBeyond60;
	else
		--iStackLevel;
	return (level & 1)? ERightToLeft : ELeftToRight;
	}


EXPORT_C void TBidirectionalState::Reset()
/** Sets the object to its default 'start of paragraph' state. */
	{
	iStackLevel = 0;
	iPushesBeyond60 = 0;
	iPushesBeyond61 = 0;
	iStack[0].iEmbeddingLevel = 0;
	iStack[0].iOverrideState = ENoOverrideState;
	iStack[0].iStartCategory = EOtherNeutral;
	iPreviousCategory = ELeftToRight;
	iPreviousStrongCategory = ELeftToRight;
	}


EXPORT_C TBool TBidirectionalState::IsDefault() const
/** Returns Gets the default 'start of paragraph' state.

@return ETrue if the object is in its default 'start of paragraph' state. */
	{
	return iStackLevel == 0 &&
		   iStack[0].iEmbeddingLevel == 0 &&
		   iStack[0].iOverrideState == ENoOverrideState &&
		   iStack[0].iStartCategory == EOtherNeutral &&
		   iPreviousCategory == ELeftToRight &&
		   iPreviousStrongCategory == ELeftToRight;
	}


EXPORT_C TBool TBidirectionalState::operator==(const TBidirectionalState& aState) const
/** Return ETrue if two bidirectional states are identical.

@param aState A bidirectional state.
@return ETrue if two bidirectional states are identical. */
	{
	if (iPreviousCategory != aState.iPreviousCategory ||
		iPreviousStrongCategory != aState.iPreviousStrongCategory ||
		iStackLevel != aState.iStackLevel)
		return FALSE;
	const TStackItem* p = iStack;
	const TStackItem* q = aState.iStack;
	for (int i = 0; i <= iStackLevel; i++, p++, q++)
		{
		if (p->iStartCategory != q->iStartCategory ||
			p->iOverrideState != q->iOverrideState ||
			p->iEmbeddingLevel != q->iEmbeddingLevel)
			return FALSE;
		}
	return TRUE;
	}


TInt TBidirectionalState::CatToNumber(TInt aCat)
/**
Finds the highest bit set in the input. Used to convert
TBidirectionalState::TCategory into TChar::TBdCategory.
@param aCat a TBidirectionalState::TCategory.
@return The equivalent TChar::TBdCategory.
@internalComponent
*/	{
	TInt shifts = 0;
	TInt bits = 32;
	TInt mask = ~0L;
	while (bits != 0)
		{
		bits >>= 1;
		mask <<= bits;
		if ((aCat & mask) == 0)
			{
			aCat <<= bits;
			shifts += bits;
			}
		}
	return 31 - shifts;
	}


EXPORT_C void TBidirectionalState::ExternalizeL(RWriteStream& aDest)
/** Serializes a bidirectional state to an output stream.

@param aDest An output stream. */
	{
	//+ put the prev cat, prev strong cat and stack levels in one number?
	// Write the previous category and previous strong category.
	aDest.WriteInt8L(CatToNumber(iPreviousCategory));
	aDest.WriteInt8L(CatToNumber(iPreviousStrongCategory));

	// Write the number of stack levels
	aDest.WriteInt8L(iStackLevel);

	/*
	Write each stack level as a single number: 5 bits for the start category, 2 for the override state,
	6 for the embedding level.
	*/
	for (int i = 0; i <= iStackLevel; i++)
		{
		TInt x = CatToNumber(iStack[i].iStartCategory);
		if (iStack[i].iOverrideState == ELeftToRightOverrideState)
			{
			x |= (KBidirectionalStateOverrideStreamValueLeftToRight << 5);	
			}
        else if (iStack[i].iOverrideState == ERightToLeftOverrideState)
        	{
        	x |= (KBidirectionalStateOverrideStreamValueRightToLeft << 5); 	
        	}
       	x |= ((TInt)iStack[i].iEmbeddingLevel << 7);
		aDest.WriteInt16L(x);
		}

	TInt level = State().iEmbeddingLevel;
	if (60 <= level)
		{
		aDest.WriteInt8L(iPushesBeyond60);
		aDest.WriteInt8L(iPushesBeyond61);
		}
	}


EXPORT_C void TBidirectionalState::InternalizeL(RReadStream& aSource)
/** Reads a bidirectional state from an input stream, translating it from its serialized 
form.

@param aSource A source stream. */
	{
	// Read the previous category and the previous strong category.
	TInt x = aSource.ReadInt8L();
	iPreviousCategory = (TCategory)(1 << x);
	x = aSource.ReadInt8L();
	iPreviousStrongCategory = (TCategory)(1 << x);

	// Read the number of stack levels.
	iStackLevel = aSource.ReadInt8L();

	// Read the stack levels.
	for (int i = 0; i <= iStackLevel; i++)
		{
		x = aSource.ReadInt16L();
		iStack[i].iStartCategory = (TCategory)(1 << (x & 0x1F));
		switch ((x >> 5) & 3)
        	{
    	case KBidirectionalStateOverrideStreamValueLeftToRight: 
    		iStack[i].iOverrideState = ELeftToRightOverrideState;
    		break;
    	case KBidirectionalStateOverrideStreamValueRightToLeft:
    		iStack[i].iOverrideState = ERightToLeftOverrideState; 
    		break;
    	case KBidirectionalStateOverrideStreamValueNone:
    	default: iStack[i].iOverrideState = ENoOverrideState; break;
        	};
		iStack[i].iEmbeddingLevel = (TUint8)(x >> 7);
		}

	TInt level = State().iEmbeddingLevel;
	if (60 <= level)
		{
		iPushesBeyond60 = aSource.ReadInt8L();
		iPushesBeyond61 = aSource.ReadInt8L();
		}
	else
		{
		iPushesBeyond60 = 0;
		iPushesBeyond61 = 0;
		}
	}


TBidirectionalState::TBidirectionalState(TChar::TBdCategory aPrevCat,
	TChar::TBdCategory aPrevStrongCat,
	TBool aParRightToLeft)
	/**
Constructor suitable for test code.
@internalComponent
*/
    {
	Reset();
	iPreviousCategory = CharToBdCat(aPrevCat);
	iPreviousStrongCategory = CharToBdCat(aPrevStrongCat);
	iStack[0].iEmbeddingLevel = (TUint8) (aParRightToLeft? 1 : 0);
	}
