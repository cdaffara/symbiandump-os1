// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "confighelper.h"
#include "directgdiadapter.h"
#include <pixelformats.h>
#include <EGL/egltypes.h>
#include <fbs.h>

/**
Symbian Config Attributes for EGL pixmaps / pbuffers / windows.
*/
const EGLint TConfigHelper::KSurfaceAttribs[][19] =
	{
		{
		// EColor64K == EUidPixelFormatRGB_565
		// 16 bpp: 5 bits red, 6 bits green, 5 bits blue
		//
		EGL_BUFFER_SIZE,   		16,
		EGL_COLOR_BUFFER_TYPE,	EGL_RGB_BUFFER,
		EGL_RED_SIZE,			5,
		EGL_GREEN_SIZE, 		6,
		EGL_BLUE_SIZE,			5,
		EGL_ALPHA_SIZE, 		0,
		EGL_LUMINANCE_SIZE,		0,
		EGL_RENDERABLE_TYPE,	EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,		EGL_PIXMAP_BIT,
		EGL_NONE
		},
		{
		//EColor16MAP == EUidPixelFormatARGB_8888_PRE
		// 32 bpp: 8 bits premultiplied alpha, 8 bits red, 8 bits green, 8 bits blue
		//
		EGL_BUFFER_SIZE,    	32,
		EGL_COLOR_BUFFER_TYPE,	EGL_RGB_BUFFER,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE, 		8,
		EGL_BLUE_SIZE,			8,
		EGL_ALPHA_SIZE, 		8,
		EGL_LUMINANCE_SIZE, 	0,
		EGL_RENDERABLE_TYPE,	EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,		EGL_PIXMAP_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
		EGL_NONE
		},
		{
		// EColor16MU == EUidPixelFormatXRGB_8888
		// 32 bpp: 8 bits alpha (ignored), 8 bits red, 8 bits green, 8 bits blue
		//
		EGL_BUFFER_SIZE,    	24,
		EGL_COLOR_BUFFER_TYPE,	EGL_RGB_BUFFER,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE, 		8,
		EGL_BLUE_SIZE,			8,
		EGL_ALPHA_SIZE, 		0,
		EGL_LUMINANCE_SIZE, 	0,
		EGL_RENDERABLE_TYPE,	EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,		EGL_PIXMAP_BIT,
		EGL_NONE
		},		
		{
		//EColor16MA == EUidPixelFormatARGB_8888
		// 32 bpp: 8 bits alpha, 8 bits red, 8 bits green, 8 bits blue
		//
		EGL_BUFFER_SIZE,    	32,
		EGL_COLOR_BUFFER_TYPE,	EGL_RGB_BUFFER,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE, 		8,
		EGL_BLUE_SIZE,			8,
		EGL_ALPHA_SIZE, 		8,
		EGL_LUMINANCE_SIZE, 	0,
		EGL_RENDERABLE_TYPE,	EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,		EGL_PIXMAP_BIT,
		EGL_NONE
		},
		{
		// EGray256 == EUidPixelFormatL_8
		// 8 bpp: 8 bits luminance
		//
		EGL_BUFFER_SIZE,    	8,
		EGL_COLOR_BUFFER_TYPE,	EGL_LUMINANCE_BUFFER,
		EGL_RED_SIZE,			0,
		EGL_GREEN_SIZE, 		0,
		EGL_BLUE_SIZE,			0,
		EGL_ALPHA_SIZE, 		0,
		EGL_LUMINANCE_SIZE, 	8,
		EGL_RENDERABLE_TYPE,	EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,		EGL_PBUFFER_BIT,
		EGL_NONE
		},
		{
		// EGray2 == EUidPixelFormatL_1
		// 1 bpp: 1 bit luminance
		//
		EGL_BUFFER_SIZE,    	1,
		EGL_COLOR_BUFFER_TYPE,	EGL_LUMINANCE_BUFFER,
		EGL_RED_SIZE,			0,
		EGL_GREEN_SIZE, 		0,
		EGL_BLUE_SIZE,			0,
		EGL_ALPHA_SIZE, 		0,
		EGL_LUMINANCE_SIZE, 	1,
		EGL_RENDERABLE_TYPE,	EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,		EGL_PBUFFER_BIT,
		EGL_NONE
		}
	};
		
/**
Finds the exact matching configuration from the available configurations, based on the attribute values.

@pre 	aConfigs should not be NULL and EGL has been initialised.
@return The index of the matching config if successful, otherwise -1.
*/
TInt TConfigHelper::GetSuitablePixmapConfigIndex(EGLDisplay aDisplay, EGLConfig *aConfigs, TInt aNumConfigs, TInt aColorAttrib)
	{
	EGLint value;
	
#ifdef _DEBUG_DIRECTGDI
	TBuf16<256> message;
	message.Format(_L("ColorAtrib : %d"), aColorAttrib);
	GRAPHICS_LOGD_DEBUG(message);
	
	const TPtrC KEglAttribs[] =
		{
		_L("EGL_BUFFER_SIZE"),
		_L("EGL_COLOR_BUFFER_TYPE"),
		_L("EGL_RED_SIZE"),
		_L("EGL_GREEN_SIZE"),
		_L("EGL_BLUE_SIZE"),
		_L("EGL_ALPHA_SIZE"),
		_L("EGL_LUMINANCE_SIZE"),
		_L("EGL_RENDERABLE_TYPE"),
		_L("EGL_SURFACE_TYPE"),
		};
#endif
	
	TBool found = EFalse;
	for (TInt i=0; i<aNumConfigs && !found && aConfigs; i++)
		{
		found = ETrue;
#ifdef _DEBUG_DIRECTGDI
		message.Format(_L("Trying to match with config number: %d"), i);
		GRAPHICS_LOGD_DEBUG(message);
#endif
		for(TInt j=EEglBufferSize; j<ELast && found; ++++j)
			{
			eglGetConfigAttrib(aDisplay, aConfigs[i], KSurfaceAttribs[aColorAttrib][j], &value);
			found = EglAttributeMatches(KSurfaceAttribs[aColorAttrib][j], KSurfaceAttribs[aColorAttrib][j+1], value);
			
#ifdef _DEBUG_DIRECTGDI
			message.Format(_L("%S: Trying to match: %d  From EGL: %d\n"), &KEglAttribs[j>>1], KSurfaceAttribs[aColorAttrib][j+1], value);
			GRAPHICS_LOGD_DEBUG(message);
#endif
			}
		if (found)
			{
#ifdef _DEBUG_DIRECTGDI
			message.Format(_L("Found a suitable EGL Config: %d\n"), i);
			GRAPHICS_LOGD_DEBUG(message);
#endif
			return i;
			}
		}
	return -1;
	}

/**
Helper function for GetSuitablePixmapConfigIndex(). Returns, for the given attribute, whether or not the 
expected value matches the value received from egl. The kind of match is dependant on the attribute being tested.

@param aAttribute The EGL attribute ID.
@param aExpected The value we require the attribute to have for the current config we are testing.
@param aActual The value read from the current EGL config for this attribute.
@return ETrue, if aActual is deemed to match aExpected, otherwise EFalse.
*/
TBool TConfigHelper::EglAttributeMatches(EGLint aAttribute, TInt aExpected, TInt aActual)
	{
	switch(aAttribute)
		{
		case EGL_RENDERABLE_TYPE:
		case EGL_SURFACE_TYPE:
			// These are bitwise masks, so we don't need an exact match, just the bits to be set.
			return ((aExpected & aActual) == aExpected);		
		default: 
			return (aExpected == aActual);
		}
	}

/**
Converts the pixel type into an enum of supported pixel types. If a pixel type is not supported, it
is scaled to the nearest one.
Six destination pixel types are supported.

@panic DGDIAdapter 1009, if the pixel type is not supported.

@return Equivalent pixel type, as an index into KPixmapAttribs[].
*/
TInt TConfigHelper::MatchPixelType(TInt aPixelType)
	{
	TInt selPixmapAttribs = 0;
	switch(aPixelType)
		{
		case EUidPixelFormatP_4:
		case EUidPixelFormatP_8:
		case EUidPixelFormatXRGB_4444:
		case EUidPixelFormatRGB_565:
			selPixmapAttribs = EPixmapAttribsColor64K;
			break;
		case EUidPixelFormatARGB_8888_PRE:
			selPixmapAttribs = EPixmapAttribsColor16MAP;
			break;
		case EUidPixelFormatRGB_888:
		case EUidPixelFormatXRGB_8888:
			selPixmapAttribs = EPixmapAttribsColor16MU;
			break;
		case EUidPixelFormatARGB_8888:
			selPixmapAttribs = EPixmapAttribsColor16MA;
			break;			
		case EUidPixelFormatL_1:
			selPixmapAttribs = EPixmapAttribsLuminance1L;
			break;
		case EUidPixelFormatL_2:
		case EUidPixelFormatL_4:
		case EUidPixelFormatL_8:
			selPixmapAttribs = EPixmapAttribsLuminance8L;
			break;
		default: 
			GRAPHICS_PANIC_ALWAYS(EDirectGdiPanicInvalidDisplayMode);			
		}
	return selPixmapAttribs;
	}

/**
Given a TUidPixelFormat, returns an EGLConfig matching it.
If no exact match can be found, then returns the first available config.

@param 	aAttributeArrayIndex Index into config attribute array.
@param  aResult The resulting EGL config.

@pre EGL has been initialised.

@return KErrNone if a suitable config was found, otherwise one of the system-wide error codes.
 */
TInt TConfigHelper::GetConfig (TInt aAttributeArrayIndex, EGLConfig& aResult)
	{
	// Choose config
	EGLint numConfigs = 0;
	EGLDisplay display = eglGetCurrentDisplay();
	
	if (!eglGetConfigs(display, NULL, 0, &numConfigs))
		return KErrGeneral;
	
	if (numConfigs == 0)
		return KErrNotFound;
	
	EGLConfig* configs = new EGLConfig[numConfigs];
	if (configs == NULL)
		return KErrNoMemory;
	
	TInt match = -1;
	if (eglChooseConfig(display, KSurfaceAttribs[aAttributeArrayIndex], configs, numConfigs,	&numConfigs))
		{
		match = GetSuitablePixmapConfigIndex(display, configs, numConfigs, aAttributeArrayIndex);
		//If no exact match can be found, then take the first config.
		if(match == -1)
			match = 0;
		aResult = configs[match];		
		}	
	delete [] configs;
	return (match == -1) ? KErrNotFound : KErrNone;
	}

/**
Given a CFbsBitmap, return an EGLConfig that matches its display mode.

@param aBitmap Bitmap to get config for.
@param aResult The resulting EGL config, if found.

@pre aBitmap has already been validated by ValidateBitmap().

@return KErrNone if successful, otherwise one of the system-wide error codes.
*/
TInt TConfigHelper::GetConfigForFbsBitmap (const CFbsBitmap& aBitmap, EGLConfig& aResult)
	{
	TInt pixelType = -1;		
	switch (aBitmap.DisplayMode())
		{
		case EGray2:
			pixelType = EUidPixelFormatL_1;
			break;
		case EGray4:
			pixelType = EUidPixelFormatL_2;
			break;
		case EGray16:
			pixelType = EUidPixelFormatL_4;
			break;
		case EGray256:
			pixelType = EUidPixelFormatL_8;
			break;
		case EColor16:
			pixelType = EUidPixelFormatP_4;
			break;
		case EColor256:
			pixelType = EUidPixelFormatP_8;
			break;
		case EColor4K:
			pixelType = EUidPixelFormatXRGB_4444;
			break;
		case EColor64K:
			pixelType = EUidPixelFormatRGB_565;
			break;
		case EColor16M:
			pixelType = EUidPixelFormatRGB_888;		
			break;
		case EColor16MU:
			pixelType = EUidPixelFormatXRGB_8888;
			break;
		case EColor16MA:
			pixelType = EUidPixelFormatARGB_8888;
			break;
		case EColor16MAP:
			pixelType = EUidPixelFormatARGB_8888_PRE;
			break;
		default:
			return KErrNotSupported;
		}
	return GetConfig(MatchPixelType(pixelType), aResult);
	}


//Currently not called anywhere. However, it may be useful for performance improvement
//in the future. Will defer removing/reinstating until then.
#ifdef _DEBUG
/**
Debug method to iterate over supported configurations and print significant attributes.
Currently only EGL_OPENVG renderable types are displayed.

@pre 	EGL is initialised.
@leave 	PushL().
*/
void TConfigHelper::SupportedSurfacePixelFormatsL()
	{
	EGLDisplay currentDisplay = eglGetCurrentDisplay();
	EGLint numConfigs;
	EGLConfig* configs;
	TInt bufferSize, depthSize, stencilSize, luminance, renderableType, surfaceType, A, R, G, B;

	_LIT(KAttribFormat, "%3d: BDS-ARGBL: %2d, %2d, %2d - %d, %d, %d, %d, %d");
	_LIT(KEGL_OPENVG_BIT, ", EGL_OPENVG_BIT");
	_LIT(KEGL_PBUFFER_BIT, ", EGL_PBUFFER_BIT");
	_LIT(KEGL_PIXMAP_BIT, ", EGL_PIXMAP_BIT");
	_LIT(KEGL_VG_ALPHA_FORMAT_PRE_BIT, ", EGL_VG_ALPHA_FORMAT_PRE_BIT");
	
	if (eglGetConfigs(currentDisplay, NULL, 0x00, &numConfigs) == EGL_TRUE)
		{
		configs = (EGLConfig*)User::Alloc (numConfigs * sizeof(EGLConfig));
		if (configs)
			{
			CleanupStack::PushL(configs);
			if (eglGetConfigs(currentDisplay, configs, numConfigs, &numConfigs) == EGL_TRUE)
				{
				for (TUint i = 0; i < numConfigs; i++)
					{
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_BUFFER_SIZE, (EGLint*)&bufferSize);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_DEPTH_SIZE, (EGLint*)&depthSize);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_STENCIL_SIZE, (EGLint*)&stencilSize);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_ALPHA_SIZE, (EGLint*)&A);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_RED_SIZE, (EGLint*)&R);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_GREEN_SIZE, (EGLint*)&G);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_BLUE_SIZE, (EGLint*)&B);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_LUMINANCE_SIZE, (EGLint*)&luminance);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_RENDERABLE_TYPE, (EGLint*)&renderableType);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_RENDERABLE_TYPE, (EGLint*)&renderableType);
					eglGetConfigAttrib(currentDisplay, configs[i], EGL_SURFACE_TYPE, (EGLint*)&surfaceType);
					
					if (renderableType & EGL_OPENVG_BIT)
						{
						TBuf<256> buf;
						buf.Format(KAttribFormat, i, bufferSize, depthSize, stencilSize, A, R, G, B, luminance);
						buf.Append(KEGL_OPENVG_BIT);
						
						if (surfaceType & EGL_PBUFFER_BIT)	buf.Append(KEGL_PBUFFER_BIT);
						if (surfaceType & EGL_PIXMAP_BIT) buf.Append(KEGL_PIXMAP_BIT);
						if (surfaceType & EGL_VG_ALPHA_FORMAT_PRE_BIT) 	buf.Append(KEGL_VG_ALPHA_FORMAT_PRE_BIT);
						RDebug::Print(buf);
						}
					}
				}
			
			CleanupStack::PopAndDestroy(1, configs);
			}			
		}
	}
#endif

