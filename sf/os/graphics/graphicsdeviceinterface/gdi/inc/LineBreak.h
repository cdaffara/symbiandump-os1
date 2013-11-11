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

#ifndef LINEBREAK_H_
#define LINEBREAK_H_

#include <e32def.h>
class MContingentLineBreaker;
class MContingentLineBreakerL;
class TDesC16;
class TUid;


/**
 This enumeration holds the possible panic codes that may be raised
 by the LineBreak API on detecting an unrecoverable error.
@publishedAll
@released
*/
enum TLineBreakPanic
	{
	/** Not used */
	ELineBreakPanic_Unknown				= 0,
	/** Internal error, error found in data table */
	ELineBreakPanic_BadTable			= 1,
	/** One or more of the input parameters to the interface were invalid */
	ELineBreakPanic_InvalidInputParam	= 2,
	/** Raised when an output parameter breaks an API condition */
	ELineBreakPanic_InvalidOutputParam	= 3,
	};



class MLineBreaker
/**
Customizes the Unicode line-breaking algorithm. 
@publishedAll
@released
*/
	{
public:
	/** The Unicode line breaking classes; see Unicode Technical Report 14.
	Not a named enumerated type, so that overriding classes may add new
	line breaking classes freely.
	The description of each constant gives the name of the line-breaking
	class, an example and a brief, imprecise description of the default
	behaviour of characters of that class.	 */
	enum
		{
		/** Opening Punctuation (e.g. '['). Breaking after prohibited. */
		EOpLineBreakClass,
		/** Closing Punctuation (e.g. ']'). Breaking before prohibited. */
		EClLineBreakClass,
		/** Ambiguous Quotes (e.g. '"'). Breaking before and after prohibited. */
		EQuLineBreakClass,
		/** Glue (e.g. Non-breaking space). Breaking before and after prohibited
		unless spaces are present. */
		EGlLineBreakClass,
		/** Non-Starter (e.g. small Japanese kana). Breaking before prohibited
		if no spaces present. */
		ENsLineBreakClass,
		/** Exclamation or Interrogation (e.g. '?'). Like closing punctuation
		except before Postfix or Non-starter. */
		EExLineBreakClass,
		/** Symbol (e.g. '/'. Like Alphabetic, but allows breaking before
		Alphabetic. */
		ESyLineBreakClass,
		/** Numeric Infix Separator (e.g. ','). Forbids breaking after any and before
		Numeric. */
		EIsLineBreakClass,
		/** Numeric Prefix (e.g. '$'). Forbids breaking before Numeric. */
		EPrLineBreakClass,
		/** Numeric Postfix (e.g. '%'). Forbids breaking after Numeric. */
		EPoLineBreakClass,
		/** Numeric (e.g. '1'). */
		ENuLineBreakClass,
		/** Alphabetic (e.g. 'a'). */
		EAlLineBreakClass,
		/** Ideographic (e.g. Japanese Kanji). Generally break before or after */
		EIdLineBreakClass,
		/** Inseparable (e.g. ellipsis). Forbid breaks between Inseparables. */
		EInLineBreakClass,
		/** Hyphen (e.g. '-'). Allows a break after except before Numeric. */
		EHyLineBreakClass,
		/** Break After. Generally allow a break after. Breaking between Break
		Afters not separated by spaces is prohibited. */
		EBaLineBreakClass,
		/** Break Before. Generally allow a break before. Breaking between Break
		Befores not separated by spaces is prohibited. */
		EBbLineBreakClass,
		/** Break Before and After. Generally allow a break before or after.
		Breaking between Break Before and Afters is prohibited, even if spaces
		are present. */
		EB2LineBreakClass,
		/** Zero-Width Space. Allow a break. */
		EZwLineBreakClass,
		/** Combining Mark. Takes on the class of its base class. */
		ECmLineBreakClass,
		/** Mandatory Break. */
		EBkLineBreakClass,
		/** Carriage Return. Break after unless part of a CRLF pair. */
		ECrLineBreakClass,
		/** Line Feed. Break after. */
		ELfLineBreakClass,
		/** Surrogate. Half of a surrogate pair. */
		ESgLineBreakClass,
		/** Contingent Break (e.g. embedded pictures). Uses external
		information */
		ECbLineBreakClass,

		/** Space. Intervening characters of class Space are indicated by
		aHaveSpaces in LineBreakPossible. */
		ESpLineBreakClass, 

		/** Complex Context (e.g. Thai). Runs of Complex Context are passed to
		GetLineBreakInContext. */
		ESaLineBreakClass,

		/** Ambiguous. Characters of ambiguous East Asian width are treated
		as Alphabetic, unless they are resolved as being "Wide", in which case
		they are treated as Ideographic. */
		EAiLineBreakClass,

		/** The Xx class is used when the class is unknown; e.g.; outside the provided context. */
		EXxLineBreakClass,

		/** The number of Unicode line break classes. */
		ELineBreakClasses
		};

	IMPORT_C virtual TUint LineBreakClass(TUint aCode,
		TUint& aRangeStart, TUint& aRangeEnd) const;
	IMPORT_C virtual TBool LineBreakPossible(TUint aPrevClass, TUint aNextClass,
		TBool aHaveSpaces) const;
	IMPORT_C virtual TBool GetLineBreakInContext(const TDesC16& aText,
		TInt aMinBreakPos, TInt aMaxBreakPos, TBool aForwards,
		TInt& aBreakPos) const;
	IMPORT_C virtual TBool IsHangingCharacter(TUint aChar) const;
	IMPORT_C TBool GetLineBreak(const TDesC16& aText,
		TInt aMinBreakPos, TInt aMaxBreakPos, TBool aForwards,
		MContingentLineBreaker* aCbDelegate,
		TInt& aBreakPos, TInt& aBreakPosAfterSpaces) const;
	IMPORT_C TBool GetLineBreakL(const TDesC16& aText,
		TInt aMinBreakPos, TInt aMaxBreakPos, TBool aForwards,
		MContingentLineBreakerL* aCbDelegate,
		TInt& aBreakPos, TInt& aBreakPosAfterSpaces) const;
	IMPORT_C virtual void* ExtendedInterface(TUid& aUid) const;
	};


class MContingentLineBreaker
/**
Used to determine the line breaks around pictures and other characters with 
the CB (contingent line break) class in situations where this operation cannot 
leave. 
@publishedAll
@released
*/
	{
public:
	/** Determines if a break is legal before the picture at aTextPosition. 
	
	@param aTextPosition The position of a character of line breaking class ECbLineBreakClass 
	in the text as supplied to MLineBreaker::GetLineBreak(), which may refer to 
	a picture.
	@param aPrecedingClass The class of the character preceding aTextPosition.
	@param aHasSpaces ETrue if spaces separate the preceding class from this.
	@return ETrue if a line break is legal here. */
	virtual TBool IsLegalBreakBefore(TInt aTextPosition, TInt aPrecedingClass,
		TBool aHasSpaces) = 0;
	/** Determines if a break is legal after the picture at aTextPosition. 
	
	@param aTextPosition The position of a character of line breaking class ECbLineBreakClass 
	in the text as supplied to MLineBreaker::GetLineBreak, which may refer to 
	a picture.
	@param aFollowingClass The class of the character immediately following aTextPosition.
	@param aHasSpaces ETrue if spaces separate the following class from this.
	@return ETrue if a line break is legal here. */
	virtual TBool IsLegalBreakAfter(TInt aTextPosition, TInt aFollowingClass,
		TBool aHasSpaces) = 0;
	};



class MContingentLineBreakerL
/**
Used to determine the line breaks around pictures and other characters with 
the CB (contingent line break) class in situations where this operation may 
leave. 
@publishedAll
@released
*/
	{
public:
	/** Determines if a break is legal before the picture at aTextPosition. 
	
	@param aTextPosition The position of a character of line breaking class ECbLineBreakClass 
	in the text as supplied to MLineBreaker::GetLineBreak(), which may refer to 
	a picture.
	@param aPrecedingClass The class of the character preceding aTextPosition.
	@param aHasSpaces ETrue if spaces separate the preceding class from this.
	@return ETrue if a line break is legal here. */
	virtual TBool IsLegalBreakBeforeL(TInt aTextPosition, TInt aPrecedingClass,
		TBool aHasSpaces) = 0;
	/** Determines if a break is legal after the picture at aTextPosition. 
	
	@param aTextPosition The position of a character of line breaking class ECbLineBreakClass 
	in the text as supplied to MLineBreaker::GetLineBreak(), which may refer to 
	a picture.
	@param aFollowingClass The class of the character immediately following aTextPosition.
	@param aHasSpaces ETrue if spaces separate the preceding class from this.
	@return ETrue if a line break is legal here. */
	virtual TBool IsLegalBreakAfterL(TInt aTextPosition, TInt aFollowingClass,
		TBool aHasSpaces) = 0;
	};

#endif
