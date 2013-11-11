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

#include "egltest_endpoint_images.h"

_LIT(KImage1, "z:\\resource\\apps\\egltest_endpoint\\image1.mbm");
_LIT(KImage2, "z:\\resource\\apps\\egltest_endpoint\\image2.mbm");
_LIT(KImage3, "z:\\resource\\apps\\egltest_endpoint\\image3.mbm");
_LIT(KImage4, "z:\\resource\\apps\\egltest_endpoint\\image4.mbm");


const static TDesC *KImageList[]=
{
        &KImage1,
        &KImage2,
        &KImage3,
        &KImage4,
};

typedef VGImage (*PFNCreateFromEglImage)(EGLImageKHR);

const TInt CTestImage::KImageCount = sizeof(KImageList)/sizeof(KImageList[0]);

CTestImage::~CTestImage()
    {
    }

CTestImage::CTestImage()
    {
    }

TBool CTestImage::ComparePixel(TRgb aPixel1, TRgb aPixel2, TBool aExpectError)
    {
    // In a 16-bit per pixel scenario, we allow (8 bits - 5 bits) = 3 bits,
    // which gives a value of 8  worth of difference.
    const TInt KMargin = 8;
    TBool match = ETrue;
    if (Abs(aPixel1.Red()-aPixel2.Red()) > KMargin ||
        Abs(aPixel1.Green()-aPixel2.Green()) > KMargin ||
        Abs(aPixel1.Blue()-aPixel2.Blue()) > KMargin)
        {
        if (!aExpectError)
            {
            RDebug::Printf("%s:%d: pixel %08x doesn't match %08x", __FILE__, __LINE__, aPixel1.Color16MA(), aPixel2.Color16MA());
            }
        match = EFalse;
        }
    else if (aExpectError)
        {
        RDebug::Printf("%s:%d: pixel %08x matches %08x", __FILE__, __LINE__, aPixel1.Color16MA(), aPixel2.Color16MA());
        }
    return match;
    }


// Compare the this image with the aImage.
// Images must be same size.
TBool CTestImage::CompareImageL(const CTestImage *aImage, TBool aExpectError) const
    {
    TSize size = Size();
    TSize otherSize = aImage->Size();
    if (otherSize != size)
        {
        size.iHeight = Min(size.iHeight, otherSize.iHeight);
        size.iWidth  = Min(size.iWidth, otherSize.iWidth);
        }

    RArray<TPoint> points;
    CleanupClosePushL(points);
    points.AppendL(TPoint(0, 0));     // Top left.
    points.AppendL(TPoint(size.iWidth-1, 0));  // Top right.
    points.AppendL(TPoint(0, size.iHeight-1)); // Bottom left.
    points.AppendL(TPoint(size.iWidth-1, size.iHeight-1)); // Bottom right
    points.AppendL(TPoint(size.iWidth >> 1, size.iHeight >> 1));   // Middle

    TBool retVal = ETrue;
    for(TInt i = 0; i < points.Count(); i++)
        {
        TInt x = points[i].iX;
        TInt y = points[i].iY;
        if (!ComparePixel(Pixel(x, y), aImage->Pixel(x, y), aExpectError))
            {
            retVal = EFalse;
            break;
            }
        }
    TInt err = vgGetError();
    if (err != VG_NO_ERROR)
        {
        RDebug::Printf("%s:%d: Could not fetch pixels??? err=%x", __FILE__, __LINE__, err);
        retVal = EFalse;
        }
    CleanupStack::PopAndDestroy(&points);
    return retVal;
    }


//Checks if the image is a solid colour.
//Only checks corners and middle pixel.
TBool CTestImage::IsSolidColourL() const
    {
    //Points in image to compare (corners and middle).
    TSize size = Size();
    RArray<TPoint> points;
    CleanupClosePushL(points);
    points.AppendL(TPoint(0, 0));                                // Top left.
    points.AppendL(TPoint(size.iWidth-1, 0));                    // Top right.
    points.AppendL(TPoint(0, size.iHeight-1));                   // Bottom left.
    points.AppendL(TPoint(size.iWidth-1, size.iHeight-1));       // Bottom right.
    points.AppendL(TPoint(size.iWidth >> 1, size.iHeight >> 1)); // Middle.

    //Check that all points contain the same colour.
    TBool retVal = ETrue;
    for(TInt i = 1; i < points.Count(); ++i)
        {
        if(Pixel(points[i-1].iX, points[i-1].iY) != Pixel(points[i].iX, points[i].iY))
            {
            retVal = EFalse;
            break;
            }
        }
    TInt err = vgGetError();
    if (err != VG_NO_ERROR)
        {
        RDebug::Printf("Could not fetch pixels??? err=%x", err);
        retVal = EFalse;
        }
    
    CleanupStack::PopAndDestroy(&points);
    return retVal;
    }


CTestCFbsImage *CTestCFbsImage::NewL(TInt aIndex)
    {
    CTestCFbsImage* self = new (ELeave) CTestCFbsImage;
    CleanupStack::PushL(self);
    self->ConstructL(aIndex);
    CleanupStack::Pop(self);
    return self;
    }


void CTestCFbsImage::ConstructL(TInt aIndex)
    {
    // Do this before we leave - else we may disconnect more than we connect!
    RFbsSession::Connect();
    if (aIndex >= KImageCount)
        {
        User::Leave(KErrOverflow);
        }
    iBitmap = new (ELeave) CFbsBitmap;
    TInt err = iBitmap->Load(*KImageList[aIndex], 0);
    if (err != KErrNone)
        {
        RDebug::Printf("Image load error %d for image %d (name=%S)", err, aIndex, KImageList[aIndex]);
        User::Leave(err);
        }

    // Check if it's a 24bpp image. If so, we must convert it to 32bpp.
    // as vgImages can't be 24bpp [in the Hybrid EGL/VG implementation],
    // and we (may) want to make this into a vg-image at some point in future.
    if(iBitmap->DisplayMode() == EColor16M)
        {
        CFbsBitmap *newBitmap = new (ELeave) CFbsBitmap;
        CleanupStack::PushL(newBitmap);
        TSize size = iBitmap->SizeInPixels();
        newBitmap->Create(size, EColor16MU);
        TInt32 *b = new TInt32[size.iWidth];
        TPtr8 buf((TUint8*)b, size.iWidth * sizeof(*b));
        for (TInt y=0; y<size.iHeight; ++y)
            {
            iBitmap->GetScanLine(buf, TPoint(0, y), size.iWidth, EColor16MU);
            newBitmap->SetScanLine(buf, y);
            }
        delete b;
        delete iBitmap;
        CleanupStack::Pop(newBitmap);
        iBitmap = newBitmap;
        }
    }


CFbsBitmap *CTestCFbsImage::Bitmap()
    {
    return iBitmap;
    }


CTestCFbsImage::CTestCFbsImage()
    {
    iBitmap = NULL;
    }


CTestCFbsImage::~CTestCFbsImage()
    {
    delete iBitmap;
    RFbsSession::Disconnect();
    }

TSize CTestCFbsImage::Size() const
    {
    ASSERT(iBitmap);
    return iBitmap->SizeInPixels();
    }

TRgb CTestCFbsImage::Pixel(TInt x, TInt y) const
    {
    TRgb rgb;
    iBitmap->GetPixel(rgb, TPoint(x, y));
    return rgb;
    }

// CTestVgImage...
CTestVgImage::CTestVgImage()
    {
    iVgImage = VG_INVALID_HANDLE;
    iDataFormat = VG_IMAGE_FORMAT_INVALID;
    }


CTestVgImage::~CTestVgImage()
    {
    vgDestroyImage(iVgImage);
    }


CTestVgImage *CTestVgImage::NewL(TInt aIndex)
    {
    CTestVgImage *self = new (ELeave) CTestVgImage;
    CleanupStack::PushL(self);
    self->ConstructL(aIndex);
    CleanupStack::Pop(self);
    return self;
    }


void CTestVgImage::ConstructL(TInt aIndex)
    {
    CTestCFbsImage* bmp = CTestCFbsImage::NewL(aIndex);
    CleanupStack::PushL(bmp);
    CFbsBitmap *bitmap = bmp->Bitmap();
    ASSERT(bitmap);
    TUint8* address = (TUint8*)bitmap->DataAddress();
    TSize size = bitmap->SizeInPixels();
    iDataFormat = VG_IMAGE_FORMAT_INVALID;
    TDisplayMode mode = bitmap->DisplayMode();
    switch(mode)
        {
        case EColor64K:
            iDataFormat = VG_sRGB_565;
            break;
        case EColor16MAP:
            iDataFormat = VG_sARGB_8888_PRE;
            break;
        case EColor16MU:
            iDataFormat = VG_sXRGB_8888;
            break;
        case EColor16MA:
            iDataFormat = VG_sARGB_8888;
            break;
        }
    if (iDataFormat == VG_IMAGE_FORMAT_INVALID)
        {
        User::Leave(KErrBadHandle);
        }
    VGint width = size.iWidth;
    VGint height = size.iHeight;
    VGint dataStride = bitmap->DataStride();

    //this allows us to copy the image correctly into vg coordinates
    address += ((height) - 1) * (dataStride);
    dataStride = 0 - (dataStride);

    //Create VGImage and load Symbian bitmap into it
    iVgImage = vgCreateImage(iDataFormat, width, height, VG_IMAGE_QUALITY_NONANTIALIASED);
    if (iVgImage == VG_INVALID_HANDLE)
        {
        VGint err = vgGetError();
        RDebug::Printf("%s:%d: Could not create vgimage: error = 0x%x", __FILE__, __LINE__, err);
        User::Leave(KErrNotSupported);
        }
    vgImageSubData(iVgImage, address, dataStride, iDataFormat, 0, 0, width, height);
    CleanupStack::PopAndDestroy(bmp);
    }

VGImage CTestVgImage::VGImage()
    {
    return iVgImage;
    }


TSize CTestVgImage::Size() const
    {
    VGint x, y;
    // If this is called a lot, maybe we should cache the value.
    x = vgGetParameteri(iVgImage, VG_IMAGE_WIDTH);
    y = vgGetParameteri(iVgImage, VG_IMAGE_HEIGHT);
    return TSize(x, y);
    }


TRgb CTestVgImage::Pixel(TInt x, TInt y) const
    {
    TRgb rgb(0, 0, 0);
    TSize size = Size();
    if (size.iHeight <= y || size.iWidth <= x)
        {
        ASSERT(0 && "Asked for pixel outside image size");
        return TRgb(0, 0, 0);
        }
    // VG images are "y = 0, x = 0 => bottom left", so we translate it from "y = 0, x = 0 => top left".
    y = (size.iHeight - 1) - y;
    switch(iDataFormat)
        {
        case VG_sRGB_565:
            {
            TUint16 intPixelSample;
            vgGetImageSubData(iVgImage, &intPixelSample, sizeof(intPixelSample), iDataFormat, x, y, 1, 1);
            rgb = TRgb::Color64K(intPixelSample);
            }
            break;
        case VG_sARGB_8888:
        case VG_sXRGB_8888:
        case VG_sARGB_8888_PRE:
            {
            TUint32 intPixelSample;
            vgGetImageSubData(iVgImage, &intPixelSample, sizeof(intPixelSample), iDataFormat, x, y, 1, 1);
            rgb = TRgb::Color16MA(intPixelSample);
            }
            break;
            
        case VG_sABGR_8888:
        case VG_sXBGR_8888:
        case VG_sABGR_8888_PRE:
            {
            TUint32 intPixelSample;
            vgGetImageSubData(iVgImage, &intPixelSample, sizeof(intPixelSample), iDataFormat, x, y, 1, 1);
            rgb = TRgb::Color16MA(intPixelSample);
            // Now swap R & B, as we have BGR in the source, not RGB!
            TInt temp = rgb.Red();
            rgb.SetRed(rgb.Blue());
            rgb.SetBlue(temp);
            }
            break;
            
        default:
            ASSERT(0 && "Invalid dataformat");
            break;
        }
    return rgb;
    }

CTestVgEglImage::CTestVgEglImage()
    {
    }

CTestVgEglImage* CTestVgEglImage::New(EGLImageKHR aEglImage)
    {
    CTestVgEglImage *self = new (ELeave) CTestVgEglImage;
    if (self)
        {
        CleanupStack::PushL(self);
        TRAPD(err, self->ConstructL(aEglImage));
        if (err != KErrNone)
            {
            CleanupStack::PopAndDestroy(self);
            return NULL;
            }
        else
            {
            CleanupStack::Pop(self);
            }
        }
    return self;
    }


CTestVgEglImage* CTestVgEglImage::NewL(EGLImageKHR aEglImage)
    {
    CTestVgEglImage *self = New(aEglImage);
    User::LeaveIfNull(self);
    return self;
    }


void CTestVgEglImage::ConstructL(EGLImageKHR aEglImage)
    {
    PFNCreateFromEglImage createFromEglTarget =
        reinterpret_cast<PFNCreateFromEglImage>(eglGetProcAddress("vgCreateEGLImageTargetKHR"));
    iVgImage = createFromEglTarget(aEglImage);
    if (iVgImage == VG_INVALID_HANDLE)
        {
        RDebug::Printf("%s:%d: Unable to create VGImage from EGLImage: Error code=0x%x", __FILE__, __LINE__, vgGetError());
        User::Leave(KErrBadHandle);
        }
    iDataFormat = static_cast<VGImageFormat>(vgGetParameteri(iVgImage, VG_IMAGE_FORMAT));
    }
