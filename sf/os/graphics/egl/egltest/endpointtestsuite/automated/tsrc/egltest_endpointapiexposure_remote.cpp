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


CEglTest_RemoteTestStep_EndpointApiExposure::CEglTest_RemoteTestStep_EndpointApiExposure() :
    CRemoteTestStepBase(ETestUidEndpointApiExposure)
    {
    //Temp solution. We probably want a remote side Egl helper class.
    //If this fails, the test will fail in a round about way with EGL_NOT_INITIALIZED.
    eglInitialize(eglGetDisplay(EGL_DEFAULT_DISPLAY), NULL, NULL);
    }


CEglTest_RemoteTestStep_EndpointApiExposure::~CEglTest_RemoteTestStep_EndpointApiExposure()
    {
    //Temp Solution. See note above.
    eglTerminate(eglGetDisplay(EGL_DEFAULT_DISPLAY));
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointApiExposure::DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams)
    {
    switch(aTestCase)
        {
        case 0:     return RemoteApiExposureTestCase(aParams);
        default:    return ERtvAbort;
        }
    }


TRemoteTestVerdict CEglTest_RemoteTestStep_EndpointApiExposure::RemoteApiExposureTestCase(const TRemoteTestParams& /*aParams*/)
    {
    //Check that the extension exists in the egl implementation.
    TPtrC8 extensionString((TUint8*)eglQueryString(eglGetDisplay(EGL_DEFAULT_DISPLAY), EGL_EXTENSIONS));
    _LIT8(KExtensionName, "EGL_NOK_image_endpoint");
    if(extensionString.Find(KExtensionName) == KErrNotFound)
        {
        REMOTE_ERR_PRINTF1(_L("Incorrect result for extensionString"));
        REMOTE_INFO_PRINTF1(_L("Rest of test is being skipped due to failure."));
        return ERtvFail;
        }
    else
        {
        REMOTE_INFO_PRINTF1(_L("Correct result for extensionString"));
        }

    //If this function leaves, a function pointer was not returned from EGL.
    //Since we are inside wserv, we expect EGL to always advertise the existance
    //of the Endpoint API.
    TRAPD(err, AttemptToGetProcAddressForAllEndpointFunctionsL());
    if(err != KErrNone)
        {
        REMOTE_INFO_PRINTF1(_L("Rest of test is being skipped due to failure."));
        return ERtvFail;
        }
    eglReleaseThread(); // otherwise we leak handles
    return ERtvPass;
    }


void CEglTest_RemoteTestStep_EndpointApiExposure::AttemptToGetProcAddressForAllEndpointFunctionsL()
    {
    //Attempt to get the proc address for each endpoint function. The expected outcome is
    //for them all to succeed. If any fail, ProcAddressL() will leave.
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


void CEglTest_RemoteTestStep_EndpointApiExposure::ProcAddressL(const char *aProcName)
    {
    //Convert the 8bit string to a 16 bit descriptor for logging.
    //None of the function names are longer than 128 chars.
    TBuf<128> procName;
    procName.Copy(TPtrC8((TText8*)aProcName));

    //Attempt to get the function pointer. We are expecting to succeed since we are inside wserv.
    if(eglGetProcAddress(aProcName) == NULL)
        {
        REMOTE_ERR_PRINTF2(_L("Incorrect result for funcPtr: %S"), &procName);
        User::Leave(KErrUnknown);
        }
    else
        {
        REMOTE_INFO_PRINTF2(_L("Correct result for funcPtr: %S"), &procName);
        }
    }
