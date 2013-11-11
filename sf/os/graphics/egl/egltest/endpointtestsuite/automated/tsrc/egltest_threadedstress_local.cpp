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


#include "egltest_threadedstress.h"


/**
@SYMTestCaseID GRAPHICS-EGL-0594

@SYMTestPriority 2

@SYMPREQ 2670

@SYMREQ

@SYMTestCaseDesc
To ensure thread safety of endpoint implementation

@SYMTestActions
Create 4 set of threads with each performing following steps:
1.  Repeat the following steps (2-16) for 60 seconds
2.  Create surface
3.  Random Delay 
4.  Create endpoint. 
5.  Random Delay
6.  Draw content to surface
7.  Random Delay 
8.  Submit Update
9.  Random Delay 
10. Acquire image
11. Random Delay
12. Compare image with expected value (??)
13. Release image
14. Random Delay 
15. Destroy endpoint
16. Destroy Surface

@SYMTestExpectedResults
The test runs to completion.
*/


CEglTest_LocalTestStep_EndpointCrazyThreading::CEglTest_LocalTestStep_EndpointCrazyThreading() :
    CLocalTestStepBase(ETestUidEndpointThreadStress)
    {
    }


CEglTest_LocalTestStep_EndpointCrazyThreading::~CEglTest_LocalTestStep_EndpointCrazyThreading()
    {
    }


void CEglTest_LocalTestStep_EndpointCrazyThreading::DoPreambleL()
    {
    //Register the test id.
    _LIT(KTestId, "594");
    RegisterTestIdsL(KTestId);
    SetCurrentTestIds(KTestId);
    }


void CEglTest_LocalTestStep_EndpointCrazyThreading::DoPostambleL()
    {
    }


TVerdict CEglTest_LocalTestStep_EndpointCrazyThreading::doTestStepL()
    {
    StartRemoteTestStep(TRemoteTestParams());
    RunRemoteTestCase(0, TRemoteTestParams());
    EndRemoteTestStep(TRemoteTestParams());
    return TestStepResult();
    }


/**
@SYMTestCaseID GRAPHICS-EGL-0601

@SYMTestPriority 2

@SYMPREQ 2670

@SYMREQ

@SYMTestCaseDesc
To ensure endpoint operations can succeed in low memory condition.

@SYMTestActions
Use Method B.
1.  Thread 1: 
1.1.    Create a heap that can expand to maximum available system memory size
1.2.    Allocate random sizes of memory on that system heap until failure occurs, recording and counting as N.
1.3.    Delay
1.4.     De-allocate last N/4 allocations. 
1.5.     Delay
1.6.     Go to 1.2
2.  Thread 2: 
2.1.    Create surface 100x100
2.2.    Draw content to surface
2.3.    Submit Update
2.4.    Create endpoint. 
2.5.    Acquire image
2.6.    Verify image content with pixel sample
2.7.    Release image
2.8.    Destroy endpoint
2.9.    Destroy Surface
3.  Repeat Step 2 1000 times.   After every 200 iterations check memory usage as described in 7.3.1.
4.  Destroy thread 2 and 1.

@SYMTestExpectedResults
The test runs to completion.
*/


CEglTest_LocalTestStep_EndpointOutOfHeapMemory::CEglTest_LocalTestStep_EndpointOutOfHeapMemory() :
    CLocalTestStepBase(ETestUidEndpointThreadStress)
    {
    }


CEglTest_LocalTestStep_EndpointOutOfHeapMemory::~CEglTest_LocalTestStep_EndpointOutOfHeapMemory()
    {
    }


void CEglTest_LocalTestStep_EndpointOutOfHeapMemory::DoPreambleL()
    {
    //Register the test id.
    _LIT(KTestId, "601, 625");
    RegisterTestIdsL(KTestId);
    SetCurrentTestIds(KTestId);
    }


void CEglTest_LocalTestStep_EndpointOutOfHeapMemory::DoPostambleL()
    {
    }


TVerdict CEglTest_LocalTestStep_EndpointOutOfHeapMemory::doTestStepL()
    {
    TRemoteTestParams params;
    StartRemoteTestStep(TRemoteTestParams());
    
    INFO_PRINTF1(_L("Performing out of heap memory test with 1 exercise thread..."));
    params.iEndpointThreadStress.iNumThreads = 1;
    TVerdict ret = RunRemoteTestCase(1, params);
    if(ret == EPass)
        {
        INFO_PRINTF1(_L("...done."));
        }
    else
        {
        ERR_PRINTF1(_L("...Failed, skipping rest of test step."));
        return ret;
        }
    
    INFO_PRINTF1(_L("Performing out of heap memory test with 10 exercise threads..."));
        params.iEndpointThreadStress.iNumThreads = 10;
        ret = RunRemoteTestCase(1, params);
        if(ret == EPass)
            {
            INFO_PRINTF1(_L("...done."));
            }
        else
            {
            ERR_PRINTF1(_L("...Failed, skipping rest of test step."));
            return ret;
            }
        
    EndRemoteTestStep(TRemoteTestParams());
    return TestStepResult();
    }
