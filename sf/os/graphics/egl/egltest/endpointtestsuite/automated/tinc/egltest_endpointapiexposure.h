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
 @internalComponent - Internal Symbian test code
*/


#ifndef __EGLTEST_ENDPOINTAPIEXPOSURE_H__
#define __EGLTEST_ENDPOINTAPIEXPOSURE_H__


#include "localtestbase.h"
#include "remotetestbase.h"


_LIT(KEGL_EndpointApiExposure, "EGL_EndpointApiExposure");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointApiExposure) : public CLocalTestStepBase
    {
public:
    CEglTest_LocalTestStep_EndpointApiExposure();
    ~CEglTest_LocalTestStep_EndpointApiExposure();
    TVerdict doTestStepL();
    void DoPreambleL();
private:
    void AttemptToGetProcAddressForAllEndpointFunctionsL();
    void ProcAddressL(const char *aProcName);
    };


NONSHARABLE_CLASS(CEglTest_RemoteTestStep_EndpointApiExposure) : public CRemoteTestStepBase
    {
public:
    CEglTest_RemoteTestStep_EndpointApiExposure();
    ~CEglTest_RemoteTestStep_EndpointApiExposure();
    TRemoteTestVerdict DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams);

    //Test Cases:
    TRemoteTestVerdict RemoteApiExposureTestCase(const TRemoteTestParams& aParams);

private:
    void AttemptToGetProcAddressForAllEndpointFunctionsL();
    void ProcAddressL(const char *aProcName);
    };


#endif

