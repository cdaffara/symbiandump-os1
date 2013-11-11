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
#include "TE_Etelmemcallowner.h"

CTestMemCallOwner::CTestMemCallOwner()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMemCallOwnPanic"));
	}

enum TVerdict CTestMemCallOwner::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Call Owner Client Panic Example B"));
    RTelServer server;
	TInt ret = server.Connect();
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall callA;
	ret = callA.OpenNewCall(server,DSTD_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);

	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	ret=callA.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrNone);
	
	callA.Close();
	ServerClose(server);

	return TestStepResult();
	}

CTestDataPortCase5::CTestDataPortCase5()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestDataPortCase5"));
	}

enum TVerdict CTestDataPortCase5::doTestStepL()
//
// Same call different sessions
//
	{
	INFO_PRINTF1(_L("Test Data Port Case 5"));
	INFO_PRINTF1(_L("Same call different sessions"));
    RTelServer serverA;
    
    TInt  ret = serverA.Connect();
	TESTCHECK(ret, KErrNone);
	
	ret = serverA.LoadPhoneModule(DSTDNC_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
	RCall callA;
	TName name;
	
	ret = callA.OpenNewCall(serverA,DSTDNC_CALL_FULLNAME,name);
	TESTCHECK(ret, KErrNone);
	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	
	ret = callA.Dial(dialString);
	TESTCHECK(ret, KErrNone);

	RCall::TCommPort commInfo;
	ret = callA.LoanDataPort(commInfo);
	TESTCHECK(ret, KErrNone);

	TName callname = DSTDNC_CALL_FULLNAME;
	callname.Append(name);
    RTelServer serverB;
    
    ret = serverB.Connect();
	TESTCHECK(ret, KErrNone);
	TRequestStatus statusB;
	RCall callB;
	
	ret = callB.OpenExistingCall(serverB,DSTDNC_CALL_FULLNAME);
	TESTCHECK(ret, KErrNone);
	callB.AcquireOwnership(statusB);
	
	callA.Close();
	ServerClose(serverA);	

	User::WaitForRequest(statusB); // B is the owner and SHOULD be able to do a Loan CommPOrt
	
	ret = callB.LoanDataPort(commInfo);
	TESTCHECK(ret, KErrNone);
	callB.Close();
	ServerClose(serverB);	

	return TestStepResult();
	}
