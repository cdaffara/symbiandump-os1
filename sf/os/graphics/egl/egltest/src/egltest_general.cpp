// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <sgresource/sgimage.h>
#else
#include <graphics/sgimage.h>
#endif //SYMBIAN_GRAPHICS_EGL_SGIMAGELITE
#include <test/tefunit.h> // for ASSERT macros
#include <GLES/gl.h>

#include "egltest_general.h"

#include <test/egltestcommonconversion.h>
#include <test/egltestcommoninisettings.h>
#include <test/egltestcommonsgimageinfo.h>


/**
@SYMTestCaseID GRAPHICS-EGL-0005

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
Dump diagnostic information that may help in debugging any issues found in the other tests.
This is not really a test in itself, but can be listed in the test script as a specific test step.
It is expected always to pass.

@SYMTestActions
Repeatedly call eglQueryString() with each of the following parameters:
	*	EGL_CLIENT_APIS
	*	EGL_EXTENSIONS
	*	EGL_VENDOR
	*	EGL_VERSION
For each call, dump the resulting string to the test log file.
If the result of EGL_CLIENT_APIS includes "OpenVG" then bind to the OpenVG API and repeatedly call vgGetString() with each of the following parameters
	*	VG_VENDOR
	*	VG_RENDERER
	*	VG_VERSION
	*	VG_EXTENSIONS
If the result of EGL_CLIENT_APIS includes "OpenGLES" then bind to the OpenGLES API and repeatedly call GetString() with each of the following parameters
	*	VENDOR
	*	RENDERER
	*	VERSION
	*	EXTENSIONS.

@SYMTestExpectedResults
All data will be dumped to the test log file.
There are no specific pass criteria for this test case.
*/
TVerdict CEglTest_DumpStrings::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0005"));
	INFO_PRINTF1(_L("CEglTest_DumpStrings::doTestStepL"));

	// Create display object
	GetDisplayL();

	CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);

	// Initialise the display object
	eglSess->InitializeL();

	// EGL Strings
	INFO_PRINTF1(_L("Dumping EGL strings"));

	// Note that eglQueryString() MUST be called AFTER eglInitialize() for iDisplay
	TPtrC8 ptrEglClientApis((const TText8 *)eglQueryString(iDisplay, EGL_CLIENT_APIS));
	DumpString(_L("EGL_CLIENT_APIS"),ptrEglClientApis);

	const char* strEglExtensions = eglQueryString(iDisplay, EGL_EXTENSIONS);
	ASSERT_EGL_TRUE(strEglExtensions!=NULL);
	DumpString(_L("EGL_EXTENSIONS"), TPtrC8((const TText8 *)strEglExtensions));

	const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
	ASSERT_EGL_TRUE(strEglVendor!=NULL);
	DumpString(_L("EGL_VENDOR"), TPtrC8((const TText8 *)strEglVendor));

	const char* strEglVersion = eglQueryString(iDisplay, EGL_VERSION);
	ASSERT_EGL_TRUE(strEglVersion!=NULL);
	DumpString(_L("EGL_VERSION"), TPtrC8((const TText8 *)strEglVersion));

	// OpenVG Strings
	if (ptrEglClientApis.Find(_L8("OpenVG")) >= 0)
		{
		INFO_PRINTF1(_L("Dumping OpenVG strings"));

		// OpenVG needs a current VG context before it will allow the call to vgGetString
        EGLConfig currentConfig = eglSess->GetConfigExactMatchL(EPBufferAttribsColor64K);
        eglSess->CreatePbufferSurfaceAndMakeCurrentL(currentConfig, KPixmapSize, EGL_OPENVG_API);

		TPtrC8 ptrVgVendor((const TText8 *)vgGetString(VG_VENDOR));
		DumpString(_L("VG_VENDOR"), ptrVgVendor);

		TPtrC8 ptrVgRenderer((const TText8 *)vgGetString(VG_RENDERER));
		DumpString(_L("VG_RENDERER"), ptrVgRenderer);

		TPtrC8 ptrVgVersion((const TText8 *)vgGetString(VG_VERSION));
		DumpString(_L("VERSION"), ptrVgVersion);

		TPtrC8 ptrVgExtensions((const TText8 *)vgGetString(VG_EXTENSIONS));
		DumpString(_L("VG_EXTENSIONS"), ptrVgExtensions);
		
		//cleanup the context & surface
		eglSess->CleanupSurfaceSgImageL();
		}

	// OpenGLES Strings
	if (ptrEglClientApis.Find(_L8("OpenGL_ES")) >= 0)
		{
		INFO_PRINTF1(_L("Dumping OpenGLES strings"));

		// OpenGLES needs a current GLES context before it will allow the call to glGetString
        EGLConfig currentConfig = eglSess->GetConfigExactMatchL(EPBufferAttribsColor64K);
        eglSess->CreatePbufferSurfaceAndMakeCurrentL(currentConfig, KPixmapSize, EGL_OPENGL_ES_API);

		TPtrC8 ptrGlesVendor((const TText8 *)glGetString(GL_VENDOR));
		DumpString(_L("GL_VENDOR"), ptrGlesVendor);

		TPtrC8 ptrGlesRenderer((const TText8 *)glGetString(GL_RENDERER));
		DumpString(_L("GL_RENDERER"), ptrGlesRenderer);

		TPtrC8 ptrGlesVersion((const TText8 *)glGetString(GL_VERSION));
		DumpString(_L("GL_VERSION"), ptrGlesVersion);

		TPtrC8 ptrGlesExtensions((const TText8 *)glGetString(GL_EXTENSIONS));
		DumpString(_L("GL_EXTENSIONS"), ptrGlesExtensions);
		
		//cleanup the context & surface
		eglSess->CleanupSurfaceSgImageL();
		}

	CleanupStack::PopAndDestroy(eglSess);
	TerminateDisplayL();
	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

void CEglTest_DumpStrings::DumpString(const TDesC& aField, const TDesC8& aValue)
	{
	const TInt KMaxValueLength = 256;
	TBuf16<KMaxValueLength> bufValue;
	bufValue.Copy(aValue.Left(KMaxValueLength));
	INFO_PRINTF3(_L("%S: \"%S\""), &aField, &bufValue);
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0010

@SYMTestPriority 1

@SYMPREQ 39

@SYMREQ See SGL.GT0386.401 document

@SYMTestCaseDesc
To ensure that eglQueryString() returns the correct information.

@SYMTestActions
Call eglQueryString() with the EGL_EXTENSIONS parameter

@SYMTestExpectedResults
The returned space separated list is expected to include the items as defined in the .ini file.
This list is implementation specific, so the .ini file can be configured depending on each implementation.
As an example, the following ones are expected in most EGL implementations:
	*	EGL_NOK_pixmap_type_rsgimage
	*	EGL_SYMBIAN_COMPOSITION
	*	EGL_KHR_image_base
	*	EGL_KHR_image_pixmap 	
	*	EGL_KHR_reusable_sync
The above list will be configurable on a per-platform basis, via an INI file.  If a platform does not support
any of the above extensions then it may opt not to test for it via the INI file.
*/
TVerdict CEglTest_QueryString_Extensions::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-EGL-0010"));
	INFO_PRINTF1(_L("CEglTest_QueryString_Extensions::doTestStepL"));

	TInt numExtensions=0;
	if (!GetIntFromConfig(ConfigSection(), KKeyCountExtensionsEGL, numExtensions))
		{
		ERR_PRINTF2(_L("Cannot find key '%S' in INI file"), &KKeyCountExtensionsEGL);
		User::Leave(KErrArgument);
		}
	INFO_PRINTF2(_L("Looking for %d EGL extensions"), numExtensions);
	TBuf<32> bufExtensionNameKey;
	TBuf16<128> bufExtensionNameValue16;
	TPtrC16 ptrExtensionNameValue16(bufExtensionNameValue16);
	for(TInt i=0; i<numExtensions; i++)
		{
		bufExtensionNameKey.Format(KKeyExtensionEGLX, i);
		if (!GetStringFromConfig(ConfigSection(), bufExtensionNameKey, ptrExtensionNameValue16))
			{
			ERR_PRINTF2(_L("Cannot find key '%S' in INI file"), &bufExtensionNameKey);
			User::Leave(KErrArgument);
			}
		INFO_PRINTF2(_L("Checking for extension \"%S\""), &ptrExtensionNameValue16);
		TEST(CheckForExtensionL(0, ptrExtensionNameValue16));
		}

	numExtensions=0;
	if (!GetIntFromConfig(ConfigSection(), KKeyCountExtensionsVG, numExtensions))
		{
		ERR_PRINTF2(_L("Cannot find key '%S' in INI file"), &KKeyCountExtensionsVG);
		User::Leave(KErrArgument);
		}
	INFO_PRINTF2(_L("Looking for %d VG extensions"), numExtensions);
	for(TUint i=0; i<numExtensions; i++)
		{
		bufExtensionNameKey.Format(KKeyExtensionVGX, i);
		if (!GetStringFromConfig(ConfigSection(), bufExtensionNameKey, ptrExtensionNameValue16))
			{
			ERR_PRINTF2(_L("Cannot find key '%S' in INI file"), &bufExtensionNameKey);
			User::Leave(KErrArgument);
			}
		INFO_PRINTF2(_L("Checking for extension \"%S\""), &ptrExtensionNameValue16);
		TEST(CheckForExtensionL(0, ptrExtensionNameValue16));
		}

	RecordTestResultL();
	CloseTMSGraphicsStep();
	return TestStepResult();
	}

/**
@SYMTestCaseID GRAPHICS-EGL-0012

@SYMTestPriority 3

@SYMPREQ 39

@SYMREQ ReqNum

@SYMTestCaseDesc
Ensure that eglGetProcAddress() returns NULL when given invalid data.

@SYMTestActions
Call eglGetProcAddress() with a <procname> that is known not to be supported - e.g. "qwerty"
Call eglGetProcAddress() with an empty <procname> - i.e. ""
Call eglGetProcAddress() with a null <procname> - i.e. NULL

@SYMTestExpectedResults
For each case, eglGetProcAddress() should return NULL, without raising an EGL error.
*/
TVerdict CEglTest_General_Negative_ProcAddress::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0012"));
    INFO_PRINTF1(_L("CEglTest_General_Negative_ProcAddress::doTestStepL"));

    // Create display object
    GetDisplayL();

    // Initialize display object
    INFO_PRINTF1(_L("Calling eglInitialize"));
    TEST_EGL_ERROR(eglInitialize(iDisplay, NULL, NULL), EGL_SUCCESS);

    INFO_PRINTF1(_L("Calling eglGetProcAddress() with a 'procname' that is known not to be supported"));
    TEST_EGL_ERROR(eglGetProcAddress("qwerty") == NULL, EGL_SUCCESS);

    INFO_PRINTF1(_L("Calling eglGetProcAddress() with an empty 'procname'"));
    TEST_EGL_ERROR(eglGetProcAddress("") == NULL, EGL_SUCCESS);

    INFO_PRINTF1(_L("Calling eglGetProcAddress() with a null 'procname'"));
    TEST_EGL_ERROR(eglGetProcAddress(NULL) == NULL, EGL_SUCCESS);

    TerminateDisplayL();
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }


/**
@SYMTestCaseID GRAPHICS-EGL-0309

@SYMPREQ 			PREQ2650

@SYMTestCaseDesc 	
1. EglTerminate() with an invalid/non-existant display
2. Double eglTerminate() with an uninitialised display
3. Double eglTerminate() with an initialised display

@SYMTestPriority 	High

@SYMTestStatus 		Implemented

@SYMTestActions		
1. Call eglTerminate with an invalid display handle argument. Call eglTerminate with EGL_NO_DISPLAY argument
2. Create an uninitialised display and call eglTerminate() twice. Re-create and terminate the display to check for memory leaks.
3. Create an initialised display and call eglTerminate() twice. Re-create and terminate the display to check for memory leaks.

@SYMTestExpectedResults		
1. eglTerminate should return EGL_FALSE on both calls with error EGL_BAD_DISPLAY
2. eglTerminate should return EGL_TRUE on all calls and the display should not be set to EGL_NO_DISPLAY.
3. eglTerminate should return EGL_TRUE on all calls and the display should not be set to EGL_NO_DISPLAY.
The UHEAP markers should indicate no memory leaks after this test case has run.
*/
TVerdict CEglTest_EglTerminate_Negative::doTestStepL()
	{
	INFO_PRINTF1(_L("CEglTest_EglTerminate_Negative::doTestStepL"));
	SetTestStepID(_L("GRAPHICS-EGL-0309"));

    //Test 1 - Arbitrary invalid display handle
    const EGLDisplay invalidDisplay = 77;

    //Call eglTerminate with the invalid display handle
    TEST(eglTerminate(invalidDisplay) == EGL_FALSE);
    ASSERT_EGL_ERROR(EGL_BAD_DISPLAY);

    //Call eglTerminate with argument EGL_NO_DISPLAY
    TEST(eglTerminate(EGL_NO_DISPLAY) == EGL_FALSE);
    ASSERT_EGL_ERROR(EGL_BAD_DISPLAY);

    //Test 2 - Create display object
    iDisplay = EGL_NO_DISPLAY;
    GetDisplayL();

	//Call eglTerminate twice
    ASSERT_EGL_TRUE(eglTerminate(iDisplay));
    TEST(eglGetError() == EGL_SUCCESS);

    ASSERT_EGL_TRUE(eglTerminate(iDisplay));
    TEST(eglGetError() == EGL_SUCCESS);

    //Recreate display object and call eglTerminate()
    iDisplay = EGL_NO_DISPLAY;
    GetDisplayL();
    ASSERT_EGL_TRUE(eglTerminate(iDisplay));
    TEST(eglGetError() == EGL_SUCCESS);

    //Test 3
    iDisplay = EGL_NO_DISPLAY;
    GetDisplayL();
    CTestEglSession* eglSess = CTestEglSession::NewLC(Logger(), iDisplay, 0);

    // Initialise the display object
    eglSess->InitializeL();

    //Call eglTerminate twice
    ASSERT_EGL_TRUE(eglTerminate(iDisplay));
    TEST(eglGetError() == EGL_SUCCESS);

    ASSERT_EGL_TRUE(eglTerminate(iDisplay));
    TEST(eglGetError() == EGL_SUCCESS);

    //Recreate display object
    iDisplay = EGL_NO_DISPLAY;
    GetDisplayL();

    //Call eglTerminate
    ASSERT_EGL_TRUE(eglTerminate(iDisplay));
    TEST(eglGetError() == EGL_SUCCESS);

    //cleanup and stuff...
    CleanupStack::PopAndDestroy(1, eglSess);
    ASSERT_EGL_TRUE(eglReleaseThread());

    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
	}
