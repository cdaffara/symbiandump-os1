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
#include "TE_Etelrelinquish.h"

CTestRelinquish::CTestRelinquish()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestRelinquish"));
	}

enum TVerdict CTestRelinquish::doTestStepL()
	{
 	INFO_PRINTF1(_L("Test relinquish ownership"));
	RTelServer serverA;
	TInt ret = serverA.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=serverA.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall callA;
    ret = callA.OpenNewCall(serverA,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

    ret=callA.Dial(callParamsPckg,DACQ_PHONE_NUMBER_TO_DIAL);
	TESTCHECK(ret, KErrNone);
	callA.Close();
	serverA.UnloadPhoneModule(DSTD_MODULE_NAME);
	ServerClose(serverA);

	return TestStepResult();
	}

CTestRelinquishOwner::CTestRelinquishOwner()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestRelinquishOwner"));
	}

enum TVerdict CTestRelinquishOwner::doTestStepL()
	{
 	INFO_PRINTF1(_L("Test Call Recover Port And Relinquish Ownership"));
	RTelServer serverA;
	TInt ret = serverA.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=serverA.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall callA;
    ret = callA.OpenNewCall(serverA,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

    ret=callA.Dial(callParamsPckg,DACQ_PHONE_NUMBER_TO_DIAL);
	TESTCHECK(ret, KErrNone);

	RCall::TCommPort port;
	ret=callA.LoanDataPort(port);
    TESTCHECK(ret, KErrNone);
	callA.Close();

	ServerClose(serverA);

	return TestStepResult();
	}

CTestRelinquishCase1::CTestRelinquishCase1()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestRelinquishCase1"));
	}

enum TVerdict CTestRelinquishCase1::doTestStepL()
//
// This test is used to check the objects are destroy in the right order
// make break points at CCallBase,CLineBase,CPhoneBase and CPhoneFactoryBase
// and ensure that to stop at the break point in order described above
//
    {
 	INFO_PRINTF1(_L("Test Call Relinquish Case 1"));
	RTelServer serverA;
	TInt ret = serverA.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=serverA.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);;

	RCall callA;
    ret = callA.OpenNewCall(serverA,DSTD_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);;

	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

    ret=callA.Dial(callParamsPckg,DACQ_PHONE_NUMBER_TO_DIAL);
	TESTCHECK(ret, KErrNone);;


	callA.Close();
	ServerClose(serverA);

	return TestStepResult();
	}
