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

#include "egltest_reference.h"

/**
@SYMTestCaseID GRAPHICS-EGL-REF-0001

@SYMPREQ 2400

@SYMTestPriority 1

@SYMTestCaseDesc
Test eglQueryString returns the expected string for Reference EGL

@SYMTestActions
Get default display
	Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS
Initialise display
	Check return value is true, error code is EGL_SUCCESS and version is updated
Query string for version
	Check return value is “1.4 Reference EGL” and error code is EGL_SUCCESS
Query string for vendor
	Check return value is “Nokia” and error code is EGL_SUCCESS
Query string for client APIs
	Check return value is “” and error code is EGL_SUCCESS
Query string for extensions
	Check return value is “EGL_KHR_reusable_sync EGL_NOK__private__signal_sync” and error code is EGL_SUCCESS
Terminate display
	Check return value is true and error code is EGL_SUCCESS
Release thread
	Check return value is true

@SYMTestExpectedResults
Non-null pointer is returned and all strings contain the expected value
*/
TVerdict CEglTest_QueryString::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-REF-0001"));
    INFO_PRINTF1(_L("CEglTest_QueryString::doTestStepL"));

    INFO_PRINTF1(_L("Create display object"));
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
   
    // Initialize display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize"));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major == 1 && minor == 4);   // Version is updated
    
    // Query string for version
    const char* strEglVersion = eglQueryString(iDisplay, EGL_VERSION);    
    TPtrC8 ptrEglVersion(reinterpret_cast<const TUint8*>(strEglVersion));
    TEST_EGL_ERROR(ptrEglVersion.Compare(_L8("1.4 Reference EGL")) == 0, EGL_SUCCESS);
    
    // Query string for vendor
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TPtrC8 ptrEglVendor(reinterpret_cast<const TUint8*>(strEglVendor));
    TEST_EGL_ERROR(ptrEglVendor.Compare(_L8("Nokia")) == 0, EGL_SUCCESS);
    
    // Query string for client APIs
    const char* strEglClinentAPI = eglQueryString(iDisplay, EGL_CLIENT_APIS);
    TPtrC8 ptrEglClinentAPI(reinterpret_cast<const TUint8*>(strEglClinentAPI));
    TEST_EGL_ERROR(ptrEglClinentAPI.Compare(_L8("")) == 0, EGL_SUCCESS);
    
    // Query string for extensions
    const char* strEglExtensions = eglQueryString(iDisplay, EGL_EXTENSIONS);
    TPtrC8 ptrEglExtensions(reinterpret_cast<const TUint8*>(strEglExtensions));
    TEST_EGL_ERROR(ptrEglExtensions.Compare(_L8("EGL_KHR_reusable_sync EGL_NOK__private__signal_sync")) == 0, EGL_SUCCESS);
 
    // Terminate display object
    INFO_PRINTF1(_L("Terminate display"));
    TerminateDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

