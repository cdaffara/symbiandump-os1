// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and/or associated documentation files (the
// "Materials"), to deal in the Materials without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Materials, and to
// permit persons to whom the Materials are furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Materials.
//
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
// 
// Description:
// Display Adaptation Interface Implementation for Display channel
// 
//

#include "openwfc_ri_displaychannel.h"
#include <e32debug.h>
#include <hal.h>

_LIT(KDisplayChannelLdd, "display0.ldd");
const TUint32 KDefaultScreenNumber = 0;

#ifdef _DEBUG
void Panic(TInt aPanic)
    {
    _LIT(KPanic, "DA-DC");
    User::Panic(KPanic, aPanic);
    }
#endif

EXPORT_C COpenWFC_RI_Display* COpenWFC_RI_Display::NewL(TUint aScreen)
    {
    return COpenWFC_RI_DisplayChannel::NewL(aScreen);
    }

EXPORT_C TUint32 COpenWFC_RI_Display::DefaultScreenNumber()
    {
    return KDefaultScreenNumber;
    }
    
COpenWFC_RI_DisplayChannel::COpenWFC_RI_DisplayChannel(TUint aScreen):
iScreenNumber(aScreen),
iCurrentSceneStream(SYMBIAN_INVALID_HANDLE)
    {
    }

COpenWFC_RI_DisplayChannel* COpenWFC_RI_DisplayChannel::NewL(TUint aScreen)
    {
    COpenWFC_RI_DisplayChannel* screenContext = new(ELeave) COpenWFC_RI_DisplayChannel(aScreen);
    CleanupStack::PushL(screenContext);
    screenContext->ConstructL();
    CleanupStack::Pop(screenContext);
    return screenContext;
    }

COpenWFC_RI_DisplayChannel::~COpenWFC_RI_DisplayChannel()
    {
    TInt count = iCompositionBuffer.Count();
    for(TInt ii = 0; ii < count; ++ii)
        {
        TBufferAddress* addr = iCompositionBuffer[ii];
        if (addr)
            {
            addr->iChunk.Close();
            delete addr;
            }
        }
    iCompositionBuffer.Close();
    // We do not need to use this boolean
    TBool unused;
    // Reset the initial rotation for next time.
    iDispChan.SetRotation(iDefaultRotation, unused);
    iDispChan.Close();
    }

void COpenWFC_RI_DisplayChannel::ConstructL()
    {
    TInt err = User::LoadLogicalDevice(KDisplayChannelLdd);
    if (err != KErrNone && err != KErrAlreadyExists)
        {
        User::Leave(err);
        }
    
    User::LeaveIfError(iDispChan.Open(iScreenNumber));
//#ifdef __WINS__
    iScreenInfo.iDefaultRotation = EScreenRotate0;
    iDefaultRotation = RDisplayChannel::ERotationNormal;
    iRotationOffset = 0;
//#else
//    iScreenInfo.iDefaultRotation = EScreenRotate270;
//    iDefaultRotation = RDisplayChannel::ERotation270CW;
//    iRotationOffset = 3;
//#endif
    RDisplayChannel::TDisplayRotation rotation = iDispChan.CurrentRotation();
    switch (rotation)
        {
        case RDisplayChannel::ERotationNormal:
            iScreenInfo.iCurrentRotation = EScreenRotate0;
            break;
        case RDisplayChannel::ERotation90CW:
            iScreenInfo.iCurrentRotation = EScreenRotate90;
            break;
        case RDisplayChannel::ERotation180:
            iScreenInfo.iCurrentRotation = EScreenRotate180;
            break;
        case RDisplayChannel::ERotation270CW:
            iScreenInfo.iCurrentRotation = EScreenRotate270;
            break;
        default:
            RDebug::Printf("COpenWFC_RI_DisplayChannel::ConstructL() unsupported rotation %d", rotation);
            User::Leave(KErrNotSupported);
        }
    
    iNewRotation = iScreenInfo.iCurrentRotation;
    TPckgBuf<RDisplayChannel::TDisplayInfo> pkgInfo;
    iDispChan.GetDisplayInfo(pkgInfo);
    RDisplayChannel::TDisplayInfo info = pkgInfo();
    /* Note that in theory other formats could be supported for composition 
     * (eg  EUidPixelFormatARGB_8888 | EUidPixelFormatARGB_8888_PRE )
     * So long as the background colour is opaque then the final composed image should end up opaque,
     * so compatible with alpha modes.
     * However, it would give problems for fastpathing, where alpha byte should be ignored,
     * probably fastpath would need to be disabled.
     * At present we only care about PlatSim, 
     * and it is unlikely that any platform will NOT support xrgb32,
     * so it is better to assert the capabilities match than think about this hypothetical case too hard.  
     */
    if (info.iPixelFormat != EUidPixelFormatXRGB_8888)
        {
        RDebug::Printf("COpenWFC_RI_DisplayChannel::ConstructL() unsupported pixel format 0x%08.8x", info.iPixelFormat);
        User::Leave(KErrNotSupported);
        }

    iScreenInfo.iPixelFormat = EUidPixelFormatXRGB_8888;
    iScreenInfo.iSupportedRotations = info.iAvailableRotations;
    
    TInt bytesPerPixel = 4;
    if((info.iBitsPerPixel != 24) && (info.iBitsPerPixel != 32))
        {
        RDebug::Printf("COpenWFC_RI_DisplayChannel::ConstructL()unexpected bpp value %d", info.iBitsPerPixel);
        }
    
    iScreenInfo.iBytesPerPixel = bytesPerPixel;
    
    if (iRotationOffset & 1)
        {
        // Either 90 or 270 degree rotation, so reverse the normal and flipped
        // sizes (which relate to zero degrees rotation).
        iScreenInfo.iNormalWidth = info.iFlipped.iWidth;
        iScreenInfo.iNormalHeight = info.iFlipped.iHeight;
        iScreenInfo.iNormalStride = info.iFlipped.iOffsetBetweenLines;
        iScreenInfo.iFlippedWidth = info.iNormal.iWidth;
        iScreenInfo.iFlippedHeight = info.iNormal.iHeight;
        iScreenInfo.iFlippedStride = info.iNormal.iOffsetBetweenLines;
        }
    else
        {
        iScreenInfo.iNormalWidth = info.iNormal.iWidth;
        iScreenInfo.iNormalHeight = info.iNormal.iHeight;
        iScreenInfo.iNormalStride = info.iNormal.iOffsetBetweenLines;
        iScreenInfo.iFlippedWidth = info.iFlipped.iWidth;
        iScreenInfo.iFlippedHeight = info.iFlipped.iHeight;
        iScreenInfo.iFlippedStride = info.iFlipped.iOffsetBetweenLines;
        }
        
    if (iScreenInfo.iNormalStride < 0)
        {
        iScreenInfo.iNormalStride = bytesPerPixel * iScreenInfo.iNormalWidth;
        }
    if (iScreenInfo.iFlippedStride < 0)
        {
        iScreenInfo.iFlippedStride = bytesPerPixel * iScreenInfo.iFlippedWidth;
        }
    
    for(TInt i = 0; i < info.iNumCompositionBuffers; ++i)
        {
        iCompositionBuffer.AppendL(NULL);
        TBufferAddress* address = new(ELeave) TBufferAddress;
        iCompositionBuffer[i] = address;
        User::LeaveIfError(iDispChan.GetCompositionBufferInfo(i, address->iChunk, address->iOffset));
        address->iAddress = address->iChunk.Base() + address->iOffset;
        }
    }

TInt COpenWFC_RI_Display::GetAttributeSize(TUint aAttributeId)
    {
    switch (aAttributeId)
        {
        case EScreenAttributeImplementationVersion:
        case EScreenAttributePixelFormat:
        case EScreenAttributeBytesPerPixel:
        case EScreenAttributeSupportedRotation:
        case EScreenAttributeNormalWidth:
        case EScreenAttributeNormalHeight:
        case EScreenAttributeNormalStride:
        case EScreenAttributeFlippedWidth:
        case EScreenAttributeFlippedHeight:
        case EScreenAttributeFlippedStride:
        case EScreenAttributeDefaultRotation:
        case EScreenAttributeCurrentRotation:
            return sizeof(TUint32);
         
        case EScreenAttributeScreenGeometry:
            return sizeof(TScreenGeometryAttribute);
            
        default:
            return 0;
        }
    }

TInt COpenWFC_RI_DisplayChannel::GetAttribute(TInt aAttributeId, TAny* aAttribute, TInt aAttributeSize)
    {
    TInt parameterSize = GetAttributeSize(aAttributeId);

    if (aAttribute == NULL)
        {
        return KErrArgument;
        }
    
    if (parameterSize == 0) 
        {
        return KErrNotSupported;
        }
    
    if (aAttributeSize != parameterSize)
        {
        return KErrArgument;
        }
    
    switch (aAttributeId)
        {
        case EScreenAttributeImplementationVersion:
            *((TUint32*)aAttribute) = KImplementationVersion;
            break;
        case EScreenAttributePixelFormat:
            *((TUint32*)aAttribute) = iScreenInfo.iPixelFormat;
            break;            
        case EScreenAttributeBytesPerPixel:
            *((TUint32*)aAttribute) = iScreenInfo.iBytesPerPixel;
            break;
        case EScreenAttributeSupportedRotation:
            *((TUint32*)aAttribute) = iScreenInfo.iSupportedRotations;
            break;
        case EScreenAttributeNormalWidth:
            *((TUint32*)aAttribute) = iScreenInfo.iNormalWidth;
            break;
        case EScreenAttributeNormalHeight:
            *((TUint32*)aAttribute) = iScreenInfo.iNormalHeight;
            break;
        case EScreenAttributeNormalStride:
            *((TUint32*)aAttribute) = iScreenInfo.iNormalStride;
            break;
        case EScreenAttributeFlippedWidth:
            *((TUint32*)aAttribute) = iScreenInfo.iFlippedWidth;
            break;
        case EScreenAttributeFlippedHeight:
            *((TUint32*)aAttribute) = iScreenInfo.iFlippedHeight;
            break;
        case EScreenAttributeFlippedStride:
            *((TUint32*)aAttribute) = iScreenInfo.iFlippedStride;
            break;
        case EScreenAttributeDefaultRotation:
            *((TUint32*)aAttribute) = iScreenInfo.iDefaultRotation;
            break;
        case EScreenAttributeCurrentRotation:
            *((TUint32*)aAttribute) = iScreenInfo.iCurrentRotation;
            break;
        case EScreenAttributeScreenGeometry:
            *((TScreenGeometryAttribute*)aAttribute) = iScreenInfo;
            break;
        default:
            return KErrNotSupported;
        }
    
    return KErrNone;
    }


TInt COpenWFC_RI_DisplayChannel::SetAttribute(TInt aAttributeId, TAny* aAttribute, TInt aAttributeSize)
    {
    // the only parameter we can modify is the current rotation
    TInt parameterSize = GetAttributeSize(aAttributeId);

    if (aAttributeSize != parameterSize || aAttribute == NULL)
        {
        return KErrArgument;
        }
    
    switch (aAttributeId)
        {
        case EScreenAttributeCurrentRotation:
            iNewRotation = static_cast<TScreenRotation>(*((TUint32*)aAttribute));
            break;
        default:
            return KErrNotSupported;
        }
    return KErrNone;
    }


TInt COpenWFC_RI_DisplayChannel::CommitAttribute()
    {
    // For DisplayChannel the new screen attributes must be applied
    // at the same time with the composition.
    if (iScreenInfo.iCurrentRotation != iNewRotation)
        {
        TInt totalRotation;
        RDisplayChannel::TDisplayRotation dcRotation;

        // Change the rotation to a numeric index from zero
        switch (iNewRotation)
            {
            case EScreenRotate90:
                totalRotation = 1;
                break;
            case EScreenRotate180:
                totalRotation = 2;
                break;
            case EScreenRotate270:
                totalRotation = 3;
                break;
            case EScreenRotate0:
            default:
                __ASSERT_DEBUG(iNewRotation == EScreenRotate0, Panic(__LINE__));
                totalRotation = 0;
                break;
            }

        // Add the initial rotation of the device and wrap to get final one.
        totalRotation = (totalRotation + iRotationOffset) % 4;

        // Now change the final rotation to the display channel rotation
        switch (totalRotation)
            {
            case 1:
                dcRotation = RDisplayChannel::ERotation90CW;
                break;
            case 2:
                dcRotation = RDisplayChannel::ERotation180;
                break;
            case 3:
                dcRotation = RDisplayChannel::ERotation270CW;
                break;
            case 0:
            default:
                __ASSERT_DEBUG(totalRotation == 0, Panic(__LINE__));
                dcRotation = RDisplayChannel::ERotationNormal;
                break;
            }

        if (!(iScreenInfo.iSupportedRotations & dcRotation))
            {
            return KErrNotSupported;
            }
        
        // We do not need to use this boolean
        TBool displayConfigChanged;
        TInt err = iDispChan.SetRotation(dcRotation, displayConfigChanged);
        __ASSERT_DEBUG(err == KErrNone, Panic(__LINE__));
        if (err == KErrNone)
            {
            iScreenInfo.iCurrentRotation = iNewRotation;
            }
        else
            {
            return err;
            }
        }
    return KErrNone;
    }

TInt COpenWFC_RI_DisplayChannel::SetLayerSurface(TInt aLayer, SymbianStreamType aStream,TInt* aNonTrivialAttribs)
    {
    // display channel interface can be seen as mono-layered
    if (aLayer != 0)
        {
        return KErrArgument;
        }
    if (aNonTrivialAttribs && *aNonTrivialAttribs)
        {
        return KErrNotSupported;
        }
    TBool acceptable=ETrue;
    khronos_int32_t format,pixSize;
    SymbianStreamGetHeader(aStream,NULL,NULL,NULL,&format,&pixSize);
    if (format!=EUidPixelFormatARGB_8888 && format!=EUidPixelFormatARGB_8888_PRE && format!=EUidPixelFormatXRGB_8888)
        {
        acceptable=EFalse;
        }
            
    if (!acceptable)
        {
        return KErrNotSupported;
        }
    
    iCurrentSceneStream = aStream;
    return KErrNone;
    }

TInt COpenWFC_RI_DisplayChannel::SetTopLayerSurface(SymbianStreamType aStream,TInt* aNonTrivialAttribs)
    {
    return SetLayerSurface(KTopMostLayer, aStream,aNonTrivialAttribs);
    }

TInt COpenWFC_RI_DisplayChannel::UpdateDisplay()
    {
    // The solution is suboptimal because a blit is used insteat of posting a user buffer
    // The solution is more simpler and because the emulator is to be phased-out soon,
    // there is no much incentive in complicating the code
    TInt err;
    
    TUint bufferIndex;
    TRequestStatus status;
    iDispChan.GetCompositionBuffer(bufferIndex, status);
    User::WaitForRequest(status);
    
    err = status.Int();
    if (err != KErrNone || bufferIndex >= iCompositionBuffer.Count()) 
        {
        return KErrGeneral;
        }
    
    TUint8* firstPixelAddr = iCompositionBuffer[bufferIndex]->iAddress;
    
    // Only 32 or 16bpp supported and assume stride == width * pixel size
    TInt size = 0;
    if (iNewRotation == EScreenRotate0 || iNewRotation == EScreenRotate180)
        {
        size = iScreenInfo.iNormalHeight * iScreenInfo.iNormalStride; 
        }
    else
        {
        size = iScreenInfo.iFlippedHeight * iScreenInfo.iFlippedStride; 
        }
    SymbianStreamBuffer frontBuffer;
    SymbianStreamAcquireReadBuffer(iCurrentSceneStream, &frontBuffer);
    void* pixelDataPtr = NULL;
    SymbianStreamGetBufferPointer(iCurrentSceneStream, frontBuffer,&pixelDataPtr);
    Mem::Move(firstPixelAddr, pixelDataPtr, size);
    RDisplayChannel::TPostCount postCount;
    err = iDispChan.PostCompositionBuffer(NULL, postCount);
    if (err == KErrNone)
        {
        // This will throttle composition more than necessary,
        // since it will wait until buffer is being displayed
        iDispChan.WaitForPost(postCount, status);
        User::WaitForRequest(status);
        err = status.Int();
        }
    
    SymbianStreamReleaseReadBuffer(iCurrentSceneStream, frontBuffer);

    return err;
    }

