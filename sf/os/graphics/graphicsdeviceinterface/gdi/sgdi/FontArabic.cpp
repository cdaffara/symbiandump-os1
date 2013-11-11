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
#include "FontArabic.h"
#include "GDIPANIC.h"


//
// Arabic Presentation Forms.
//


/**
 The contextual forms of each character in the range 0621 - 064A, in
 the order final, initial, medial; followed by the forms of
 the lam-alef ligatures (lam with 0622, 0623, 0625 and 0627);
 followed by the forms of the characters in the range 0671 - 06D3
*/
LOCAL_D const TUint16 TheArabicShapeTable[] =
	{
	// Contextual forms for standard Arabic letters
	0x621, 0x621, 0x621,			// 621
	0xfe82, 0x622, 0x622,			// 622
	0xfe84, 0x623, 0x623,			// 623
	0xfe86, 0x624, 0x624,			// 624
	0xfe88, 0x625, 0x625,			// 625
	0xfe8a, 0xfe8b, 0xfe8c,			// 626
	0xfe8e, 0x627,	0x627,			// 627
	0xfe90, 0xfe91, 0xfe92,			// 628
	0xfe94, 0x629, 0x629,			// 629
	0xfe96, 0xfe97, 0xfe98,			// 62a
	0xfe9a, 0xfe9b, 0xfe9c,			// 62b
	0xfe9e, 0xfe9f, 0xfea0,			// 62c
	0xfea2, 0xfea3, 0xfea4,			// 62d
	0xfea6, 0xfea7, 0xfea8,			// 62e
	0xfeaa, 0x62f, 0x62f,			// 62f
	0xfeac, 0x630, 0x630,			// 630
	0xfeae, 0x631, 0x631,			// 631
	0xfeb0, 0x632, 0x632,			// 632
	0xfeb2, 0xfeb3, 0xfeb4,			// 633
	0xfeb6, 0xfeb7, 0xfeb8,			// 634
	0xfeba, 0xfebb, 0xfebc,			// 635
	0xfebe, 0xfebf, 0xfec0,			// 636
	0xfec2, 0xfec3, 0xfec4,			// 637
	0xfec6, 0xfec7, 0xfec8,			// 638
	0xfeca, 0xfecb, 0xfecc,			// 639
	0xfece, 0xfecf, 0xfed0,			// 63a
	0x63b, 0x63b, 0x63b,			// placeholder
	0x63c, 0x63c, 0x63c,			// placeholder
	0x63d, 0x63d, 0x63d,			// placeholder
	0x63e, 0x63e, 0x63e,			// placeholder
	0x63f, 0x63f, 0x63f,			// placeholder
	0x640, 0x640, 0x640,			// 640
	0xfed2, 0xfed3, 0xfed4,			// 641
	0xfed6, 0xfed7, 0xfed8,			// 642
	0xfeda, 0xfedb, 0xfedc,			// 643
	0xfede, 0xfedf, 0xfee0,			// 644
	0xfee2, 0xfee3, 0xfee4,			// 645
	0xfee6, 0xfee7, 0xfee8,			// 646
	0xfeea, 0xfeeb, 0xfeec,			// 647
	0xfeee, 0x648, 0x648,			// 648
	0xfef0, 0xfbe8, 0xfbe9,			// 649
	0xfef2, 0xfef3, 0xfef4,			// 64A

	// Contextual forms for lam-alef ligatures
	0xfef6, 0xfef5, 0xfef5,			// 644 + 622
	0xfef8, 0xfef7, 0xfef7,			// 644 + 623
	0xfefa, 0xfef9, 0xfef9,			// 644 + 625
	0xfefc, 0xfefb, 0xfefb,			// 644 + 627

	// Contextual forms for extended Arabic letters for Persian, Urdu, Sindhi, etc.
	0xfb51, 0x0671, 0x0671,			// 671
	0x0672, 0x0672, 0x0672,			// placeholder
	0x0673, 0x0673, 0x0673,			// placeholder
	0x0674, 0x0674, 0x0674,			// placeholder
	0x0675, 0x0675, 0x0675,			// placeholder
	0x0676, 0x0676, 0x0676,			// placeholder
	0x0677, 0x0677, 0x0677,			// placeholder
	0x0678, 0x0678, 0x0678,			// placeholder
	0xfb67, 0xfb68, 0xfb69,			// 679
	0xfb5f, 0xfb60, 0xfb61,			// 67a
	0xfb53, 0xfb54, 0xfb55,			// 67b
	0x067c, 0x067c, 0x067c,			// placeholder
	0x067d, 0x067d, 0x067d,			// placeholder
	0xfb57, 0xfb58, 0xfb59,			// 67e
	0xfb63, 0xfb64, 0xfb65,			// 67f

	0xfb5b, 0xfb5c, 0xfb5d,			// 680
	0x0681, 0x0681, 0x0681,			// placeholder
	0x0682, 0x0682, 0x0682,			// placeholder
	0xfb77, 0xfb78, 0xfb79,			// 683
	0xfb73, 0xfb74, 0xfb75,			// 684
	0x0685, 0x0685, 0x0685,			// placeholder
	0xfb7b, 0xfb7c, 0xfb7d,			// 686
	0xfb7f, 0xfb80, 0xfb81,			// 687
	0xfb89, 0x0688, 0x0688,			// 688
	0x0689, 0x0689, 0x0689,			// placeholder
	0x068a, 0x068a, 0x068a,			// placeholder
	0x068b, 0x068b, 0x068b,			// placeholder
	0xfb85, 0x068c, 0x068c,			// 68c
	0xfb83, 0x068d, 0x068d,			// 68d
	0xfb87, 0x068e, 0x068e,			// 68e
	0x068f, 0x068f, 0x068f,			// placeholder

	0x0690, 0x0690, 0x0690,			// placeholder
	0xfb8d, 0x0691, 0x0691,			// 691
	0x0692, 0x0692, 0x0692,			// placeholder
	0x0693, 0x0693, 0x0693,			// placeholder
	0x0694, 0x0694, 0x0694,			// placeholder
	0x0695, 0x0695, 0x0695,			// placeholder
	0x0696, 0x0696, 0x0696,			// placeholder
	0x0697, 0x0697, 0x0697,			// placeholder
	0xfb8b, 0x0698, 0x0698,			// 698
	0x0699, 0x0699, 0x0699,			// placeholder
	0x069a, 0x069a, 0x069a,			// placeholder
	0x069b, 0x069b, 0x069b,			// placeholder
	0x069c, 0x069c, 0x069c,			// placeholder
	0x069d, 0x069d, 0x069d,			// placeholder
	0x069e, 0x069e, 0x069e,			// placeholder
	0x069f, 0x069f, 0x069f,			// placeholder

	0x06a0, 0x06a0, 0x06a0,			// placeholder
	0x06a1, 0x06a1, 0x06a1,			// placeholder
	0x06a2, 0x06a2, 0x06a2,			// placeholder
	0x06a3, 0x06a3, 0x06a3,			// placeholder
	0xfb6b, 0xfb6c, 0xfb6d,			// 6a4
	0x06a5, 0x06a5, 0x06a5,			// placeholder
	0xfb6f, 0xfb70, 0xfb71,			// 6a6
	0x06a7, 0x06a7, 0x06a7,			// placeholder
	0x06a8, 0x06a8, 0x06a8,			// placeholder
	0xfb8f, 0xfb90, 0xfb91,			// 6a9
	0x06aa, 0x06aa, 0x06aa,			// 6aa
	0x06ab, 0x06ab, 0x06ab,			// 6ab
	0x06ac, 0x06ac, 0x06ac,			// 6ac
	0xfbd4, 0xfbd5, 0xfbd6,			// 6ad
	0x06ae, 0x06ae, 0x06ae,			// 6ae
	0xfb93, 0xfb94, 0xfb95,			// 6af

	0x06b0, 0x06b0, 0x06b0,			// placeholder
	0xfb9b, 0xfb9c, 0xfb9d,			// 6b1
	0x06b2, 0x06b2, 0x06b2,			// 6b2
	0xfb97, 0xfb98, 0xfb99,			// 6b3
	0x06b4, 0x06b4, 0x06b4,			// placeholder
	0x06b5, 0x06b5, 0x06b5,			// placeholder
	0x06b6, 0x06b6, 0x06b6,			// placeholder
	0x06b7, 0x06b7, 0x06b7,			// placeholder
	0x06b8, 0x06b8, 0x06b8,			// placeholder
	0x06b9, 0x06b9, 0x06b9,			// placeholder
	0xfb9f, 0x06ba, 0x06ba,			// 6ba
	0xfba1, 0xfba2, 0xfba3,			// 6bb
	0x06bc, 0x06bc, 0x06bc,			// placeholder
	0x06bd, 0x06bd, 0x06bd,			// placeholder
	0xfbab, 0xfbac, 0xfbad,			// 6be
	0x06bf, 0x06bf, 0x06bf,			// placeholder

	0xfba5, 0x06c0, 0x06c0,			// 6c0
	0xfba7, 0xfba8, 0xfba9,			// 6c1
	0x06c2, 0x06c2, 0x06c2,			// placeholder
	0x06c3, 0x06c3, 0x06c3,			// placeholder
	0x06c4, 0x06c4, 0x06c4,			// placeholder
	0xfbe1, 0x06c5, 0x06c5,			// 6c5
	0xfbda, 0x06c6, 0x06c6,			// 6c6
	0xfbd8, 0x06c7, 0x06c7,			// 6c7
	0xfbdc, 0x06c8, 0x06c8,			// 6c8
	0xfbe3, 0x06c9, 0x06c9,			// 6c9
	0x06ca, 0x06ca, 0x06ca,			// placeholder
	0xfbdf, 0x06cb, 0x06cb,			// 6cb
	0xfbfd, 0xfbfe, 0xfbff,			// 6cc
	0x06cd, 0x06cd, 0x06cd,			// placeholder
	0x06ce, 0x06ce, 0x06ce,			// placeholder
	0x06cf, 0x06cf, 0x06cf,			// placeholder

	0xfbe5, 0xfbe6, 0xfbe7,			// 6d0
	0x06d1, 0x06d1, 0x06d1,			// placeholder
	0xfbaf, 0x06d2, 0x06d2,			// 6d2
	0xfbb1, 0x06d3, 0x06d3			// 6d3
	};


static const int KFinal = 0;
static const int KInitial = 1;
static const int KMedial = 2;
static const int KStandardShapeTableOffset = 0x621;
static const int KLamAlefShapeTableStart = 0x64B - 0x621;
static const int KExtendedShapeTableOffset = 0x671 - (0x64f - 0x621);


static inline TInt ArabicShapeTableIndex(TInt aCode)
	{
	if (aCode >= 0x621 && aCode <= 0x64a)
		return (aCode - KStandardShapeTableOffset) * 3;
	if (aCode >= 0x671 && aCode <= 0x6d3)
		return (aCode - KExtendedShapeTableOffset) * 3;
	return KErrNotFound;
	}


static inline TBool IsArabicPoint(TInt aCode)
	{
	return (aCode >= 0x64B && aCode <= 0x655) || aCode == 0x670;
	}

TInt ConsumeCharsAndDeterminePresentationForm(const TUtf32Iterator& aText,
	TUint aFlags, TInt& aLigaturePortionsRemaining)
/**
 Get the Arabic or other presentation form, consuming as many characters 
 as necessary. This function can cope with text that has already been 
 reversed.
@internalComponent
*/
	{
	TUint c = aText.Get();
	TInt index = ArabicShapeTableIndex(c);
	if (index != -1)
		{
		if (aLigaturePortionsRemaining != 0)
			{
			--aLigaturePortionsRemaining;
			GDI_ASSERT_DEBUG(!aText.AtEnd(), EGdiPanic_Invariant);
			return 0xFFFF;
			}

		int position = -1;

		// Find previous and next character, skipping Arabic points.
		TUtf32Iterator tmpText(aText);		
		tmpText.Prev();
        TUint prev = 0xFFFF;
        while (!tmpText.BeforeStart() && (prev == 0xFFFF || IsArabicPoint(prev)))
            prev = tmpText.GetThenPrev();		

		tmpText = aText;
		tmpText.Next();
        TUint next = 0xFFFF;
        while (!tmpText.AtEnd() && (next == 0xFFFF || IsArabicPoint(next)))
            next = tmpText.GetThenNext();		
		
		TInt possible_lam = (aFlags & CFont::TPositionParam::EFLogicalOrder)?
			c : next;
		TInt possible_alef = (aFlags & CFont::TPositionParam::EFLogicalOrder)?
			next : c;

		// Perform lam-alef ligation if necessary.
		if (possible_lam == 0x644 &&
			(possible_alef == 0x622 || possible_alef == 0x623 || possible_alef == 0x625 || possible_alef == 0x627))
			{
			switch(possible_alef)
				{
				case 0x622:
					{
					c = 0xfef5;
					index = KLamAlefShapeTableStart;
					break;
					}
				case 0x623:
					{
					c = 0xfef7;
					index = KLamAlefShapeTableStart + 1;
					break;
					}
				case 0x625:
					{
					c = 0xfef9;
					index = KLamAlefShapeTableStart + 2;
					break;
					}
				default:
					{
					c = 0xfefb;
					index = KLamAlefShapeTableStart + 3;
					}
				}
			index *= 3;
			aLigaturePortionsRemaining = 2;

			next = 0xFFFF;
			while (!tmpText.AtEnd() && (next == 0xFFFF || IsArabicPoint(next)))
				{
				next = tmpText.Get();
				tmpText.Next();
				}
			}

		TInt left = (aFlags & CFont::TPositionParam::EFLogicalOrder)? prev : next;
		TInt right = (aFlags & CFont::TPositionParam::EFLogicalOrder)? next : prev;

		TBool prefix = FALSE;
		if (left == 0x640 || left == 0x200D) // kashida or Zero-Width Joiner
			prefix = TRUE;
		else
			{
			TInt left_index = ArabicShapeTableIndex(left);
			if (left_index != -1)
				{
				if (static_cast<TInt>(TheArabicShapeTable[left_index + KInitial]) != left ||
					static_cast<TInt>(TheArabicShapeTable[left_index + KMedial]) != left)
					prefix = TRUE;
				}
			}
		TBool suffix = FALSE;
		if (right == 0x640 || right == 0x200D) // kashida or Zero-Width Joiner
			suffix = TRUE;
		else 
			{
			TInt right_index = ArabicShapeTableIndex(right);
			if (right_index != -1)
				{
				if (static_cast<TInt>(TheArabicShapeTable[right_index + KFinal]) != right ||
					static_cast<TInt>(TheArabicShapeTable[right_index + KMedial]) != right)
					suffix = TRUE;
				}
			}
		TBool medial = static_cast<TUint>(TheArabicShapeTable[index + KMedial]) != c;
		if (medial)
			{
			if (prefix && suffix)
				position = KMedial;
			else if (prefix)
				position = KFinal;
			else if (suffix)
				position = KInitial;
			}
		else
			{
			if (prefix)
				position = KFinal;
			else if (suffix)
				position = KInitial;
			}
		if (position >= 0)
			c = TheArabicShapeTable[index + position];
		}

	return c;
	}


// 
//
// GlyphSelector_Arabic Class definition
//
//

TBool GlyphSelector_Arabic::Process(TGlyphSelectionState& aGss, RShapeInfo&) 
	{
	// Select glyph - use presentation form if applicable.
	TInt ligaturePortions = aGss.iLigaturePortionsRemaining;
	TUint flags = aGss.iParam.iFlags;
	TUint code = ConsumeCharsAndDeterminePresentationForm(aGss.iText,
		flags, ligaturePortions);

	aGss.iText.Next();
	aGss.iClusterState = !aGss.iText.AtEnd() &&
		(aGss.iText.Get().GetCategory() & 0xF0) == TChar::EMarkGroup?
		TGlyphSelectionState::EGClusterNotComplete
		: TGlyphSelectionState::EGClusterComplete;

	// If we are beginning a new portion of a ligature, we must set the state.
	if (ligaturePortions != aGss.iLigaturePortionsRemaining)
		{
		if (ligaturePortions != 1)
			aGss.iClusterState = TGlyphSelectionState::EGClusterNotComplete;
		aGss.iLigaturePortionFirstMark = aGss.iParam.iOutputGlyphs;
		if (ligaturePortions < aGss.iLigaturePortionsRemaining)
			{
			aGss.iLigaturePortionsRemaining = ligaturePortions;
			// We have just swallowed a non-initial part of the ligature, which
			// must not be drawn.
			GDI_ASSERT_DEBUG(code == 0xFFFF, EGdiPanic_Invariant);
			return ETrue;
			}
		aGss.iLigaturePortionsRemaining = ligaturePortions;
		}

	// In this method we always output the glyph code calculate previously.
	if (!aGss.AppendGlyphToCluster(code))
		return EFalse;

	if (!aGss.IsCombiningClass())
		{
		aGss.iPen = TGlyphSelectionState::EPenAdvance_Yes;
		return ETrue;
		}

	// Adjust glyph's bounds further to position this character.
	aGss.iPen = TGlyphSelectionState::EPenAdvance_No;
	TRect baseBounds = aGss.iParam.iOutput[0].iBounds;
	TInt firstMark = 1;
	if (aGss.iLigaturePortionsRemaining != 0)
		{
		TInt centre = (baseBounds.iBr.iX + baseBounds.iTl.iX) >> 1;
		TBool firstHalf = aGss.iLigaturePortionsRemaining == 2;
		TBool rightHandSide = flags & CFont::TPositionParam::EFLogicalOrder?
			firstHalf : !firstHalf;
		if (rightHandSide)
			baseBounds.iTl.iX = centre;
		else
			baseBounds.iBr.iX = centre;
		if (firstHalf)
			aGss.iClusterState = TGlyphSelectionState::EGClusterNotComplete;
		firstMark = aGss.iLigaturePortionFirstMark;
		}
	aGss.CombineLastGlyphToBase(baseBounds, firstMark);

	return ETrue;
	}


TBool GlyphSelector_Arabic::CharactersJoin(TInt aLeftCharacter, TInt aRightCharacter)
	{
	// All the characters join to the right, so we don't need to look them up.
	// as long as they are there at all, it is OK.
	TBool leftIsFixedJoiner = aLeftCharacter == 0x640 || aLeftCharacter == 0x200D;
	if (!leftIsFixedJoiner)
		{
		TInt leftIndex = ArabicShapeTableIndex(aLeftCharacter);
		if (leftIndex == -1)
			return EFalse;
		if (static_cast<TInt>(TheArabicShapeTable[leftIndex + KFinal])
			== aLeftCharacter)
			return EFalse;
		}
	if (aRightCharacter == 0x640 || aRightCharacter == 0x200D)
		return !leftIsFixedJoiner;
	TInt rightIndex = ArabicShapeTableIndex(aRightCharacter);
	return rightIndex != -1
		&& static_cast<TInt>(TheArabicShapeTable[rightIndex + KInitial])
			!= aRightCharacter;
	}

