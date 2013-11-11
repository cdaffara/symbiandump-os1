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


#include "egltest_surface.h"
#include "egltest_endpoint_images.h"
#include <graphics/surfaceconfiguration.h>
#include <e32std.h>
#include <e32math.h>
#include <VG/vgu.h>


#define SURF_ASSERT(x) { if (!(x)) { RDebug::Printf("ASSERT(%s) failed at %s:%d", #x, __FILE__, __LINE__); User::Panic(_L("ASSERT SURF"), __LINE__); }}

// Macros for indicating what is what.
#define SIZE(x, y) x, y
#define Buffers(x) x
#define DefaultAlignment 32  // Pick some value that is valid.
#define Alignment(x) x
#define Stride(x) x
#define DefaultStride 0
// Note: Offset to first buffer.
#define Offset(x) x
#define WindowPos(x, y)  x, y
#define WindowMode(m) m

#define LARGEST_POSSIBLE_FLAG 0x80000000

static const TSurfaceParamsCommon KSurfaceParams[] =
{
    {
        EStandardSurface,
        SIZE(100, 100),
        Buffers(2),
        DefaultAlignment,
        DefaultStride,
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        EFalse,
        { 0 },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },
    {
        EBadAttribSurface,
        SIZE(100, 100),
        Buffers(2),
        DefaultAlignment,
        DefaultStride,
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        ETrue,
        { 1, 1, EGL_NONE },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },
    {
        EEmptyAttribSurface,
        SIZE(100, 100),
        Buffers(2),
        DefaultAlignment,
        DefaultStride,
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        ETrue,
        { EGL_NONE },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },
    {
        EStandard128sqSurface,
        SIZE(128, 128),
        Buffers(3),
        DefaultAlignment,
        DefaultStride,
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        EFalse,
        { 0 },
        WindowPos(20, 20),
        WindowMode(EColor16MAP)
    },
    {
        EUnusualStrideSurface,
        SIZE(167,263),
        Buffers(2),
        Alignment(8),
        Stride(167*4+64),
        Offset(200),
        EUidPixelFormatARGB_8888_PRE,
        EFalse,
        { 0 },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },
    {
        EUnalignedPixelSizeSurface,
        SIZE(103, 107),
        Buffers(2),
        Alignment(8),
        Stride(103*4),
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        EFalse,
        { 0 },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },
    {
        ELargeSurface,
        SIZE(800, 600),
        Buffers(2),
        DefaultAlignment,
        DefaultStride,
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        EFalse,
        { 0 },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },
    {
        ELargestPossibleSurface,
        SIZE(LARGEST_POSSIBLE_FLAG, LARGEST_POSSIBLE_FLAG),
        Buffers(2),
        DefaultAlignment,
        DefaultStride,
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        EFalse,
        { 0 },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },    
    {
        ESmallSurface,
        SIZE(16, 16),
        Buffers(1),
        DefaultAlignment,
        DefaultStride,
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        EFalse,
        { 0 },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },
    {
        ETinySurface,
        SIZE(8, 8),
        Buffers(1),
        DefaultAlignment,
        DefaultStride,
        Offset(0),
        EUidPixelFormatARGB_8888_PRE,
        EFalse,
        { 0 },
        WindowPos(0, 0),
        WindowMode(EColor16MAP)
    },
};

const TInt KSurfaceMaxIndex = sizeof(KSurfaceParams) / sizeof(KSurfaceParams[0]);

struct TSurfaceSize
    {
    TInt iWidth;
    TInt iHeight;
    };

static const TSurfaceSize KSurfaceSizes[] =
    {
        {  320,  240 },
        {  640,  480 },
        {  720,  480 },
        {  854,  480 },
        {  720,  576 },
        {  854,  576 },
        { 1280,  720 },
        { 1024,  768 },
        { 1280, 1024 },
        { 1920, 1080 },
        { 1600, 1200 },
#if 0
        { 2048, 1536 },
        { 2560, 1920 },
        { 3648, 2736 },
        { 4216, 2638 },
        { 4000, 3000 },
        { 4616, 2600 },
#endif
    };

const TInt KMaxSurfaceSizes =  sizeof(KSurfaceSizes) / sizeof(KSurfaceSizes[0]);

LOCAL_C TUint RandomNumberInRange(TUint aLow, TUint aHigh)
    {
    TReal32 rand = Math::Random();
    rand /= KMaxTUint;
    rand *= aHigh - aLow;
    rand += aLow;
    return TUint(rand);
    }


void CSurface::CreateL(TInt aIndex)
    {
    CreateL(aIndex, TPoint(0, 0));
    }


TSize CSurface::Size()
    {
    return iActualSize;
    }


TInt CSurface::SizeInBytes() const
    {
    RSurfaceManager::TInfoBuf infoBuf;
    RSurfaceManager surfMgr;
    TInt err = surfMgr.Open();
    if (err != KErrNone)
        {
        RDebug::Printf("Error opening surface manager... Err=%d", err);
        return 0;
        }
    err = surfMgr.SurfaceInfo(SurfaceId(), infoBuf);
    if (err != KErrNone)
        {
        RDebug::Printf("Could not get surface info - err = %d", err);
        return 0;
        }
    TInt size = infoBuf().iBuffers * infoBuf().iSize.iHeight * infoBuf().iStride;
    surfMgr.Close();
    return size;
    }


CRawSurface* CRawSurface::NewL()
    {
    CRawSurface* obj = new (ELeave) CRawSurface();
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
    }



CRawSurface::CRawSurface() : iDrawBuffer(0), iBuffers(0)
    {
    }


void CRawSurface::ConstructL()
    {
    iSurfaceId = TSurfaceId::CreateNullId();
    User::LeaveIfError(iSurfaceManager.Open());
    User::LeaveIfError(iSurfaceUpdate.Connect());
    }


CRawSurface::~CRawSurface()
    {
    iSurfaceUpdate.Close();
    if(!iSurfaceId.IsNull())
        {
        iSurfaceManager.CloseSurface(iSurfaceId);
        }
    iSurfaceManager.Close();
    }


TInt CRawSurface::PixelSize(TUidPixelFormat aPixelFormat)
    {
    switch(aPixelFormat)
        {
        case EUidPixelFormatARGB_8888_PRE:
        case EUidPixelFormatARGB_8888:
        case EUidPixelFormatABGR_8888:
        case EUidPixelFormatABGR_8888_PRE:
            return 4;

        case EUidPixelFormatARGB_4444:
        case EUidPixelFormatRGB_565:
            return 2;

        default:
            SURF_ASSERT(0);
            break;
        }
    return 0; // Make sure no compiler moans about "not all paths return a value".
    }


void CRawSurface::GetSurfAttribs(RSurfaceManager::TSurfaceCreationAttributesBuf &aSurfaceAttribs, 
        TInt aIndex, TInt aSizeIndex)
    {
    SURF_ASSERT(aIndex < KSurfaceMaxIndex);
    SURF_ASSERT(aIndex == KSurfaceParams[aIndex].iIndex);
    iParamIndex = aIndex;
    if (KSurfaceParams[aIndex].iXSize & LARGEST_POSSIBLE_FLAG)
        {
        
        aSurfaceAttribs().iSize = 
                TSize(KSurfaceSizes[aSizeIndex].iWidth, KSurfaceSizes[aSizeIndex].iHeight);
        }
    else
        {
        aSurfaceAttribs().iSize = 
                TSize(KSurfaceParams[aIndex].iXSize, KSurfaceParams[aIndex].iYSize);
        }
    iBuffers = KSurfaceParams[aIndex].iBuffers;
    aSurfaceAttribs().iBuffers = iBuffers;
    aSurfaceAttribs().iPixelFormat = KSurfaceParams[aIndex].iPixelFormat;
    TInt stride = KSurfaceParams[aIndex].iStrideInBytes;
    if (stride == 0)
        {
        stride = aSurfaceAttribs().iSize.iWidth * PixelSize(KSurfaceParams[aIndex].iPixelFormat);
        }
    aSurfaceAttribs().iStride = stride;
    aSurfaceAttribs().iOffsetToFirstBuffer = KSurfaceParams[aIndex].iOffsetToFirstBuffer;
    aSurfaceAttribs().iAlignment = KSurfaceParams[aIndex].iAlignment;
    aSurfaceAttribs().iContiguous = EFalse;
    aSurfaceAttribs().iCacheAttrib = RSurfaceManager::ECached;
    aSurfaceAttribs().iOffsetBetweenBuffers = 0;
    aSurfaceAttribs().iSurfaceHints = NULL;
    aSurfaceAttribs().iHintCount = 0;
    aSurfaceAttribs().iMappable = ETrue;
    }


void CRawSurface::CreateL(TInt aIndex, const TPoint &/* aPoint */)
    {
    RSurfaceManager::TSurfaceCreationAttributesBuf surfaceAttribs;
    SURF_ASSERT(aIndex < KSurfaceMaxIndex);
    TInt sizeIndex = 0;
    if (KSurfaceParams[aIndex].iXSize & LARGEST_POSSIBLE_FLAG)
        {
        sizeIndex = KMaxSurfaceSizes-1;
        
        }
    TInt err = KErrNone;
    do
        {
        GetSurfAttribs(surfaceAttribs, aIndex, sizeIndex);
        err = iSurfaceManager.CreateSurface(surfaceAttribs, iSurfaceId);
        iActualSize = surfaceAttribs().iSize;
        sizeIndex--;
        }
    while(err != KErrNone && sizeIndex >= 0);
    User::LeaveIfError(err);
    }


TUint8* CRawSurface::MapSurfaceAndGetInfoLC(RSurfaceManager::TSurfaceInfoV01& aInfo)
    {
    SURF_ASSERT(!iSurfaceId.IsNull());
    User::LeaveIfError(iSurfaceManager.MapSurface(iSurfaceId, iSurfaceChunk));
    CleanupClosePushL(iSurfaceChunk);
    RSurfaceManager::TInfoBuf infoBuf;
    User::LeaveIfError(iSurfaceManager.SurfaceInfo(iSurfaceId, infoBuf));
    aInfo = infoBuf();
    TInt offset = -1000;  // So we hopefully detect when it goes horribly wrong.
    User::LeaveIfError(iSurfaceManager.GetBufferOffset(iSurfaceId, iDrawBuffer, offset));
    SURF_ASSERT(offset >= 0);
    return iSurfaceChunk.Base() + offset;
    }


void CRawSurface::DrawContentL(TInt aImageIndex)
    {
    CTestCFbsImage *image = CTestCFbsImage::NewL(aImageIndex);
    CleanupStack::PushL(image);

    RSurfaceManager::TSurfaceInfoV01 info;
    TUint8 *dataAddress = MapSurfaceAndGetInfoLC(info);
    TInt stride = info.iStride;

    CFbsBitmap *bitmap = image->Bitmap();
    TDisplayMode displaymode = bitmap->DisplayMode();
    TInt pixelStride = stride / CFbsBitmap::ScanLineLength(1, displaymode);
    for(TInt y = 0; y < image->Size().iHeight; y++)
        {
        TPtr8 buf(dataAddress + y * stride, stride);

        // TODO: We need to check that the bitsperpixel matches between the surface and bitmap.
        bitmap->GetScanLine(buf, TPoint(0, y), pixelStride, displaymode);
        }

    CleanupStack::PopAndDestroy(2, image);
    }

void CRawSurface::DrawContentL(const TRgb& aColour)
    {
    //Map the surface and get its info.
    RSurfaceManager::TSurfaceInfoV01 surfaceInfo;
    TUint32* buffer = (TUint32*)MapSurfaceAndGetInfoLC(surfaceInfo);

    //Currently this function only supports drawing into ARGB_8888_PRE surfaces.
    //This is because the only test that uses this function uses this type of surface.
    //If this functionallity needs expanding, you must correctly convert the TRgb colour
    //and pack it into the surface buffer correctly.
    SURF_ASSERT(surfaceInfo.iPixelFormat == EUidPixelFormatARGB_8888_PRE);

    TUint32 fillColour = aColour._Color16MAP();

    //Loop over each pixel in the surface and colour it.
    //This is deliberately slow since it is only used for the tearing test
    //and we want to spend most of our time drawing so that the chances of the other thread
    //picking up a buffer in the middle of drawing is increased.
    for(TInt y=0; y < surfaceInfo.iSize.iHeight; ++y)
        {
        for(TInt x=0; x < surfaceInfo.iSize.iWidth; ++x)
            {
            buffer[x] = fillColour;
            }
        buffer += surfaceInfo.iStride >> 2;
        }

    CleanupStack::PopAndDestroy();
    }


void CRawSurface::DrawComplexL(const TRgb& aColour)
    {
    DrawContentL(aColour);
    }


TInt CRawSurface::SubmitContent(TBool aShouldWaitForDisplay, TInt /* aRectsIndex */)
    {
    TRequestStatus displayNotify = KRequestPending;
    TTimeStamp timeStamp;

    if(aShouldWaitForDisplay)
        {
        Notify(ENotifyWhenDisplayed, displayNotify, 0);
        }

    TInt err = iSurfaceUpdate.SubmitUpdate(KAllScreens, iSurfaceId, iDrawBuffer, NULL);
    if (err != KErrNone)
        {
        if (err != KErrNone)
            {
            RDebug::Printf("%s:%d: SubmitUpdate gave unexpected error %d", __FILE__, __LINE__, err);
            }
        return err;
        }
    iDrawBuffer = (iDrawBuffer + 1) % iBuffers;

    if(aShouldWaitForDisplay)
        {
        TUint32 dummy;
        err = WaitFor(ENotifyWhenDisplayed, displayNotify, 100 * 1000, dummy);
        if (err != KErrNone && err != KErrNotVisible && err != KErrOverflow)
            {
//            RDebug::Printf("%s:%d: NotifyWhenDisplayed gave unexpected error %d", __FILE__, __LINE__, err);
            return err;
            }
        }
    return KErrNone;
    }


TSurfaceId CRawSurface::SurfaceId() const
    {
    return iSurfaceId;
    }


void CRawSurface::GetSurfaceParamsL(TSurfaceParamsRemote &aParams)
    {
    aParams.iCommonParams = KSurfaceParams[iParamIndex];
    aParams.iCommonParams.iBuffers = iBuffers; // May have been changed if it's a single buffered surface...
    aParams.iSurfaceId = SurfaceId();
    }

const TText *CRawSurface::GetSurfaceTypeStr() const
    {
    return _S("CRawSurface");
    }

TInt CRawSurface::Notify(TNotification aWhen, TRequestStatus& aStatus, TUint32 aXTimes)
    {
    aStatus = KRequestPending;
    switch(aWhen)
        {
        case ENotifyWhenAvailable:
            iSurfaceUpdate.NotifyWhenAvailable(aStatus);
            break;
        case ENotifyWhenDisplayed:
            iSurfaceUpdate.NotifyWhenDisplayed(aStatus, iTimeStamp);
            break;
        case ENotifyWhenDispXTimes:
            iSurfaceUpdate.NotifyWhenDisplayedXTimes(aXTimes, aStatus);
            break;
        default:
            RDebug::Printf("%s:%d: Invalid notification: %d. Panicking...", __FILE__, __LINE__, aWhen);
            User::Panic(_L("CRawSurface::Notify()"), __LINE__);
            break;
        }
    return KErrNone;
    }


TInt CRawSurface::WaitFor(TNotification aWhen, TRequestStatus& aStatus, TInt aTimeoutInMicroSeconds, TUint32& aTimeStamp)
    {
    RTimer timer;
    TInt err = timer.CreateLocal();
    if (err != KErrNone)
        {
        RDebug::Printf("%s:%d: Could not create timer... err= %d", __FILE__, __LINE__, err);
        return err;
        }
    TRequestStatus timerStatus = KRequestPending;
#if __WINS__
    // Windows timer isn't very precise - add some "fuzz" to the timeout to ensure we do not wait "too little".
    const TInt KTimeOutExtra = 20000;
#else
    // On hardware, we should be able to run with less "fuzz".
    const TInt KTimeOutExtra = 10000;
#endif
    timer.HighRes(timerStatus, aTimeoutInMicroSeconds + KTimeOutExtra);
    User::WaitForRequest(timerStatus, aStatus);
    if (aStatus == KRequestPending)
        {
        if (aWhen == ENotifyWhenDisplayed)
            {
            aTimeStamp = User::FastCounter();
            }
        return KErrTimedOut;
        }
    if (aWhen == ENotifyWhenDisplayed)
        {
        aTimeStamp = iTimeStamp();
        }
    timer.Cancel();
    timer.Close();
    TInt result = aStatus.Int();
    aStatus = KRequestPending;
    return result;
    }

const TText *CRawSingleBufferSurface::GetSurfaceTypeStr() const
    {
    return _S("CRawSingleBufferedSurface");
    }

CRawSingleBufferSurface *CRawSingleBufferSurface::NewL()
    {
    CRawSingleBufferSurface* obj = new (ELeave) CRawSingleBufferSurface();
    CleanupStack::PushL(obj);
    obj->ConstructL();
    CleanupStack::Pop(obj);
    return obj;
    }


void CRawSingleBufferSurface::CreateL(TInt aIndex, const TPoint & /*aPoint */)
    {
    RSurfaceManager::TSurfaceCreationAttributesBuf surfaceAttribs;

    TInt sizeIndex = 0;
    if (KSurfaceParams[aIndex].iXSize & LARGEST_POSSIBLE_FLAG)
        {
        sizeIndex = KMaxSurfaceSizes-1;
        }
    TInt err = KErrNone;
    do
        {
        GetSurfAttribs(surfaceAttribs, aIndex, sizeIndex);
        iBuffers = 1;
        surfaceAttribs().iBuffers = 1;
        err = iSurfaceManager.CreateSurface(surfaceAttribs, iSurfaceId);
        iActualSize = surfaceAttribs().iSize;
        sizeIndex--;
        }
    while(err != KErrNone && sizeIndex >= 0);
    }

CRawSingleBufferSurface::~CRawSingleBufferSurface()
    {
    }


TInt CEglSurfaceBase::Activate()
    {
    if (!eglMakeCurrent(iDisplay, iSurface, iSurface, iContext))
        {
        EGLint err = eglGetError();
        RDebug::Printf("%s:%d: eglMakeCurrent gave error 0x%x", __FILE__, __LINE__, err);
        return KErrBadHandle;
        }
    return KErrNone;
    }

void CEglSurfaceBase::ActivateL()
    {
    User::LeaveIfError(Activate());
    }

void CEglSurfaceBase::DrawComplexL(const TRgb& aColour)
    {
    ActivateL();

    TSize size;
    eglQuerySurface(iDisplay, iSurface, EGL_WIDTH, &size.iWidth);
    eglQuerySurface(iDisplay, iSurface, EGL_HEIGHT, &size.iHeight);
    
    //Paint lots of random circles to keep the GPU busy.
    for(TInt i=0; i < 300; i++)
        {
        VGPaint paint = vgCreatePaint();
        VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_APPEND_TO);
        
        TInt minDim = Min(size.iWidth, size.iHeight);
        VGfloat cx = RandomNumberInRange(0, size.iWidth);
        VGfloat cy = RandomNumberInRange(0, size.iHeight);
        VGfloat diameter = RandomNumberInRange(minDim / 20, minDim / 3);
        TRgb fillColour(RandomNumberInRange(0, 255), RandomNumberInRange(0, 255), RandomNumberInRange(0, 255), RandomNumberInRange(0, 255));
        
        vguEllipse(path, cx, cy, diameter, diameter);
        vgSetPaint(paint, VG_FILL_PATH);
        vgSetColor(paint, fillColour.Value());
        vgDrawPath(path, VG_FILL_PATH);
        
        vgDestroyPath(path);
        vgDestroyPaint(paint);
        }
    
    //Paint the top corner with aColour so we can identify the drawing.
    VGfloat fillColour[4];
    fillColour[0] = (VGfloat)aColour.Red() / 255.0f;
    fillColour[1] = (VGfloat)aColour.Green() / 255.0f;
    fillColour[2] = (VGfloat)aColour.Blue() / 255.0f;
    fillColour[3] = (VGfloat)aColour.Alpha() / 255.0f;
    
    vgSetfv(VG_CLEAR_COLOR, 4, fillColour);
    vgClear(0, 0, 20, size.iHeight);
    }

void CEglSurfaceBase::DrawContentL(const TRgb& aColour)
    {
    ActivateL();

    TSize size;
    eglQuerySurface(iDisplay, iSurface, EGL_WIDTH, &size.iWidth);
    eglQuerySurface(iDisplay, iSurface, EGL_HEIGHT, &size.iHeight);

    VGfloat fillColour[4];
    fillColour[0] = (VGfloat)aColour.Red() / 255.0f;
    fillColour[1] = (VGfloat)aColour.Green() / 255.0f;
    fillColour[2] = (VGfloat)aColour.Blue() / 255.0f;
    fillColour[3] = (VGfloat)aColour.Alpha() / 255.0f;

    vgSetfv(VG_CLEAR_COLOR, 4, fillColour);
    vgClear(0, 0, size.iWidth, size.iHeight);
    }

void CEglSurfaceBase::CreateL(TInt aIndex, const TPoint &aOffset)
    {
    SURF_ASSERT(aIndex < KSurfaceMaxIndex);
    SURF_ASSERT(aIndex == KSurfaceParams[aIndex].iIndex);

    TInt sizeIndex = 0;
    if (KSurfaceParams[aIndex].iXSize & LARGEST_POSSIBLE_FLAG)
        {
        sizeIndex = KMaxSurfaceSizes-1;
        }
    TInt err = KErrNone;
    do
        {
        TRAP(err, DoCreateL(aIndex, aOffset, sizeIndex));
        sizeIndex--;
        }
    while(err != KErrNone && sizeIndex >= 0);
    if (err != KErrNone)
        {
//        RDebug::Printf("%s:%d: err=%d (%d x %d)", __FILE__, __LINE__, err, iActualSize.iWidth, iActualSize.iHeight);
        User::Leave(err);
        }
    }

TInt CEglSurfaceBase::SubmitContent(TBool aShouldWaitForDisplay, TInt /* aRectsIndex */)
    {
    TInt err = Activate();
    if (err != KErrNone)
        {
        return err;
        }
    if (!eglSwapBuffers(iDisplay, iSurface))
        {
        EGLint err = eglGetError();
        RDebug::Printf("%s:%d: eglSwapBuffers gave error 0x%x", __FILE__, __LINE__, err);
        return KErrBadHandle;
        }
    if (aShouldWaitForDisplay)
        {
        // We are cheating: We just wait for a bit to ensure that the swapbuffer is actually finished.
        // There is no way to determine how long this takes, so we just grab a number that should be
        // large enough...
        User::After(100 * 1000);  // Wait 100ms.
        }
    return KErrNone;
    }

void CEglSurfaceBase::DrawContentL(TInt aIndex)
    {
    ActivateL();
    CTestVgImage *vgImage = CTestVgImage::NewL(aIndex);
    CleanupStack::PushL(vgImage);
    vgDrawImage(vgImage->VGImage());
    CleanupStack::PopAndDestroy(vgImage);
    }

void CEglSurfaceBase::GetSurfaceParamsL(TSurfaceParamsRemote &aParams)
    {
    RSurfaceManager surfaceManager;
    User::LeaveIfError(surfaceManager.Open());
    RSurfaceManager::TInfoBuf infoBuf;
    TInt err = surfaceManager.SurfaceInfo(SurfaceId(), infoBuf);
    User::LeaveIfError(err);
    surfaceManager.Close();
    RSurfaceManager::TSurfaceInfoV01& info = infoBuf();
    aParams.iSurfaceId = SurfaceId();
    aParams.iCommonParams.iAlignment = -1;  // N/A
    aParams.iCommonParams.iBuffers = info.iBuffers;
    aParams.iCommonParams.iOffsetToFirstBuffer = -1;
    aParams.iCommonParams.iPixelFormat = info.iPixelFormat;
    aParams.iCommonParams.iStrideInBytes = info.iStride;
    aParams.iCommonParams.iXSize = info.iSize.iWidth;
    aParams.iCommonParams.iYSize = info.iSize.iHeight;
    aParams.iCommonParams.iUseAttribList = KSurfaceParams[iParamIndex].iUseAttribList;
    for(TInt i = 0; i < KNumAttribs; i++)
        {
        aParams.iCommonParams.iAttribs[i] = KSurfaceParams[iParamIndex].iAttribs[i];
        }
    }


TInt CEglSurfaceBase::Notify(TNotification /*aWhen*/, TRequestStatus& /*aStatus*/, TUint32 /*aXTImes*/)
    {
    return KErrNotSupported;
    }

TInt CEglSurfaceBase::WaitFor(TNotification /*aWhen*/, TRequestStatus& /*aStatus*/,
        TInt /*aTimeoutinMicroseconds*/, TUint32 & /*aTimeStamp*/)
    {
    return KErrNotSupported;
    }

void CEglSurfaceBase::BaseCreateL(TInt aIndex, EGLint aSurfaceType)
    {
    iParamIndex = aIndex;
    iDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    
    EGLint err;
    if (iDisplay == EGL_NO_DISPLAY)
        {
        err = eglGetError();
        RDebug::Printf("%s:%d: err = 0x%x", __FILE__, __LINE__, err);
        User::Leave(KErrNotSupported);
        }
    
    EGLint nConfigs = 0;
    
    // TODO: Need to use differnet config attribs based on aIndex.
    EGLint configAttribs[] =
    {
        EGL_BUFFER_SIZE,    32,
        EGL_RED_SIZE,       8,
        EGL_GREEN_SIZE,     8,
        EGL_BLUE_SIZE,      8,
        EGL_ALPHA_SIZE,     8,
        EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
        EGL_NONE
    };
    
    // Update surfacetype type to match 
    for(TInt i = 0; configAttribs[i] != EGL_NONE; i += 2)
        {
        if (configAttribs[i] == EGL_SURFACE_TYPE)
            {
            configAttribs[i+1] = aSurfaceType;
            }
        }
    // Need some way to configure the attribs ...
    eglChooseConfig(iDisplay, configAttribs, &iConfig, 1, &nConfigs);
    if (!nConfigs)
        {
        err = eglGetError();
        RDebug::Printf("%s:%d: err = %d", __FILE__, __LINE__, err);
        User::Leave(KErrNotSupported);
        }
    
    if (!eglBindAPI(EGL_OPENVG_API))
        {
        err = eglGetError();
        RDebug::Printf("%s:%d: err = %d", __FILE__, __LINE__, err);
        User::Leave(KErrNotSupported);
        }
    iContext = eglCreateContext(iDisplay, iConfig, 0, NULL);
    if (iContext == EGL_NO_CONTEXT)
        {
        err = eglGetError();
        //RDebug::Printf("%s:%d: err = %d", __FILE__, __LINE__, err);
        User::Leave(KErrNotSupported);
        }
    }

void CEglSurfaceBase::Destroy()
    {
    eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (iSurface != EGL_NO_SURFACE)
        {
        eglDestroySurface(iDisplay, iSurface);
        iSurface = EGL_NO_SURFACE;
        }

    if (iDisplay != EGL_NO_DISPLAY)
        {
        eglDestroyContext(iDisplay, iContext);
        }
    }


class CWindow: public CBase
    {
public:
    static CWindow *NewL(TInt aIndex, const TPoint &aOffset, TInt aSizeIndex);
    RWindow& Window();
    ~CWindow();
private:
    void ConstructL(TInt aIndex, const TPoint &aOffset, TInt aSizeIndex);
    CWindow();

private:
    RWindow iWindow;
    RWindowGroup iWindowGroup;
    RWsSession iWsSession;
    };


CWindow* CWindow::NewL(TInt aIndex, const TPoint &aOffset, TInt aSizeIndex)
    {
    CWindow *self = new (ELeave) CWindow;
    CleanupStack::PushL(self);
    self->ConstructL(aIndex, aOffset, aSizeIndex);
    CleanupStack::Pop(self);
    return self;
    }


void CWindow::ConstructL(TInt aIndex, const TPoint &aOffset, TInt aSizeIndex)
    {
    RFbsSession::Connect();
    if (aIndex >= KSurfaceMaxIndex)
        {
        User::Leave(KErrOverflow);
        }
    User::LeaveIfError(iWsSession.Connect());
    iWindowGroup = RWindowGroup(iWsSession);
    User::LeaveIfError(iWindowGroup.Construct((TUint32)this));
    iWindow = RWindow(iWsSession);
    User::LeaveIfError(iWindow.Construct(iWindowGroup, -1U));
    TSurfaceParamsCommon winAttrib = KSurfaceParams[aIndex];
    TSize winSize;
    if (winAttrib.iXSize & LARGEST_POSSIBLE_FLAG)
        {
        winSize = TSize(KSurfaceSizes[aSizeIndex].iWidth, KSurfaceSizes[aSizeIndex].iHeight);
        }
    else
        {
        winSize = TSize(winAttrib.iXSize, winAttrib.iYSize);
        }
    iWindow.SetExtent(TPoint(winAttrib.iXPos + aOffset.iX, winAttrib.iYPos + aOffset.iY), winSize);
    iWindow.SetRequiredDisplayMode(winAttrib.iDisplayMode);
    iWindow.Activate();
    }


CWindow::~CWindow()
    {
    iWindow.Close();
    iWindowGroup.Close();
    iWsSession.Close();
    RFbsSession::Disconnect();
    }


CWindow::CWindow()
    {
    }


RWindow& CWindow::Window()
    {
    return iWindow;
    }


CEglWindowSurface* CEglWindowSurface::NewL()
    {
    CEglWindowSurface* self = new (ELeave) CEglWindowSurface;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


void CEglWindowSurface::ConstructL()
    {
    }


CEglWindowSurface::CEglWindowSurface()
    {
    }


void CEglWindowSurface::DoCreateL(TInt aIndex, const TPoint &aOffset, TInt aSizeIndex)
    {
    iParamIndex = aIndex;
    iWindow = CWindow::NewL(aIndex, aOffset, aSizeIndex);
    iActualSize = iWindow->Window().Size();
    
    CEglSurfaceBase::BaseCreateL(aIndex, EGL_WINDOW_BIT);
    
    iSurface = eglCreateWindowSurface(iDisplay, iConfig, &iWindow->Window(), NULL);
    if (iSurface == EGL_NO_SURFACE)
        {
        EGLint err = eglGetError();
        RDebug::Printf("%s:%d: err = %x (%d x %d)", __FILE__, __LINE__, err, iActualSize.iWidth, iActualSize.iHeight);
        User::Leave(KErrNotSupported);
        }
    }


CEglWindowSurface::~CEglWindowSurface()
    {
    Destroy();
	eglReleaseThread();
    delete iWindow;
    }


TSurfaceId CEglWindowSurface::SurfaceId() const
    {
    // Default constructor for id sets it to a NULL-value, so if no window is created, we get
    // a defined surface id value that is invalid.
    TSurfaceId id;
    if (iWindow)
        {
        TSurfaceConfiguration surfConfig;
        iWindow->Window().GetBackgroundSurface(surfConfig);
        surfConfig.GetSurfaceId(id);
        }
    return id;
    }


const TText *CEglWindowSurface::GetSurfaceTypeStr() const
    {
    return _S("CEglWindowSurface");
    }


CEglPBufferSurface::CEglPBufferSurface()
    {
    }


CEglPBufferSurface::~CEglPBufferSurface()
    {
    Destroy();
	eglReleaseThread();
    }
    

CEglPBufferSurface* CEglPBufferSurface::NewL()
    {
    CEglPBufferSurface* self = new (ELeave) CEglPBufferSurface;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


void CEglPBufferSurface::ConstructL()
    {
    }
    

const TText *CEglPBufferSurface::GetSurfaceTypeStr() const
    {
    return _S("CEglPBufferSurface");
    }


void CEglPBufferSurface::DoCreateL(TInt aIndex, const TPoint &/*aOffset*/, TInt aSizeIndex)
    {
    CEglSurfaceBase::BaseCreateL(aIndex, EGL_PBUFFER_BIT);

    EGLint attribs[] = 
            {
                EGL_WIDTH, 0,
                EGL_HEIGHT, 0,
                EGL_NONE,
            };
    if (KSurfaceParams[aIndex].iXSize & ELargestPossibleSurface)
        {
        iActualSize.iWidth = KSurfaceSizes[aSizeIndex].iWidth;
        iActualSize.iHeight = KSurfaceSizes[aSizeIndex].iHeight;
        }
    else
        {
        iActualSize.iWidth = KSurfaceParams[aIndex].iXSize;
        iActualSize.iHeight = KSurfaceParams[aIndex].iYSize;
        }
    for(TInt i = 0; attribs[i] != EGL_NONE; i += 2)
        {
        switch(attribs[i])
            {
            case EGL_HEIGHT:
                attribs[i+1] = iActualSize.iHeight;
                break;
            case EGL_WIDTH:
                attribs[i+1] = iActualSize.iWidth;
                break;
            }
        }
    
    iSurface = eglCreatePbufferSurface(iDisplay, iConfig, attribs);
    if (iSurface == EGL_NO_SURFACE)
        {
        EGLint err = eglGetError();
        User::Leave(KErrNotSupported);
        }
    }


TSurfaceId CEglPBufferSurface::SurfaceId() const
    {
    SURF_ASSERT(0);  // We shouldn't call this!
    return TSurfaceId::CreateNullId();
    }

TInt CEglPBufferSurface::SizeInBytes() const
    {
    // size of a pixel in bits. 
    EGLint size;
    if (!eglGetConfigAttrib(iDisplay, iConfig, EGL_BUFFER_SIZE, &size))
        {
        RDebug::Printf("Unable to get EGL_BUFFER_SIZE from config %d, err = %04x", iConfig, eglGetError());
        return 0;
        }
    
    return (KSurfaceParams[iParamIndex].iXSize * KSurfaceParams[iParamIndex].iYSize * size) / 8;
    }


// Factory function.
CSurface *CSurface::SurfaceFactoryL(TSurfaceType aSurfaceType)
    {
    switch(aSurfaceType)
        {
        case ESurfTypeRaw:
            return CRawSurface::NewL();
            
        case ESurfTypeEglWindow:
            return CEglWindowSurface::NewL();
            
        case ESurfTypeRawSingleBuffered:
            return CRawSingleBufferSurface::NewL();
            
        case ESurfTypePBuffer:
            return CEglPBufferSurface::NewL();
            
        default:
            SURF_ASSERT(0);
            return NULL;
        }
    }
