// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __GLYPHSEL_H__
#define __GLYPHSEL_H__


#include <e32std.h>
#include <gdi.h>
#include "ShapeInfo.h"

/**
Forward declarations.
@internalComponent
*/
void Panic(TGdiPanic aError);

class TGlyphSelectionState;


/** 
 The Unicode char to use for glyph cluster without a base char 
@internalComponent
*/
#define KUnicodeDottedCircle 0x25CC

// 
//
// TUtf32Iterator Class declaration
//
//

class TUtf32Iterator
/*
 Converts UTF16 encoded array of bytes into UTF32 characters, 
 ignoring non-characters and unpaired surrogates and 
 combining paired surrogates.
@internalComponent
*/
	{
public:
	TUtf32Iterator(const TText16* aStart, const TText16* aEnd, TInt aOffset=0);

	inline TBool AtEnd() const;
	inline TBool BeforeStart() const;
	TChar Next();
	TChar Prev();
	void SetPos(TInt aOffset);
	inline TChar Get() const;
	TUint Get(TInt offset);
	TChar GetThenNext();
	TChar GetThenPrev();
	const TText16* CurrentPosition() const;
	void SetCurrentPosition(const TText16*);

	TInt LengthToStart() const;
	TInt LengthToEnd() const;

private:
	TUint UTF16ToTChar(const TText16* a);

private:
	/** Start address of the UTF16 array */
	const TText16* iStart;
	/** Address of current position in array */
	const TText16* iCurrent;
	/** Address of the first entry past the end of the array */
	const TText16* iEnd;

	/** UTF32 value of the character at the current iterator position */
	TChar iChar;
	};


// 
//
// GlyphSelUtils Namespace declaration
//
//


namespace GlyphSelUtils
/**
 This namespace holds a collection of useful common utility 
 functions used in glyph selection. These functions are intended to be
 used by the glyph selector classes.
@internalComponent
*/
	{
    inline TBool IsSurrogate(TText a) 
    	{ 
    	return 0xD800 == (a & 0xF800); 
    	}

    inline TBool IsHighSurrogate(TText a) 
    	{ 
    	return 0xD800 == (a & 0xFC00); 
    	}

    inline TBool IsLowSurrogate(TText a) 
    	{ 
    	return 0xDC00 == (a & 0xFC00); 
    	}

    inline TChar PairSurrogates(TText aHigh, TText aLow)
    	{
    	return ((aHigh - 0xd7f7) << 10) + aLow;
    	}

    inline TBool IsThaiCharacter(TUint code)
    	{
    	return ((code > 0x0E00 && code < 0x0E3B) ||
    			(code > 0x0E3E && code < 0x0E5C));
    	}

	}


// 
//
// TGlyphSelectionState Class declaration
//
//


class TGlyphSelectionState
/**
 This container class holds the data for glyph selection algorithm and is
 used to pass this data around the algorithm methods.
@internalComponent
*/
	{
public:
    enum TPenAdvance
    /** 
     Enum used in glyph selection code indicating if a pen advance is needed following
     the processing of the current glyph. 
    */
    	{
    	EPenAdvance_No,
    	EPenAdvance_Yes,
    	};

    enum TGlyphClusterStateOverride
    /**
     These enumeration values are used by the glyph selector classes to indicated
     back to the glyph selection algorithm when they find a cluster complete.
    */
    	{
    	EGClusterComplete,
    	EGClusterNotComplete
    	};
    	
    enum TGlyphPostCombine
    /**
    These enumeration values are used by the glyph selector classes to decide whether
    post combining is needed to combine the diacritic to the base character.
    */
    	{
    	EGPostCombine_No,
    	EGPostCombine_Yes
    	};
    	
    	
	TGlyphSelectionState(TUtf32Iterator& aIter, const CFont* aFont, CFont::TPositionParam& aParam)
		: iCodePt(0xFFFF), iCodeChar(0xFFFF), iCombCls(-1), iCats(0),
		  iText(aIter), iFont(aFont), iParam(aParam),
		  iClusterState(EGClusterNotComplete), iPen(EPenAdvance_No),
		  iAdvance(), iLigaturePortionsRemaining(0), iGlyphPostCombine(EGPostCombine_No) { };

	TBool IsCombiningClass() { return (iCats & 0xF0) ==  TChar::EMarkGroup; }
	void CombineLastGlyphToBase(const TRect& aBase, TInt aFirstDiacritic);

	TBool AppendGlyphToCluster(TUint code);

public:
	/** The properties of the current character being processed */
	TUint   iCodePt;
	TChar	iCodeChar;
	TInt	iCombCls;
	TUint	iCats;

	/** The Unicode iterator to the text processed */ 
	TUtf32Iterator& iText;

	/** The font to select glyphs from */
	const CFont* iFont;

	/** The in/out parameter data to the glyph selection code from outside */
	CFont::TPositionParam& iParam;	

	/** Result from the glyph selector class as to whether it thinks the 
	    cluster is complete or incomplete. 
	*/
	TGlyphClusterStateOverride	iClusterState;
	
	/** These hold the possible pen advance and if it should be applied */
	TPenAdvance iPen;
	TSize       iAdvance;

	/** Can be used in any way or not at all by the processing function. It is
	set to 0 on intitialisation. Suggested use is to pass information about
	which part of a ligature is currently having diacritics attatched to it. */
	TInt iLigaturePortionsRemaining;
	/** Can be used in any way or not at all by the processing function. It is
	not initialised. Suggested use is to record the position in the output
	glyph array of the first diacritic on this portion of the ligature. */
	TInt iLigaturePortionFirstMark;
	
	/** 
	Result from the glyph selector class as to whether it needs to
	combine the diacritic with the base character.
	*/
	TGlyphPostCombine iGlyphPostCombine;
	};


// 
//
// GlyphSelector_SoftHyphen Class declaration
//
//

class GlyphSelector_SoftHyphen
/**
 This glyph selector class processes the Unicode soft hyphen U+00AD
 character.
 This is a discretionary hyphen, i.e. it is only rendered when required
 and in Symbian OS that is when it is found at the end of a line.
@internalComponent
*/
	{
public:

	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};


// 
//
// GlyphSelector_Default Class declaration
//
//

class GlyphSelector_Default
/**
 This is the default glyph selector class which has the behaviour of outputting
 all glyphs it is invoked to process.
@internalComponent
*/
	{
public:

	static TBool Process(TGlyphSelectionState& aGss, RShapeInfo&);
	};


// 
// TUtf32Iterator inline Function definitions.
//


inline TBool TUtf32Iterator::AtEnd() const
	{
	return iEnd == iCurrent;
	}

inline TBool TUtf32Iterator::BeforeStart() const
	{
	return iStart > iCurrent;
	}

inline TChar TUtf32Iterator::Get() const
	{
	__ASSERT_DEBUG(iCurrent >= iStart && iCurrent < iEnd, Panic(EGdiPanic_OutOfText));

	return iChar;
	}

#endif // __GLYPHSEL_H__
