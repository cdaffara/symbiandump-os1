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

#ifndef __EGLTEST_ENDPOINT_IMAGE_H__
#define __EGLTEST_ENDPOINT_IMAGE_H__

#include <e32std.h>
#include <fbs.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <VG/openvg.h>

// Class Hierarchy to manage a set of images, based on an index.

class CTestImage: public CBase
    {
public:
    virtual ~CTestImage();
    virtual TRgb Pixel(TInt aX, TInt aY) const = 0;
    virtual TSize Size() const = 0;
    TBool CompareImageL(const CTestImage *aImage, TBool aExecptError) const;
    TBool IsSolidColourL() const;
private:
    static TBool ComparePixel(TRgb aPixel1, TRgb aPixel2, TBool aExecptError);
protected:
    CTestImage();
public:
    static const TInt KImageCount;
    };

class CTestCFbsImage: public CTestImage
    {
public:
    static CTestCFbsImage *NewL(TInt aIndex);
    CFbsBitmap *Bitmap();
    virtual ~CTestCFbsImage();
    virtual TRgb Pixel(TInt aX, TInt aY) const;
    virtual TSize Size() const;
protected:
    void ConstructL(TInt aIndex);
    CTestCFbsImage();

private:
    CFbsBitmap *iBitmap;
    RFbsSession iSession;
    };

class CTestVgImage: public CTestImage
    {
protected:
    VGImage iVgImage;
    VGImageFormat iDataFormat;

public:
    static CTestVgImage *NewL(TInt aIndex);
    VGImage VGImage();
    virtual ~CTestVgImage();
    virtual TRgb Pixel(TInt aX, TInt aY) const;
    virtual TSize Size() const;
private:
    void ConstructL(TInt aIndex);
protected:
    CTestVgImage();
    };


class CTestVgEglImage: public CTestVgImage
    {
public:
    static CTestVgEglImage *New(EGLImageKHR aEglImage);
    static CTestVgEglImage *NewL(EGLImageKHR aEglImage);
private:
    void ConstructL(EGLImageKHR aEglImage);

    CTestVgEglImage();
    };


#endif // __EGLTEST_ENDPOINT_IMAGE_H__
