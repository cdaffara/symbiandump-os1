// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TGLYPHIMAGECACHE_H__
#define __TGLYPHIMAGECACHE_H__

#include "tdirectgdi_test_step_base.h"
#include <graphics/directgdiextensioninterfaces.h>

enum DrawTextDirection
	{
	EDrawTextHorizontal,
	EDrawTextVerticalUp,
	EDrawTextVerticalDown,
	
	EDrawTextLast
	};
	 
enum DrawTextAdjustment
	{
	EDrawFromPoint,
	EDrawInBox,
	EDrawRightAlignment,
	};

class CTGlyphImageCache : public CTDirectGdiStepBase
	{
public:
	CTGlyphImageCache();
	~CTGlyphImageCache();
	
private:
	// Main test methods
 	void TestRetrieveEntryL(MFontGlyphImageStorage* aGlyphStorage, TGlyphBitmapType aGlyphType); 	
	void TestDrawGlyphL(MFontGlyphImageStorage* aGlyphStorage, TGlyphBitmapType aGlyphBitmapType, DrawTextDirection aDirection, DrawTextAdjustment aDrawAdjustment, TBool aClipRegion);
	void FillUpCacheL(MFontGlyphImageStorage* aGlyphStorage);
	void FillCacheL(MFontGlyphImageStorage* aGlyphStorage);
	void FontListIdOrderL(MFontGlyphImageStorage* aGlyphStorage);
	void WrongParameterL(MFontGlyphImageStorage* aGlyphStorage);

	//Supplementary methods 
	void DrawText(const TDesC& aText, const TPoint& aPt, DrawTextDirection aDirection, DrawTextAdjustment aDrawAdjustment, TBool aUpdateJustification);
	TInt GlyphImageSizeInByte(TGlyphBitmapType aGlyphType, const TSize& aSize) const;
	TBool DetectMonotypeFontL();

	// From CStepStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();	
	
	// from CTDirectGdiStepBase
	void RunTestsL();
private:
	TBool iMonotypeFont;
	};		
	
_LIT(KTGlyphImageCacheStep, "TDirectGDIGlyphImageCache");

#endif
