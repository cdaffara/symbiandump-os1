/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __EGLTEST_ENDPOINTTEARING_H__
#define __EGLTEST_ENDPOINTTEARING_H__


#include "localtestbase.h"
#include "remotetestbase.h"


_LIT(KEGL_EndpointTearing, "EGL_EndpointTearing");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointTearing) : public CLocalTestStepBase
    {
public:
    CEglTest_LocalTestStep_EndpointTearing();
    ~CEglTest_LocalTestStep_EndpointTearing();
    void DoPreambleL();
    void DoPostambleL();
    TVerdict doTestStepL();
    
private:
    void StartDrawThreadL();
    void StopDrawThread();
    static TInt DrawThreadEntryPoint(TAny* aSelf);
    void EnterDrawLoopL();
    
private:
    RThread iDrawThread;
    TSurfaceId iSurfaceId;
    volatile TBool iStopDrawing;
    RFastLock iStartDraw;
    RFastLock iFirstDrawDone;
    };


NONSHARABLE_CLASS(CEglTest_RemoteTestStep_EndpointTearing) : public CRemoteTestStepBase
    {
public:
    CEglTest_RemoteTestStep_EndpointTearing();
    ~CEglTest_RemoteTestStep_EndpointTearing();
    
    virtual TRemoteTestVerdict DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams);
    virtual TRemoteTestVerdict DoStartRemoteTestStepL(const TRemoteTestParams& aMessageIn);
    virtual TRemoteTestVerdict DoEndRemoteTestStepL(const TRemoteTestParams& aMessageIn);
    
private:
    TRemoteTestVerdict TearingTestCaseL(const TRemoteTestParams& aParams);
    TRemoteTestVerdict AcquireAndCheckImageL();
    TRemoteTestVerdict CheckImageL(EGLImageKHR aEglImage);
    
private:
    EGLDisplay iDisplay;
    EGLEndpointNOK iEndpoint;
    };


#endif
