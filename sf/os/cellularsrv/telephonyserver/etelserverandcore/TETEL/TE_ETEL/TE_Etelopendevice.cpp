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
#include "TE_Etelopendevice.h"

CTestOpenPhoneFrServ::CTestOpenPhoneFrServ()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenPhoneFrServ"));
	}

enum TVerdict CTestOpenPhoneFrServ::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open Phone From Server"));

    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
    ret = phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	phone.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
    }

CTestOpenLineFrServ::CTestOpenLineFrServ()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenLineFrServ"));
	}

enum TVerdict CTestOpenLineFrServ::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open Line From Server"));

    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RLine line;
    ret = line.Open(server,DSTD_LINE_FULLNAME);
	TESTCHECK(ret, KErrNone);
	line.Close();

  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
    }

CTestOpenCallFrServ::CTestOpenCallFrServ()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenCallFrServ"));
	}

enum TVerdict CTestOpenCallFrServ::doTestStepL()
	{
	INFO_PRINTF1(_L("Open Call from Server"));
    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	ret = call.OpenNewCall(server,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	call.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestOpenCallFrPhone::CTestOpenCallFrPhone()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenCallFrPhone"));
	}

enum TVerdict CTestOpenCallFrPhone::doTestStepL()
	{
	INFO_PRINTF1(_L("Open Call from RPhone"));
    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
    ret = phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	ret = call.OpenNewCall(phone,DSTD_OPEN_CALL_FROM_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	call.Close();

	phone.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestOpenLineFrPhone::CTestOpenLineFrPhone()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenLineFrPhone"));
	}

enum TVerdict CTestOpenLineFrPhone::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open Line From Phone"));

    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone phone;
    ret = phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	RLine line;
    ret = line.Open(phone,DSTD_LINE_NAME);
	TESTCHECK(ret, KErrNone);

	line.Close();
	phone.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
    }

CTestOpenCallFrLine::CTestOpenCallFrLine()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenCallFrLine"));
	}

enum TVerdict CTestOpenCallFrLine::doTestStepL()
	{
	INFO_PRINTF1(_L("Open Call from RLine!"));
    RTelServer server;
	TInt ret = server.Connect();
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
	call.Close();

	line.Close();
	phone.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestOpenFaxFrCall::CTestOpenFaxFrCall()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestOpenFaxFrCall"));
	}

enum TVerdict CTestOpenFaxFrCall::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Open Fax From Call"));

    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	ret=call.OpenNewCall(server,DSTD_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);

	RFax fax;
	ret=fax.Open(call);
    TESTCHECK(ret, KErrNone);

	fax.Close();
	call.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}
