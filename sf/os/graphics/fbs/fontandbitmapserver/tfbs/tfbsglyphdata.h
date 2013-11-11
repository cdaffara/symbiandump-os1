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

#ifndef TFBSGLYPHDATA_H
#define TFBSGLYPHDATA_H

#include <EGL/eglext.h>
#include <VG/openvg.h>
#include <sgresource/sgresource.h>
#include "test/TGraphicsHarness.h"
class RSgImage;
class CEGLHelper;

//#define SAVEGLYPHSTOMBMDURINGCOMPARISON // Enable saving of glyphs in the CompareSgImages()function
//#define SAVEGLYPHSTOMBMDEBUGFUNCTION //  Enable compilation of the member function SaveRSgImagesAsMbms(). Occasionally useful.

typedef EGLBoolean (*TvgCreateEGLImageTargetKHRTypefPtr) (VGeglImageKHR image);

/**
Multi-thread test enum.
 */
enum EGlyphDataMultithreadTest
	{
	EGlyphDataMultiSessionTestShareGlyphs,
	EGlyphDataMultiSessionTestStressAtlas
	};

/**
Struct passed to multi-thread test functions.
 */
struct TGlyphDataMultithreadParams
	{
	TFontSpec iFontSpec;
	TUint* iGlyphCodes;
	TInt iGlyphCodesCount;
	CEGLHelper* iEGL;
	};

/**
Struct used to define the mutlithreaded test to run when launching a new thread.
 */
struct TGlyphDataThreadInfo
	{
	EGlyphDataMultithreadTest iTest;
	TGlyphDataMultithreadParams iParams;
	TInt iThreadNum;
	CTestStep* iStep;
	};


/**
 *
 * Class to provide a dummy font for test purposes
 * See CTFbsGlyphData::TestGlyphDataIteratorNotSupportedFontL()
 */
class CTestFont : public CFont
	{
public:
	TUid DoTypeUid() const {return TUid::Uid(12345);}
	TInt DoHeightInPixels() const {return 12;}
	TInt DoAscentInPixels() const {return 10;}
	TInt DoCharWidthInPixels(TChar /*aChar*/) const  {return 0;}
	TInt DoTextWidthInPixels(const TDesC& /*aText*/) const {return 0;}
	TInt DoBaselineOffsetInPixels() const {return 0;}
	TInt DoTextCount(const TDesC& /*aText*/,TInt /*aWidthInPixels*/) const {return 0;}
	TInt DoTextCount(const TDesC& /*aText*/,TInt /*aWidthInPixels*/,TInt& /*aExcessWidthInPixels*/) const {return 0;}
	TInt DoMaxCharWidthInPixels() const  {return 0;}
	TInt DoMaxNormalCharWidthInPixels() const  {return 0;}
	TFontSpec DoFontSpecInTwips() const  {return TFontSpec();}
	};

/**
Created by each process/thread that needs to do RSgImage image comparison.
The class can be shared in a process between threads and handles synchronisation
with EGL itself.
 */
class CEGLHelper : public CBase
	{
public:
	~CEGLHelper();
	static CEGLHelper* NewL();
		
	TInt GetSgImageData(const RSgImage& aSgImage, const TRect& aRect, TUint8*& aBuf);
	
private:
	CEGLHelper();
	void ConstructL();

private:
	RMutex iMutex;
	RSgDriver iSgDriver;
	EGLDisplay iDisplay;
	EGLContext iContext;
	EGLSurface iSurface;
	// Function pointer for creation and destruction of EGLImages, and the creation
	// of VGImages from EGLImages.
	TvgCreateEGLImageTargetKHRTypefPtr vgCreateImageTargetKHR;
	PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
	PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR;
	};

/**
Test class for the glyph data extensions RFbsGlyphDataIterator and 
RFbsGlyphMetricsArray. Positive and negative tests
*/
class CTFbsGlyphData : public CTGraphicsBase
	{
public:
	CTFbsGlyphData(CTestStep* aStep);
	~CTFbsGlyphData();
protected:
//from 	CTGraphicsBase
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
	
private:
	// Test Cases
	void TestConsistencyWithGetCharacterData();
	void TestInvalidGlyphCode();
	void TestGlyphMetricsArrayParameters();
	void TestGlyphMetricsArrayReuse();
	void TestGlyphDataIteratorClose();
	void TestGlyphDataIteratorSequence();
	void TestGlyphDataIteratorMultipleUsesOnMultipleFonts();
	void TestGlyphDataIteratorImageValidity();
	void TestGlyphDataIteratorOpenInvalidCode();
	void TestGlyphDataIteratorOpenTwice();
	void TestGlyphDataIteratorOpenTwiceWithDifferentFonts();
	void TestGlyphDataIteratorOpenTooBigFont();
	void TestGlyphDataIteratorOpenWithWrongArgument();
	void TestGlyphDataIteratorImageMemoryLeak();
	void TestGlyphDataIteratorLargeFontStress();
	void TestGlyphDataIteratorManyFontsStressL();
	void TestGlyphDataIteratorNoGraphicsMemory();
	void TestGlyphDataIteratorNextIsAtomic();
	void TestGlyphDataIteratorManyArraySizes();
	void TestBitmapFontSupport();
	// White-Box Test Cases
	void TestGlyphDataIteratorSameGlyphCodes();
	void TestMultithreadShareSingleFont();
	void TestMultithreadStressAtlas();
	// Out-of-system-memory tests
	void TestGlyphMetricsArrayHeapOOML();
	void TestGlyphDataIteratorHeapOOML();
	// Multithread funtions
	static void ThreadShareGlyphsL(TInt aThreadNum, TGlyphDataMultithreadParams& aParam, CTestStep* aStep);
	static void ThreadStressAtlasL(TInt aThreadNum, TGlyphDataMultithreadParams& aParam, CTestStep* aStep);	

	// Utility functions for the test cases.
	static TInt ThreadFunction(TAny* aParam); 
	TInt GetImageDataFromSgImage(const RSgImage& aSgImage, const TRect& aRect, TUint8*& aBuf);
	TUint32 CompareMetrics(const TOpenFontCharMetrics& aMetrics1, const TOpenFontCharMetrics& aMetrics2);
	static TInt CompareSgImages(CEGLHelper* aEGL, const RSgImage& aImageA, const TRect& aRectA, const RSgImage& aImageB, const TRect& aRectB, TBool& aMatch);
	static TInt CompareSgImages(CEGLHelper* aEGL, const RSgImage& aImageA, const TRect& aRectA, TUint8* aBufferA, const RSgImage& aImageB, const TRect& aRectB, TUint8* aBufferB, TBool& aMatch);
#ifdef SAVEGLYPHSTOMBMDEBUGFUNCTION
	void SaveRSgImagesAsMbms( CEGLHelper* aEGL, const RSgImage& aImageA, const TRect& aRectA, const RSgImage& aImageB, const TRect& aRectB );
#endif // SAVEGLYPHSTOMBMDEBUGFUNCTION
	void InfoPrintFontSpec(const CFont& aFont);
	void SetupEglL();
	
private:
	RFbsSession* iFbs;
	CFbsTypefaceStore* iTs;
	TUint* iGlyphCodesLatin;
	
	CFbsFont* iFont;
	CFbsFont* iFont2;

	TUint8* iTempBuf1;
	TUint8* iTempBuf2;
	RSgDriver iSgDriver;
	CEGLHelper* iEGL;
	};


class CTFbsGlyphDataStep : public CTGraphicsStep
	{
public:
	CTFbsGlyphDataStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFbsGlyphDataStep,"TFbsGlyphData");

#endif /* TFBSGLYPHDATA_H */
