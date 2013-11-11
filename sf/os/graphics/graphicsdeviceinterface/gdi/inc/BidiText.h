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

#ifndef BIDITEXT_H_
#define BIDITEXT_H_

#include <e32std.h>
#include <gdi.h>
#include <bidi.h> // class TBidirectionalState::TRunInfo
enum CGraphicsContext::TTextAlign;
class MLineBreaker;
class RRunInfoArray;
class CBidiTextTls;


/**
This enumeration holds the possible panic codes that may be raised 
by the BidiText API on detecting an unrecoverable error.
@publishedAll
@released
*/
enum TBidiPanic
	{
	/** The call to RRunInfoArray::OpenL() has not been made prior to this call to TBidiText::SetText()
	@see TBidiText::SetText	*/
	EBidiPanic_RunArrayNull						= 0,
	
	/** Maximum number of lines that the text will need to be split into is invalid i.e less than zero or negative.
	@see TBidiText::NewL
	@see TBidiText::MinimumSize	*/
	EBidiPanic_InvalidMaxline					= 1,
	
	/** Maximum number of characters are invalid i.e less than zero or negative.
	@see TBidiText::NewL */
	EBidiPanic_InvalidReservedMaxLength			= 2,
	
	/** Maximum width of the text in pixels is invalid i.e zero or negative.
	@see TBidiText::MinimumSize */
	EBidiPanic_InvalidWrappingWidth				= 3,
	
	/** Number of empty pixels between two lines of text are invalid i.e zero or negative.
	@see TBidiText::MinimumSize */
	EBidiPanic_InvalidLineGap					= 4,
	
	/** Text length for display is invalid i.e zero or negative.
	@see TBidiText::DisplayText
	@see TBidiText::LineOfDisplayText */
	EBidiPanic_InvalidVisualOrderedTextLength	= 5,
	
	/** Line number to retrieve is invalid i.e zero or negative. 
	@see TBidiText::LineOfDisplayText */
	EBidiPanic_InvalidLineNumber				= 6
	};

/** 
This class works as a replacement for HBufC in those cases where a small 
amount of text is being formatted simply. The text must be all of one style 
and broken into lines of all the same length. The number of lines and the text 
itself is specified on construction, and the text may then be re-broken, 
re-truncated and re-drawn with non-leaving functions. 
@publishedAll
@released
*/
class TBidiText
	{
public:
	/** Overall directionality of the text. */
	enum TDirectionality
		{
		/** Text reading begins on the left. */
		ELeftToRight = 0,
		/** Text reading begins on the right. */
		ERightToLeft = 1
		};

	/** Class specific panic codes 
	@deprecated This enum has been replaced by TBidiPanic.
    */
	enum TPanicCodes
		{
		EPanicRunArrayNull,
		};

	IMPORT_C static TDirectionality ScriptDirectionality(TLanguage aLanguage);
	IMPORT_C static TDirectionality TextDirectionality(
		const TDesC& aText, TBool* aFound = 0);
protected:
	TBidiText();
public:
	IMPORT_C static TBidiText* NewL(TInt aReservedMaxLength, TInt aMaxLines);
	IMPORT_C static TBidiText* NewL(const TDesC& aText, TInt aMaxLines);
	IMPORT_C static TBidiText* NewL(const TDesC& aText, TInt aMaxLines,
		TDirectionality aDirectionality);
	IMPORT_C TInt SetText(const TDesC& aText, RRunInfoArray& aRunInfoArray);
	IMPORT_C TInt SetText(const TDesC& aText, TDirectionality aDirectionality, RRunInfoArray& aRunInfoArray);
	IMPORT_C void SetTruncationChar(TChar aTruncateWith);
	IMPORT_C void WrapText(TInt aWrappingWidth, const CFont& aFont,
		const MLineBreaker* aBreaker, TInt aMaxLines);
	IMPORT_C void WrapText(TInt aWrappingWidth, const CFont& aFont,
		const MLineBreaker* aBreaker = NULL);
	IMPORT_C TSize MinimumSize(TInt aWrappingWidth, const CFont& aFont, TInt aLineGap, TInt aMaxLines = -1,
		const MLineBreaker* aBreaker = NULL) const;	
	IMPORT_C TPtrC Text() const;
	IMPORT_C TPtrC DisplayText() const;
	IMPORT_C TInt WrappingWidth() const;
	IMPORT_C TDirectionality Directionality() const;
	IMPORT_C TChar TruncationChar() const;
	IMPORT_C TInt NumberOfLinesInDisplayText() const;
	IMPORT_C TPtrC LineOfDisplayText(TInt aLine, TInt& aWidthInPixels) const;
	IMPORT_C void DrawText(CGraphicsContext& aGc, const TPoint& aLeft) const;
	IMPORT_C void DrawText(CGraphicsContext& aGc,
		const TPoint& aLeft, TInt aBaseLineSpacing,
		CGraphicsContext::TTextAlign aAlignment) const;
	IMPORT_C void DrawText(CGraphicsContext& aGc,
		const TPoint& aLeft, TInt aBaseLineSpacing) const;
private:
	TInt DoWrapText(TInt aWrappingWidth, const CFont& aFont, const MLineBreaker* aBreaker, 
		TInt aMaxLines, TText*& aOutputText, TInt& aNumLines, TInt16* aLineWidthArray) const;
	};


/**
This class is used to keep track of the shared run info array. 
Keep it open for as long as the TBidiText object is used. The same
RRunInfoArray object can be used with several different TBidiText objects.
@publishedAll
@released
*/
class RRunInfoArray
	{
	friend class TBidiText;

public:	
	IMPORT_C RRunInfoArray();
	IMPORT_C void OpenL();
	IMPORT_C void Close();

private:
	TBidirectionalState::TRunInfo* RunArray() const;

private:
	CBidiTextTls* iTls;
	};

#endif
