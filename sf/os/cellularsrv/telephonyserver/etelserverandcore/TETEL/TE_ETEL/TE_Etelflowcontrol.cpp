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
#include "TE_Etelflowcontrol.h"

CTestFlowControl1::CTestFlowControl1()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestFlowControl1"));
	}

enum TVerdict CTestFlowControl1::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Flow Control Case 1"));
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

	TBuf8<DACQ_FAX_BUF_SIZE> buf=DACQ_FAX_BUF_DATA;
	fax.Write(faxWriteStatus,buf);
	fax.Read(faxReadStatus,buf);
	User::WaitForRequest(faxWriteStatus);
	User::WaitForRequest(faxReadStatus);

	fax.Close();
	call.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestFlowControl2::CTestFlowControl2()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestFlowControl2"));
	}

enum TVerdict CTestFlowControl2::doTestStepL()
//
// This test suspended flow control and setup TCall back function which expired later on
// In the mean time send another command phone.Caps - this command should not reach
// The Tsy until the Timer expired which resume flow control
//
	{
	INFO_PRINTF1(_L("Test Flow Control Case 2"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	call.OpenNewCall(server,DSTD_CALL_FULLNAME);

	RFax fax;
    ret = fax.Open(call);
    TESTCHECK(ret, KErrNone);

	// This suspend flow control and completed somtime later
	// during this time fax.Write is blocked by server
	TRequestStatus faxReadStatus;
	TRequestStatus faxWriteStatus;
	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;
	// Check tsy timer works first
	fax.Read(faxReadStatus,buf);
	User::WaitForRequest(faxReadStatus);
	fax.Write(faxWriteStatus,buf);
	User::WaitForRequest(faxWriteStatus);

	fax.Read(faxReadStatus,buf); // this set flow control suspend and complete later
	fax.Write(faxWriteStatus,buf); // mean while send this and will buffer by server
	TRequestStatus readPageNotifyStatus;
	fax.WaitForEndOfPage(readPageNotifyStatus);

	User::WaitForRequest(faxReadStatus); // completed and set flow control resume
	User::WaitForRequest(faxWriteStatus); // this trigger this to completed
	User::WaitForRequest(readPageNotifyStatus);

	fax.Close();
	call.Close();

	// Resume normal Tsy service
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestFlowControl3::CTestFlowControl3()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestFlowControl3"));
	}

enum TVerdict CTestFlowControl3::doTestStepL()
//
// This test Open a phone Tsy do an Up call FlowControlSuspend()
// while initialising new phone Tew Asynch requests arrived to the server
// These reqs should be buffered by server. Once phone initalised it will do
// another up call FlowControlResume() the server will look in the waiting queue for any
// outstanding req and resume normal service
//
	{
	INFO_PRINTF1(_L("Test Flow Control Case 3"));
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

	TRequestStatus faxReadStatus;
	TRequestStatus faxWriteStatus;

	TAcqFaxBuf readbuf=DACQ_FAX_BUF_DATA;
	TAcqFaxBuf writebuf=DACQ_FAX_BUF_DATA;

	// Set Open timer in Phone FC long enough so that these two reqs are buffered
	// eventually timer expired which call FlowControlResume(0 and completed these Reqs
	// in due course
	fax.Read(faxReadStatus,readbuf);
	fax.Write(faxWriteStatus,writebuf);

	User::WaitForRequest(faxWriteStatus);
	User::WaitForRequest(faxReadStatus);

	fax.Close();
	call.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestFlowControl4::CTestFlowControl4()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestFlowControl4"));
	}

enum TVerdict CTestFlowControl4::doTestStepL()
//
// Test phone in parallel mode
// All request will be pass to TSY without buffering
//
	{
	INFO_PRINTF1(_L("Test Flow Control Case 4"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	call.OpenNewCall(server,DSTDPM_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);

	RFax fax;
    ret = fax.Open(call);
    TESTCHECK(ret, KErrNone);

	TRequestStatus faxReadStatus;
	TRequestStatus faxWriteStatus;
	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;
	// check each req complete correctly
	fax.Read(faxReadStatus,buf);
	User::WaitForRequest(faxReadStatus);
	fax.Write(faxWriteStatus,buf);
	User::WaitForRequest(faxWriteStatus);

	// These requests will go straight to TSY module without buffering
	fax.Read(faxReadStatus,buf);
	fax.Write(faxWriteStatus,buf);
	User::WaitForRequest(faxReadStatus);
	User::WaitForRequest(faxWriteStatus);

	fax.Close();
	call.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestFlowControl5::CTestFlowControl5()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestFlowControl5"));
	}

enum TVerdict CTestFlowControl5::doTestStepL()
//
// A Message to Tsy to suspend flow control and resume on completion
// another req for notification
//
	{
	INFO_PRINTF1(_L("Test Flow Control Case 5"));
    RTelServer server;
	TInt ret = server.Connect();
 	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	call.OpenNewCall(server,DSTD_CALL_FULLNAME);

	RFax fax;
    ret = fax.Open(call);
    TESTCHECK(ret, KErrNone);

	// This suspend flow control and completed somtime later
	// during this time fax.Write is blocked by server
	TRequestStatus faxReadStatus;
	TAcqFaxBuf buf=DACQ_FAX_BUF_DATA;

	fax.Read(faxReadStatus,buf); // this set flow control suspend and complete later
	TRequestStatus readPageNotifyStatus;
	fax.WaitForEndOfPage(readPageNotifyStatus);

	User::WaitForRequest(faxReadStatus); // completed and set flow control resume
	User::WaitForRequest(readPageNotifyStatus);

	fax.Close();
	call.Close();

	// Resume normal Tsy service
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestFlowControl6::CTestFlowControl6()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestFlowControl6"));
	}

enum TVerdict CTestFlowControl6::doTestStepL()
//
// Check a request with request mode multiple comnpletion disabled And
// Not obeyed flow control : LinInfo on phoneFc does this
//
	{
	INFO_PRINTF1(_L("Test Flow Control Case 6"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
	phone.Open(server,DSTDFC_PHONE_NAME);
	TInt count;
	ret=phone.EnumerateLines(count);
    TESTCHECK(ret, KErrNone);
	TESTCHECK(count, DSTDFC_NUMBER_OF_LINES);
	RPhone::TLineInfo info;
	ret=phone.GetLineInfo(count-DSTDFC_NUMBER_OF_LINES,info);
    TESTCHECK(ret, KErrNone);
	phone.Close();
    ServerClose(server);

	return TestStepResult();
	}

CTestFlowControl7::CTestFlowControl7()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestFlowControl7"));
	}

enum TVerdict CTestFlowControl7::doTestStepL()
//
// This steps first sends an async phone initialise request to the ETel server. 
// While TSY is dealing with that request, a cancel phone initialization request is sent from the same client.
// In the mean time, another client sends a new phone initialization request. At this point,
// there are two active requests which must obey the flow control:
// the cancellation of the first phone initialisation request and the second phone initialisation request.
// It is expected that the flow control is correctly suspended and resumed for both requests in the active list.
//
	{
	INFO_PRINTF1(_L("Test Flow Control Case 7"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect (1) returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    ret = server.ShareAuto();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
    
    TInt phoneIndx = 0;
    RTelServer::TPhoneInfo phoneInfo;
    ret = server.GetPhoneInfo(phoneIndx,phoneInfo);   
    TESTCHECK(ret, KErrNone);
    
	RPhone phone;
	ret = phone.Open(server,phoneInfo.iName);
    TESTCHECK(ret, KErrNone);
    
    // Send an asynchronous phone initializtion request from the first client.
    TRequestStatus status;
    phone.Initialise(status);
   
    // Now open another connection the server.
    RTelServer server2;
    ret = server2.Connect();
    INFO_PRINTF2(_L("RTelServer::Connect (2) returned %d."),ret);
    TESTCHECK(ret, KErrNone);
    ret = server2.ShareAuto();
    TESTCHECK(ret, KErrNone);
    ret=server2.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
    
	RPhone phone2;
	ret = phone2.Open(server,phoneInfo.iName);
    TESTCHECK(ret, KErrNone);
    
    // Cancel the initalisation request sent by the first client. This cancellation will be the first active
    // request in ETel server which obeys the flow control.
    phone.InitialiseCancel();
    // Then the second client sends its own initialisation request which will be placed as the second active request.
    phone2.Initialise();    
    
	phone.Close();
	phone2.Close();
    ServerClose(server);
    ServerClose(server2);
    
	return TestStepResult();
	}

