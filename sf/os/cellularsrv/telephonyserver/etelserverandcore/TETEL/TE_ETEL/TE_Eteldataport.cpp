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
#include "TE_Eteldataport.h"

CTestDataPort1::CTestDataPort1()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestDataPort1"));
	}

enum TVerdict CTestDataPort1::doTestStepL()
//
// This call will leave unless it has the matching pair between
// LoanDataPort / RecoverDataPort - normal behaviour
//
	{
	INFO_PRINTF1(_L("Test Data Port Case 1"));
    RTelServer server;
    
    TInt  ret = server.Connect();
    TESTCHECK(ret, KErrNone);
    
    ret = server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall call;
	
	ret = call.OpenNewCall(server,DSTDNC_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);
	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	
	ret = call.Dial(dialString);
    TESTCHECK(ret, KErrNone);
	RCall::TCommPort commInfo;
	
	ret = call.LoanDataPort(commInfo);
    TESTCHECK(ret, KErrNone);
    
    ret = call.RecoverDataPort();
    TESTCHECK(ret, KErrNone);
    
    ret = call.HangUp();
    TESTCHECK(ret, KErrNone);
	call.Close();
	ServerClose(server);

	return TestStepResult();
	}

CTestDataPort2::CTestDataPort2()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestDataPort2"));
	}

enum TVerdict CTestDataPort2::doTestStepL()
//
//  Bad behaviour - loan and close
//
	{
	INFO_PRINTF1(_L("Bad behaviour - loan and close"));
    RTelServer server;
    
    TInt  ret = server.Connect();
    TESTCHECK(ret, KErrNone);
    
    ret = server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall call;
	
	ret = call.OpenNewCall(server,DSTDNC_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);
	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	
	ret = call.Dial(dialString);
    TESTCHECK(ret, KErrNone);

	RCall::TCommPort commInfo;
	
	ret = call.LoanDataPort(commInfo);
    TESTCHECK(ret, KErrNone);
	call.Close();
	ServerClose(server);

	return TestStepResult();
	}

CTestDataPort4::CTestDataPort4()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestDataPort4"));
	}

enum TVerdict CTestDataPort4::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Data Port Case 4"));
    RTelServer server;
    
    TInt  ret = server.Connect();
    TESTCHECK(ret, KErrNone);
    
    ret = server.LoadPhoneModule(DSTDNC_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall callA;
	TName name;
	
	ret = callA.OpenNewCall(server,DSTDNC_CALL_FULLNAME,name);
    TESTCHECK(ret, KErrNone);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;

	ret = callA.Dial(dialString);
    TESTCHECK(ret, KErrNone);

	RCall::TCommPort commInfo;
	
	ret = callA.LoanDataPort(commInfo);
    TESTCHECK(ret, KErrNone);

	TRequestStatus statusB;
	RCall callB;
	
	ret = callB.OpenNewCall(server,DSTDNC_NEWCALL_FULLNAME);
    TESTCHECK(ret, KErrNone);
	callB.AcquireOwnership(statusB);
	callA.Close();

	User::WaitForRequest(statusB); // B is the owner and SHOULD be able to do a Loan CommPOrt

	ret = callB.LoanDataPort(commInfo);
    TESTCHECK(ret, KErrNone);

	callB.Close();
	ServerClose(server);

	return TestStepResult();
	}
