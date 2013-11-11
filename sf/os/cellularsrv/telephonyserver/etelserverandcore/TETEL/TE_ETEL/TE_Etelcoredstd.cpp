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
// TE_ETELCOREDTSD.CPP
// 
//

#include "TE_EtelBase.h"
#include "TE_Etelcoredstd.h"

CTestOpenPhoneFc::CTestOpenPhoneFc()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenPhoneFc"));
	}

enum TVerdict CTestOpenPhoneFc::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open Phone Fc"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phoneFc;
    ret = phoneFc.Open(server,DSTDFC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	phoneFc.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestOpenAllSubSessInFc::CTestOpenAllSubSessInFc()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenSubSessInFc"));
	}

enum TVerdict CTestOpenAllSubSessInFc::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open All Sub Sessions In Fc"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phoneFc;
    ret = phoneFc.Open(server,DSTDFC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	RLine lineFc;
    ret = lineFc.Open(phoneFc,DSTDFC_LINE_NAME);
	TESTCHECK(ret, KErrNone);
	RCall callFc;
	ret = callFc.OpenNewCall(lineFc);
    TESTCHECK(ret, KErrNone);
	RFax faxFc;
    ret = faxFc.Open(callFc);
    TESTCHECK(ret, KErrNone);

	faxFc.Close();
	callFc.Close();
	lineFc.Close();
	phoneFc.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestOpenAllSubSess::CTestOpenAllSubSess()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenAllSubSess"));
	}

enum TVerdict CTestOpenAllSubSess::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open All Sub Sessions"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
    ret = phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	RLine line;
    ret = line.Open(phone,DSTD_LINE_NAME);
	TESTCHECK(ret, KErrNone);
	RCall call;
	ret = call.OpenNewCall(line);
    TESTCHECK(ret, KErrNone);
	RFax fax;
    ret = fax.Open(call);
    TESTCHECK(ret, KErrNone);

	RPhone phoneFc;
    ret = phoneFc.Open(server,DSTDFC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	RLine lineFc;
    ret = lineFc.Open(phoneFc,DSTDFC_LINE_NAME);
	TESTCHECK(ret, KErrNone);
	RCall callFc;
	ret = callFc.OpenNewCall(lineFc);
    TESTCHECK(ret, KErrNone);
	RFax faxFc;
    ret = faxFc.Open(callFc);
    TESTCHECK(ret, KErrNone);

	RPhone phonePm;
    ret = phonePm.Open(server,DSTDPM_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	RLine linePm;
    ret = linePm.Open(phonePm,DSTDPM_LINE_NAME);
	TESTCHECK(ret, KErrNone);
	RCall callPm;
	ret = callPm.OpenNewCall(linePm);
    TESTCHECK(ret, KErrNone);
	RFax faxPm;
    ret = faxPm.Open(callPm);
    TESTCHECK(ret, KErrNone);

	fax.Close();
	call.Close();
	line.Close();
	phone.Close();

	faxFc.Close();
	callFc.Close();
	lineFc.Close();
	phoneFc.Close();

	faxPm.Close();
	callPm.Close();
	linePm.Close();
	phonePm.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestServerOpenClose::CTestServerOpenClose()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestServerOpenClose"));
	}

enum TVerdict CTestServerOpenClose::doTestStepL()
//
// Test Server ability to load and unload phone module
//
    {
	INFO_PRINTF1(_L("Test server Open Close"));

    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	// second name has ".tsy" appended
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME2);
    TESTCHECK(ret, KErrNone);
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME2);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
    }

CTestUnload::CTestUnload()
/* Each test step initialises it's own name */
	{
	SetTestStepName(_L("TestUnload"));
	}

enum TVerdict CTestUnload::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Unload"));

    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
    ret = phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	RLine line;
    ret = line.Open(phone,DSTD_LINE_NAME);
	TESTCHECK(ret, KErrNone);

	RCall call;
    ret = call.OpenNewCall(line);
	TESTCHECK(ret, KErrNone);

	RFax fax;
	ret=fax.Open(call);
	TESTCHECK(ret, KErrNone);

	fax.Close();
	call.Close();
	line.Close();
	phone.Close();

  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestOpenMoreSubSess::CTestOpenMoreSubSess()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenMoreSubSess"));
	}

enum TVerdict CTestOpenMoreSubSess::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Opens More Sub Session"));

    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone1;
    ret = phone1.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	// open existing phone
	RPhone phone2;
    ret = phone2.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	phone2.Close();

	RLine line1;
    ret = line1.Open(phone1,DSTD_LINE_NAME);
	TESTCHECK(ret, KErrNone);

	// Open existing line
	RLine line2;
    ret = line2.Open(phone1,DSTD_LINE_NAME);
	TESTCHECK(ret, KErrNone);
	line2.Close();

	RCall call1;
	TName name;
    ret = call1.OpenNewCall(line1,name);
	TESTCHECK(ret, KErrNone);
	RLine::TCallInfo callinfo;
	line1.GetCallInfo(0,callinfo);

	// open call by handle
	RCall call2;
    ret = call2.OpenExistingCall(line1,name);
	TESTCHECK(ret, KErrNone);
	call2.Close();

	TName badName;
	RCall call3;
	INFO_PRINTF1(_L("Check zero length callnames"));
	ret = call3.OpenExistingCall(line1,badName);
    TESTCHECK(ret, KErrBadName);
    call3.Close();

	call1.Close();
	line1.Close();
	phone1.Close();
	phone2.Close();
	line2.Close();

  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
    }

CTestNotify::CTestNotify()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestNotify"));
	}

enum TVerdict CTestNotify::doTestStepL()
	{
	INFO_PRINTF1(_L("Notify a request!"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
	ret = phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	TRequestStatus phoneStatus;
	RPhone::TModemDetection detection;

	phone.NotifyModemDetected(phoneStatus,detection);
	User::WaitForRequest(phoneStatus);

	phone.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestNotify2Client::CTestNotify2Client()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestNotify2Client"));
	}

enum TVerdict CTestNotify2Client::doTestStepL()
	{
	INFO_PRINTF1(_L("Notify 2 clients for the same request!"));
    RTelServer server1;
	TInt ret = server1.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server1.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

    RTelServer server2;
    ret = server2.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server2.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone1,phone2;
	ret = phone1.Open(server1,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	ret = phone2.Open(server2,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	TRequestStatus phone1Status;
	TRequestStatus phone2Status;
	RPhone::TModemDetection detection1;
	RPhone::TModemDetection detection2;

	phone1.NotifyModemDetected(phone1Status,detection1);
	phone2.NotifyModemDetected(phone2Status,detection2);
	User::WaitForRequest(phone1Status);
	User::WaitForRequest(phone2Status);

	phone1.Close();
	phone2.Close();

	ret=server1.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    server1.Close();

  	ret=server2.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server2);

	return TestStepResult();
	}
