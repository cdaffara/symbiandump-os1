/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __EGLTEST_THREADEDSTRESS_H__
#define __EGLTEST_THREADEDSTRESS_H__


#include "localtestbase.h"
#include "remotetestbase.h"


_LIT(KEGL_EndpointCrazyThreading, "EGL_EndpointCrazyThreading");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointCrazyThreading) : public CLocalTestStepBase
    {
public:
    CEglTest_LocalTestStep_EndpointCrazyThreading();
    ~CEglTest_LocalTestStep_EndpointCrazyThreading();
    void DoPreambleL();
    void DoPostambleL();
    TVerdict doTestStepL();
    };


_LIT(KEGL_EndpointOutOfHeapMemory, "EGL_EndpointOutOfHeapMemory");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointOutOfHeapMemory) : public CLocalTestStepBase
    {
public:
CEglTest_LocalTestStep_EndpointOutOfHeapMemory();
    ~CEglTest_LocalTestStep_EndpointOutOfHeapMemory();
    void DoPreambleL();
    void DoPostambleL();
    TVerdict doTestStepL();
    };


NONSHARABLE_CLASS(CEglTest_RemoteTestStep_EndpointThreadStress) : public CRemoteTestStepBase
    {
public:
    CEglTest_RemoteTestStep_EndpointThreadStress();
    ~CEglTest_RemoteTestStep_EndpointThreadStress();
    
    virtual TRemoteTestVerdict DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams);
    virtual TRemoteTestVerdict DoStartRemoteTestStepL(const TRemoteTestParams& aMessageIn);
    virtual TRemoteTestVerdict DoEndRemoteTestStepL(const TRemoteTestParams& aMessageIn);
    
    virtual TInt Timeout() const;
    
private:
    TRemoteTestVerdict CrazyThreadingTestCaseL(const TRemoteTestParams& aParams);
    TRemoteTestVerdict OutOfHeapMemoryTestCaseL(const TRemoteTestParams& aParams);
    TRemoteTestVerdict DoOutOfHeapMemoryTestCaseL(const TRemoteTestParams& aParams);
    };


#endif
