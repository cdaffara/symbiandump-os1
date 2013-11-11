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
*/

#include "egltest_createpixmapsurface.h"

#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <sgresource/sgimage.h>
#else
#include <graphics/sgimage.h>
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <test/tefunit.h> // for ASSERT macros

#include <test/egltestcommonconversion.h>
#include <test/egltestcommoninisettings.h>
#include <test/egltestcommonsgimageinfo.h>



/**
@SYMTestCaseID GRAPHICS-EGL-0063

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
This test ensures the matching algorithm between EGLConfig and RSgImages when the RSgImage format is EUidPixelFormatARGB_8888_PRE.
The algorithm is tested in both eglChooseConfig and eglCreatePixmap.

@SYMTestActions
Create an RSgImage format with the following parameters:
* format: EUidPixelFormatARGB_8888_PRE
* usage: ESgUsageBitOpenVgSurface
Create second RSgImage format with the following parameters:
* format: EUidPixelFormatXRGB_8888
* usage: ESgUsageBitOpenVgSurface

Query a compatible config via eglChooseConfig.
If we get one compatible config check that is accepted by eglCreatePixmapSurface.
1) call eglCreatePixmap with the just queried config and as attribute:
	* EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_NONPRE
	* as native pixmap format supply RSgImage with premultiply alpha   
This call has to fail and give back a EGL_BAD_MATCH error.
2) call eglCreatePixmapSurface with the just queried config and as attribute:
    * EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_PRE
    * as native pixmap format supply RSgImage with non-premultiply alpha   
This call has to fail and give back a EGL_BAD_MATCH error.
3) call eglCreatePixmapSurface with the just queried config and as attribute:
	* EGL_VG_ALPHA_FORMAT, EGL_VG_ALPHA_FORMAT_PRE
This call has to succed.
No memory or handle leaks.

@SYMTestExpectedResults
The first and second calls to eglCreatePixmapSurface have to fail and give back a EGL_BAD_MATCH error.
The third call has to succed.
No memory or handle leaks.
*/
TVerdict CEglTest_CreatePixmapSurface_Alpha_bit_RSgImage::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0063"));
	INFO_PRINTF1(_L("CEglTest_CreatePixmapSurface_Alpha_bit_RSgImage::doTestPartialStepL"));	

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// Create display object
	GetDisplayL();
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);
	eglSess->InitializeL();
	
	eglSess->OpenSgDriverL();
	TSgImageInfoOpenVgTarget imageInfo = TSgImageInfoOpenVgTarget(EUidPixelFormatARGB_8888_PRE);
    RSgImage sgImageWithPre;
    // Create a SgImage ARGB_8888_PRE
	CleanupClosePushL(sgImageWithPre);
    ret = sgImageWithPre.Create(imageInfo, NULL, NULL);
    ASSERT_EQUALS(ret, KErrNone);
    
	EGLint numConfigsWithPre = 0;		
	EGLConfig configWithPre[KMaxEglConfigs];
	const EGLint KAttrib_list_ImageWithPre[] = { EGL_MATCH_NATIVE_PIXMAP,	(TInt)&sgImageWithPre,
												 EGL_RENDERABLE_TYPE, 		EGL_OPENVG_BIT,
												 EGL_SURFACE_TYPE, 			EGL_PIXMAP_BIT | EGL_VG_ALPHA_FORMAT_PRE_BIT,
												 EGL_NONE };

	INFO_PRINTF1(_L("Calling eglChooseConfig with the EGL_MATCH_NATIVE_PIXMAP flag set"));
	eglChooseConfig(iDisplay,KAttrib_list_ImageWithPre,configWithPre,KMaxEglConfigs,&numConfigsWithPre);
    if(!numConfigsWithPre)
        {
        ERR_PRINTF1(_L("No EGL Config with EGL_VG_ALPHA_FORMAT_PRE available for EUidPixelFormatARGB_8888_PRE"));
        SetTestStepResult(EFail);           
        }

	for(TInt index = 0; index < numConfigsWithPre; index++)
	    {
	    INFO_PRINTF1(_L("Calling eglBindAPI(EGL_OPENVG_API)"));
	    ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));
	
	    INFO_PRINTF1(_L("Calling eglCreatePixmapSurface"));
		eglSess->SetExpectedError(EGL_BAD_MATCH);
		EGLSurface surface = eglCreatePixmapSurface(iDisplay, configWithPre[index],&sgImageWithPre, KPixmapAttribsVgAlphaFormatNonPre);
		if ( !eglSess->CheckExpectedError(EGL_FALSE) )
			{
			ERR_PRINTF1(_L("eglCreatePixmapSurface didn't fail as expected."));
			SetTestStepResult(EFail);			
			if(surface != EGL_NO_SURFACE)
				{
				eglDestroySurface(iDisplay,surface);
				eglSess->CheckExpectedError(EGL_SUCCESS);
				surface = EGL_NO_SURFACE;
				}
			}		
	
		surface = eglCreatePixmapSurface(iDisplay, configWithPre[index],&sgImageWithPre, KPixmapAttribsVgAlphaFormatPre);
		if ( !eglSess->CheckExpectedError(EGL_TRUE) )
			{
			ERR_PRINTF1(_L("eglCreatePixmapSurface failed."));
			SetTestStepResult(EFail);			
			}
		if(surface != EGL_NO_SURFACE)
			{
			eglDestroySurface(iDisplay,surface);
			eglSess->CheckExpectedError(EGL_SUCCESS);
			surface = EGL_NO_SURFACE;
			}		
		}
	
	CleanupStack::PopAndDestroy(2,eglSess);
	TerminateDisplayL();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0066

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Test passing a non-initialised RSgImage into eglCreatePixmapSurface()

@SYMTestActions
Initialise the thread to use RSgImage as the native pixmap type
For each config returned from eglGetConfig() that supports OpenGLES or OpenVG rendering to pixmaps
	*	Create an empty RSgImage object but do not initialize it
	*	Use eglCreatePixmapSurface() to construct a surface from the RSgImage object.
		o	Use a NULL attrib_list
	*	Close the RSgImage object

@SYMTestExpectedResults
Check that eglCreatePixmapSurface() returns EGL_NO_SURFACE
Check that eglGetError() returns EGL_BAD_NATIVE_PIXMAP
Check for memory and handle leaks.
*/
TVerdict CEglTest_CreatePixmapSurface_RSgImage_Negative_RSgImage_NotInitialised::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0066"));
	INFO_PRINTF1(_L("CEglTest_CreatePixmapSurface_RSgImage_Negative_RSgImage_NotInitialised::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// Create display object
	GetDisplayL();
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);
	eglSess->InitializeL();

	EGLConfig configs[KMaxEglConfigs];
	EGLint numConfigs=0;

	// Query number of configs available
	INFO_PRINTF1(_L("Calling eglGetConfigs to get the number of configs available..."));
	ASSERT_EGL_TRUE(eglGetConfigs(iDisplay, NULL, KMaxTInt, &numConfigs));

	INFO_PRINTF1(_L("Checking number of configs..."));
	ASSERT_TRUE(numConfigs >= 1);
	ASSERT_TRUE(numConfigs <= KMaxEglConfigs);
	INFO_PRINTF2(_L("Found %d configs"), numConfigs);
		
	// Get the configs
	INFO_PRINTF1(_L("Calling eglGetConfigs to get configs..."));
	ASSERT_EGL_TRUE(eglGetConfigs(iDisplay, configs, KMaxEglConfigs, &numConfigs));
	
	for(TUint index = 0; index < numConfigs; index++)
		{
		INFO_PRINTF2(_L("Getting Config Attributes for index %d"), index);
		
		EGLint surfaceType=0;
		ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, configs[index], EGL_SURFACE_TYPE, &surfaceType));
		if (!(surfaceType & EGL_PIXMAP_BIT))
			{
			// This is just an info print because we don't want to test surfaces that do not support pixmaps.
			VERBOSE_INFO_PRINTF1(_L("Skipping config: Does not support pixmaps"));
			continue;
			}
		
		EGLint renderableType=0;
		RSgImage sgImage; // Deliberately don't initialise it
		ASSERT_EGL_TRUE(eglGetConfigAttrib(iDisplay, configs[index], EGL_RENDERABLE_TYPE, &renderableType));		
		if (renderableType & EGL_OPENVG_BIT)
			{
			VERBOSE_INFO_PRINTF1(_L("...is OpenVg renderable"));
			eglSess->OpenSgDriverL();
			
			// Need to set the API type before creating the context (H/W requires this to be done before the surface is created, rather than the context)
			VERBOSE_INFO_PRINTF1(_L("Calling eglBindAPI(EGL_OPENVG_API)"));
			ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));
			
			// Create a pixmap surface from the native image
			VERBOSE_INFO_PRINTF1(_L("Calling eglCreatePixmapSurface"));
			EGLSurface surface = eglCreatePixmapSurface(iDisplay, configs[index], &sgImage, KPixmapAttribsNone);
			
			// Expect this to fail
			ASSERT_EQUALS(surface, EGL_NO_SURFACE);
	 		ASSERT_EGL_ERROR(EGL_BAD_NATIVE_PIXMAP);
			eglSess->CloseSgDriver();
			}
		
		if ((renderableType & EGL_OPENGL_ES_BIT) || (renderableType & EGL_OPENGL_ES2_BIT))
			{
			VERBOSE_INFO_PRINTF1(_L("...is OpenGles renderable"));
			eglSess->OpenSgDriverL();
			
			// Need to set the API type before creating the context (H/W requires this to be done before the surface is created, rather than the context)
			VERBOSE_INFO_PRINTF1(_L("Calling eglBindAPI(EGL_OPENGL_ES_API)"));
			ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENGL_ES_API));
			
			// Create a pixmap surface from the native image
			VERBOSE_INFO_PRINTF1(_L("Calling eglCreatePixmapSurface"));
			EGLSurface surface = eglCreatePixmapSurface(iDisplay, configs[index], &sgImage, KPixmapAttribsNone);
			
			// Expect this to fail
			ASSERT_EQUALS(surface, EGL_NO_SURFACE);
	 		ASSERT_EGL_ERROR(EGL_BAD_NATIVE_PIXMAP);
			eglSess->CloseSgDriver();
			}
		}	
	
	CleanupStack::PopAndDestroy(eglSess);
	TerminateDisplayL();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0075

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Test that the system can detect obviously invalid pixmap objects 

@SYMTestActions
Try to create a pixmap surface from an odd integer

@SYMTestExpectedResults
This should fail with EGL_BAD_NATIVE_PIXMAP
Check that all memory and handles have been deallocated.
*/
TVerdict CEglTest_CreatePixmapSurface_Negative_Invalid_Pixmap_Type::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0075"));
	INFO_PRINTF1(_L("CEglTest_CreatePixmapSurface_Negative_Invalid_Pixmap_Type::doTestStepL"));

	// Create display object
	GetDisplayL();
	
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);

	// Initialise the display object
	eglSess->InitializeL();
	eglSess->OpenSgDriverL();
	
    EGLConfig configs[KMaxEglConfigs];
	EGLint numConfigs=0;

	const EGLint KConfigAttribPixmapSurface[] = 
		{
		EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
		EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
		EGL_NONE
		};
	
	// Query number of configs available
    INFO_PRINTF1(_L("Calling eglChooseConfig to get the number of configs available for pixmap surfaces..."));
	ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribPixmapSurface, NULL, KMaxTInt, &numConfigs));

	INFO_PRINTF1(_L("Checking number of configs..."));
	ASSERT_TRUE(numConfigs >= 1);
	ASSERT_TRUE(numConfigs <= KMaxEglConfigs);
	INFO_PRINTF2(_L("Found %d configs"), numConfigs);
		
	// Get the configs
	INFO_PRINTF1(_L("Calling eglChooseConfig to get configs..."));
	ASSERT_EGL_TRUE(eglChooseConfig(iDisplay, KConfigAttribPixmapSurface, configs, KMaxEglConfigs, &numConfigs));
	
	EGLint badPixmap=12345;
	
	EGLSurface surface = eglCreatePixmapSurface(iDisplay, configs[0], &badPixmap, NULL);	
	eglSess->SetExpectedError(EGL_BAD_NATIVE_PIXMAP);	
	ASSERT_TRUE(eglSess->CheckExpectedError(surface));
	
	CleanupStack::PopAndDestroy(eglSess);
	TerminateDisplayL();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0420

@SYMTestPriority 1

@SYMPREQ 2637

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Test the successful creation and use of a pixmap surface for all supporting configs.

@SYMTestActions
Initialize the thread to use RSgImage as the native pixmap type.
For each supported combination of pixel mode and usage bits (see Graphics Resource documentation for detail)
•   Create an SgImage
•   Choose EGL config, supplying in attribute list as EGL_MATCH_NATIVE_PIXMAP the SgImage created on the previous step
•   For each configuration obtained on the previous step and for three possible resource close rules (close SgImage late, close SgImage early, close SgDriver and SgImage early):
o   Use eglCreatePixmapSurface() to construct a surface from the RSgImage object and make it current
o   Make some basic drawing to the surface (fill the area with pre-defined  colour)
o   Call eglWaitClient() to finish the above drawing instructions synchronously.
o   Deallocate the surface and the pixmap

@SYMTestExpectedResults
There is at least one configuration available for all mandatory combinations of pixel formats and usage bits.
Use EGL client API to check that the surface with underlying SgImage has the expected pixel data.
*/
TVerdict CEglTest_CreatePixmapSurface_RSgImage_Positive_MantadoryFormatUsageSupport::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0420"));
    INFO_PRINTF1(_L("CEglTest_CreatePixmapSurface_RSgImage_Positive_MantadoryFormatUsageSupport::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage | KEGL_KHR_image_base | KEGL_KHR_image_pixmap);
	if(!ret)
		{
		// The extension is not supported
		RecordTestResultL();
		CloseTMSGraphicsStep();
		return TestStepResult();
		}

	// Create display object
	GetDisplayL();
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);
	eglSess->InitializeL();
	eglSess->OpenSgDriverL();

    // Initialise ini parser
    CEglTestCommonIniSettings* iniParser = CEglTestCommonIniSettings::NewL();
    CleanupStack::PushL(iniParser);
    TInt numPixmapSgSurfaceFormats = iniParser->GetNumberOfFormats(KSectionPixmapSgSurfaceFormats);
    
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    const EGLint KSgPixmapUsageSupport[] =
        {
        ESgUsageBitOpenVgSurface,         
        ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface,
        ESgUsageBitOpenVgSurface | ESgUsageBitOpenGles2Texture2D,
        ESgUsageBitOpenVgSurface | ESgUsageBitOpenVgImage| ESgUsageBitOpenGles2Texture2D,
        ESgUsageBitOpenGles2Surface,
        ESgUsageBitOpenGles2Surface | ESgUsageBitOpenVgImage,
        ESgUsageBitOpenGles2Texture2D |ESgUsageBitOpenGles2Surface,
        ESgUsageBitOpenVgImage | ESgUsageBitOpenGles2Texture2D | ESgUsageBitOpenGles2Surface
        };

    for(TInt i = 0; i < numPixmapSgSurfaceFormats; i++)
        {
        VGImageFormat surfaceFormat = iniParser->GetVgFormat(KSectionPixmapSgSurfaceFormats,i);
        TSgImageInfoTest imageInfo = TSgImageInfoTest();
        imageInfo.iPixelFormat = EglTestConversion::VgFormatToSgPixelFormat(surfaceFormat);;
        EGLint alphaPreBit = (imageInfo.iPixelFormat == EUidPixelFormatARGB_8888_PRE) ? EGL_VG_ALPHA_FORMAT_PRE_BIT : 0;
        TInt numUsageCombinations = sizeof(KSgPixmapUsageSupport) / sizeof(KSgPixmapUsageSupport[0]);
        for(TUint32 j = 0; j < numUsageCombinations; j++)
            {
            if(KSgPixmapUsageSupport[j] & (ESgUsageBitOpenGles2Texture2D | ESgUsageBitOpenGles2Surface))
                {
                if(!eglSess->IsOpenGLES2Supported())
                    continue;
                }
            if(KSgPixmapUsageSupport[j] & (ESgUsageBitOpenGlesTexture2D | ESgUsageBitOpenGlesSurface))
                {
                if(!eglSess->IsOpenGLESSupported())
                    continue;
                }

            imageInfo.iUsage = KSgPixmapUsageSupport[j]; 
            RSgImage image;
            ASSERT_EQUALS(image.Create(imageInfo, NULL, NULL), KErrNone);
            CleanupClosePushL(image);
            TSgImageInfoTest imageInfoSupported; 
            ASSERT_EQUALS(image.GetInfo(imageInfoSupported), KErrNone);//returned info may include more usage bits than we actually requested
            if(((imageInfoSupported.iUsage & (ESgUsageBitOpenGles2Texture2D | ESgUsageBitOpenGles2Surface)) && 
                               (!eglSess->IsOpenGLES2Supported()) ||
                ((imageInfoSupported.iUsage & (ESgUsageBitOpenGlesTexture2D | ESgUsageBitOpenGlesSurface)) && 
                               (!eglSess->IsOpenGLESSupported()))))
                {
                CleanupStack::PopAndDestroy(&image);
                continue;
                }	
            
            EGLint eglRenderableTypeValue = 0;
            if(imageInfoSupported.iUsage & ESgUsageBitOpenVgSurface)
                {
                eglRenderableTypeValue = EGL_OPENVG_BIT;
                }
            else if(imageInfoSupported.iUsage & ESgUsageBitOpenGles2Surface)
                {
                eglRenderableTypeValue = EGL_OPENGL_ES2_BIT;
                }
            else if(imageInfoSupported.iUsage & ESgUsageBitOpenGlesSurface)
                {
                eglRenderableTypeValue = EGL_OPENGL_ES_BIT;
                }
            else if(imageInfoSupported.iUsage & ESgUsageBitOpenGlSurface)
                {
                eglRenderableTypeValue = EGL_OPENGL_BIT;
                }
            TEST(eglRenderableTypeValue != 0);

            // only minimum amount of renderable type matching to Native pixmap will be passed into attribute list.
            // And egl will decide whatever configs include at least given renderable type.
            const EGLint KAttrib_list[] = { EGL_MATCH_NATIVE_PIXMAP, reinterpret_cast<EGLint>(&image),
											EGL_RENDERABLE_TYPE, eglRenderableTypeValue,
											EGL_SURFACE_TYPE, EGL_PIXMAP_BIT | alphaPreBit,
											EGL_NONE };     
            
            EGLConfig configs[KMaxEglConfigs];
            EGLint numConfigs=0;
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,KAttrib_list,configs, KMaxEglConfigs,&numConfigs));
            CleanupStack::PopAndDestroy(&image);
            TEST(numConfigs > 0);
            
            for(TInt index = 0; index < numConfigs; index++)
                {
                const TInt numCloseRules = 3; 
                for(TInt closeRule = 0; closeRule < numCloseRules; closeRule++)
                    {
                    CTestEglSession::TResourceCloseRule resourceCloseRule = CTestEglSession::EResourceCloseSgImageLate;
                    switch(closeRule)
                        {
                    case 0:
                        VERBOSE_INFO_PRINTF1(_L("Close Image Late"));
                        resourceCloseRule = CTestEglSession::EResourceCloseSgImageLate;
                        break;
                    case 1:
                        VERBOSE_INFO_PRINTF1(_L("Close Image Early"));
                        resourceCloseRule = CTestEglSession::EResourceCloseSgImageEarly;
                        break;
                    case 2:
                        VERBOSE_INFO_PRINTF1(_L("Close Sg Driver and Image Early"));
                        resourceCloseRule = CTestEglSession::EResourceCloseSgDriverAndImageEarly;
                        break;
                    default:
                        break;
                        }
                
                    if (KSgPixmapUsageSupport[j] & ESgUsageBitOpenVgSurface)
                        {
                        VERBOSE_INFO_PRINTF1(_L("...is OpenVG renderable"));
                        eglSess->TryUsePixmapRSgImageOpenVgL(configs[index], imageInfo, resourceCloseRule); // we can't fail to create an image as we have already tried it 
                        }
                    if (KSgPixmapUsageSupport[j] & (ESgUsageBitOpenGlesSurface | ESgUsageBitOpenGlesTexture2D))
                        {
                        VERBOSE_INFO_PRINTF1(_L("...is OpenGLES renderable"));
                        eglSess->TryUsePixmapRSgImageOpenGlesL(configs[index], imageInfo, resourceCloseRule, 1); // we can't fail to create an image as we have already tried it
                        }
                    if (KSgPixmapUsageSupport[j] & (ESgUsageBitOpenGles2Surface | ESgUsageBitOpenGles2Texture2D))
                        {
                        VERBOSE_INFO_PRINTF1(_L("...is OpenGLES2 renderable"));
                        eglSess->TryUsePixmapRSgImageOpenGlesL(configs[index], imageInfo, resourceCloseRule, 2); // we can't fail to create an image as we have already tried it
                        }
                    }//for(TInt closeRule; closeRule < numCloseRules; closeRule++)
                }//for(TInt index = 0; index < numConfigs; index++)
            }//for(TUint32 j = 0; j < numUsageCombinations; j++)
        }//for(TInt i = 0; i < numSgImagePixelFormatSupport; i++)
#else
    const EGLint KSgPixmapUsageSupport[] =
        {
        ESgUsageOpenVgTarget,         
        ESgUsageOpenVgImage | ESgUsageOpenVgTarget,
        ESgUsageOpenVgTarget | ESgUsageOpenGles2Texture2D,
        ESgUsageOpenVgTarget | ESgUsageOpenVgImage| ESgUsageOpenGles2Texture2D,
        ESgUsageOpenGles2Target,
        ESgUsageOpenGles2Target | ESgUsageOpenVgImage,
        ESgUsageOpenGles2Texture2D |ESgUsageOpenGles2Target,
        ESgUsageOpenVgImage | ESgUsageOpenGles2Texture2D | ESgUsageOpenGles2Target
        };

    for(TInt i = 0; i < numPixmapSgSurfaceFormats; i++)
        {
        VGImageFormat surfaceFormat = iniParser->GetVgFormat(KSectionPixmapSgSurfaceFormats,i);
        TSgImageInfoTest imageInfo = TSgImageInfoTest();
        imageInfo.iPixelFormat = EglTestConversion::VgFormatToSgPixelFormat(surfaceFormat);;
        EGLint alphaPreBit = (imageInfo.iPixelFormat == EUidPixelFormatARGB_8888_PRE) ? EGL_VG_ALPHA_FORMAT_PRE_BIT : 0;
        TInt numUsageCombinations = sizeof(KSgPixmapUsageSupport) / sizeof(KSgPixmapUsageSupport[0]);
        for(TUint32 j = 0; j < numUsageCombinations; j++)
            {
            if(KSgPixmapUsageSupport[j] & (ESgUsageOpenGles2Texture2D | ESgUsageOpenGles2Target))
                {
                if(!eglSess->IsOpenGLES2Supported())
                    continue;
                }
            if(KSgPixmapUsageSupport[j] & (ESgUsageOpenGles2Texture2D | ESgUsageOpenGlesTarget))
                {
                if(!eglSess->IsOpenGLESSupported())
                    continue;
                }
            imageInfo.iUsage = KSgPixmapUsageSupport[j]; 
            RSgImage image;
            ASSERT_EQUALS(image.Create(imageInfo, NULL, NULL), KErrNone);
            CleanupClosePushL(image);
            TSgImageInfoTest imageInfoSupported; 
            ASSERT_EQUALS(image.GetInfo(imageInfoSupported), KErrNone);//returned info may include more usage bits than we actually requested
            if(((imageInfoSupported.iUsage & (ESgUsageOpenGles2Texture2D | ESgUsageOpenGles2Target)) && 
                               (!eglSess->IsOpenGLES2Supported()) ||
                ((imageInfoSupported.iUsage & (ESgUsageOpenGles2Texture2D | ESgUsageOpenGlesTarget)) && 
                               (!eglSess->IsOpenGLESSupported()))))
                {
                CleanupStack::PopAndDestroy(&image);
                continue;
                }   
            
            EGLint eglRenderableTypeValue = 0;
            if(imageInfoSupported.iUsage & ESgUsageOpenVgTarget)
                {
                eglRenderableTypeValue = EGL_OPENVG_BIT;
                }
            else if(imageInfoSupported.iUsage & ESgUsageOpenGles2Target)
                {
                eglRenderableTypeValue = EGL_OPENGL_ES2_BIT;
                }
            else if(imageInfoSupported.iUsage & ESgUsageOpenGlesTarget)
                {
                eglRenderableTypeValue = EGL_OPENGL_ES_BIT;
                }
            TEST(eglRenderableTypeValue != 0);
            
            // only minimum amount of renderable type matching to Native pixmap will be passed into attribute list.
            // And egl will decide whatever configs include at least given renderable type.
            const EGLint KAttrib_list[] = { EGL_MATCH_NATIVE_PIXMAP, reinterpret_cast<EGLint>(&image),
                                            EGL_RENDERABLE_TYPE, eglRenderableTypeValue,
                                            EGL_SURFACE_TYPE, EGL_PIXMAP_BIT | alphaPreBit,
                                            EGL_NONE };     
            
            EGLConfig configs[KMaxEglConfigs];
            EGLint numConfigs=0;
            ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,KAttrib_list,configs, KMaxEglConfigs,&numConfigs));
            CleanupStack::PopAndDestroy(&image);
            TEST(numConfigs > 0);
            
            for(TInt index = 0; index < numConfigs; index++)
                {
                const TInt numCloseRules = 3; 
                for(TInt closeRule = 0; closeRule < numCloseRules; closeRule++)
                    {
                    CTestEglSession::TResourceCloseRule resourceCloseRule = CTestEglSession::EResourceCloseSgImageLate;
                    switch(closeRule)
                        {
                    case 0:
                        VERBOSE_INFO_PRINTF1(_L("Close Image Late"));
                        resourceCloseRule = CTestEglSession::EResourceCloseSgImageLate;
                        break;
                    case 1:
                        VERBOSE_INFO_PRINTF1(_L("Close Image Early"));
                        resourceCloseRule = CTestEglSession::EResourceCloseSgImageEarly;
                        break;
                    case 2:
                        VERBOSE_INFO_PRINTF1(_L("Close Sg Driver and Image Early"));
                        resourceCloseRule = CTestEglSession::EResourceCloseSgDriverAndImageEarly;
                        break;
                    default:
                        break;
                        }
                
                    if (KSgPixmapUsageSupport[j] & ESgUsageOpenVgTarget)
                        {
                        VERBOSE_INFO_PRINTF1(_L("...is OpenVG renderable"));
                        eglSess->TryUsePixmapRSgImageOpenVgL(configs[index], imageInfo, resourceCloseRule); // we can't fail to create an image as we have already tried it 
                        }
                    if (KSgPixmapUsageSupport[j] & (ESgUsageOpenGlesTarget | ESgUsageOpenGles2Texture2D))
                        {
                        VERBOSE_INFO_PRINTF1(_L("...is OpenGLES renderable"));
                        eglSess->TryUsePixmapRSgImageOpenGlesL(configs[index], imageInfo, resourceCloseRule, 1); // we can't fail to create an image as we have already tried it
                        }
                    if (KSgPixmapUsageSupport[j] & (ESgUsageOpenGles2Target | ESgUsageOpenGles2Texture2D))
                        {
                        VERBOSE_INFO_PRINTF1(_L("...is OpenGLES2 renderable"));
                        eglSess->TryUsePixmapRSgImageOpenGlesL(configs[index], imageInfo, resourceCloseRule, 2); // we can't fail to create an image as we have already tried it
                        }
                    }//for(TInt closeRule; closeRule < numCloseRules; closeRule++)
                }//for(TInt index = 0; index < numConfigs; index++)
            }//for(TUint32 j = 0; j < numUsageCombinations; j++)
        }//for(TInt i = 0; i < numSgImagePixelFormatSupport; i++)
    
 #endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
    
    CleanupStack::PopAndDestroy(2, eglSess);//iniParser, eglSess
    TerminateDisplayL();
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }
