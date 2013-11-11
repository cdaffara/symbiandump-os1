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

#include <graphics/fbsglyphdataiterator.h>
#include <sgresource/sgimage.h>

#include "tfbsglyphdataoutlineshadow.h"

_LIT(KTypefaceName, "DejaVu Sans Condensed");

//Note: This test must be run with a rasterizer which supports EFourColourBlendGlyphBitmap (e.g. iType)
//otherwise the test will fail.
CTFbsGlyphDataOutlineAndShadow::CTFbsGlyphDataOutlineAndShadow(CTestStep* aStep) :
    CTGraphicsBase(aStep)
    {
    }

CTFbsGlyphDataOutlineAndShadow::~CTFbsGlyphDataOutlineAndShadow()
    {
    if (iTs)
       {
       iTs->ReleaseFont(iFont);
       delete iTs;
       }
    }

void CTFbsGlyphDataOutlineAndShadow::ConstructL()
    {
    iTs = (CFbsTypefaceStore*)CFbsTypefaceStore::NewL(NULL);
    TFontSpec fontSpec;
    fontSpec.iTypeface.iName = KTypefaceName;
    fontSpec.iHeight = 15;
    fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
    fontSpec.iFontStyle.SetEffects(FontEffect::EDropShadow, EDropShadowOn);
    fontSpec.iFontStyle.SetEffects(FontEffect::EOutline, EOutlineOn);
    User::LeaveIfError(iTs->GetNearestFontToDesignHeightInPixels((CFont*&)iFont, fontSpec));
    //now check if the font is what we requested
    if(iFont->FontSpecInTwips().iFontStyle.BitmapType()!=EFourColourBlendGlyphBitmap)
        {
        WARN_PRINTF1(_L("Monotype font is not installed, run iType\\binaries\\ityperast.cmd to install it"));
        User::Leave(KErrNotSupported);
        }
    }

/**
@SYMTestCaseID      GRAPHICS-FBSERV-0646
@SYMTestPriority    High
@SYMTestType        UT
@SYMTestStatus      Implemented
@SYMPREQ            PREQ2678

@SYMTestCaseDesc
    To ensure that glyph iterator doesn't support outline-and-shadow font
@SYMTestActions
    i. Call RFbsGlyphDataIterator::Open() with an outline-and-shadow font.

@SYMTestExpectedResults
    RFbsGlyphDataIterator::Open() returns KErrNotSupported.
*/
void CTFbsGlyphDataOutlineAndShadow::RunTestCaseL(TInt /*aCurTestCase*/)
    {
    INFO_PRINTF1(_L("Ensure that glyph iterator doesn't support outline-and-shadow font"));
    ((CTFbsGlyphDataOutlineAndShadowStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0646"));
    RFbsGlyphDataIterator iter;
    const TUint glyphCode = 50;
    TInt err = iter.Open(*iFont, &glyphCode, 1);
    TESTE(err == KErrNotSupported, err);
    iter.Close();

    ((CTFbsGlyphDataOutlineAndShadowStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);//_L("GRAPHICS-FBSERV-0645"));
    ((CTFbsGlyphDataOutlineAndShadowStep*)iStep)->CloseTMSGraphicsStep();
    TestComplete();     
    ((CTFbsGlyphDataOutlineAndShadowStep*)iStep)->RecordTestResultL();
    }

__CONSTRUCT_STEP__(FbsGlyphDataOutlineAndShadow)
