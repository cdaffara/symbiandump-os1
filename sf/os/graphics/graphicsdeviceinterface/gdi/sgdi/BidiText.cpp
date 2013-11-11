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

#include <e32svr.h>
#include "BidiTextImp.h"
#include "BidiCopy.h"
#include "BidiCompact.h"
#include <bidi.h>
#include <e32base.h>
#include <gdi.h>
#include <linebreak.h>

_LIT(KBidiPanicCategory,"Bidi");
static const TInt KLineSeparator = 0x2028;
static const TInt KParagraphSeparator = 0x2029;
static const TInt KCodeCR = 0x000D;
static const TInt KCodeLF = 0x000A;
static const TInt KCodeEllipsis = 0x2026;

void DeleteTRunInfo(void* aRunInfo)
	{
	delete[] reinterpret_cast<TBidirectionalState::TRunInfo*>(aRunInfo);
	}

void BidiPanic(TInt aError)
	{
	User::Panic(KBidiPanicCategory, aError);
	}


// One page-full of TRunInfos
const TInt KMaxRunInfoArraySize = 4*1024 / sizeof(TBidirectionalState::TRunInfo);
const TInt KBidiTlsHandle = 0x101F633D;


/*
* Ref-counted TLS for the shared run info array used by the SetText() method.
*/
NONSHARABLE_CLASS(CBidiTextTls) : public CObject
	{
public:	
	static CBidiTextTls* NewL();
	static CBidiTextTls* GetTls();
	~CBidiTextTls();
	inline TUint MaxArraySize();
	inline TBidirectionalState::TRunInfo* RunArray();

private:
	CBidiTextTls();
	void ConstructL(TUint aMaxArraySize);

private:
	TBidirectionalState::TRunInfo* iRunInfoArray;
	TUint iMaxArraySize;	
	};



CBidiTextTls::CBidiTextTls()
:	iRunInfoArray(NULL),
 	iMaxArraySize(0)
	{
	}


CBidiTextTls::~CBidiTextTls()
	{
	UserSvr::DllFreeTls(KBidiTlsHandle);
	
	if (iRunInfoArray)
		{
		delete [] iRunInfoArray;	
		}
	}


TUint CBidiTextTls::MaxArraySize()
	{
	return iMaxArraySize;
	}
	

TBidirectionalState::TRunInfo* CBidiTextTls::RunArray()
	{
	return iRunInfoArray;
	}
	
	
/**
 * Helper function provided to simplify reading the TLS data and improve the
 * readability of the code 
 */
CBidiTextTls* CBidiTextTls::GetTls()
	{
	return reinterpret_cast<CBidiTextTls*>(UserSvr::DllTls(KBidiTlsHandle));
	}


CBidiTextTls* CBidiTextTls::NewL()
	{
	CBidiTextTls* self = new (ELeave) CBidiTextTls;
	CleanupClosePushL(*self);
	self->ConstructL(KMaxRunInfoArraySize);
	CleanupStack::Pop(self);
	return self;
	}


void CBidiTextTls::ConstructL(TUint aMaxArraySize)
	{
	iMaxArraySize = aMaxArraySize;
	iRunInfoArray = new (ELeave) TBidirectionalState::TRunInfo[aMaxArraySize];
	User::LeaveIfError(UserSvr::DllSetTls(KBidiTlsHandle, this));
	}


EXPORT_C RRunInfoArray::RRunInfoArray() 
:	iTls(NULL)
	{
	}
	

/** 
Creates the run array if necessary and increases the reference count on it.
RRunInfoArray::OpenL() must be called prior to calling TBidiText::SetText().
 */
EXPORT_C void RRunInfoArray::OpenL()
	{
	if(!iTls)
		{
		iTls = CBidiTextTls::GetTls();
		if(iTls)
			{
			iTls->Open();	// Increase ref count	
			}
		else	
			{
			iTls = CBidiTextTls::NewL();
			}
		}	
	}
	
	
/**
Decreases the reference count on the run array. The run array will be deleted
if the reference count reaches zero. The client application must ensure that
there is a matching call to Close() for every call to OpenL() or memory will
be leaked.
 */
EXPORT_C void RRunInfoArray::Close()
	{
	if(iTls)
		{
		iTls->Close();	
		iTls = NULL;
		}
	}
	

/**
@return Pointer to the run array buffer
@internalComponent
 */
TBidirectionalState::TRunInfo* RRunInfoArray::RunArray() const
	{
	return iTls ? iTls->RunArray() : NULL;
	}
	

/**
@return Number of bytes needed to hold the TBidiTextImp member variables, plus the 
		text data allocated off the end of the TBidiTextImp object.
@internalComponent
*/
TInt TBidiTextImp::RequiredBytes(TInt aLength, TInt aMaxLines, TInt aBdRunArraySize)		
	{
	// size of TBidiTextImp class
	TInt bytes = TBidiTextImp::AlignedSizeOf();
	// size of text for logical and visual orderings.
	// This includes aMaxLines - 1 line breaks with surrounding
	// zero-width joiners, and a truncation character (possibly
	// a surrogate pair) plus a zero-width joiner.
	bytes += sizeof(TText) * (aLength * 2 + aMaxLines * 3);
	// size of line length array
	bytes += sizeof(TInt16*) * aMaxLines;
	// alignment
	bytes = (bytes + 3) & 0xFFFFFFFC;
	// array of TRunInfoCompact
	bytes += sizeof(TRunInfoCompact) * aBdRunArraySize;
	
	return bytes;
	}

	
/**
@return A TBidiTextImp object of sufficient size to hold the amount of text data specified
		by the the arguments.
@param aLength The number of characters in the text.
@param aMaxLines The maximum number of lines 
@param aBdRunArraySize The size of the bidi run array. 
@internalComponent
*/
TBidiTextImp* TBidiTextImp::NewL(TInt aLength, TInt aMaxLines, TInt aBdRunArraySize)
	{
	const TInt bytes = RequiredBytes(aLength, aMaxLines, aBdRunArraySize);
	TInt8* mem = static_cast<TInt8*>(User::AllocL(bytes));
	TBidiTextImp* me = reinterpret_cast<TBidiTextImp*>(mem);

	me->iTextLengthAndFlags = aLength;
	me->iVisualOrderedTextLength = -1;
	me->iWrappingWidth = 0xFFFF;
	me->iBidiRunArrayLength = aBdRunArraySize;
	me->iLines = static_cast<TUint8>(aMaxLines);
	me->iTruncationCharPlane = 0;
	me->iTruncationChar16 = KCodeEllipsis;
	me->SetAllocatedTextDataBytes(bytes - TBidiTextImp::AlignedSizeOf() - (sizeof(TRunInfoCompact) * aBdRunArraySize));
	return me;
	}

/**
@return Position of logically-ordered text portion of the heap cell.
@internalComponent
*/
TText* TBidiTextImp::LogicalText()
	{
	return reinterpret_cast<TText*>(
		reinterpret_cast<TInt8*>(this)
		+ TBidiTextImp::AlignedSizeOf());
	}

/**
@return Position of visually-ordered text portion of the heap cell.
@internalComponent
*/
TText* TBidiTextImp::VisualText()
	{
	TInt bytes = TBidiTextImp::AlignedSizeOf();
	bytes += sizeof(TText) * TextLength();
	return reinterpret_cast<TText*>(
		reinterpret_cast<TInt8*>(this) + bytes);
	}

/**
Returns a pointer to the array containing the width in pixels of each and every line.
@return Position of the array of line widths portion of the heap cell.
@internalComponent
*/
TInt16* TBidiTextImp::LineWidthArray()
	{
	TInt bytes = TBidiTextImp::AlignedSizeOf();
	bytes += sizeof(TText) * (TextLength() * 2 + iLines + 2);
	return reinterpret_cast<TInt16*>(
		reinterpret_cast<TInt8*>(this) + bytes);
	}

/**
@return Position of the array of runs portion of the heap cell.
@internalComponent
*/
TRunInfoCompact* TBidiTextImp::BidiRunArray()
	{
	TInt bytes = TBidiTextImp::AlignedSizeOf();
	bytes += sizeof(TText) * (TextLength() * 2 + iLines + 2);
	bytes += sizeof(TInt16*) * iLines;
	bytes = (bytes + 3) & 0xFFFFFFFC;
	return reinterpret_cast<TRunInfoCompact*>(
		reinterpret_cast<TInt8*>(this) + bytes);
	}

/** 
Report if the current character is an explicit line break. Both
aText[0] and aText[1] must be part of the string.
@return Size of line break.
@internalComponent 
*/
TInt SizeLineBreak(const TText* aText, const TText* aTextEnd)
	{
	if (aText == aTextEnd )
		return 0;
		
	if (*aText == KLineSeparator || *aText == KParagraphSeparator
		|| *aText == KCodeLF)
		return 1;
	if (aText[0] == KCodeCR)
		{
		// first check for space before checking for LF
		if (aText+1 < aTextEnd )
			{
			return aText[1] == KCodeLF? 2 : 1;
			}
		else
			return 1;
		}
	return 0;
	}

/** 
Find the next line break character.
@internalComponent 
*/
const TText* FindEndOfThisLine(const TText* aStart, const TText* aEnd)
	{
	while (aStart != aEnd && *aStart != KLineSeparator
		&& *aStart != KParagraphSeparator && *aStart != KCodeLF
		&& *aStart != KCodeCR)
		++aStart;
	return aStart;
	}

/**
Count number of lines in text.
@internalComponent
*/
TInt NumberOfLines(const TText* aStart, const TText* aEnd)
	{
	TInt num = 0;
	while (aStart != aEnd)
		{
		aStart = FindEndOfThisLine(aStart, aEnd);
		aStart += SizeLineBreak(aStart, aEnd);
		++num;
		}
	return num;
	}

/** Returns the directionality of a given language.
@param aLanguage Language.
@return The directionality of the given language. */
EXPORT_C TBidiText::TDirectionality TBidiText::ScriptDirectionality(
	TLanguage aLanguage)
	{
	const TUint32 DirectionalityBitmap[] =
		{
		0,
		// Arabic, Farsi, Hebrew
		0x02040020,
		// Urdu
		0x40000000
		};
	TUint index = aLanguage;
	if (index < sizeof(DirectionalityBitmap) * 8)
		{
		index >>= 5;
		TInt bit = aLanguage & 31;
		return (DirectionalityBitmap[index] >> bit) & 1?
			ERightToLeft : ELeftToRight;
		}
	return ELeftToRight;
	}


/** Reports the implicit directionality of a piece of text.

@param aText The text to be examined. 
@param aFound If non-null, returns ETrue if there were any strongly directional 
characters and EFalse if there were none. If a piece of text is spread over 
several descriptors, They need to be queried in sequence until one returns 
ETrue in aFound. 
@return The directionality implicit in aText. 131 */ 
EXPORT_C TBidiText::TDirectionality TBidiText::TextDirectionality(
	const TDesC& aText, TBool* aFound)
	{
	return BidiCopy::ImplicitDirectionalityIsRightToLeft(
		aText.Ptr(), aText.Length(), aFound)?
		ERightToLeft : ELeftToRight;
	}

/** Creates a bidirectional text object with directionality determined by
aDirectionality. Use this for text that has come from user input.

@param aText The text in logical order.
@param aMaxLines
	The maximum number of lines that this text will need to be split into. Must
	be at least 1, but should not be too large, as each potential line takes an
	extra 8 bytes of memory.
@param aDirectionality Direction to use.
@return The newly constructed object.
 */
EXPORT_C TBidiText* TBidiText::NewL(const TDesC& aText, TInt aMaxLines,
	TBidiText::TDirectionality aDirectionality)
	{
	__ASSERT_ALWAYS(0 < aMaxLines, BidiPanic(EBidiPanic_InvalidMaxline));
	const TText* text = aText.Ptr();
	const TInt length = aText.Length();
	TInt linesInOriginalText = NumberOfLines(text, text + length);
	if (aMaxLines < linesInOriginalText)
		aMaxLines = linesInOriginalText;

	const TInt arraySize = TBidirectionalState::GenerateBdRunArray(text, length, 0, 0);
	TBidirectionalState::TRunInfo* runInfoArray = new(ELeave) TBidirectionalState::TRunInfo[arraySize];
	TCleanupItem ci(DeleteTRunInfo, runInfoArray);
	CleanupStack::PushL(ci);
	TBidirectionalState::GenerateBdRunArray(text, length, runInfoArray, arraySize);
	TBidirectionalState state;
	state.ReorderLine(runInfoArray, arraySize, ETrue, ETrue, aDirectionality,
		TChar::EOtherNeutral, TChar::EOtherNeutral);
	TInt compactArraySize = TRunInfoCompact::Convert(0, aText, runInfoArray, arraySize);

	// size of TBidiTextImp class
	TBidiTextImp* me = TBidiTextImp::NewL(length, aMaxLines, compactArraySize);
	me->SetRightToLeftDirectionality(aDirectionality != ELeftToRight);
		
	TRunInfoCompact::Convert(me->BidiRunArray(), aText, runInfoArray, arraySize);
	CleanupStack::PopAndDestroy(runInfoArray);

	Mem::Copy(me->LogicalText(), text, length * sizeof(TText));
	return me;
	}

/** Creates a bidirectional text object with directionality determined by the text 
itself. Use this for text that has been obtained from a resource file. 

@param aText The text in logical order.
@param aMaxLines The maximum number of lines that this text will need to be 
split into. Must be at least 1, but should not be too large, as each potential 
line takes an extra 8 bytes of memory.
@return The newly constructed object. */
EXPORT_C TBidiText* TBidiText::NewL(const TDesC& aText, TInt aMaxLines)
	{
	return NewL(aText, aMaxLines, TextDirectionality(aText));
	}


/** Creates a bidirectional text object with enough room for up to aReservedMaxLength
number of characters. The number of characters that will actually fit (when calling 
SetText()) might be slightly less than aReservedMaxLength, as each change between a 
left-to-right and a right-to-left sub-string (and the other way around) needs about
two characters worth of memory.

@param aReservedMaxLength The maximum number of characters.
@param aMaxLines The maximum number of lines that this text will need to be 
split into. Must be at least 1, but should not be too large, as each potential 
line takes an extra 8 bytes of memory.
@return The newly constructed object. */
EXPORT_C TBidiText* TBidiText::NewL(TInt aReservedMaxLength, TInt aMaxLines)
	{
	__ASSERT_ALWAYS(0 < aReservedMaxLength, BidiPanic(EBidiPanic_InvalidReservedMaxLength));
	__ASSERT_ALWAYS(0 < aMaxLines,          BidiPanic(EBidiPanic_InvalidMaxline));
	
	const TInt compactArraySize = 1;	// Always at least one needed
	
	TBidiTextImp* me = TBidiTextImp::NewL(aReservedMaxLength, aMaxLines, compactArraySize);
	me->SetTextLength(0);	// no text yet, just reserved memory
	return me;
	}

/** Sets the text of the bidirectional text object with directionality determined 
by the text itself. Use this for text that has been obtained from a resource file. 

@param aText The text in logical order.
@return The number of characters that didn't fit in the available buffer.
*/
EXPORT_C TInt TBidiText::SetText(const TDesC& aText, RRunInfoArray& aRunInfoArray)
	{
	return SetText(aText, TextDirectionality(aText), aRunInfoArray);
	}


/** Sets the text of the bidirectional text with directionality determined by
aDirectionality. Use this for text that has come from user input.

@param aText The text in logical order.
@param aDirectionality Direction to use.
@return The number of characters that didn't fit in the available buffer.
@panic Bidi EBidiPanic_RunArrayNull The call to RRunInfoArray::OpenL() has not
been made prior to this call to TBidiText::SetText()
*/
EXPORT_C TInt TBidiText::SetText(const TDesC& aText,
								 TDirectionality aDirectionality, 
								 RRunInfoArray& aRunInfoArray)
	{
	TBidirectionalState::TRunInfo* const runArray = aRunInfoArray.RunArray();
	__ASSERT_ALWAYS(runArray, BidiPanic(EBidiPanic_RunArrayNull));
	
	TBidiTextImp* me = TBidiTextImp::Imp(this);
	const TInt maxLines = me->iLines;	
	const TText* text = aText.Ptr();
	TInt length = aText.Length();
	
	TInt requiredArraySize = TBidirectionalState::GenerateBdRunArray(text, length, 0, 0);
	const TInt actualArraySize = aRunInfoArray.iTls->MaxArraySize();
		
	if (requiredArraySize > actualArraySize)
		{
		// Handle the case where we do not have enough space in the run array
		// to cope with the input text. The text will be truncated to ensure
		// we don't overrun the buffer and the number of excess characters 
		// returned as a negative number.
		requiredArraySize = actualArraySize;
		TBidirectionalState::GenerateBdRunArray(text, length, runArray, requiredArraySize);
		
		length = 0;
		for (TInt index = 0; index < requiredArraySize; index++)
			{
			length += runArray[index].iLength;
			}
		}
	else
		{
		TBidirectionalState::GenerateBdRunArray(text, length, runArray, requiredArraySize);
		}
	
	

	TBidirectionalState state;
	state.ReorderLine(runArray, 
					  requiredArraySize, 
					  ETrue, 
					  ETrue, 
					  aDirectionality, 
					  TChar::EOtherNeutral, 
					  TChar::EOtherNeutral);
	const TInt compactArraySize = TRunInfoCompact::Convert(0, 
														   aText, 
														   runArray, 
														   requiredArraySize);

	// Calculate number of bytes needed to keep text data
	TInt requiredBytes = sizeof(TText) * (length * 2 + maxLines * 3); // size of text for logical & visual orderings.
	requiredBytes += sizeof(TInt16*) * maxLines;					  // size of line length array
	requiredBytes = (requiredBytes + 3) & 0xFFFFFFFC;				  // alignment
	
	TInt textLength = length;
	const TInt excessData = Max(0, requiredBytes - me->AllocatedTextDataBytes());
	TInt excessChars = 0;
	if(excessData)
		{
		// Calculate how much text data that can be fitted into the available bytes, 
		// given the bytes needed for run array data
		excessChars = excessData / (sizeof(TText) * 2);
		textLength -= excessChars;
		}
	else if (aText.Length() > length)
		{
		excessChars = aText.Length() - length;
		}
	
	me->SetTextLength(textLength);
	me->SetRightToLeftDirectionality(aDirectionality != ELeftToRight);			
	me->iVisualOrderedTextLength = -1;
	me->iBidiRunArrayLength = static_cast<TUint16>(compactArraySize);

	TRunInfoCompact::Convert(me->BidiRunArray(), aText, runArray, requiredArraySize);
	Mem::Copy(me->LogicalText(), text, textLength * sizeof(TText));

	return excessChars;
	}

/** Sets the character that will be added at the end of the text if the whole text 
cannot fit into the space specified. 

@param aTruncateWith The truncation char. */
EXPORT_C void TBidiText::SetTruncationChar(TChar aTruncateWith)
	{
	TBidiTextImp* me = TBidiTextImp::Imp(this);
	me->iTruncationCharPlane = static_cast<TUint8>(aTruncateWith >> 16);
	me->iTruncationChar16 = static_cast<TUint16>(aTruncateWith);
	}

TInt RemoveTrailingSpaces(const MLineBreaker* aBreaker,
	const TText* aInput, TInt aMinPos, TInt aEndPos)
	{
	// Ignore space characters at the end of the line.
	// Don't bother to ignore spaces made of surrogate pairs:
	// more processing than it's worth.
	TUint dummy1, dummy2;
	while (aEndPos != aMinPos && MLineBreaker::ESpLineBreakClass
		== aBreaker->LineBreakClass(aInput[aEndPos - 1], dummy1, dummy2))
		{
		--aEndPos;
		}
	return aEndPos;
	}

/** Prepares the visually-ordered text according to the wrapping width and font
specified. Text cannot be drawn until this has been done.
@param aWrappingWidth
	The maximum width of the text in pixels. Note that this distance should be
	slightly less than the available width to allow for characters such as "W"
	which can have side-bearings that leak into the margins.
@param aFont The font that will provide the character metrics.
@param aBreaker
	An object for breaking the lines. May be NULL for default behaviour.
@param aMaxLines
	Number of lines to restrict wrapping to. The truncation character will be
	used if the text is too long for this number of lines. The number of lines
	wrapped to may not be greater than the figure passed to NewL.
*/
EXPORT_C void TBidiText::WrapText(TInt aWrappingWidth, const CFont& aFont,
	const MLineBreaker* aBreaker, TInt aMaxLines)
	{
	TBidiTextImp* me = TBidiTextImp::Imp(this);
	me->iWrappingWidth = aWrappingWidth;
	
	TInt16* lineWidths = me->LineWidthArray();
	TText* output = me->VisualText();
	
	TInt numLines = 0;
	DoWrapText(aWrappingWidth, aFont, aBreaker, aMaxLines, output, numLines, lineWidths);	
	me->iVisualOrderedTextLength = output - me->VisualText();
	}

/** Calculate the minimum size needed to draw the current text, given the specified
wrapping width, font, and line gap. Calling this method will not rewrap the object's
text.

@param aWrappingWidth
	The maximum width of the text in pixels. Note that this distance should be
	slightly less than the available width to allow for characters such as "W"
	which can have side-bearings that leak into the margins.
@param aFont The font that will provide the character metrics.
@param aLineGap The number of empty pixels between two lines of text. 
	Note that this is not the same as the baseline spacing, which is the font 
	height plus the line gap.
@param aMaxLines
	Number of lines to restrict wrapping to. The truncation character will be
	used if the text is too long for this number of lines. The number of lines
	wrapped to may be greater than the figure passed to NewL, and that figure
	will be used if the number of lines is specified as -1. If 0 (zero) is specified
	no limit is applied.
@param aBreaker
	An object for breaking the lines. May be NULL for default behaviour.
*/
EXPORT_C TSize TBidiText::MinimumSize(TInt aWrappingWidth, const CFont& aFont, TInt aLineGap, 
									TInt aMaxLines, const MLineBreaker* aBreaker) const
	{
	__ASSERT_ALWAYS(0  <= aWrappingWidth, BidiPanic(EBidiPanic_InvalidWrappingWidth));
	__ASSERT_ALWAYS(0  <=  aLineGap,       BidiPanic(EBidiPanic_InvalidLineGap));
	__ASSERT_ALWAYS(-1 <= aMaxLines,      BidiPanic(EBidiPanic_InvalidMaxline));

	TInt numLines = 0;
	TText* output = NULL;
	const TInt minWidth = DoWrapText(aWrappingWidth, 
									 aFont, 
									 aBreaker, 
									 (aMaxLines = 0 ? KMaxTInt : aMaxLines), 
									 output, 
									 numLines, 
									 NULL);
	const TInt minHeight = (aFont.FontMaxHeight() + aLineGap) * numLines - aLineGap; 
	return TSize(minWidth, minHeight);	
	}


TInt TBidiText::DoWrapText(TInt aWrappingWidth, const CFont& aFont, const MLineBreaker* aBreaker, 
	TInt aMaxLines, TText*& aOutputText, TInt& aNumLines, TInt16* aLineWidthArray) const
	{
	MLineBreaker defaultBreaker;
	if (!aBreaker)
		aBreaker = &defaultBreaker;
	
	const TBidiTextImp* me = TBidiTextImp::Imp(this);
	if (me->iLines < aMaxLines)
		aMaxLines = me->iLines;
	
	const TRunInfoCompact* runArray = me->BidiRunArray();	
	const TRunInfoCompact* runArrayEnd = runArray + me->iBidiRunArrayLength;
	
	const TText* input = me->LogicalText();
	const TInt inputLength = me->TextLength();
	TPtrC textDes(input, inputLength);
	const TText* output = me->VisualText();

	TRunInfoCompact::TReorderingContext context;
	context.iSource = input;
	context.iTruncation = 0xFFFF;
	context.iJoinsAtEnd = EFalse;
	
	TInt start = 0;
	CFont::TMeasureTextInput measureInput;
	measureInput.iMaxAdvance = aWrappingWidth;
	measureInput.iEndInputChar = FindEndOfThisLine(input, input + inputLength) - input;
	CFont::TMeasureTextOutput measureOutput;
	TBool truncated;

	TInt widestLineWidth = 0;
	TBool bLastLine = EFalse;
	for (aNumLines = 0; aNumLines != aMaxLines && start < inputLength; ++aNumLines)
		{
		truncated=EFalse;
		context.iJoinsAtStart = context.iJoinsAtEnd;
		if(aNumLines != 0 && aOutputText)
			*(aOutputText++) = KLineSeparator;
		
		measureInput.iStartInputChar = start;
		TInt advance = aFont.MeasureText(textDes, &measureInput, &measureOutput);
		TInt breakPos = measureOutput.iChars;
		TInt endOfLine = breakPos;
		// truncationCharWidth is the width of any truncation character on this
		// line only.
		TInt truncationCharWidth = 0;
		if (endOfLine == measureInput.iEndInputChar)
			{
			//handle the dangling lines here
			TInt sizeLineBreak = SizeLineBreak(input + endOfLine, input + inputLength);
			if((measureInput.iEndInputChar < inputLength - sizeLineBreak) && (aNumLines == aMaxLines - 1))
				bLastLine = ETrue;
			}
		else if (aNumLines == aMaxLines - 1)
			{
			bLastLine = ETrue;
			}
		else 
			{ // Not last line, so find a legal line break.
			aBreaker->GetLineBreak(textDes, 
								   start + 1, 
								   measureOutput.iChars, 
								   EFalse, 
								   0, 
								   breakPos, 
								   endOfLine);
			}

		if (bLastLine)
			{
			// Last line, so re-measure leaving enough room for
			// truncation character.
			context.iTruncation = me->TruncationChar();
			truncationCharWidth = aFont.CharWidthInPixels(context.iTruncation);
			measureInput.iMaxAdvance = aWrappingWidth - truncationCharWidth;
			advance = aFont.MeasureText(textDes, &measureInput, &measureOutput) + truncationCharWidth;
			breakPos = RemoveTrailingSpaces(aBreaker, input, start, measureOutput.iChars);
			truncated=ETrue;
			bLastLine = EFalse;
			}

		// if the break position has changed, we need to remeasure
		if (breakPos != measureOutput.iChars)
			{
			const TInt oldEnd = measureInput.iEndInputChar;
			measureInput.iEndInputChar = breakPos;
			advance = aFont.MeasureText(textDes, &measureInput, &measureOutput) + truncationCharWidth;
			measureInput.iEndInputChar = oldEnd;
			truncated=ETrue;
			}

		//width may be greater than advance
		advance = Max(advance,measureOutput.iBounds.Width());

		if(widestLineWidth < advance)
			widestLineWidth = advance;
		
		if(aLineWidthArray)
			*(aLineWidthArray++) = static_cast<TInt16>(advance);

		context.iStart = start;
		context.iEnd = breakPos;
		if (truncated)
			{
			context.iJoinsAtEnd = breakPos < inputLength?
				TRunInfoCompact::JoinBefore(input, breakPos) : EFalse;
			}
		else
			{
			context.iJoinsAtEnd = endOfLine < inputLength?
				TRunInfoCompact::JoinBefore(input, endOfLine) : EFalse;
			}
		if (aOutputText)
			{
			for (const TRunInfoCompact* p = runArray; p != runArrayEnd; ++p)
				aOutputText = p->Reorder(aOutputText, context);
			}
		// Set 'start' to the beginning of the next line...
		start = endOfLine;
		
		// ...move it past any line break...
		const TInt sizeOfLineBreak = SizeLineBreak(input + start, input + inputLength);
		if (sizeOfLineBreak != 0)
			{
			start += sizeOfLineBreak;
			// ...and find the end of this next line.
			const TText* endLine = FindEndOfThisLine(input + start, input + inputLength);
			measureInput.iEndInputChar = endLine - input;
			}
		}
		
	return widestLineWidth;	
	}


/** Prepares the visually-ordered text according to the wrapping width and font 
specified. Text cannot be drawn until this has been done. 

@param aWrappingWidth The maximum width of the text in pixels. Note that this 
distance should be slightly less than the available width to allow for characters 
such as "W" which can have side-bearings that leak into the margins.
@param aFont The font that will provide the character metrics.
@param aBreaker An object for breaking the lines. May be NULL for default behaviour. */
EXPORT_C void TBidiText::WrapText(TInt aWrappingWidth, const CFont& aFont,
	const MLineBreaker* aBreaker)
	{
	WrapText(aWrappingWidth, aFont, aBreaker, KMaxTInt);
	}

/** Returns the original logically-ordered text supplied in the constructor. 
@return The original logically-ordered text supplied in the constructor. */
EXPORT_C TPtrC TBidiText::Text() const
	{
	const TBidiTextImp* me = TBidiTextImp::Imp(this);
	const TText* text = me->LogicalText();
	return TPtrC(text, me->TextLength());
	}

/** Returns the text as prepared for display, provided that WrapText has been called. 
If WrapText has not been called, a panic will result. 

@return The text as prepared for display */
EXPORT_C TPtrC TBidiText::DisplayText() const
	{
	const TBidiTextImp* me = TBidiTextImp::Imp(this);
	__ASSERT_ALWAYS(me->iVisualOrderedTextLength >= 0, BidiPanic(EBidiPanic_InvalidVisualOrderedTextLength));
	const TText* text = me->VisualText();
	return TPtrC(text, me->iVisualOrderedTextLength);
	}

/** Returns the wrapping width previously supplied to WrapText. 

@return The wrapping. */
EXPORT_C TInt TBidiText::WrappingWidth() const
	{
	const TBidiTextImp* me = TBidiTextImp::Imp(this);
	return me->iWrappingWidth;
	}

/** Returns the directionality of the text. 

@return The directionality. */
EXPORT_C TBidiText::TDirectionality TBidiText::Directionality() const
	{
	const TBidiTextImp* me = TBidiTextImp::Imp(this);
	return me->HasRightToLeftDirectionality() ? ERightToLeft : ELeftToRight;
	}

/** Returns the truncation character used. 

@return The truncation character. */
EXPORT_C TChar TBidiText::TruncationChar() const
	{
	const TBidiTextImp* me = TBidiTextImp::Imp(this);
	return me->TruncationChar();
	}

/** Reports the number of lines in the text to be drawn.

WrapText must have been called already.
@return
	The number of lines in the text which would be drawn by DrawText.
*/
EXPORT_C TInt TBidiText::NumberOfLinesInDisplayText() const
	{
	const TBidiTextImp* me = TBidiTextImp::Imp(this);
	if (me->iVisualOrderedTextLength <0)
		{
		return 0;
		}
	const TText* text = me->VisualText();
	const TText* textEnd = text + me->iVisualOrderedTextLength;
	return NumberOfLines(text, textEnd);
	}

/** Returns the text as prepared for display, provided that WrapText has been called. 
If WrapText has not been called, a panic will result. 
@param aLine Line number to retrieve.
@param aWidth Returns the width in pixels of the line retrieved.
@return The text as prepared for display. */
EXPORT_C TPtrC TBidiText::LineOfDisplayText(TInt aLine, TInt& aWidthInPixels) const
	{
	const TBidiTextImp* me = TBidiTextImp::Imp(this);
	__ASSERT_ALWAYS(me->iVisualOrderedTextLength >= 0, BidiPanic(EBidiPanic_InvalidVisualOrderedTextLength));
	__ASSERT_ALWAYS(0 <= aLine && aLine < me->iLines, BidiPanic(EBidiPanic_InvalidLineNumber));
	aWidthInPixels = me->LineWidthArray()[aLine];
	const TText* text = me->VisualText();
	const TText* textEnd = text + me->iVisualOrderedTextLength;
	for (; aLine != 0; --aLine)
		{
		text = FindEndOfThisLine(text, textEnd);
		text += SizeLineBreak(text, textEnd);
		}
	const TText* endOfLine = FindEndOfThisLine(text, textEnd);
	return TPtrC(text, endOfLine - text);
	}

/** Draws all of the text. WrapText must have been called already. 

@param aGc The graphics context to draw the text to. The graphics context's 
font is assumed to have been set to the same font that was passed to the previous 
call to WrapText.
@param aLeft The left extreme of the baseline. Note that this should not be 
at the very edge of the available space, or characters such as "W" with left 
side bearings may be truncated.
@param aBaseLineSpacing The spacing between each line. If 0, only the first 
line is drawn.
@param aAlignment How to position the text horizontally. */
EXPORT_C void TBidiText::DrawText(CGraphicsContext& aGc, const TPoint& aLeft,
	TInt aBaseLineSpacing, CGraphicsContext::TTextAlign aAlignment) const
	{
	TPoint origin;
	origin.iY = aLeft.iY;
	TInt lines = aBaseLineSpacing == 0? 1 : NumberOfLinesInDisplayText();
	TInt wrappingWidth = WrappingWidth();
	for (TInt i = 0; i != lines; ++i)
		{
		TInt width;
		TPtrC textLine = LineOfDisplayText(i, width);
		origin.iX = aLeft.iX;
		if (aAlignment != CGraphicsContext::ELeft)
			{
			TInt excess = wrappingWidth - width;
			origin.iX += aAlignment != CGraphicsContext::ECenter?
				excess : excess >> 1;
			}
		aGc.DrawText(textLine, origin);
		origin.iY += aBaseLineSpacing;
		}
	}

/** Draws all of the text. Alignment is taken from the directionality of the text. 
WrapText must have been called already. 

@param aGc The graphics context to draw the text to. The graphics context's 
font is assumed to have been set to the same font that was passed to the previous 
call to WrapText.
@param aLeft The left extreme of the baseline. Note that this should not be 
at the very edge of the available space, or characters such as "W" with left 
side bearings may be truncated.
@param aBaseLineSpacing The spacing between each line. If 0, only the first 
line is drawn. */
EXPORT_C void TBidiText::DrawText(CGraphicsContext& aGc, const TPoint& aLeft,
	TInt aBaseLineSpacing) const
	{
	DrawText(aGc, aLeft, aBaseLineSpacing,
		Directionality() == ELeftToRight?
		CGraphicsContext::ELeft : CGraphicsContext::ERight);
	}

/** Draws the first line of the text. WrapText must have been called already. Alignment 
is taken from the directionality of the text. 

@param aGc The graphics context to draw the text to. The graphics context's 
font is assumed to have been set to the same font that was passed to the previous 
call to WrapText.
@param aLeft The left extreme of the baseline. Note that this should not be 
at the very edge of the available space, or characters such as "W" with left 
side bearings may be truncated. */
EXPORT_C void TBidiText::DrawText(CGraphicsContext& aGc, const TPoint& aLeft) const
	{
	DrawText(aGc, aLeft, 0);
	}

