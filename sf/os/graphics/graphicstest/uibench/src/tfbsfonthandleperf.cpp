// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// fbsfonthandleperf.cpp
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "tfbsfonthandleperf.h"
#include <linkedfonts.h>

const TInt KIterationsToTest = 150;
const TInt KShortIterationsToTest = 100;
const TInt KMinimumIterations = 51;
const TInt KEqual = 0;

// Define font typefaces to test  
_LIT(KTestOpenFontName,"DejaVu Sans Condensed");
_LIT(KTestFontTypefaceFnt,"fnt");
_LIT(KTestInvalidFontName,"Invalid");
_LIT(KHindiFontName,"Devanagari OT Eval");
_LIT(KHindiShort,"\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020"); // 9 characters
_LIT(KEnglishShort,"The quick"); //9 characters
_LIT(K4thFontText,"HALGEBICK"); //9 characters from LinkedFont1
_LIT(KWithoutlinkedfont,"Withoutlinkedfont");

// hindi sample text - these have repeat characters - alternative phrases - use number 9 since it is the longest
// sample 1
#if 0
_LIT16(KHindi1,"\x0915\x094D\x0937\x0924\x094D\x0930\x093F\x092F\x0020\x0909\x0926\x094D\x0926\x0947\x0936\x094D\x0020\x0915\x094D\x0937\x093F\x092A\x094D\x0930");
_LIT16(KHindi2,"\x092d\x0942\x0930\x094d\x092d\x0941\x0935\x0903\x0020\x0938\x094d\x0935\x0903\x0020\x0924\x0924\x094d\x0938\x0935\x093f\x0924\x0941\x0930\x094d\x0935\x0930\x0947\x0923\x094d\x092f\x0902");
_LIT16(KHindi3,"\x092d\x0930\x094d\x0917\x094b\x0020\x0926\x0947\x0935\x0938\x094d\x092f\x0020\x0927\x0940\x092e\x0939\x093f\x0020\x0927\x093f\x092f\x094b\x0020\x092f\x094b\x0928\x0903\x0020\x092a\x094d\x0930\x091a\x094b\x0926\x092f\x093e\x0924\x094d\x0020");

// sample 2
_LIT16(KHindi4,"\x092f\x0924\x094d\x0930\x0020\x092f\x094b\x0917\x0947\x0936\x094d\x0935\x0930\x0020\x0915\x0943\x0937\x094d\x0923\x094b\x0020\x092f\x0924\x094d\x0930\x0020\x092a\x093e\x0930\x094d\x0925\x094b\x0020\x0927\x0930\x094d\x0928\x0941\x0927\x0930\x0903");
_LIT16(KHindi5,"\x0924\x0924\x094d\x0930\x0020\x0936\x094d\x0930\x0940\x0930\x094d\x0935\x093f\x091c\x092f\x094b\x0020\x092d\x0942\x0924\x093f\x0930\x094d\x0927\x0941\x0935\x093e\x0020\x0928\x0940\x0924\x093f\x0930\x094d\x092e\x0924\x093f\x0930\x094d\x092e\x092e");
_LIT16(KHindi6,"\x0915\x0941\x0924\x0938\x094d\x0924\x094d\x0935\x093e\x0020\x0915\x0936\x094d\x092e\x0932\x092e\x093f\x0926\x0902\x0020\x0935\x093f\x0936\x092e\x0947\x0020\x0938\x092e\x0941\x092a\x0938\x094d\x0925\x093f\x0924\x092e\x094d");

// sample 3
_LIT16(KHindi7,"\x0905\x0928\x093e\x0930\x094d\x092f\x091c\x0941\x0937\x094d\x091f\x092e\x0938\x094d\x0935\x0930\x094d\x0917\x094d\x092f\x092e\x0915\x0940\x0930\x094d\x0924\x093f\x0915\x0930\x092e\x0930\x094d\x091c\x0941\x0928");
_LIT16(KHindi8,"\x0915\x094d\x0932\x0948\x092c\x094d\x092f\x0902\x0020\x092e\x0020\x0938\x094d\x092e\x0020\x0917\x092e\x0903\x0020\x092a\x093e\x0930\x094d\x0925\x0928\x0948\x0924\x0924\x094d\x0924\x094d\x0935\x092f\x094d\x092f\x0941\x092a\x092a\x0926\x094d\x092f\x0924\x0947");
#endif

_LIT16(KHindi9,"\x0915\x094d\x0937\x0941\x0926\x094d\x0930\x0902\x0020\x0939\x0943\x0926\x092f\x0926\x094c\x0930\x094d\x092c\x0932\x094d\x092f\x0902\x0020\x0924\x094d\x092f\x0915\x094d\x0924\x094d\x0935\x094b\x0924\x094d\x0924\x093f\x0937\x094d\x091f\x0020\x092a\x0930\x0902\x0924\x092a");//45 characters
_LIT(KEnglishLong,"The quick brown fox jumped over the lazy dog.");//45 characters


/** Information about linked fonts
TLinkedFont1.gdr
Typeface name: LinkedFont1
This will be the dominant font. It contains 177 characters.
Some of the characters (13) are deleted from this file and separated in to other three files.
Characters missing are F, D, J, M, P, S, V, X, Z, a, n, l, y.

TLinkedFont2.gdr
Typeface name: LinkedFont2
Characters available (7):	F, J, M, S, Z, a, y.

TLinkedFont3.gdr
Typeface name: LinkedFont3
Characters available (4):	D, P, x, l

TLinkedFont4.gdr
Typeface name: LinkedFont4
Characters available (2):	V, n

Withoutlinkedfont.gdr
Typeface name: Withoutlinkedfont
Characters available:	It contains all 190 characters
*/
//Define fonts to test
_LIT(KDummy, "Dummy");
_LIT(KDummyBold, "Dummy Bold");
_LIT(KDummyItalic, "Dummy Italic");
_LIT(KDummyBoldItalic, "Dummy Bold Italic");

//Define font paths to test
_LIT(KFontDummy,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummyFont");
_LIT(KFontDummy_b,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummyFont_b");
_LIT(KFontDummy_i,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummyFont_i");
_LIT(KFontDummy_bi,"z:\\PlatTest\\Graphics\\TestData\\dummy_fonts\\dummyFont_bi");
_LIT(KBitmapFont,"z:\\PlatTest\\Graphics\\TestData\\ceurope.gdr");

//Define Test case names
_LIT(KAddRemoveOpenFontFiles, "Add-and-remove-'OpenFont'-files");
_LIT(KAddOpenedOpenFontFiles, "Add-opened-'OpenFont'-files");
_LIT(KAddRemoveBitmapFontFiles, "Add-and-remove-bitmap-font-files");
_LIT(KAddOpenedBitmapFontFiles, "Add-opened-bitmap-font-files");
_LIT(KFindFontSize, "FontSize");
_LIT(KAliasedFont,"PerftestAliasedFont");
_LIT(KFontSansSerif,"DejaVu Sans Condensed");
_LIT(KFontCreationSansSerif,"FontCreationSansSerif");
_LIT(KFontCreationAliasedFont,"FontCreationAliasedFont");

//*********** DummyFontFile *****************//
/* DummyFontFile class is used to create the Dummy Font File without actually
creating the Font.  It is used in AddAndRemoveFilesL() testcase.
*/
CDummyFontFile::CDummyFontFile(TInt aUid, const TDesC& aFileName):
	COpenFontFile(aUid, aFileName)
	{
	}

CDummyFontFile* CDummyFontFile::NewL(TInt aUid, const TDesC& aFileName)
	{
	CDummyFontFile* f = new(ELeave)CDummyFontFile(aUid, aFileName);
	CleanupStack::PushL(f);
	f->ConstructL();
	CleanupStack::Pop();
	return f;
	}

CDummyFontFile::~CDummyFontFile()
	{
	iFile.Close();
	iFs.Close();
	}

//*********** Pure Virtual Functions of COpenFontFile **********//
void CDummyFontFile::GetNearestFontInPixelsL(RHeap* aHeap,
                                             COpenFontSessionCacheList* aSessionCacheList,
                                             const TOpenFontSpec& aDesiredFontSpec,
                                             TInt aPixelWidth,
                                             TInt aPixelHeight,
                                             COpenFont*& aFont,
                                             TOpenFontSpec& aActualFontSpec)

	{
	aFont = NULL;
	TInt face_index = 0;
	if (GetNearestFontHelper(aDesiredFontSpec,aPixelWidth,aPixelHeight,face_index,aActualFontSpec))
		{
		aFont = CDummyFont::NewL(aHeap,aSessionCacheList,this,aActualFontSpec.Height());
		}
	}

TBool CDummyFontFile::HasUnicodeCharacterL(TInt /*aFaceIndex*/, TInt /*aCode*/) const
	{
	return EFalse; // this dummy font has no characters
	}

//---------------------------------------------Pure Virtual Functions---------//
void CDummyFontFile::ConstructL()
	{
	const TDesC& filename = FileName();

	// for comparative benchmarking at least open the file
	User::LeaveIfError(iFs.Connect());
	const TInt openError = iFile.Open(iFs, filename, EFileRead | EFileShareReadersOnly);
	User::LeaveIfError(openError);

	// check that file is > 0 bytes
	TInt fontFileSize = 0;
	User::LeaveIfError(iFile.Size(fontFileSize));
	if (fontFileSize == 0)
		{
		User::Leave(KErrEof);
		}

	TOpenFontFaceAttrib attrib;

	if (KEqual == filename.CompareF(KFontDummy))
		attrib.SetFullName(KDummy);
	else if (KEqual == filename.CompareF(KFontDummy_b))
		{
		attrib.SetFullName(KDummyBold);
		attrib.SetBold(TRUE);
		}
	else if (KEqual == filename.CompareF(KFontDummy_i))
		{
		attrib.SetFullName(KDummyItalic);
		attrib.SetItalic(TRUE);
		}
	else if (KEqual == filename.CompareF(KFontDummy_bi))
		{
		attrib.SetFullName(KDummyBoldItalic);
		attrib.SetBold(TRUE);
		attrib.SetItalic(TRUE);
		}
	attrib.SetFamilyName(KDummy);
	attrib.SetLocalFullName(attrib.FullName());
	attrib.SetCoverage(TOpenFontFaceAttrib::ELatinSet);
	attrib.SetMinSizeInPixels(8);
	
	AddFaceL(attrib);
	}

//*------------------------------------DummyFontFile--*/

//************ DummyRasterizer***************//
/* DummyRasterizer class is used in AddAndRemoveFilesL() Test case to use the
Dummy Rasterizer without actually installing the system rasterizer.
*/
CDummyRasterizer* CDummyRasterizer::NewL()
	{
	return new(ELeave) CDummyRasterizer;
	}

COpenFontFile* CDummyRasterizer::NewFontFileL(TInt aUid,
                                              const TDesC& aFileName,
                                              RFs& /*aFileSession*/)
	{
	// Allow 'dummy' 'dummy_bold' 'dummy_italic' 'dummy_bold_italic'. 
	//Don't actually open a file; just create a dummy font.
	if (KEqual == aFileName.CompareF(KFontDummy)||
		KEqual == aFileName.CompareF(KFontDummy_b)||
		KEqual == aFileName.CompareF(KFontDummy_i)||
		KEqual == aFileName.CompareF(KFontDummy_bi))
		{
		return CDummyFontFile::NewL(aUid,aFileName);
		}
	return NULL;
	}
//*------------------------------------DummyRasterizer--*//

//************ DummyFont***************//
/*
DummyFont is used to create a Dummy Font without actually installing it.
It is used by AddAndRemoveFilesL() Testcase.
*/
CDummyFont* CDummyFont::NewL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
							 CDummyFontFile* aFontFile,TInt aSizeInPixels)
	{
	CDummyFont* f = reinterpret_cast<CDummyFont*>(aHeap->AllocL(sizeof(CDummyFont)));
	new(f) CDummyFont(aHeap,aSessionCacheList,aFontFile,aSizeInPixels);
	return f;
	}

CDummyFont::CDummyFont(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,CDummyFontFile* aFontFile,
					   TInt aSizeInPixels):
	COpenFont(aHeap,aSessionCacheList,aFontFile)
	{
	iMetrics.SetSize(aSizeInPixels);
	iMetrics.SetAscent(aSizeInPixels * 3 / 4);
	iMetrics.SetDescent(aSizeInPixels - iMetrics.Ascent());
	iMetrics.SetMaxHeight(iMetrics.Ascent());
	iMetrics.SetMaxDepth(iMetrics.Descent());
	iMetrics.SetMaxWidth(aSizeInPixels * 2);
	}

void CDummyFont::RasterizeL(TInt /*aCode*/,TOpenFontGlyphData* /*aGlyphData*/)
	{
	User::Leave(KErrNotSupported); // this dummy font has no glyphs
	}

//*------------------------------------DummyFont--*//

CTFbsFontHandlePerf::CTFbsFontHandlePerf()
	{
	SetTestStepName(KTFbsFontHandlePerfName);
	}

TVerdict CTFbsFontHandlePerf::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	SetScreenModeL(EColor16MAP);
	iDevice = &iScreenDevice->BitmapDevice();
	return TestStepResult();
	}

/**
Override of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.

@return - TVerdict code
*/
TVerdict CTFbsFontHandlePerf::doTestStepL()
	{	
	// Created Linked font with only one typeface
	_LIT(KLinkedTypeface,"LinkedTypefaceTest");
	CLinkedTypefaceSpecification *typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedTypeface);

	const TInt KGroup1Id = 1;	
	CLinkedTypefaceGroup* group1 = CLinkedTypefaceGroup::NewLC(KGroup1Id);
	typefaceSpec->AddLinkedTypefaceGroupL(*group1);
	CleanupStack::Pop(group1);
		
	CLinkedTypefaceElementSpec* element1 = CLinkedTypefaceElementSpec::NewLC(KTestOpenFontName, KGroup1Id);
	element1->SetCanonical(ETrue);
	
	typefaceSpec->AddTypefaceAtIndexL(*element1, 0);
	CleanupStack::Pop(element1);
	
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(iDevice);
	CleanupStack::PushL(ts);
	TInt ret;
	ret = ts->RegisterLinkedTypeface(*typefaceSpec);
	CleanupStack::PopAndDestroy(2, typefaceSpec);
	if (ret == KErrNotSupported)
		{
		//If the rasterizer does not support the linked fonts feature do not continue the test:
		//log and do not fail test.
		INFO_PRINTF1(_L("Linked fonts feature is not supported by rasterizer"));
		}
	else
		{
		TEST((ret == KErrNone) || (ret == KErrAlreadyExists));

		_LIT(KLinkedTypefaceHindi,"HindiLinked");
		typefaceSpec = CLinkedTypefaceSpecification::NewLC(KLinkedTypefaceHindi);

		group1 = CLinkedTypefaceGroup::NewLC(KGroup1Id);
		typefaceSpec->AddLinkedTypefaceGroupL(*group1);
		CleanupStack::Pop(group1);
		
		element1 = CLinkedTypefaceElementSpec::NewLC(KHindiFontName, KGroup1Id);
		element1->SetCanonical(ETrue);
		typefaceSpec->AddTypefaceAtBackL(*element1);
		CleanupStack::Pop(element1);
		
		ts = CFbsTypefaceStore::NewL(iDevice);
		CleanupStack::PushL(ts);
		ret = ts->RegisterLinkedTypeface(*typefaceSpec);
		TEST((ret == KErrNone) || (ret == KErrAlreadyExists));
		CleanupStack::PopAndDestroy(2, typefaceSpec);
		
		//register a linked typeface with four fonts.
        _LIT(KFourLinked,"FourBitmapLinked");
        _LIT(KLinkedTypeFace1, "DejaVu Sans Condensed");
        _LIT(KLinkedTypeFace2, "Devanagari OT Eval");
        //_LIT(KLinkedTypeFace3, "TBKomachiG-R"); // This font is not included in uibench roms.
		_LIT(KLinkedTypeFace3, "DejaVu Sans Mono");
        _LIT(KLinkedTypeFace4, "DejaVu Serif Condensed");

		typefaceSpec = CLinkedTypefaceSpecification::NewLC(KFourLinked);

		group1 = CLinkedTypefaceGroup::NewLC(KGroup1Id);
		typefaceSpec->AddLinkedTypefaceGroupL(*group1);
		CleanupStack::Pop(group1);
		
		const TInt KGroup2Id = 3;	
		CLinkedTypefaceGroup* group2 = CLinkedTypefaceGroup::NewLC(KGroup2Id);
		typefaceSpec->AddLinkedTypefaceGroupL(*group2);
		CleanupStack::Pop(group2);
		
		const TInt KGroup3Id = 4;	
		CLinkedTypefaceGroup* group3 = CLinkedTypefaceGroup::NewLC(KGroup3Id);
		typefaceSpec->AddLinkedTypefaceGroupL(*group3);
		CleanupStack::Pop(group3);
		
		element1 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypeFace1, KGroup1Id);
		element1->SetCanonical(ETrue);
		typefaceSpec->AddTypefaceAtBackL(*element1);
		CleanupStack::Pop(element1);
		
		CLinkedTypefaceElementSpec* element2 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypeFace2, KGroup2Id);
		typefaceSpec->AddTypefaceAtBackL(*element2);
		CleanupStack::Pop(element2);
		
		CLinkedTypefaceElementSpec* element3 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypeFace3, KGroup3Id);
		typefaceSpec->AddTypefaceAtBackL(*element3);
		CleanupStack::Pop(element3);
		
		CLinkedTypefaceElementSpec* element4 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypeFace4, KGroup3Id);
		typefaceSpec->AddTypefaceAtBackL(*element4);
		CleanupStack::Pop(element4);

		ts = CFbsTypefaceStore::NewL(iDevice);
		CleanupStack::PushL(ts);
		ret = ts->RegisterLinkedTypeface(*typefaceSpec);
		TEST((ret == KErrNone) || (ret == KErrAlreadyExists));
		CleanupStack::PopAndDestroy(2, typefaceSpec);
		
		_LIT(KOneLinked,"OneLinked");
		typefaceSpec = CLinkedTypefaceSpecification::NewLC(KOneLinked);
		
		group1 = CLinkedTypefaceGroup::NewLC(KGroup1Id);
		typefaceSpec->AddLinkedTypefaceGroupL(*group1);
		CleanupStack::Pop(group1);
		
		element1 = CLinkedTypefaceElementSpec::NewLC(KLinkedTypeFace1, KGroup1Id);
		element1->SetCanonical(ETrue);
		typefaceSpec->AddTypefaceAtBackL(*element1);
		CleanupStack::Pop(element1);
		
		ts = CFbsTypefaceStore::NewL(iDevice);
		CleanupStack::PushL(ts);
		TRAP(ret, typefaceSpec->RegisterLinkedTypefaceL(*ts));
		TEST((ret == KErrNone) || (ret == KErrAlreadyExists));
		
		TRAP(ret, typefaceSpec->UpdateLinkedTypefaceL(*ts));
		
		TEST(ret == KErrNone);
		
		CleanupStack::PopAndDestroy(2, typefaceSpec);

	/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0052

	@SYMTestType UT

	@SYMTestPriority Critical

	@SYMPREQ PREQ1431

	@SYMTestCaseDesc 
	Test performance timing of linked font creation

	@SYMTestActions 
	Create a linked font from linked typeface with 1 component font. 

	@SYMTestExpectedResults 
	Measure performance compared with non-linked font
	*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0052"));
	
	FontGetNearestFontToDesignHeightInPixelsL(KLinkedTypeface, _L("Font-GetNearest-1stLinkedTypeface-Open"));
	TInt getNearestFont1stLinkedTime = iProfiler->GetTrimedMean();

	FontGetNearestFontToDesignHeightInPixelsL(KFourLinked, _L("Font-GetNearest-4LinkedTypeface-Bitmap"));
	TInt getNearestFont4thLinkedTime = iProfiler->GetTrimedMean();
	
	FontGetNearestFontToDesignHeightInPixelsL(KOneLinked, _L("Font-GetNearest-1LinkedTypeface-Bitmap"));
	TInt getNearestFont1stBitmapLinkedTime = iProfiler->GetTrimedMean();
	RecordTestResultL();
	/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0053

	@SYMTestType UT

	@SYMTestPriority Critical

	@SYMPREQ PREQ1431

	@SYMTestCaseDesc 
	Test performance timing of linked font character access without glyph cache

	@SYMTestActions 
	Check performance on first access to character in first font of linked font where same character in component font has not yet been accessed

	@SYMTestExpectedResults 
	Measure performance compared with non-linked font access
	*/
		// These must be first to avoid using glyph cache
		FontDrawStringWithoutGlyphCacheL(KLinkedTypeface, _L("Font-Drawtext-NoCache-1stLinkedTypeface-Open"), 10, KEnglishShort);
		TInt drawText1stLinkedWithoutGlyphCacheTime = iProfiler->GetTrimedMean();
		
		FontDrawStringWithoutGlyphCacheL(KTestOpenFontName, _L("Font-Drawtext-NoCache-Non-LinkedTypeface-Open"), 11, KEnglishShort);
		TInt drawTextNonLinkedWithoutGlyphCacheTime = iProfiler->GetTrimedMean();	

		INFO_PRINTF2(_L("--Font linking DrawText without glyph cache performance compared with non-linked font: %d percent"), iProfiler->PercentageChange(drawTextNonLinkedWithoutGlyphCacheTime, drawText1stLinkedWithoutGlyphCacheTime));
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0053"));

		//now testing complex fonts.	
		INFO_PRINTF1(_L("Hindi Text output - without Glyph cache"));
		FontDrawStringWithoutGlyphCacheL(KLinkedTypefaceHindi, _L("Font-Drawtext-NoCache-Complex-Linked-Open"), 10, KHindiShort);
		TInt drawTextHindiLinkedWithoutGlyphCacheTime = iProfiler->GetTrimedMean();
		
		FontDrawStringWithoutGlyphCacheL(KHindiFontName, _L("Font-Drawtext-NoCache-Non-Linked-Complex-Open"), 11, KHindiShort);
		TInt drawTextHindiNonLinkedWithoutGlyphCacheTime = iProfiler->GetTrimedMean();	
		INFO_PRINTF2(_L("--Complex Font linking DrawText without glyph cache performance compared with non-linked font: %d percent"), iProfiler->PercentageChange(drawTextHindiNonLinkedWithoutGlyphCacheTime, drawTextHindiLinkedWithoutGlyphCacheTime));

		//Compare the difference of complex and normal fonts.	
		INFO_PRINTF2(_L("Time difference between complex font and regular font (non linked) %d percent"), iProfiler->PercentageChange(drawTextNonLinkedWithoutGlyphCacheTime,drawTextHindiLinkedWithoutGlyphCacheTime));

		//try drawing text with bitmap fonts
		FontDrawStringWithoutGlyphCacheL(KFourLinked, _L("Font-Drawtext-NoCache-Linked Typeface 4th Bitmap"), 11, K4thFontText);
		TInt drawTextWithoutGlyphCacheTime4thBitmap = iProfiler->GetTrimedMean();
		
		//for comparision - without any linking
		FontDrawStringWithoutGlyphCacheL(KWithoutlinkedfont, _L("Font-Drawtext-No Cache-Non Linked Typeface"), 11, K4thFontText);
		TInt drawTextWithoutGlyphCacheTimeAllBitmap = iProfiler->GetTrimedMean();	
		
		//for comparision - linked, with one linked font
		FontDrawStringWithoutGlyphCacheL(KOneLinked, _L("Font-Drawtext-No Cache Linked Typeface 1st Bitmap"), 11, K4thFontText);
		TInt drawTextWithoutGlyphCacheTime1stBitmap = iProfiler->GetTrimedMean();
		
		//Compare the difference of linked and non linked fonts.	
		INFO_PRINTF2(_L("Non cached Time difference between non linked font and 4th font down (bitmap) %d percent"), iProfiler->PercentageChange(drawTextWithoutGlyphCacheTimeAllBitmap,drawTextWithoutGlyphCacheTime4thBitmap));

		//Compare the difference of linked and lined 4th font down.	
		INFO_PRINTF2(_L("Non cached Time difference between 1st linked font and 4th font down (bitmap) %d percent"), iProfiler->PercentageChange(drawTextWithoutGlyphCacheTime1stBitmap,drawTextWithoutGlyphCacheTime4thBitmap));

		RecordTestResultL();
	/**
	@SYMTestCaseID
	GRAPHICS-UI-BENCH-0054

	@SYMTestType UT

	@SYMTestPriority Critical

	@SYMPREQ PREQ1431

	@SYMTestCaseDesc 
	Test performance timing of linked font character access with glyph cache

	@SYMTestActions 
	Check performance on access to character in first font of linked font where same character in component font has been already accessed and stored in the glyph cache

	@SYMTestExpectedResults 
	Measure performance compared with non-linked font access
	*/	
		// Non-Linked font DrawText
		FontDrawTextGlyphCacheL(KLinkedTypeface, _L("Font-Drawtext-Cache-1stLinkedTypeface-Open"),KEnglishLong);
		TInt drawText1stLinkedWithGlyphCacheTime = iProfiler->GetTrimedMean();	

	// Linked font DrawText
		SetTestStepID(_L("GRAPHICS-UI-BENCH-0054"));
		FontDrawTextGlyphCacheL(KTestOpenFontName, _L("Font-Drawtext-Cache-NonLinked-Open"),KEnglishLong);
		TInt drawTextNonLinkedWithGlyphCacheTime = iProfiler->GetTrimedMean();	
		INFO_PRINTF2(_L("--Font linking DrawText with glyph cache performance compared with non-linked font: %d percent"), iProfiler->PercentageChange(drawTextNonLinkedWithGlyphCacheTime, drawText1stLinkedWithGlyphCacheTime));

		//Now Hindi with and without
		FontDrawTextGlyphCacheL(KLinkedTypefaceHindi, _L("Font-Drawtext-Cache-1stLinkedTypeface-Complex-Open"),KHindi9);
		TInt drawText1stComplexLinkedWithGlyphCacheTime = iProfiler->GetTrimedMean();	

		// Non-Linked font DrawText
		FontDrawTextGlyphCacheL(KHindiFontName, _L("Font-Drawtext-Cache-NonLinked-Complex-Open"),KHindi9);
		TInt drawTextComplexNonLinkedWithGlyphCacheTime = iProfiler->GetTrimedMean();	
		INFO_PRINTF2(_L("--Font complex font linking DrawText with glyph cache performance compared with non-linked font: %d percent"), iProfiler->PercentageChange(drawTextComplexNonLinkedWithGlyphCacheTime, drawText1stComplexLinkedWithGlyphCacheTime));

		//Compare Difference between complex font and normal font
		INFO_PRINTF2(_L("--Font complex font not linked DrawText with glyph cache performance compared with non-complex font non linked: %d percent"), iProfiler->PercentageChange(drawTextNonLinkedWithGlyphCacheTime, drawTextComplexNonLinkedWithGlyphCacheTime));
		
		//try drawing text with bitmap fonts
		FontDrawTextGlyphCacheL(KFourLinked, _L("Font-Drawtext-Cache-Linked Typeface 4th Bitmap"), K4thFontText);
		TInt drawTextWithGlyphCacheTime4thBitmap = iProfiler->GetTrimedMean();
		
		//for comparision - non linked
		FontDrawTextGlyphCacheL(KWithoutlinkedfont, _L("Font-Drawtext-Cache-Non Linked Typeface text as for 4th Bitmap"), K4thFontText);
		TInt drawTextWithGlyphCacheTimeAllBitmap = iProfiler->GetTrimedMean();	
		
		//for comparision - linked at 1st position
		FontDrawTextGlyphCacheL(KOneLinked, _L("Font-Drawtext-Cache- Linked Typeface 1st Linked text as for 4th Bitmap"), K4thFontText);
		TInt drawTextWithGlyphCacheTime1stBitmap = iProfiler->GetTrimedMean();
		
		//Compare the difference of non linked 4th font in stack.	
		INFO_PRINTF2(_L("Cached Time difference between non linked and 4th font down (bitmap) %d percent"), iProfiler->PercentageChange(drawTextWithGlyphCacheTimeAllBitmap,drawTextWithGlyphCacheTime4thBitmap));

		//Compare the difference of 1st font in stack and 4th font in stack
		INFO_PRINTF2(_L("Cached Time difference between 1st linked font and 4th font down (bitmap) %d percent"), iProfiler->PercentageChange(drawTextWithGlyphCacheTime1stBitmap,drawTextWithGlyphCacheTime4thBitmap));
		
		//Compare the difference of non linked and 1st linked in stack	
		INFO_PRINTF2(_L("Cached Time difference between non linked and 1st font down (bitmap) %d percent"), iProfiler->PercentageChange(drawTextWithGlyphCacheTimeAllBitmap,drawTextWithGlyphCacheTime1stBitmap));

		RecordTestResultL();
				
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0008

@SYMTestCaseDesc
Fetches a font with a valid typeface

@SYMTestActions
Fetch a valid font using FontGetNearestFontToDesignHeightInPixels

@SYMTestExpectedResults
Measures performance of finding the font speicified
*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0008"));
	FontGetNearestFontToDesignHeightInPixelsL(KTestOpenFontName, _L("Font-GetNearest-ValidFont"));
	TInt getNearestFontNonLinkedTime = iProfiler->GetTrimedMean();
	INFO_PRINTF2(_L("--Font linking GetNearestFont compared with non-linked font: %d percent"), iProfiler->PercentageChange(getNearestFontNonLinkedTime, getNearestFont1stLinkedTime));
	INFO_PRINTF2(_L("--Font linking GetNearestFont 1 linked font compared with linked 4 fonts: %d percent"), iProfiler->PercentageChange(getNearestFont1stBitmapLinkedTime, getNearestFont4thLinkedTime));
	RecordTestResultL();
		}
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0009

@SYMTestCaseDesc
Fetches a font with an invalid typeface. 

@SYMTestActions
Fetch a invalid font using FontGetNearestFontToDesignHeightInPixels, this will find the nearest match.
On Techview this by default will give back a bitmap font which is much faster than an Open Font

@SYMTestExpectedResults
Measures performance of finding a font
*/			
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0009"));
	FontGetNearestFontToDesignHeightInPixelsL(KTestInvalidFontName, _L("Font-GetNearest-InvalidFont"));	
	RecordTestResultL();

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0010

@SYMREQ REQ6158

@SYMTestCaseDesc
Time to find nearest font when specifying a blank font with no default system typeface name specified

@SYMTestActions
Ensure no default system typeface name is set and measure time to fetch a blank font typeface name
On Techview this by default will give back a bitmap font "fnt"

@SYMTestExpectedResults
The time to fetch typeface name
*/					
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0010"));
	FontGetNearestFontToDesignHeightInPixelsL(KNullDesC, _L("Font-GetNearest-SystemDefaultNoDefault"));
	RecordTestResultL();

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0011

@SYMREQ REQ6158

@SYMTestCaseDesc
Time to find the nearest font when specifying a blank font with a default system typeface name specified

@SYMTestActions
Set default system typeface name and measure time to fetch a blank font typeface name
Find a nesrest match font using FontGetNearestFontToDesignHeightInPixels

@SYMTestExpectedResults
The time to fetch typeface name
*/						
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0011"));
	ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	ts->SetSystemDefaultTypefaceNameL(KTestFontTypefaceFnt);			
	FontGetNearestFontToDesignHeightInPixelsL(KNullDesC, _L("Font-GetNearest-SystemDefaultSetDefault"));	
	// Set system default font back to an emtpy descriptor i.e. turn it off
	ts->SetSystemDefaultTypefaceNameL(KNullDesC);
	CleanupStack::PopAndDestroy(ts);
	
	RecordTestResultL();
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0012

@SYMTestCaseDesc
Duplicated a font

@SYMTestActions
Duplicate a font

@SYMTestExpectedResults
Measures performance of duplicating a font
*/								
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0012"));
	FontDuplicateL();	
	RecordTestResultL();

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0048

@SYMDEF DEF091107

@SYMTestCaseDesc
Measure performance of adding and removing Bitmap font files from a font store.

@SYMTestActions
Measure the time to add a font file and remove the added font file, repeated 4 times.

@SYMTestExpectedResults
The time to add and remove a bitmap font file to the font store.
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0048"));
	AddAndRemoveFilesL(EFalse);
	RecordTestResultL();
 	
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0049

@SYMDEF DEF091107

@SYMTestCaseDesc
Measure performance of adding an already opened Bitmap font file to a font store.

@SYMTestActions
A Bitmap font is loaded into the Font Store.
Measure the time to add the same Bitmap font file again.

@SYMTestExpectedResults
The time to add an already opened Bitmap font file to the font store.
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0049"));
	AddingOpenedFontFilesL(EFalse);
	RecordTestResultL();

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0050

@SYMDEF DEF091107

@SYMTestCaseDesc
Measure performance of adding and removing Open Font files from a font store.

@SYMTestActions
Measure the time to add a (dummy) Open Font file and remove the added font file, repeated 4 times.

@SYMTestExpectedResults
The time to add and remove a dummy Open Font file to the font store.
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0050"));
	AddAndRemoveFilesL(ETrue);
	RecordTestResultL();
 	
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0051
 
@SYMDEF DEF091107
 
@SYMTestCaseDesc
Measure performance of adding an already opened Open Font file to a font store.

@SYMTestActions
A dummy Open Font is loaded into the Font Store.
Measure the time to add the same Open Font file again.
 	
@SYMTestExpectedResults
The time to add an already opened Open Font file to the font store.
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0051"));
	AddingOpenedFontFilesL(ETrue);
	RecordTestResultL();
	
/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0055

@SYMDEF DEF095589

@SYMTestCaseDesc
Measure performance of finding the fontsize.

@SYMTestActions
Measure the time to add 4 font files, find the fontsize and remove the 4 added font files.

@SYMTestExpectedResults
The time to add, find fontsize and remove a dummy font file to the font store.
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0055"));
	FindFontSizeL();
	RecordTestResultL();

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0138

@SYMDEF INC127988

@SYMTestCaseDesc
Measure performance of creating a non aliased font.

@SYMTestActions
Measure the time it takes to create a non-aliased font.

@SYMTestExpectedResults
The time to create a non aliased font is obtained.
*/	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0138"));
	NonAliasedFontCreationL();
	RecordTestResultL();

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0140

@SYMDEF INC127988

@SYMTestCaseDesc
Measure performance of creating an aliased font.

@SYMTestActions
Measure the time it takes to create an aliased font (aliased to KFontSansSerif).

@SYMTestExpectedResults
The time to create an aliased font is obtained.
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0140"));
	AliasedFontCreationL();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

// Measure performance for font duplication
void CTFbsFontHandlePerf::FontDuplicateL()
	{		
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	CFbsFont* font;
	TInt err=ts->GetNearestFontToDesignHeightInPixels((CFont*&)font, TFontSpec(KTestOpenFontName,15));
	User::LeaveIfError(err);
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		CFbsBitGcFont* duplicateFont=new (ELeave) CFbsBitGcFont;
		CleanupStack::PushL(duplicateFont);
		err=duplicateFont->Duplicate(font->Handle());
		User::LeaveIfError(err);
		CleanupStack::PopAndDestroy(duplicateFont);
		iProfiler->MarkResultSetL();
		}
	TInt64 duration=iProfiler->GetTrimedMean();
	iProfiler->ResultsAnalysis(_L("Font-Duplicate"), 0, 0, 0, KIterationsToTest);	
	ts->ReleaseFont(font);
	CleanupStack::PopAndDestroy(ts);
	}
	
// Measure performance using get nearest font
void CTFbsFontHandlePerf::FontGetNearestFontToDesignHeightInPixelsL(const TDesC& aFontName, const TDesC& aTestName)
	{	
	CFbsFont* font;
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		TInt err=ts->GetNearestFontToDesignHeightInPixels((CFont*&)font,TFontSpec(aFontName, 15));
		User::LeaveIfError(err);
		ts->ReleaseFont(font);	// Release font 
		iProfiler->MarkResultSetL();
		}
	TInt64 duration=iProfiler->GetTrimedMean();
	iProfiler->ResultsAnalysis(aTestName, 0, 0, 0, KIterationsToTest);	
	CleanupStack::PopAndDestroy(ts);
	}

// Add and remove files.
void CTFbsFontHandlePerf::AddAndRemoveFilesL(TBool aTestOpenFont)
	{
	RHeap* heap = UserHeap::ChunkHeap(NULL,0x10000,0x10000);
	if (NULL == heap)
		{
		_LIT(KLog,"Not enough memory to create heap for test");
		INFO_PRINTF1(KLog);
		User::Leave(KErrGeneral);
		}

	CFontStore* fontStore = CFontStore::NewL(heap);
	CleanupStack::PushL(fontStore);

	// Install the dummy rasterizer.
	COpenFontRasterizer* dummyRasterizer = CDummyRasterizer::NewL();
	CleanupStack::PushL(dummyRasterizer);
	fontStore->InstallRasterizerL(dummyRasterizer);
	CleanupStack::Pop();
	
	// Check font files exist before testing
	TRAPD(err, DoAddAndRemoveFilesL(aTestOpenFont, fontStore));
	if (err != KErrNone)
        {
        _LIT(KLog,"Failed to load font files. Error code: %d");
        INFO_PRINTF2(KLog,err);
        User::Leave(err);
        }
	
	// Perform the iterations to test
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
        DoAddAndRemoveFilesL(aTestOpenFont, fontStore);
		iProfiler->MarkResultSetL();
		}
	
	TInt64 duration=iProfiler->GetTrimedMean();
	if (aTestOpenFont)
		{
		iProfiler->ResultsAnalysis(KAddRemoveOpenFontFiles, 0, 0, 0, KIterationsToTest);
		}
	else
		{
		iProfiler->ResultsAnalysis(KAddRemoveBitmapFontFiles, 0, 0, 0, KIterationsToTest);
		}
	CleanupStack::PopAndDestroy(fontStore);

	heap->Close();
	}

// Do the add and removing files.
void CTFbsFontHandlePerf::DoAddAndRemoveFilesL(TBool aTestOpenFont, CFontStore* aFontStore)
    {
    if (aTestOpenFont)
        {
        //Add font files to fontstore
        TUid id1 = aFontStore->AddFileL(KFontDummy);
        TUid id2 = aFontStore->AddFileL(KFontDummy_b);
        TUid id3 = aFontStore->AddFileL(KFontDummy_i);
        TUid id4 = aFontStore->AddFileL(KFontDummy_bi);
        //Remove font files from fontstore
        aFontStore->RemoveFile(id1);
        aFontStore->RemoveFile(id2);
        aFontStore->RemoveFile(id3);
        aFontStore->RemoveFile(id4);
        }
    else
        {
        //Add & remove font file to fontstore a total of 4 times, for comparison with Open Font test
        TUid id1 = aFontStore->AddFileL(KBitmapFont);
        aFontStore->RemoveFile(id1);
        TUid id2 = aFontStore->AddFileL(KBitmapFont);
        aFontStore->RemoveFile(id2);
        TUid id3 = aFontStore->AddFileL(KBitmapFont);
        aFontStore->RemoveFile(id3);
        TUid id4 = aFontStore->AddFileL(KBitmapFont);
        aFontStore->RemoveFile(id4);
        }
    }

// Add fontfile that is already opened.
void CTFbsFontHandlePerf::AddingOpenedFontFilesL(TBool aTestOpenFont)
	{
	RHeap* heap = UserHeap::ChunkHeap(NULL,0x10000,0x10000);
	if (NULL == heap )
		{
		User::Leave(KErrGeneral);
		}

	CFontStore* fontStore = CFontStore::NewL(heap);
	CleanupStack::PushL(fontStore);

	// Install the dummy rasterizer.
	COpenFontRasterizer* dummyRasterizer = CDummyRasterizer::NewL();
	CleanupStack::PushL(dummyRasterizer);
	fontStore->InstallRasterizerL(dummyRasterizer);
	CleanupStack::Pop();

	// initial load of font file
	TUid id = KNullUid;
	if (aTestOpenFont)
		{
		id = fontStore->AddFileL(KFontDummy);
		}
	else
		{
		id = fontStore->AddFileL(KBitmapFont);
		}
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		//Add font file to fontstore again
		if (aTestOpenFont)
			{
			(void) fontStore->AddFileL(KFontDummy);
			}
		else
			{
			(void) fontStore->AddFileL(KBitmapFont);
			}

		iProfiler->MarkResultSetL();
		}
	TInt64 duration=iProfiler->GetTrimedMean();
	if (aTestOpenFont)
		{
		iProfiler->ResultsAnalysis(KAddOpenedOpenFontFiles, 0, 0, 0, KIterationsToTest);
		}
	else
		{
		iProfiler->ResultsAnalysis(KAddOpenedBitmapFontFiles, 0, 0, 0, KIterationsToTest);
		}
	CleanupStack::PopAndDestroy(fontStore);

	heap->Close();
	}

// Measure performance for DrawText without glyph cache
// In order to avoid using the glyph cache a different character must be drawn each time
void CTFbsFontHandlePerf::FontDrawStringWithoutGlyphCacheL(const TDesC& aFontName, const TDesC& aTestName, TInt aFontHeight,const TDesC& aOutputText)
	{
	CFbsFont* font;
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	
	TInt loopCount;
	for (loopCount=0;loopCount<KMinimumIterations;loopCount++)
		{
		TInt err=ts->GetNearestFontToDesignHeightInPixels((CFont*&)font,TFontSpec(aFontName, aFontHeight));
		aFontHeight+=2;
		TEST(err == KErrNone);
		iGc->UseFont(font);

		// Construct a descriptor with increasing character codes so that we do not fetch glyphs from the glpyh cache
		if (loopCount==0)
		    iProfiler->InitResults();
		else
		    iProfiler->StartTimer();
		iGc->DrawText(aOutputText,TPoint(10,100));
		iProfiler->MarkResultSetL();
		ts->ReleaseFont(font);
		}

	TInt64 duration=iProfiler->GetTrimedMean();
	iProfiler->ResultsAnalysis(aTestName, 0, 0, 0, KShortIterationsToTest);	
	CleanupStack::PopAndDestroy(ts);
	}



// Measure performance for DrawText using glyph cache
void CTFbsFontHandlePerf::FontDrawTextGlyphCacheL(const TDesC& aFontName, const TDesC& aTestName, const TDesC& aOutputText)
	{
	CFbsFont* font;
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	
	TInt err=ts->GetNearestFontToDesignHeightInPixels((CFont*&)font,TFontSpec(aFontName, 15));	
	TEST(err == KErrNone);
	iGc->UseFont(font);
	
	iGc->DrawText(aOutputText,TPoint(10,100));
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		iGc->DrawText(aOutputText,TPoint(10,100));
		iProfiler->MarkResultSetL();
		}
		
	TInt64 duration=iProfiler->GetTrimedMean();
	iProfiler->ResultsAnalysis(aTestName, 0, 0, 0, KIterationsToTest);	
	ts->ReleaseFont(font);	// Release font 
	CleanupStack::PopAndDestroy(ts);
	}

	
// Create and release fonts.
void CTFbsFontHandlePerf::FindFontSizeL()
	{
	RHeap* heap = UserHeap::ChunkHeap(NULL,0x10000,0x10000);
	if (NULL == heap )
		{
		User::Leave(KErrGeneral);
		}

	CFontStore* fontStore = CFontStore::NewL(heap);
	CleanupStack::PushL(fontStore);

	// Install the dummy rasterizer.
	COpenFontRasterizer* dummyRasterizer = CDummyRasterizer::NewL();
	CleanupStack::PushL(dummyRasterizer);
	fontStore->InstallRasterizerL(dummyRasterizer);
	CleanupStack::Pop();
	
	iProfiler->InitResults();
	for(TInt count=KIterationsToTest; count>=0; --count)
		{		
		//Add font files to fontstore
		TUid id1 = fontStore->AddFileL(KFontDummy);
		TUid id2 = fontStore->AddFileL(KFontDummy_b);
		TUid id3 = fontStore->AddFileL(KFontDummy_i);
		TUid id4 = fontStore->AddFileL(KFontDummy_bi);
		TInt typefaces = fontStore->NumTypefaces();
		for (TInt typeface = typefaces - 1; typeface >= 0 ; --typeface)
			{
			TTypefaceSupport support;
			fontStore->TypefaceSupport(support,typeface);
			TFontSpec fs;
			fs.iTypeface = support.iTypeface;
			for (int height = 0; height < support.iNumHeights; height++)
				{
				fs.iHeight = fontStore->FontHeightInTwips(typeface,height);
				}
			}		
		//Remove font files from fontstore
		fontStore->RemoveFile(id1);
		fontStore->RemoveFile(id2);
		fontStore->RemoveFile(id3);
		fontStore->RemoveFile(id4);
			
		iProfiler->MarkResultSetL();
	}
		
	TInt64 duration=iProfiler->GetTrimedMean();
	iProfiler->ResultsAnalysis(KFindFontSize, 0, 0, 0, KIterationsToTest);


	CleanupStack::PopAndDestroy(fontStore);
	heap->Close();
	}

void CTFbsFontHandlePerf::NonAliasedFontCreationL()
	{
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	FontCreationPerformanceL(ts, KFontSansSerif, KFontCreationSansSerif);
	CleanupStack::PopAndDestroy(ts);
	}

void CTFbsFontHandlePerf::AliasedFontCreationL()
	{
	CFbsTypefaceStore* ts = CFbsTypefaceStore::NewL(NULL);
	CleanupStack::PushL(ts);
	ts->SetFontNameAliasL(KAliasedFont, KFontSansSerif);
	FontCreationPerformanceL(ts, KAliasedFont, KFontCreationAliasedFont);
	CleanupStack::PopAndDestroy(ts);
	}

/** Function which is used to measure font creation performance.  This is a helper function
and not a stand alone test.  The font name is expected to be KFontSansSerif or aliased 
to KFontSansSerif.

@param aTs the CFbsTypefaceStore to be used
@param aFontName the name of the font to be created
@param aTestDescription the description of the test used in logging
*/  
void CTFbsFontHandlePerf::FontCreationPerformanceL(CFbsTypefaceStore* aTs, const TDesC& aFontName, const TDesC& aTestDescription)
	{
	iProfiler->InitResults();
	CFont* font = NULL;
	TFontSpec spec(aFontName, 0);
	for(TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		TInt ret = aTs->GetNearestFontToDesignHeightInPixels(font, spec);
		iProfiler->MarkResultSetL();
		TEST(ret == KErrNone);
		TFontSpec returnedSpecs = font->FontSpecInTwips();
		TEST(returnedSpecs.iTypeface.iName == KFontSansSerif);
		aTs->ReleaseFont(font);
		}
	TInt64 duration=iProfiler->GetTrimedMean();
	iProfiler->ResultsAnalysis(aTestDescription, 0, 0, 0, KIterationsToTest);
	}
