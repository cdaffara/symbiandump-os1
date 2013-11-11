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
#include "TE_Etelreqmode.h"

CTestReqMode::CTestReqMode()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestReqMode"));
	}

enum TVerdict CTestReqMode::doTestStepL()
	{
 	INFO_PRINTF1(_L("Check request mode"));
	RTelServer serverA;
	TInt ret = serverA.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=serverA.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	RCall callA;
    ret = callA.OpenNewCall(serverA,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	// should call ReqMode in Tsy
	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

    ret=callA.Dial(callParamsPckg,DACQ_PHONE_NUMBER_TO_DIAL);
	TESTCHECK(ret, KErrNone);

	RCall callB;
    ret = callB.OpenNewCall(serverA,DSTDFC_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	// should call ReqMode in Server
    ret=callB.Dial(callParamsPckg,DACQ_PHONE_NUMBER_TO_DIAL);
	TESTCHECK(ret, KErrNone);

	callA.Close();
	callB.Close();
	serverA.UnloadPhoneModule(DSTD_MODULE_NAME);
	ServerClose(serverA);

	return TestStepResult();
	}
