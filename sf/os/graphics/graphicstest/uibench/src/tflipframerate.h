// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TFLIPFRAMERATE_H
#define TFLIPFRAMERATE_H

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <graphics/sgimagecollection.h>
#include <graphics/sgimage.h>
#endif
#include <graphics/surfaceupdateclient.h>
#include <graphics/surfaceconfiguration.h>
#include "tdirectgditestbase.h"

/**
Test case to test the relative performance when a surface is rotated and/or flipped
*/
class CTFlipFramerate : public CTDirectGdiTestBase
    {      
private:
    enum TFlipped
        {
        ENotFlipped =EFalse,
        EFlipped =ETrue
        };
    enum TScreenSizeDivisors
        {        
        EScreensizeFull =1,
        EScreenSizeQuarter =2
        };    
    
    // From CTDirectGdiTestBase
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
#endif
    virtual TVerdict doTestStepL();
    
    void RenderL();
    void GetFontL();
    void TestStepFramerateL(const TFlipped aIsFlipped,
                        const TScreenSizeDivisors aWindowSizeDivisor,
                        const TDesC& aStepName);
    void TestFramerateOrientationL(const TFlipped aIsFlipped,
                        const TScreenSizeDivisors aWindowSizeDivisor,
                        const CFbsBitGc::TGraphicsOrientation aOrientation);
    void SetSizeL(const TScreenSizeDivisors aWindowSizeDivisor);
    
private:    
    RWsSession iWs;
    RWindowGroup iWinGroup;    
    RWindow iWindow;
    CWsScreenDevice* iScreenDev;
    RSurfaceUpdateSession iUpdateSession;
    TSize iSurfaceSize;
    TReal iTextPos;
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    TSgImageInfo iImageInfo;
    RSgImageCollection iImgCol;
    RSgImage iImage;
#endif
    TInt iTextPhraseLength;
    TReal iLastFrameTime;
    CFbsTypefaceStore* iTsStore;
    CFont* iFont;
    TTimeStamp iTimeStamp;
    TReal iTextPhraseSpeed;
    TSurfaceConfiguration iSurfaceConfiguration;    
    };

_LIT(KTFlipFramerate,"tflipframerate");

#endif /* TFLIPFRAMERATE_H */
