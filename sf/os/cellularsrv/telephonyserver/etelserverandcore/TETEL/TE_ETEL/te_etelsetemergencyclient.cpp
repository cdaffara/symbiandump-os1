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
// TE_ETELEMERGENCYCLIENT.CPP
// 
//

#include "TE_EtelBase.h"
#include "te_etelsetemergencyclient.h"

CTestSetEmergencyClientCapabilityPosCheck::CTestSetEmergencyClientCapabilityPosCheck()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetEmergencyClientCapabilityPosCheck"));
	}
	
enum TVerdict CTestSetEmergencyClientCapabilityPosCheck::doTestStepL()
//
// Test connecting  Positive check of Capability check of SetEmergencyClient API on the same session
//
	{
	INFO_PRINTF1(_L("Test connecting Capability check of SetEmergencyClient API on the same session"));
	RTelServer server1;
	TInt ret = server1.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
   	INFO_PRINTF2(_L("server1.LoadPhoneModule returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    RPhone phone;
	ret = phone.Open(server1,DSTD_PHONE_NAME);
	INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	TInt setemergency=phone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
  	TESTCHECK(setemergency, KErrNone);
   	INFO_PRINTF2(_L(" RPhone::SetEmergencyClient(LCS request) %d."),setemergency);
	phone.Close();
	server1.Close();
	return TestStepResult();
	}	
	
CTestSetEmergencyClientCapabilityNegCheck::CTestSetEmergencyClientCapabilityNegCheck()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetEmergencyClientCapabilityNegCheck"));
	}
	
enum TVerdict CTestSetEmergencyClientCapabilityNegCheck::doTestStepL()
//
// Test connecting  Negative check of Capability check of SetEmergencyClient API on the same session
//
	{
	INFO_PRINTF1(_L("Test connecting Capability check of SetEmergencyClient API on the same session"));
	RTelServer server1;
	TInt ret = server1.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
   	INFO_PRINTF2(_L("server1.LoadPhoneModule returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    RPhone phone;
	ret = phone.Open(server1,DSTD_PHONE_NAME);
	INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	TInt setemergency=phone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
  	TESTCHECK(setemergency,KErrPermissionDenied);
   	INFO_PRINTF2(_L(" RPhone::SetEmergencyClient(LCS request) %d."),setemergency);
	phone.Close();
	server1.Close();
	return TestStepResult();
	}	

CTestSetEmgClientLCSReqMulCallFrSameSes::CTestSetEmgClientLCSReqMulCallFrSameSes()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetEmgClientLCSReqMulCallFrSameSes"));
	}

enum TVerdict CTestSetEmgClientLCSReqMulCallFrSameSes::doTestStepL()
//
// Test connecting Multiple call of SetEmergencyClientLCSRequet API on the same session
//
	{
	INFO_PRINTF1(_L("Test connecting Multiple call of SetEmergencyClientLCSRequet API on the same session"));
	RTelServer server1;
	TInt ret = server1.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
   	INFO_PRINTF2(_L("server1.LoadPhoneModule returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    RPhone phone;
	ret = phone.Open(server1,DSTD_PHONE_NAME);
	INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	TInt setemergency=phone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
   	TESTCHECK(setemergency, KErrNone);
   	INFO_PRINTF2(_L("first call of RPhone::SetEmergencyClient(LCS request) in same session  %d."),setemergency);
	setemergency=phone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
	TESTCHECK(setemergency, KErrAlreadyExists);

	INFO_PRINTF2(_L("Second call of RPhone::SetEmergencyClient(LCS request) in same session  %d."),setemergency);
	phone.Close();
	server1.Close();
	
	return TestStepResult();
	}
//
CTestSetEmgClientLCSReqMulCallFrDiffSubSes::CTestSetEmgClientLCSReqMulCallFrDiffSubSes()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetEmgClientLCSReqMulCallFrDiffSubSes"));
	}

enum TVerdict CTestSetEmgClientLCSReqMulCallFrDiffSubSes::doTestStepL()
//
// Test connecting Multiple call of SetEmergencyClientLCSRequest API on the different sub session
//
	{
	INFO_PRINTF1(_L("Test connecting Multiple call of SetEmergencyClientLCSRequest API on the different sub session"));
	TInt setemergency;
	RTelServer server1;
	TInt ret = server1.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
   	INFO_PRINTF2(_L("server1.LoadPhoneModule(DSTD_MODULE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
	RPhone phone1;
	ret = phone1.Open(server1,DSTD_PHONE_NAME);
	INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    
    RTelServer server2;
	ret = server2.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server2.LoadPhoneModule(DSTD_MODULE_NAME);
   	INFO_PRINTF2(_L("server2.LoadPhoneModule(DSTD_MODULE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
	RPhone phone2;
	ret = phone2.Open(server2,DSTD_PHONE_NAME);
	INFO_PRINTF2(_L("phone2.Open(server2,DSTD_PHONE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    
    setemergency=phone1.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
    TESTCHECK(setemergency, KErrNone);
	INFO_PRINTF2(_L("first call of RPhone::SetEmergencyClient(LCS request) in different session  %d."),setemergency);

	setemergency=phone2.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
	TESTCHECK(setemergency, KErrNone);
 	INFO_PRINTF2(_L("Second call of RPhone::SetEmergencyClient(LCS request) in different session  %d."),setemergency);

	phone2.Close();
	server2.Close();
	phone1.Close();
	server1.Close();
	
	return TestStepResult();
	}
//
CTestSetEmgClientLCSReqMulCallFrDiffClt::CTestSetEmgClientLCSReqMulCallFrDiffClt()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetEmgClientLCSReqMulCallFrDiffClt"));
	}
	
TInt CTestSetEmgClientLCSReqMulCallFrDiffClt ::FirstThreadFunctiona(TAny*  aThreadData1)
	{
	CTestStep* testStep = static_cast<CTestStep*>(aThreadData1);
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;
	testStep->INFO_PRINTF1(_L("Test connecting under SetEmergencyClient FirstThread"));
	RTelServer server1;
	TInt ret = server1.Connect();
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
   	testStep->TEST1(ret==KErrNone,ETrue);
   	testStep->INFO_PRINTF2(_L("server1.LoadPhoneModule returned %d."),ret);
    RPhone phone;
	ret = phone.Open(server1,DSTD_PHONE_NAME);
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
   	ret=phone.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
   	testStep->TEST1(ret==KErrNone,ETrue);
   	testStep->INFO_PRINTF2(_L("call of RPhone::SetEmergencyClient in thread1 %d."),ret);
	phone.Close();
	server1.Close();
	delete cleanup;
	return ret;
	}

TInt CTestSetEmgClientLCSReqMulCallFrDiffClt ::SecondThreadFunctiona(TAny*  aThreadData2)
	{
	CTestStep* testStep = static_cast<CTestStep*>(aThreadData2);
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;
	
	testStep->INFO_PRINTF1(_L("Test connecting under SetEmergencyClient SecondThread"));
	RTelServer server2;
	TInt ret = server2.Connect();
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    ret=server2.LoadPhoneModule(DSTD_MODULE_NAME);
   	testStep->TEST1(ret==KErrNone,ETrue);
   	testStep->INFO_PRINTF2(_L("server1.LoadPhoneModule returned %d."),ret);
    RPhone phone2;
	ret = phone2.Open(server2,DSTD_PHONE_NAME);
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
   	ret=phone2.SetEmergencyClient(RPhone::EEmergencyLCSRequest);
   	testStep->TEST1(ret==KErrNone,ETrue);
   	testStep->INFO_PRINTF2(_L("call of RPhone::SetEmergencyClient in thread2 %d."),ret);

	phone2.Close();
	server2.Close();
	delete cleanup;
	return ret;
	}
		
enum TVerdict CTestSetEmgClientLCSReqMulCallFrDiffClt::doTestStepL()
//
//Test connecting Multiple call of SetEmergencyClientLCSRequest API on the different thread
//
	{
	RThread thread1;
	RThread thread2;
	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	Logger().ShareAuto();
	INFO_PRINTF1(_L("Test connecting Multiple call of SetEmergencyClientLCSRequest API on the different thread"));
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
	thread2.Resume();
	User::WaitForRequest(reqStatus1);
	TEST(reqStatus1==KErrNone);
	User::WaitForRequest(reqStatus2);
	TEST(reqStatus2==KErrNone);
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
//
CTestSetEmgClientVoiceReqMulCallFrSameSes::CTestSetEmgClientVoiceReqMulCallFrSameSes()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetEmgClientVoiceReqMulCallFrSameSes"));
	}

enum TVerdict CTestSetEmgClientVoiceReqMulCallFrSameSes::doTestStepL()
//
// Test connecting Multiple call of CTestSetEmergencyClientVoiceRequest API on the same session
//
	{
	INFO_PRINTF1(_L("Test connecting Multiple call of CTestSetEmergencyClientVoiceRequest API on the same session"));
	TInt setemergency;
	RTelServer server1;
	TInt ret = server1.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
   	INFO_PRINTF2(_L("server1.LoadPhoneModule returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    RPhone phone;
	ret = phone.Open(server1,DSTD_PHONE_NAME);
	INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    
	setemergency=phone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
	TESTCHECK(setemergency, KErrNone);
 	INFO_PRINTF2(_L("first call of RPhone::SetEmergencyClient(Voice Request) in same session %d."),setemergency);
	setemergency=phone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
	TESTCHECK(setemergency, KErrAlreadyExists);
 	INFO_PRINTF2(_L("Second call of RPhone::SetEmergencyClient(Voice Request) in same session %d."),setemergency);

	phone.Close();
	server1.Close();
	return TestStepResult();
	}
//
CTestSetEmgClientVoiceReqMulCallFrDiffSubSes::CTestSetEmgClientVoiceReqMulCallFrDiffSubSes()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetEmgClientVoiceReqMulCallFrDiffSubSes"));
	}

enum TVerdict CTestSetEmgClientVoiceReqMulCallFrDiffSubSes::doTestStepL()
//
// Test connecting Multiple call of SetEmergencyClientVoiceRequest API on the different sub session
//
	{
	INFO_PRINTF1(_L("Test connecting Multiple call of SetEmergencyClientVoiceRequest API on the different sub session"));
	TInt setemergency;
	RTelServer server1;
	TInt ret = server1.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
   	INFO_PRINTF2(_L("server1.LoadPhoneModule(DSTD_MODULE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
	RPhone phone1;
	ret = phone1.Open(server1,DSTD_PHONE_NAME);
	INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    
    RTelServer server2;
	ret = server2.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server2.LoadPhoneModule(DSTD_MODULE_NAME);
   	INFO_PRINTF2(_L("server2.LoadPhoneModule(DSTD_MODULE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
	RPhone phone2;
	ret = phone2.Open(server2,DSTD_PHONE_NAME);
	INFO_PRINTF2(_L("phone2.Open(server2,DSTD_PHONE_NAME); returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    
    setemergency=phone1.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
    TESTCHECK(setemergency, KErrNone);
	INFO_PRINTF2(_L("first call of RPhone::SetEmergencyClient(Voice Request)  in different session %d."),setemergency);

	setemergency=phone2.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
	TESTCHECK(setemergency, KErrNone);
 	INFO_PRINTF2(_L("Second call of RPhone::SetEmergencyClient(Voice Request)  in different session %d."),setemergency);

	phone2.Close();
	server2.Close();
	phone1.Close();
	server1.Close();
	return TestStepResult();
	}
//
CTestSetEmgClientVoiceReqMulCallFrDiffClt::CTestSetEmgClientVoiceReqMulCallFrDiffClt()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetEmgClientVoiceReqMulCallFrDiffClt"));
	}
	
TInt CTestSetEmgClientVoiceReqMulCallFrDiffClt ::FirstThreadFunctiona(TAny*  aThreadData1)
	{
	CTestStep* testStep = static_cast<CTestStep*>(aThreadData1);
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;
	testStep->INFO_PRINTF1(_L("Test connecting under SetEmergencyClient FirstThread"));
	RTelServer server1;
	TInt ret = server1.Connect();
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
   	testStep->TEST1(ret==KErrNone,ETrue);
   	testStep->INFO_PRINTF2(_L("server1.LoadPhoneModule returned %d."),ret);
    RPhone phone;
	ret = phone.Open(server1,DSTD_PHONE_NAME);
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
   	ret=phone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
   	testStep->TEST1(ret==KErrNone,ETrue);
  	testStep->INFO_PRINTF2(_L("call of RPhone::SetEmergencyClient(Voice Request)  in thread1 %d."),ret);
	
	phone.Close();
	server1.Close();
	delete cleanup;
	return ret;
	}

TInt CTestSetEmgClientVoiceReqMulCallFrDiffClt ::SecondThreadFunctiona(TAny*  aThreadData2)
	{
	CTestStep* testStep = static_cast<CTestStep*>(aThreadData2);
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;
	testStep->INFO_PRINTF1(_L("Test connecting under SetEmergencyClient SecondThread"));
	RTelServer server2;
	TInt ret = server2.Connect();
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    ret=server2.LoadPhoneModule(DSTD_MODULE_NAME);
    testStep->TEST1(ret==KErrNone,ETrue);
   	testStep->INFO_PRINTF2(_L("server1.LoadPhoneModule returned %d."),ret);
    RPhone phone2;
	ret = phone2.Open(server2,DSTD_PHONE_NAME);
	testStep->TEST1(ret==KErrNone,ETrue);
	testStep->INFO_PRINTF2(_L("phone.Open(server1,DSTD_PHONE_NAME); returned %d."),ret);
   	ret=phone2.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
   	testStep->TEST1(ret==KErrNone,ETrue);
   	testStep->INFO_PRINTF2(_L("call of RPhone::SetEmergencyClient(Voice Request)  in thread2 %d."),ret);

	phone2.Close();
	server2.Close();
	delete cleanup;
	return ret;
	}
		
enum TVerdict CTestSetEmgClientVoiceReqMulCallFrDiffClt::doTestStepL()
//
//Test connecting Multiple call of SetEmergencyClientLCSRequest API on the different thread
//
	{
	RThread thread1;
	RThread thread2;
	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	Logger().ShareAuto();
	INFO_PRINTF1(_L("Test connecting Multiple call of SetEmergencyClientLCSRequest API on the different thread"));
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
	thread2.Resume();
	User::WaitForRequest(reqStatus1);
	TEST(reqStatus1==KErrNone);
	User::WaitForRequest(reqStatus2);
	TEST(reqStatus2==KErrNone);
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
