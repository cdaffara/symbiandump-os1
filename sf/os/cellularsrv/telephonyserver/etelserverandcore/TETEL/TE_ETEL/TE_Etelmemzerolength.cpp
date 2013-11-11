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
#include "TE_Etelmemzerolength.h"

CTestMemZeroLength::CTestMemZeroLength()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMemZeroLength"));
	}

enum TVerdict CTestMemZeroLength::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Zero Length Dials"));
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
	TName name;
    ret = call.OpenNewCall(line,name);
	TESTCHECK(ret, KErrNone);

	RCall::TTelNumberC number(_L(""));
	TPtrC8 callParams(_L8(""));
	ret=call.Dial(number);
    TESTCHECK(ret, KErrArgument);

	ret=call.Dial(callParams,number);
    TESTCHECK(ret, KErrArgument);

	TRequestStatus status;
	call.Dial(status,number);
	User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrArgument);

	call.Dial(status,callParams,number);
	User::WaitForRequest(status);
    TESTCHECK(status.Int(), KErrArgument);

	call.Close();
	line.Close();

	INFO_PRINTF1(_L("Passed illegal Cancel Test"));
	INFO_PRINTF1(_L("Test an illegal Cancel"));

// This shouldn't panic
	phone.NotifyModemDetectedCancel();

	phone.Close();
    ServerClose(server);

	INFO_PRINTF1(_L("Passed illegal Cancel Test"));

	return TestStepResult();
	}
