// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code
*/

#include "tglyphatlas.h"
#include "TFBS.H"
#include "glyphatlastestwrapper.h"

_LIT(KTypefaceName, "DejaVu Sans Condensed");
_LIT(KMonoTypefaceName, "DejaVu Sans Mono");
const TInt KNumGlyphCodesLatin = 96;


CTGlyphAtlas::CTGlyphAtlas(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iFbs(NULL),
	iTs(NULL),
	iGlyphCodesLatin(NULL),
	iFont(NULL)
	{
	}

void CTGlyphAtlas::ConstructL()
	{
	iFbs = RFbsSession::GetSession();
	iTs = (CFbsTypefaceStore*)CFbsTypefaceStore::NewL(NULL);
	User::LeaveIfError(iTs->GetNearestFontToDesignHeightInPixels((CFont*&)iFont, TFontSpec(KTypefaceName, 15)));
	User::LeaveIfError(iTs->GetNearestFontToDesignHeightInPixels((CFont*&)iFont2, TFontSpec(KMonoTypefaceName, 8)));
	
	iGlyphCodesLatin = new(ELeave) TUint[KNumGlyphCodesLatin];
	for (TInt ii = 0; ii < KNumGlyphCodesLatin; ++ii)
		{
		iGlyphCodesLatin[ii] = ii+32; // ASCII characters from 0020 to 007F
		}

	INFO_PRINTF1(_L("FBSERV Glyph Atlas Testing"));
	}

CTGlyphAtlas::~CTGlyphAtlas()
	{
	if (iTs)
		{
		iTs->ReleaseFont(iFont);
		}
	delete iTs;
	delete[] iGlyphCodesLatin;
	}

void CTGlyphAtlas::RunTestCaseL(TInt aCurTestCase)
	{
	((CTGlyphAtlasStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
		((CTGlyphAtlasStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0663"));
		TestFullCache();
		break;
	case 2:
		((CTGlyphAtlasStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0664"));
		TestFontReleased();
		break;
	default:
		((CTGlyphAtlasStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTGlyphAtlasStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();		
		break;
		}
	((CTGlyphAtlasStep*)iStep)->RecordTestResultL();
	}

/**
@SYMTestCaseID		GRAPHICS-FBSERV-0663
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Shows that when the glyph atlas reaches its memory limit, any new added 
	glyphs will cause the eviction of the least recently used glyphs.
	The new glyph will be added successfully.

@SYMTestActions
	i. Create a glyph atlas with a memory limit of 1000 bytes.
	ii. Add glyphs to the atlas such that its memory consumption reaches the limit.
	iii. Add one more glyph.
	iv. Call CGlyphAtlas::GetGlyph() for the last glyph added.
	v. Call CGlyphAtlas::GetGlyph() for the least recently used glyphs to check 
		that it has been evicted.
	vi. Delete glyph atlas

@SYMTestExpectedResults
	Each glyph is added successfully.
	CGlyphAtlas::GetGlyph() returns KErrNone for the last glyph added.
	CGlyphAtlas::GetGlyph() returns KErrNotFound for the least recently used glyph.
*/
void CTGlyphAtlas::TestFullCache()
	{
	INFO_PRINTF1(_L("Test full cache eviction"));

	__UHEAP_MARK;
	// Fill cache up using expected size of glyph using AddGlyph.
	const TInt KMaxAtlasSizeInBytes = 1000;
	CGlyphAtlasTestWrapper* atlas = NULL;
	TRAPD(ret, atlas = CGlyphAtlasTestWrapper::NewL(KMaxAtlasSizeInBytes));
	TESTNOERROR(ret);
	if (KErrNone != ret)
		{
		return;
		}
	TOpenFontCharMetrics charMetrics;
	TGlyphImageInfo imageInfo;
	TSize bitmapSize;
	const TUint8* bitmapData = NULL;
	TInt glyphIndex = 0;
	TBool atlasFull = EFalse;
	CBitmapFont* bmFont = CTFbsFont::FontAddress(iFont);
	// Fill up atlas by adding glyphs.
	// Next glyph shold tip the atlas over the memory limit.
	// Glyphs are added in ascending glyph code order.
	// Leave at least one glyph so that we can guarantee that we can add one more unique glyph.
	while (glyphIndex < KNumGlyphCodesLatin-1 && !atlasFull)
		{
		iFont->GetCharacterData(iGlyphCodesLatin[glyphIndex], charMetrics, bitmapData, bitmapSize);
		TInt sizeInBytes = charMetrics.Height() * charMetrics.Width();
		if (atlas->SizeInBytes() + sizeInBytes <= KMaxAtlasSizeInBytes)
			{
			CGlyphAtlas::TAddGlyphArgs args(bitmapData, iGlyphCodesLatin[glyphIndex++], charMetrics);
			TESTNOERROR(atlas->AddGlyph(*bmFont, args, imageInfo));
			}
		else
			{
			atlasFull = ETrue;
			}
		}
	TEST(atlasFull);

	// check least recently used page contains the first glyph in glyph codes
	TUint leastUsedGlyphCode = iGlyphCodesLatin[0];
	TEST(atlas->LruPageContainsGlyph(leastUsedGlyphCode));

	// To ensure that the test does not pass if a FIFO eviction policy occurs,
	// get the least recently used glyph so that it is moved internally.
	TESTNOERROR(atlas->GetGlyph(*bmFont, leastUsedGlyphCode, imageInfo));

	// glyphIndex, bitmapData and charMetrics now current for next glyph which 
	// will take the atlas over the cache limit.
	CGlyphAtlas::TAddGlyphArgs args(bitmapData, iGlyphCodesLatin[glyphIndex], charMetrics);
	TESTNOERROR(atlas->AddGlyph(*bmFont, args, imageInfo));

	// check that searching for most recently added glyph is successful
	TGlyphImageInfo newInfo;
	TESTNOERROR(atlas->GetGlyph(*bmFont, iGlyphCodesLatin[glyphIndex], newInfo));

	// check atlas size is still under the limit
	TEST(atlas->SizeInBytes() <= KMaxAtlasSizeInBytes);

	// check that the pages which were evicted contained the least used glyphs
	// i.e. searching for these returns KErrNotFound
	TInt err = KErrNotFound;
	TInt index = 1;
	for (; KErrNotFound == err && index <= glyphIndex; ++index)
		{
		err = atlas->GetGlyph(*bmFont, iGlyphCodesLatin[index], newInfo);
		}
	TESTNOERROR(err);
	// first found glyph should be greater than glyph at index 1
	TEST(index-1 > 1);
	
	// check that the remaining pages contained the least used glyphs
	for (; index <= glyphIndex; ++index)
		{
		TESTNOERROR(atlas->GetGlyph(*bmFont, iGlyphCodesLatin[index], newInfo));
		}

	delete atlas;
	__UHEAP_MARKEND;
	}


/**
@SYMTestCaseID		GRAPHICS-FBSERV-0664
@SYMTestPriority	High
@SYMTestType		UT
@SYMTestStatus		Implemented
@SYMPREQ			PREQ2678

@SYMTestCaseDesc
	Shows that CGlyphAtlas::FontReleased() does actually delete all the glyphs 
	associated with the released font and leaves glyphs associated with other 
	fonts untouched.

@SYMTestActions
	i. Create a glyph atlas with no memory limit.
	ii. Add glyphs for two different fonts to the atlas.
	iii. Check all glyphs for both fonts were successfully added.
	iv. Call CGlyphAtlas::ReleaseFont for one of the fonts.
	v. Check that there are no glyphs associated with the released font.
	vi. Call CGlyphAtlas::ReleaseFont for the remaining font.
	vii Check that there are no glyphs associated with the released font and 
		that the atlas is empty.
	viii Delete the glyph atlas.
	
@SYMTestExpectedResults
	After each font is released, there are no glyphs associated with that font 
	left in the atlas.
*/
void CTGlyphAtlas::TestFontReleased()
	{
	INFO_PRINTF1(_L("Test behaviour of CGlyphAtlas::FontReleased()"));

	__UHEAP_MARK;
	CGlyphAtlasTestWrapper* atlas = NULL;
	TRAPD(ret, atlas = CGlyphAtlasTestWrapper::NewL(KGlyphAtlasNoCacheLimit));
	TESTNOERROR(ret);
	if (KErrNone != ret)
		{
		return;
		}
	TOpenFontCharMetrics charMetrics;
	TGlyphImageInfo imageInfo;
	TSize bitmapSize;
	const TUint8* bitmapData = NULL;
	CBitmapFont* bmFont = CTFbsFont::FontAddress(iFont);
	CBitmapFont* bmFont2 = CTFbsFont::FontAddress(iFont2);
	for (TInt glyphIndex = 0; glyphIndex < KNumGlyphCodesLatin; ++glyphIndex)
		{
		iFont->GetCharacterData(iGlyphCodesLatin[glyphIndex], charMetrics, bitmapData, bitmapSize);
		CGlyphAtlas::TAddGlyphArgs args(bitmapData, iGlyphCodesLatin[glyphIndex], charMetrics);
		TESTNOERROR(atlas->AddGlyph(*bmFont, args, imageInfo));

		iFont2->GetCharacterData(iGlyphCodesLatin[KNumGlyphCodesLatin-1-glyphIndex], charMetrics, bitmapData, bitmapSize);
		CGlyphAtlas::TAddGlyphArgs args2(bitmapData, iGlyphCodesLatin[glyphIndex], charMetrics);
		TESTNOERROR(atlas->AddGlyph(*bmFont2, args2, imageInfo));
		}
	// check there are font entries for these 2 fonts
	TEST(2 == atlas->FontCount());

	// check actual number of glyphs in atlas for each font is as expected
	TEST(KNumGlyphCodesLatin == atlas->GlyphCountByFont(bmFont));
	TEST(KNumGlyphCodesLatin == atlas->GlyphCountByFont(bmFont2));
	TEST(2*KNumGlyphCodesLatin == atlas->GlyphCount());

	// release one font and check number of glyphs in atlas for each font 
	// is as expected
	atlas->FontReleased(*bmFont);
	TEST(1 == atlas->FontCount());
	TEST(0 == atlas->GlyphCountByFont(bmFont));
	TEST(KNumGlyphCodesLatin == atlas->GlyphCountByFont(bmFont2));
	TEST(KNumGlyphCodesLatin == atlas->GlyphCount());

	// release one font and check number of glyphs in atlas for each font 
	// is as expected
	atlas->FontReleased(*bmFont2);
	TEST(0 == atlas->FontCount());
	TEST(0 == atlas->GlyphCountByFont(bmFont));
	TEST(0 == atlas->GlyphCountByFont(bmFont2));
	TEST(0 == atlas->GlyphCount());

	delete atlas;

	__UHEAP_MARKEND;
	}



//--------------
__CONSTRUCT_STEP__(GlyphAtlas)
