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


#ifndef __EGLTEST_RELEASEIMAGEGLES_H__
#define __EGLTEST_RELEASEIMAGEGLES_H__


#include "localtestbase.h"
#include "remotetestbase.h"


_LIT(KEGL_EndpointReleaseImageGles, "EGL_EndpointReleaseImageGles");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointReleaseImageGles) : public CLocalTestStepBase
    {
public:
CEglTest_LocalTestStep_EndpointReleaseImageGles();
    ~CEglTest_LocalTestStep_EndpointReleaseImageGles();
    void DoPreambleL();
    void DoPostambleL();
    TVerdict doTestStepL();
    };


NONSHARABLE_CLASS(CEglTest_RemoteTestStep_EndpointReleaseImageGles) : public CRemoteTestStepBase
    {
public:
    CEglTest_RemoteTestStep_EndpointReleaseImageGles();
    ~CEglTest_RemoteTestStep_EndpointReleaseImageGles();
    
    virtual TRemoteTestVerdict DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aParams);
    virtual TRemoteTestVerdict DoStartRemoteTestStepL(const TRemoteTestParams& aMessageIn);
    virtual TRemoteTestVerdict DoEndRemoteTestStepL(const TRemoteTestParams& aMessageIn);
    
private:
    TRemoteTestVerdict GlesContextTestCaseL(const TRemoteTestParams& aParams);
    TRemoteTestVerdict DoGlesContextTestCaseL(const TRemoteTestParams& aParams);
    
private:
    EGLDisplay iDisplay;
    CSurface* iSurface;
    EGLEndpointNOK iEndpoint;
    };


#endif
