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
#include "TE_Etelcoredstdnc.h"

CTestNcTermFaxSess::CTestNcTermFaxSess()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestNcTermFaxSess"));
	}

enum TVerdict CTestNcTermFaxSess::doTestStepL()
	{
	INFO_PRINTF1(_L("Test \"N.C\" Terminate Fax Session"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
    ret = call.OpenNewCall(server,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	ret=call.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrNone);

	RFax fax;
	fax.Open(call);
	TRequestStatus readFax;
	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;
	fax.Read(readFax,buf);

	ret=fax.TerminateFaxSession();
    TESTCHECK(ret, KErrNone);
	
	fax.Close();
	call.Close();
	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
    ServerClose(server);

	return TestStepResult();
	}

CTestTermFaxSess1::CTestTermFaxSess1()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestTermFaxSess1"));
	}

enum TVerdict CTestTermFaxSess1::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Terminate Fax Session Case 1"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
    ret = call.OpenNewCall(server,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);

	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	ret=call.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrNone);

	RFax fax;
	fax.Open(call);
	TRequestStatus readFax;
	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;
	fax.Read(readFax,buf);
	User::WaitForRequest(readFax);

	ret=fax.TerminateFaxSession();
    TESTCHECK(ret, KErrNone);
	fax.Close();
	call.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestTermFaxSess2::CTestTermFaxSess2()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestTermFaxSess2"));
	}

enum TVerdict CTestTermFaxSess2::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Terminate Fax Session Case 2"));

    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
    ret = call.OpenNewCall(server,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	RFax fax;
	ret=fax.Open(call);
	TESTCHECK(ret, KErrNone);

	TRequestStatus readFaxStatus;
	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;
	fax.Read(readFaxStatus,buf);
	ret=fax.TerminateFaxSession();

	fax.Close();
	call.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

    ServerClose(server);

	return TestStepResult();
    }

CTestNcCancelLineCall::CTestNcCancelLineCall()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestNcCancelLineCall"));
	}

enum TVerdict CTestNcCancelLineCall::doTestStepL()
	{
	INFO_PRINTF1(_L("Cancel a line request !"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
	ret = phone.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	RLine line;
    ret = line.Open(phone,DSTDNC_LINE_NAME);
	TESTCHECK(ret, KErrNone);
	TRequestStatus incomingCallStatus;
	TName name;
	line.NotifyIncomingCall(incomingCallStatus,name);	//Request 1
	User::WaitForRequest(incomingCallStatus);	//Completed Request 1
	line.NotifyIncomingCallCancel();

	RCall call;
    ret = call.OpenNewCall(line);
	TESTCHECK(ret, KErrNone);

	TRequestStatus hookChangeStatus;
	TRequestStatus bearerChangeStatus;
	RCall::THookStatus hookStatus;
	RCall::TStatus callStatus;

	call.NotifyHookChange(hookChangeStatus,hookStatus);	//Request 2
	User::WaitForRequest(hookChangeStatus);	//Completed Request 2
	call.NotifyHookChangeCancel();
	call.NotifyStatusChange(bearerChangeStatus,callStatus);	//Request 3
	User::WaitForRequest(bearerChangeStatus);	//Completed Request 3
	call.NotifyStatusChangeCancel();
//
	line.NotifyIncomingCall(incomingCallStatus,name);	//Request 4
	User::WaitForRequest(incomingCallStatus);	//Completed Request 4
	call.NotifyHookChange(hookChangeStatus,hookStatus);	//Request 5
	User::WaitForRequest(hookChangeStatus);	//Completed Request 5
	call.NotifyStatusChange(bearerChangeStatus,callStatus);	//Request 6
	User::WaitForRequest(bearerChangeStatus);	//Completed Request 6
	call.NotifyStatusChangeCancel();
	line.NotifyIncomingCallCancel();
	call.NotifyHookChangeCancel();
/*
	line.NotifyIncomingCall(incomingCallStatus);
	call.NotifyHookChange(hookChangeStatus,hookStatus);
	call.NotifyStatusChange(bearerChangeStatus,callStatus);
	line.NotifyIncomingCallCancel();
	call.NotifyStatusChangeCancel();
	call.NotifyHookChangeCancel();

	line.NotifyIncomingCallCancel();
	call.NotifyStatusChangeCancel();
	call.NotifyHookChangeCancel();
*/
	RFax fax;
	fax.Open(call);
	TRequestStatus readFax;
	TRequestStatus readPageFax;

	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;

	fax.Read(readFax ,buf);	//Request 7
	User::WaitForRequest(readFax);	//Completed request 7
	fax.WaitForEndOfPage(readPageFax);	//Request 8
	User::WaitForRequest(readPageFax);	//Completed request 8
	fax.TerminateFaxSession();

	fax.Close();
	call.Close();
	line.Close();
	phone.Close();
  	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestCancelNoReq::CTestCancelNoReq()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCancelNoReq"));
	}

enum TVerdict CTestCancelNoReq::doTestStepL()
	{
	INFO_PRINTF1(_L("Cancel request without any previous request"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
	ret = phone.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	phone.Close();
  	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestNcCancelPhone::CTestNcCancelPhone()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestNcCancelPhone"));
	}

enum TVerdict CTestNcCancelPhone::doTestStepL()
	{
	INFO_PRINTF1(_L("Cancel a phone request !"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
	ret = phone.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	TRequestStatus phoneDetectedStatus;
	RPhone::TModemDetection detection;
	phone.NotifyModemDetected(phoneDetectedStatus,detection);	//Request 1
	User::WaitForRequest(phoneDetectedStatus);	//Completed request 1
    phone.NotifyModemDetectedCancel();

	phone.Close();
  	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestLoadTwice::CTestLoadTwice()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLoadTwice"));
	}

enum TVerdict CTestLoadTwice::doTestStepL()
	{
	INFO_PRINTF1(_L("Load Twice!"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	INFO_PRINTF1(_L("Must unload twice !"));
 	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
  	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestNcCancelWrFax::CTestNcCancelWrFax()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestNcCancelWrFax"));
	}

enum TVerdict CTestNcCancelWrFax::doTestStepL()
	{
	INFO_PRINTF1(_L("Cancel a Fax request !"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
	ret = phone.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	RLine line;
    ret = line.Open(phone,DSTDNC_LINE_NAME);
	TESTCHECK(ret, KErrNone);

	RCall call;
    ret = call.OpenNewCall(line);
	TESTCHECK(ret, KErrNone);

	RFax fax;
	fax.Open(call);
	TRequestStatus writeFax;
	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;
	fax.Write(writeFax,buf);

	ret=fax.TerminateFaxSession();
    TESTCHECK(ret, KErrNone);

	fax.Close();
	call.Close();
	line.Close();
	phone.Close();
  	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestGetNcFns::CTestGetNcFns()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestGetNcFns"));
	}

enum TVerdict CTestGetNcFns::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Get \"Nc\" Caps"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
	ret = phone.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone::TCaps caps;
    ret=phone.GetCaps(caps);
    TESTCHECK(ret, KErrNone);

	TRequestStatus status;
	RPhone::TModemDetection detection;
	phone.NotifyModemDetected(status,detection);	//Request 1
	User::WaitForRequest(status);	//Completed request 1
	phone.NotifyModemDetectedCancel();

	phone.Close();
    ServerClose(server);

	return TestStepResult();
	}
