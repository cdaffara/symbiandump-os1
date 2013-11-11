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

#include "egltestcommonsession.h"
#include "egltestcommonutils.h"
#include "egltestcommonsgimageinfo.h"

#include <test/tefunit.h>
#include <e32cmn.h>
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <sgresource/sgresource.h>
#else
#include <graphics/sgresourceinternal.h>
#include <graphics/sgresource.h>
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <VG/openvg.h>
#include <GLES/gl.h>


#define	CHECK_EXPECTED_ERROR(aFunctionReturnValue) \
	if(!(CheckExpectedError(aFunctionReturnValue))) \
		{ \
		ERR_PRINTF2(_L("thread %d: Unexpected EGL error information"), iThreadIdx); \
		User::Leave(KErrTEFUnitFail); \
		}

const TMapEglConfigToPixelFormat KMapEglConfigToPixelFormat[] =
	{
	{16, 0, 5,6,5, 0, 							EGL_RGB_BUFFER, EUidPixelFormatRGB_565, EColor64K},
	{32, 0, 8,8,8, 0,							EGL_RGB_BUFFER, EUidPixelFormatXRGB_8888, EColor16MU},
    {32, 8, 8,8,8, EGL_VG_ALPHA_FORMAT_NONPRE, 	EGL_RGB_BUFFER, EUidPixelFormatARGB_8888, EColor16MA},
	{32, 8, 8,8,8, EGL_VG_ALPHA_FORMAT_PRE, 	EGL_RGB_BUFFER, EUidPixelFormatARGB_8888_PRE, EColor16MAP}
	};

EXPORT_C CTestEglSession::CTestEglSession(CTestExecuteLogger& aLogger, EGLDisplay& aDisplay, TInt aThreadIdx)
	: iLogger(aLogger), iThreadIdx(aThreadIdx), iExtensionGroupCached(EIsUndefined), iDisplay(aDisplay), iExpectedErrorCode(EGL_SUCCESS)
	{}

EXPORT_C CTestEglSession* CTestEglSession::NewLC(CTestExecuteLogger& aLogger, EGLDisplay& aDisplay, TInt aThreadIdx)
	{
	CTestEglSession* self = new (ELeave) CTestEglSession(aLogger, aDisplay, aThreadIdx);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CTestEglSession* CTestEglSession::NewL(CTestExecuteLogger& aLogger, EGLDisplay& aDisplay, TInt aThreadIdx)
	{
	CTestEglSession* self = CTestEglSession::NewLC(aLogger, aDisplay, aThreadIdx);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CTestEglSession::~CTestEglSession()
	{
	iExtensionStrings.Reset();
	EGLBoolean ret;
	if (iDisplay != EGL_NO_DISPLAY)
		{
		// Unbind and destroy context only if we successfuly created it
		if (iContext != EGL_NO_CONTEXT)
		    {
            INFO_PRINTF2(_L("thread %d: Calling eglMakeCurrent(NULL values) from ~CTestEglSession..."), iThreadIdx);
            ret = eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if(ret == EGL_FALSE)
                {
                WARN_PRINTF3(_L("thread %d: eglMakeCurrent returned error = %x ~CTestEglSession..."), iThreadIdx, eglGetError());
                }

            // Warn because this should be done by the test, rather than relying on the d'tor
            // which may not leave if there is an error (so no CHECK_EXPECTED_ERROR)
            WARN_PRINTF2(_L("thread %d: Calling eglDestroyContext() from ~CTestEglSession..."), iThreadIdx);
            ret = eglDestroyContext(iDisplay, iContext);
            if(ret == EGL_FALSE)
                {
                WARN_PRINTF3(_L("thread %d: eglDestroyContext returned error = %x ~CTestEglSession..."), iThreadIdx, eglGetError());
                }
		    }

		if (iSurface != EGL_NO_SURFACE)
			{
			// Warn because this should be done by the test, rather than relying on the d'tor
			// which may not leave if there is an error (so no CHECK_EXPECTED_ERROR)
			WARN_PRINTF2(_L("thread %d: Calling eglDestroySurface() from ~CTestEglSession..."), iThreadIdx);
			ret = eglDestroySurface(iDisplay, iSurface);
			if(ret == EGL_FALSE)
				{
				WARN_PRINTF3(_L("thread %d: eglDestroySurface returned error = %x ~CTestEglSession..."), iThreadIdx, eglGetError());
				}
			}

		if (iTerminateDisplay)
			{
			INFO_PRINTF1(_L("Calling eglTerminate..."));
			ret = eglTerminate(iDisplay);
			iDisplay = EGL_NO_DISPLAY;
			if(ret == EGL_FALSE)
				{
				WARN_PRINTF3(_L("thread %d: eglTerminate returned error = %x ~CTestEglSession..."), iThreadIdx, eglGetError());
				}
			}
		}

	// Only destroy native resource after the surface and context that wraps it has been destroyed.
	delete iFbsBitmap;
	CloseFbsSession();

	iSgImage.Close();
	CloseSgDriver();
	}

EXPORT_C void CTestEglSession::SetExpectedError(EGLint aExpectedErrorCode)
	{
	VERBOSE_INFO_PRINTF3(_L("thread %d: Setting the expected error code for the next EGL call to %x"), iThreadIdx, aExpectedErrorCode);
		{
		iExpectedErrorCode = aExpectedErrorCode;
		}
	}

EXPORT_C void CTestEglSession::ResetExpectedError()
	{
	iExpectedErrorCode = EGL_SUCCESS;
	}

/**
Checks whether the call to an EGL method returned the correct error information.
The method checks whether eglGetError() returns the expected error value as specified by calling SetExpectedError()
It also checks that the value returned by the EGL method was appropriate for the expected error value.
@param aFunctionReturnValue Pass in the value retured from the EGL method
@return Whether the expected error information was returned from a call to an EGL method.
*/
EXPORT_C TBool CTestEglSession::CheckExpectedError(EGLint aFunctionReturnValue)
	{
	TBool isExpectedError = ETrue;
	EGLint eglErr = eglGetError();

	// First check that we got the correct return value
	if ((iExpectedErrorCode == EGL_SUCCESS) && !aFunctionReturnValue)
		{
		ERR_PRINTF3(_L("thread %d: Wrong function return value: %d"), iThreadIdx, aFunctionReturnValue);
		isExpectedError = EFalse;
		}
	// Also check that we got the
	if (eglErr != iExpectedErrorCode)
		{
		ERR_PRINTF4(_L("thread %d: eglGetError() returned %x, but expected %x"), iThreadIdx, eglErr, iExpectedErrorCode);
		isExpectedError = EFalse;
		}
	else if (eglErr != EGL_SUCCESS)
		{
		VERBOSE_INFO_PRINTF3(_L("thread %d: eglGetError() returned %x, as expected"), iThreadIdx, eglErr);
		}

	// Reset the expected error
	ResetExpectedError();

	return isExpectedError;
	}

EXPORT_C void CTestEglSession::CheckExpectedErrorL(EGLint aExpectedErrorCode)
	{
	EGLint eglErr = eglGetError();
	// check that we got the expected error
	if (eglErr != aExpectedErrorCode)
		{
		ERR_PRINTF4(_L("thread %d: eglGetError() returned %x, but expected %x"), iThreadIdx, eglErr, aExpectedErrorCode);
		User::Leave(KErrTEFUnitFail);
		}
	}

void CTestEglSession::QueryExtensionsL(TExtensionsGroups aExtensionBelongsTo)
	{
	// reset the cached extensions
	iExtensionStrings.Reset();

	const char* extensionsString = NULL;
	if(aExtensionBelongsTo == EIsEGL)
		{
		INFO_PRINTF2(_L("thread %d: Calling eglQueryString for EGL_EXTENSIONS)"), iThreadIdx);
		extensionsString = eglQueryString(iDisplay, EGL_EXTENSIONS);
		}
	else if(aExtensionBelongsTo == EIsVG)
		{
		INFO_PRINTF2(_L("thread %d: Calling vgGetString for VG_EXTENSIONS)"), iThreadIdx);

		// OpenVG needs a current VG context before it will allow the call to vgGetString
		// The created surface will remain un-used, hence we create it with an arbitrary pixel format 
        EGLConfig currentConfig = GetConfigExactMatchL(EPBufferAttribsColor64K);
        CreatePbufferSurfaceAndMakeCurrentL(currentConfig, TSize(1,1), EGL_OPENVG_API);

		extensionsString = (const char*) vgGetString(VG_EXTENSIONS);

		//cleanup the context & surface
		CleanupSurfaceSgImageL();
		}
	else
		{
		ERR_PRINTF2(_L("CTestEglSession::QueryExtensionsL() - Unknown extension group provided (%d)."), aExtensionBelongsTo);
		User::Leave(KErrArgument);
		}
	CHECK_EXPECTED_ERROR(extensionsString!=NULL);

	TPtrC8 ptrExtensions((const TUint8 *) extensionsString );

	RBuf buffer;
    buffer.CreateL(ptrExtensions.Length());
    buffer.CleanupClosePushL();
    buffer.Copy(ptrExtensions);
	INFO_PRINTF3(_L("thread %d: QueryExtensionsL: \"%S\""), iThreadIdx, &buffer);
    CleanupStack::PopAndDestroy(&buffer);

	TInt posSpace=1;
	while (posSpace > 0 && ptrExtensions.Length() > 0)
		{
		posSpace = ptrExtensions.Locate(' '); // strictly looking for a single space
		ASSERT_FALSE(posSpace==0); // Would imply extension starting with a space or with 2 spaces in a row
		if (posSpace > 0)
			{
			iExtensionStrings.Append(ptrExtensions.Left(posSpace));
			if (posSpace <= ptrExtensions.Length())
				{
				ptrExtensions.Set(ptrExtensions.Mid(posSpace+1));
				}
			}
		else
			{
			iExtensionStrings.Append(ptrExtensions);
			}
		}
	}

TBool CTestEglSession::FindExtensionStringL(TExtensionsGroups aExtensionBelongsTo, const TDesC8& aExtensionString)
	{
	//Load the extensions for this group if not already cached
	if (iExtensionGroupCached != aExtensionBelongsTo)
		{
		QueryExtensionsL(aExtensionBelongsTo); // Load extension info
		iExtensionGroupCached = aExtensionBelongsTo;
		}

	TInt countExtensionStrings = iExtensionStrings.Count();
	for(TUint i=0; i<countExtensionStrings; i++)
		{
		if (iExtensionStrings[i].Compare(aExtensionString)==0)
			{
			// We have a match!
			return ETrue;
			}
		}

	// No match: copy the extension string into a 16 bit buffer for logging
	const TInt KBufLenMissingExtension=128;
	TBuf16<KBufLenMissingExtension> bufMissingExtension16;
	bufMissingExtension16.Copy(aExtensionString.Left(KBufLenMissingExtension));

	WARN_PRINTF2(_L("EGL extension missing: [%S]"), &bufMissingExtension16);
	return EFalse;
	}

/**
 Converts from TRgb space to Vg floating point colour
 */
EXPORT_C void CTestEglSession::ConvertColor(const TRgb& aSource, VGfloat* aTarget)
//static
	{
	aTarget[0] = (VGfloat)aSource.Red() / 255.f;
	aTarget[1] = (VGfloat)aSource.Green() / 255.f;
	aTarget[2] = (VGfloat)aSource.Blue() / 255.f;
	aTarget[3] = (VGfloat)aSource.Alpha() / 255.f;
	}

/** Breakpoint to manually view available configs while debugging */
EXPORT_C void CTestEglSession::ViewConfigsL()
	{
	EGLConfig configs[KMaxEglConfigs];
	EGLint numConfigs=0;

	// Query number of configs available
	INFO_PRINTF1(_L("Calling eglGetConfigs to get the number of configs available..."));
	CHECK_EXPECTED_ERROR(eglGetConfigs(iDisplay, NULL, KMaxTInt, &numConfigs));

	INFO_PRINTF1(_L("Checking number of configs..."));
	ASSERT_TRUE(numConfigs >= 1);
	INFO_PRINTF2(_L("Found %d configs"), numConfigs);

	// Get the configs
	INFO_PRINTF1(_L("Calling eglGetConfigs to get configs..."));
	CHECK_EXPECTED_ERROR(eglGetConfigs(iDisplay, configs, KMaxEglConfigs, &numConfigs));

	for(TUint index = 0; index < numConfigs; index++)
		{
		EGLint EGL_BUFFER_SIZE_value;
		EGLint EGL_ALPHA_SIZE_value;
		EGLint EGL_BLUE_SIZE_value;
		EGLint EGL_GREEN_SIZE_value;
		EGLint EGL_RED_SIZE_value;
#ifdef PRINT_ALL_CONFIGS_DETAILS
		EGLint EGL_DEPTH_SIZE_value;
		EGLint EGL_STENCIL_SIZE_value;
		EGLint EGL_CONFIG_CAVEAT_value;
		EGLint EGL_CONFIG_ID_value;
		EGLint EGL_LEVEL_value;
		EGLint EGL_MAX_PBUFFER_HEIGHT_value;
		EGLint EGL_MAX_PBUFFER_PIXELS_value;
		EGLint EGL_MAX_PBUFFER_WIDTH_value;
		EGLint EGL_NATIVE_RENDERABLE_value;
		EGLint EGL_NATIVE_VISUAL_ID_value;
		EGLint EGL_NATIVE_VISUAL_TYPE_value;
		EGLint EGL_SAMPLES_value;
		EGLint EGL_SAMPLE_BUFFERS_value;
#endif
		EGLint EGL_SURFACE_TYPE_value;
#ifdef PRINT_ALL_CONFIGS_DETAILS
		EGLint EGL_TRANSPARENT_TYPE_value;
		EGLint EGL_TRANSPARENT_BLUE_VALUE_value;
		EGLint EGL_TRANSPARENT_GREEN_VALUE_value;
		EGLint EGL_TRANSPARENT_RED_VALUE_value;
		EGLint EGL_BIND_TO_TEXTURE_RGB_value;
		EGLint EGL_BIND_TO_TEXTURE_RGBA_value;
		EGLint EGL_MIN_SWAP_INTERVAL_value;
		EGLint EGL_MAX_SWAP_INTERVAL_value;
		EGLint EGL_LUMINANCE_SIZE_value;
		EGLint EGL_ALPHA_MASK_SIZE_value;
		EGLint EGL_COLOR_BUFFER_TYPE_value;
#endif
		EGLint EGL_RENDERABLE_TYPE_value;

		eglGetConfigAttrib(iDisplay, configs[index], EGL_BUFFER_SIZE, &EGL_BUFFER_SIZE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_ALPHA_SIZE, &EGL_ALPHA_SIZE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_BLUE_SIZE, &EGL_BLUE_SIZE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_GREEN_SIZE, &EGL_GREEN_SIZE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_RED_SIZE, &EGL_RED_SIZE_value);
#ifdef PRINT_ALL_CONFIGS_DETAILS
		eglGetConfigAttrib(iDisplay, configs[index], EGL_DEPTH_SIZE, &EGL_DEPTH_SIZE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_STENCIL_SIZE, &EGL_STENCIL_SIZE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_CONFIG_CAVEAT, &EGL_CONFIG_CAVEAT_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_CONFIG_ID, &EGL_CONFIG_ID_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_LEVEL, &EGL_LEVEL_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_MAX_PBUFFER_HEIGHT, &EGL_MAX_PBUFFER_HEIGHT_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_MAX_PBUFFER_PIXELS, &EGL_MAX_PBUFFER_PIXELS_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_MAX_PBUFFER_WIDTH, &EGL_MAX_PBUFFER_WIDTH_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_NATIVE_RENDERABLE, &EGL_NATIVE_RENDERABLE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_NATIVE_VISUAL_ID, &EGL_NATIVE_VISUAL_ID_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_NATIVE_VISUAL_TYPE, &EGL_NATIVE_VISUAL_TYPE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_SAMPLES, &EGL_SAMPLES_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_SAMPLE_BUFFERS, &EGL_SAMPLE_BUFFERS_value);
#endif
		eglGetConfigAttrib(iDisplay, configs[index], EGL_SURFACE_TYPE, &EGL_SURFACE_TYPE_value);
#ifdef PRINT_ALL_CONFIGS_DETAILS
		eglGetConfigAttrib(iDisplay, configs[index], EGL_TRANSPARENT_TYPE, &EGL_TRANSPARENT_TYPE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_TRANSPARENT_BLUE_VALUE, &EGL_TRANSPARENT_BLUE_VALUE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_TRANSPARENT_GREEN_VALUE, &EGL_TRANSPARENT_GREEN_VALUE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_TRANSPARENT_RED_VALUE, &EGL_TRANSPARENT_RED_VALUE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_BIND_TO_TEXTURE_RGB, &EGL_BIND_TO_TEXTURE_RGB_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_BIND_TO_TEXTURE_RGBA, &EGL_BIND_TO_TEXTURE_RGBA_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_MIN_SWAP_INTERVAL, &EGL_MIN_SWAP_INTERVAL_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_MAX_SWAP_INTERVAL, &EGL_MAX_SWAP_INTERVAL_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_LUMINANCE_SIZE, &EGL_LUMINANCE_SIZE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_ALPHA_MASK_SIZE, &EGL_ALPHA_MASK_SIZE_value);
		eglGetConfigAttrib(iDisplay, configs[index], EGL_COLOR_BUFFER_TYPE, &EGL_COLOR_BUFFER_TYPE_value);
#endif
		eglGetConfigAttrib(iDisplay, configs[index], EGL_RENDERABLE_TYPE, &EGL_RENDERABLE_TYPE_value);

		TBool good = ETrue;

		INFO_PRINTF7(_L("(idx: %3d) RGBA=(%2d) %2d,%2d,%2d,%2d"), index, EGL_BUFFER_SIZE_value,
				EGL_RED_SIZE_value, EGL_GREEN_SIZE_value, EGL_BLUE_SIZE_value,
				EGL_ALPHA_SIZE_value);
		if (!(EGL_RED_SIZE_value == 8 && EGL_GREEN_SIZE_value == 8 && EGL_BLUE_SIZE_value == 8 && EGL_ALPHA_SIZE_value == 0))
			{
			continue;
			}

		INFO_PRINTF2(_L("\n\n^^^^^^ CONFIG [%d] VALUES ******"), index);
		if (EGL_SURFACE_TYPE_value & EGL_PIXMAP_BIT)
			{
			INFO_PRINTF2(_L("EGL_SURFACE_TYPE=%d. <<< Has EGL_PIXMAP_BIT"), EGL_SURFACE_TYPE_value);
			}
		else
			{
			INFO_PRINTF2(_L("EGL_SURFACE_TYPE=%d. <<< BAD (not pixmap)"), EGL_SURFACE_TYPE_value);
			good = EFalse;
			}

		if (EGL_RENDERABLE_TYPE_value & EGL_OPENVG_BIT)
			{
			INFO_PRINTF2(_L("EGL_RENDERABLE_TYPE=%d. <<< Has EGL_OPENVG_BIT"), EGL_RENDERABLE_TYPE_value);
			}
		else
			{
			INFO_PRINTF2(_L("EGL_RENDERABLE_TYPE=%d. <<< BAD (not open vg)"), EGL_RENDERABLE_TYPE_value);
			good = EFalse;
			}

		if (good)
			{
			INFO_PRINTF1(_L("^^^^^^^ GOOD ^^^^^^^"));
			}

#ifdef PRINT_ALL_CONFIGS_DETAILS
		INFO_PRINTF2(_L("\n\n***** CONFIG [%d] VALUES ******"), index);
		INFO_PRINTF2(_L("EGL_BUFFER_SIZE=%d."), EGL_BUFFER_SIZE_value);
		INFO_PRINTF2(_L("EGL_ALPHA_SIZE=%d."), EGL_ALPHA_SIZE_value);
		INFO_PRINTF2(_L("EGL_BLUE_SIZE=%d."), EGL_BLUE_SIZE_value);
		INFO_PRINTF2(_L("EGL_GREEN_SIZE=%d."), EGL_GREEN_SIZE_value);
		INFO_PRINTF2(_L("EGL_RED_SIZE=%d."), EGL_RED_SIZE_value);
		INFO_PRINTF2(_L("EGL_DEPTH_SIZE=%d."), EGL_DEPTH_SIZE_value);
		INFO_PRINTF2(_L("EGL_STENCIL_SIZE=%d."), EGL_STENCIL_SIZE_value);
		INFO_PRINTF2(_L("EGL_CONFIG_CAVEAT=%d."), EGL_CONFIG_CAVEAT_value);
		INFO_PRINTF2(_L("EGL_CONFIG_ID=%d."), EGL_CONFIG_ID_value);
		INFO_PRINTF2(_L("EGL_LEVEL=%d."), EGL_LEVEL_value);
		INFO_PRINTF2(_L("EGL_MAX_PBUFFER_HEIGHT=%d."), EGL_MAX_PBUFFER_HEIGHT_value);
		INFO_PRINTF2(_L("EGL_MAX_PBUFFER_PIXELS=%d."), EGL_MAX_PBUFFER_PIXELS_value);
		INFO_PRINTF2(_L("EGL_MAX_PBUFFER_WIDTH=%d."), EGL_MAX_PBUFFER_WIDTH_value);
		INFO_PRINTF2(_L("EGL_NATIVE_RENDERABLE=%d."), EGL_NATIVE_RENDERABLE_value);
		INFO_PRINTF2(_L("EGL_NATIVE_VISUAL_ID=%d."), EGL_NATIVE_VISUAL_ID_value);
		INFO_PRINTF2(_L("EGL_NATIVE_VISUAL_TYPE=%d."), EGL_NATIVE_VISUAL_TYPE_value);
		INFO_PRINTF2(_L("EGL_SAMPLES=%d."), EGL_SAMPLES_value);
		INFO_PRINTF2(_L("EGL_SAMPLE_BUFFERS=%d."), EGL_SAMPLE_BUFFERS_value);
		INFO_PRINTF2(_L("EGL_SURFACE_TYPE=%d."), EGL_SURFACE_TYPE_value);
		INFO_PRINTF2(_L("EGL_TRANSPARENT_TYPE=%d."), EGL_TRANSPARENT_TYPE_value);
		INFO_PRINTF2(_L("EGL_TRANSPARENT_BLUE_VALUE=%d."), EGL_TRANSPARENT_BLUE_VALUE_value);
		INFO_PRINTF2(_L("EGL_TRANSPARENT_GREEN_VALUE=%d."), EGL_TRANSPARENT_GREEN_VALUE_value);
		INFO_PRINTF2(_L("EGL_TRANSPARENT_RED_VALUE=%d."), EGL_TRANSPARENT_RED_VALUE_value);
		INFO_PRINTF2(_L("EGL_BIND_TO_TEXTURE_RGB=%d."), EGL_BIND_TO_TEXTURE_RGB_value);
		INFO_PRINTF2(_L("EGL_BIND_TO_TEXTURE_RGBA=%d."), EGL_BIND_TO_TEXTURE_RGBA_value);
		INFO_PRINTF2(_L("EGL_MIN_SWAP_INTERVAL=%d."), EGL_MIN_SWAP_INTERVAL_value);
		INFO_PRINTF2(_L("EGL_MAX_SWAP_INTERVAL=%d."), EGL_MAX_SWAP_INTERVAL_value);
		INFO_PRINTF2(_L("EGL_LUMINANCE_SIZE=%d."), EGL_LUMINANCE_SIZE_value);
		INFO_PRINTF2(_L("EGL_ALPHA_MASK_SIZE=%d."), EGL_ALPHA_MASK_SIZE_value);
		INFO_PRINTF2(_L("EGL_COLOR_BUFFER_TYPE=%d."), EGL_COLOR_BUFFER_TYPE_value);
		INFO_PRINTF2(_L("EGL_RENDERABLE_TYPE=%d."), EGL_RENDERABLE_TYPE_value);
		INFO_PRINTF2(_L("\n*********************************\n\n"), index);
#endif
		}
	}

LOCAL_C TEglConfigMatchType GetMatchType(EGLint aAttrib, TEglConfigMatchRule aMatchRule)
	{
	const TConfigMatchRuleItem* curMatchRuleItem = KConfigMatchRules[aMatchRule];

	while(curMatchRuleItem->iAttrib != EGL_NONE)
		{
		if (aAttrib == curMatchRuleItem->iAttrib)
			{
			return curMatchRuleItem->iMatchType;
			}
		curMatchRuleItem++;
		}

	RDebug::Printf("Unknown attrib %x", aAttrib);
	return eMatchUnknown;
	}

/**
 Returns the first index of a config that matches the requested config extactly
 */
TInt CTestEglSession::GetFullMatchConfigIndex(EGLDisplay aDisplay, EGLConfig *aConfigs, TInt aNumConfigs,
											  const EGLint aWantedAttribs[], TEglConfigMatchRule aMatchRule)
	{
	EGLint value=0;
	for(TUint idxConfig=0; idxConfig<aNumConfigs; idxConfig++)
		{
		const EGLint *curAttrib = aWantedAttribs;
		TBool match = ETrue;

		while(*curAttrib != EGL_NONE && match)
			{
			ASSERT_EGL_TRUE(eglGetConfigAttrib(aDisplay, aConfigs[idxConfig], *curAttrib, &value));

			switch(GetMatchType(*curAttrib, aMatchRule))
				{
				case eMatchEqual:
					if (value != curAttrib[1])
						{
						match = EFalse;
						}
					break;
				case eMatchAtLeast:
					if (value < curAttrib[1])   // Note, we detect "failure to match", hence "<" not ">="!
						{
						match = EFalse;
						}
					break;
				case eMatchBitMask:
					if ((value & curAttrib[1]) != curAttrib[1])
						{
						match = EFalse;
						}
					break;
				case eMatchAlways:
				    break;

				default:
					// We should not get here.
				    ASSERT(FALSE);
				    break;
				}
			curAttrib += 2;
			}

		// If we get here with match set, we have matched all attributes, and have found a match.
		if (match)
			{
			return idxConfig;
			}
		}
	return KErrNotFound;
	}

TInt CTestEglSession::ConfigToPixelFormatTableLength() const
	{
	return sizeof(KMapEglConfigToPixelFormat) / sizeof(TMapEglConfigToPixelFormat);
	}

const TMapEglConfigToPixelFormat& CTestEglSession::ConfigToPixelFormatTable(TInt aIndex) const
	{
	return KMapEglConfigToPixelFormat[aIndex];
	}

/**
 Returns pixel format inforamtion for a given EGL config.
 @param aConfig The EGL config for which pixel format information will be returned.
 @return A pointer to the pixel format information for the given EGL config.
         Tf the config cannot be mapped, then NULL is returned.
 */
EXPORT_C const TMapEglConfigToPixelFormat* CTestEglSession::GetPixelFormatFromEglConfigL(EGLConfig aConfig)
	{
	EGLint bufferSize=0;
	EGLint alphaSize=0;
	EGLint redSize=0;
	EGLint greenSize=0;
	EGLint blueSize=0;
	EGLint colorBufferType=0;
	EGLint surfaceType = 0;

	CHECK_EXPECTED_ERROR(eglGetConfigAttrib(iDisplay, aConfig, EGL_BUFFER_SIZE, &bufferSize));
	CHECK_EXPECTED_ERROR(eglGetConfigAttrib(iDisplay, aConfig, EGL_ALPHA_SIZE, &alphaSize));
	CHECK_EXPECTED_ERROR(eglGetConfigAttrib(iDisplay, aConfig, EGL_RED_SIZE, &redSize));
	CHECK_EXPECTED_ERROR(eglGetConfigAttrib(iDisplay, aConfig, EGL_GREEN_SIZE, &greenSize));
	CHECK_EXPECTED_ERROR(eglGetConfigAttrib(iDisplay, aConfig, EGL_BLUE_SIZE, &blueSize));
	CHECK_EXPECTED_ERROR(eglGetConfigAttrib(iDisplay, aConfig, EGL_COLOR_BUFFER_TYPE, &colorBufferType));
	CHECK_EXPECTED_ERROR(eglGetConfigAttrib(iDisplay, aConfig, EGL_SURFACE_TYPE, &surfaceType));

	INFO_PRINTF7(_L(">>>>> Config info: %d, %d, %d,%d,%d, 0x%x"), bufferSize, alphaSize, redSize, greenSize, blueSize, colorBufferType);

	for(TUint i=0; i<ConfigToPixelFormatTableLength(); i++)
		{
		if ((ConfigToPixelFormatTable(i).iBufferSize == bufferSize)
				&& (ConfigToPixelFormatTable(i).iAlphaSize == alphaSize)
				&& (ConfigToPixelFormatTable(i).iRedSize == redSize)
				&& (ConfigToPixelFormatTable(i).iGreenSize == greenSize)
				&& (ConfigToPixelFormatTable(i).iBlueSize == blueSize)
				&& (ConfigToPixelFormatTable(i).iColorBufferType == colorBufferType)
				&& ((ConfigToPixelFormatTable(i).iSurfaceTypeFlags & surfaceType) == ConfigToPixelFormatTable(i).iSurfaceTypeFlags))
			{
			return &ConfigToPixelFormatTable(i);
			}
		}

	return NULL;
	}

EXPORT_C void CTestEglSession::CleanupSurfaceFbsBitmapL()
	{
	CleanupSurfaceAndContextL();

	delete iFbsBitmap;
	iFbsBitmap = NULL;
	}

EXPORT_C void CTestEglSession::CleanupSurfaceAndContextL()
	{
	ASSERT_TRUE(iSurface != EGL_NO_SURFACE);
	ASSERT_TRUE(iContext != EGL_NO_CONTEXT);

	// Clean up
	INFO_PRINTF1(_L("Calling eglMakeCurrent(NULL values)..."));
	CHECK_EXPECTED_ERROR(eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));

	INFO_PRINTF1(_L("Calling eglDestroyContext()..."));
	CHECK_EXPECTED_ERROR(eglDestroyContext(iDisplay, iContext));
	iContext = EGL_NO_CONTEXT;

	INFO_PRINTF1(_L("Calling eglDestroySurface()..."));
	CHECK_EXPECTED_ERROR(eglDestroySurface(iDisplay, iSurface));
	iSurface = EGL_NO_SURFACE;
	}

EXPORT_C CFbsBitmap* CTestEglSession::NativeFbsBitmap()
	{
	return iFbsBitmap;
	}

EXPORT_C RSgImage& CTestEglSession::NativeSgImage()
	{
	return iSgImage;
	}

EXPORT_C EGLSurface CTestEglSession::Surface() const
	{
	return iSurface;
	}

EXPORT_C EGLContext CTestEglSession::Context() const
	{
	return iContext;
	}

EXPORT_C EGLConfig CTestEglSession::GetConfigExactMatchL(TEglTestConfig aConfigAttribIndex, TEglConfigMatchRule aMatchRule)
	{
	if(aConfigAttribIndex >= EEglTestConfigMax )
		{
		ERR_PRINTF1(_L("Attribute index out of range, please check the INI file"));
		User::Leave(KErrArgument);
		}

	EGLConfig configs[KMaxEglConfigs];
	EGLint numConfigs=0;
	ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribs[aConfigAttribIndex], configs, KMaxEglConfigs, &numConfigs));
	if (numConfigs <= 0)
		{
		// we just WARN, as we don't know if this is expected yet (if it is, then catch the leaving error)
		WARN_PRINTF1(_L("GetConfigExactMatchL - Could not find a matching config, eglChooseConfig returned 0 configs!"));
		User::Leave(KTestNoMatchingConfig);
		}

	// Check that any of the configs returned matches the desired attributes
	TInt match = GetFullMatchConfigIndex(iDisplay, configs, numConfigs, KConfigAttribs[aConfigAttribIndex], aMatchRule);
	if (match == KErrNotFound)
		{
		// we just WARN, as we don't know if this is expected yet (if it is, then catch the leaving error)
		WARN_PRINTF1(_L("GetConfigExactMatchL - Could not find a matching config amongst those returned by eglChooseConfig!"));
		User::Leave(KTestNoMatchingConfig);
		}
	return configs[match];
	}

/**
Creates a pixmap surface from a CFbsBitmap, draws to it and checks that the drawing operation succeeded.
*/
EXPORT_C void CTestEglSession::TryUsePixmapCFbsBitmapL()
	{
	INFO_PRINTF2(_L("thread %d: TryUsePixmapCFbsBitmapL"), iThreadIdx);
	EGLConfig currentConfig = GetConfigExactMatchL(EPixmapAttribsColor64K);
	TryUsePixmapCFbsBitmapOpenVgL(currentConfig, KPixmapSize, EColor64K);
	}

/**
Creates a pixmap surface from a CFbsBitmap, draws to it and checks that the drawing operation succeeded.
*/
EXPORT_C void CTestEglSession::TryUsePixmapCFbsBitmapOpenVgL(EGLConfig aConfig, const TSize& aSize, TDisplayMode aDisplayMode)
	{
	CreatePixmapSurfaceAndMakeCurrentL(aConfig, aSize, aDisplayMode);
	DrawOpenVgL();

	// Wait for the drawing to complete
	eglWaitClient();

	CheckImageDataL(iFbsBitmap);
	CheckImageDataVgL(VG_sRGB_565);
	CleanupSurfaceFbsBitmapL();
	CloseFbsSession();
	}

/**
Creates a pixmap surface from a CFbsBitmap, draws to it and checks that the drawing operation succeeded.
*/
EXPORT_C void CTestEglSession::TryUsePixmapCFbsBitmapOpenGlesL(EGLConfig aConfig, const TSize& aSize, TDisplayMode aDisplayMode, TInt aRenderVersion)
	{
	CreatePixmapSurfaceAndMakeCurrentL(aConfig, aSize, aDisplayMode,EGL_OPENGL_ES_API, aRenderVersion);
	DrawOpenGLesL();

	// Wait for the drawing to complete
	eglWaitClient();

	CheckImageDataFullRedishL(iFbsBitmap);
	CleanupSurfaceFbsBitmapL();
	CloseFbsSession();
	}

/**
 Creates a pixmap surface from a RSgImage, draws to it and checks that the drawing operation succeeded.
 */
EXPORT_C void CTestEglSession::TryUsePixmapRSgImageL()
	{
	INFO_PRINTF2(_L("thread %d: TryUsePixmapRSgImageL"), iThreadIdx);
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = KPixmapSize;
	imageInfo.iPixelFormat = EUidPixelFormatXRGB_8888;
	// will draw to and read from the image using OpenVg
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	imageInfo.iUsage = ESgUsageBitOpenVgSurface | ESgUsageBitOpenVgImage;
#else
    // will also read from the image using DirectGdi
    imageInfo.iUsage = ESgUsageOpenVgTarget | ESgUsageDirectGdiSource;
    imageInfo.iShareable = EFalse;
    imageInfo.iCpuAccess = ESgCpuAccessNone;
    imageInfo.iScreenId = KSgScreenIdMain;
    imageInfo.iUserAttributes = NULL;
    imageInfo.iUserAttributeCount=0;
#endif
	CreatePixmapSurfaceAndMakeCurrentAndMatchL(imageInfo, EResourceCloseSgImageLate);
	DrawOpenVgL();

	// Wait for the drawing to complete
	 eglWaitClient();

	//we can't retrieve data directly from the SgImage as
	//non-unified architecture doesn't allow readback from the GPU
	CheckImageDataVgL(VG_sXRGB_8888);
	CleanupSurfaceSgImageL();
	CloseSgDriver();
	}

EXPORT_C TBool CTestEglSession::TryUsePixmapRSgImageOpenGlesL(EGLConfig aConfig, const TSgImageInfo& aImageInfo, TResourceCloseRule aResourceCloseRule, TInt aRenderVersion)
	{
	CreatePixmapSurfaceAndMakeCurrentL(aConfig, aImageInfo, aResourceCloseRule ,EGL_OPENGL_ES_API, aRenderVersion);

	DrawOpenGLesL();

	// Wait for the drawing to complete
	eglWaitClient();

	//we can't retrieve data directly from SgImage as non-unified
	//architecture doesn't allow readback from the GPU

	if (aImageInfo.iPixelFormat == EUidPixelFormatXRGB_8888  ||
		aImageInfo.iPixelFormat == EUidPixelFormatARGB_8888_PRE)
		{
		CheckImageDataGLesL();
		}

	CleanupSurfaceSgImageL();
	CloseSgDriver();
	return ETrue;
	}

/**
Compound operation that constructs an RSgImage and uses it to create a pixmap suface.
It then draws to the surface using OpenVG and checks whether the drawing succeeded.
It cleans up the pixmap surface it destroyed.
@param aConfig The EGL config to be used when creating the pixmap surface
@param aImageInfo Used to create the RSgImage pixmap
@param aCloseNativeImageEarly When ETrue, the RSgImage is closed as soon as the pixmap surface has been
		created.  Otherwise, the RSgImage is closed during cleanup, when the pixmap surface is destroyed.
@return Whether it was possible to create an iSgImage for the given aImageInfo
*/
EXPORT_C TBool CTestEglSession::TryUsePixmapRSgImageOpenVgL(EGLConfig aConfig, const TSgImageInfo& aImageInfo, TResourceCloseRule aResourceCloseRule)
	{
	CreatePixmapSurfaceAndMakeCurrentL(aConfig, aImageInfo, aResourceCloseRule);

	DrawOpenVgL();

	// Wait for the drawing to complete
	eglWaitClient();

	//we can't retrieve data directly from the SgImage as
	//non-unified architecture doesn't allow readback from the GPU
	CheckImageDataVgL(VG_sXRGB_8888);
	CleanupSurfaceSgImageL();
	CloseSgDriver();
	return ETrue;
	}

EXPORT_C void CTestEglSession::CreateWindowSurfaceAndMakeCurrentL(EGLConfig aConfig, RWindow& aWindow, TBool aVgAlphaFormatPre, EGLenum aBindAPI, TInt aRenderVersionNumber, EGLint* aAttribList)
	{
	ASSERT_EQUALS(iSurface, EGL_NO_SURFACE);
	ASSERT_EQUALS(iContext, EGL_NO_CONTEXT);

	ASSERT_EGL_TRUE(eglBindAPI(aBindAPI));

	// Create a Window surface from the native image
	const EGLint* windowAttribs = aAttribList;
    if(!windowAttribs)
        {
        windowAttribs = (aVgAlphaFormatPre && aBindAPI == EGL_OPENVG_API) ? KPixmapAttribsVgAlphaFormatPre : KPixmapAttribsNone;
        }
	iSurface = eglCreateWindowSurface(iDisplay, aConfig, &aWindow, windowAttribs);
	ASSERT_EGL_TRUE(iSurface != EGL_NO_SURFACE);

	// Create a context for drawing to/reading from the pixmap surface and make it current
	const EGLint KAttribsListCtxNone[] = { EGL_NONE };
	const EGLint KAttribsListCtxGles2[] = { EGL_CONTEXT_CLIENT_VERSION, aRenderVersionNumber, EGL_NONE };
	const EGLint* attrib_list_ctx = (aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber == 2) ? KAttribsListCtxGles2 : KAttribsListCtxNone;
	iContext = eglCreateContext(iDisplay, aConfig, EGL_NO_CONTEXT, attrib_list_ctx);
	ASSERT_EGL_TRUE(iContext != EGL_NO_CONTEXT);
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
	}


EXPORT_C TBool CTestEglSession::CongfigSupportsOpenVgL(EGLConfig aConfig)
	{
	EGLint EGL_RENDERABLE_TYPE_value = 0;
	ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, aConfig, EGL_RENDERABLE_TYPE, &EGL_RENDERABLE_TYPE_value));
	return (EGL_RENDERABLE_TYPE_value&EGL_OPENVG_BIT)>0;
	}

EXPORT_C void CTestEglSession::CreatePixmapSurfaceAndMakeCurrentAndMatchL(const TSgImageInfo& aImageInfo, TResourceCloseRule aResourceCloseRule, EGLenum aBindAPI, TInt aRenderVersionNumber)
	{
	ASSERT_EQUALS(iSurface, EGL_NO_SURFACE);
	ASSERT_EQUALS(iContext, EGL_NO_CONTEXT);

    OpenSgDriverL();
    ASSERT_EQUALS(iSgImage.Create(aImageInfo, NULL, 0), KErrNone);

	EGLint renderableType = 0;
	if(aBindAPI == EGL_OPENVG_API)
		{
		renderableType = EGL_OPENVG_BIT;
		}
	else if(aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber==1)
		{
		renderableType = EGL_OPENGL_ES_BIT;
		}
	else if(aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber==2)
		{
		renderableType = EGL_OPENGL_ES2_BIT;
		}
	else
		{
		ERR_PRINTF1(_L("CreatePixmapSurfaceAndMakeCurrentAndMatchL - Unkown API requested!"));
		User::Leave(KErrArgument);
		}
	EGLint attrib_list[] = {
			  EGL_MATCH_NATIVE_PIXMAP,(TInt)&iSgImage,
			  EGL_RENDERABLE_TYPE,renderableType,
			  EGL_SURFACE_TYPE,EGL_PIXMAP_BIT,
			  EGL_NONE};

	ASSERT_EGL_TRUE(eglBindAPI(aBindAPI));

	EGLConfig config;
	EGLint numConfigs;
	ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,attrib_list,&config,1,&numConfigs));
	if (numConfigs <= 0)
		{
		// we just WARN, as we don't know if this is expected yet (if it is, then catch the leaving error)
		WARN_PRINTF1(_L("CreatePixmapSurfaceAndMakeCurrentAndMatchL - Could not find a matching config!"));
		iSgImage.Close();
		CloseSgDriver();
		// Leave with a unique knonwn error code - useful to catch this error in negative tests
		User::Leave(KTestNoMatchingConfig);
		}

	// Create a pixmap surface from the native image
	EGLint EGL_RENDERABLE_TYPE_value = 0;
	ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, config, EGL_RENDERABLE_TYPE, &EGL_RENDERABLE_TYPE_value));
	const EGLint* pixmapAttribs = ((aImageInfo.iPixelFormat == EUidPixelFormatARGB_8888_PRE) && (EGL_RENDERABLE_TYPE_value & EGL_OPENVG_BIT)) ? KPixmapAttribsVgAlphaFormatPre : KPixmapAttribsNone;
	iSurface = eglCreatePixmapSurface(iDisplay, config, &iSgImage, pixmapAttribs);
	ASSERT_EGL_TRUE(iSurface != EGL_NO_SURFACE);

	if (aResourceCloseRule == EResourceCloseSgImageEarly)
		{
		// EGL should have taken its own reference to the SgImage, so it should be able to continue
		// to use the underlying data after this local image has been closed.
		iSgImage.Close();
		}

	if (aResourceCloseRule == EResourceCloseSgDriverAndImageEarly)
		{
		// EGL should have taken its own reference to the SgDriver, so it should be able to continue
		// to use its reference to the image resource after this local reference to the driver has
		// been closed.
		iSgImage.Close();
		CloseSgDriver();
		}

	// Create a context for drawing to/reading from the pixmap surface and make it current
	const EGLint KAttribsListCtxNone[] = { EGL_NONE };
	const EGLint KAttribsListCtxGles2[] = { EGL_CONTEXT_CLIENT_VERSION, aRenderVersionNumber, EGL_NONE };
	const EGLint* attrib_list_ctx = (aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber == 2) ? KAttribsListCtxGles2 : KAttribsListCtxNone; 
	iContext = eglCreateContext(iDisplay, config, EGL_NO_CONTEXT, attrib_list_ctx);
	ASSERT_EGL_TRUE(iContext != EGL_NO_CONTEXT);	
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
	}

/**
Compound operation that constructs an RSgImage and uses it to create a pixmap suface.
It then makes it current to the thread.
@param aConfig The EGL config to be used when creating the pixmap surface
@param aImageInfo Used to create the RSgImage pixmap
@param aCloseNativeImageEarly When ETrue, the RSgImage is closed as soon as the pixmap surface has been
		created.  Otherwise, the RSgImage is left to be destroyed later by some other method
		- e.g at the same time as destroying the surface.
@return Whether it was possible to create an iSgImage for the given aImageInfo
*/
EXPORT_C void CTestEglSession::CreatePixmapSurfaceAndMakeCurrentL(EGLConfig aConfig, const TSgImageInfo& aImageInfo, TResourceCloseRule aResourceCloseRule, EGLenum aBindAPI, TInt aRenderVersionNumber)
	{
	ASSERT_EQUALS(iSurface, EGL_NO_SURFACE);
	ASSERT_EQUALS(iContext, EGL_NO_CONTEXT);

	OpenSgDriverL();
    ASSERT_EQUALS(iSgImage.Create(aImageInfo, NULL, 0), KErrNone);

	ASSERT_EGL_TRUE(eglBindAPI(aBindAPI));

	// Create a pixmap surface from the native image
	EGLint EGL_RENDERABLE_TYPE_value = 0;
	ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, aConfig, EGL_RENDERABLE_TYPE, &EGL_RENDERABLE_TYPE_value));
	const EGLint* pixmapAttribs = ((aImageInfo.iPixelFormat == EUidPixelFormatARGB_8888_PRE) && (EGL_RENDERABLE_TYPE_value & EGL_OPENVG_BIT)) ? KPixmapAttribsVgAlphaFormatPre : KPixmapAttribsNone;
	iSurface = eglCreatePixmapSurface(iDisplay, aConfig, &iSgImage, pixmapAttribs);
	ASSERT_EGL_TRUE(iSurface != EGL_NO_SURFACE);

	if (aResourceCloseRule == EResourceCloseSgImageEarly)
		{
		// EGL should have taken its own reference to the SgImage, so it should be able to continue
		// to use the underlying data after this local image has been closed.
		iSgImage.Close();
		}
	if (aResourceCloseRule == EResourceCloseSgDriverAndImageEarly)
		{
		// EGL should have taken its own reference to the SgDriver, so it should be able to continue
		// to use its reference to the image resource after this local reference to the driver has
		// been closed.
		iSgImage.Close();
		CloseSgDriver();
		}

	// Create a context for drawing to/reading from the pixmap surface and make it current
	const EGLint KAttribsListCtxNone[] = { EGL_NONE };
	const EGLint KAttribsListCtxGles2[] = { EGL_CONTEXT_CLIENT_VERSION, aRenderVersionNumber, EGL_NONE };
	const EGLint* attrib_list_ctx = (aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber == 2) ? KAttribsListCtxGles2 : KAttribsListCtxNone;
	iContext = eglCreateContext(iDisplay, aConfig, EGL_NO_CONTEXT, attrib_list_ctx);
	ASSERT_EGL_TRUE(iContext != EGL_NO_CONTEXT);
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
	}

EXPORT_C TInt CTestEglSession::CreateBitmap(CFbsBitmap* aFbsBitmap, const TSize& aSize, TDisplayMode aDisplayMode)
	{
	//Fist try with CreateHardwareBitmap to check whether we are on hardware
	TInt result = aFbsBitmap->CreateHardwareBitmap(aSize, aDisplayMode, KUidEglTestServer);

	if(result == KErrNotSupported)
		{
		//we are not on hardware
		result = aFbsBitmap->Create(aSize, aDisplayMode);
		}
	return result;
	}

EXPORT_C void CTestEglSession::CreatePixmapSurfaceAndMakeCurrentL(EGLConfig aConfig, const TSize& aSize, TDisplayMode displayMode, EGLenum aBindAPI, TInt aRenderVersionNumber)
	{
	ASSERT_EQUALS(iSurface, EGL_NO_SURFACE);
	ASSERT_EQUALS(iContext, EGL_NO_CONTEXT);

	// Create an empty native CFbsBitmap
	OpenFbsSessionL();
	iFbsBitmap = new(ELeave)CFbsBitmap;
	ASSERT_EQUALS(CreateBitmap(iFbsBitmap, aSize, displayMode), KErrNone);

    const EGLint* attrib_list = NULL;
	if(aBindAPI == EGL_OPENVG_API)
		{
		// no attribs to modify
		}
	else if(aBindAPI == EGL_OPENGL_ES_API)
		{
		// no attribs to modify
		}
	else if(aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber==2)
		{
	    const EGLint KAttribGLES2[] = {EGL_CONTEXT_CLIENT_VERSION, aRenderVersionNumber, EGL_NONE};
		attrib_list=KAttribGLES2;
		}
	else
		{
		ERR_PRINTF1(_L("CreatePixmapSurfaceAndMakeCurrentL - Unkown API requested!"));
		User::Leave(KErrArgument);
		}
	ASSERT_EGL_TRUE(eglBindAPI(aBindAPI));

	// Create a pixmap surface from the native image
	EGLint EGL_RENDERABLE_TYPE_value = 0;
	ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, aConfig, EGL_RENDERABLE_TYPE, &EGL_RENDERABLE_TYPE_value));
	const EGLint* pixmapAttribs = ((displayMode == EColor16MAP) && (EGL_RENDERABLE_TYPE_value & EGL_OPENVG_BIT)) ? KPixmapAttribsVgAlphaFormatPre : KPixmapAttribsNone;
	iSurface = eglCreatePixmapSurface(iDisplay, aConfig, iFbsBitmap, pixmapAttribs);
	ASSERT_EGL_TRUE(iSurface != EGL_NO_SURFACE);

	// Create a context for drawing to/reading from the pixmap surface and make it current
	iContext = eglCreateContext(iDisplay, aConfig, EGL_NO_CONTEXT, attrib_list);
	ASSERT_EGL_TRUE(iContext != EGL_NO_CONTEXT);
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
	}

EXPORT_C void CTestEglSession::CreatePixmapSurfaceAndMakeCurrentAndMatchL(const TSize& aSize, TDisplayMode aDisplayMode, EGLenum aBindAPI, TInt aRenderVersionNumber)
	{
	ASSERT_EQUALS(iSurface, EGL_NO_SURFACE);
	ASSERT_EQUALS(iContext, EGL_NO_CONTEXT);

	OpenFbsSessionL();
	iFbsBitmap = new(ELeave)CFbsBitmap;
	ASSERT_EQUALS(CreateBitmap(iFbsBitmap, aSize, aDisplayMode), KErrNone);

	EGLint renderableType = 0;
	if(aBindAPI == EGL_OPENVG_API)
		{
		renderableType = EGL_OPENVG_BIT;
		}
	else if(aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber==1)
		{
		renderableType = EGL_OPENGL_ES_BIT;
		}
	else if(aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber==2)
		{
		renderableType = EGL_OPENGL_ES2_BIT;
		}
	else
		{
		ERR_PRINTF1(_L("CreatePixmapSurfaceAndMakeCurrentAndMatchL - Unkown API requested!"));
		User::Leave(KErrArgument);
		}
	EGLint attrib_list[] = {
			  EGL_MATCH_NATIVE_PIXMAP,(TInt)iFbsBitmap,
			  EGL_RENDERABLE_TYPE,renderableType,
			  EGL_SURFACE_TYPE,EGL_PIXMAP_BIT,
			  EGL_NONE};

	ASSERT_EGL_TRUE(eglBindAPI(aBindAPI));

	EGLConfig config;
	EGLint numConfigs;
	ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,attrib_list,&config,1,&numConfigs));
	if (numConfigs <= 0)
		{
		// we just WARN, as we don't know if this is expected yet (if it is, then catch the leaving error)
		WARN_PRINTF1(_L("CreatePixmapSurfaceAndMakeCurrentAndMatchL - Could not find a matching config!"));
		iSgImage.Close();
		CloseSgDriver();
		// Leave with a unique knonwn error code - useful to catch this error in negative tests
		User::Leave(KTestNoMatchingConfig);
		}

	// Create a pixmap surface from the native image
	const EGLint* pixmapAttribs = (aDisplayMode == EColor16MAP && aBindAPI == EGL_OPENVG_API) ? KPixmapAttribsVgAlphaFormatPre : KPixmapAttribsNone;
	iSurface = eglCreatePixmapSurface(iDisplay, config, iFbsBitmap, pixmapAttribs);
	ASSERT_EGL_TRUE(iSurface != EGL_NO_SURFACE);

	// Create a context for drawing to/reading from the pixmap surface and make it current
	const EGLint KAttribsListCtxNone[] = { EGL_NONE };
	const EGLint KAttribsListCtxGles2[] = { EGL_CONTEXT_CLIENT_VERSION, aRenderVersionNumber, EGL_NONE };
	const EGLint* attrib_list_ctx = (aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber == 2) ? KAttribsListCtxGles2 : KAttribsListCtxNone;
	iContext = eglCreateContext(iDisplay, config, EGL_NO_CONTEXT, attrib_list_ctx);
	ASSERT_EGL_TRUE(iContext != EGL_NO_CONTEXT);
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
	}

EXPORT_C void CTestEglSession::CreatePbufferSurfaceAndMakeCurrentL(EGLConfig aConfig, const TSize& aSize, EGLenum aBindAPI, TInt aRenderVersionNumber)
	{
	ASSERT_EQUALS(iSurface, EGL_NO_SURFACE);
	ASSERT_EQUALS(iContext, EGL_NO_CONTEXT);

	ASSERT_EQUALS(iSurface, EGL_NO_SURFACE);
	ASSERT_EQUALS(iContext, EGL_NO_CONTEXT);

	ASSERT_EGL_TRUE(eglBindAPI(aBindAPI));

	//	PBuffer attribs options are:
	//		EGL_WIDTH, EGL_HEIGHT, EGL_LARGEST_PBUFFER,
	//		EGL_TEXTURE_FORMAT, EGL_TEXTURE_TARGET, EGL_MIPMAP_TEXTURE,
	//		EGL_VG_COLORSPACE, and EGL_VG_ALPHA_FORMAT
	// Create a pbuffer surface of the given size
	const EGLint KPbufferAttribs[] = {
			EGL_WIDTH,	aSize.iWidth,
			EGL_HEIGHT,	aSize.iHeight,
			EGL_NONE };
	iSurface = eglCreatePbufferSurface(iDisplay, aConfig, KPbufferAttribs);
	ASSERT_EGL_TRUE(iSurface != EGL_NO_SURFACE);

	// Create a context for drawing to/reading from the pixmap surface and make it current
	const EGLint KAttribsListCtxNone[] = { EGL_NONE };
	const EGLint KAttribsListCtxGles2[] = { EGL_CONTEXT_CLIENT_VERSION, aRenderVersionNumber, EGL_NONE };
	const EGLint* attrib_list_ctx = (aBindAPI == EGL_OPENGL_ES_API && aRenderVersionNumber == 2) ? KAttribsListCtxGles2 : KAttribsListCtxNone;
	iContext = eglCreateContext(iDisplay, aConfig, EGL_NO_CONTEXT, attrib_list_ctx);
	ASSERT_EGL_TRUE(iContext != EGL_NO_CONTEXT);
	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, iSurface, iSurface, iContext));
	}

EXPORT_C void CTestEglSession::DrawOpenVgL()
	{
	ASSERT_TRUE(iSurface != EGL_NO_SURFACE);
	ASSERT_TRUE(iContext != EGL_NO_CONTEXT);

	// Draw to the pixmap surface
	// Clear it: redish
	VGfloat colorBackground[4];
	ConvertColor(KRgbReddish, colorBackground);
	vgSetfv(VG_CLEAR_COLOR, 4, colorBackground);
	vgClear(0, 0, KPixmapSize.iWidth, KPixmapSize.iHeight);

	// And add a square: greenish
	VGfloat colorSquare[4];
	ConvertColor(KRgbGreenish, colorSquare);
	vgSetfv(VG_CLEAR_COLOR, 4, colorSquare);
	vgClear(KPixmapSquare.iTl.iX, KPixmapSquare.iTl.iX, KPixmapSquare.Width(), KPixmapSquare.Height());
	}

EXPORT_C void CTestEglSession::DrawOpenGLesL()
	{
	ASSERT_TRUE(iSurface != EGL_NO_SURFACE);
	ASSERT_TRUE(iContext != EGL_NO_CONTEXT);
	//GreenishRGB 0x46, 0xDC, 0x78
	const GLfloat glRed = KRgbReddish.Red();
	const GLfloat glGreen = KRgbReddish.Green();
	const GLfloat glBlue  = KRgbReddish.Blue();
	const GLfloat glAlpha = KRgbReddish.Alpha();
	const GLfloat glRedBits = 255.f;
	const GLfloat glGreenBits = 255.f;
	const GLfloat glBlueBits  = 255.f;
	const GLfloat glAlphaBits = 255.f;

	// Disable  cdither - when using exact comparison to reference bitmap
	// because reference bitmap cannot be created to match dither exactly
	glDisable(GL_DITHER);
	// Clear the surface to the colour specified
	glClearColor((glRed)/glRedBits, (glGreen)/glGreenBits, (glBlue)/glBlueBits, glAlpha/glAlphaBits);

	//glColor3f(KRgbGreenish.Red(),KRgbGreenish.Green(),KRgbGreenish.Blue());
	//glRectf(KPixmapSquare.iTl.iX, KPixmapSquare.iTl.iY,KPixmapSquare.iTl.iX + KPixmapSquare.Width(),KPixmapSquare.iTl.iY + KPixmapSquare.Height());

	glClear(GL_COLOR_BUFFER_BIT);
	}

EXPORT_C void CTestEglSession::CheckImageDataL(CFbsBitmap* aFbsBitmap)
	{
	// Use native API's to check pixel values in the surface - note that this is not an OpenVG test, so no boundary pixels are tested
	TRgb rgbPixelSample;
	// Outside the square
	aFbsBitmap->GetPixel(rgbPixelSample, TPoint(10,10));
	ASSERT_TRUE(PixelsMatch(KRgbReddish, rgbPixelSample, EFalse));
	aFbsBitmap->GetPixel(rgbPixelSample, TPoint(90,90));
	ASSERT_TRUE(PixelsMatch(KRgbReddish, rgbPixelSample, EFalse));

	// Inside the square
	aFbsBitmap->GetPixel(rgbPixelSample, TPoint(30,30));
	ASSERT_TRUE(PixelsMatch(KRgbGreenish, rgbPixelSample, EFalse));
	aFbsBitmap->GetPixel(rgbPixelSample, TPoint(70,70));
	ASSERT_TRUE(PixelsMatch(KRgbGreenish, rgbPixelSample, EFalse));
	}

EXPORT_C void CTestEglSession::CheckImageDataFullRedishL(CFbsBitmap* aFbsBitmap)
	{
	// Use native API's to check pixel values in the surface - note that this is not an OpenVG test, so no boundary pixels are tested
	TRgb rgbPixelSample;
	// Outside the square
	aFbsBitmap->GetPixel(rgbPixelSample, TPoint(10,10));
	ASSERT_TRUE(PixelsMatch(KRgbReddish, rgbPixelSample, EFalse));
	aFbsBitmap->GetPixel(rgbPixelSample, TPoint(90,90));
	ASSERT_TRUE(PixelsMatch(KRgbReddish, rgbPixelSample, EFalse));

	// Inside the square
	aFbsBitmap->GetPixel(rgbPixelSample, TPoint(30,30));
	ASSERT_TRUE(PixelsMatch(KRgbReddish, rgbPixelSample, EFalse));
	aFbsBitmap->GetPixel(rgbPixelSample, TPoint(70,70));
	ASSERT_TRUE(PixelsMatch(KRgbReddish, rgbPixelSample, EFalse));
	}

/**
Use OpenVG to check pixel values
@param aDataFormat The VG image data format of the current surface
*/
EXPORT_C void CTestEglSession::CheckImageDataVgL(VGImageFormat aDataFormat)
	{
	switch(aDataFormat)
		{
		case VG_sRGB_565:
			{
			TUint16 intPixelSample=0;

			// Outside the square
			vgReadPixels(&intPixelSample, 1, aDataFormat, 10,10, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color64K(intPixelSample), EFalse));
			vgReadPixels(&intPixelSample, 1, aDataFormat, 90,90, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color64K(intPixelSample), EFalse));

			// Inside the square
			vgReadPixels(&intPixelSample, 1, aDataFormat, 30,30, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbGreenish, TRgb::Color64K(intPixelSample), EFalse));
			vgReadPixels(&intPixelSample, 1, aDataFormat, 70,70, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbGreenish, TRgb::Color64K(intPixelSample), EFalse));
			break;
			}
		case VG_sXRGB_8888:
			{
			TUint32 intPixelSample=0;

			// Outside the square
			vgReadPixels(&intPixelSample, 1, aDataFormat, 10,10, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color16MU(intPixelSample), EFalse));
			vgReadPixels(&intPixelSample, 1, aDataFormat, 90,90, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color16MU(intPixelSample), EFalse));

			// Inside the square
			vgReadPixels(&intPixelSample, 1, aDataFormat, 30,30, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbGreenish, TRgb::Color16MU(intPixelSample), EFalse));
			vgReadPixels(&intPixelSample, 1, aDataFormat, 70,70, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbGreenish, TRgb::Color16MU(intPixelSample), EFalse));
			break;
			}
		case VG_sARGB_8888:
			{
			TUint32 intPixelSample=0;

			// Outside the square
			vgReadPixels(&intPixelSample, 1, aDataFormat, 10,10, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color16MA(intPixelSample), EFalse));
			vgReadPixels(&intPixelSample, 1, aDataFormat, 90,90, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color16MA(intPixelSample), EFalse));

			// Inside the square
			vgReadPixels(&intPixelSample, 1, aDataFormat, 30,30, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbGreenish, TRgb::Color16MA(intPixelSample), EFalse));
			vgReadPixels(&intPixelSample, 1, aDataFormat, 70,70, 1,1);
			ASSERT_TRUE(PixelsMatch(KRgbGreenish, TRgb::Color16MA(intPixelSample), EFalse));
			break;
			}
		default:
			WARN_PRINTF2(_L("Unexpected image data format %d in CTestEglSession::CheckImageDataVgL"), aDataFormat);
			ASSERT_TRUE(EFalse);
		}
	}

/**
Use OpenGLes to check pixel values
@param aDataFormat The VG image data format of the current surface
*/
EXPORT_C void CTestEglSession::CheckImageDataGLesL()
	{
	TUint32 intPixelSample;
	glReadPixels(10,10,1,1,GL_RGBA,GL_UNSIGNED_BYTE ,&intPixelSample);
	SwapChannels(intPixelSample);
	ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color16MU(intPixelSample), EFalse));

	glReadPixels(45,45,1,1,GL_RGBA,GL_UNSIGNED_BYTE ,&intPixelSample);
	SwapChannels(intPixelSample);
	ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color16MU(intPixelSample), EFalse));

	glReadPixels(55,55,1,1,GL_RGBA,GL_UNSIGNED_BYTE ,&intPixelSample);
	SwapChannels(intPixelSample);
	ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color16MU(intPixelSample), EFalse));

	glReadPixels(99,99,1,1,GL_RGBA,GL_UNSIGNED_BYTE ,&intPixelSample);
	SwapChannels(intPixelSample);
	ASSERT_TRUE(PixelsMatch(KRgbReddish, TRgb::Color16MU(intPixelSample), EFalse));

	}

EXPORT_C void CTestEglSession::SwapChannels(TUint32 &aSwapMe)
	{
	TUint32 buff = aSwapMe;

	aSwapMe = 0;
	aSwapMe |= (buff & 0x000000FF) << 16;
	aSwapMe |= (buff & 0x0000FF00);
	aSwapMe |= (buff & 0x00FF0000) >> 16;
	aSwapMe |= (buff & 0xFF000000);
	}

/**
Destroys the surface and underlying RSgImage object.
Nulls the current context.
*/
EXPORT_C void CTestEglSession::CleanupSurfaceSgImageL()
	{
	CleanupSurfaceAndContextL();

	iSgImage.Close();
	}

/**
Resets the internal surface and context handles without destroying them.
Also closes the internal RSgImage handled that is associated to the surface.

EGL destroys all surface and context handles associated with a display when
eglTerminate() is called.
*/
EXPORT_C void CTestEglSession::ResetSurfaceAndContextSgImageL()
	{
	INFO_PRINTF1(_L("CTestEglSession::ResetSurfaceAndContextSgImageL()"));

	iContext = EGL_NO_CONTEXT;
	iSurface = EGL_NO_SURFACE;
	iSgImage.Close();
	}

/**
 Check that the pixel values match within a predefined tolerance
 @param aExpected the expected pixel colour vaule
 @param aActual the actual pixel colour value
 @param aCheckAlpha Whether to check the alpha channel value
 @return Whether the pixel values match - within the allowed tolerance
 */
EXPORT_C TBool CTestEglSession::PixelsMatch(const TRgb& aExpected, const TRgb& aActual, TBool aCheckAlpha)
	{
	// This value has been carefully selected. If it is too high it could be hidding genuine pixel value
	//	differences, while too low will be too strict to allow for pixel conversions, i.e. from 16bpp to 32bpp
	const TInt KPixelTolerance = 8;

	if (aCheckAlpha && aExpected.Alpha()== 0 &&
		aActual.Red() == 0 &&
		aActual.Green() == 0 &&
		aActual.Blue() == 0 &&
		aActual.Alpha() == 0)
		{
		// if the expected value for alpha is 0, all actual values should be 0
		return ETrue;
		}
	else if (Abs(aExpected.Red() - aActual.Red()) > KPixelTolerance ||
			 Abs(aExpected.Green() - aActual.Green()) > KPixelTolerance ||
			 Abs(aExpected.Blue() - aActual.Blue()) > KPixelTolerance ||
			 aCheckAlpha && Abs(aExpected.Alpha() - aActual.Alpha()) > KPixelTolerance)
		{
		// one or more of the actual values differ by more than the allowed tolerance
		ERR_PRINTF6(_L("thread %d: Expected r:%d g:%d b:%d a:%d"), iThreadIdx, aExpected.Red(), aExpected.Green(), aExpected.Blue(), aExpected.Alpha());
		ERR_PRINTF6(_L("thread %d: Actual r:%d g:%d b:%d a:%d"), iThreadIdx, aActual.Red(), aActual.Green(), aActual.Blue(), aActual.Alpha());
		ERR_PRINTF6(_L("thread %d: diff r:%d g:%d b:%d a:%d"), iThreadIdx, Abs(aExpected.Red() - aActual.Red()), Abs(aExpected.Green() - aActual.Green()), Abs(aExpected.Blue() - aActual.Blue()), Abs(aExpected.Alpha() - aActual.Alpha()));
		return EFalse;
		}
	return ETrue;
	}

EXPORT_C void CTestEglSession::InitializeL(TBool aTerminateDisplay)
	{
	// Initialize display object
	INFO_PRINTF2(_L("thread %d: Calling eglInitialize..."), iThreadIdx);
	EGLint major=0;
	EGLint minor=0;
	CHECK_EXPECTED_ERROR(eglInitialize(iDisplay, &major, &minor));
	// Remember if the user wants us to terminate the display.
	iTerminateDisplay = aTerminateDisplay;

	INFO_PRINTF4(_L("thread %d: Initialised: EGL Version %d.%d"), iThreadIdx, major, minor);
	}


EXPORT_C void CTestEglSession::TerminateDisplayL()
	{
	if (iSurface != EGL_NO_SURFACE)
		{
		WARN_PRINTF2(_L("thread %d: iSurface has not been destoroyed in TerminateDisplayL..."), iThreadIdx);
		}
	if (iContext != EGL_NO_CONTEXT)
		{
		WARN_PRINTF2(_L("thread %d: iContext has not been destoroyed in TerminateDisplayL..."), iThreadIdx);
		}
	if (iDisplay != EGL_NO_DISPLAY)
		{
		INFO_PRINTF2(_L("thread %d: Calling eglMakeCurrent(NULL values) from TerminateDisplayL..."), iThreadIdx);

		ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));

		INFO_PRINTF1(_L("Calling eglTerminate..."));
		ASSERT_EGL_TRUE(eglTerminate(iDisplay));
		iDisplay = EGL_NO_DISPLAY;
		}
	iTerminateDisplay = EFalse;
	}

EXPORT_C void CTestEglSession::OpenSgDriverL()
	{
	if (!iSgDriverOpen)
		{
		VERBOSE_INFO_PRINTF2(_L("CTestEglSession<0x%08x> Opening SgDriver"), this);
#ifdef  SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		TInt ret=iSgDriver.Open();
#else
		TInt ret=SgDriver::Open();
#endif	//SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		ASSERT_EQUALS(ret, KErrNone);
		iSgDriverOpen = ETrue;
		}
	}

EXPORT_C void CTestEglSession::CloseSgDriver()
	{
	if (iSgDriverOpen)
		{
		VERBOSE_INFO_PRINTF2(_L("CTestEglSession<0x%08x> Closing SgDriver"), this);
#ifdef  SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
        iSgDriver.Close();
#else
        SgDriver::Close();
#endif  //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		iSgDriverOpen = EFalse;
		}
	}

EXPORT_C void CTestEglSession::OpenFbsSessionL()
	{
	if (!iFbsSessionOpen)
		{
		INFO_PRINTF2(_L("CTestEglSession<0x%08x> Opening FbsSession"), this);
		TInt ret=RFbsSession::Connect();
		ASSERT_EQUALS(ret, KErrNone);
		iFbsSessionOpen = ETrue;
		}
	}

EXPORT_C void CTestEglSession::CloseFbsSession()
	{
	if (iFbsSessionOpen)
		{
		INFO_PRINTF2(_L("CTestEglSession<0x%08x> Closing FbsSession"), this);
		RFbsSession::Disconnect();
		iFbsSessionOpen = EFalse;
		}
	}

EXPORT_C TBool CTestEglSession::FetchProcEglCreateImageKhr()
	{
	if (!ipfnEglCreateImageKHR)
		{
		INFO_PRINTF2(_L("thread %d: Calling eglGetProcAddress(\"eglCreateImageKHR\")"), iThreadIdx);
		ipfnEglCreateImageKHR = reinterpret_cast<TFPtrEglCreateImageKhr>(eglGetProcAddress("eglCreateImageKHR"));
		if (ipfnEglCreateImageKHR==NULL)
			{
			EGLint eglError = eglGetError();
			WARN_PRINTF2(_L("eglCreateImageKHR() not found - EGL Error: 0x%x"), eglError);
			}
		}
	return (ipfnEglCreateImageKHR!=NULL);
	}

EXPORT_C EGLImageKHR CTestEglSession::eglCreateImageKhrL(EGLDisplay aDisplay,EGLContext aContext,EGLenum aTarget,RSgImage* aSgImage,const EGLint *aAttr_List)
	{
	TBool bSuccess = FetchProcEglCreateImageKhr();
	ASSERT_TRUE(bSuccess);
	VERBOSE_INFO_PRINTF2(_L("thread %d: Calling eglCreateImageKHR"), iThreadIdx);
	EGLImageKHR eglImage = ipfnEglCreateImageKHR(aDisplay,aContext,aTarget,reinterpret_cast<EGLClientBuffer>(aSgImage),const_cast<EGLint *>(aAttr_List));
	return eglImage;
	}

EXPORT_C EGLImageKHR CTestEglSession::eglCreateImageKhrL(EGLDisplay aDisplay,EGLContext aContext,EGLenum aTarget,CFbsBitmap &aCFbsBitmap, const EGLint *aAttr_List)
	{
	TBool bSuccess = FetchProcEglCreateImageKhr();
	ASSERT_TRUE(bSuccess);
	INFO_PRINTF2(_L("thread %d: Calling eglCreateImageKHR, with CFBsBitmap)"), iThreadIdx);
	//the following call to eglCreateImageKHR MUST fail, error handling is made outside
	EGLImageKHR eglImage = ipfnEglCreateImageKHR(aDisplay,aContext,aTarget,reinterpret_cast<EGLClientBuffer>(&aCFbsBitmap),const_cast<EGLint *>(aAttr_List));
	return eglImage;
	}

EXPORT_C TBool CTestEglSession::FetchProcEglDestroyImageKhr()
	{
	if (!ipfnEglDestroyImageKHR)
		{
		INFO_PRINTF2(_L("thread %d: Calling eglGetProcAddress(\"eglDestroyImageKHR\")"), iThreadIdx);
		ipfnEglDestroyImageKHR = reinterpret_cast<TFPtrEglDestroyImageKhr>(eglGetProcAddress("eglDestroyImageKHR"));
		if (ipfnEglDestroyImageKHR==NULL)
			{
			EGLint eglError = eglGetError();
			WARN_PRINTF2(_L("eglDestroyImageKHR() not found - EGL Error: 0x%x"), eglError);
			}
		}
	return (ipfnEglDestroyImageKHR!=NULL);
	}

EXPORT_C TBool CTestEglSession::DestroyEGLImage(EGLDisplay aDisplay, EGLImageKHR aEGLImageKHR)
	{
	TBool bSuccess = FetchProcEglDestroyImageKhr();
	ASSERT_TRUE(bSuccess);
	VERBOSE_INFO_PRINTF2(_L("thread %d: Calling eglDestroyImageKHR"), iThreadIdx);
	return ipfnEglDestroyImageKHR(aDisplay,aEGLImageKHR);
	}

EXPORT_C TBool CTestEglSession::FetchProcvgCreateImageTargetKhr()
	{
	if (!ipfnvgCreateImageTargetKHR)
		{
		INFO_PRINTF2(_L("thread %d: Calling eglGetProcAddress (\"vgCreateEGLImageTargetKHR\")"), iThreadIdx);
		ipfnvgCreateImageTargetKHR = reinterpret_cast<TFPtrVgCreateEglImageTargetKhr>(eglGetProcAddress("vgCreateEGLImageTargetKHR"));
		if (ipfnvgCreateImageTargetKHR==NULL)
			{
			EGLint eglError = eglGetError();
			WARN_PRINTF2(_L("vgCreateImageTargetKHR() not found - EGL Error: 0x%x"), eglError);
			}
		}
	return (ipfnvgCreateImageTargetKHR!=NULL);
	}

EXPORT_C VGImage CTestEglSession::vgCreateImageTargetKHR(VGeglImageKHR aImage)
	{
	TBool bSuccess = FetchProcvgCreateImageTargetKhr();
	ASSERT_TRUE(bSuccess);
	VERBOSE_INFO_PRINTF2(_L("thread %d: Calling vgCreateEGLImageTargetKHR"), iThreadIdx);
	return ipfnvgCreateImageTargetKHR(aImage);
	}

EXPORT_C TBool CTestEglSession::IsACompatibleConfig(EGLConfig aConfig,RSgImage& aImage,TBool aLog)
	{
	EGLint EGL_BUFFER_SIZE_value;
	EGLint EGL_ALPHA_SIZE_value;
	EGLint EGL_BLUE_SIZE_value;
	EGLint EGL_GREEN_SIZE_value;
	EGLint EGL_RED_SIZE_value;
	EGLint EGL_SURFACE_TYPE_value;
	EGLint EGL_RENDERABLE_TYPE_value;
	EGLint EGL_CONFIG_ID_value;

	eglGetConfigAttrib(iDisplay, aConfig, EGL_CONFIG_ID, &EGL_CONFIG_ID_value);
	eglGetConfigAttrib(iDisplay, aConfig, EGL_BUFFER_SIZE, &EGL_BUFFER_SIZE_value);
	eglGetConfigAttrib(iDisplay, aConfig, EGL_ALPHA_SIZE, &EGL_ALPHA_SIZE_value);
	eglGetConfigAttrib(iDisplay, aConfig, EGL_BLUE_SIZE, &EGL_BLUE_SIZE_value);
	eglGetConfigAttrib(iDisplay, aConfig, EGL_GREEN_SIZE, &EGL_GREEN_SIZE_value);
	eglGetConfigAttrib(iDisplay, aConfig, EGL_RED_SIZE, &EGL_RED_SIZE_value);
	eglGetConfigAttrib(iDisplay, aConfig, EGL_SURFACE_TYPE, &EGL_SURFACE_TYPE_value);
	eglGetConfigAttrib(iDisplay, aConfig, EGL_RENDERABLE_TYPE, &EGL_RENDERABLE_TYPE_value);
#ifdef PRINTG_CONFIGS_LOG
	INFO_PRINTF7(_L("(Config: %3d) RGBA=(%2d) %2d,%2d,%2d,%2d"), EGL_CONFIG_ID_value, EGL_BUFFER_SIZE_value,
					EGL_RED_SIZE_value, EGL_GREEN_SIZE_value, EGL_BLUE_SIZE_value,
					EGL_ALPHA_SIZE_value);
	INFO_PRINTF2(_L("RENDERABLE_TYPE %d"),EGL_RENDERABLE_TYPE_value);
#endif

	if(!(EGL_SURFACE_TYPE_value & EGL_PIXMAP_BIT))
		{
		return EFalse;
		}

	TBool good = ETrue;
	//requested usage bits
	TSgImageInfo requestedImageInfo;
	aImage.GetInfo(requestedImageInfo);

#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	//potential usage bits
    RSgImage potential;
    potential.Open(aImage.Id(),ESgDoNotRestrictUsage);
    TSgImageInfo potentialImageInfo;
    potential.GetInfo(potentialImageInfo);
    potential.Close();
#endif

	switch(requestedImageInfo.iPixelFormat)
		{
		case EUidPixelFormatRGB_565:
			if (!(EGL_RED_SIZE_value == 5 && EGL_GREEN_SIZE_value == 6 && EGL_BLUE_SIZE_value == 5 && EGL_ALPHA_SIZE_value == 0))
				{
				good = EFalse;
				}
			break;
		case EUidPixelFormatXRGB_8888:
			if (!(EGL_RED_SIZE_value == 8 && EGL_GREEN_SIZE_value == 8 && EGL_BLUE_SIZE_value == 8 && EGL_ALPHA_SIZE_value == 0))
				{
				good = EFalse;
				}
			break;
		case EUidPixelFormatARGB_8888_PRE:
			if (!(EGL_RED_SIZE_value == 8 && EGL_GREEN_SIZE_value == 8 && EGL_BLUE_SIZE_value == 8 && EGL_ALPHA_SIZE_value == 8))
				{
				good = EFalse;
				}
			//just OVG cares about the premultiplied alpha
			if(EGL_RENDERABLE_TYPE_value& EGL_OPENVG_BIT)
			//if(requestedImageInfo.iUsage & ESgUsageBitOpenVgSurface)
				{
				if(!(EGL_SURFACE_TYPE_value & EGL_VG_ALPHA_FORMAT_PRE_BIT))
					{
					if(aLog)
						{
						WARN_PRINTF2(_L("This Config can't be used with Pre-multipliedAlpha becasue EGL_SURFACE_TYPE has the wrong bits = %x"), EGL_SURFACE_TYPE_value);
						}
					good = EFalse;
					}
				}
			break;
		default:
			if(aLog)
				{
				ERR_PRINTF2(_L("Wrong PixelFormat for a target, %x"), requestedImageInfo.iPixelFormat);
				}
			good = EFalse;
		}
	if(!good)
		{
		return EFalse;
		}

#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	//
	//Check if strict matching requirements are met:
	//- All the supported APIs included in the EGL_RENDERABLE_TYPE of the
	// EGLConfig must be matched by the corresponding iUsage of RSgImage
	//- The following usages included in the iUsage of the RSgImage must be
	// matched by the corresponding API in EGL_RENDERABLE_TYPE of the EGLConfig:
	//         ESgUsageBitOpenGlSurface-> EGL_OPENGL_BIT
	//         ESgUsageBitOpenGlesSurface-> EGL_OPENGL_ES_BIT
	//         ESgUsageBitOpenGles2Surface-> EGL_OPENGL_ES2_BIT
	//         ESgUsageBitOpenVgSurface->EGL_OPENVG_BIT

	EGLint usageBitsMask = 0;
	if(requestedImageInfo.iUsage & ESgUsageBitOpenVgSurface)
		{
		usageBitsMask |= EGL_OPENVG_BIT;
		}
	if(requestedImageInfo.iUsage & ESgUsageBitOpenGles2Surface)
		{
		usageBitsMask |= EGL_OPENGL_ES2_BIT;
		}
    if(requestedImageInfo.iUsage & ESgUsageBitOpenGlesSurface)
        {
        usageBitsMask |= EGL_OPENGL_ES_BIT;
        }
    if(requestedImageInfo.iUsage & ESgUsageBitOpenGlSurface)
        {
        usageBitsMask |= EGL_OPENGL_BIT;
        }

	if(usageBitsMask  != EGL_RENDERABLE_TYPE_value)
		{
		return EFalse;
		}
#else
    // requested usage & RENDERABLE_TYPE > 0
    EGLint usageBitsMask = 0;
    if(requestedImageInfo.iUsage & ESgUsageOpenVgTarget)
        {
        usageBitsMask |= EGL_OPENVG_BIT;
        }
    if(requestedImageInfo.iUsage & ESgUsageOpenGlesTarget)
        {
        usageBitsMask |= EGL_OPENGL_ES_BIT;
        }
    if(requestedImageInfo.iUsage & ESgUsageOpenGles2Target)
        {
        usageBitsMask |= EGL_OPENGL_ES2_BIT;
        }
    if(!(usageBitsMask & EGL_RENDERABLE_TYPE_value))
        {
        return EFalse;
        }

    // potential usage >= RENDERABLE_TYPE

    EGLint potentialUsageBitsMask = 0;
    if(potentialImageInfo.iUsage & ESgUsageOpenVgTarget)
        {
        potentialUsageBitsMask |= EGL_OPENVG_BIT;
        }
    if(potentialImageInfo.iUsage & ESgUsageOpenGlesTarget)
        {
        potentialUsageBitsMask |= EGL_OPENGL_ES_BIT;
        }
    if(potentialImageInfo.iUsage & ESgUsageOpenGles2Target)
        {
        potentialUsageBitsMask |= EGL_OPENGL_ES2_BIT;
        }

    potentialUsageBitsMask = EGL_RENDERABLE_TYPE_value &~ potentialUsageBitsMask;
    if(potentialUsageBitsMask)
        {
        return EFalse;
        }
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
	return good;
	}

EXPORT_C TBool  CTestEglSession::CheckNeededExtensionL(TInt aExtension, const TDesC& aExtensionName)
	{
	TBuf8<128> extensionName;
	extensionName.Copy(aExtensionName);
	if(aExtension & KEGL_RSgimage || extensionName.Compare(KEglRSgImage)==0)
		{
		TBool bFoundExtensionEGL_NOK_pixmap_type_rsgimage = FindExtensionStringL(EIsEGL,KEglRSgImage);
		if (!bFoundExtensionEGL_NOK_pixmap_type_rsgimage)
			{
			// The extension is not supported
			return EFalse;
			}
		}
	if(aExtension & KEGL_KHR_image_base || extensionName.Compare(KEglKhrImageBase)==0)
		{
		TBool bFoundExtensionEGL_KHR_image = FindExtensionStringL(EIsEGL,KEglKhrImageBase);
		if (!bFoundExtensionEGL_KHR_image)
			{
			// The extension is not supported
			return EFalse;
			}
		}
	if(aExtension & KEGL_KHR_image_pixmap || extensionName.Compare(KEglKhrImagePixmap)==0)
		{
		TBool bFoundExtensionEGL_KHR_image = FindExtensionStringL(EIsEGL,KEglKhrImagePixmap);
		if (!bFoundExtensionEGL_KHR_image)
			{
			// The extension is not supported
			return EFalse;
			}
		}
	if(aExtension & KVG_KHR_EGL_image || extensionName.Compare(KVgKhrEglImage)==0)
		{
		TBool bFoundExtensionVG_KHR_EGL_image = FindExtensionStringL(EIsVG,KVgKhrEglImage);
		if (!bFoundExtensionVG_KHR_EGL_image)
			{
			// The extension is not supported
			return EFalse;
			}
		}
	if(aExtension & KEGL_KHR_reusable_sync || extensionName.Compare(KEglKhrReusableSync)==0)
		{
		TBool bFoundExtensionEGL_KHR_reusable_sync = FindExtensionStringL(EIsEGL,KEglKhrReusableSync);
		if (!bFoundExtensionEGL_KHR_reusable_sync)
			{
			// The extension is not supported
			return EFalse;
			}
		}
    if(aExtension & KEGL_NOK__private__signal_sync || extensionName.Compare(KEglNokPrivateSignalSync)==0)
        {
        TBool bFoundExtensionEGL_NOK__private__signal_sync = FindExtensionStringL(EIsEGL,KEglNokPrivateSignalSync);
        if (!bFoundExtensionEGL_NOK__private__signal_sync)
            {
            // The extension is not supported
            return EFalse;
            }
        }
    if(aExtension & KEGL_NOKIA_swap_buffers || extensionName.Compare(KEglNokiaSwapBuffers)==0)
        {
        TBool bFoundExtensionEGL_NOKIA_swap_buffer = FindExtensionStringL(EIsEGL,KEglNokiaSwapBuffers);
        if (!bFoundExtensionEGL_NOKIA_swap_buffer)
            {
            // The extension is not supported
            return EFalse;
            }
        }
#ifndef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    if(aExtension & KEGL_SYMBIAN_image_preserved || extensionName.Compare(KEglSymbianImagePreserved)
        {
        TBool bFoundExtensionEGL_SYMBIAN_image_preserved = FindExtensionStringL(EIsEGL,KEglSymbianImagePreserved);
        if (!bFoundExtensionEGL_SYMBIAN_image_preserved)
            {
            // The extension is not supported
            return EFalse;
            }
        }
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    if(aExtension & KEGL_NOK_image_endpoint || extensionName.Compare(KEglNokiaImageEndpoint)==0)
        {
        TBool bFoundExtensionEGL_NOK_image_endpoint = FindExtensionStringL(EIsEGL,KEglNokiaImageEndpoint);
        if (!bFoundExtensionEGL_NOK_image_endpoint)
            {
            // The extension is not supported
            return EFalse;
            }
        }
    if(aExtension & KEGL_NOK_surface_scaling || extensionName.Compare(KEglNokiaSurfaceScaling)==0)
        {
        TBool bFoundExtensionEGL_NOK_surface_scaling = FindExtensionStringL(EIsEGL,KEglNokiaSurfaceScaling);
        if (!bFoundExtensionEGL_NOK_surface_scaling)
            {
            // The extension is not supported
            return EFalse;
            }
        }
	return ETrue;
	}

EXPORT_C CFbsBitmap* CTestEglSession::CreateReferenceBitmapL(TDisplayMode aMode)
	{
	if(aMode == ENone )
		{
		ERR_PRINTF1(_L("Queried Reference Bitmap dispaly mode equal to ENone"));
		User::Leave(KErrTEFUnitFail);
		}
	return CreateReferenceBitmapL(aMode,KRgbGreenish);
	}

EXPORT_C CFbsBitmap* CTestEglSession::CreateReferenceBitmapL(TDisplayMode aMode,const TRgb& aColour)
	{
	OpenFbsSessionL();
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	if(bitmap->Create(KPixmapSize,aMode) != KErrNone)
		{
		return NULL;
		}
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(bitmapDevice);
	CFbsBitGc* fbsBitGc = CFbsBitGc::NewL();
	CleanupStack::PushL(fbsBitGc);
	fbsBitGc->Activate(bitmapDevice);
	fbsBitGc->SetBrushColor(aColour);
	fbsBitGc->Clear();
	CleanupStack::PopAndDestroy(2,bitmapDevice);
	CleanupStack::Pop(bitmap);
	return bitmap;
	}

EXPORT_C CFbsBitmap* CTestEglSession::CreateReferenceMaskedBitmapL(TDisplayMode aRefBitmapMode, const TRgb& aPenBitmapColor, const CFbsBitmap* aMaskBitmap)
	{
	OpenFbsSessionL();

	// create the refBitmat (same size as the mask bitmap)
	// Note that we clear it to 'opaque black' as we assume the target surface has been cleared to 'opaque black' too
	// If either the surface or the refBitmap are cleared to another colour, update the other accordingly
	CFbsBitmap* refBitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(refBitmap);
	User::LeaveIfError(refBitmap->Create(aMaskBitmap->SizeInPixels(), aRefBitmapMode));
	CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(refBitmap);
	CleanupStack::PushL(bitmapDevice);
	CFbsBitGc* fbsBitGc = CFbsBitGc::NewL();
	CleanupStack::PushL(fbsBitGc);
	fbsBitGc->Activate(bitmapDevice);
	fbsBitGc->SetBrushColor(KRgbBlack);
	fbsBitGc->Clear();

	// create the penBitmap (same size as the mask bitmap)
	CFbsBitmap* penBitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(penBitmap);
	User::LeaveIfError(penBitmap->Create(aMaskBitmap->SizeInPixels(), aRefBitmapMode));
	CFbsBitmapDevice* penbitmapDevice = CFbsBitmapDevice::NewL(penBitmap);
	CleanupStack::PushL(penbitmapDevice);
	CFbsBitGc* penBitGc = CFbsBitGc::NewL();
	CleanupStack::PushL(penBitGc);
	penBitGc->Activate(penbitmapDevice);
	penBitGc->SetBrushColor(aPenBitmapColor);
	penBitGc->Clear();

	// perform a masked bitmap transfer to the active refBitmap
	TRect bmpRect(TPoint(0, 0), refBitmap->SizeInPixels());
	fbsBitGc->Activate(bitmapDevice);
	fbsBitGc->BitBltMasked(TPoint(0, 0), penBitmap, bmpRect, aMaskBitmap, EFalse);

	CleanupStack::PopAndDestroy(5, bitmapDevice);
	CleanupStack::Pop(refBitmap);
	return refBitmap;
	}


EXPORT_C CFbsBitmap* CTestEglSession::CreateReferenceBitmapL(TDisplayMode aMode, const TSize &aSize, const TInt aIndex)
	{
	OpenFbsSessionL();
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(aSize,aMode));

	TInt height = bitmap->SizeInPixels().iHeight;
	TInt width  = bitmap->SizeInPixels().iWidth;

	// Initialise colour values to a random value (guarantees pixel uniqueness if update is done accordingly)
	TInt red=0;
	TInt green=127;
	TInt blue=127;
	TInt alpha=255;

	TBitmapUtil bmpUtil(bitmap);
	bmpUtil.Begin(TPoint(0,0));
	for (TInt colIndex = 0; colIndex < width; ++colIndex)
		{
		bmpUtil.SetPos(TPoint(colIndex, 0));
		for (TInt rowIndex =0; rowIndex < height; ++rowIndex)
			{
			TRgb rgb(red, green, blue, alpha);
			switch(bitmap->DisplayMode())
				{
				case EColor64K:
					{
					bmpUtil.SetPixel(rgb.Color64K());
					break;
					}
				case EColor16MU:
					{
					bmpUtil.SetPixel(rgb.Color16MU());
					break;
					}
				case EColor16MA:
					{
					bmpUtil.SetPixel(rgb.Color16MA());
					break;
					}
				case EColor16MAP:
					{
					bmpUtil.SetPixel(rgb.Color16MAP());
					break;
					}
				case EGray256:
					{
					bmpUtil.SetPixel(rgb.Gray256());
					break;
					}
				default:
					{
					// We should not get here - colour mode not supported by these tests
					ERR_PRINTF1(_L("CTestEglSession::CreateReferenceBitmapL - Colour mode not supported!"));
				    ASSERT(FALSE);
					}
				}
			bmpUtil.IncYPos();

			// Update red bit
			red = ++red + aIndex;
			if (red>255)
				red = red - 256;

			// Update green bit
			green = --green - aIndex;
			if (green<0)
				green = green + 256;

			// Update blue bit
			blue = ++blue + aIndex;
			if (blue>255)
				blue = blue - 256;

			// Update alpha bit
			alpha = --alpha - aIndex;
			if (alpha<0)
				alpha = alpha + 256;
			}
		}
	bmpUtil.End();
	CleanupStack::Pop(bitmap);
	return bitmap;
	}

EXPORT_C void CTestEglSession::CheckVgDrawingL(VGImageFormat aDataFormat, const CFbsBitmap* aReferenceBitmap)
	{
	TRgb refPixel;
	TInt height = aReferenceBitmap->SizeInPixels().iHeight;
	TInt width  = aReferenceBitmap->SizeInPixels().iWidth;

	switch(aDataFormat)
		{
		case VG_sRGB_565:
			{
			TUint16* vgPixel = new(ELeave) TUint16[width];
			CleanupArrayDeletePushL(vgPixel);
			for (TInt y=0; y < height; y++)
				{
                // Mind the fact that CFbsBitmap and VGImages use different coordinates origin!
                vgReadPixels(vgPixel, 1, aDataFormat, 0,height-y-1, width,1);

				for (TInt x=0; x < width; x++)
					{
					aReferenceBitmap->GetPixel(refPixel, TPoint(x,y));
					if(!PixelsMatch(refPixel, TRgb::Color64K(vgPixel[x]), EFalse))
						{
						User::Leave(KErrTEFUnitFail);
						}
					}
				}
			CleanupStack::PopAndDestroy(vgPixel);
			break;
			}

		case VG_sXRGB_8888:
			{
			TUint32* vgPixel = new(ELeave) TUint32[width];
			CleanupArrayDeletePushL(vgPixel);
			for (TInt y=0; y < height; y++)
				{
                // Mind the fact that CFbsBitmap and VGImages use different coordinates origin!
                vgReadPixels(vgPixel, 1, aDataFormat, 0,height-y-1, width,1);

				for (TInt x=0; x < width; x++)
					{
					aReferenceBitmap->GetPixel(refPixel, TPoint(x,y));
					if(!PixelsMatch(refPixel, TRgb::Color16MU(vgPixel[x]), EFalse))
						{
						User::Leave(KErrTEFUnitFail);
						}
					}
				}
			CleanupStack::PopAndDestroy(vgPixel);
			break;
			}

		case VG_sARGB_8888:
			{
			TUint32* vgPixel = new(ELeave) TUint32[width];
			CleanupArrayDeletePushL(vgPixel);
			for (TInt y=0; y < height; y++)
				{
                // Mind the fact that CFbsBitmap and VGImages use different coordinates origin!
                vgReadPixels(vgPixel, 1, aDataFormat, 0,height-y-1, width,1);

				for (TInt x=0; x < width; x++)
					{
					aReferenceBitmap->GetPixel(refPixel, TPoint(x,y));
					if(!PixelsMatch(refPixel, TRgb::Color16MA(vgPixel[x]), ETrue))
						{
						User::Leave(KErrTEFUnitFail);
						}
					}
				}
			CleanupStack::PopAndDestroy(vgPixel);
			break;
			}

		case VG_sARGB_8888_PRE:
			{
			TUint32* vgPixel = new(ELeave) TUint32[width];
			CleanupArrayDeletePushL(vgPixel);
			for (TInt y=0; y < height; y++)
				{
                // Mind the fact that CFbsBitmap and VGImages use different coordinates origin!
                vgReadPixels(vgPixel, 1, aDataFormat, 0,height-y-1, width,1);

				for (TInt x=0; x < width; x++)
					{
					aReferenceBitmap->GetPixel(refPixel, TPoint(x,y));
					if(!PixelsMatch(refPixel, TRgb::Color16MAP(vgPixel[x]), ETrue))
						{
						User::Leave(KErrTEFUnitFail);
						}
					}
				}
			CleanupStack::PopAndDestroy(vgPixel);
			break;
			}

		default:
			// We should not get here - colour mode not supported by these tests
			ERR_PRINTF1(_L("CTestEglSession::CheckVgDrawingL - Colour mode not supported!"));
		    ASSERT(FALSE);
			break;
		}
	}

EXPORT_C TBool CTestEglSession::IsOpenGLESSupported()
    {
    if(!iIsSupportedRenderInitialized)
        {
        CheckAllAvailableRenders();
        }
    return iIsOpenGLESSupported;
    }

EXPORT_C TBool CTestEglSession::IsOpenGLES2Supported()
    {
    if(!iIsSupportedRenderInitialized)
        {
        CheckAllAvailableRenders();
        }
    return iIsOpenGLES2Supported;
    }
EXPORT_C TBool CTestEglSession::IsOpenVGSupported()
    {
    if(!iIsSupportedRenderInitialized)
        {
        CheckAllAvailableRenders();
        }
    return iIsOpenVGSupported;
    }

void CTestEglSession::CheckAllAvailableRenders()
    {
    ASSERT_EGL_TRUE(iDisplay != EGL_NO_DISPLAY);
    TPtrC8 ptrEglClientApis((const TText8 *)eglQueryString(iDisplay, EGL_CLIENT_APIS));
    _LIT8(KOpenGLES, "OpenGL_ES");
    EGLint numConfigs= 0;
    EGLConfig config;
    if(ptrEglClientApis.Find(KOpenGLES) != KErrNotFound)
        {
        //check GLES2
        const EGLint KAttrib_list_gles2[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                              EGL_NONE };
        EGLBoolean eglRes = eglChooseConfig(iDisplay, KAttrib_list_gles2, &config,1, &numConfigs);
        ASSERT_EGL_TRUE(eglRes == EGL_TRUE);
        if(numConfigs > 0)
            {
            iIsOpenGLES2Supported = ETrue;
            }
        //check GLES
        numConfigs = 0;
        const EGLint KAttrib_list_gles[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
                                             EGL_NONE };
        eglRes = eglChooseConfig(iDisplay, KAttrib_list_gles, &config,1, &numConfigs);
        ASSERT_EGL_TRUE(eglRes == EGL_TRUE);
        if(numConfigs > 0)
            {
            iIsOpenGLESSupported = ETrue;
            }
        }
    _LIT8(KOpenVG, "OpenVG");
    if(ptrEglClientApis.Find(KOpenVG) != KErrNotFound)
        {
        numConfigs= 0;
        //check VG
        const EGLint KAttrib_list_vg[] = { EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
                                           EGL_NONE };
        EGLBoolean eglRes = eglChooseConfig(iDisplay, KAttrib_list_vg, &config,1, &numConfigs);
        ASSERT_EGL_TRUE(eglRes == EGL_TRUE);
        if(numConfigs > 0)
            {
            iIsOpenVGSupported = ETrue;
            }
        }
    iIsSupportedRenderInitialized = ETrue;
    }


