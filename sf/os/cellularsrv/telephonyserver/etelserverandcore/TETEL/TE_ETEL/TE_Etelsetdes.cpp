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
// TE_ETESETDES.CPP
// 
//

#include "TE_EtelBase.h"
#include "TE_Etelsetdes.h"

CTestSetDes::CTestSetDes()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestSetDes"));
	}

enum TVerdict CTestSetDes::doTestStepL()
	{
	INFO_PRINTF1(_L("Set By Des"));
    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RPhone phone;
	ret = phone.Open(server,DSTD_PHONE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall call;
	ret=call.OpenNewCall(phone,DSTD_OPEN_CALL_FROM_PHONE_NAME);
	TESTCHECK(ret, KErrNone);

	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	ret=call.Dial(callParamsPckg,dialString);
	TESTCHECK(ret, KErrNone);

	RFax fax;
	ret=fax.Open(call);

	TRequestStatus faxReadStatus;
	TRequestStatus faxWriteStatus;

	TAcqFaxBuf readbuf=DACQ_FAX_BUF_DATA;
	TAcqFaxBuf writebuf=DACQ_FAX_BUF_DATA;

	// check each req complete correctly
	fax.Read(faxReadStatus,readbuf);
	User::WaitForRequest(faxReadStatus);
	TESTCHECKSTR(readbuf, DACQ_FAX_BUF_DATA);

	fax.Write(faxWriteStatus,writebuf);
	User::WaitForRequest(faxWriteStatus);

	fax.Close();
	call.Close();
	phone.Close();
  	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
    ServerClose(server);

	return TestStepResult();
	}
