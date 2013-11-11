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

#ifndef LINEBREAKIMP_H_
#define LINEBREAKIMP_H_

#include <e32def.h>

const TText KZeroWidthSpace = 0x200B;
const TText KWhiteSpace = 0x0020;

// Forward delcarations.
GLREF_C void Panic(TInt aError);
class MLineBreaker;

/**
 Rule for which classes may be broken before.
@internalComponent
 */
struct TLineBreakRule
	{
	/** Classes that breaks are illegal before, even after spaces. */
	TUint iForbid;
	/** Classes that breaks are legal before, even without spaces. */
	TUint iAllow;
	};

/**
 Range of characters which have a particular line breaking class.
@internalComponent
 */
struct TLineBreakRange
	{
	TUint iStart;
	TUint iEnd;
	TUint iClass;
	};

/**
 Single-entry line break cache. Saves asking the MLineBreaker for the classes
 of multiple characters in the same run.
@internalComponent
 */
class TLineBreakClassCache
	{
public:
	TLineBreakClassCache(const MLineBreaker& aBreaker): iBreaker(aBreaker), iStart(0), iEnd(0), iClass(0) { }
	TUint LineBreakClass(TUint aChar);
	const MLineBreaker& Breaker() const { return iBreaker; }

private:
	const MLineBreaker& iBreaker;
	TUint iStart;
	TUint iEnd;
	TUint iClass;
	};

/**
@internalComponent
 */
void TestLineBreakTables(void);

/**
@internalComponent
 */
class TLineBreakClassIterator
	{
public:
	void Set(const TText* aFirst, const TText* aText, TLineBreakClassCache& aBreaker);
	void SetNull();
	/** Returns the pointer to the character that has the class returned by
	Class(). */
	const TText* Ptr() const { return iCurrent; }
	/** Returns the class of the current character. */
	TInt Class() const { return iClass; }
	// Will not go beyond maximum of aLimit
	// Should not be called with aLimit == Ptr()
	// Will return EFalse if the limit has been exceeded
	// aOffset must be 1 or -1
	TBool Move(const TText* aLimit, const TText* aLimitAfterSpaces,
		TInt aOffset, TBool& aHasSpaces, TLineBreakClassCache& aBreaker);
private:
    /** Addres of first character in the string to iterator through */
    const TText* iFirst;
	/** Current position within the iteration. */
	const TText* iCurrent;
	/** Class of the character at the currrent position. */
	TInt iClass;
	};


/**
@internalComponent
 */
TInt MoveTextPtr(const TText*& aPtr, const TText* aLimit, TInt aOffset);

/**
 Class for implementing the Unicode line breaking algorithm
@internalComponent
 */
class TLineBreakIterator
	{
public:
	TLineBreakIterator(TLineBreakClassCache& aBreaker,
		const TText* aText, TInt aLength, TBool aForwards,
		TInt aMinBreakPos, TInt aMaxBreakPos);
	TBool IsBreak(TBool aForwards);
	// Is one side of the potential line break CB class?
	TBool HasContingentBreak() const;
	// Get class before the break: useful for CB
	TInt PreviousClass() const;
	// Get class after the break: useful for CB
	TInt NextClass() const;
	// Are there spaces between the classes: useful for CB
	TInt HasSpaces() const;
	// go backwards
	TBool Decrement();
	// go forwards
	TBool Increment();
	// position of iterator at the break
	TInt BreakPos() const;
	// position of iterator after the break
	TInt AfterBreakPos() const;
	// position of iterator before the break
	TInt BeforeBreakPos() const;
private:
	TLineBreakClassCache iBreaker;
	const TText* iText;
	TInt iTextLength;
	const TText* iLimit;
	/** The limit that we are allowed to search beyond space characters. For
	forwards this will be up to the end of the text, for backwards we may not
	search beyond the minimum break position because that would mean that the
	break position returned would be below the minimum. */
	const TText* iLimitAfterSpaces;
	TLineBreakClassIterator iBeforeBreak;
	TLineBreakClassIterator iAfterBreak;
	TBool iHasSpaces;
	/** Holds the address of the lowest point allowed to break at */
	const TText* iMinBreakPos;
	/** Holds the address of the highest point allowed to break at */
	const TText* iMaxBreakPos;
	};

/** 
@internalComponent 
*/
TBool HasContingentBreak(TLineBreakIterator& aIterator, TBool aForwards,
	MContingentLineBreaker& aCbDelegate);

/** 
@internalComponent 
*/
TBool HasContingentBreakL(TLineBreakIterator& aIterator, TBool aForwards,
	MContingentLineBreakerL& aCbDelegate);

/** 
@internalComponent 
*/
TBool FindBreak(TLineBreakIterator& aIterator, TBool aForwards,
	MContingentLineBreaker* aCbDelegate);

/** 
@internalComponent 
*/
TBool FindBreakL(TLineBreakIterator& aIterator, TBool aForwards,
	MContingentLineBreakerL* aCbDelegate);

#endif
