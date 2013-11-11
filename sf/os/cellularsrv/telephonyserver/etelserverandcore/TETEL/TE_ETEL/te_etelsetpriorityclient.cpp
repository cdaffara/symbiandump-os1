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
//

#include "TE_EtelBase.h"
#include "te_etelsetpriorityclient.h"

CTestSetPrClientCapNsidPosCheck::CTestSetPrClientCapNsidPosCheck()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetPrClientCapNsidPosCheck"));
	}

enum TVerdict CTestSetPrClientCapNsidPosCheck::doTestStepL()
//
// Test connecting Positive check of Capability and SID check for SetPriorityClient API
//
	{
	INFO_PRINTF1(_L("Test connecting Capability and SID check for SetPriorityClient API"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF1(_L("Capability and SID check of setpriorityclient API"));
	ret=first.SetPriorityClient();
	TESTCHECK(ret,KErrNone);
	INFO_PRINTF2(_L("first call of RTelServer::SetPriorityClient %d."),ret);
	return TestStepResult();
	}
	
CTestSetPrClientCapNsidNegCheck::CTestSetPrClientCapNsidNegCheck()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetPrClientCapNsidNegCheck"));
	}

enum TVerdict CTestSetPrClientCapNsidNegCheck::doTestStepL()
//
// Test connecting Negative check of Capability and SID check for SetPriorityClient API
//
	{
	INFO_PRINTF1(_L("Test connecting Capability and SID check for SetPriorityClient API"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF1(_L("Capability and SID check of setpriorityclient API"));
	ret=first.SetPriorityClient();
	TESTCHECK(ret,KErrPermissionDenied);
	INFO_PRINTF2(_L("first call of RTelServer::SetPriorityClient %d."),ret);
	return TestStepResult();
	}	

CTestSetPrClientMulCallFrSameSes::CTestSetPrClientMulCallFrSameSes()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetPrClientMulCallFrSameSes"));
	}

enum TVerdict CTestSetPrClientMulCallFrSameSes::doTestStepL()
//
// Test connecting Multiple call of setpriorityclient API from the same session
//
	{
	INFO_PRINTF1(_L("Test connecting ultiple call of setpriorityclient API from the same session"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF1(_L("Multiple call of setpriorityclient API on the same session"));
	ret=first.SetPriorityClient();
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("first call of RTelServer::SetPriorityClient in same session  %d."),ret);
	ret=first.SetPriorityClient();
	TESTCHECK(ret, KErrAlreadyExists);
	INFO_PRINTF2(_L("Second call of RTelServer::SetPriorityClient in same session %d."),ret);
	first.Close();
	
	return TestStepResult();
	}
//
CTestSetPrClientMulCallFrDiffSubSes::CTestSetPrClientMulCallFrDiffSubSes()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetPrClientMulCallFrDiffSubSes"));
	}

enum TVerdict CTestSetPrClientMulCallFrDiffSubSes::doTestStepL()
//
// Test connecting Multiple call of setpriorityclient API from the different sub session
//
	{
	INFO_PRINTF1(_L("Test connecting Multiple call of setpriorityclient API on the different sub session"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	RTelServer second;
	ret=second.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect (second) returned %d."),ret);
	INFO_PRINTF1(_L("Multiple call of setpriorityclient API on the different session"));
	ret=first.SetPriorityClient();
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("first call of RTelServer::SetPriorityClient in different session %d."),ret);
	ret=second.SetPriorityClient();
	TESTCHECK(ret, KErrAlreadyExists);
	INFO_PRINTF2(_L("second call of RTelServer::SetPriorityClient in different session %d."),ret);
	second.Close();
	first.Close();
	
	return TestStepResult();
	}
//
CTestSetPrClientMulCallFrDiffClt::CTestSetPrClientMulCallFrDiffClt()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetPrClientMulCallFrDiffClt"));
	}
	
TInt CTestSetPrClientMulCallFrDiffClt ::FirstThreadFunctiona(TAny*  aThreadData1)
	{
	RTelServer server;
	CTestStep* testStep = static_cast<CTestStep*>(aThreadData1);
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		{
		return KErrGeneral;
		}
	TInt ret = server.Connect();
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("Thread1 RTelServer::Connect returned %d."),ret);
	ret=server.SetPriorityClient();
	testStep->TEST1((ret == KErrNone) || (ret == KErrAlreadyExists), ETrue);
	testStep->INFO_PRINTF2(_L("Thread1 RTelServer::SetPriorityClient returned %d."),ret);
	User::After(KETelThreadShutdownGuardPeriod);
	server.Close();	
	delete cleanup;
	return ret;
	}

TInt CTestSetPrClientMulCallFrDiffClt ::SecondThreadFunctiona(TAny*  aThreadData2)
	{
	RTelServer server;
	CTestStep* testStep = static_cast<CTestStep*>(aThreadData2);
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		{
		return KErrGeneral;	
		}
	TInt ret = server.Connect();
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("Thread2 RTelServer::Connect returned %d."),ret);
	ret=server.SetPriorityClient();
	testStep->TEST1((ret == KErrNone) || (ret == KErrAlreadyExists),ETrue);
	testStep->INFO_PRINTF2(_L("Thread2 RTelServer::SetPriorityClient returned %d."),ret);
	server.Close();
	delete cleanup;
	return ret;
	}
		
enum TVerdict CTestSetPrClientMulCallFrDiffClt::doTestStepL()
//
// Test connecting Multiple call of setpriorityclient API from the different threads
//
	{
	RThread thread1;
	RThread thread2;
	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	Logger().ShareAuto();
	INFO_PRINTF1(_L("Test connecting Multiple call of setpriorityclient API from the different threads"));
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;
	_LIT(KFirstThreadName,"FirstThread");
	_LIT(KSecondThreadName,"SecondThread");
	TInt res1=thread1.Create(KFirstThreadName, FirstThreadFunctiona,KStackSize,KHeapSize,KMaxHeapSize,(void*)this, EOwnerThread);
	TESTCHECK(res1, KErrNone);
	if(res1!=KErrNone)
		{
		ERR_PRINTF1(_L("CTestSetPrClientMulCallFrDiffClt:thread1.Create Failed to create a thread1"));
		SetTestStepResult(EFail);
		}
	thread1.Logon(reqStatus1);	//Request 1
	TInt resu1=(reqStatus1.Int(), KRequestPending);
	TInt res2=thread2.Create(KSecondThreadName,SecondThreadFunctiona,KStackSize,KHeapSize,KMaxHeapSize,(void*)this, EOwnerThread);
	TESTCHECK(res2, KErrNone);
	if(res2!=KErrNone)
		{
		ERR_PRINTF1(_L("CTestSetPrClientMulCallFrDiffClt:thread2.Create Failed to create a thread2"));
		SetTestStepResult(EFail);
		}
	thread2.Logon(reqStatus2);	//Request 2
	TInt resu2=(reqStatus2.Int(), KRequestPending);
	thread1.Resume();
	thread2.Resume();
	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);
	TEST( ((reqStatus1==KErrNone) && (reqStatus2==KErrAlreadyExists)) ||
		  ((reqStatus2==KErrNone) && (reqStatus1==KErrAlreadyExists)));
	if (thread1.ExitType() != EExitKill && thread1.ExitReason() != 0)
		{
		User::Leave(thread1.ExitReason());
		}//end of if (iThread1.ExitType() != EExitKill && iThread1.ExitReason() != 0)
	thread1.Close();
	INFO_PRINTF2(_L("First thread opened ETel session and returned %d"), res1);
	if (thread2.ExitType() != EExitKill && thread2.ExitReason() != 0)
		{
		User::Leave(thread2.ExitReason());
		}//end of if (iThread2.ExitType() != EExitKill && iThread2.ExitReason() != 0)
	thread2.Close();
	INFO_PRINTF2(_L("Second thread opened ETel session and returned %d"), res2);

	return TestStepResult();
	}
	
CTestSetPrClientV2CapNsidPosCheck::CTestSetPrClientV2CapNsidPosCheck()
	{
	SetTestStepName(_L("TestSetPrClientV2CapNsidPosCheck"));
	}
	
enum TVerdict CTestSetPrClientV2CapNsidPosCheck::doTestStepL()
//
// Test connecting  Positive check of Capability and SID check for SetPriorityClientV2 API
//
	{
	INFO_PRINTF1(_L("Test connecting Capability and SID check for SetPriorityClientV2 API"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF1(_L("Capability and SID check of setpriorityclientV2 API"));
	ret=first.SetPriorityClientV2();
	TESTCHECK(ret,KErrNone);
	INFO_PRINTF2(_L("first call of RTelServer::SetPriorityClientV2 %d."),ret);
	return TestStepResult();
	}
	
CTestSetPrClientV2CapNsidNegCheck::CTestSetPrClientV2CapNsidNegCheck()
	{
	SetTestStepName(_L("TestSetPrClientV2CapNsidNegCheck"));
	}
	
enum TVerdict CTestSetPrClientV2CapNsidNegCheck::doTestStepL()
//
// Test connecting Negative check of Capability and SID check for SetPriorityClientV2 API
//
	{
	INFO_PRINTF1(_L("Test connecting Capability and SID check for SetPriorityClientV2 API"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF1(_L("Capability and SID check of setpriorityclientV2 API"));
	ret=first.SetPriorityClientV2();
	TESTCHECK(ret,KErrPermissionDenied);
	INFO_PRINTF2(_L("first call of RTelServer::SetPriorityClientV2 %d."),ret);
	return TestStepResult();
	}	
	
	
CTestSetPrClientV2MulCallFrSameSes::CTestSetPrClientV2MulCallFrSameSes()
	{
	SetTestStepName(_L("TestSetPrClientV2MulCallFrDiffSubSes"));
	}
	
enum TVerdict CTestSetPrClientV2MulCallFrSameSes::doTestStepL()
//
// Test connecting Multiple call of setpriorityclientV2 API from the same session
//	
	{
	INFO_PRINTF1(_L("Test connecting Multiple call of setpriorityclientV2 API from the same session"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF1(_L("Multiple call of setpriorityclientV2 API on the same session"));
	ret=first.SetPriorityClientV2();
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("first call of RTelServer::SetPriorityClientV2 in same session  %d."),ret);
	ret=first.SetPriorityClientV2();
	TESTCHECK(ret, KErrAlreadyExists);
	INFO_PRINTF2(_L("Second call of RTelServer::SetPriorityClientV2 in same session %d."),ret);
	first.Close();
	
	return TestStepResult();
	}
	
CTestSetPrClientV2MulCallFrDiffSubSes::CTestSetPrClientV2MulCallFrDiffSubSes()
	{
	SetTestStepName(_L("TestSetPrClientV2MulCallFrDiffSubSes"));
	}
	
enum TVerdict CTestSetPrClientV2MulCallFrDiffSubSes::doTestStepL()
//
// Test connecting Multiple call of setpriorityclientV2 API from the different sub session
//
	{
	INFO_PRINTF1(_L("Test connecting Multiple call of setpriorityclientV2 API from the different sub session"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	RTelServer second;
	ret=second.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect (second) returned %d."),ret);
	INFO_PRINTF1(_L("Multiple call of setpriorityclientV2 API on the different session"));
	ret=first.SetPriorityClientV2();
	TESTCHECK(ret, KErrNone);
	INFO_PRINTF2(_L("first call of RTelServer::SetPriorityClientV2 in different session %d."),ret);
	ret=second.SetPriorityClientV2();
	TESTCHECK(ret, KErrAlreadyExists);
	INFO_PRINTF2(_L("second call of RTelServer::SetPriorityClientV2 in different session %d."),ret);
	second.Close();
	first.Close();
	return TestStepResult();
	}
	
CTestSetPrClientV2MulCallFrDiffClt::CTestSetPrClientV2MulCallFrDiffClt()
	{
	SetTestStepName(_L("TestSetPrClientV2MulCallFrDiffClt"));
	}
	
TInt CTestSetPrClientV2MulCallFrDiffClt ::FirstThreadFunctiona(TAny*  aThreadData1)
	{
	RTelServer server;
	CTestStep* testStep = static_cast<CTestStep*>(aThreadData1);
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		{
		return KErrGeneral;	
		}
	TInt ret = server.Connect();
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("Thread1 RTelServer::Connect returned %d."),ret);
	ret=server.SetPriorityClientV2();
	testStep->TEST1(( (ret==KErrAlreadyExists) || (ret==KErrNone) ),ETrue);
	testStep->INFO_PRINTF2(_L("Thread1 RTelServer::SetPriorityClientV2 returned %d."),ret);
	User::After(KETelThreadShutdownGuardPeriod);
	server.Close();	
	delete cleanup;
	return ret;
	}
	
TInt CTestSetPrClientV2MulCallFrDiffClt ::SecondThreadFunctiona(TAny*  aThreadData2)
	{
	RTelServer server;
	CTestStep* testStep = static_cast<CTestStep*>(aThreadData2);
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		{
		return KErrGeneral;	
		}
	TInt ret = server.Connect();
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("Thread2 RTelServer::Connect returned %d."),ret);
	ret=server.SetPriorityClientV2();
	testStep->TEST1(( (ret==KErrAlreadyExists) || (ret==KErrNone) ),ETrue);
	testStep->INFO_PRINTF2(_L("Thread2 RTelServer::SetPriorityClientV2 returned %d."),ret);
	User::After(KETelThreadShutdownGuardPeriod);
	server.Close();
	delete cleanup;
	return ret;
	}
	
enum TVerdict CTestSetPrClientV2MulCallFrDiffClt::doTestStepL()
//
// Test connecting Multiple call of setpriorityclientV2 API from the different threads
//	
	{
	RThread thread1;
	RThread thread2;
	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	Logger().ShareAuto();
	INFO_PRINTF1(_L("Test connecting Multiple call of setpriorityclientV2 API from the different threads"));
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;
	_LIT(KFirstThreadName,"FirstThread");
	_LIT(KSecondThreadName,"SecondThread");
	TInt res1=thread1.Create(KFirstThreadName, FirstThreadFunctiona,KStackSize,KHeapSize,KMaxHeapSize,(void*)this, EOwnerThread);
	TESTCHECK(res1, KErrNone);
	if(res1!=KErrNone)
		{
		ERR_PRINTF1(_L("CTestSetPriorityClientc:thread1.Create Failed to create a thread1"));
		SetTestStepResult(EFail);
		}
	thread1.Logon(reqStatus1);	//Request 1
	TInt resu1=(reqStatus1.Int(), KRequestPending);
	TInt res2=thread2.Create(KSecondThreadName,SecondThreadFunctiona,KStackSize,KHeapSize,KMaxHeapSize,(void*)this, EOwnerThread);
	TESTCHECK(res2, KErrNone);
	if(res2!=KErrNone)
		{
		ERR_PRINTF1(_L("CTestSetPriorityClientc:thread2.Create Failed to create a thread2"));
		SetTestStepResult(EFail);
		}
	thread2.Logon(reqStatus2);	//Request 2
	TInt resu2=(reqStatus2.Int(), KRequestPending);
	thread1.Resume();
	User::After(1);
	thread2.Resume();
	User::WaitForRequest(reqStatus1);
	User::WaitForRequest(reqStatus2);
	TEST( ((reqStatus1==KErrNone) && (reqStatus2==KErrAlreadyExists)) ||
		  ((reqStatus2==KErrNone) && (reqStatus1==KErrAlreadyExists)));
	if (thread1.ExitType() != EExitKill && thread1.ExitReason() != 0)
		{
		User::Leave(thread1.ExitReason());
		}//end of if (iThread1.ExitType() != EExitKill && iThread1.ExitReason() != 0)
	thread1.Close();
	INFO_PRINTF2(_L("First thread opened ETel session and returned %d"), res1);
	if (thread2.ExitType() != EExitKill && thread2.ExitReason() != 0)
		{
		User::Leave(thread2.ExitReason());
		}//end of if (iThread2.ExitType() != EExitKill && iThread2.ExitReason() != 0)
	thread2.Close();
	INFO_PRINTF2(_L("Second thread opened ETel session and returned %d"), res2);
	return TestStepResult();
	}
