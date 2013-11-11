// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// StateMachineImpl.cpp
// This is the header file for StateMachine test implementation
// 
//

#ifndef STATEMACHINESTEP_H_
#define STATEMACHINESTEP_H_

#include "ElementServer.h"
#include "StateMachineImpl.h"

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <testexecutestepbase.h>
#else
#include <test/testexecutestepbase.h>
#endif

/*
 * @file StateMachineStep.h
 */

_LIT(KElementStateMachineStep1_1,"ElementStateMachineStep0101");
_LIT(KElementStateMachineStep1_2,"ElementStateMachineStep0102");
_LIT(KElementStateMachineStep1_3,"ElementStateMachineStep0103");
_LIT(KElementStateMachineStep1_4,"ElementStateMachineStep0104");
_LIT(KElementStateMachineStep1_5,"ElementStateMachineStep0105");
_LIT(KElementStateMachineStep2_1,"ElementStateMachineStep0201");
_LIT(KElementStateMachineStep2_2,"ElementStateMachineStep0202");
_LIT(KElementStateMachineStep2_3,"ElementStateMachineStep0203");
_LIT(KElementStateMachineStep2_4,"ElementStateMachineStep0204");
_LIT(KElementStateMachineStep2_5,"ElementStateMachineStep0205");
_LIT(KElementStateMachineStep2_6,"ElementStateMachineStep0206");
_LIT(KElementStateMachineStep2_7,"ElementStateMachineStep0207");
_LIT(KElementStateMachineStep2_8,"ElementStateMachineStep0208");
_LIT(KElementStateMachineHistory,"History");
_LIT(KElementStateMachineLastError,"LastError");
_LIT(KElementStateMachNullRAllcLngth, "NullReAllocLength");
_LIT(KElementStateMachSetRAllocLngth, "SetReAllocLength");

class CElementStateMachineTestBase
    {
protected:
    void Setup();
    void Destroy();
protected:
    TVerdict iResult;
    CActiveScheduler* iActiveScheduler;
    CStateMachineImpl* iStateMachine;
    };

class CElementStateMachineStep1_1 : public CTestStep, public CElementStateMachineTestBase
    {
public:
    CElementStateMachineStep1_1();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
private:
    TInt iHistory;
    };

class CElementStateMachineStep1_2 : public CTestStep, public CElementStateMachineTestBase
    {
public:
    CElementStateMachineStep1_2();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
private:
    TInt iLastError;
    };

class CElementStateMachineStep1_3 : public CTestStep, public CElementStateMachineTestBase
    {
public:
    CElementStateMachineStep1_3();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    };

class CElementStateMachineStep1_4 : public CTestStep, public CElementStateMachineTestBase
    {
public:
    CElementStateMachineStep1_4();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
private:
    TInt iNullReAllocLength;
    TInt iSetReAllocLength;
    };

class CElementStateMachineStep1_5 : public CTestStep, public CElementStateMachineTestBase
    {
public:
    CElementStateMachineStep1_5();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
private:
    TInt iNullReAllocLength;
    TInt iSetReAllocLength;
    };

class CElementStateMachineStep2_1 : public CTestStep, public CElementStateMachineTestBase, public MStateMachineStepLogger
    {
public:
    CElementStateMachineStep2_1();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    void LogStateChange(TAsyncEventTest aAsyncEventTest);
private:
    TAsyncEventTest iExpectedAsyncEvent;
    };

class CElementStateMachineStep2_2 : public CTestStep, public CElementStateMachineTestBase, public MStateMachineStepLogger
    {
public:
    CElementStateMachineStep2_2();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    void LogStateChange(TAsyncEventTest aAsyncEventTest);
private:
    TAsyncEventTest iExpectedAsyncEvent;
    };

class CElementStateMachineStep2_3 : public CTestStep, public CElementStateMachineTestBase, public MStateMachineStepLogger
    {
public:
    CElementStateMachineStep2_3();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    void LogStateChange(TAsyncEventTest aAsyncEventTest);
private:
    TAsyncEventTest iExpectedAsyncEvent;
    };

class CElementStateMachineStep2_4 : public CTestStep, public CElementStateMachineTestBase, public MStateMachineStepLogger
    {
public:
    CElementStateMachineStep2_4();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    void LogStateChange(TAsyncEventTest aAsyncEventTest);
private:
    TAsyncEventTest iExpectedAsyncEvent;
    };

class CElementStateMachineStep2_5 : public CTestStep, public CElementStateMachineTestBase, public MStateMachineStepLogger
    {
public:
    CElementStateMachineStep2_5();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    void LogStateChange(TAsyncEventTest aAsyncEventTest);
private:
    TAsyncEventTest iExpectedAsyncEvent;
    };

class CElementStateMachineStep2_6 : public CTestStep, public CElementStateMachineTestBase, public MStateMachineStepLogger
    {
public:
    CElementStateMachineStep2_6();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    void LogStateChange(TAsyncEventTest aAsyncEventTest);
private:
    TAsyncEventTest iExpectedAsyncEvent;
    };

class CElementStateMachineStep2_7 : public CTestStep, public CElementStateMachineTestBase, public MStateMachineStepLogger
    {
public:
    CElementStateMachineStep2_7();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    void LogStateChange(TAsyncEventTest aAsyncEventTest);
private:
    TAsyncEventTest iExpectedAsyncEvent;
    };

class CElementStateMachineStep2_8 : public CTestStep, public CElementStateMachineTestBase, public MStateMachineStepLogger
    {
public:
    CElementStateMachineStep2_8();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();
    virtual TVerdict doTestStepL();
    void LogStateChange(TAsyncEventTest aAsyncEventTest);
private:
    TAsyncEventTest iExpectedAsyncEvent;
    };

#endif /* STATEMACHINESTEP_H_ */
