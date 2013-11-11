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

#ifndef BIDICOMPACT_H_
#define BIDICOMPACT_H_

#include <e32std.h>
#include <bidi.h>

class TRunInfoCompact
/**
Stores information about a run and how to re-order it.
@internalTechnology
*/
	{
public:
	/** Describes text to be re-ordered.
	@internalTechnology */
	class TReorderingContext
		{
	public:
		/** Where the logical-order text starts. */
		const TText* iSource;
		/** The start of the line. */
		TInt iStart;
		/** One past the end of the line. */
		TInt iEnd;
		/**
		 * Truncation character, if there is to be one on this line. Otherwise,
		 * 0xFFFF.
		 */
		TChar iTruncation;
		/** ETrue if the line must join at its beginning. */
		TBool iJoinsAtStart;
		/** ETrue if the line must join at its end. */
		TBool iJoinsAtEnd;
		};
	
	/** How to re-order the run.
	@internalTechnology */
	enum TTypeFlags
		{
		/** Reverse the run, but not surrogate pairs or combining characters. */
		EFRightToLeft = 0x80000000,
		/** Simply reverse the run, do not check for surrogates or combiners. */
		EFNoPairsNoCombiners = 0x40000000,
		/** No mirrored characters are present. */
		EFNoMirroredCharacters = 0x20000000
		};
	
	/**	Sets up an empty run.
	@internalTechnology */
	TRunInfoCompact() : iStart(0), iLengthAndType(0) {}
	TRunInfoCompact(TInt aStart, TInt aLength, TBool aReverse,
		const TText* aText);
	TRunInfoCompact(TInt aStart, TInt aLength, TBool aReverse);
	TBool AddRun(const TRunInfoCompact& aToBeAdded);
	TText* Reorder(TText* aDestination, const TReorderingContext& aContext) const;

	/**	@return The start of this run.
	@internalTechnology */
	TInt Start() const { return iStart; }

	/**@return The length of this run.
	@internalTechnology */
	TInt Length() const { return static_cast<TInt>(iLengthAndType & 0x0FFFFFFF); }

	/** @return The flags associated with this run.
	@internalTechnology */
	TInt TypeFlags() const { return iLengthAndType & 0xF0000000; }

	static TInt Convert(TRunInfoCompact* aBuffer, const TDesC& aText,
		const TBidirectionalState::TRunInfo* aRunArray, TInt aArraySize);
	static TBool JoinBefore(const TText* aText, TInt aIndex);
private:
	TInt iStart;
	TUint iLengthAndType;
	};

#endif

