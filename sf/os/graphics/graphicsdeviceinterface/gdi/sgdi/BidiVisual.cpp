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

#include "BidiVisual.h"
#include "BidiCopy.h"
#include "BidiTextImp.h"
#include "BidiCompact.h"

extern void BidiPanic(TInt aError);

/** Construct the class with a full paragraph of text and the run array to be used 
to hold all the runs. 

@param aText The text to be analyzed. This must not be de-allocated until 
all operations on it through this class have been completed.
@param aRightToLeft Paragraph directionality.
@param aRunInfoArray The array to be used to hold the run information. This 
must not be de-allocated until all operations on it through this class have 
been completed. Ownership is not passed.
@param aRunInfoLength The number of TRunInfo<\code> objects in aRunInfoArray. */
EXPORT_C TBidiLogicalToVisual::TBidiLogicalToVisual(
	const TDesC& aText,
	TBool aRightToLeft,
	TBidirectionalState::TRunInfo* aRunInfoArray,
	TInt aRunInfoLength)
	: iText(aText), iRightToLeft(aRightToLeft),
	iRunInfoArray(aRunInfoArray), iRunInfoArrayLength(aRunInfoLength), iRuns(aRunInfoLength)
	{
	}

/** Construct the class with a full paragraph of text and the run array to be used 
to hold all the runs. Directionality is taken as the implicit directionality 
of the text passed. 

@param aText The text to be analyzed. This must not be de-allocated until 
all operations on it through this class have been completed.
@param aRunInfoArray The array to be used to hold the run information. This 
must not be de-allocated until all operations on it through this class have 
been completed. Ownership is not passed.
@param aRunInfoLength The number of TRunInfo objects in aRunInfoArray. */
EXPORT_C TBidiLogicalToVisual::TBidiLogicalToVisual(
	const TDesC& aText,
	TBidirectionalState::TRunInfo* aRunInfoArray,
	TInt aRunInfoLength)
	: iText(aText),
	iRunInfoArray(aRunInfoArray), iRunInfoArrayLength(aRunInfoLength), iRuns(aRunInfoLength)
	{
	iRightToLeft = BidiCopy::ImplicitDirectionalityIsRightToLeft(
		aText.Ptr(), aText.Length(), 0);
	}

/** Process the text. This must be called before any call to GetVisualLine. 

@return The number of runs that were or would be required to reorder the complete 
line. If this is less than the value passed into the constructor as aRunInfoLength, 
the full text will not be able to be Reordered. */
EXPORT_C TInt TBidiLogicalToVisual::Reorder()
	{
	TInt required = TBidirectionalState::GenerateBdRunArray(
		iText.Ptr(), iText.Length(), iRunInfoArray, iRunInfoArrayLength);
	iRuns = Min(required, iRunInfoArrayLength);
	TBidirectionalState state;
	state.ReorderLine(iRunInfoArray, iRuns, ETrue, ETrue, iRightToLeft,
		TChar::EOtherNeutral, TChar::EOtherNeutral);
	return required;
	}

/** Get a line of visually ordered text. 

@param aLine Line of text in visual order. There must be aEnd - aStart + 4 
characters available in the buffer. Only characters within the range aStart 
to aEnd, plus perhaps a truncation character and up to two zero-width joiners 
will be present in the string at the end.
@param aStart The first character in the text that is to be on the line.
@param aEnd One plus the last character in the test that is to be on the line.
@param aTruncationChar The character to mark the end of the text, if appropriate, 
or 0xFFFF if no truncation character is to be used. */ 
EXPORT_C void TBidiLogicalToVisual::GetVisualLine(TDes& aLine,
	TInt aStart, TInt aEnd, TChar aTruncationChar)
	{
	__ASSERT_ALWAYS(aEnd - aStart + KMinCharAvailable <= aLine.MaxLength(), BidiPanic(KErrArgument));

	if(iText.Length() == 0)
		{
		aLine.Zero();
		return;//We can't do anything with iText, if its length is 0.
		}

	TRunInfoCompact::TReorderingContext context;

	const TText* input = &iText[0];
	context.iStart = aStart;
	context.iEnd = aEnd;
	context.iTruncation = aTruncationChar;
	context.iSource = input;
	context.iJoinsAtStart = TRunInfoCompact::JoinBefore(input, aStart);
	context.iJoinsAtEnd = aEnd < iText.Length() ? TRunInfoCompact::JoinBefore(input, aEnd) : EFalse;

	aLine.FillZ(1);
	TText* output = &aLine[0];
	TText* outputStart = output;
	TBidirectionalState::TRunInfo* endInfo = iRunInfoArray + iRuns;
	for (TBidirectionalState::TRunInfo* currentInfo = iRunInfoArray;
		currentInfo != endInfo; ++currentInfo)
		{
		TRunInfoCompact info(currentInfo->iStart,
			currentInfo->iLength, currentInfo->iDirection);
		output = info.Reorder(output, context);
		}
	aLine.SetLength(output - outputStart);
	}
