/*------------------------------------------------------------------------
 *
 * EGL 1.3
 * -------
 *
 * Copyright (c) 2007 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Materials. 
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *//**
 * \file
 * \brief	Generic OS EGL functionality (not thread safe, no window rendering)
 * \note
  *//*-------------------------------------------------------------------*/

#include "egl.h"
#include "riImage.h"
#include "riDefs.h"

#include <e32std.h>
#include <w32std.h>
#include <eglosnativewindowtype.h>

namespace OpenVGRI
{
//internal fucntions
VGImageFormat MapSymbianDisplayModeToVG(TDisplayMode aMode);
void ReadPixelsToCFbsBitmap(CFbsBitmap& aBitmap, TUint aW, TUint aH);


/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void* OSGetCurrentThreadID(void)
{
	int g_threadid = RThread().Id();
	return (void*)g_threadid;
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

void OSDeinitMutex(void)
{
}

void OSAcquireMutex(void)
{
}

void OSReleaseMutex(void)
{
}

/*-------------------------------------------------------------------*//*!
* \brief	
* \param	
* \return	
* \note		
*//*-------------------------------------------------------------------*/

struct OSWindowContext
{
	TNativeWindowType*  iNativeWindowType;   
};

void* OSCreateWindowContext(EGLNativeWindowType window)
{
    OSWindowContext* ctx = NULL;
    try
    {
        ctx = RI_NEW(OSWindowContext, ());
    }
	catch(std::bad_alloc)
	{
		return NULL;
	}
    ctx->iNativeWindowType = (TNativeWindowType*)window;
    return ctx;
}

void OSDestroyWindowContext(void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        RI_DELETE(ctx);
    }
}

bool OSIsWindow(const void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
        return true;
    }
    return false;
}

void OSGetWindowSize(const void* context, int& width, int& height)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    if(ctx)
    {
    	width=ctx->iNativeWindowType->iSize.iWidth;
		height=ctx->iNativeWindowType->iSize.iHeight; 
    }
    else
    {
        width = 0;
        height = 0;
    }
}


EGLDisplay OSGetDisplay(EGLNativeDisplayType display_id)
{
    RI_UNREF(display_id);
    return (EGLDisplay)1;    //support only a single display
}


void OSBlitToWindow(void* context, const Drawable* drawable)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    //blit if either of iBitmap or iMaskBitmap exist
    if(ctx && ctx->iNativeWindowType->iBitmap)
    {
        TUint w = drawable->getWidth();
        TUint h = drawable->getHeight();
        
        //these should be same as bitmap
        if(ctx->iNativeWindowType->iBitmap)
        	{
        	CFbsBitmap* bitmap = ctx->iNativeWindowType->iBitmap;        	
        	ReadPixelsToCFbsBitmap(*bitmap, w, h);        	
        	}
    }
}

void ReadPixelsToCFbsBitmap(CFbsBitmap& aBitmap, TUint aW, TUint aH)
	{
	TSize sz = aBitmap.SizeInPixels();
	RI_ASSERT(aW==sz.iWidth);
	RI_ASSERT(aH==sz.iHeight);
	
	VGImageFormat f = MapSymbianDisplayModeToVG(aBitmap.DisplayMode());
	
	TInt bitmapDataStride = aBitmap.DataStride();
	
	//copy from source OpenVG model to target(CFbsBitmap) buffer			
	aBitmap.BeginDataAccess();
	//openvg returns pixels in cartesian coordinates (y+) where as CFbsBitmap
	//wants screen coordinates (y-) so we read pixels out starting at top left corner 
	TUint8* ptarget = (TUint8*)aBitmap.DataAddress()+(aH-1)*bitmapDataStride;
	vgReadPixels(ptarget, -bitmapDataStride, f, 0, 0, aW, aH);
	aBitmap.EndDataAccess(EFalse);
	}

VGImageFormat MapSymbianDisplayModeToVG(TDisplayMode aMode)
	{
	VGImageFormat f;
	switch(aMode)
		{
		case EColor16MA:
			f = VG_sARGB_8888;
			break;
		case EColor16MU:
			f = VG_sRGBX_8888;
			break;
		case EColor64K:
			f = VG_sRGB_565;
			break;
		case EGray256:
			f = VG_A_8;
			break;
		case EGray2:
			f = VG_BW_1;
			break;
		default:
			RI_ASSERT(0);
		}
	return f;
	}
EGLBoolean OSGetNativePixmapInfo(NativePixmapType pixmap, int* width, int* height, int* stride, VGImageFormat* format, int** data)
{
	TSize size = ((CFbsBitmap*)pixmap)->SizeInPixels();
    *width  = size.iWidth;
    *height = size.iHeight;
    *stride = ((CFbsBitmap*)pixmap)->DataStride();    
    
    *data = (int*)(((CFbsBitmap*)pixmap)->DataAddress());
    TDisplayMode mode = ((CFbsBitmap*)pixmap)->DisplayMode(); 
	switch(mode)
	{
	case EColor16MA:
		*format = VG_sRGBA_8888;
		break;
	case EColor16MU:
		*format = VG_sRGBX_8888;
		break;
	case EColor64K:
		*format = VG_sRGB_565;
		break;
	case EGray256:
		*format = VG_A_8;
		break;
	case EGray2:
		*format = VG_BW_1;
		break;
	default:
		RI_ASSERT(0);
	}
    return EGL_TRUE;
}
}   //namespace OpenVGRI
