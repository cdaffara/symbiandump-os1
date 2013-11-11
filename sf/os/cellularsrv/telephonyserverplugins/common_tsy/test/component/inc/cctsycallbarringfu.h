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
// The TEFUnit header file which tests the CallBarring
// functional unit of the Common TSY.
// 
//

#ifndef CCTSYCALLBARRINGFU_H
#define CCTSYCALLBARRINGFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>
#include <mmretrieve.h>

#include "cctsycomponenttestbase.h"

class CCTsyCallBarringFU : public CCtsyComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestStart0001L();
	void TestStart0002L();
	void TestStart0003L();
	void TestStart0004L();
	void TestStart0005L();
	void TestSetSSPassword0001L();
	void TestSetSSPassword0002L();
	void TestSetSSPassword0003L();
	void TestSetSSPassword0004L();
	void TestSetSSPassword0005L();
	void TestGetCompMethodName0001L();
	void TestSetCallBarringStatus0001L();
	void TestSetCallBarringStatus0002L();
	void TestSetCallBarringStatus0003L();
	void TestSetCallBarringStatus0004L();
	void TestSetCallBarringStatus0005L();
	void TestSetCallBarringPassword0001L();
	void TestNotifyCallBarringStatusChange0001L();
	void TestNotifyCallBarringStatusChange0002L();
	void TestNotifyCallBarringStatusChange0004L();


private:
	
	void OpenPhoneCBListLC();
	
private:
	CRetrieveMobilePhoneCBList* iCBList;

	}; // class CCTsyCallBarringFU

typedef void (CCTsyCallBarringFU::*TestStepFunc)();

class MFuncHolderBase
	{
public:
	virtual void Call() = 0;
	};

template <typename T, void (T::*TFunctionPtr)(TRequestStatus& aStatus)>
/*class MFuncHolder0 : public MFuncHolderBase
	{
private:
	T& iT;

public:
	MFuncHolder0(T& aT) : iT(aT) {}

public:
	void Call()
		{
		(iT.*TFunctionPtr)();
		}
	};
*/
class CTestStepRunner : public CActive
	{
public:
	static CTestStepRunner* NewLC(T& aT);
	void RunTestAsyncL();
	
private:
//	static void DoRunTestAsyncL(CCtsyComponentTestBase* aTestSuite, TestStepFunc aTestStep);

//	CTestStepRunner(CCtsyComponentTestBase* aTestSuite, TestStepFunc aTestStep, TBool aStopSchedulerWhenFinish);
	CTestStepRunner(T& aT);
	virtual ~CTestStepRunner();
	
	void ConstructL();
	
	void RunL();
	void DoCancel();
	
	inline TRequestStatus* Status() {return &iStatus;}
	
	void Call(TRequestStatus& aStatus)
		{
		(iT.*TFunctionPtr)(aStatus);
		}
		
	TInt StatusCompleteThreadFunc(TAny* );
private:
	T& iT;
//	TestStepFunc iTestStep;
	TBool iStopSchedulerWhenFinish;
	TInt iStage;
	CActiveScheduler* iScheduler;
//	CCtsyComponentTestBase* iTestSuite;
	};

class CAsyncRequestStatusHolder : public CActive
	{
public:
	static CAsyncRequestStatusHolder* NewLC();
	inline TRequestStatus* Status(){ return &iStatus; }
private:
	CAsyncRequestStatusHolder();
	~CAsyncRequestStatusHolder();
	void ConstructL();
	
	virtual void DoCancel();
	virtual void RunL();
private:
	TBool iSchedulerAlreadyStopped;
	
	};

#endif // CCTSYCALLBARRINGFU_H

