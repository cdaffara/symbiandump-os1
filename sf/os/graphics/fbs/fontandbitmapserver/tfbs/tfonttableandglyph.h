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

#ifndef TFONTTABLEANDGLYPH_H_
#define TFONTTABLEANDGLYPH_H_

#include <gdi.h>
#include "test/TGraphicsHarness.h"

class CTFontAndGlyphStep : public CTGraphicsStep
    {
public:
    CTFontAndGlyphStep();
protected:  
    //from CTGraphicsStep
    virtual CTGraphicsBase* CreateTestL();
    };

_LIT(KTFontTableAndGlyphStep,"TFontTableAndGlyphOutline");



class CFbsTestForFont: public CFbsFont 
    {
public:
    CFbsTestForFont():CFbsFont() { }
    void Duplicate(TInt aFontHandle) {CFbsFont::Duplicate(aFontHandle);}    
    void SetHandle(TInt aHandle) { iHandle = aHandle; }
    TInt ClientHandle(){return iHandle;};
    ~CFbsTestForFont() { iHandle = 0; }
    };




class CTFontAndGlyph : public CTGraphicsBase
    {
    
public:
    CTFontAndGlyph(CTestStep* aStep);
    ~CTFontAndGlyph();

protected:
//from  CTGraphicsStep
    virtual void RunTestCaseL(TInt aCurTestCase);

private:
    void ConstructL();
    void CleanEnv();

    void GetHintedGlyphById();
    void GetUnHintedGlyphById();
    void GetHintedGlyphByWrongId();
    void GetGlyphWithNoMem();
    void GetFontTable();    
    void GetFontTableByWrongTag();
    void GetFontTableWithNoMem();
    
    void TestFbsFontHandleIsZeroForFont();          
    void TestFbsFontHandleIsZeroForGlyph();         
    void GlyphOutlineIteratorPanics();
    void SMPAndWDPSafeTest();
    
    void OutlinePanics(TInt (*aThreadFun)(TAny*));
    void OutlineLengthPanics(TInt (*aThreadFun)(TAny*));
       
private:
    CFbsBitmap*         iBmp;
    CFbsBitmap*         iBmp2;
    CFbsBitmapDevice*   iDevice;
    CFbsBitmapDevice*   iDevice2;
    CFont*       iFont;
    CFont*       iFont2;
    CFontStore*         iFontStore;
    TFontSpec           iFontSpec, iFontSpec2;
    TUid                iUid, iUid2;    
    };



#endif /* TFONTTABLEANDGLYPH_H_ */
