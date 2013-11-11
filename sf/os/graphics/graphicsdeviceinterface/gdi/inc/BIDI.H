/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Definitions for bidirectional text reordering.
* 
*
*/



#ifndef BIDI_H_
#define BIDI_H_

#include <e32std.h>

class RWriteStream;
class RReadStream;


class TBidirectionalState
/** 
The bidirectional state class. 
This class contains functions that implement the Unicode Bidirectional Algorithm, 
which changes text from logical order to display order for the correct display 
of right-to-left scripts, like Arabic and Hebrew. A TBidirectionalState object 
carries the embedding level and stack from one line to the next. On construction 
it is in its 'start of paragraph' state. 
@publishedAll
@released
*/
	{
	friend class CTBiDi;
	
public:
	
	class TRunInfo
	/** 
	Information about a run of characters with the same bidirectional 
	category. 

	An array of these representing a line is passed into, and reordered by, 
	ReorderLine(). 
	@publishedAll
	@released
	*/
		{
		public:
		/** Category of a run of text: passed to ReorderLine(); these are 
		passed in as TChar::TBdCategory values but are modified for internal 
		use. */
		TUint iCategory;		
		/** Embedding level of this run: used internally by ReorderLine(). */
		TUint8 iEmbeddingLevel;	
		/** Resolved direction of this run: 0 for left to right, 1 for right 
		to left. */
		TUint8 iDirection;		
		/** Index of this run after reordering. */
		TInt iIndex;			
		/** Start of text; returned by ReorderText(). */
		TInt iStart;			
		/** Length of text; returned by ReorderText(). */
		TInt iLength;			
		};

	IMPORT_C TBidirectionalState();
	IMPORT_C void ReorderLine(TRunInfo* aRunInfo,TInt aRuns,TBool aParStart,TBool aParEnd,TBool aParRightToLeft,
					 TChar::TBdCategory aNextCategory,TChar::TBdCategory aNextStrongCategory,
					 TBool& aVisualEndIsAmbiguous);
	IMPORT_C void ReorderLine(TRunInfo* aRunInfo,TInt aRuns,TBool aParStart,TBool aParEnd,TBool aParRightToLeft,
					 TChar::TBdCategory aNextCategory,TChar::TBdCategory aNextStrongCategory);
	IMPORT_C static TInt ReorderText(const TText* aText,TInt aLength,TBool aParRightToLeft,TText*& aNewText);
	IMPORT_C static void ReverseGroups(TText* aStart,TInt aLength);
	IMPORT_C void Reset();
	IMPORT_C TBool IsDefault() const;
	IMPORT_C TBool operator==(const TBidirectionalState& aState) const;
	IMPORT_C void ExternalizeL(RWriteStream& aDest);
	IMPORT_C void InternalizeL(RReadStream& aSource);
	TBool ParRightToLeft() const { return iStack[0].iEmbeddingLevel & 1; }

private:
	// Bidirectional categories coded as bit flags
	enum TCategory
		{
		ELeftToRight = 1 << TChar::ELeftToRight, 
		ELeftToRightEmbedding = 1 << TChar::ELeftToRightEmbedding,
		ELeftToRightOverride = 1 << TChar::ELeftToRightOverride,
		ERightToLeft = 1 << TChar::ERightToLeft,
		ERightToLeftArabic = 1 << TChar::ERightToLeftArabic,
		ERightToLeftEmbedding = 1 << TChar::ERightToLeftEmbedding, 
		ERightToLeftOverride = 1 << TChar::ERightToLeftOverride,
		EPopDirectionalFormat = 1 << TChar::EPopDirectionalFormat, 
		EEuropeanNumber = 1 << TChar::EEuropeanNumber, 
		EEuropeanNumberSeparator = 1 << TChar::EEuropeanNumberSeparator, 
		EEuropeanNumberTerminator = 1 << TChar::EEuropeanNumberTerminator, 
		EArabicNumber = 1 << TChar::EArabicNumber,
		ECommonNumberSeparator = 1 << TChar::ECommonNumberSeparator, 
		ENonSpacingMark = 1 << TChar::ENonSpacingMark, 
		EBoundaryNeutral = 1 << TChar::EBoundaryNeutral, 
		EParagraphSeparator = 1 << TChar::EParagraphSeparator, 
		ESegmentSeparator = 1 << TChar::ESegmentSeparator, 
		EWhitespace = 1 << TChar::EWhitespace, 
		EOtherNeutral = 1 << TChar::EOtherNeutral,

		// useful groups
		EBdControlsGroup = ELeftToRightEmbedding | ERightToLeftEmbedding |
						   ELeftToRightOverride | ERightToLeftOverride | EPopDirectionalFormat,
		ELeftToRightGroup = ELeftToRight | EEuropeanNumber | ELeftToRightOverride | ELeftToRightEmbedding,
		ERightToLeftGroup = ERightToLeft | EArabicNumber | ERightToLeftArabic | ERightToLeftOverride |
							ERightToLeftEmbedding,
		EStrongGroup = ELeftToRightEmbedding | ERightToLeftEmbedding
			| ELeftToRightOverride | ERightToLeftOverride
			| ELeftToRight | ERightToLeft | ERightToLeftArabic,

		ENoCategory = 0
		};

	enum
		{
		EMaxExplicitLevel = 61,
		EMaxLevel = 63,
		EMaxStackLevels = 62
		};

	enum TOverrideState
		{
		ENoOverrideState = 0,
		ELeftToRightOverrideState = ELeftToRightOverride,
		ERightToLeftOverrideState = ERightToLeftOverride
		};

	class TStackItem
		{
	public:
		TUint8 iEmbeddingLevel;			// embedding level
		TOverrideState iOverrideState;	// directional override state
		TCategory iStartCategory;		// category that started this level; EOtherNeutral if none
		};

private:
	/**	The information needed during line reordering.
	WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.	
	 */
	class TReorderContext
		{
	public:
		void SetNextCategory(TChar::TBdCategory aCat);
		void SetNextStrongCategory(TChar::TBdCategory aCat);
	public:
		/** Category at start of next line, or ON if at the end of the
		paragraph. */
		TCategory iNextCategory;
		/** Which of L, R or AL appears first in the remainder of the
		paragraph, or ON if none. */
		TCategory iNextStrongCategory;
		/** The run array for this line. */
		TRunInfo* iRunInfo;
		/** The length of iRunInfo. */
		TInt iRuns;
		/** Bitmap of categories currently present in iRunInfo. */
		TUint32 iCategories;
		/** Found by the algorithm to set iPreviousStrongCategory. */
		TCategory iLastStrongCategory;
		};

public:
	static TInt GenerateBdRunArray(const TText* aText, TInt aLength,
		TBidirectionalState::TRunInfo* aRun, TInt aMaxRuns);

private:
	const TStackItem& State() const { return iStack[iStackLevel]; }
	TCategory Push(TCategory aStartCategory);
	TCategory Pop();

public:
	TBidirectionalState(TChar::TBdCategory aPrevCat,
		TChar::TBdCategory aPrevStrongCat, TBool aParRightToLeft);
	void HandleBdControls(TReorderContext& aContext); 
	void ResolveWeakTypesW1W2W3(TReorderContext& aContext);
	void ResolveWeakTypesW4W5W6(TReorderContext& aContext); 
	void ResolveWeakTypesW7(TReorderContext& aContext); 
	void ResolveNeutralTypes(TReorderContext& aContext); 
	void ResolveImplicitLevels(TReorderContext& aContext); 
	void PrepareForNextLine(const TReorderContext& aContext);
	void ReorderRuns(TReorderContext& aContext);
	static TInt CatToNumber(TInt aCat);
	static TCategory CharToBdCat(TChar::TBdCategory aCat);
	static TCategory UintToBdCat(TUint aCat);
	static void DeneutralizeRuns(TRunInfo* aStart, TRunInfo* aEnd,
		TCategory aStartCategory, TCategory aEndCategory);
private:
	TCategory iPreviousCategory;			// category at end of last line, or EStartOfParagraph if at start of par
	TCategory iPreviousStrongCategory;		// L or R; derived from embedding level if at start of par
	TInt16 iStackLevel;						// current stack level
	TInt8 iPushesBeyond60;	// number of times Push called with iStackLevel == 60 and Left-To-Right category
	TInt8 iPushesBeyond61;	// number of times Push called with iStackLevel == 61
	TStackItem iStack[EMaxStackLevels];		// the stack of embedding levels
	};

#endif // BIDI_H_
