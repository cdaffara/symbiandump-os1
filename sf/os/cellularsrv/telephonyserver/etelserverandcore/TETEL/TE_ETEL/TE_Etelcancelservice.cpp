// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "TE_Etelcancelservice.h"

CTestCancelService1::CTestCancelService1()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCancelService1"));
	}

enum TVerdict CTestCancelService1::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Cancel Service Case 1"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
    ret = phone.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus phoneStatus;
	RPhone::TModemDetection detection;
	phone.NotifyModemDetected(phoneStatus,detection);	//Request 1
	User::WaitForRequest(phoneStatus);	//Completed Request 1
	phone.NotifyModemDetectedCancel();

	RLine line;
    ret = line.Open(phone,DSTDNC_LINE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus lineStatus;
	TName name;
	line.NotifyIncomingCall(lineStatus,name);	//Request 2
	User::WaitForRequest(lineStatus);	//Completed Request 2
	line.NotifyIncomingCallCancel();

	line.Close();
	phone.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestCancelService2::CTestCancelService2()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCancelService2"));
	}

enum TVerdict CTestCancelService2::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Cancel Service Case 2"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phoneA;
    ret = phoneA.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus phoneStatusA;
	RPhone::TModemDetection detection;

	phoneA.NotifyModemDetected(phoneStatusA,detection);	//Request 1
	User::WaitForRequest(phoneStatusA);	//Completed Request 1
	phoneA.NotifyModemDetectedCancel();

	RPhone phoneB;
    ret = phoneB.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus phoneStatusB;
	phoneB.NotifyModemDetected(phoneStatusB,detection);	//Request 2
	User::WaitForRequest(phoneStatusB);	//Completed Request 2
	phoneB.NotifyModemDetectedCancel();

	phoneA.Close();
	phoneB.Close();

    ServerClose(server);

	return TestStepResult();
	}

CTestCancelService3::CTestCancelService3()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCancelService3"));
	}

enum TVerdict CTestCancelService3::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Cancel Service Case 3"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
    ret = phone.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus phoneStatus;
	RPhone::TModemDetection detection;
	phone.NotifyModemDetected(phoneStatus,detection);	//Request 1
	User::WaitForRequest(phoneStatus);	//Completed Request 1

	RLine line;
    ret = line.Open(phone,DSTDNC_LINE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus lineStatus;
	TName name;
	line.NotifyIncomingCall(lineStatus,name);	//REquest 2
	User::WaitForRequest(lineStatus);	//Completed Request 2

	// ensure bothe asynch reqs are passed to server ...
	RPhone::TCaps caps;
	ret=phone.GetCaps(caps);
    TESTCHECK(ret, KErrNone);

	phone.NotifyModemDetectedCancel();
	line.NotifyIncomingCallCancel();

	line.Close();
	phone.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestCancelService4::CTestCancelService4()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCancelService4"));
	}

enum TVerdict CTestCancelService4::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Cancel Service Case 4"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phoneA;
    ret = phoneA.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus phoneStatusA;
	RPhone::TModemDetection detection;
	phoneA.NotifyModemDetected(phoneStatusA,detection);	//Request 1
	User::WaitForRequest(phoneStatusA);	//Completed Request 1
	phoneA.NotifyModemDetectedCancel();

	RPhone phoneB;
    ret = phoneB.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus phoneStatusB;
	phoneB.NotifyModemDetected(phoneStatusB,detection);	//Request 2
	User::WaitForRequest(phoneStatusB);	//Completed Request 2
	phoneB.NotifyModemDetectedCancel();

    phoneA.Close();
	phoneB.Close();

	ServerClose(server);

	return TestStepResult();
	}

CTestCancelService6::CTestCancelService6()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCancelService6"));
	}

enum TVerdict CTestCancelService6::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Cancel Service Case 6"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
	ret = phone.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	TRequestStatus hookChangeStatus;
	RPhone::TModemDetection detection;

    phone.NotifyModemDetected(hookChangeStatus,detection);	//Request 1
	User::WaitForRequest(hookChangeStatus);	//Completed Request 1
    phone.NotifyModemDetectedCancel();

	phone.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestCancelService7::CTestCancelService7()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCancelService7"));
	}

enum TVerdict CTestCancelService7::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Cancel Service Case 7"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	RPhone phone;
	ret = phone.Open(server,DSTDNC_PHONE_NAME);
	INFO_PRINTF2(_L("RPhone::Open returned %d."),ret);
	TESTCHECK(ret, KErrNone);

	RLine line;
    ret = line.Open(phone,DSTDNC_LINE_NAME);
	INFO_PRINTF2(_L("RLine::Open returned %d."),ret);
	TESTCHECK(ret, KErrNone);
	TRequestStatus incomingCallStatus;
	TName name;
	line.NotifyIncomingCall(incomingCallStatus,name);	//Request 1
	User::WaitForRequest(incomingCallStatus);	//Completed Request 1

	RCall call;
    ret = call.OpenNewCall(line);
	TESTCHECK(ret, KErrNone);

	TRequestStatus hookChangeStatus;
	TRequestStatus bearerChangeStatus;
	RCall::THookStatus hookStatus;
	RCall::TStatus callStatus;

	call.NotifyHookChange(hookChangeStatus,hookStatus); //Request 2
	User::WaitForRequest(hookChangeStatus);	//Completed Request 2
	call.NotifyStatusChange(bearerChangeStatus,callStatus);	//Request 3
	User::WaitForRequest(bearerChangeStatus);	//Completed Request 3

	RFax fax;
	fax.Open(call);
	TRequestStatus readFax;
	TRequestStatus writeFax;
	TRequestStatus readPageFax;

	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;

	fax.Read(readFax ,buf);	//Request 4
	User::WaitForRequest(readFax);	//Completed Request 4
	fax.Write(writeFax,buf);	//Request 5
	User::WaitForRequest(writeFax);	//Completed Request 5
	fax.WaitForEndOfPage(readPageFax);	//Request 6
	User::WaitForRequest(readPageFax);	//Completed Request 6

	ret=fax.TerminateFaxSession();
	TESTCHECK(ret, KErrNone);

	fax.Close();
	call.Close();
	line.Close();
	phone.Close();
    ServerClose(server);

	return TestStepResult();
	}
