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


#ifndef __EGLTEST_ENDPOINTSTREAMINGTESTS_H__
#define __EGLTEST_ENDPOINTSTREAMINGTESTS_H__


#include "egltest_endpoint_engine.h"

_LIT(KEGL_EndpointStreamingTests, "EGL_EndpointStreamingTests");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointStreamingTests) : public CEgltest_Local_Engine
    {
public:
    CEglTest_LocalTestStep_EndpointStreamingTests();
    };


_LIT(KEGL_EndpointStreamingDispXTimesInStream, "EGL_EndpointStreamingDispXTimesInStream");
NONSHARABLE_CLASS(CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream): public CLocalTestStepBase
    {
public:
    CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream();
    ~CEglTest_LocalTestStep_EndpointStreamingDispXTimesInStream();
    virtual TVerdict doTestStepL();
    virtual void DoPreambleL();
    virtual void DoPostambleL();
private:
    void RunScript(const TEngineTestCase *aTestCases, TRemoteTestParams &params);
    void DispXTimesInStreamTestL(TSurfaceType aSurfaceType);
    TUint TicksForDisplayXTimesL(CSurface* aSurface, TInt aNumTimes);
    static void CleanupRemoteTestStep(TAny* aStreamingDispXTimesInStream);
private:
    TRemoteTestParams iParams;
    };


#endif
