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

#ifndef TFBSGLYPHDATAPANIC_H
#define TFBSGLYPHDATAPANIC_H

#include <sgresource/sgresource.h>
#include "test/TGraphicsHarness.h"
class RFbsGlyphDataIterator;

/**
Test class which exercises all panics for the glyph data extensions 
RFbsGlyphDataIterator and RFbsGlyphMetricsArray. 
Negative tests
*/
class CTFbsGlyphDataPanic : public CTGraphicsBase
    {
public:
    CTFbsGlyphDataPanic(CTestStep* aStep);
    ~CTFbsGlyphDataPanic();
protected:
//from  CTGraphicsBase
    virtual void RunTestCaseL(TInt aCurTestCase);
    void ConstructL();
    
//test cases
    void TestGlyphDataIteratorNotInitializedNext();
    void TestGlyphDataIteratorNotInitializedImage();
    void TestGlyphDataIteratorNotInitializedMetrics();
    void TestGlyphDataIteratorNotInitializedGlyphCode();
    void TestGlyphDataIteratorClosedNext();
    void TestGlyphDataIteratorClosedImage();
    void TestGlyphDataIteratorClosedMetrics();
    void TestGlyphDataIteratorClosedGlyphCode();
    void TestGlyphDataIteratorDiscardUsedFont();
    void TestGlyphMetricsArrayIndexNegative();
    void TestGlyphMetricsArrayIndexTooHigh();
    void TestGlyphDataIteratorOpenFailed();

    //helper function
    void OpenAndCloseIterator(RFbsGlyphDataIterator& aIterator); // the function opens and then closes an iterator
private:
    RFbsSession* iFbs;
    CFbsTypefaceStore* iTs;
    CFbsFont* iFont;
    RSgDriver iSgDriver;
    TUint* iGlyphCodes;
   };

class CTFbsGlyphDataPanicStep : public CTGraphicsStep
    {
public:
    CTFbsGlyphDataPanicStep();
protected:  
    //from CTGraphicsStep
    virtual CTGraphicsBase* CreateTestL();
    };

_LIT(KTFbsGlyphDataPanicStep,"TFbsGlyphDataPanic");


#endif /* TFBSGLYPHDATAPANIC_H */
