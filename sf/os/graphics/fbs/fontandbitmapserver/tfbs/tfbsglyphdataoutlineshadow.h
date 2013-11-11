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

#ifndef TFBSGLYPHDATAOUTLINESHADOW_H
#define TFBSGLYPHDATAOUTLINESHADOW_H

#include "test/TGraphicsHarness.h"


/** 
Enumeration for the attributes to be set in fontspec and gc.
See CTFbsGlyphDataOutlineAndShadow::ConstructL()
*/
enum TFontEffectsFlags
    {
    EDropShadowOn = 0x1,
    EOutlineOn = 0x2,
    };

/**
Test class which checks that glyph data extensions 
RFbsGlyphDataIterator processes and returns correct error if outline-and-shadow
font is supplied. 

This test must be run with a rasterizer which supports EFourColourBlendGlyphBitmap (e.g. iType)
otherwise the test will fail.

Negative test
*/
class CTFbsGlyphDataOutlineAndShadow : public CTGraphicsBase
    {
public:
    CTFbsGlyphDataOutlineAndShadow(CTestStep* aStep);
    ~CTFbsGlyphDataOutlineAndShadow();
protected:
//from  CTGraphicsBase
    virtual void RunTestCaseL(TInt aCurTestCase);
    void ConstructL();
    
private:
    CFbsTypefaceStore* iTs;
    CFbsFont* iFont;
   };

class CTFbsGlyphDataOutlineAndShadowStep : public CTGraphicsStep
    {
public:
    CTFbsGlyphDataOutlineAndShadowStep();
protected:  
    //from CTGraphicsStep
    virtual CTGraphicsBase* CreateTestL();
    };

_LIT(KTFbsGlyphDataOutlineAndShadowStep,"TFbsGlyphDataOutlineAndShadow");


#endif /* TFBSGLYPHDATAOUTLINESHADOW_H */
