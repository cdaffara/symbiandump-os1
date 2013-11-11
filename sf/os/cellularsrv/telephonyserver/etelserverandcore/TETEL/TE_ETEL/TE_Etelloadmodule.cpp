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
#include "TE_Etelloadmodule.h"

CTestLoadModule1::CTestLoadModule1()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLoadModule1"));
	}

enum TVerdict CTestLoadModule1::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Load Modules Case 1"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
  	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestLoadModule2::CTestLoadModule2()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLoadModule2"));
	}

enum TVerdict CTestLoadModule2::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Load Modules Case 2"));
    RTelServer server;
	// Unload in the wrong order
	INFO_PRINTF1(_L("Load two modules wrong order!"));
    TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

  	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}

CTestLoadModule3::CTestLoadModule3()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLoadModule3"));
	}

enum TVerdict CTestLoadModule3::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Load Modules Case 3"));
    RTelServer server;

    TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RPhone pDstd;
	RPhone pDstdFc;
	RPhone pDstdPm;
	RPhone pDstdNc;

	ret=pDstd.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	ret=pDstdFc.Open(server,DSTDFC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	// There is a timer on the above Open
	// Must do a synch req to ensure this time is expired
	RPhone::TCaps cap;
	ret=pDstdFc.GetCaps(cap);
    TESTCHECK(ret, KErrNone);
	ret=pDstdPm.Open(server,DSTDPM_PHONE_NAME);
    TESTCHECK(ret, KErrNone);
	ret=pDstdNc.Open(server,DSTDNC_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall callHarryDstd;
	RCall callHarryDstdFc;
	RCall callHarryDstdPm;
	RCall callHarryDstdNc;

    ret = callHarryDstd.OpenNewCall(pDstd,DSTD_OPEN_CALL_FROM_PHONE_NAME);
	TESTCHECK(ret, KErrNone);
    ret = callHarryDstdFc.OpenNewCall(pDstdFc,DSTDFC_OPEN_CALL_FROM_PHONE_NAME);
	TESTCHECK(ret, KErrNone);
    ret = callHarryDstdPm.OpenNewCall(pDstdPm,DSTDPM_OPEN_CALL_FROM_PHONE_NAME);
	TESTCHECK(ret, KErrNone);
    ret = callHarryDstdNc.OpenNewCall(pDstdNc,DSTDNC_OPEN_CALL_FROM_PHONE_NAME);
	TESTCHECK(ret, KErrNone);

	RCall::TStatus status;
	ret = callHarryDstd.GetStatus(status);
	TESTCHECK(ret, KErrNone);
	ret = callHarryDstdFc.GetStatus(status);
	TESTCHECK(ret, KErrNone);
	ret = callHarryDstdPm.GetStatus(status);
	TESTCHECK(ret, KErrNone);
	ret = callHarryDstdNc.GetStatus(status);
	TESTCHECK(ret, KErrNone);
	// close ...
	callHarryDstd.Close();
	callHarryDstdFc.Close();
	callHarryDstdPm.Close();
	callHarryDstdNc.Close();

	pDstd.Close();
	pDstdFc.Close();
	pDstdPm.Close();
	pDstdNc.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
  	ret=server.UnloadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}
