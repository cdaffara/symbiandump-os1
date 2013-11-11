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
#include "TE_Etelmulticompletion.h"

CTestMultiCompletion::CTestMultiCompletion()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMCompletion"));
	}

enum TVerdict CTestMultiCompletion::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Multiple Completion On Same Call"));

	RTelServer server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);


	RCall callA;
	ret = callA.OpenNewCall(server,DSTDPM_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	
	TName badName;
	RCall callC;
	INFO_PRINTF1(_L("Check zero length callnames"));
	ret = callC.OpenExistingCall(server,badName);
    TESTCHECK(ret, KErrBadName);

	RCall callB;
	ret = callB.OpenExistingCall(server,DSTDPM_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);

	RCall::THookStatus hookA;
	TRequestStatus statusA;
	callA.NotifyHookChange(statusA,hookA);
	RCall::THookStatus hookB;
	TRequestStatus statusB;
	callB.NotifyHookChange(statusB,hookB);

	User::WaitForRequest(statusA);
	User::WaitForRequest(statusB);

	callA.Close();
	callB.Close();
	callC.Close();

	server.UnloadPhoneModule(DSTD_MODULE_NAME);
	ServerClose(server);

	return TestStepResult();
	}

CTestMultiCompletionDif::CTestMultiCompletionDif()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMCompletionD"));
	}

enum TVerdict CTestMultiCompletionDif::doTestStepL()
	{
	INFO_PRINTF1(_L("Test multiple completions on different calls"));
	RTelServer server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	RLine line;
	line.Open(server,DSTDPM_LINE_FULLNAME);

	RCall callA;
	TName name;
	ret = callA.OpenNewCall(line,name);
	TESTCHECK(ret, KErrNone);
	RCall callB;
	ret = callB.OpenNewCall(line);
	TESTCHECK(ret, KErrNone);

	RCall::THookStatus hookA;
	TRequestStatus statusA;
	callA.NotifyHookChange(statusA,hookA);
	RCall::THookStatus hookB;
	TRequestStatus statusB;
	callB.NotifyHookChange(statusB,hookB);

	User::WaitForRequest(statusA);
	User::WaitForRequest(statusB);

	callA.Close();
	callB.Close();
	line.Close();
	//ServerClose(server);
	ServerClose(server);

	return TestStepResult();
	}

