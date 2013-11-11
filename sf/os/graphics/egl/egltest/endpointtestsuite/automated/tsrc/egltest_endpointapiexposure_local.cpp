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


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "egltest_endpointapiexposure.h"


CEglTest_LocalTestStep_EndpointApiExposure::CEglTest_LocalTestStep_EndpointApiExposure() :
    CLocalTestStepBase(ETestUidEndpointApiExposure)
    {
    //Temp solution. We probably want a local side Egl helper class.
    //If this fails, the test will fail in a round about way with EGL_NOT_INITIALIZED.
    eglInitialize(eglGetDisplay(EGL_DEFAULT_DISPLAY), NULL, NULL);
    }


CEglTest_LocalTestStep_EndpointApiExposure::~CEglTest_LocalTestStep_EndpointApiExposure()
    {
    //Temp Solution. See note above.
    eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));
    }


void CEglTest_LocalTestStep_EndpointApiExposure::DoPreambleL()
    {
    //Register the test id.
    _LIT(KTestId, "563");
    RegisterTestIdsL(KTestId);
    SetCurrentTestIds(KTestId);
    }


TVerdict CEglTest_LocalTestStep_EndpointApiExposure::doTestStepL()
    {
    //First, test the local side.

    //temp solution. We probably want an Egl helper class for the local side too.
    eglInitialize(eglGetDisplay(EGL_DEFAULT_DISPLAY), NULL, NULL);

    //Check that the extension does not exist in the egl implementation.
    TPtrC8 extensionString((TUint8*)eglQueryString(eglGetDisplay(EGL_DEFAULT_DISPLAY), EGL_EXTENSIONS));
    _LIT8(KExtensionName, "EGL_NOK_image_endpoint");
    if(extensionString.Find(KExtensionName) != KErrNotFound)
        {
        ERR_PRINTF1(_L("Incorrect result for extensionString"));
        INFO_PRINTF1(_L("Rest of test is being skipped due to failure."));
        eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));
        SetTestStepResult(EFail);
        return TestStepResult();
        }
    else
        {
        INFO_PRINTF1(_L("Correct result for extensionString"));
        }

    //If this function leaves, a function pointer was returned from EGL.
    //Since we are outside wserv, we expect EGL never to advertise the existance
    //of the Endpoint API.
    TRAPD(err, AttemptToGetProcAddressForAllEndpointFunctionsL());
    if(err != KErrNone)
        {
        INFO_PRINTF1(_L("Rest of test is being skipped due to failure."));
        eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));
        SetTestStepResult(EFail);
        return TestStepResult();
        }

    eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));


    //Now test remote side.

    //Params for the remote test step.
    TRemoteTestParams params;

    //Run the test step and return the result.
    StartRemoteTestStep(TRemoteTestParams());
    RunRemoteTestCase(0, params);
    EndRemoteTestStep(TRemoteTestParams());

    return TestStepResult();
    }


void CEglTest_LocalTestStep_EndpointApiExposure::AttemptToGetProcAddressForAllEndpointFunctionsL()
    {
    //Attempt to get the proc address for each endpoint function. The expected outcome is
    //for them all to fail. If any succeed, ProcAddressL() will leave.
    ProcAddressL("eglCreateEndpointNOK");
    ProcAddressL("eglDestroyEndpointNOK");
    ProcAddressL("eglGetEndpointAttribNOK");
    ProcAddressL("eglSetEndpointAttribNOK");
    ProcAddressL("eglEndpointBeginStreamingNOK");
    ProcAddressL("eglEndpointEndStreamingNOK");
    ProcAddressL("eglAcquireImageNOK");
    ProcAddressL("eglReleaseImageNOK");
    ProcAddressL("eglGetEndpointDirtyAreaNOK");
    ProcAddressL("eglEndpointRequestNotificationNOK");
    ProcAddressL("eglEndpointCancelNotificationNOK");
    }


void CEglTest_LocalTestStep_EndpointApiExposure::ProcAddressL(const char *aProcName)
    {
    //Convert the 8bit string to a 16 bit descriptor for logging.
    //None of the function names are longer than 128 chars.
    TBuf<128> procName;
    procName.Copy(TPtrC8((TText8*)aProcName));

    //Attempt to get the function pointer. We are expecting to fail since we are outside wserv.
    if(eglGetProcAddress(aProcName) != NULL)
        {
        ERR_PRINTF2(_L("Incorrect result for funcPtr: %S"), &procName);
        User::Leave(KErrUnknown);
        }
    else
        {
        INFO_PRINTF2(_L("Correct result for funcPtr: %S"), &procName);
        }
    }

