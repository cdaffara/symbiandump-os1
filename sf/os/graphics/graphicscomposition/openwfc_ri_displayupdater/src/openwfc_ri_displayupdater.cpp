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
// Display Adaptation Interface Implementation for Display Updater
// 
//

#include "openwfc_ri_displayupdater.h"
#include <e32debug.h>
#include <hal.h>

#if defined(ENABLE_LOGGING)
#define LOG(X)  RDebug::Printf X
#else
#define LOG(X)
#endif

// #define LOG_DISPLAY_PROPERTIES

const TUint32 KDefaultScreenNumber = 0;

#ifdef _DEBUG
void Panic(TInt aPanic)
    {
    _LIT(KPanic, "DA-DU");
    User::Panic(KPanic, aPanic);
    }
#endif

EXPORT_C COpenWFC_RI_Display* COpenWFC_RI_Display::NewL(TUint aScreen)
    {
    return COpenWFC_RI_DisplayUpdater::NewL(aScreen);
    }

EXPORT_C TUint32 COpenWFC_RI_Display::DefaultScreenNumber()
    {
    return KDefaultScreenNumber;
    }
    
COpenWFC_RI_DisplayUpdater::COpenWFC_RI_DisplayUpdater(TUint aScreen):
iScreenNumber(aScreen),
iDisplayUpdater(NULL),
iCurrentSceneStream(SYMBIAN_INVALID_HANDLE),
iCurrentReadBuffer(SYMBIAN_INVALID_HANDLE),
iNextSceneStream(SYMBIAN_INVALID_HANDLE),
iNextReadBuffer(SYMBIAN_INVALID_HANDLE)
    {
    }

COpenWFC_RI_DisplayUpdater* COpenWFC_RI_DisplayUpdater::NewL(TUint aScreen)
    {
    LOG((" +++ COpenWFC_RI_DisplayUpdater::NewL(%d)", aScreen));
    COpenWFC_RI_DisplayUpdater* screenContext = new(ELeave) COpenWFC_RI_DisplayUpdater(aScreen);
    CleanupStack::PushL(screenContext);
    screenContext->ConstructL();
    CleanupStack::Pop(screenContext);
    LOG((" --- COpenWFC_RI_DisplayUpdater::NewL(%d) : 0x%X", aScreen, screenContext));
    return screenContext;
    }

COpenWFC_RI_DisplayUpdater::~COpenWFC_RI_DisplayUpdater()
    {
    LOG((" +++ COpenWFC_RI_DisplayUpdater::~COpenWFC_RI_DisplayUpdater(%d)", iScreenNumber));
    if (iDisplayUpdater)
        {
        TDisplayViewSettings viewSettings;  // DEFAULT
        TBufferInfo bufferInfo; // DEFAULT
        TInt err = KErrNone;

        // Set to default view to release layer
        err = iDisplayUpdater->SetView(KTopMostLayer, viewSettings, bufferInfo);
        if (err != KErrNone)
            {
            LOG((" !!! COpenWFC_RI_DisplayUpdater::~COpenWFC_RI_DisplayUpdater()ERROR: Display update set view failed %d", err));
            }

        TRequestStatus completedWhenReady = KRequestPending;
        LOG(("\n  +++ COpenWFC_RI_DisplayUpdater::~COpenWFC_RI_DisplayUpdater() FLUSHING *******\n"));
        iDisplayUpdater->Flush(completedWhenReady);
        LOG(("\n  +++ COpenWFC_RI_DisplayUpdater::~COpenWFC_RI_DisplayUpdater() AFTER FLUSHING ******* 2\n"));
        User::WaitForRequest(completedWhenReady);
        LOG(("\n  +++ COpenWFC_RI_DisplayUpdater::~COpenWFC_RI_DisplayUpdater() FLUSHING:REQUEST COMPLETED %d*******\n", completedWhenReady.Int()));

        if (iCurrentSceneStream != SYMBIAN_INVALID_HANDLE && iCurrentReadBuffer != SYMBIAN_INVALID_HANDLE)
            {
            SymbianStreamReleaseReadBuffer(iCurrentSceneStream, iCurrentReadBuffer);
            }
        if (iNextSceneStream != SYMBIAN_INVALID_HANDLE && iNextReadBuffer != SYMBIAN_INVALID_HANDLE)
            {
            SymbianStreamReleaseReadBuffer(iNextSceneStream, iNextReadBuffer);
            }
        }
    delete iDisplayUpdater;
    iSurfaceManager.Close();
    LOG((" --- COpenWFC_RI_DisplayUpdater::~COpenWFC_RI_DisplayUpdater() completed"));    
    }

void COpenWFC_RI_DisplayUpdater::ConstructL()
    {
    LOG((" +++ COpenWFC_RI_DisplayUpdater::ConstructL"));
    User::LeaveIfError(iSurfaceManager.Open());
    iDisplayUpdater = CDisplayUpdater::NewL(iScreenNumber);
    iScreenInfo.iDefaultRotation = EScreenRotate0;
    iDefaultRotation = ERotate0Deg;
    iCurrentRotation = ERotate0Deg;
    iRotationOffset = 0;
    iScreenInfo.iCurrentRotation = EScreenRotate0;
    iNewRotation = iScreenInfo.iCurrentRotation;
    
    TDisplayDrvInfo displayInfo;
    iDisplayUpdater->GetDisplayProperties(displayInfo);

#if defined(LOG_DISPLAY_PROPERTIES)
    LOG((" ??? Display resolution (%d,%d)", displayInfo.iPanelResolution.iWidth, displayInfo.iPanelResolution.iHeight));
    LOG((" ??? Display twips (%d,%d)", displayInfo.iPanelTwips.iWidth, displayInfo.iPanelTwips.iHeight));
    LOG((" ??? Display refresh rate %dHz", displayInfo.iPanelRefreshRateHz));
    LOG((" ??? Display %d internal composition buffers", (TInt)displayInfo.iNumberOfBuffers));
    LOG((" ??? Display pixel formats: 0x%08.8X", (TInt)displayInfo.iSupportedFormats));
    LOG((" ??? Display pixel aspect ratio %d/%d", (TInt)displayInfo.iPixelAspectRNumerator, (TInt)displayInfo.iPixelAspectRDenominator));
    LOG((" ??? Display supported per-layer rotations: 0x%08.8X", (TInt)displayInfo.iSupportedPerLayerRotations));
    LOG((" ??? Display rotation works with: 0x%08.8X", (TInt)displayInfo.iRotationWorksWithTheseFeatures));
    LOG((" ??? Display scaling works with: 0x%08.8X", (TInt)displayInfo.iScalingWorksWithTheseFeatures));
    LOG((" ??? Display minimum scaling: 100/%d", (TInt)displayInfo.iScalingMinDenominator));
    LOG((" ??? Display maximum scaling: %d/100", (TInt)displayInfo.iScalingMaxNumerator));
    LOG((" ??? Display maximum overlays: %d", (TInt)displayInfo.iNumberOfOverlays));
    LOG((" ??? Display colour space: 0x%08.8X", (TInt)displayInfo.iDestinationColorSpace));
    LOG((" ??? Display colour coords: (%d,%d)R (%d,%d)G (%d,%d)B (%d,%d)W",
            displayInfo.iDestinationColorCoordinates.iRed.iX, displayInfo.iDestinationColorCoordinates.iRed.iY,
            displayInfo.iDestinationColorCoordinates.iGreen.iX, displayInfo.iDestinationColorCoordinates.iGreen.iY,
            displayInfo.iDestinationColorCoordinates.iBlue.iX, displayInfo.iDestinationColorCoordinates.iBlue.iY,
            displayInfo.iDestinationColorCoordinates.iWhite.iX, displayInfo.iDestinationColorCoordinates.iWhite.iY));
    LOG((" ??? Display direct buffer display: %d", (TInt)displayInfo.iGivenBufferWorksAsFrameBuffer));
    LOG((" ??? Display variable resolution: %d", (TInt)displayInfo.iVariableResolution));
    LOG((" ??? Display draw overscan: %d", (TInt)displayInfo.iOverscanToBeDrawn));
    LOG((" ??? Display is on: %d", (TInt)displayInfo.iDisplayOn));
#endif

    TInt supportedFormats = displayInfo.iSupportedFormats;
    /* Note that in theory other formats could be supported for composition 
     * (eg  EPixelFormatARgb8888Le | EPixelFormatARgb8888LeP )
     * So long as the background colour is opaque then the final compiosed image should end up opaque,
     * so compatible with alpha modes.
     * However, it would give problems for fastpathing, where alpha byte should be ignored,
     * probably fastpath would need to be disabled.
     * At present we only care about PlatSim, 
     * and it is unlikely that any platform will NOT support xrgb32,
     * so it is better to assert the capabilities match than think about this hypothetical case too hard.  
     */
    if (supportedFormats & (EPixelFormatXRgb8888Le ))
        {
        iScreenInfo.iPixelFormat = EUidPixelFormatXRGB_8888;
        iScreenInfo.iBytesPerPixel = 4;
        }
    else
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::ConstructL() unsupported pixel formats 0x%08.8x", supportedFormats));
        User::Leave(KErrNotSupported);
        }

    iScreenInfo.iPixelFormat = EUidPixelFormatXRGB_8888;
    if (displayInfo.iRotationWorksWithTheseFeatures & EFeatureExternalBuf)
        {
        iScreenInfo.iSupportedRotations = displayInfo.iSupportedPerLayerRotations & 0x0f;
        }
    else
        {
        LOG((" +++ COpenWFC_RI_DisplayUpdater::ConstructL() fixed output rotation"));
        iScreenInfo.iSupportedRotations = (TInt32)ERotate0Deg;
        }
    
    iScreenInfo.iNormalWidth = displayInfo.iPanelResolution.iWidth;
    iScreenInfo.iNormalHeight = displayInfo.iPanelResolution.iHeight;
    iScreenInfo.iFlippedWidth = iScreenInfo.iNormalHeight;
    iScreenInfo.iFlippedHeight = iScreenInfo.iNormalWidth;
        
    iScreenInfo.iNormalStride = iScreenInfo.iBytesPerPixel * iScreenInfo.iNormalWidth;
    iScreenInfo.iFlippedStride = iScreenInfo.iBytesPerPixel * iScreenInfo.iFlippedWidth;
    LOG((" --- COpenWFC_RI_DisplayUpdater::ConstructL"));
    }

TInt COpenWFC_RI_Display::GetAttributeSize(TUint aAttributeId)
    {
    LOG((" +++ COpenWFC_RI_DisplayUpdater::GetAttributeSize(%d)", aAttributeId));
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
            LOG((" --- COpenWFC_RI_DisplayUpdater::GetAttributeSize(%d) = 4", aAttributeId));
            return sizeof(TUint32);
         
        case EScreenAttributeScreenGeometry:
            LOG((" --- COpenWFC_RI_DisplayUpdater::GetAttributeSize(%d) = %d", aAttributeId, sizeof(TScreenGeometryAttribute)));
            return sizeof(TScreenGeometryAttribute);
            
        default:
            LOG((" !!! COpenWFC_RI_DisplayUpdater::GetAttributeSize(%d) UNKNOWN", aAttributeId));
            return 0;
        }
    }

TInt COpenWFC_RI_DisplayUpdater::GetAttribute(TInt aAttributeId, TAny* aAttribute, TInt aAttributeSize)
    {
    LOG((" +++ COpenWFC_RI_DisplayUpdater::GetAttribute(%d, 0x%X, %d)", aAttributeId, aAttribute, aAttributeSize));
    TInt parameterSize = GetAttributeSize(aAttributeId);

    if (aAttribute == NULL)
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::GetAttribute() aAttribute NULL ptr"));
        return KErrArgument;
        }
    
    if (parameterSize == 0)
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::GetAttribute() Not Supported parameter aAttributeId"));
        return KErrNotSupported;
        }
    
    if (aAttributeSize != parameterSize)
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::GetAttribute() aAttributeSize wrong size"));
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
            LOG((" !!! COpenWFC_RI_DisplayUpdater::GetAttribute() attribute unrecognised"));
            return KErrNotSupported;
        }
    
    LOG((" --- COpenWFC_RI_DisplayUpdater::GetAttribute() = %d", *((TUint32*)aAttribute)));
    return KErrNone;
    }


TInt COpenWFC_RI_DisplayUpdater::SetAttribute(TInt aAttributeId, TAny* aAttribute, TInt aAttributeSize)
    {
    LOG((" +++ COpenWFC_RI_DisplayUpdater::SetAttribute(%d, 0x%X, %d)", aAttributeId, aAttribute, aAttributeSize));
    // the only parameter we can modify is the current rotation
    TInt parameterSize = GetAttributeSize(aAttributeId);

    if (aAttributeSize != parameterSize || aAttribute == NULL)
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::SetAttribute() Invalid parameter"));
        return KErrArgument;
        }
    
    switch (aAttributeId)
        {
        case EScreenAttributeCurrentRotation:
            iNewRotation = static_cast<TScreenRotation>(*((TUint32*)aAttribute));
            LOG((" +++ COpenWFC_RI_DisplayUpdater::SetAttribute(CurrentRotation, %d)", (TInt)iNewRotation));
            break;
        default:
            LOG((" !!! COpenWFC_RI_DisplayUpdater::SetAttribute() Invalid attribute"));
            return KErrNotSupported;
        }
    LOG((" --- COpenWFC_RI_DisplayUpdater::SetAttribute()"));
    return KErrNone;
    }


TInt COpenWFC_RI_DisplayUpdater::CommitAttribute()
    {
    LOG((" +++ COpenWFC_RI_DisplayUpdater::CommitAttribute"));
    // Hardware rotation is not demonstrated on PlatSim
    __ASSERT_DEBUG(iScreenInfo.iCurrentRotation == iNewRotation, Panic(__LINE__));
    return KErrNone;
    }

TInt COpenWFC_RI_DisplayUpdater::SetLayerSurface(TInt aLayer, SymbianStreamType aStream, TInt* aNonTrivialAttribs)
    {
    // display channel interface can be seen as mono-layered
    if (aLayer != 0)
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : non-zero layer not supported"));
        return KErrArgument;
        }
    
    if (aNonTrivialAttribs && *aNonTrivialAttribs)
        { 
        LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : Non-trivial attributes not accepted."));
        return KErrNotSupported;
        }
    
    if (aStream!=iNextSceneStream)
        {   //If, in future non-trivial attributes are supported then this condition is inadequate
         SymbianStreamBuffer nextReadBuffer=NULL;
        //Scene stream has changed - re-validate and set up for new info
        TInt err=SymbianStreamAcquireReadBuffer(aStream,&nextReadBuffer);
        if (err<KErrNone)
            {
            LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : Falied to access stream properties"));
            return err;
            }
        const TSurfaceId* surfaceId;
        khronos_int32_t index;
        err= SymbianStreamGetBufferId(aStream,nextReadBuffer,&index,&surfaceId);
        if (err<KErrNone)
            {
            LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : Falied to access stream properties"));
            SymbianStreamReleaseReadBuffer(aStream, nextReadBuffer);
            return err;
            }
        RSurfaceManager::TInfoBuf surfaceInfo;
        iSurfaceManager.SurfaceInfo(*surfaceId,surfaceInfo);
        TBool acceptable=ETrue;
        if (!surfaceInfo().iContiguous)
            {
            acceptable=EFalse;
            LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : Stream 0x%08x iContiguous not accepted ",aStream));
            }
        khronos_int32_t format=surfaceInfo().iPixelFormat;
        if (format!=EUidPixelFormatARGB_8888 && format!=EUidPixelFormatARGB_8888_PRE && format!=EUidPixelFormatXRGB_8888)
            {
            acceptable=EFalse;
            LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : Stream 0x%08x iPixelFormat 0x%08x not accepted ",aStream,surfaceInfo().iPixelFormat));
            }
                
        if (!acceptable)
            {
            LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : Stream 0x%08x properties not accepted ",aStream));
            SymbianStreamReleaseReadBuffer(aStream, nextReadBuffer);
            return KErrNotSupported;
            }
        
        //Note that if we early-return before this point the previous top stream should still be locked for use.
        
        LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : Stream changed 0x%08x --> 0x%08x ",iNextSceneStream,aStream));
        if (iNextSceneStream != SYMBIAN_INVALID_HANDLE && iNextReadBuffer != SYMBIAN_INVALID_HANDLE)
             {
             LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : double-call to SetLayerSurface. First discarded."));
             SymbianStreamReleaseReadBuffer(iNextSceneStream, iNextReadBuffer);
             }
        iNextReadBuffer=nextReadBuffer;
        iNextSceneStream=aStream;
        }
    else
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::SetLayerSurface : Stream still 0x%08x",aStream));
        }
    return KErrNone;
    }

TInt COpenWFC_RI_DisplayUpdater::SetTopLayerSurface(SymbianStreamType aStream, TInt* aNonTrivialAttribs)
    {
    return SetLayerSurface(KTopMostLayer, aStream, aNonTrivialAttribs);
    }

TInt COpenWFC_RI_DisplayUpdater::UpdateDisplay()
    {
    TInt err = KErrNone;

    SymbianStreamBuffer readBuffer = iNextReadBuffer;
    iNextReadBuffer = SYMBIAN_INVALID_HANDLE;
    if (!iNextSceneStream)
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::UpdateDisplay() ASSERT: No top layer has been set!"));
        return KErrNotReady;
        }
    if (readBuffer == SYMBIAN_INVALID_HANDLE)
        {
        if ((err = SymbianStreamAcquireReadBuffer(iNextSceneStream, &readBuffer)) != KErrNone)
            {   //AcquireReadBuffer "Can't fail" in theory, but just incase....
            LOG((" !!! COpenWFC_RI_DisplayUpdater::UpdateDisplay() Acquire failed: %d", err));
            return err;
            }
        }
    TBufferInfo bufferInfo;
    khronos_int32_t width, height, stride;
    SymbianStreamGetHeader(iNextSceneStream, &width, &height, &stride, NULL, NULL);
    bufferInfo.iBufferWidth = (TInt16)width;
    bufferInfo.iBufferHeight = (TInt16)height;
    bufferInfo.iStride = (TInt16)stride;
    bufferInfo.iBufferFormat = EPixelFormatXRgb8888Le;
    bufferInfo.iLinearAddress = 0;
    err = SymbianStreamGetBufferPointer(iNextSceneStream, readBuffer,&bufferInfo.iLinearAddress);
    if (err == KErrNone)
        {
        err = SymbianStreamGetChunkHandle(iNextSceneStream, &bufferInfo.iBufferRChunkHandle);
        }

    if (err != KErrNone)
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::UpdateDisplay() error %d getting stream data", err));
        SymbianStreamReleaseReadBuffer(iNextSceneStream, readBuffer);
        return err;
        }

    TRect displayRect;
    displayRect.SetRect(TPoint(0, 0), TSize(bufferInfo.iBufferWidth, bufferInfo.iBufferHeight ));
    
    TDisplayViewSettings viewSettings; // Default
    viewSettings.iSourceScissor         = displayRect;
    viewSettings.iDestinationScaledRect = displayRect;
    viewSettings.iLayerRotation         = ERotate0Deg;
    viewSettings.iUsageHint             = EPurposeUi;
    viewSettings.iTearingFree           = ETrue;
      
    LOG((" $$$*** bufferInfo.iBufferWidth %d", bufferInfo.iBufferWidth));
    LOG((" $$$*** bufferInfo.iBufferHeight %d", bufferInfo.iBufferHeight));
    LOG((" $$$*** bufferInfo.iStride %d", bufferInfo.iStride));
    LOG((" $$$*** bufferInfo.iBufferFormat %d", bufferInfo.iBufferFormat));
    LOG((" $$$*** bufferInfo.iLinearAddress %08x", bufferInfo.iLinearAddress));
    LOG((" $$$*** bufferInfo.iBufferRChunkHandle %d", bufferInfo.iBufferRChunkHandle));

    LOG((" $$$*** viewSettings iSourceScissor width %d", viewSettings.iSourceScissor.Width()));
    LOG((" $$$*** viewSettings iSourceScissor height %d", viewSettings.iSourceScissor.Height()));
    LOG((" $$$*** viewSettings iDestinationScaledRect width %d", viewSettings.iDestinationScaledRect.Width()));
    LOG((" $$$*** viewSettings iDestinationScaledRect height %d", viewSettings.iDestinationScaledRect.Height()));
    LOG((" $$$*** viewSettings iLayerRotation %d", viewSettings.iLayerRotation));
    LOG((" $$$*** viewSettings iUsageHint %d", viewSettings.iUsageHint));
    LOG((" $$$*** viewSettings iTearingFree %d", viewSettings.iTearingFree));

    TRequestStatus completedWhenReady = KRequestPending;
    viewSettings.iLayerRotation = iCurrentRotation;

    err = iDisplayUpdater->SetView(KTopMostLayer, viewSettings, bufferInfo);
    
    if (err != KErrNone)
        {
        LOG((" !!! COpenWFC_RI_DisplayUpdater::UpdateDisplay()ERROR: Display update set view failed %d", err));
        }
    else
        {
        err = iDisplayUpdater->SetBuffer(KTopMostLayer, displayRect, bufferInfo );
        if (err != KErrNone)
            {
            LOG((" !!! COpenWFC_RI_DisplayUpdater::UpdateDisplay()ERROR: Display Update set buffer failed %d", err));
            }
        }

    ////////////////////////////////////////////////////////////////////////////////////////////
    iDisplayUpdater->Flush(completedWhenReady);
    
    /* The TRequestStatus& aCompletedWhenReady parameter is signalled 
     * either when the buffer has been copied, 
     * or it is streaming the first pixel to the display
     * In either case this indicates that the previous buffer is no longer required.
     * By waiting here we guarantee that the previous iCurrentReadBuffer is no longer required,
     * but here are more intelligent ways to wait, I am sure!
     */
    User::WaitForRequest(completedWhenReady);
    if (iCurrentReadBuffer != SYMBIAN_INVALID_HANDLE)
        {
        // Release the previous update's read buffer.
        /* This ensures that a lock is held on the displayed buffer until the next commit/compose
         * This is correct behaviour for streaming direct to the display, but is actually not
         * necessary if the display makes its own copy.
         * However, PlatSim currently supports only the latter, and we think the former is important,
         * so this implementation allows us to validate that the behaviour is correct in direct streaming.
         */
        SymbianStreamReleaseReadBuffer(iCurrentSceneStream, iCurrentReadBuffer);
        }
    iCurrentReadBuffer = readBuffer;
    iCurrentSceneStream = iNextSceneStream;
    return err;
    }

