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
#include "egltest_geterroranddisplay.h"

/**
@SYMTestCaseID GRAPHICS-EGL-0507

@SYMPREQ 2400

@SYMTestPriority 1

@SYMTestCaseDesc
Test eglGetError behavior

@SYMTestExpectedResults
Error code is set according to EGL specification

@SYMTestActions
Call eglGetError
Check error is EGL_SUCCESS

Get default display
Check error code is EGL_SUCCESS

Query string for version
Check return value is null and error code is EGL_NOT_INITIALIZED

Call eglGetError
Check error is EGL_SUCCESS

Query string for vendor
Check return value is null and error code is EGL_NOT_INITIALIZED

Release thread
Call eglGetError
Check error is EGL_SUCCESS

Release thread
Check return value is true
*/
TVerdict CEglTest_GetError::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0507"));
    INFO_PRINTF1(_L("CEglTest_GetError::doTestStepL"));

    TEST(eglGetError() == EGL_SUCCESS);
    
    // Create display object
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Query string for version
    const char* strEglVersion = eglQueryString(iDisplay, EGL_VERSION);
    TEST_EGL_ERROR(strEglVersion == NULL, EGL_NOT_INITIALIZED);

    // Query string for vendor
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor == NULL, EGL_NOT_INITIALIZED);

    TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0508

@SYMPREQ 2400

@SYMTestPriority 1

@SYMTestCaseDesc
Test eglGetError behavior and to prove that error is specific to each thread

@SYMTestExpectedResults
Error code is set according to EGL specification

@SYMTestActions
Main Thread
Call eglGetError
Check error is EGL_SUCCESS

Get default display
Check error code is EGL_SUCCESS

Query string for version
Check return value is null but do not check error code

Launch thread A
Wait for thread A to exit
    Thread A
    Call eglGetError
    Check error is EGL_SUCCESS

    Get default display
    Check error code is EGL_SUCCESS

    Release thread
    Check return value is true
    Exit

Main Thread
Check error code is EGL_NOT_INITIALIZED

Query string for version
Check return value is null and error code is EGL_NOT_INITIALIZED

Release thread
Check return value is true
Exit
*/
TVerdict CEglTest_GetErrorMultiThread::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0508"));
    INFO_PRINTF1(_L("CEglTest_GetErrorMultiThread::doTestStepL"));

    TEST(eglGetError() == EGL_SUCCESS);

    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    INFO_PRINTF1(_L("Query string for version..."));
    const char* strEglVersion = eglQueryString(iDisplay, EGL_VERSION);
    TEST(strEglVersion == NULL);
    
    INFO_PRINTF1(_L("Launch thread A"));
    Test_MultiThreadL(1, ETrue);
    
    // Check that eglGetError is specific to each thread
    TEST(eglGetError() == EGL_NOT_INITIALIZED);
    
    INFO_PRINTF1(_L("Query string for vendor..."));
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor == NULL, EGL_NOT_INITIALIZED);

    TEST(eglReleaseThread() == EGL_TRUE);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_GetErrorMultiThread::doThreadFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_GetErrorMultiThread::doThreadFunctionL, Thread %d"),aIdx);
    
    TEST(eglGetError() == EGL_SUCCESS);
    
    INFO_PRINTF2(_L("Create display object from thread: %d"), aIdx);
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);    
    TEST_EGL_ERROR(dpy == iDisplay, EGL_SUCCESS);
    
    TEST(eglReleaseThread() == EGL_TRUE);
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0509

@SYMPREQ 2400

@SYMTestPriority 1

@SYMTestPurpose
Test eglGetDisplay, eglInitialize, eglTerminate and eglQueryString behaviors from a single thread

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Basic positive test

@SYMTestActions
Get default display
Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS

Initialise display
Check return value is true, error code is EGL_SUCCESS and version is updated

Query string for vendor
Check return value is not null and error code is EGL_SUCCESS

Terminate display
Check return value is true and error code is EGL_SUCCESS

Release thread
Check return value is true
*/
TVerdict CEglTest_Display_Positive_Basic::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0509"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_Basic::doTestStepL"));

    // Create display object
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Initialise display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);   // Version is updated
    
    // Query string for vendor
    INFO_PRINTF1(_L("Query string for vendor..."));
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);

    // Terminate display
    TerminateDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Release thread
    TEST(eglReleaseThread() == EGL_TRUE);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0510
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize and eglTerminate behaviors from a single thread

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Invalid display test

@SYMTestActions
Get display other than default
Check return value is EGL_NO_DISPLAY and error code is EGL_SUCCESS

Initialise display
Check return value is false, error code is EGL_BAD_DISPLAY and version is not updated

Terminate display
Check return value is false and error code is EGL_BAD_DISPLAY

Release thread
Check return value is true
*/
TVerdict CEglTest_Display_Negative_InvalidDisplay::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0510"));
    INFO_PRINTF1(_L("CEglTest_Display_Negative_InvalidDisplay::doTestStepL"));

    // Get display other than default
    const NativeDisplayType nonDefaultDisplay = -1;
    iDisplay = eglGetDisplay(nonDefaultDisplay);
    TEST_EGL_ERROR(iDisplay == EGL_NO_DISPLAY, EGL_SUCCESS);
    
    // Initialise display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor) == EGL_FALSE, EGL_BAD_DISPLAY);
    TEST(major == -1 && minor == -1);   // Version is Not updated

    // Terminate display
    TEST_EGL_ERROR(eglTerminate(iDisplay) == EGL_FALSE, EGL_BAD_DISPLAY);
    
    // Release thread
    TEST(eglReleaseThread() == EGL_TRUE);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0511
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize, eglTerminate and eglQueryString behaviors from a single thread

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Un-initialised display usage test

@SYMTestActions
Get default display
Check return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS

Query string for vendor
Check return value is null and error code is EGL_NOT_INITIALIZED

Terminate display
Check return value is true and error code is EGL_SUCCESS

Release thread
Check return value is true
*/
TVerdict CEglTest_Display_Negative_NotInitializedDisplay::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0511"));
    INFO_PRINTF1(_L("CEglTest_Display_Negative_NotInitializedDisplay::doTestStepL"));

    // Create display object
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Query string for vendor
    INFO_PRINTF1(_L("Query string for vendor"));
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor == NULL, EGL_NOT_INITIALIZED);

    // Terminate display
    TerminateDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Release thread
    TEST(eglReleaseThread() == EGL_TRUE);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0512
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize, eglTerminate and eglQueryString behaviors from a single thread

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Reinitialise display test

@SYMTestActions
Get default display
Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS

Initialise display
Check return value is true, error code is EGL_SUCCESS and version is updated

Terminate display
Check return value is true and error code is EGL_SUCCESS

Query string for version
Check return value is null and error code is EGL_NOT_INITIALIZED

Initialise display
Check return value is true, error code is EGL_SUCCESS and version is updated

Query string for version
Check return value is not null and error code is EGL_SUCCESS

Terminate display
Check return value is true and error code is EGL_SUCCESS

Release thread
Check return value is true
*/
TVerdict CEglTest_Display_Positive_ReinitializeDisplay::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0512"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_ReinitializeDisplay::doTestStepL"));

    // Create display object
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Initialise display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);   // Version is updated
    
    // Terminate display
    INFO_PRINTF1(_L("Calling eglTerminate..."));
    TEST_EGL_ERROR(eglTerminate(iDisplay), EGL_SUCCESS);
    
    // Query string for version
    INFO_PRINTF1(_L("Query string for version..."));
    const char* strEglVersion = eglQueryString(iDisplay, EGL_VERSION);
    TEST_EGL_ERROR(strEglVersion == NULL, EGL_NOT_INITIALIZED);
    
    // Re-Initialise display object
    major = -1;
    minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);   // Version is updated
    
    // Query string for version
    INFO_PRINTF1(_L("Query string for version..."));
    strEglVersion = eglQueryString(iDisplay, EGL_VERSION);
    TEST_EGL_ERROR(strEglVersion != NULL, EGL_SUCCESS);

    // Terminate display
    TerminateDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Release thread
    TEST(eglReleaseThread() == EGL_TRUE);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0513
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize and eglTerminate behavior from a single thread

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Multiple initialisation test

@SYMTestActions
Get default display
Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS

Initialise display
Check return value is true, error code is EGL_SUCCESS and version is updated

Initialise display
Check return value is true, error code is EGL_SUCCESS and version is updated

Terminate display
Check return value is true and error code is EGL_SUCCESS

Query string for version
Check return value is null and error code is EGL_NOT_INITIALIZED

Release thread
Check return value is true
*/
TVerdict CEglTest_Display_Positive_MultipleInitialization::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0513"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_MultipleInitialization::doTestStepL"));

    // Create display object
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Initialise display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);   // Version is updated
    
    // Initialise display object
    major = -1; 
    minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);   // Version is updated
    
    // Terminate display
    INFO_PRINTF1(_L("Calling eglTerminate..."));
    TEST_EGL_ERROR(eglTerminate(iDisplay), EGL_SUCCESS);
    
    // Query string for version
    INFO_PRINTF1(_L("Query string for version..."));
    const char* strEglVersion = eglQueryString(iDisplay, EGL_VERSION);
    TEST_EGL_ERROR(strEglVersion == NULL, EGL_NOT_INITIALIZED);
    
    // Release thread
    TEST(eglReleaseThread() == EGL_TRUE);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0514
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize and eglTerminate behavior from a single thread

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Multiple termination test

@SYMTestActions
Get default display
Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS

Initialise display
Check return value is true, error code is EGL_SUCCESS and version is updated

Query string for extensions
Check return value is not null and error code is EGL_SUCCESS

Terminate display
Check return value is true and error code is EGL_SUCCESS

Terminate display
Check return value is true and error code is EGL_SUCCESS

Query string for extensions
Check return value is null and error code is EGL_NOT_INITIALIZED

Release thread
Check return value is true

Release thread
Check return value is true
*/
TVerdict CEglTest_Display_Positive_MultipleTermination::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0514"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_MultipleTermination::doTestStepL"));

    // Create display object
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Initialise display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);   // Version is updated
    
    // Query string for extensions
    INFO_PRINTF1(_L("Query string for extensions..."));
    const char* strEglExtensions = eglQueryString(iDisplay, EGL_EXTENSIONS);
    TEST_EGL_ERROR(strEglExtensions != NULL, EGL_SUCCESS);
    
    // Terminate display
    INFO_PRINTF1(_L("Calling eglTerminate..."));
    TEST_EGL_ERROR(eglTerminate(iDisplay), EGL_SUCCESS);
    
    // Terminate display
    INFO_PRINTF1(_L("Calling eglTerminate..."));
    TEST_EGL_ERROR(eglTerminate(iDisplay), EGL_SUCCESS);

    // Query string for extensions
    INFO_PRINTF1(_L("Query string for extensions..."));
    strEglExtensions = eglQueryString(iDisplay, EGL_EXTENSIONS);
    TEST_EGL_ERROR(strEglExtensions == NULL, EGL_NOT_INITIALIZED);
    
    // Release thread
    TEST(eglReleaseThread() == EGL_TRUE);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0515
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize, eglTerminate and eglQueryString behaviors from multiple threads within a process

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Basic multi threaded test

@SYMTestActions
Main Thread
Get default display
Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS

Initialise display
Check return value is true, error code is EGL_SUCCESS and version is updated

Query string for vendor
Check return value is not null and error code is EGL_SUCCESS

Launch thread A
Wait for thread A to exit
    Thread A
    Get default display
    Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS
    
    Initialise display
    Check return value is true, error code is EGL_SUCCESS and version is updated
    
    Query string for vendor
    Check return value is not null and error code is EGL_SUCCESS
    
    Terminate display
    Check return value is true and error code is EGL_SUCCESS
    
    Release thread
    Check return value is true
    Exit

Main Thread
Query string for vendor
Check return value is null and error code is EGL_NOT_INITIALIZED

Terminate display
Check return value is true and error code is EGL_SUCCESS

Release thread
Check return value is true
Exit
*/
TVerdict CEglTest_Display_Positive_Multithread_Basic::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0515"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_Multithread_Basic::doTestStepL"));

    // Create display object
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Initialise display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);
        
    // Query string for vendor
    INFO_PRINTF1(_L("Query string for vendor..."));
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);

    INFO_PRINTF1(_L("Launch extra thread..."));
    Test_MultiThreadL(1, ETrue);

    // Query string for vendor
    INFO_PRINTF1(_L("Query string for vendor..."));
    strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor == NULL, EGL_NOT_INITIALIZED);
  
    // Terminate display
    TerminateDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Release thread
    TEST(eglReleaseThread() == EGL_TRUE);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_Display_Positive_Multithread_Basic::doThreadFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_Display_Positive_Multithread_Basic::doThreadFunctionL, Thread %d"),aIdx);
    
    INFO_PRINTF2(_L("Create display object from thread: %d"), aIdx);
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // Initialise display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);   // Version is updated

    // Query string for vendor
    INFO_PRINTF1(_L("Query string for vendor..."));
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);
    
    // Terminate display
    INFO_PRINTF1(_L("Calling eglTerminate..."));
    TEST_EGL_ERROR(eglTerminate(iDisplay), EGL_SUCCESS);
    
    TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0516
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize and eglTerminate behavior from multiple threads within a process

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Initialise and terminate display from one thread only

@SYMTestActions
Main Thread
Launch thread A and B
Wait for both thread A and B to exit

    Thread A
    Get default display
    Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS
    
    Initialise display
    Check return value is true, error code is EGL_SUCCESS and version is 1.4
    
    Rendezvous
    
    Thread A and B
    Query string for vendor
    Check return value is not null and error code is EGL_SUCCESS
    
    Rendezvous
    
    Thread A
    Terminate display
    Check return value is true and error code is EGL_SUCCESS
    
    Rendezvous
    
    Thread A and B
    Query string for vendor
    Check return value is null and error code is EGL_NOT_INITIALIZED
    
    Release thread
    Check return value is true
    Exit

Main Thread
Exit
*/
TVerdict CEglTest_Display_Positive_Multithread_InitTerminateInOneTread::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0516"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_Multithread_InitTerminateInOneTread::doTestStepL"));

    INFO_PRINTF1(_L("Launch two threads"));
    Test_MultiThreadL(2, ETrue);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_Display_Positive_Multithread_InitTerminateInOneTread::doThreadFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_Display_Positive_Multithread_InitTerminateInOneTread::doThreadFunctionL, Thread %d"),aIdx);
    
    if(aIdx == 0)
        {
        INFO_PRINTF2(_L("Create display object from thread: %d"), aIdx);
        GetDisplayL();
        TEST(eglGetError() == EGL_SUCCESS);
            
        // Initialise display object
        EGLint major = -1, minor = -1;
        INFO_PRINTF2(_L("Calling eglInitialize from thread %d"),aIdx);
        TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
        TEST(major != -1 && minor != -1);   // Version is updated
        }
    
    Rendezvous(aIdx);
    
    // Query string for vendor
    INFO_PRINTF2(_L("Query string for vendor from thread %d"), aIdx);
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);
    
    Rendezvous(aIdx);
    
    if(aIdx == 0)
        {
        INFO_PRINTF2(_L("Calling eglTerminate... from thread: %d"), aIdx);
        TEST_EGL_ERROR(eglTerminate(iDisplay), EGL_SUCCESS);
        }
    
    Rendezvous(aIdx);
    
    // Query string for vendor
    INFO_PRINTF2(_L("Query string for vendor from thread %d"), aIdx);
    strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor == NULL, EGL_NOT_INITIALIZED);
    
    TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
    }

/*
@SYMTestCaseID GRAPHICS-EGL-0517
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize, eglTerminate and eglQueryString behaviors from multiple threads within a process

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Initialise display from one thread and terminate from another thread

@SYMTestActions
Main Thread
Launch thread A and B
Wait for both thread A and B to exit

    Thread A
    Get default display
    Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS
    
    Initialise display
    Check return value is true, error code is EGL_SUCCESS and version is 1.4
    
    Rendezvous
    
    Thread A and B
    Query string for vendor
    Check return value is not null and error code is EGL_SUCCESS
    
    Rendezvous
    
    Thread B
    Terminate display
    Check return value is true and error code is EGL_SUCCESS
    
    Rendezvous
    
    Thread A and B
    Query string for vendor
    Check return value is null and error code is EGL_NOT_INITIALIZED
    
    Release thread
    Check return value is true
    Exit

Main Thread
Exit
*/
TVerdict CEglTest_Display_Positive_Multithread_InitTerminateFromDifferentThread::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0517"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_Multithread_InitTerminateFromDifferentThread::doTestStepL"));

    INFO_PRINTF1(_L("Launch two threads"));
    Test_MultiThreadL(2, ETrue);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_Display_Positive_Multithread_InitTerminateFromDifferentThread::doThreadFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_Display_Positive_Multithread_InitTerminateFromDifferentThread::doThreadFunctionL, Thread %d"),aIdx);
    
    if(aIdx == 0)
        {
        INFO_PRINTF2(_L("Create display object from thread: %d"), aIdx);
        GetDisplayL();
        TEST(eglGetError() == EGL_SUCCESS);
            
        // Initialise display object
        EGLint major = -1, minor = -1;
        INFO_PRINTF2(_L("Calling eglInitialize from thread %d"),aIdx);
        TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
        TEST(major != -1 && minor != -1);
        }
    
    Rendezvous(aIdx);
    
    // Query string for vendor
    INFO_PRINTF2(_L("Query string for vendor from thread %d"), aIdx);
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);

    Rendezvous(aIdx);
    
    if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Calling eglTerminate... from thread: %d"), aIdx);
        TEST_EGL_ERROR(eglTerminate(iDisplay), EGL_SUCCESS);
        }
    
    Rendezvous(aIdx);
    
    // Query string for vendor
    INFO_PRINTF2(_L("Query string for vendor from thread %d"), aIdx);
    strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor == NULL, EGL_NOT_INITIALIZED);
    
    TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0518
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize, eglTerminate and eglQueryString behaviors from multiple threads within a process

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Test large number of threads

@SYMTestActions
Main thread
Get default display
Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS

Launch 100 threads
Wait for all threads to exit

From each spawned thread
Get default display
Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS

Initialise display
Check return value is true, error code is EGL_SUCCESS and version is updated

Query string for vendor
Check return value is not null and error code is EGL_SUCCESS
            
Release thread
Check return value is true
Exit

Main thread
Terminate display
Check return value is true and error code is EGL_SUCCESS

Query string for vendor
Check return value is null and error code is EGL_NOT_INITIALIZED

Release thread
Check return value is true
Exit
*/
TVerdict CEglTest_Display_Positive_Multithread_Stress::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0518"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_Multithread_Stress::doTestStepL"));

    INFO_PRINTF1(_L("Create display object from main thread"));
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
    
    // launch 100 threads.
    Test_MultiThreadL(100, ETrue);
    
    INFO_PRINTF1(_L("Calling eglTerminate from main thread"));
    TEST_EGL_ERROR(eglTerminate(iDisplay), EGL_SUCCESS);
    
    // Query string for vendor
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor == NULL, EGL_NOT_INITIALIZED);
    
    TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_Display_Positive_Multithread_Stress::doThreadFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_Display_Positive_Multithread_Stress::doThreadFunctionL, Thread %d"),aIdx);

    //Create display objec
    GetDisplayL();
    TEST(eglGetError() == EGL_SUCCESS);
        
    // Initialise display object
    EGLint major = -1, minor = -1;
    INFO_PRINTF1(_L("Calling eglInitialize..."));
    TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
    TEST(major != -1 && minor != -1);   // Version is updated

    // Query string for vendor
    const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
    TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);
    
    TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0519
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test eglGetDisplay, eglInitialize, eglTerminate and eglQueryString behaviors from multiple processes

@SYMTestExpectedResults
Return value and error code shall conform to EGL specification

@SYMTestCaseDesc
Basic multi process test

@SYMTestActions
Main Process
Launch process A and B
Wait until process A and B are terminated

    Process A
    Get default display
    Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS
    
    Initialise display
    Check return value is true, error code is EGL_SUCCESS and version is updated
    
    Query string for vendor
    Check return value is not null and error code is EGL_SUCCESS
    
    Rendezvous
    
    Process B
    Get default display
    Check the return value is not EGL_NO_DISPLAY and error code is EGL_SUCCESS
    
    Initialise display
    Check return value is true, error code is EGL_SUCCESS and version is updated
    
    Query string for vendor
    Check return value is not null and error code is EGL_SUCCESS
    
    Terminate display
    Check return value is true and error code is EGL_SUCCESS
    
    Release thread
    Check return value is true
    Exit
    
    Rendezvous
    
    Process A
    Query string for vendor
    Check return value is not null and error code is EGL_SUCCESS
    
    Terminate display
    Check return value is true and error code is EGL_SUCCESS
    
    Release thread
    Check return value is true
    Exit

Main Process
Exit
*/
TVerdict CEglTest_Display_Positive_Multiprocess_Basic::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-0519"));
    INFO_PRINTF1(_L("CEglTest_Display_Positive_Multiprocess_Basic::doTestStepL"));

    INFO_PRINTF1(_L("Launch two processes"));
    Test_MultiProcessL(KEglTestStepDllName, 2, KDisplay_Positive_Multiprocess_Basic);
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

void CEglTest_Display_Positive_Multiprocess_Basic::doProcessFunctionL(TInt aIdx)
    {
    INFO_PRINTF2(_L("CEglTest_Display_Positive_Multiprocess_Basic::doThreadFunctionL, Thread %d"),aIdx);
    
    if(aIdx == 0)
        {
        INFO_PRINTF2(_L("Create display object from Process: %d"), aIdx);
        GetDisplayL();
        TEST(eglGetError() == EGL_SUCCESS);
        
        // Initialise display object
        EGLint major = -1, minor = -1;
        INFO_PRINTF2(_L("Calling eglInitialize from Process %d"),aIdx);
        TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
        TEST(major != -1 && minor != -1);   // Version is updated
        
        // Query string for vendor
        INFO_PRINTF2(_L("Query string for vendor from Process %d"),aIdx);
        const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
        TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);
        }
    
    Rendezvous(aIdx);
    
    if(aIdx == 1)
        {
        INFO_PRINTF2(_L("Create display object from Process: %d"), aIdx);
        GetDisplayL();
        TEST(eglGetError() == EGL_SUCCESS);
        
        // Initialise display object
        EGLint major = -1, minor = -1;
        INFO_PRINTF2(_L("Calling eglInitialize from Process %d"),aIdx);
        TEST_EGL_ERROR(eglInitialize(iDisplay, &major, &minor), EGL_SUCCESS);
        TEST(major != -1 && minor != -1);   // Version is updated
        
        // Query string for vendor
        INFO_PRINTF2(_L("Query string for vendor from Process %d"),aIdx);
        const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
        TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);
    
        // Terminate display object
        INFO_PRINTF2(_L("Terminate display from Process %d"),aIdx);
        TerminateDisplayL();
        TEST(eglGetError() == EGL_SUCCESS);
        
        TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
        }
    
    Rendezvous(aIdx);
    
    if(aIdx == 0)
        {
        // Query string for vendor
        INFO_PRINTF2(_L("Query string for vendor from Process %d"),aIdx);
        const char* strEglVendor = eglQueryString(iDisplay, EGL_VENDOR);
        TEST_EGL_ERROR(strEglVendor != NULL, EGL_SUCCESS);
    
        // Terminate display object
        INFO_PRINTF2(_L("Terminate display from Process %d"),aIdx);
        TerminateDisplayL();
        TEST(eglGetError() == EGL_SUCCESS);
        
        TEST_EGL_ERROR(eglReleaseThread() == EGL_TRUE, EGL_SUCCESS);
        }    
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0520
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test the behaviour of eglGetDisplay in a Client heap OOM situation.

@SYMTestExpectedResults
No memory or handle leaks.

@SYMTestCaseDesc
Client heap OOM test

@SYMTestActions
Within a for loop that iterates through increasing memory failure at failAt:
    *mark client heap with __UHEAP_SETFAIL(RHeap::EDeterministic,failAt)
    *mark client heap with __UHEAP_MARK
    *call eglGetDisplay()
    *call eglGetError()
    *reset client heap with __UHEAP_RESET
    *If either egl function call fails and raises EGL_BAD_ALLOC, increase the failAt and restart the loop.
If the procedure was successful release all the resources and check the client heap for memory leaks.
No memory or handle leaks.
*/
TVerdict CEglTest_Display_OOM_ClientHeap::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-520"));
    INFO_PRINTF1(_L("CEglTest_OOM_ClientHeap::doTestStepL"));
    
    EGLint errorCode = EGL_BAD_ALLOC;
    for(TInt failAt=1; errorCode == EGL_BAD_ALLOC; ++failAt)
        {
        INFO_PRINTF2(_L("Set Client heap to fail at %u allocations"), failAt);
        
        __UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
        __UHEAP_MARK;
        EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        errorCode = eglGetError();
        __UHEAP_RESET;
        
        if(display == EGL_NO_DISPLAY)
            {
            // If it's not EGL_BAD_ALLOC, loop will exit naturally.
            TEST(errorCode == EGL_BAD_ALLOC);
            __UHEAP_MARKEND;
            }
        else
            {
            // All memory failure points have been tested. So can exit the test.
            TEST(errorCode == EGL_SUCCESS);
            eglReleaseThread();
            __UHEAP_MARKEND;
            break;            
            }        
        }
    
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

/**
@SYMTestCaseID GRAPHICS-EGL-0521
@SYMPREQ 2400
@SYMTestPriority 1
@SYMTestPurpose
Test the behaviour of eglInitialize in a Client heap OOM situation.

@SYMTestExpectedResults
No memory or handle leaks.

@SYMTestCaseDesc
Client heap OOM test

@SYMTestActions
Get the display without setting memory failure.

Within a for loop that iterates through increasing memory failure at failAt:
    *mark client heap with __UHEAP_SETFAIL(RHeap::EDeterministic,failAt)
    *mark client heap with __UHEAP_MARK
    *call eglInitialize()
    *call eglGetError()
    *reset client heap with __UHEAP_RESET
    *If either egl function call fails and raises EGL_BAD_ALLOC, increase the failAt and restart the loop.
If the procedure was successful release all the resources and check the client heap for memory leaks.
No memory or handle leaks.
*/
TVerdict CEglTest_Display_OOM_ClientHeap_Initialize::doTestStepL()
    {
    SetTestStepID(_L("GRAPHICS-EGL-521"));
    INFO_PRINTF1(_L("CEglTest_Display_OOM_ClientHeap_Initialize::doTestStepL"));
    
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    TEST(eglGetError() == EGL_SUCCESS);
    
    TEST(display != EGL_NO_DISPLAY);
    if(display != EGL_NO_DISPLAY)
        {
        EGLint errorCode = EGL_BAD_ALLOC;
        for(TInt failAt=1; errorCode == EGL_BAD_ALLOC; ++failAt)
            {       
            INFO_PRINTF2(_L("Set Client heap to fail at %u allocations"), failAt);
            __UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
            __UHEAP_MARK;
            EGLBoolean res = eglInitialize(display, NULL, NULL);
            errorCode = eglGetError();
            __UHEAP_RESET;
                    
            if(res == EGL_TRUE)
                {
                // All memory failure points have been tested. So can exit the test.
                TEST(errorCode == EGL_SUCCESS);
                TEST_EGL_ERROR(eglTerminate(display), EGL_SUCCESS);
                __UHEAP_MARKEND;
                 break;            
                }
            else
                {
                // If it's not EGL_BAD_ALLOC, loop will exit naturally.
                TEST(errorCode == EGL_BAD_ALLOC);
                __UHEAP_MARKEND;
                }                                
            }
        }
    
    eglReleaseThread();
  
    RecordTestResultL();
    CloseTMSGraphicsStep();
    return TestStepResult();
    }

