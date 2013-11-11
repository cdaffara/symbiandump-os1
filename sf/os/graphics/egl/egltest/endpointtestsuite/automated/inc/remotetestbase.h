// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __REMOTETESTBASE_H__
#define __REMOTETESTBASE_H__


#include <e32base.h>
#include <e32msgqueue.h>
#include <test/tefexportconst.h>
#include "eglendpointwrap.h"
#include "egltest_commscommon.h"


class CRemoteTestStepBase;
class CRemoteTestEnv;


//Active object used to generate a timeout if worker thread takes too long.
class CTimeoutTimer : public CTimer
    {
public:
    static CTimeoutTimer* NewL(CRemoteTestEnv& aEnv, TInt aPriority);
    ~CTimeoutTimer();

private:
    CTimeoutTimer(CRemoteTestEnv& aEnv, TInt aPriority);
    void ConstructL();
    void RunL();

private:
    CRemoteTestEnv& iEnv;
    };


//Active object used to listen to the worker thread to see if it panics.
class CWorkerListener : public CActive
    {
public:
    static CWorkerListener* NewL(CRemoteTestEnv& aEnv, TInt aPriority);
    ~CWorkerListener();
    void Listen(RThread& aThread);

private:
    CWorkerListener(CRemoteTestEnv& aEnv, TInt aPriority);
    void ConstructL();
    void RunL();
    void DoCancel();

private:
    CRemoteTestEnv& iEnv;
    RThread* iThread;
    };


//Active object used to listen for test case completion from the worker thread.
class CTestCaseListener : public CActive
    {
public:
    static CTestCaseListener* NewL(CRemoteTestEnv& aEnv, TInt aPriority);
    ~CTestCaseListener();
    void Listen();

private:
    CTestCaseListener(CRemoteTestEnv& aEnv, TInt aPriority);
    void ConstructL();
    void RunL();
    void DoCancel();

private:
    CRemoteTestEnv& iEnv;
    };


//This class provides the remote test environment. CreateRemoteTestStepL()
//Should be edited to return an instance of a derived CRemoteTestStepBase
//class that corresponds with the passed in aTestUid.
class CRemoteTestEnv : public CActive
    {
public:
    static CRemoteTestEnv* NewL();
    virtual ~CRemoteTestEnv();

    void StartReceivingCmds();
    CRemoteTestStepBase* CreateRemoteTestStepL(TTestUid aTestUid);

    void SendResult(TRemoteTestVerdict aVerdict);
    void SendLog(const TDesC8& aFile, TInt aLine, TInt aSeverity, const TDesC& aMessage);

    void TestCaseCompleted();
    void TestCaseTimedOut();
    void WorkerExitted();

protected:
    CRemoteTestEnv();
    void ConstructL();

private:
    void RunL();
    void DoCancel();

    void ReceiveCmd();
    TBool SetupTestStep();

    //These functions run in the context of the worker thread.
    void RunCurrentTestStepL();
    static TInt TestThreadEntryPoint(TAny* aSelf);

    void DoEglHeapMark();
    void DoEglHeapCheck();

private:
    RMsgQueue<TRemoteTestResult> iResultOutQueue;
    RMsgQueue<TRemoteTestParamsPacket> iParamsInQueue;

    CRemoteTestStepBase* iCurTestStep;
    TRemoteTestParamsPacket iCurTestCaseParamsPacket;
    TRemoteTestVerdict iCurTestCaseVerdict;
    RThread iCurWorker;

    TThreadId iSupervisorId;
    TRequestStatus iNotifyRunTestCase;

    CTimeoutTimer* iTimeoutTimer;
    CWorkerListener* iWorkerListener;
    CTestCaseListener* iTestCaseListener;
    };


//This is the base class for all remote test steps. Derived classes should implement
//DoRemoteTestStepL(), and return the result of the test as a TVerdict.
class CRemoteTestStepBase : public CBase, public MLog
    {
public:
    virtual ~CRemoteTestStepBase();

    virtual TRemoteTestVerdict DoStartRemoteTestStepL(const TRemoteTestParams& aMessageIn);
    virtual TRemoteTestVerdict DoRunRemoteTestCaseL(TInt aTestCase, const TRemoteTestParams& aMessageIn) = 0;
    virtual TRemoteTestVerdict DoEndRemoteTestStepL(const TRemoteTestParams& aMessageIn);

    virtual TInt Timeout() const;
    
    void Log(const TText8* aFile, TInt aLine, TInt aSeverity, TRefByValue<const TDesC> aFmt, ...);
    const TEglEndpointWrap& EglEndpoint() const;

    void EglStartL();
    void EglEndL();

protected:
    CRemoteTestStepBase(TTestUid aUid);

private:
    friend class CRemoteTestEnv;
    //Function called by CRemoteTestEnv.
    //It should NOT be called by a derived class during construction.
    void ConstructL(CRemoteTestEnv& aTestEnv);

private:
    const TTestUid iUid;
    TInt iCurrentTestCase;
    CRemoteTestEnv* iTestEnv;
    TEglEndpointWrap iEndpoint;
    };


// Logger Macros - based on TEF but for use with CRemoteTestStepBase.
//The severity enumeration is from TEF.
#define REMOTE_INFO_PRINTF1(p1)                            Log(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1))
#define REMOTE_INFO_PRINTF2(p1, p2)                        Log(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2))
#define REMOTE_INFO_PRINTF3(p1, p2, p3)                    Log(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3))
#define REMOTE_INFO_PRINTF4(p1, p2, p3, p4)                Log(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4))
#define REMOTE_INFO_PRINTF5(p1, p2, p3, p4, p5)            Log(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5))
#define REMOTE_INFO_PRINTF6(p1, p2, p3, p4, p5, p6)        Log(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6))
#define REMOTE_INFO_PRINTF7(p1, p2, p3, p4, p5, p6, p7)    Log(((TText8*)__FILE__), __LINE__, ESevrInfo, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define REMOTE_WARN_PRINTF1(p1)                            Log(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1))
#define REMOTE_WARN_PRINTF2(p1, p2)                        Log(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2))
#define REMOTE_WARN_PRINTF3(p1, p2, p3)                    Log(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3))
#define REMOTE_WARN_PRINTF4(p1, p2, p3, p4)                Log(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4))
#define REMOTE_WARN_PRINTF5(p1, p2, p3, p4, p5)            Log(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5))
#define REMOTE_WARN_PRINTF6(p1, p2, p3, p4, p5, p6)        Log(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6))
#define REMOTE_WARN_PRINTF7(p1, p2, p3, p4, p5, p6, p7)    Log(((TText8*)__FILE__), __LINE__, ESevrWarn, (p1), (p2), (p3), (p4), (p5), (p6), (p7))

#define REMOTE_ERR_PRINTF1(p1)                             Log(((TText8*)__FILE__), __LINE__, ESevrErr, (p1))
#define REMOTE_ERR_PRINTF2(p1, p2)                         Log(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2))
#define REMOTE_ERR_PRINTF3(p1, p2, p3)                     Log(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3))
#define REMOTE_ERR_PRINTF4(p1, p2, p3, p4)                 Log(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4))
#define REMOTE_ERR_PRINTF5(p1, p2, p3, p4, p5)             Log(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5))
#define REMOTE_ERR_PRINTF6(p1, p2, p3, p4, p5, p6)         Log(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6))
#define REMOTE_ERR_PRINTF7(p1, p2, p3, p4, p5, p6, p7)     Log(((TText8*)__FILE__), __LINE__, ESevrErr, (p1), (p2), (p3), (p4), (p5), (p6), (p7))


#endif
