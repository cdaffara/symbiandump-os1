// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#ifndef EGLTESTCOMMONUTILS_H
#define EGLTESTCOMMONUTILS_H

#include <w32std.h>
#include <pixelformats.h>
#include <EGL/eglext.h>
#include <VG/openvg.h>


/******************************************************************************/	
/***																		***/	
/*** 	Global constants and definitions for EGL Tests						***/
/***																		***/	
/******************************************************************************/	

const TUidPixelFormat KDefaultSourceFormat = EUidPixelFormatARGB_8888_PRE;
const VGImageFormat   KDefaultSurfaceFormat = VG_sARGB_8888_PRE;

const TInt KMaxEglConfigs = 1024;
const TInt KTestNoMatchingConfig = -1714;

enum TExtensionsGroups
	{
    EIsUndefined,
    EIsRSgImage,
	EIsEGL,
	EIsVG,	
	EIsGLES
	};
	
/**
 Provides an index into KConfigAttribs
 */
enum TEglTestConfig
	{ 
	EPixmapAttribsColor64K,
	EPixmapAttribsColor16MU,
	EPixmapAttribsColor16MAP,
	EWindowAttribsColor64K,
	EWindowAttribsColor16MA,
	EWindowAttribsColor16MAP,
	EPBufferAttribsColor64K,
	EPBufferAttribsColor16MU,
	EPBufferAttribsColor16MA,
	EPBufferAttribsColor16MAP,	
	EPBufferAttribsColorGles64K,
	EWindowAttribsColor16MA_Preserved,
	EEglTestConfigMax			// Must be the last item in the list. 
	};

// Symbian config attributes for pixmaps / pbuffers / windows
static const EGLint KConfigAttribs[EEglTestConfigMax][15] =
	{
		{
		//EColor64K - Pixmap
		EGL_BUFFER_SIZE,    16,
		EGL_RED_SIZE,		5,
		EGL_GREEN_SIZE, 	6,
		EGL_BLUE_SIZE,		5,
		EGL_ALPHA_SIZE, 	0,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_PIXMAP_BIT,
		EGL_NONE
		},
		{
		//EColor16MU - Pixmap
		EGL_BUFFER_SIZE,    24,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE,     0,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_PIXMAP_BIT,
		EGL_NONE
		},
		{
		//EColor16MAP - Pixmap
		EGL_BUFFER_SIZE,    32,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	8,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_PIXMAP_BIT | EGL_VG_COLORSPACE_LINEAR_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
		EGL_NONE
		},
		{
		//EWindowAttribsColor64K - Window
		EGL_BUFFER_SIZE,    16,
		EGL_RED_SIZE,		5,
		EGL_GREEN_SIZE, 	6,
		EGL_BLUE_SIZE,		5,
		EGL_ALPHA_SIZE, 	0,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_NONE
		},
		{
		//EWindowAttribsColor16MA - Window
		EGL_BUFFER_SIZE,    32,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	8,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT,
		EGL_NONE
		},
		{
		//EWindowAttribsColor16MAP - Window
		EGL_BUFFER_SIZE,    32,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	8,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_WINDOW_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
		EGL_NONE
		},
		{
		// EPBufferAttribsColor64K - PBuffer
		EGL_BUFFER_SIZE,    16,
		EGL_RED_SIZE,		5,
		EGL_GREEN_SIZE, 	6,
		EGL_BLUE_SIZE,		5,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_PBUFFER_BIT,
		EGL_NONE
		},
		{
		// EPBufferAttribsColor16MU - PBuffer
		EGL_BUFFER_SIZE,    24,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	0,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_PBUFFER_BIT,
		EGL_NONE
		},
		{
		// EPBufferAttribsColor16MA - PBuffer
		EGL_BUFFER_SIZE,    32,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	8,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_PBUFFER_BIT,
		EGL_NONE
		},
		{
		// EPBufferAttribsColor16MAP - PBuffer
		EGL_BUFFER_SIZE,    32,
		EGL_RED_SIZE,		8,
		EGL_GREEN_SIZE, 	8,
		EGL_BLUE_SIZE,		8,
		EGL_ALPHA_SIZE, 	8,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,	EGL_PBUFFER_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
		EGL_NONE
		},
		{
		// EPBufferAttribsColorGles64K - PBuffer
		EGL_BUFFER_SIZE,    16,
		EGL_RED_SIZE,		5,
		EGL_GREEN_SIZE, 	6,
		EGL_BLUE_SIZE,		5,
		EGL_ALPHA_SIZE, 	0,
		EGL_RENDERABLE_TYPE,EGL_OPENGL_ES_BIT,
		EGL_SURFACE_TYPE,	EGL_PBUFFER_BIT,
		EGL_NONE
		},
		{
		//EWindowAttribsColor16MA Preserved - Window 
		EGL_BUFFER_SIZE,    32,
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_RENDERABLE_TYPE,EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE,   EGL_WINDOW_BIT | EGL_SWAP_BEHAVIOR_PRESERVED_BIT,
		EGL_NONE
		},	
	};

enum TEglConfigMatchType
{
	/** Dummy, used for "uninitialized/unknowns/error" situations */
    eMatchUnknown,			
    /** Matches whatever - don't actually compare anything. */
    eMatchAlways,   		
    /** Matches equal values - config must match requested value. */
    eMatchEqual,			
    /** Matches greater or equal - config must be equal or greater than requested value. */
    eMatchAtLeast,			
    /** Match bitmask - config bits must have the requested bits. */
    eMatchBitMask			
};

const TInt KNumConfigMatchRules = 2;

struct TConfigMatchRuleItem
	{
	EGLint 				iAttrib;
	TEglConfigMatchType iMatchType;
	};

/**
 Defines rules for how to match a requested config with the configs returned by eglChooseConfig.
 */
static const TConfigMatchRuleItem KConfigMatchRules[KNumConfigMatchRules][15] =
{
	{
		// EConfigMatchStandard
		{ EGL_BUFFER_SIZE, eMatchAtLeast },
		{ EGL_RED_SIZE, eMatchEqual },
		{ EGL_GREEN_SIZE, eMatchEqual },
		{ EGL_BLUE_SIZE, eMatchEqual },
		{ EGL_ALPHA_SIZE, eMatchEqual },		
		{ EGL_RENDERABLE_TYPE, eMatchBitMask },
		{ EGL_SURFACE_TYPE, eMatchBitMask },
		{ EGL_NONE, eMatchUnknown }
	},
	{
		// EConfigMatchIgnoreAlpha
		{ EGL_BUFFER_SIZE, eMatchAtLeast },
		{ EGL_RED_SIZE, eMatchEqual },
		{ EGL_GREEN_SIZE, eMatchEqual },
		{ EGL_BLUE_SIZE, eMatchEqual },
		{ EGL_ALPHA_SIZE, eMatchAlways },		
		{ EGL_RENDERABLE_TYPE, eMatchBitMask },
		{ EGL_SURFACE_TYPE, eMatchBitMask },
		{ EGL_NONE, eMatchUnknown }
	}
};

/**
 Provides an index into KConfigMatchRules
 */
enum TEglConfigMatchRule
	{ 
	EConfigMatchStandard,
	EConfigMatchIgnoreAlpha,
	};

// Attributes that can be passed into eglCreatePixmapSurface()
const EGLint KPixmapAttribsNone[] = { EGL_NONE };
const EGLint KPixmapAttribsVgAlphaFormatPre[] = 
	{
	EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_PRE,
	EGL_NONE
	};

const EGLint KPixmapAttribsVgAlphaFormatNonPre[] = 
    {
    EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_NONPRE,
    EGL_NONE
    };

// Attributes that can be passed into eglCreateImageKHR()
const EGLint KEglImageAttribsNone[] = { EGL_NONE };
const EGLint KEglImageAttribsPreservedTrue[] = 
	{
	EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
	EGL_NONE
	};
const EGLint KEglImageAttribsPreservedFalse[] = 
	{
	EGL_IMAGE_PRESERVED_KHR, EGL_FALSE,
	EGL_NONE
	};

// Attributes that can be passed into eglCreateSyncKHR()
const EGLint KEglSyncAttribsNone[] = {EGL_NONE};

/**
 Performs a TEFUNIT style ASSERT, but also outputs the last EGL error code.
 Use this macro to check that EGL function calls have succeeded.
 */
#define ASSERT_EGL_TRUE(aFunction) \
	if(!(aFunction)) \
		{ \
		EGLint eglError = eglGetError(); \
		ERR_PRINTF2(_L("EGL Error: 0x%x"), eglError); \
		User::Leave(KErrTEFUnitFail); \
		}

/**
 Performs a TEFUNIT style ASSERT, but also outputs the last EGL error code.
 Use this macro to check that the expected EGL error has occurred.
*/
#define ASSERT_EGL_ERROR(aExpectedError) \
		{ \
		EGLint eglError = eglGetError(); \
		if (eglError != aExpectedError) \
			{ \
			ERR_PRINTF3(_L("Actual EGL Error: 0x%x, but Expected EGL Error: 0x%x"), eglError, aExpectedError); \
			User::Leave(KErrTEFUnitFail); \
			} \
		}

/**
  Performs a TEFUNIT style TEST. Use this macro to check that 
  EGL function calls have succeeded and the expected EGL error has occurred.
 */
#define TEST_EGL_ERROR(aFunction, aExpectedError) \
    TEST(aFunction); \
    TEST(eglGetError() == aExpectedError);

/**
 Performs a TEFUNIT style ASSERT, but also outputs the last VG error code.
 Use this macro to check that VG function calls have succeeded.
 */
#define ASSERT_VG_TRUE(aFunction) \
	if(!(aFunction)) \
		{ \
		VGErrorCode vgError = vgGetError(); \
		ERR_PRINTF2(_L("VG Error: 0x%x"), vgError); \
		User::Leave(KErrTEFUnitFail); \
		}

/**
 Performs a TEFUNIT style ASSERT, but also outputs the last VG error code.
 Use this macro to check that the expected VG error has occurred.
*/
#define ASSERT_VG_ERROR(aExpectedError) \
		{ \
		VGErrorCode vgError = vgGetError(); \
		if (vgError != aExpectedError) \
			{ \
			ERR_PRINTF3(_L("Actual VG Error: 0x%x, but Expected VG Error: 0x%x"), vgError, aExpectedError); \
			User::Leave(KErrTEFUnitFail); \
			} \
		}


// The following macros allow verbose logging to be switched on and off programmatically. 
#define VERBOSE_INFO_PRINTF1(p1)		if (iVerboseLogging) { INFO_PRINTF1(p1); }
#define VERBOSE_INFO_PRINTF2(p1, p2)	if (iVerboseLogging) { INFO_PRINTF2(p1, p2); }
#define VERBOSE_INFO_PRINTF3(p1, p2, p3)	if (iVerboseLogging) { INFO_PRINTF3(p1, p2, p3); }

#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
_LIT8(KEglRSgImage,"EGL_NOK_pixmap_type_rsgimage");
#else
_LIT8(KEglRSgImage,"EGL_SYMBIAN_PIXMAP_TYPE_RSGIMAGE");
#endif
_LIT8(KEglKhrImageBase,"EGL_KHR_image_base");
_LIT8(KEglKhrImagePixmap,"EGL_KHR_image_pixmap");
_LIT8(KVgKhrEglImage,"VG_KHR_EGL_image");
_LIT8(KEglKhrReusableSync,"EGL_KHR_reusable_sync");
_LIT8(KEglNokPrivateSignalSync,"EGL_NOK__private__signal_sync");
_LIT8(KEglNokiaSwapBuffers,"EGL_NOKIA_swap_buffers");
_LIT8(KEglNokiaImageEndpoint,"EGL_NOK_image_endpoint");
_LIT8(KEglNokiaSurfaceScaling,"EGL_NOK_surface_scaling");
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
_LIT8(KEglSymbianImagePreserved,"EGL_SYMBIAN_image_preserved");
#endif


enum TExtensionsId
	{
	KEGLUndefined,
    KEGL_RSgimage 					= 0x01,
    KEGL_KHR_image_base				= 0x02,
    KEGL_KHR_image_pixmap			= 0x04,	
    KVG_KHR_EGL_image				= 0x08,
	KEGL_KHR_reusable_sync			= 0x10,
	KEGL_NOK__private__signal_sync  = 0x20,
	KEGL_NOKIA_swap_buffers         = 0x40,
    KEGL_SYMBIAN_image_preserved    = 0x80,
    KEGL_NOK_image_endpoint         = 0x100,
    KEGL_NOK_surface_scaling        = 0x200
	};

const TUid KUidEglTestServer={0x10281B40};
const TUid KUidEglTestServerNga={0x10285DC0};

#endif	// EGLTESTCOMMONUTILS_H
