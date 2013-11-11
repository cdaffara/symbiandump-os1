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
 @internalComponent - Internal Symbian test code
*/

#ifndef __EGLTEST_ENDPOINTIMAGE_H__
#define __EGLTEST_ENDPOINTIMAGE_H__

#include "egltest_endpoint_engine.h"

_LIT(KResultProcessQueueName, "ProcessTestEnvResultQueue");
_LIT(KParamsProcessQueueName, "ProcessTestEnvParamsQueue");


enum TEndpointLifetimeExit
    {
    EEndpointLifetimeNormalExit,
    EEnfpointLifetimePanicExit,
    };

_LIT(KEGL_EndpointImage, "EGL_EndpointImage");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointImage) : public CEgltest_Local_Engine
    {
public:
    CEglTest_LocalTestStep_EndpointImage();
    };

_LIT(KEGL_EndpointImageLifetime, "EGL_EndpointImageLifetime");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointImageLifetime): public CLocalTestStepBase
    {
public:
    CEglTest_LocalTestStep_EndpointImageLifetime();
    ~CEglTest_LocalTestStep_EndpointImageLifetime();
    virtual TVerdict doTestStepL();
    virtual void DoPreambleL();
    virtual void DoPostambleL();
private:
    void RunScript(const TEngineTestCase *aTestCases, TRemoteTestParams &params);
    void EndpointLifetimeTestL(TSurfaceType aSurfaceType, TInt aExitType);

private:
    struct TProcessStatus
        {
        RProcess        iProcess;
        TRequestStatus  iStatus;
        } iProcessStatus;

    RMsgQueue<TInt> iProcessResultInQueue;
    RMsgQueue<TSurfaceId> iProcessParamsOutQueue;
    };

_LIT(KEGL_EndpointImageLifetimeProcess, "EGL_EndpointImageLifetimeProcess");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointImageLifetimeProcess): public CTestStep //CTTMSGraphicsStep
    {
public:
    CEglTest_LocalTestStep_EndpointImageLifetimeProcess();
    virtual ~CEglTest_LocalTestStep_EndpointImageLifetimeProcess();
    virtual TVerdict doTestStepL();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    static void MainL();

private:
    RMsgQueue<TInt> iResultOutQueue;
    RMsgQueue<TSurfaceId> iParamsInQueue;
    };

#endif  // __EGLTEST_ENDPOINTIMAGE_H__
