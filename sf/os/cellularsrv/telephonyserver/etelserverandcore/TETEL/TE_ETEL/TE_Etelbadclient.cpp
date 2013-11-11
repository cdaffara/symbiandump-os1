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
#include "TE_Etelbadclient.h"


CTestBadClient1::CTestBadClient1()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient1"));
	}

enum TVerdict CTestBadClient1::doTestStepL()
//
// Close more than once
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 1 : Close more than once"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	RCall call;
	call.OpenNewCall(server,DSTDFC_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	call.Close();
	call.Close();
	call.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestBadClient1a::CTestBadClient1a()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient1a"));
	}

enum TVerdict CTestBadClient1a::doTestStepL()
//
// Close more than once
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 1a : Close more than once"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	RCall call;
	call.OpenNewCall(server,DSTDFC_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	RFax fax;
	ret = fax.Open(call);
    TESTCHECK(ret, KErrNone);

	fax.Close();
	fax.Close();
	fax.Close();
	fax.Close();
	call.Close();
	call.Close();
	call.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestBadClient1b::CTestBadClient1b()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient1b"));
	}

enum TVerdict CTestBadClient1b::doTestStepL()
//
// Bad client case 1 :
// Open a Call on a phone that has a timer for initialisation so
// it set FlowControlSuspend() but a client decided to close the server !
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 1b"));
	INFO_PRINTF1(_L("Open a Call on a phone that has a timer for initialisation so"));
	INFO_PRINTF1(_L("it set FlowControlSuspend() but a client decided to close the server !"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	call.OpenNewCall(server,DSTD_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);

	// This set Open timer going but client decided to close
	call.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);


	return TestStepResult();
	}

CTestBadClient2::CTestBadClient2()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient2"));
	}

enum TVerdict CTestBadClient2::doTestStepL()
//
// close server without unload tsy
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 2 : "));
	INFO_PRINTF1(_L("Close server without unload tsy"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
    ServerClose(server);


	return TestStepResult();
	}

CTestBadClient2a::CTestBadClient2a()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient2a"));
	}

enum TVerdict CTestBadClient2a::doTestStepL()
//
// Close Server without Unloading Two TSY's
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 2a"));
	INFO_PRINTF1(_L("Close Server without Unloading Two TSY's"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestBadClient3::CTestBadClient3()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient3"));
	}

enum TVerdict CTestBadClient3::doTestStepL()
//
// Same as case 1 :
// Open a Call on this phone that has a timer for initialisation so
// it set FlowControlSuspend() a client then send a asynch. req and close server !
// before the req completed nor the initialisation completed
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 3 : "));
	INFO_PRINTF1(_L("Same as case 1 :"));
	INFO_PRINTF1(_L("Open a Call on this phone that has a timer"));
	INFO_PRINTF1(_L("for initialisation so it set FlowControlSuspend()"));
	INFO_PRINTF1(_L("a client then send a asynch. req and close server !"));
	INFO_PRINTF1(_L("before the req completed nor the initialisation completed"));
	
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	call.OpenNewCall(server,DSTDFC_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);

	RCall::TCommPort port;
	ret=call.LoanDataPort(port);
    TESTCHECK(ret, KErrNone);

	// This set Open timer going but client decided to close
	call.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestBadClient3a::CTestBadClient3a()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient3a"));
	}

enum TVerdict CTestBadClient3a::doTestStepL()
//
// 2 Request are sent - 1st send flow control suspend and set timer to expired later
// second req coming this is buffered in 'wait' list .
// client decided to close every thing - server should clean up
// both 'wait' list and 'active' list
//
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 3a"));
	INFO_PRINTF1(_L("2 Request are sent - 1st send flow control suspend"));
	INFO_PRINTF1(_L("and set timer to expired later"));
	INFO_PRINTF1(_L("second req coming this is buffered in 'wait' list"));
	INFO_PRINTF1(_L("client decided to close every thing"));
	INFO_PRINTF1(_L("server should clean up"));
	INFO_PRINTF1(_L("both 'wait' list and 'active' list"));

    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	call.OpenNewCall(server,DSTDFC_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);

	// This ensure the Open timer has expired
	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	ret=call.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrNone);

	RFax fax;
    ret = fax.Open(call);
    TESTCHECK(ret, KErrNone);
	// Write timer is much much longer than Read Timer
	TRequestStatus faxReadStatus;
	TRequestStatus faxWriteStatus;

	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;
	fax.Write(faxWriteStatus,buf);	//Request 1
	User::WaitForRequest(faxWriteStatus);	//Completed request 1
	fax.Read(faxReadStatus,buf);	//Request 2
	User::WaitForRequest(faxReadStatus);	//Completed request 2
	// decided to close everything !

	ret=fax.TerminateFaxSession();
    TESTCHECK(ret, KErrNone);

	fax.Close();
	call.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestBadClient4::CTestBadClient4()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient4"));
	}

enum TVerdict CTestBadClient4::doTestStepL()
//
// Open twice
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 4 : Open twice"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
	ret=phone.Open(server,DSTD_PHONE_NAME);
	TESTCHECK(ret, KErrNotFound);
	phone.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestBadClient5::CTestBadClient5()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient5"));
	}

enum TVerdict CTestBadClient5::doTestStepL()
//
// Instances of SubSession should be valid even
// client unload the TSY module
//
	{
	INFO_PRINTF1(_L("Test Bad Client Case 5"));
	INFO_PRINTF1(_L("Instances of SubSession should be valid even"));
	INFO_PRINTF1(_L("client unload the TSY module"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
	phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	// instances of phone should still be valid
	TInt count;
	ret=phone.EnumerateLines(count);
    TESTCHECK(ret, KErrNone);
    TESTCHECK(count, DSTD_NUMBER_OF_LINES);
	RPhone::TLineInfo info;
	ret=phone.GetLineInfo(count-DSTD_NUMBER_OF_LINES,info);
    TESTCHECK(ret, KErrNone);

	phone.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestBadClient6::CTestBadClient6()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient6"));
	}

enum TVerdict CTestBadClient6::doTestStepL()
//
// Close server before open
//
	{
	INFO_PRINTF1(_L("TestBadClientCase6 : Close server before open"));
    RTelServer server;
    ServerClose(server);
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
	phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	phone.Close();
    ServerClose(server);
    ServerClose(server);

	return TestStepResult();
	}

CTestBadClient8::CTestBadClient8()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestBadClient8"));
	}

TInt DoublePhoneOpenThread(TAny * /* anArg */)
//
// Thread which attempts to open a phone twice and should panic
//
	{
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack

    RTelServer server;
	TInt ret = server.Connect();
//    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
//    TESTCHECK(ret, KErrNone);
	RPhone phone;
	phone.Open(server,DSTD_PHONE_NAME);
//    TESTCHECK(ret, KErrNone);
	phone.Open(server,DSTD_PHONE_NAME);	//this line should cause a client panic
//    TESTCHECK(ret, KErrNone);
	phone.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
//	TESTCHECK(ret, KErrNone);
	server.Close();
	User::After(KETelThreadShutdownGuardPeriod);
//	ServerClose(server);
	delete cleanup;
//	test(FALSE);	// Shouldn't reach here
	return KErrNone;
	}

enum TVerdict CTestBadClient8::doTestStepL()
//
// Open phone twice with the same name !
//
	{
	User::SetJustInTime(EFalse);	// switches off the WINS threadbreakpoints
	INFO_PRINTF1(_L("Main thread : TestBadClientCase8"));
	INFO_PRINTF1(_L("Main thread : Open phone twice with the same name !"));
	INFO_PRINTF1(_L("Main thread : Multithreaded test"));
	
	
	RThread t;
	TInt res=t.Create(_L("DoublePhoneOpen"),DoublePhoneOpenThread,KDefaultStackSize,KDefaultHeapSize,KDefaultHeapSize,NULL);
	TESTCHECK(res, KErrNone);

	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);

	TExitCategoryName  exitCategoryName = t.ExitCategory();

	TESTCHECKSTR(exitCategoryName, _L("Etel Client Faul"));
	TESTCHECK(t.ExitReason(), 11);
	TESTCHECK(t.ExitType(), EExitPanic);
	INFO_PRINTF1(_L("If test above was OK, second thread has exited"));
	INFO_PRINTF1(_L("Because it was panicked"));
	t.Close();
	User::SetJustInTime(ETrue);

	return TestStepResult();
	}

