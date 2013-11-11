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
// fbsfonthandleperf.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 
#ifndef __TFBSFONTHANDLEPERF_H__
#define __TFBSFONTHANDLEPERF_H__

#include "te_graphicsperformanceSuiteStepBase.h"
#include <test/ttmsgraphicsstep.h>
#include <graphics/openfontrasterizer.h>

//******** CDummyFontFile **********//

class CDummyFontFile: public COpenFontFile
	{
public:
	static CDummyFontFile* NewL(TInt aUid,const TDesC& aFileName);
private:
	CDummyFontFile(TInt aUid,const TDesC& aFileName);
	~CDummyFontFile();
	void ConstructL();
	void GetNearestFontInPixelsL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,
								 const TOpenFontSpec& aDesiredFontSpec,TInt aPixelWidth,TInt aPixelHeight,
								 COpenFont*& aFont,TOpenFontSpec& aActualFontSpec);
	TBool HasUnicodeCharacterL(TInt /*aFaceIndex*/,TInt /*aCode*/) const;
private:
	RFs		iFs;
	RFile	iFile;
	};

//********** CDummyRasterizer **********//

class CDummyRasterizer: public COpenFontRasterizer
	{
public:
	static CDummyRasterizer* NewL();
	// virtual function from COpenFontRasterizer
	COpenFontFile* NewFontFileL(TInt aUid,const TDesC& aFileName,RFs& aFileSession);
	};

//*********** CDummy Font **********************//
class CDummyFont: public COpenFont
	{
public:
	static CDummyFont* NewL(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,CDummyFontFile* aFontFile,TInt aSizeInPixels);
private:
	CDummyFont(RHeap* aHeap,COpenFontSessionCacheList* aSessionCacheList,CDummyFontFile* aFontFile,TInt aSizeInPixels);
	// From COpenFont
	virtual void RasterizeL(TInt aCode,TOpenFontGlyphData* aGlyphData);
	inline void operator delete (TAny *){} //to avoid warning 
	inline void operator delete (TAny *, TAny* ){} //to avoid warning 
	};


class CTFbsFontHandlePerf : public CTe_graphicsperformanceSuiteStepBase
	{
public:
	CTFbsFontHandlePerf();
	
private:
	void FontDuplicateL();
	void FontGetNearestFontToDesignHeightInPixelsL(const TDesC& aFontName, const TDesC& aTestName);	
	void AddAndRemoveFilesL(TBool aTestOpenFont);
	void DoAddAndRemoveFilesL(TBool aTestOpenFont, CFontStore* aFontStore);
	void AddingOpenedFontFilesL(TBool aTestOpenFont);
	void FontDrawTextGlyphCacheL(const TDesC& aFontName, const TDesC& aTestName, const TDesC& aOutputText);
	void FontDrawStringWithoutGlyphCacheL(const TDesC& aFontName, const TDesC& aTestName, TInt aFontHeight,const TDesC& aOutputString);
	void FindFontSizeL();
	void NonAliasedFontCreationL();
	void AliasedFontCreationL();
	void FontCreationPerformanceL(CFbsTypefaceStore* aTs, const TDesC& aFontName, const TDesC& aTestDescription);

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();

private:
	CBitmapDevice* iDevice;	
	};
	
_LIT(KTFbsFontHandlePerfName,"tfbsfonthandleperf");

#endif
