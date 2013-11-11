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
 @test
 @internalComponent - Internal Symbian test code 
*/

#include <test/graphicsfontutils.h>
#include <graphics/fbsglyphdataiterator.h>
#include <graphics/fbsglyphmetricsarray.h>
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <sgresource/sgimage.h>
#include <egl/egl.h>
#include <vg/openvg.h>
typedef EGLBoolean (*TvgCreateEGLImageTargetKHRTypefPtr) (VGeglImageKHR image);
#endif
#include "tfbsglyphdata.h"

// Size of EGLSurface used for rendering to, in pixels.
const TSize KEglTargetSize(512, 512);

CTFbsGlyphData::CTFbsGlyphData()
	{
	SetTestStepName(KTFbsGlyphData);
	}

CTFbsGlyphData::~CTFbsGlyphData()
	{
	}

TVerdict CTFbsGlyphData::doTestStepPreambleL()
    {
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    User::LeaveIfError(iFbs.Connect());
	User::LeaveIfError(iSgDriver.Open());
#endif
	iCharCodeConverter = CCharCodeConverter::NewL();
    return CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
    }

TVerdict CTFbsGlyphData::doTestStepPostambleL()
    {
    delete iCharCodeConverter;
    iCharCodeConverter = NULL;
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    iSgDriver.Close();
    iFbs.Disconnect();
#endif
    return CTe_graphicsperformanceSuiteStepBase::doTestStepPostambleL();
    }

TVerdict CTFbsGlyphData::doTestStepL()
	{
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    INFO_PRINTF1(_L("CTFbsGlyphData can only be run with SgImage 'Lite'"));
    return TestStepResult();
#else
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0178"));
	GlyphMetricsArrayL(ETestLanguageLatin, ETrue, 1000);
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-UI-BENCH-0179"));
	GlyphMetricsArrayL(ETestLanguageLatin, EFalse, 50000);
	RecordTestResultL();
	
    // Tests 180 and 181 require a CMap table in order to convert CharacterCodes to GlyphCodes.
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0180"));
	GlyphMetricsArrayL(ETestLanguageHindi, ETrue, 25);
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0181"));
	GlyphMetricsArrayL(ETestLanguageHindi, EFalse, 50000);
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0182"));
	GlyphMetricsQuerySingleGlyphL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0183"));
	GlyphDataIteratorOpenL(ETestLanguageLatin, ETrue, 50);
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-UI-BENCH-0184"));
	GlyphDataIteratorOpenL(ETestLanguageLatin, EFalse, 500);
	RecordTestResultL();
	
    // Tests 185 and 186 require a CMap table in order to convert CharacterCodes to GlyphCodes.
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0185"));
	GlyphDataIteratorOpenL(ETestLanguageHindi, ETrue, 10);
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0186"));
	GlyphDataIteratorOpenL(ETestLanguageHindi, EFalse, 5000);
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0187"));
	GlyphDataIteratorOpenSingleFontL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0188"));
	GlyphMetricsQueryUnrasterizedL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0189"));
	GlyphMetricsQueryPreRasterizedL();
	RecordTestResultL();
	
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0190"));
	GlyphRenderingL();
	RecordTestResultL();

	return TestStepResult();
#endif
    }

#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0178...0181

@SYMTestType UT

@SYMPREQ PREQ2678

@SYMTestCaseDesc 
Measures the performance of calling RFbsGlyphMetricsArray::Get() with different sample data.
The sample data can be a single word, or a very long array of glyphs, in latin or non-latin
alphabets. At each repetition a different font is used, cycled over nine fonts, to reduce the
effect of having cached glyphs.

@SYMTestActions
i. Create some sample fonts to cycle through. 
ii. Load sample data from config file, specified by aSampleDataKey.
iii. Create RFbsGlyphMetricsArray, open on sample data.
iv. For each repetition, call RFbsGlyphMetricsArray::Get(), adjusting font at each repetition.
v. Measure time from from first to last repetition.

@param aLanguage The language this test will use.
@param aLongData If ETrue, tells the test to use the long sample data string for the test, EFalse
	will make the test use the short string data.
@param aReps The number of times to repeat the test.
*/
void CTFbsGlyphData::GlyphMetricsArrayL(TTestLanguage aLanguage, TBool aLongData, TInt aReps)
	{
	TBuf<128> KTestName;
	TPtrC KTestVariant = ConfigKeyNameL(aLanguage, aLongData);
	KTestName.Format(_L("GlyphMetricsArray %S"), &KTestVariant);
	
	// Create some test fonts using the font factory.
	CTFontFactory* fontFactory = CTFontFactory::NewLC();
	fontFactory->CreateFontsL(aLanguage, 9);

	// Load the sample string data from the config file.
	TInt numGlyphCodes = 0;
	TUint* glyphCodes;
	LoadConfigSampleDataL(fontFactory->NextFont(), aLanguage, aLongData, glyphCodes, numGlyphCodes);

	// Run the test.
	TInt err = KErrNone;
	RFbsGlyphMetricsArray array;
	CleanupClosePushL(array);
	iProfiler->InitResults();
	for (TInt rep = aReps; (rep != 0) && (err == KErrNone); --rep)
		{
		err = array.Get(*(fontFactory->NextFont()), glyphCodes, numGlyphCodes);
		}
	iProfiler->MarkResultSetL();
	TESTE(err == KErrNone, err);
	iProfiler->ResultsAnalysisGlyphRate(KTestName, 0, 0, 0, aReps, numGlyphCodes);
	
	CleanupStack::PopAndDestroy(2); // array, fontFactory
	delete [] glyphCodes;
	}

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0182

@SYMTestType UT

@SYMPREQ PREQ2678

@SYMTestCaseDesc 
Measures the performance of calling RFbsGlyphDataIterator::Get() with a single glyph,
versus CFont::GetCharacterData(). Using a single glyph code is a very common use case.
The glyph and the font is changed at each iteration.

@SYMTestActions
i. Create some sample fonts to cycle through. 
ii. Create RFbsGlyphMetricsArray.
iii. For each repetition, call RFbsGlyphMetricsArray::Get(), adjusting the glyph at 
	each iteration.
iv. Measure time from from first to last iteration.
v. Repeat steps iii. and iv. with CFont::GetCharacterData().
*/
void CTFbsGlyphData::GlyphMetricsQuerySingleGlyphL()
	{
	_LIT(KTestName, "GlyphMetricsQuerySingleGlyph");
	const TInt KNumIterations = 50000;
	TInt err = KErrNone;
	TBuf<128> KTestNameVariant;

	// Create some test fonts using the font factory.
	CTFontFactory* fontFactory = CTFontFactory::NewLC();
	
	// Run the test for RFbsGlyphMetricsArray, with a different character 
	// and font for each iteration.
	KTestNameVariant.Format(_L("%S RFbsGlyphMetricsArray"), &KTestName);
	fontFactory->CreateFontsL(ETestLanguageLatin, 9);
	RFbsGlyphMetricsArray array;
	CleanupClosePushL(array);
	iProfiler->InitResults();
	for (TInt rep = KNumIterations; rep != 0 && (err == KErrNone); --rep)
		{
		const TUint KGlyphCode = 32 + (rep % 96);
		err = array.Get(*(fontFactory->NextFont()), &KGlyphCode, 1);
		}
	iProfiler->MarkResultSetL();
	TESTE(err == KErrNone, err);
	CleanupStack::PopAndDestroy(1); // array
	iProfiler->ResultsAnalysisGlyphRate(KTestNameVariant, 0, 0, 0, KNumIterations, 1);
	fontFactory->ReleaseFonts();
	
	// Run the test for GetCharacterData(), with a different character
	// and font for each iteration.
	KTestNameVariant.Format(_L("%S GetCharacterData"), &KTestName);
	fontFactory->CreateFontsL(ETestLanguageLatin, 9);
	TOpenFontCharMetrics metrics;
	const TUint8* bitmapData = NULL;
	TSize bitmapSize;
	iProfiler->InitResults();
	for (TInt rep = KNumIterations; rep != 0; --rep)
		{
		const TUint KGlyphCode = 32 + (rep % 96);
		fontFactory->NextFont()->GetCharacterData(KGlyphCode, metrics, bitmapData, bitmapSize);
		}
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisGlyphRate(KTestNameVariant, 0, 0, 0, KNumIterations, 1);
	
	CleanupStack::PopAndDestroy(1); // fontFactory
	}

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0183...0186

@SYMTestType UT

@SYMPREQ PREQ2678

@SYMTestCaseDesc 
Measures the performance of calling RFbsGlyphDataIterator::Open() with different 
sample data, and iterating through the data with RFbsGlyphDataIterator::Next(). 
The sample data can be a single word, or a very long array of glyphs,
in various languages. At each repetition a different font is used, cycled
over nine fonts.

@SYMTestActions
i. Create some sample fonts to cycle through. 
ii. Create RFbsGlyphDataIterator.
iii. For each repetition, call RFbsGlyphDataIterator::Open(), adjusting the glyph at each 
	iteration. The font is changed at each repetition.
iv. Measure time from from first to last repetition.

@param aLanguage The language this test will use.
@param aLongData If ETrue, tells the test to use the long sample data string for the test, EFalse
	will make the test use the short string data.
@param aReps The number of times to repeat the test.
 */
void CTFbsGlyphData::GlyphDataIteratorOpenL(TTestLanguage aLanguage, TBool aLongData, TInt aReps)
	{
	TBuf<128> KTestName;
	TPtrC KTestVariant = ConfigKeyNameL(aLanguage, aLongData);
	KTestName.Format(_L("GlyphDataIteratorOpen %S"), &KTestVariant);
	
	// Create some test fonts using the font factory.
	CTFontFactory* fontFactory = CTFontFactory::NewLC();
	fontFactory->CreateFontsL(aLanguage, 9);
	
	// Load the sample string data from the config file.
	TInt numGlyphCodes = 0;
	TUint* glyphCodes;
	LoadConfigSampleDataL(fontFactory->NextFont(), aLanguage, aLongData, glyphCodes, numGlyphCodes);

	// Run the test.
	TInt err = KErrNotFound;
	RFbsGlyphDataIterator iter;
	CleanupClosePushL(iter);
	iProfiler->InitResults();
	for (TInt rep = aReps; (rep != 0) && (err == KErrNotFound); --rep)
		{
		err = iter.Open(*(fontFactory->NextFont()), glyphCodes, numGlyphCodes);
		for (; err == KErrNone; err = iter.Next())
			{
			// no operation
			}
		iter.Close();
		}
	iProfiler->MarkResultSetL();
	TESTE(err == KErrNotFound, err);

	CleanupStack::PopAndDestroy(2); // iter, fontFactory
	iProfiler->ResultsAnalysisGlyphRate(KTestName, 0, 0, 0, aReps, numGlyphCodes);
	delete [] glyphCodes;
	}

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0187

@SYMTestType UT

@SYMPREQ PREQ2678

@SYMTestCaseDesc 
Measures the performance of calling RFbsGlyphDataIterator::Open() with different 
lengthed arrays but the same font. The sample data is a long array of characters.

@SYMTestActions
i. Create a single test font 
ii. Create RFbsGlyphDataIterator.
iii. Pass an array to RFbsGlyphDataIterator::Open(), starting with a single glyph.
	For each iteration, increase the length of the array by one until the entire
	string has been opened.
iv. Measure the time to perform all the iterations.

@param aSampleDataKey The string key to lookup under the GlyphArraySampleText section of the 
	config file where the sample data is read.
@param aReps The number of times to repeat the test.
 */
void CTFbsGlyphData::GlyphDataIteratorOpenSingleFontL()
	{
	_LIT(KTestName, "GlyphDataIteratorOpenSingleFont");
	// A cap on the max number of iterations to complete.
	const TInt KMaxNumIterations = 200;
    const TTestLanguage KTestLanguage = ETestLanguageHindi;
	
	// Create some test fonts using the font factory.
	CTFontFactory* fontFactory = CTFontFactory::NewLC();
	fontFactory->CreateFontsL(KTestLanguage, 1);
	CFbsFont* font = fontFactory->NextFont();
	
	// Load the sample string data from the config file.
	TInt numGlyphCodes = 0;
	TUint* glyphCodes;
	LoadConfigSampleDataL(font, KTestLanguage, ETrue, glyphCodes, numGlyphCodes); 
	
	const TInt KNumRepetitions = Min<TInt>(numGlyphCodes - 1, KMaxNumIterations);
	RFbsGlyphDataIterator iter;
	CleanupClosePushL(iter);
	TInt iterErr = KErrNone;
	TInt glyphCount = 0;
	iProfiler->InitResults();
	for (glyphCount = 1; (glyphCount < KNumRepetitions); ++glyphCount)
		{
		iterErr = iter.Open(*font, glyphCodes, glyphCount);
		for (; iterErr == KErrNone; iterErr = iter.Next())
			{
			// no operation
			}
		iter.Close();
		}
	iProfiler->MarkResultSetL();
	TEST(glyphCount == KNumRepetitions);
	TESTE(iterErr == KErrNotFound, iterErr);
	
	const TInt KAvgNumCharsPerIteration = KNumRepetitions/2;
	iProfiler->ResultsAnalysisGlyphRate(KTestName, 0, 0, 0, KNumRepetitions, KAvgNumCharsPerIteration);
	
	CleanupStack::PopAndDestroy(2); // iter, fontFactory
	delete [] glyphCodes;
	}
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0188

@SYMTestType UT

@SYMPREQ PREQ2678

@SYMTestCaseDesc 
Measures the performance of querying the TOpenFontCharMetrics using the different
available APIs. RFbsGlyphMetricsArray, RFbsGlyphDataIterator, and CFont::GetCharacterData()
are compared against each other, using the same fonts, and same sample data.
This test uses glyphs that have not been rasterized before, therefore for certain
APIs this will mean rasterizing the glyphs.

@SYMTestActions
i. Load sample text data from config file.
ii. For each of RFbsGlyphMetricsArray, RFbsGlyphDataIterator, and CFont::GetCharacterData():
	1. Create 50 new fonts.
	2. Run the test, calling the necessary API once per font/loop.
	3. For GetCharacterData() and RFbsGlyphDataIterator(), cycle through each glyph
		to ensure all metrics have been retrieved. This is not necessary for 
		RFbsGlyphMetricsArray.
	4. Measure time between first and last font/loop.
	5. Destroy test fonts so that next test has to re-rasterize the glyphs.	

@SYMTestExpectedResults
Since this test uses non-rasterized fonts, RFbsGlyphMetricsArray should be faster than
GetCharacterData() and RFbsGlyphDataIterator, which both rasterize the glyphs in order to 
get their metrics information.
 */
void CTFbsGlyphData::GlyphMetricsQueryUnrasterizedL()
	{
	_LIT(KTestName, "GlyphMetricsQueryUnrasterized");
	
	TBuf<128> KTestNameVariant;
	// First do the test for the iterator. To ensure fair comparison with
    // RFbsGlyphMetricsArray, cycle through each iteration to ensure the metrics
    // for each glyph is found.
    KTestNameVariant.Format(_L("%S RFbsGlyphDataIterator"), &KTestName);
    
	const TInt KNumFonts = 50;
	const TTestLanguage KTestLanguage = ETestLanguageLatin;
	
	// Create some test fonts using the font factory.
	CTFontFactory* fontFactory = CTFontFactory::NewLC();
    fontFactory->CreateFontsL(KTestLanguage, KNumFonts);
	    
	// Load the sample string data from the config file. Both the iterator and the
	// array will use this same sample data.
	TInt numGlyphCodes = 0;
	TUint* glyphCodes;
	LoadConfigSampleDataL(fontFactory->NextFont(), KTestLanguage, ETrue, glyphCodes, numGlyphCodes); 
	
	RFbsGlyphDataIterator iter;
	CleanupClosePushL(iter);
	TInt iterErr = KErrNone;
	TInt rep = 0;
	iProfiler->InitResults();
	for (rep = KNumFonts; (rep != 0); --rep)
		{
		iterErr = iter.Open(*(fontFactory->NextFont()), glyphCodes, numGlyphCodes);
		for (; iterErr == KErrNone; iterErr = iter.Next())
			{
			// no operation
			}
		iter.Close();
		}
	iProfiler->MarkResultSetL();
	TEST(rep == 0);
	TESTE(iterErr == KErrNotFound, iterErr);
	CleanupStack::PopAndDestroy(1); // iter		
	iProfiler->ResultsAnalysisGlyphRate(KTestNameVariant, 0, 0, 0, KNumFonts, numGlyphCodes);
	
	// Second, do the test for the array. This should be faster.
	// Destroy the fonts and re-create them so that they have to be re-rasterized
	// for a fair comparison.
	TInt arrayErr = KErrNone;
	KTestNameVariant.Format(_L("%S RFbsGlyphMetricsArray"), &KTestName);
	fontFactory->ReleaseFonts();
	fontFactory->CreateFontsL(KTestLanguage, KNumFonts);
	RFbsGlyphMetricsArray array;
	CleanupClosePushL(array);
	iProfiler->InitResults();	
	for (TInt rep = KNumFonts; (rep != 0) && (arrayErr == KErrNone); --rep)
		{
		arrayErr = array.Get(*(fontFactory->NextFont()), glyphCodes, numGlyphCodes);
		}
	iProfiler->MarkResultSetL();
	CleanupStack::PopAndDestroy(1); // array
	TEST(rep == 0);
	TESTE(arrayErr == KErrNone, arrayErr);
	iProfiler->ResultsAnalysisGlyphRate(KTestNameVariant, 0, 0, 0, KNumFonts, numGlyphCodes);
	
	// Third, do the test using GetCharacterData() to get the metrics.
	// Destroy the fonts and re-create them so that they have to be re-rasterized
	// for a fair comparison.
	KTestNameVariant.Format(_L("%S GetCharacterData"), &KTestName);
	fontFactory->ReleaseFonts();
	fontFactory->CreateFontsL(KTestLanguage, KNumFonts);	
	iProfiler->InitResults();
	const TUint8* bitmapData = NULL;
	TSize bitmapSize;
	TOpenFontCharMetrics metrics;	
	for (TInt rep = KNumFonts; (rep != 0); --rep)
		{
		CFbsFont* font = fontFactory->NextFont(); 
		for (TInt glyphIndex = 0; glyphIndex < numGlyphCodes; glyphIndex++)
			{
			font->GetCharacterData(glyphCodes[glyphIndex], metrics, bitmapData, bitmapSize);
			}
		}
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisGlyphRate(KTestNameVariant, 0, 0, 0, KNumFonts, numGlyphCodes);
	
	CleanupStack::PopAndDestroy(1); // fontFactory
	delete [] glyphCodes;
	}

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0189

@SYMTestType UT

@SYMPREQ PREQ2678

@SYMTestCaseDesc 
Measures the performance of querying the TOpenFontCharMetrics using the different
available APIs. RFbsGlyphMetricsArray, RFbsGlyphDataIterator, and CFont::GetCharacterData()
are compared against each other, using the same fonts, and same sample data.
This test uses glyphs that have already been rasterized, thereby possibly reducing the 
extra overhead this has.

@SYMTestActions
i. Load sample text data from config file.
ii. Create test font.
iii. Pre-rasterize glyphs using RFbsGlyphDataIterator. This will rasterize the glyphs 
	and cause them to be cached for use by all the APIs tested here.
iv. For each of RFbsGlyphMetricsArray, RFbsGlyphDataIterator, and CFont::GetCharacterData():
	1. Begin the loop, calling the necessary API once per font/loop.
	2. For each glyph, request the glyph metrics.
	3. Measure time between first and last font/loop.
v. Destroy test font.

@SYMTestExpectedResults
All results should be improved over GlyphMetricsQueryUnrasterized (GRAPHICS-UI-BENCH-0187).
since no rasterizing should take place during these tests.
 */
void CTFbsGlyphData::GlyphMetricsQueryPreRasterizedL()
	{
	_LIT(KTestName, "GlyphMetricsQueryPreRasterized");
	TBuf<128> KTestNameVariant;
	const TInt KNumIterations = 500;
	const TTestLanguage KTestLanguage = ETestLanguageLatin;

	// Create a test font using the font factory.
    CTFontFactory* fontFactory = CTFontFactory::NewLC();
    fontFactory->CreateFontsL(ETestLanguageLatin, 1);   
    CFbsFont* font = fontFactory->NextFont();
	    
    TInt numGlyphCodes = 0;
	TUint* glyphCodes;
	LoadConfigSampleDataL(font, KTestLanguage, ETrue, glyphCodes, numGlyphCodes); 
	
	TInt iterErr = KErrNone;
	TInt rep = 0;
	// Rasterize the glyphs first.
	RFbsGlyphDataIterator iter;
	CleanupClosePushL(iter);
	for (rep = KNumIterations; (rep != 0) ; --rep)
		{
		iterErr = iter.Open(*font, glyphCodes, numGlyphCodes);
		for (; iterErr == KErrNone; iterErr = iter.Next())
			{
			// no operation
			}
		iter.Close();
		}
	TEST(rep == 0);
	TESTE(iterErr == KErrNotFound, iterErr);
	
	TOpenFontCharMetrics metrics;
		
	// First do the test for the iterator. To ensure fair comparison with
	// RFbsGlyphMetricsArray, cycle through each iteration to ensure the metrics
	// for each glyph is found.
	iterErr = KErrNone;
	KTestNameVariant.Format(_L("%S RFbsGlyphDataIterator"), &KTestName);
	iProfiler->InitResults();
	for (TInt rep = KNumIterations; (rep != 0); --rep)
		{
		for (iterErr = iter.Open(*font, glyphCodes, numGlyphCodes); iterErr == KErrNone; iterErr = iter.Next())
			{
			metrics = iter.Metrics();
			}
		iter.Close();
		}
	iProfiler->MarkResultSetL();
	TESTE(iterErr == KErrNotFound, iterErr);
	CleanupStack::PopAndDestroy(1); // iter
	iProfiler->ResultsAnalysisGlyphRate(KTestNameVariant, 0, 0, 0, KNumIterations, numGlyphCodes);
	
	
	// Second, do the test for the array. This should be faster.
	TInt arrayErr = KErrNone;
	KTestNameVariant.Format(_L("%S RFbsGlyphMetricsArray"), &KTestName);
	RFbsGlyphMetricsArray array;
	CleanupClosePushL(array);
	iProfiler->InitResults();
	for (TInt rep = KNumIterations; (rep != 0) && (arrayErr == KErrNone); --rep)
		{
		arrayErr = array.Get(*font, glyphCodes, numGlyphCodes);
		for (TInt i = 0; i < numGlyphCodes; ++i)
			{
			metrics = array[i];
			}
		}
	iProfiler->MarkResultSetL();
	TESTE(arrayErr == KErrNone, arrayErr);
	CleanupStack::PopAndDestroy(1); // array
	iProfiler->ResultsAnalysisGlyphRate(KTestNameVariant, 0, 0, 0, KNumIterations, numGlyphCodes);
	
	
	// Third, do the test using GetCharacterData() to get the metrics.
	KTestNameVariant.Format(_L("%S GetCharacterData"), &KTestName);
	const TUint8* bitmapData;
	TSize bitmapSize;

	iProfiler->InitResults();
	for (TInt rep = KNumIterations; (rep != 0); --rep)
		{
		for (TInt glyphIndex = 0; glyphIndex < numGlyphCodes; glyphIndex++)
			{
			font->GetCharacterData(glyphCodes[glyphIndex], metrics, bitmapData, bitmapSize);
			}
		}
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisGlyphRate(KTestNameVariant, 0, 0, 0, KNumIterations, numGlyphCodes);
	
	CleanupStack::PopAndDestroy(1); // fontFactory
	delete [] glyphCodes;
	}

/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0190

@SYMTestType UT

@SYMPREQ PREQ2678

@SYMTestCaseDesc 
Measures the end-to-end performance of using Khronos APIs to render glyphs using the
RFbsGlyphDataIterator API. Positioning is very basic and is not reflective of a production-
quality text-rendering algorithm, but serves as a useful benchmark of the overall
text-rendering performance using this API.

@SYMTestActions
i. Create a sample font to use. 
ii. Create a RSgImage to be used as a target for Khronos API rendering.
iii. Set-up EGL and OpenVG.
iv. Construct RFbsGlyphDataIterator, and open on sample data. At each iteration:
	1. Create an EGLImage from the RSgImage.
	2. Create a VGImage from the EGLImage.
	3. Render the VGImage using vgDrawImage().
	4. Destroy VGImage.
	5. Destroy EGLImage.
	6. Advance the current rendering position for the next glyph, ensuring that every glyph
	will be within the bounds of the target surface.
v. Measure time from from first to last iteration.
*/
void CTFbsGlyphData::GlyphRenderingL()
	{
	_LIT(KTestName, "GlyphRendering");
	const TInt KNumIterations = 500;
    const TTestLanguage KTestLanguage = ETestLanguageHindi;
	
	// Create some test fonts using the font factory.
	CTFontFactory* fontFactory = CTFontFactory::NewLC();
	fontFactory->CreateFontsL(KTestLanguage, 1, 20);
	CFbsFont* font = fontFactory->NextFont();
	const TInt KFontHeightInPixels = font->HeightInPixels();

	// Create RSgImage to be used as OpenVG Pixmap Surface

	RSgImage target;
	TInt err = target.Create(TSgImageInfo(KEglTargetSize, ESgPixelFormatARGB_8888_PRE, ESgUsageBitOpenVgSurface));
	TESTL(err == KErrNone);
	CleanupClosePushL(target);
	
	// Initialize EGL/OpenVG for rendering.
	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY)
		{
		ERR_PRINTF2(_L("Failed to get EGLDisplay. [eglError=%X]"), eglGetError());
		User::Leave(KErrGeneral);
		}
	TESTL(display != EGL_NO_DISPLAY);
	if (EGL_FALSE == eglInitialize(display, NULL, NULL))
		{
		ERR_PRINTF2(_L("Failed to initialize EGLDisplay. [eglError=%X]"), eglGetError());
		User::Leave(KErrGeneral);
		}
	eglBindAPI(EGL_OPENVG_API);
	
	EGLint imageAttribs[] =
	    {
	    EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, 
	    EGL_NONE
	    };
	EGLint configAttribs[] = 
		{
		EGL_MATCH_NATIVE_PIXMAP, (EGLint)&target,
		EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE, EGL_PIXMAP_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT, 
		EGL_NONE
		};
	
	const EGLint KPixmapAttribsVgAlphaFormatPre[] = 
	    {
	    EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_PRE,
	    EGL_NONE
	    };
		
	EGLint configId = 0;
	EGLint numConfigs = 0;
	if (EGL_FALSE == eglChooseConfig(display, configAttribs, &configId, 1, &numConfigs) || numConfigs == 0)
		{
		ERR_PRINTF3(_L("Failed to find suitable EGLConfig. [eglError=%X, configs=%d]"), eglGetError(), numConfigs);
		User::Leave(KErrGeneral);
		}
	EGLContext context = eglCreateContext(display, configId, EGL_NO_CONTEXT, NULL);
	if (context == EGL_NO_CONTEXT)
		{
		ERR_PRINTF2(_L("Failed to create EGLContext. [eglError=%X]"), eglGetError());
		User::Leave(KErrGeneral);
		}
	EGLSurface surface = eglCreatePixmapSurface(display, configId, &target, KPixmapAttribsVgAlphaFormatPre);
	if (EGL_FALSE == eglMakeCurrent(display, surface, surface, context))
		{
		ERR_PRINTF2(_L("Failed to create make surface and context current. [eglError=%X]"), eglGetError());
		eglDestroyContext(display, context);
		User::Leave(KErrGeneral);
		}
	
	// Load the necessary EGL extensions...
	TvgCreateEGLImageTargetKHRTypefPtr vgCreateImageTargetKHR;
	PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
	PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
	eglCreateImageKHR = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(eglGetProcAddress("eglCreateImageKHR"));
	eglDestroyImageKHR = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(eglGetProcAddress("eglDestroyImageKHR"));
	vgCreateImageTargetKHR = reinterpret_cast<TvgCreateEGLImageTargetKHRTypefPtr>(eglGetProcAddress("vgCreateEGLImageTargetKHR"));
	if (!eglCreateImageKHR || !eglDestroyImageKHR || !vgCreateImageTargetKHR)
		{
		ERR_PRINTF1(_L("Failed to get EGL Image extension functions."));
		User::Leave(KErrNotSupported);
		}
	// Now we have an OpenVG window to render to!
	
	TInt numGlyphCodes = 0;
	TUint* glyphCodes;
	LoadConfigSampleDataL(font, KTestLanguage, EFalse, glyphCodes, numGlyphCodes); 

	// Set up an identity matrix compatible with the Symbian co-ordinate system.
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
	vgScale(1.f, -1.f);
	vgTranslate(0, -KFontHeightInPixels);	
	VGfloat vgIdentityMatrix[16];
	vgGetMatrix(vgIdentityMatrix);
	
	RFbsGlyphDataIterator iter;
	CleanupClosePushL(iter);
		
	// Render some glyphs.
	TInt iterErr = KErrNone;
	TInt rep = 0;
	vgClear(0, 0, KEglTargetSize.iWidth, KEglTargetSize.iHeight);
	TPoint glyphOrigin(0, 0);
	iProfiler->InitResults();
	for (rep = 0; rep < KNumIterations; rep++)
		{
		iterErr = iter.Open(*font, glyphCodes, numGlyphCodes);
		for (;iterErr == KErrNone; iterErr = iter.Next())
			{
			const TOpenFontCharMetrics& metrics = iter.Metrics();
			const RSgImage& glyphSgImage = iter.Image();
			EGLImageKHR eglImage = eglCreateImageKHR(display, EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR, 
					reinterpret_cast<EGLClientBuffer>(&glyphSgImage), imageAttribs);
			VGImage vgImage = vgCreateImageTargetKHR(eglImage);
			
			// wrapped text placement.			
			TInt horizAdvance = metrics.HorizAdvance();
			if (glyphOrigin.iX + horizAdvance >= KEglTargetSize.iWidth)
				{
				vgLoadMatrix(vgIdentityMatrix);
				glyphOrigin.iX = 0;
				glyphOrigin.iY -= KFontHeightInPixels;
				if (glyphOrigin.iY - KFontHeightInPixels < -KEglTargetSize.iHeight)
					{
					glyphOrigin.iY = 0;
					}
				vgTranslate(glyphOrigin.iX, glyphOrigin.iY);
				}
			
			vgDrawImage(vgImage);
			vgDestroyImage(vgImage);
			eglDestroyImageKHR(display, eglImage);
				
			// Move to next glyph position.
			glyphOrigin.iX += horizAdvance;
			vgTranslate(horizAdvance, 0);
			}
		iter.Close();
		eglSwapBuffers(display, surface);
		}
	iProfiler->MarkResultSetL();
	iProfiler->ResultsAnalysisGlyphRate(KTestName, 0, 0, 0, KNumIterations, numGlyphCodes);
	TEST(rep == KNumIterations);
	TESTE(iterErr == KErrNotFound, iterErr);	
	WriteTargetOutput(KTestName());
	
	eglDestroySurface(display, surface);
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(display);
	eglReleaseThread();
	
	CleanupStack::PopAndDestroy(3); // iter, target, fontFactory
	delete [] glyphCodes;
	}
#endif

/**
Captures the EGL Surface (it is assumed to be an OpenVG surface) to a 256-grey CFbsBitmap,
used when sanity-checking bitmaps are enabled. 
*/
CFbsBitmap* CTFbsGlyphData::GetTargetAsBitmapL()
	{
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	// For debugging purposes only.
	// Capture the final state of the EGL Pixmap surface as an mbm.
	const TInt KDataStride = KEglTargetSize.iWidth;

	TUint8* imageScanline = reinterpret_cast<TUint8*>(User::AllocZL(KDataStride));
	CleanupStack::PushL(imageScanline);
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(KEglTargetSize, EGray256));
	bitmap->BeginDataAccess();
	TUint8* dataAddress = reinterpret_cast<TUint8*>(bitmap->DataAddress());
	const TInt dataStride = bitmap->DataStride();	
	for (TInt scanline = 0; scanline < KEglTargetSize.iHeight; scanline++)
		{
	    vgReadPixels(imageScanline, KDataStride, VG_A_8, 0, scanline, KEglTargetSize.iWidth, 1);
		Mem::Copy(dataAddress, imageScanline, KEglTargetSize.iWidth);
		dataAddress += dataStride;
		}
	bitmap->EndDataAccess(EFalse);
	CleanupStack::Pop(2); // bitmap, imageScanline
	return bitmap;
#else
	return NULL;
#endif
	}

/**
Utility method. Loads sample glyph code data from the config ini file
into a TUint array.
@param aFont The font that the glyph codes will be associated with.
@param aLanguage The language variant to load from the config file as sample data.
@param aLongData Whether to use the long variant (ETrue) or short variant (EFalse) 
    of the sample data for the given language.
@param aGlyphCodes On success, holds an array of glyph codes, to be freed by the caller.
@param aNumGlyphCodes On success, holds the count of the glyph code array.
@leave KErrNotFound if the test data cannot be found or is empty in the config file.
 */
void CTFbsGlyphData::LoadConfigSampleDataL(CFbsFont* aFont, TTestLanguage aLanguage, TBool aLongData, TUint*& aGlyphCodes, TInt& aNumGlyphCodes)
	{
	// The name of the section in the config file to look-up the sample data
	_LIT(KConfigFileSampleData, "GlyphDataSampleText");

	TBuf<32> keyName = ConfigKeyNameL(aLanguage, aLongData);
	
	// Setup the converter to use the passed font.
	iCharCodeConverter->UseFontL(aFont);

	// Load the sample string data from the config file.
	TPtrC sampleText;
	TESTL(GetStringFromConfig(KConfigFileSampleData, keyName, sampleText));
	aNumGlyphCodes = sampleText.Length();
	if (aNumGlyphCodes <= 0)
		{
		User::Leave(KErrNotFound);
		}
	aGlyphCodes = new(ELeave) TUint[aNumGlyphCodes];
	for (TInt code = 0; code < aNumGlyphCodes; ++code)
		{
		aGlyphCodes[code] = iCharCodeConverter->GlyphCodeL(sampleText[code]); 
		}
	}

/**
Creates the name of the key to look for in the config file for the test
with the specified parameters.
@param aLanguage The language the test will use.
@param aLongData Whether to use long or short sample data.
@return A descriptor value of the language.
@leave KErrNotSupported if aLanguage is not recognised.
 */
TBufC<32> CTFbsGlyphData::ConfigKeyNameL(TTestLanguage aLanguage, TBool aLongData)
	{
	if (aLanguage < 0 || aLanguage > 1)
		{
		User::Leave(KErrNotSupported);
		}
	TBuf<32> langName[2];
	langName[ETestLanguageLatin].Append(_L("Latin"));
	langName[ETestLanguageHindi].Append(_L("Hindi"));	
	langName[aLanguage].Append((aLongData) ? _L("Long") : _L("Short"));
	return langName[aLanguage];
	}

/**
Font factory.
Utiltiy class for providing fonts for the performance tests.
*/

CTFontFactory::CTFontFactory()
	{
	}

CTFontFactory::~CTFontFactory()
	{
	ReleaseFonts();
	delete iTs;
	}

/**
@return A new Font Factory ready to create fonts.
*/
CTFontFactory* CTFontFactory::NewLC()
	{
	CTFontFactory* fontFactory = new (ELeave) CTFontFactory();
	CleanupStack::PushL(fontFactory);
	fontFactory->iTs = static_cast<CFbsTypefaceStore*>(CFbsTypefaceStore::NewL(NULL));
	return fontFactory;
	}

/**
Creates a number of fonts for use by tests. All the fonts are created up-front so 
that NextFont() can be called as a very lightweight call, so it can be used inside
tests with minimal impact.
Once fonts are created, the factory must not be destroyed until the fonts it created
are finished with.
@param aLanaugeMask Which language needs to be supported by the returned fonts.
@param aNumFonts The number of fonts to create
@param aStartSizeInPixels The lower bound font height of the fonts that are created. All
	fonts will be at least as big as this value.
*/
void CTFontFactory::CreateFontsL(TTestLanguage aLanguageMask, TInt aNumFonts, TInt aStartSizeInPixels)
	{
	ReleaseFonts();
	
	RArray <TPtrC> typefaceNames;
	CleanupClosePushL(typefaceNames);
	switch(aLanguageMask)
		{
		case ETestLanguageHindi:	
			User::LeaveIfError(typefaceNames.Reserve(1));
			typefaceNames.Append(_L("Devanagari OT Eval"));
			break;
		case ETestLanguageLatin:
			User::LeaveIfError(typefaceNames.Reserve(3));
			typefaceNames.Append(_L("DejaVu Sans Condensed"));
			typefaceNames.Append(_L("DejaVu Serif"));
			typefaceNames.Append(_L("DejaVu Sans Mono"));
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	const TInt KNumTypefaces = typefaceNames.Count();
		
	iFont = new CFbsFont*[aNumFonts];
	for (TInt count = 0; count < aNumFonts; ++count)
		{
		// After every KNumTypefaces font, increase size by 5.
		TInt size = aStartSizeInPixels + (5 *(count / KNumTypefaces));
		TPtrC typefaceName = typefaceNames[count % KNumTypefaces];
		TFontSpec fontSpec(typefaceName, size);
		User::LeaveIfError(iTs->GetNearestFontToDesignHeightInPixels((CFont*&)iFont[count], fontSpec));
		++iNumFonts;
		}
	iCurFont = -1;
	CleanupStack::PopAndDestroy(1); // typefaceNames
	}
/**
Releases all created fonts and frees associated memory.
*/
void CTFontFactory::ReleaseFonts()
	{
	for (TInt font = 0; font < iNumFonts; ++font)
		{
		iTs->ReleaseFont(iFont[font]);
		}
	delete [] iFont;
	iFont = NULL;
	iNumFonts = 0;
	}

/**
@return The next font to be used. If it reaches the last font, the next font will
	cycle back around to the first font.
*/
CFbsFont* CTFontFactory::NextFont()
	{
	return iFont[++iCurFont%iNumFonts];
	}
