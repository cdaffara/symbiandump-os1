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
// Display Channel adaptation for owfscreen
// 
//

#include <WF/openwfc_ri_display.h>
#include <hal.h>
#include "owfscreen.h"
#include "owfdebug.h"
#include "owfdisplaycontext.h"

#include <assert.h>

static OWFboolean OWF_Number_Of_Screens(OWFint *pNumberOfScreens)
    {
    TInt screens = 0;
    TInt err = HAL::Get(HAL::EDisplayNumberOfScreens, screens);   
    if (err != KErrNone)
        {
        DPRINT(("Cannot get number of screens from HAL, error: %d", err));
        return OWF_FALSE;
        }
    *pNumberOfScreens = (OWFint)screens;
    return OWF_TRUE;
    }

OWF_ROTATION ConvertRotation(TUint32 rotation)
    {
    switch(rotation)
        {
            case COpenWFC_RI_Display::EScreenRotate0:
                return OWF_ROTATION_0;
            case COpenWFC_RI_Display::EScreenRotate90:
                return OWF_ROTATION_90;
            case COpenWFC_RI_Display::EScreenRotate180:
                return OWF_ROTATION_180;
            default:
                OWF_ASSERT(rotation == COpenWFC_RI_Display::EScreenRotate270);
                return OWF_ROTATION_270;
        }
    }

OWF_API_CALL OWFint OWF_Screen_GetDefaultNumber()
    {
    return COpenWFC_RI_Display::DefaultScreenNumber();
    }

OWF_API_CALL OWFboolean OWF_Screen_Valid(OWFint screen)
    {
    OWFint numberOfScreens = 0;
    if (!OWF_Number_Of_Screens(&numberOfScreens))
        {
        return OWF_FALSE;
        }
    if (screen < 0 || screen >= numberOfScreens)
        {
        return OWF_FALSE;
        }
    return OWF_TRUE;
    }


OWF_API_CALL OWFboolean OWF_Screen_GetHeader(OWF_DISPCTX dc, OWF_SCREEN* header)
    {
    OWF_ASSERT(dc && header);
    
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    
    OWF_ASSERT(pDispCtx->iScreenContext);
    
    COpenWFC_RI_Display::TScreenGeometryAttribute info;
    if (KErrNone != pDispCtx->iScreenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeScreenGeometry,
                                           &info,
                                           sizeof(info)))
        {
        return OWF_FALSE;
        }
    
    header->currentRotation = ConvertRotation(info.iCurrentRotation);
    header->initialRotation = ConvertRotation(info.iDefaultRotation);
    if (info.iPixelFormat != EUidPixelFormatXRGB_8888)
        {
        return OWF_FALSE;
        }
    header->pixelFormat = OWF_IMAGE_XRGB8888;
    header->normal.height = info.iNormalHeight;
    header->normal.width = info.iNormalWidth;
    header->normal.stride = info.iNormalStride;
    header->flipped.height = info.iFlippedHeight;
    header->flipped.width = info.iFlippedWidth;
    header->flipped.stride = info.iFlippedStride;
    return OWF_TRUE;
    }

OWF_API_CALL OWFboolean OWF_Screen_Rotation_Supported(OWF_DISPCTX dc)
    {
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    if (!dc || !pDispCtx->iScreenContext)
        {
        return OWF_FALSE;
        }
    
    TUint32 supportedRotations;
    pDispCtx->iScreenContext->GetAttribute(COpenWFC_RI_Display::EScreenAttributeSupportedRotation,
                                           &supportedRotations,
                                           sizeof(supportedRotations));
    
    return ((supportedRotations & 0xF) == 0xF) ? OWF_TRUE : OWF_FALSE;
    }

OWF_API_CALL OWFboolean OWF_Screen_Post_Topmost_Layer(OWF_DISPCTX dc, 
                                                      OWFNativeStreamType stream, 
                                                      OWF_ROTATION rotation)
    {
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    OWF_ASSERT(dc && stream && pDispCtx->iScreenContext);
    COpenWFC_RI_Display* screenContext= pDispCtx->iScreenContext;

    // If the display driver cannot handle rotation itself, the buffer will
    // already have been rotated. 
    TInt setTopErr=screenContext->SetTopLayerSurface((SymbianStreamType)stream);
    if (setTopErr)
        {
        return OWF_FALSE;
        }

    if (OWF_Screen_Rotation_Supported(dc))
        {
        TUint32 screenRotation;
        switch(rotation)
            {
                case OWF_ROTATION_0:
                    screenRotation = COpenWFC_RI_Display::EScreenRotate0;
                    break;
                case OWF_ROTATION_90:
                    screenRotation = COpenWFC_RI_Display::EScreenRotate90;
                    break;
                case OWF_ROTATION_180:
                    screenRotation = COpenWFC_RI_Display::EScreenRotate180;
                    break;
                default:
                    OWF_ASSERT(rotation == OWF_ROTATION_270);
                    screenRotation = COpenWFC_RI_Display::EScreenRotate270;
            }
        
        
        screenContext->SetAttribute(COpenWFC_RI_Display::EScreenAttributeCurrentRotation, 
                                    &screenRotation,
                                    sizeof(screenRotation));
        screenContext->CommitAttribute();
        }
    screenContext->UpdateDisplay();
    return OWF_TRUE;
    }

OWF_API_CALL OWFboolean OWF_Screen_Create(OWFint screen, OWF_DISPCTX dc)
    {
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    OWF_ASSERT(dc);
    
    TRAPD(err, pDispCtx->iScreenContext = COpenWFC_RI_Display::NewL(screen))
    
    return (err == KErrNone) ? OWF_TRUE : OWF_FALSE;
    }

OWF_API_CALL void OWF_Screen_Destroy(OWF_DISPCTX dc)
    {
    OWFDisplayContext *pDispCtx = _OWF_DISPLAYCONTEXT(dc);
    OWF_ASSERT(dc && pDispCtx->iScreenContext);
    delete pDispCtx->iScreenContext;
    pDispCtx->iScreenContext = NULL;
    }
