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
#include "TE_Etelcallowner.h"

CTestCallOwner1::CTestCallOwner1()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCallOwner1"));
	}

enum TVerdict CTestCallOwner1::doTestStepL()
//
// Simple Example
//
	{
	INFO_PRINTF1(_L("Test Call Owner Case 1 : A simple example"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall callA;
	TName name;
	ret = callA.OpenNewCall(server,DSTD_CALL_FULLNAME,name);
    TESTCHECK(ret, KErrNone);
	RTelServer server2;
	
	ret = server2.Connect();
	TESTCHECK(ret, KErrNone);
	
	ret = server2.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	TName callname = DSTD_CALL_FULLNAME;
	callname.Append(name);
	RCall callB;
	ret = callB.OpenExistingCall(server2,callname);
    TESTCHECK(ret, KErrNone);

	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	ret=callA.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrNone);

	// Test error case: non owner client attempts to modify the call status
	ret=callB.HangUp();
    TESTCHECK(ret, KErrEtelNotCallOwner);

 	// Test error case: non owner client attempts to modify the call status
 	ret=callB.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrEtelNotCallOwner);

	ret=callA.HangUp();
    TESTCHECK(ret, KErrNone);

	ret=callB.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrNone);

	callA.Close();
	callB.Close();
	server2.UnloadPhoneModule(DSTD_MODULE_NAME);
	server.UnloadPhoneModule(DSTD_MODULE_NAME);
	ServerClose(server2);
	ServerClose(server);

	return TestStepResult();
	}

CTestCallOwner2::CTestCallOwner2()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCallOwner2"));
	}

enum TVerdict CTestCallOwner2::doTestStepL()
//
// Simple Example
//
	{
	INFO_PRINTF1(_L("Test Call Owner Case 2 : Simple example"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall callA;
	TName name;

	ret = callA.OpenNewCall(server,DSTD_CALL_FULLNAME,name);
    TESTCHECK(ret, KErrNone);
	RTelServer server2;
	
	ret = server2.Connect();
    TESTCHECK(ret, KErrNone);
    
    ret = server2.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	TName callname = DSTD_CALL_FULLNAME;
	callname.Append(name);
	RCall callB;
	ret = callB.OpenExistingCall(server2,callname);
    TESTCHECK(ret, KErrNone);

	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	ret=callA.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrNone);

	TRequestStatus statusB;
	callB.AcquireOwnership(statusB);

 	// The TSY does not allow more than one client to attempt to acquire
 	// ownership of a call. KErrInUse is returned.
 	RTelServer server3;
 	
 	ret = server3.Connect();
    TESTCHECK(ret, KErrNone);
    
    ret = server3.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
 	RCall callC;
 	ret = callC.OpenExistingCall(server3,callname);
    TESTCHECK(ret, KErrNone);
 	TRequestStatus statusC;
 	callC.AcquireOwnership(statusC);
 	User::WaitForRequest(statusC);
 	// KErrInUse is returned because client 2 has already requested ownership
    TESTCHECK(statusC.Int(), KErrInUse);
 
	ret=callA.TransferOwnership();
    TESTCHECK(ret, KErrNone);

   	User::WaitForRequest(statusB);

	ret=callB.HangUp();
    TESTCHECK(ret, KErrNone);

	callA.Close();
	callB.Close();
 	callC.Close();
	ServerClose(server);
	ServerClose(server2);
 	ServerClose(server3);

	return TestStepResult();
	}

CTestCallOwnerCliPanicA::CTestCallOwnerCliPanicA()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestCallOwnerClPanic"));
	}

enum TVerdict CTestCallOwnerCliPanicA::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Call Owner Client Panic Example A"));
    RTelServer server;
	TInt ret = server.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);
	RCall callA;
	ret = callA.OpenNewCall(server,DSTD_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);

	RCall callB;
	ret = callB.OpenNewCall(server,DSTD_CALL_FULLNAME);
    TESTCHECK(ret, KErrNone);

	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);

	TDummyString dialString=DACQ_PHONE_NUMBER_TO_DIAL;
	ret=callA.Dial(callParamsPckg,dialString);
    TESTCHECK(ret, KErrNone);

	TRequestStatus statusB;
	callB.AcquireOwnership(statusB);

	callA.Close(); // Send Relingquish() to Tsy

   	User::WaitForRequest(statusB);

	ret=callB.HangUp();
    TESTCHECK(ret, KErrNone);

	callB.Close();
	ServerClose(server);

	return TestStepResult();
	}
