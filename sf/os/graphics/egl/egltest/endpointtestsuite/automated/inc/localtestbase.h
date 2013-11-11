// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __LOCALTESTBASE_H__)
#define __LOCALTESTBASE_H__


#include <ecom/ecom.h>
#include <e32msgqueue.h>
#include <test/testexecuteserverbase.h>
//#include <test/ttmsgraphicsstep.h>
#include "egltest_commscommon.h"


// Entry point
GLDEF_C TInt E32Main();

/**
CEglEndpointTestServer	Server encapsulates the functionality required to execute Test Execute based test cases for the EGL endpoint extension.
*/
class CEglEndpointTestServer : public CTestServer
	{
public:
	static CEglEndpointTestServer* NewL();

	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	};


/**
Logger used to log result of every GRAPHICS-EGL-* test to 
file, even in event of panic. wrapped by CLocalTestStepBase.
*/
class CTestIdResultLogger : public CBase
    {
private:
    typedef TBuf<8> TTestId;
    
public:
    static CTestIdResultLogger* NewL(CTestExecuteLogger& aLogger);
    ~CTestIdResultLogger();
    
    void RegisterTestIdsL(const TDesC& aTestString);
    void SetCurrentTestIds(const TDesC& aTestString);
    void LogResult(TVerdict aVerdict);
    
private:
    CTestIdResultLogger(CTestExecuteLogger& aLogger);
    void ConstructL();
    static TInt PanicMonitorMain(TAny* aSelf);
    void PanicMonitorMainL();
    void LogResult(CTestExecuteLogger& aLogger, const TTestId& aTestId, const TDesC& aVerdict);

private:
    const TThreadId iOriginalThread;
    RThread iPanicMonitor;
    TRequestStatus* iCloseMonitor;
    CTestExecuteLogger& iLogger;
    RArray<TTestId> iRegisteredTestIds;
    RArray<TTestId> iCurrentTestIds;
    };


/**
Base class for Egl test steps where the testing is done in a render stage
*/
NONSHARABLE_CLASS(CLocalTestStepBase) : public CTestStep //CTTMSGraphicsStep
    {
public:
    // from CTestStep
    CLocalTestStepBase(const TTestUid aUid);
    IMPORT_C virtual ~CLocalTestStepBase();
    TVerdict doTestStepPreambleL(); //setup the queues
    TVerdict doTestStepPostambleL(); //close the queues

    //Functions to control remote test.
    //If we are not currently in a test step (ie between StartRemoteTestStepL() and
    //EndRemoteTestStepL(), this includes the case where StartRemoteTestStepL() fails),
    //the framework does not allow running test cases. It will instead return EFail
    //immediately to the caller.

    TVerdict StartRemoteTestStep(const TRemoteTestParams& aMessageIn);
    TVerdict RunRemoteTestCase(TInt aTestCase, const TRemoteTestParams& aMessageIn);
    TVerdict EndRemoteTestStep(const TRemoteTestParams& aMessageIn);

    virtual void DoPreambleL();
    virtual void DoPostambleL();
    
    void RegisterTestIdsL(const TDesC& aTestString);
    void SetCurrentTestIds(const TDesC& aTestString);
    void SetTestStepResult(TVerdict aVerdict);

public:
    RMsgQueue<TRemoteTestResult> iResultOutQueue;
    RMsgQueue<TRemoteTestParamsPacket> iParamsInQueue;

protected:
    const TTestUid iTestId;
    TInt iCurrentTestCase;
    TBool iIsInTestStep;
    TVerdict iTestIdVerdict;
    TBool iHasCurrentTestIds;
    CTestIdResultLogger* iResultLog;
    };


#endif	// __EGLENDPOINTTESTSERVER_H__
