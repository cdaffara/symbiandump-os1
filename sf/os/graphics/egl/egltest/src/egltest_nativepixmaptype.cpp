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

#include <iniparser.h>
#include <test/tefunit.h> // for ASSERT macros

#include "egltest_nativepixmaptype.h"
#include <test/egltestcommonconversion.h>
#include <test/egltestcommoninisettings.h>


/**
@SYMTestCaseID GRAPHICS-EGL-0015

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Checks the single threaded usage of a Pixmap<RSgImage>

@SYMTestActions
Call eglInitialize()
Use a Pixmap<RSgImage>

@SYMTestExpectedResults
Use of Pixmap<RSgImage> succeeds
*/
TVerdict CEglTest_NativePixmapType_SingleThread_Positive_RSgImage::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0015"));
	INFO_PRINTF1(_L("CEglTest_NativePixmapType_SingleThread_Positive_RSgImage::doTestStepL"));
	
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

	// Try to create and use an RSgImage pixmap
	eglSess->TryUsePixmapRSgImageL();
	
	CleanupStack::PopAndDestroy(eglSess);
	TerminateDisplayL();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0028

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
These tests check usage of a Pixmap<RSgImage> within two threads.

@SYMTestActions
Execute threads A and B in the following order
A: Call eglInitialize()
B: Call eglInitialize()
-----
A: Use Pixmap<RSgImage>
B: Use Pixmap<RSgImage>

@SYMTestExpectedResults
Use_Pixmap<RSgImage> succeeds for both threads
*/
TVerdict CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0028"));
	INFO_PRINTF1(_L("CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice::doTestStepL"));
	
	TBool ret = CheckForExtensionL(KEGL_RSgimage);
	
	if(ret)
		{
		Test_MultiThreadL(2, ETrue);
		TerminateDisplayL();
		}
	
	INFO_PRINTF1(_L("Exit: CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice::doTestStepL"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("thread %d: CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice::doThreadFunctionL"), aIdx);
	
	GetDisplayL();
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	
	eglSess->InitializeL();
	
	Rendezvous(aIdx);
	
	eglSess->TryUsePixmapRSgImageL();
	
	CleanupStack::PopAndDestroy(eglSess);
	
	INFO_PRINTF2(_L("thread %d: Exit CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice::doThreadFunctionL"), aIdx);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0029

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
These tests check usage of a Pixmap<RSgImage> within two threads.
Threads are indipendent (no rendezvous).

@SYMTestActions
Execute threads A and B in the following order
A: Call eglInitialize()
B: Call eglInitialize()
-----
A: Use Pixmap<RSgImage>
B: Use Pixmap<RSgImage>

@SYMTestExpectedResults
Use_Pixmap<RSgImage> succeeds for both threads
*/
TVerdict CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0029"));
	INFO_PRINTF1(_L("CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous::doTestStepL"));
	
	TBool ret = CheckForExtensionL(KEGL_RSgimage);
		
	if(ret)
		{
		Test_MultiThreadL(2, ETrue);
		TerminateDisplayL();
		}
	
	INFO_PRINTF1(_L("Exit: CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous::doTestStepL"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("thread %d: CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous::doThreadFunctionL"), aIdx);
	GetDisplayL();
	//first check if the extension is present
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);	
	eglSess->InitializeL();
		
	eglSess->TryUsePixmapRSgImageL();
	
	CleanupStack::PopAndDestroy(eglSess);
	
	INFO_PRINTF2(_L("thread %d: Exit CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitTwice_NoRendezvous::doThreadFunctionL"), aIdx);
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0030

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
These test checks that one call to eglInitialize per process is sufficient.
Both threads use a Pixmap<RSgImage> 

@SYMTestActions
Execute threads A and B in the following order
A: Call eglInitialize()
B: <no initialisation>
-----
A: Use Pixmap<RSgImage>
B: Use Pixmap<RSgImage>

@SYMTestExpectedResults
Use_Pixmap<RSgImage> succeeds for both threads
*/
TVerdict CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0030"));
	INFO_PRINTF1(_L("CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly::doTestStepL"));

	TBool ret = CheckForExtensionL(KEGL_RSgimage);
		
	if(ret)
		{
		Test_MultiThreadL(2, ETrue);
		TerminateDisplayL();
		}
	
	INFO_PRINTF1(_L("Exit: CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly::doTestStepL"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly::doThreadFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("thread %d: CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly::doThreadFunctionL"), aIdx);
	
	GetDisplayL();
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	
	if (aIdx == 0)
		{
		eglSess->InitializeL();
		}
	Rendezvous(aIdx);
	if (aIdx == 1)
		{
		INFO_PRINTF2(_L("thread %d: skipping initialisation"), aIdx);
		}
	
	Rendezvous(aIdx);
	
	eglSess->TryUsePixmapRSgImageL();
	
	CleanupStack::PopAndDestroy(eglSess);

	INFO_PRINTF2(_L("thread %d: Exit CEglTest_NativePixmapType_MultiThread_Positive_RSgImage_InitOneThreadOnly::doThreadFunctionL"), aIdx);
	}

// MultiProcess tests

/**
@SYMTestCaseID GRAPHICS-EGL-0041

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
These tests check usage of a Pixmap<RSgImage> within two processes.

@SYMTestActions
Execute processes A and B in the following order

A: Call eglInitialize()
B: Call eglInitialize()
-----
A: Use_Pixmap<RSgImage>
B: Use_Pixmap<RSgImage>

@SYMTestExpectedResults
Use_Pixmap<RSgImage> should succeed for both processes
*/
TVerdict CEglTest_NativePixmapType_MultiProcess_Both_RSgImage::doTestStepL()
	{
    SetTestStepID(_L("GRAPHICS-EGL-0041"));
	INFO_PRINTF1(_L("CEglTest_NativePixmapType_MultiProcess_Both_RSgImage::doTestStepL"));

	// First check whether RSgImage is supported
	TBool ret = CheckForExtensionL(KEGL_RSgimage);
		
	if(ret)
		{
		// Now run the multiprocess test - each process can safely assume that RSgImage support is available.
		Test_MultiProcessL(KEglTestStepDllName, 2, KNativePixmapType_MultiProcess_Both_RSgImage);
		}
	
	INFO_PRINTF1(_L("Exit: CEglTest_NativePixmapType_MultiProcess_Both_RSgImage::doTestStepL"));
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_NativePixmapType_MultiProcess_Both_RSgImage::doProcessFunctionL(TInt aIdx)
	{
	INFO_PRINTF2(_L("process %d: CEglTest_NativePixmapType_MultiProcess_Both_RSgImage::doProcessFunctionL"), aIdx);

	// Create display object
	GetDisplayL();
	
	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, aIdx);
	
	// Initialize the thread
	eglSess->InitializeL();
	
	// Rendezvous: ensure that both threads get to this point before continuing
	Rendezvous(aIdx);	

	// Try to create and use an RSgImage pixmap
	INFO_PRINTF2(_L("process %d: Calling TryUsePixmapRSgImageL..."), aIdx);
	eglSess->TryUsePixmapRSgImageL();
	
	CleanupStack::PopAndDestroy(eglSess);
	TerminateDisplayL();
	
	INFO_PRINTF2(_L("process %d: Exit CEglTest_NativePixmapType_MultiProcess_Both_RSgImage::doThreadFunctionL"), aIdx);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0044

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Ensure that when asking for an EGLconfig via eglChooseConfig
passing an RSgImage with given iUsage and iPixelFormat attributes,
just the matching configs are returned.

@SYMTestActions
Fully construct an RSgImage setting iUsage and/or iPixelFormat
Loop through different values of iUsage and iPixelFormat and change RSgImage attributes accordingly
•	Query all the configs available on the display calling eglGetConfigs.
•	Loop through the possible configs and manually check how many they are and save their index.
•	Call eglChooseConfig() having in the attrib_list the attribute EGL MATCH NATIVE PIXMAP set as the handle to the RSgImage previously created.
•	Check if the number of the returned matching config is equal to the one manually checked earlier. Check if the returned configs are actually the same.
End Loop
Destroy the RSgImage object

@SYMTestExpectedResults
Returned Configs have to be the ones expected.
The values that iPixelFormat have to assume are:
•	EUidPixelFormatRGB_565
•	EUidPixelFormatXRGB_8888
•	EUidPixelFormatARGB_8888_PRE
It’s not possible to test any other pixel formats (such as 
EUidPixelFormatARGB_8888) because it is not allowed to create an RSgImage 
with that pixel format and iUsage bit saying that it is going to be used 
as a target.
The values that iUsage have to assume are:
•   ESgUsageBitOpenVgSurface
•   ESgUsageBitOpenGles2Surface
•	ESgUsageBitOpenVgSurface | ESgUsageBitOpenGles2Surface
•	ESgUsageBitOpenVgImage
If OpenGLES2 is not supported creation of the SgImage is expected to fail 
with KErrNotSupported error code.
*/
TVerdict CEglTest_ChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0044"));
	INFO_PRINTF1(_L("CEglTest_ChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR::doTestStepL"));

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
	CreateEglSessionL();
	iEglSess->InitializeL();
	iEglSess->OpenSgDriverL();
	
	CEglTestCommonIniSettings* iniParser = CEglTestCommonIniSettings::NewL();
	CleanupStack::PushL(iniParser);
				
	TInt numRSgImageTargetFormats = iniParser->GetNumberOfFormats(KSectionPixmapSgSurfaceFormats);
	if(!numRSgImageTargetFormats)
		{
		ERR_PRINTF1(_L("No formats to iterate through!"));
		User::Leave(KErrArgument);		
		}
	
	TBool isGles2Supported = iEglSess->IsOpenGLES2Supported();
	for(TUint i=0; i < numRSgImageTargetFormats; i++)
		{
		//We are running thorugh all the possible "target" configurations because
		//they have one allowed pixel format less than the source has
		//so this test is expected to fail for EUidPixelFormatARGB_8888
		iSurfaceFormat			= iniParser->GetVgFormat(KSectionPixmapSgSurfaceFormats,i);
		iSourceFormat 			= EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		INFO_PRINTF1(_L("Trying usage bit ESgUsageBitOpenVgSurface"));
		iSgImageSurfaceUsage 	= ESgUsageBitOpenVgSurface;
#else
		INFO_PRINTF1(_L("Trying usage bit ESgUsageOpenVgTarget"));
		iSgImageSurfaceUsage 	= ESgUsageOpenVgTarget;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		doTestPartialStepL();

		//if OpenGLES2 is not supported we must not succeed in creating SgImage
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		INFO_PRINTF1(_L("Trying usage bit ESgUsageVitOpenGles2Surface"));
		iSgImageSurfaceUsage    = ESgUsageBitOpenGles2Surface; 
		TRAPD(res, doTestPartialStepL());
		TESTL(res == (isGles2Supported ? KErrNone : KErrTEFUnitFail));
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

		//if OpenGLES2 is not supported we must not succeed in creating SgImage
#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		INFO_PRINTF1(_L("Trying usage bit ESgUsageBitOpenVgSurface | ESgUsageBitOpenGles2Surface"));
		iSgImageSurfaceUsage 	= ESgUsageBitOpenVgSurface | ESgUsageBitOpenGles2Surface; 
		TRAP(res, doTestPartialStepL());
		TESTL(res == (isGles2Supported ? KErrNone : KErrTEFUnitFail));
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE

#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		INFO_PRINTF1(_L("Trying usage bit ESgUsageBitOpenVgImage"));
		iSgImageSurfaceUsage 	= ESgUsageBitOpenVgImage;
#else
		INFO_PRINTF1(_L("Trying usage bit ESgUsageOpenVgImage"));
		iSgImageSurfaceUsage 	= ESgUsageOpenVgImage;
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
		doTestPartialStepL();
		}
	
	CleanupStack::PopAndDestroy(iniParser);
	CleanAll();
	
    RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CEglTest_ChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_ChooseConfig_EGL_MATCH_NATIVE_PIXMAP_KHR::doTestPartialStepL"));
	PrintUsedPixelConfiguration();
		
	//Creating one RSgImage containing reference bitmap's contents
	INFO_PRINTF1(_L("Creating one RSgImage"));
	RSgImage sgImage;
	CleanupClosePushL(sgImage);
	TUidPixelFormat pixelFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
	TSgImageInfo imageInfo;
	imageInfo.iSizeInPixels = KPixmapSize;
	imageInfo.iPixelFormat = pixelFormat;
	imageInfo.iUsage = iSgImageSurfaceUsage;

	// Create a reference bitmap which we use to init the SgImage (we use index=4)
	TDisplayMode bitmapMode = EglTestConversion::VgFormatToDisplayMode(iSurfaceFormat);
	CFbsBitmap* bitmap = iEglSess->CreateReferenceBitmapL(bitmapMode, 4);
	CleanupStack::PushL(bitmap);	
	ASSERT_EQUALS(sgImage.Create(imageInfo, bitmap->DataAddress(), bitmap->DataStride()), KErrNone);
	CleanupStack::PopAndDestroy(bitmap);
	bitmap = NULL;

	//create bitmap for further use
    bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    TSgImageInfo requestedImageInfo;
    sgImage.GetInfo(requestedImageInfo);

    TDisplayMode displayMode = EglTestConversion::PixelFormatToDisplayMode((TUidPixelFormat) (requestedImageInfo.iPixelFormat));
    TESTL(bitmap->Create(requestedImageInfo.iSizeInPixels, displayMode) == KErrNone);
	
	//Query all the config available on the display
	INFO_PRINTF1(_L("Calling eglGetConfigs, querying for the configs' number available on this display"));
	EGLint numConfigs;
	eglGetConfigs(iDisplay,NULL,0,&numConfigs);
	if(!numConfigs)
		{
		ERR_PRINTF1(_L("No available configs!"));
		CleanAll();
		User::Leave(KErrTEFUnitFail);
		}	
	
	//allocate a array to store all these configs and retrieve all the IDs	
	INFO_PRINTF2(_L("There are %d configs available on this display"),numConfigs);
	EGLConfig* configs = new(ELeave) EGLConfig[numConfigs];
	CleanupStack::PushL(configs);
	INFO_PRINTF1(_L("Calling eglGetConfigs, querying for all configs available on this display"));
	ASSERT_EGL_TRUE(eglGetConfigs(iDisplay,configs,KMaxEglConfigs,&numConfigs));		
	
	//"manually check" if a config is compatible with this RSgImage
    RArray<EGLint> compatibleConfigsIdsArray;
    CleanupClosePushL(compatibleConfigsIdsArray);
    RArray<EGLConfig> compatibleConfigsHandleArray;
    CleanupClosePushL(compatibleConfigsHandleArray);
  	for(TUint i=0;i<numConfigs;i++)
		{
		if(GetEglSess()->IsACompatibleConfig(configs[i],sgImage,EFalse))
			{
			compatibleConfigsHandleArray.AppendL(configs[i]);
			EGLint configID = 0;
			eglGetConfigAttrib(iDisplay, configs[i], EGL_CONFIG_ID, &configID);
			compatibleConfigsIdsArray.Append(configID);
            VERBOSE_INFO_PRINTF2(_L("Config %d is compatible"), configs[i]);
			}
		}
    const TInt numCompatibleConfigs = compatibleConfigsIdsArray.Count();  
	INFO_PRINTF2(_L("Of which %d are compatible with this RSgImage"),numCompatibleConfigs);

	//Setting the attributes list to EGL_MATCH_NATIVE_PIXMAP with value the handle to the RSgImage	
	const EGLint KAttrib_listFail[] = { EGL_MATCH_NATIVE_PIXMAP,NULL,EGL_NONE };
	const EGLint KAttrib_list[] = { EGL_MATCH_NATIVE_PIXMAP,(TInt)&sgImage,EGL_NONE };

	Mem::FillZ(configs, sizeof(EGLConfig)*numConfigs);
	
	//Ask all the configs eglChooseConfig thinks are compatible with this RSgImage
	//just a negative test
	INFO_PRINTF1(_L("Calling eglChooseConfig with the EGL_MATCH_NATIVE_PIXMAP flag set, but null pointer"));	
	ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,KAttrib_listFail,configs,numConfigs,&numConfigs) == EGL_FALSE);
	GetEglSess()->CheckExpectedErrorL(EGL_BAD_ATTRIBUTE);
	
	//actually retrieve those config
	INFO_PRINTF1(_L("Calling eglChooseConfig with the EGL_MATCH_NATIVE_PIXMAP flag set"));
	ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,KAttrib_list,configs,numConfigs,&numConfigs) == EGL_TRUE);

    RArray<EGLint> configsIdsArray;
    CleanupClosePushL(configsIdsArray);
    RArray<EGLConfig> configsHandleArray;
    CleanupClosePushL(configsHandleArray);
 	for(TUint i=0;i<numConfigs;i++)
        {
        configsHandleArray.Append(configs[i]);
        EGLint configID = 0;
        eglGetConfigAttrib(iDisplay, configs[i], EGL_CONFIG_ID, &configID);
        configsIdsArray.Append(configID);
        }

    INFO_PRINTF3(_L("Checking number of configs returned by eglChooseConfig: %d, number configs, calculated mannually: %d"), numConfigs, numCompatibleConfigs);
    ASSERT_EGL_TRUE(numConfigs <= numCompatibleConfigs);
    
	//Check if the config "manually checked" match with the ones eglChooseConfig returned
	TInt numFoundInConfig = 0; //signify, how many IDs from compatible configs matches 
	//to the IDs from config retreived by eglChooseConfig, where image is supplied as a native pixmap

	//check that all configs we checked previously mannually, have corresponding config retrieved via EGL API
	for(TUint i=0;i<numCompatibleConfigs;i++)
		{
		TBool found = EFalse;
	    if (configsIdsArray.Find(compatibleConfigsIdsArray[i]) != KErrNotFound)
            {
            found = ETrue;
            VERBOSE_INFO_PRINTF2(_L("Config %d It's compatible as expected!"),compatibleConfigsHandleArray[i]);
            numFoundInConfig++;
            }
		if(!found)
			{
			//Check whether we can create pixmap surface based on CFbsBitmap. 
			//If this succeeds the config was not intended to be used 
			//for RSgImage but CFbsBitmap, which is legitimate and we 
			//shall exclude this config from our consideration.
            EGLint EGL_RENDERABLE_TYPE_value = 0;
            eglGetConfigAttrib(iDisplay, compatibleConfigsHandleArray[i], EGL_RENDERABLE_TYPE, &EGL_RENDERABLE_TYPE_value);
            const EGLint* KAttribs = ((pixelFormat == EUidPixelFormatARGB_8888_PRE) && (EGL_RENDERABLE_TYPE_value & EGL_OPENVG_BIT)) ? KPixmapAttribsVgAlphaFormatPre : KPixmapAttribsNone;
            EGLSurface surface = eglCreatePixmapSurface(iDisplay,compatibleConfigsHandleArray[i],bitmap,KAttribs);
            ASSERT_EGL_TRUE(surface != EGL_NO_SURFACE);
            ASSERT_EGL_TRUE(eglDestroySurface(iDisplay,surface));
			}
		}
	TEST(numFoundInConfig==numConfigs);

	//now checking that eglCreatePixmapSurface returns a surface just with the proper configs
	//just the configs seen as compatible should allow to create a pixmap surface
	memset(configs,0,sizeof(EGLConfig)*numConfigs);
	
	INFO_PRINTF1(_L("Now testing eglCreatePixmapSurface"));	
	INFO_PRINTF1(_L("Calling eglGetConfigs, querying for all configs available on this display"));
	ASSERT_EGL_TRUE(eglGetConfigs(iDisplay,configs,KMaxEglConfigs,&numConfigs));

	configsHandleArray.Reset();
	configsIdsArray.Reset();
    for(TUint i=0;i<numConfigs;i++)
        {
        configsHandleArray.Append(configs[i]);
        EGLint configID = 0;
        eglGetConfigAttrib(iDisplay, configs[i], EGL_CONFIG_ID, &configID);
        configsIdsArray.Append(configID);
        }

	for(TUint i=0;i<numConfigs;i++)//iterates through all the configs available on this display
		{		
		EGLint EGL_SURFACE_TYPE_value = 0;
		EGLint EGL_RENDERABLE_TYPE_value = 0;
		eglGetConfigAttrib(iDisplay, configsHandleArray[i], EGL_SURFACE_TYPE, &EGL_SURFACE_TYPE_value);
		eglGetConfigAttrib(iDisplay, configsHandleArray[i], EGL_RENDERABLE_TYPE, &EGL_RENDERABLE_TYPE_value);
		
		//bind the API to the renderable type of the config
		if (EGL_RENDERABLE_TYPE_value & EGL_OPENVG_BIT)
			{
			eglBindAPI(EGL_OPENVG_API);
			}
		else
			{
			eglBindAPI(EGL_OPENGL_ES_API);
			}

		TBool found = EFalse;
	    if (compatibleConfigsIdsArray.Find(configsIdsArray[i]) != KErrNotFound)
            {
            found = ETrue;
            VERBOSE_INFO_PRINTF2(_L("Config %d It's compatible as expected!"),configsHandleArray[i]);
            numFoundInConfig++;
            }

	    if(EGL_SURFACE_TYPE_value & EGL_PIXMAP_BIT)	//we are interested just on pixmap surfaces
			{
			//we need to set the alpha bit value if we are using 16MAP sgImages and vgtarget
			const EGLint* KAttribs = ((pixelFormat == EUidPixelFormatARGB_8888_PRE) && (EGL_RENDERABLE_TYPE_value & EGL_OPENVG_BIT)) ? KPixmapAttribsVgAlphaFormatPre : KPixmapAttribsNone;
			EGLSurface surface = eglCreatePixmapSurface(iDisplay,configsHandleArray[i],&sgImage,KAttribs);
			if(surface != EGL_NO_SURFACE)
				{
				ASSERT_EGL_ERROR(EGL_SUCCESS);
				}
			else if (found)
				{
				//if EGL_NO_SURFACE and found,
				//we have to eliminate the case when it is possible to create pixmap surface based on CFbsBitmap. 
				//If this succeeds the config was not intended to be used 
				//for RSgImage but CFbsBitmap, which is legitimate and we 
				//shall exclude this config from our consideration.
				surface = eglCreatePixmapSurface(iDisplay,configsHandleArray[i],bitmap,KAttribs);
				ASSERT_EGL_TRUE(surface != EGL_NO_SURFACE);
				ASSERT_EGL_ERROR(EGL_SUCCESS);
				}
			else 
				{
				// if EGL_NO_SURFACE and !found, then the error raised by eglCreatePixmapSurface must be EGL_BAD_MATCH
				ASSERT_EGL_ERROR(EGL_BAD_MATCH);
				}
			if(surface != EGL_NO_SURFACE)
				{
				ASSERT_EGL_TRUE(eglDestroySurface(iDisplay,surface));
				}
			}
		}
	
	// clean-up everything
	CleanupStack::PopAndDestroy(7, &sgImage); // the 4 arrays, config, bitmap, sgimage
	return TestStepResult();
	}


/**
@SYMTestCaseID GRAPHICS-EGL-0420

@SYMTestPriority 1

@SYMPREQ 2637

@SYMTestCaseDesc
Ensure that the system can tell the difference between an RSgImage and a CFbsBitmap.

@SYMTestActions
Cycling through 64K, 16MU and 16MAP.
 - For each pixel format, create a CFbsBitmap and pass it into eglChooseConfig() via EGL_MATCH_NATIVE_PIXMAP.
 - If eglChooseConfig() returns no results for any pixel format then that’s OK, as we are not mandating that 
   the EGL implementation supports CFbsBitmaps 
	- call eglCreatePixmapSurface() with the bitmap to check it returns a suitable error and doesn’t crash. 
	- If it doesn’t crash then we have started to demonstrate it recognises this is not an RSgImage.
	- This also ensures that we are not mandating any of the above pixel formats, just trying them out.
 - If eglChooseConfig() does return some results, then use each config to create a pixmap surface and draw something. 
	- This will check that eglCreatePixmapSurface() can tell the difference, as well as some very basic sanity checking.

@SYMTestExpectedResults
If eglChooseConfig() returns no results, that's ok as long as it doesn’t crash.
If eglChooseConfig() returns some results, eglCreatePixmapSurface(), a surface can be successfully created and used.
*/
TVerdict CEglTest_CFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0420"));
	INFO_PRINTF1(_L("CEglTest_CFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR::doTestStepL"));

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
	CreateEglSessionL();
	iEglSess->InitializeL();
    iEglSess->OpenSgDriverL();

	CEglTestCommonIniSettings* iniParser = CEglTestCommonIniSettings::NewL();
	CleanupStack::PushL(iniParser);
	TInt numPixmapFbsSurfaceFormats = iniParser->GetNumberOfFormats(KSectionPixmapFbsSurfaceFormats);
	if(!numPixmapFbsSurfaceFormats)
		{
		ERR_PRINTF1(_L("No formats to iterate through!"));
		User::Leave(KErrArgument);		
		}

	for(TUint i=0; i < numPixmapFbsSurfaceFormats; i++)
		{
		//We are running thorugh all the possible CFbsBitmap format configurations
		iSurfaceFormat = iniParser->GetVgFormat(KSectionPixmapFbsSurfaceFormats,i);
		iSourceFormat = EglTestConversion::VgFormatToSgPixelFormat(iSurfaceFormat);
		doTestPartialStepL();
		}

	CleanupStack::PopAndDestroy(iniParser);
	CleanAll();
	
    RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CEglTest_CFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR::doTestPartialStepL()
	{
	INFO_PRINTF1(_L("CEglTest_CFbsBitmap_EGL_MATCH_NATIVE_PIXMAP_KHR::doTestPartialStepL"));
	PrintUsedPixelConfiguration();

	//Creating one reference CFbsBitmap in the specified format
	INFO_PRINTF1(_L("Creating a CFbsBitmap..."));
    TDisplayMode bitmapMode = EglTestConversion::PixelFormatToDisplayMode(iSourceFormat);
	CFbsBitmap* bitmap = GetEglSess()->CreateReferenceBitmapL(bitmapMode);
	CleanupStack::PushL(bitmap);	
	
	//Calling eglBindAPI
    ASSERT_EGL_TRUE(eglBindAPI(EGL_OPENVG_API));

	const EGLint KAttribList[] = {EGL_MATCH_NATIVE_PIXMAP, reinterpret_cast<EGLint>(bitmap), 
							 	  EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
							 	  EGL_SURFACE_TYPE, EGL_PIXMAP_BIT,
							 	  EGL_NONE };
	const EGLint* KPixmapAttribs = (iSourceFormat == EUidPixelFormatARGB_8888_PRE) ? KPixmapAttribsVgAlphaFormatPre : KPixmapAttribsNone;

  	//Calling eglChooseConfig with the reference CFbsBitmap
	INFO_PRINTF1(_L("Calling eglChooseConfig with EGL_MATCH_NATIVE_PIXMAP to check if the system understands CFbsBitmaps..."));
	EGLConfig currentConfig;
	EGLint numconfigs=0;
    ASSERT_EGL_TRUE(eglChooseConfig(iDisplay,KAttribList,&currentConfig,1,&numconfigs))
    if (numconfigs==0)
    	{
    	//If eglChooseConfig() returns no results for any pixel format then that’s OK.
    	//This also ensures that we are not mandating any of the above pixel formats, just trying them out.
    	INFO_PRINTF1(_L("eglChooseConfig() returns no results for this pixel format. Continue the test"));
    	}
    else
    	{
    	//If eglChooseConfig() does return some results, then use config to create a pixmap surface and draw something
    	INFO_PRINTF1(_L("eglChooseConfig() returns a config for this pixel format. Do some sanity"));
    	EGLSurface surface = eglCreatePixmapSurface(iDisplay,currentConfig,bitmap,KPixmapAttribs);
    	ASSERT_EGL_TRUE(surface != EGL_NO_SURFACE);
	    EGLContext context = eglCreateContext(iDisplay, currentConfig, EGL_NO_CONTEXT, NULL);
	    ASSERT_EGL_TRUE(context != EGL_NO_CONTEXT);
	    ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, surface, surface, context));

	    // Draw something to the surface
		VGfloat color[4] = { 1.0f, 0.f, 1.0f, 0.f };
		vgSetfv(VG_CLEAR_COLOR, 4, color);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
		vgClear(0, 0, bitmap->SizeInPixels().iWidth, bitmap->SizeInPixels().iHeight);
		ASSERT_EGL_TRUE(vgGetError() == VG_NO_ERROR);
    	INFO_PRINTF1(_L("Pixmap successfully created and drawn onto. Sanity check completed!"));

	    //destroy surface-context
	    ASSERT_EGL_TRUE(eglDestroySurface(iDisplay,surface));
    	ASSERT_EGL_TRUE(eglMakeCurrent(iDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
    	}

	// clean-up everything
	CleanupStack::PopAndDestroy(bitmap);
	return TestStepResult();
	}
