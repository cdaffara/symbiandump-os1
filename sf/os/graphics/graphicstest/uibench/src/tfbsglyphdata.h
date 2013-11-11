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

#ifndef TFBSGLYPHDATA_H
#define TFBSGLYPHDATA_H

#include <fbs.h>
#include <sgresource/sgresource.h>
#include "te_graphicsperformanceSuiteStepBase.h"
class CCharCodeConverter;

/**
Enumeration of languages that are supported for testing in this framework.
 */
enum TTestLanguage
	{
	ETestLanguageLatin,
	ETestLanguageHindi
	};

/**
Test case to test the performance of RFbsGlyphDataIterator and RFbsGlyphMetricsArray.
*/
class CTFbsGlyphData : public CTe_graphicsperformanceSuiteStepBase
    {
public:
	CTFbsGlyphData();
	~CTFbsGlyphData();
	
	// From CTe_graphicsperformanceSuiteStepBase
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	virtual CFbsBitmap* GetTargetAsBitmapL();
    
private:
	// Tests
	void GlyphMetricsArrayL(TTestLanguage aLanguage, TBool aLongData, TInt aReps);
	void GlyphMetricsQuerySingleGlyphL();
	void GlyphDataIteratorOpenL(TTestLanguage aLanguage, TBool aLongData, TInt aReps);
	void GlyphDataIteratorOpenSingleFontL();
	void GlyphMetricsQueryUnrasterizedL();
	void GlyphMetricsQueryPreRasterizedL();
	void GlyphRenderingL();
	
	// Utility methods
	void LoadConfigSampleDataL(CFbsFont* aFont, TTestLanguage aLanguage, TBool aLongData, TUint*& aGlyphCodes, TInt& aNumGlyphCodes);
	static TBufC<32> ConfigKeyNameL(TTestLanguage aLanguage, TBool aLongData);
	
private:
	RFbsSession iFbs;
	RSgDriver iSgDriver;
	CCharCodeConverter* iCharCodeConverter;
    };

_LIT(KTFbsGlyphData,"tfbsglyphdata");

/**
Utility class. Provides the tests with a number of different fonts to use.
Manages their creation and destruction.
 */
class CTFontFactory : public CBase
	{
public:
	static CTFontFactory* NewLC();
	~CTFontFactory();
	
	void CreateFontsL(TTestLanguage aLanguageMask, TInt aNumFonts, TInt aStartSizeInPixels = 5);
	TInt NumFonts() const;
	CFbsFont* NextFont();
	void ReleaseFonts();
private:
	CTFontFactory();
private:
	CFbsFont** iFont;
	TInt iNumFonts;
	TInt iCurFont;
	CFbsTypefaceStore* iTs;
	};


#endif /* TFBSGLYPHDATA_H */
