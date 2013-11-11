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

/**
 @file
 @internalComponent
*/


#include <gdi.h>
#include <openfont.h>
#include "GlyphSel.h"
#include "GDIPANIC.h"


static const TText16 KLatinGlyph_SoftHyphen = 0x00AD;


// 
//
// TUtf32Iterator Class definition
//
//


TUint TUtf32Iterator::UTF16ToTChar(const TText16* a)
/**
 This routine takes an encoded UTF16 byte array and decodes the
 first character at the start of the array and returns it as a TChar.
 If the char is "not a char" character 0xFFFF results.
@param a 
 UTF16 byte array to be decoded.
@param aPr
 Position pointer 'a' derived from, incremented if surragote pairs decoded.
@return
 The character value in UTF32 format or 0xFFFF it not a character.
*/
	{
	// Is next char a surrogate?
	if (0xD800 == (a[0] & 0xF800)) 
		{
		// Is it a high surrogate in the range D800..DBFF?
		if (0xD800 == (a[0] & 0xFC00)) 
			{
			// Its a high surrogate, is the next char a low surrogate?
			if (0xDC00 == (a[1] & 0xFC00))
				{
				// It's a low surrogate
				return ((a[0] - 0xd7f7) << 10) + a[1];
				}
			else
				return 0xFFFF;
			}
		else
			return 0xFFFF;
		}
	else
		return a[0];
	}


TUtf32Iterator::TUtf32Iterator(const TText16* aStart, const TText16* aEnd, TInt aStartingIndex)
/**
 Construct iterator given UTF16 encoded byte array.
@param aStart
 Start address of the array.
@param aEnd
 Address of the byte just beyond the end of the array.
@param aStartingIndex
 Optional UTF16 offset into the array to initialise the current position to.
@panic EGdiPanic_InvalidInputParam
 Raised when array start if passed the array end.
*/
: iStart(aStart), iCurrent(aStart+aStartingIndex), iEnd(aEnd), iChar(0xffff)
	{
	GDI_ASSERT_DEBUG(iStart < iEnd, EGdiPanic_InvalidInputParam);
	
	if (iCurrent > iEnd) 
	    iCurrent = iEnd;
	else if (iCurrent < iStart)
	    iCurrent = iStart;
	else
		{
		// Sanatise array end checking for an unpaired surrogate value
		// so that UTF16ToTChar() does not read off the end of the array.
        if (0xD800 == (iEnd[-1] & 0xFC00))
			{
			if (iCurrent == iEnd-1)
				++iCurrent;
			else
				--iEnd;
			}

		// Setup initial position UTF32 character value 
		iChar = UTF16ToTChar(iCurrent);
		}
	}


TChar TUtf32Iterator::Next()
/**
Moves the iterator forward to the next valid UTF32 character value.
@return TChar The next character in the text towards the end.
@panic EGdiPanic_OutOfText
Raised when there is no next position to move to.
*/
	{
	GDI_ASSERT_DEBUG(iCurrent < iEnd, EGdiPanic_OutOfText);

    iCurrent += (iChar > 0xffff) ? 2 : 1;
    if (iCurrent < iEnd)
	    iChar = UTF16ToTChar(iCurrent);
	else
	    iChar = 0xFFFF;  
	return iChar;
	}


TChar TUtf32Iterator::Prev()
/**
Moves the iterator backwards to the next valid UTF32 character value.
@return TChar The prev character in the text towards the start.
@panic EGdiPanic_OutOfText Raised when there is no next position to move to.
*/
	{
	GDI_ASSERT_DEBUG(iCurrent >= iStart, EGdiPanic_OutOfText);

    --iCurrent;
    if (iCurrent >= iStart)
	    iChar = UTF16ToTChar(iCurrent);
	else
	    iChar = 0xFFFF;
	return iChar; 
	}


void TUtf32Iterator::SetPos(TInt aPos)
/**
 Moves the iterator to the position specified by array start+offset.
@param aPos
  UTF16 offset into the array to set the current position to.
@panic EGdiPanic_OutOfText
 Raised when there is no next position to move to.
*/
	{
	GDI_ASSERT_DEBUG(iStart+aPos <= iEnd, EGdiPanic_OutOfText);
	GDI_ASSERT_DEBUG(iStart+aPos >= iStart, EGdiPanic_OutOfText);

	iCurrent = iStart+aPos;
	iChar = UTF16ToTChar(iCurrent);
	}


TUint TUtf32Iterator::Get(TInt offset)
/**
 Returns the UTF32 char value at the offset specified. 0xFFFF may be returned
 for unpaired surrogate and noncharacters. Does not change the current 
 position.
@param offset
 UTF16 offset from current iterator position to get UTF32 char form. 
@return TChar
 UTF32 char value found at the iterator+offset, or 0xFFFF in error.
@panic EGdiPanic_OutOfText
 Raised when offset found to be outside the bounds of the original text array.
*/
	{
	GDI_ASSERT_DEBUG(iCurrent+offset >= iStart, EGdiPanic_OutOfText);
	GDI_ASSERT_DEBUG(iCurrent+offset < iEnd, EGdiPanic_OutOfText);
	
	return UTF16ToTChar(iCurrent+offset);
	}


TChar TUtf32Iterator::GetThenNext()
/**
 Return the UTF32 value at the current position.
@return TChar
 UTF32 value currently pointed to by iterator.
@panic EGdiPanic_EndOfText
 Raised when current iterator position is not valid.
*/
	{
	GDI_ASSERT_DEBUG(iCurrent < iEnd, EGdiPanic_OutOfText);

	TChar current(iChar);
    iCurrent += (iChar > 0xffff) ? 2 : 1;
    if (iCurrent < iEnd)
	    iChar = UTF16ToTChar(iCurrent);
	else
	    iChar = 0xFFFF;  
	return current;
	}


TChar TUtf32Iterator::GetThenPrev()
/**
 Return the UTF32 value at the current position.
@return TChar
 UTF32 value currently pointed to by iterator.
@panic EGdiPanic_EndOfText
 Raised when current iterator position is not valid.
*/
	{
	GDI_ASSERT_DEBUG(iCurrent >= iStart, EGdiPanic_OutOfText);

	TChar current(iChar);
    --iCurrent;
    if (iCurrent >= iStart)
	    iChar = UTF16ToTChar(iCurrent);
	else
	    iChar = 0xFFFF;
	return current;
	}
	
	
TInt TUtf32Iterator::LengthToStart() const
/**
 Returns the number of TText16 codes between the start point and its
 current position.
@return TInt
 Number of TText16 characters between array start and current iterator
 position.
*/
	{
	return iCurrent-iStart;
	}


TInt TUtf32Iterator::LengthToEnd() const
/**
 Returns the number of remaining TText16 codes still ahead of the
 iterator.
@return TInt
 Number of TText16 characters between array current iterator position
 and the end of the array.
*/
	{
	return iEnd - iCurrent;
	}

const TText16* TUtf32Iterator::CurrentPosition() const
	{
	return iCurrent;
	}

void TUtf32Iterator::SetCurrentPosition(const TText16* a)
	{
	iCurrent = a;
	}

// 
//
// TGlyphSelectionState Class definition
//
//


/** 
 The Unicode Combining Class values recognised by the
 GlyphSelUtils::CombineLastGlyphToBase method.
@internalComponent
*/
enum TCombiningClass
	{
	EArabicFathatan = 27,
	EArabicDammatan = 28,
	EArabicKasratan = 29,
	EArabicFatha = 30,
	EArabicDamma = 31,
	EArabicKasra = 32,
	EArabicShadda = 33,
	EArabicSukun = 34,
	ECombineBelowLeftAttached = 200,
	ECombineBelowAttached = 202,
	ECombineBelowRightAttached = 204,
	ECombineLeftAttached = 208,
	ECombineRightAttached = 210,
	ECombineAboveLeftAttached = 212,
	ECombineAboveAttached = 214,
	ECombineAboveRightAttached = 216,
	ECombineBelowLeft = 218,
	ECombineBelow = 220,
	ECombineBelowRight = 222,
	ECombineLeft = 224,
	ECombineRight = 226,
	ECombineAboveLeft = 228,
	ECombineAbove = 230,
	ECombineAboveRight = 232
	};


/**
 This method is called to attach (by adjusing its bounding box) the current end
 glyph in the output array of iParam to the base glyph bounding box based on
 the Unicode combining class of the character.
@param aGss
 The general input/output glyph selection data for the routine.
@param aGss.iOutput
 Input: Glyph cluster with last glyph an actual combining character. Output:
 Bounding box of last glyph adjusted according to char combining class.
@param aFirstDiacritic
 Which character in the output array to treat as the first diacritic of the
 cluster. Usually 1, but can be more if the base class is a ligature.
*/
void TGlyphSelectionState::CombineLastGlyphToBase(const TRect& aBase, TInt aFirstDiacritic)
	{
	// Get the bounds of all the base characters.
	TRect base = aBase;
	int last = iParam.iOutputGlyphs-1;
	for (int i = aFirstDiacritic; i < last; i++)
		base.BoundingRect(iParam.iOutput[i].iBounds);

	// Calculate the attachment points.
	TRect& r = iParam.iOutput[last].iBounds;
	int w = r.Width();
	int h = r.Height();
	int t = r.iTl.iY;
	int l = r.iTl.iX;
	int left = base.iTl.iX;
	int center = base.iTl.iX + (base.Width() - w) / 2;
	int right = base.iBr.iX - w;
	int below = base.iBr.iY;
	int above = base.iTl.iY - h;
	int left_of = left - w;
	int right_of = right + w;
	int xGap = 1;
	int yGap = iFont->HeightInPixels()/10;
	
	// Select attachment based on combining class.
	switch (iCombCls)
		{
		case ECombineBelowLeftAttached:
			t = below;
			l = left;
			break;
		case ECombineBelowAttached:
			t = below;
			l = center;
			break;
		case ECombineBelowRightAttached:
			t = below;
			l = right;
			break;
		case ECombineLeftAttached:
			l = left_of;
			break;
		case ECombineRightAttached:
			l = right_of;
			break;
		case ECombineAboveLeftAttached:
			t = above;
			l = left;
			break;
		case ECombineAboveAttached:
			t = above;
			l = center;
			break;
		case ECombineAboveRightAttached:
			t = above;
			l = right;
			break;
		case ECombineBelowLeft:
			t = below + yGap;
			l = left;
			break;
		case ECombineBelow:
		case EArabicKasratan:
		case EArabicKasra:
			t = below + yGap;
			l = center;
			break;
		case ECombineBelowRight:
			t = below + yGap;
			l = right;
			break;
		case ECombineLeft:
			l = left_of - xGap;
			break;
		case ECombineRight:
			l = right_of + xGap;
			break;
		case ECombineAboveLeft:
			t = above - yGap;
			l = left;
			break;
		case ECombineAbove:
		case EArabicFathatan:
		case EArabicDammatan:
		case EArabicFatha:
		case EArabicDamma:
		case EArabicShadda:
		case EArabicSukun:
			t = above - yGap;
			l = center;
			break;
		case ECombineAboveRight:
			t = above - yGap;
			l = right;
			break;
		default:
			l = center;
			break;
		}

	// Adjust the bounding box of the last glyph to fix position
	// based on the characters combining class. For speed, do directly.
	// r.SetRect(l,t,l + w,t + h);
	r.iTl.iX = l;
	r.iTl.iY = t;
	r.iBr.iX = l+w;
	r.iBr.iY = t+h;
	}


TBool TGlyphSelectionState::AppendGlyphToCluster(TUint aCode)
/**
 This common method is used by glyph selector classes to add a glyph to
 the end of the aGss.iParam output field filling in all the glyph info 
 needed.
@param aCode
 The Unicode character for which a glyph should be appended.
@param aGss
 The general input/output glyph selection data for the routine. 
@return TBool
 ETrue when successful, EFalse when failure occurs e..g no char data, overflow
*/
	{
	// Setup reference to next free glyph record we need to update.
	GDI_ASSERT_DEBUG(iParam.iOutputGlyphs < CFont::TPositionParam::EMaxOutputGlyphs, 
		EGdiPanic_InvalidInputParam);
	   
	CFont::TPositionParam::TOutput* output = iParam.iOutput+iParam.iOutputGlyphs;

	// Retrieve the glyph details from the Font. Essential to proceed, abort
	// if not available.
	TOpenFontCharMetrics metrics;
	if (iFont->GetCharacterData(aCode, metrics, output->iBitmap, 
		output->iBitmapSize) == CFont::ENoCharacterData)
		return EFalse;

	// Set code point of glyph in output record.
	output->iCode = aCode;
	
	// Set the glyph's bounds in the output record and record pen advancement.
	if (iParam.iDirection == CFont::EVertical)
		{
		metrics.GetVertBounds(output->iBounds);
		iAdvance.iHeight = Max(iAdvance.iHeight, metrics.VertAdvance());
		}
	else
		{
		metrics.GetHorizBounds(output->iBounds);
		iAdvance.iWidth = Max(iAdvance.iWidth, metrics.HorizAdvance());
		}

	// Next adjust the glyph's bounding box to offset it from the pen
	// position (origin of drawing). For speed increment attributes directly.
	// output->iBounds.Move(aGss.iParam.iPen);
	output->iBounds.iTl.iX += iParam.iPen.iX;
	output->iBounds.iBr.iX += iParam.iPen.iX;
	output->iBounds.iTl.iY += iParam.iPen.iY;
	output->iBounds.iBr.iY += iParam.iPen.iY;
	
	// Before we exit with success, increment the glyph array counter.
	// for the new glyph we've added here.
	iParam.iOutputGlyphs++;
	return ETrue;
	}


// 
//
// GlyphSelector_SoftHyphen Class definition
//
//

TBool GlyphSelector_SoftHyphen::Process(TGlyphSelectionState& aGss, RShapeInfo&) 
/**
@see GlyphSelUtils 
 See this class for the method description.
*/
	{
 	aGss.iText.Next();
	if (!aGss.iText.AtEnd())
		{
		// Here we skip & don't output hyphen since its not at the end a line.
		aGss.iPen = TGlyphSelectionState::EPenAdvance_No;
		}
	else
		{
		// If we reach here we must output hyphen.
		if (!aGss.AppendGlyphToCluster(KLatinGlyph_SoftHyphen))
			return EFalse;
		
		aGss.iPen = TGlyphSelectionState::EPenAdvance_Yes;
		}

	// Logic to determine if we are now at the end of the glyph cluster.
	// Default logic, based on whether a combining mark follows or not.
	aGss.iClusterState = 
		(!aGss.iText.AtEnd() &&
			((aGss.iText.Get().GetCategory() & 0xF0) == TChar::EMarkGroup)) ?
			TGlyphSelectionState::EGClusterNotComplete : TGlyphSelectionState::EGClusterComplete;

	return ETrue;
	}


// 
//
// GlyphSelector_Default Class definition
//
//


TBool GlyphSelector_Default::Process(TGlyphSelectionState& aGss, RShapeInfo&) 
/**
@see GlyphSelUtils 
 See this class for the method description.
*/
	{
	
	// In this method we always output the glyph.
	if (!aGss.AppendGlyphToCluster(aGss.iText.GetThenNext()))
		return EFalse;

	// Adjust glyph's bounds further to position this character if it is a
	// combining mark
	if (aGss.IsCombiningClass())
		{
		aGss.iPen = TGlyphSelectionState::EPenAdvance_No;
		
		TRect baseBounds(aGss.iParam.iOutput[0].iBounds);
		// Get first character in this glyph cluster. In this default process function, the iCode should 
		// be Unicode Point Code.  
		TChar startChar = TChar(aGss.iParam.iOutput[0].iCode);
		// Character index in the output array to treat as the first diacritic of the
		// cluster. It will be used as first character for combine to. usually 1, but when
		// the cluster starts with a combining mark, it should be set to 0.  
		TInt indexOfFirstCombining = 1; 
		TInt startCharCat = startChar.GetCategory() & 0xF0;
		
		// if the first character in this cluster is a combining mark or a graphically empty character, 
		// (such as a space Character0x0020), a fake bound, formed from the Ascent of the font, will be 
		// used for combining
		if ((startCharCat == TChar::EMarkGroup) || baseBounds.Size() == TSize(0,0)) 
			{
			// Determine the height of the combining glyph.
			TInt glyphHeight = 0;
			if (aGss.iParam.iOutputGlyphs == 1)
				{
				glyphHeight = aGss.iParam.iOutput[0].iBitmapSize.iHeight;
				}
			else
				{
				glyphHeight = aGss.iParam.iOutput[1].iBitmapSize.iHeight;
				}
			// Adjust Y values to a ficticious but reasonable range for it to combine to using the glyph height to adjust correctly below the font ascent.
			baseBounds.iTl.iY = aGss.iParam.iPen.iY - aGss.iFont->AscentInPixels() + glyphHeight; //modest ascender
			baseBounds.iBr.iY = aGss.iParam.iPen.iY; //No descender
			}
		
		if (startCharCat == TChar::EMarkGroup)
			indexOfFirstCombining = 0;
								
		aGss.CombineLastGlyphToBase(baseBounds, indexOfFirstCombining);
		aGss.iGlyphPostCombine = TGlyphSelectionState::EGPostCombine_Yes;
		}	
	else
		aGss.iPen = TGlyphSelectionState::EPenAdvance_Yes;

	// Logic to determine if we are now at the end of the glyph cluster.
	// Default logic, based on whether a combining mark follows or not.
	aGss.iClusterState = 
		(!aGss.iText.AtEnd() &&
		 ((aGss.iText.Get().GetCategory() & 0xF0) == TChar::EMarkGroup)) ?
			TGlyphSelectionState::EGClusterNotComplete : TGlyphSelectionState::EGClusterComplete;

	return ETrue;
	}

