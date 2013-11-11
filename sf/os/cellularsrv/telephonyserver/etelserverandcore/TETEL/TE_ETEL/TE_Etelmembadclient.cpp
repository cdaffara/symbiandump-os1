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
#include "TE_Etelmembadclient.h"


/**
Each test step initialises it's own name
*/
CTestMemBadClient7::CTestMemBadClient7()
	{
	SetTestStepName(_L("TestMemBadClient7"));
	}

enum TVerdict CTestMemBadClient7::doTestStepL()
	//
	// Close server without closing its subsession !
	//
	{
 	User::SetJustInTime(EFalse);
 	INFO_PRINTF1(_L("Test Bad Client Case 7"));
 
 	RThread t;
 	TInt res=t.Create(_L("LeaveSubsessionsOpen"),LeaveSubsessionsOpen,KDefaultStackSize,KDefaultHeapSize,KDefaultHeapSize,NULL);
 	TESTCHECK(res, KErrNone);
 
 	TRequestStatus stat;
 	t.Logon(stat);
 	t.Resume();
 	User::WaitForRequest(stat);

 	TESTCHECK(t.ExitType(), EExitKill);
 	//TESTCHECKSTR(t.ExitCategory(), _L("Etel Client Faul"));
 	//TESTCHECK(t.ExitReason(), 11);	// EEtelPanicHandleNotClosed is 11
 	//TESTCHECK(t.ExitType(), EExitPanic);
 	t.Close();

 	User::SetJustInTime(ETrue);
 	User::After(KETelThreadShutdownGuardPeriod);
 
 	return TestStepResult();
 	}


TInt CTestMemBadClient7::LeaveSubsessionsOpen(TAny * /* aArg */)
 	{
 	//
 	// Thread which deliberately attempts to do silly stuff and should panic
 	//
 	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
 
	RTelServer server;
	TInt ret = server.Connect();

 	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
 	RPhone phone;
 	phone.Open(server,DSTD_PHONE_NAME);
 	RLine line;
	ret = line.Open(phone,DSTD_LINE_NAME);
 	RCall call;
 	ret = call.OpenNewCall(line);
 	RFax fax;
 	ret = fax.Open(call);
	server.Close(); // this is where the thread should be killed

 	User::After(KETelThreadShutdownGuardPeriod);
 	delete cleanup;
 	return KErrNone; // (execution should never reach here...)
   	}



CTestMemBadClient7a::CTestMemBadClient7a()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMemBadClient7a"));
	}

enum TVerdict CTestMemBadClient7a::doTestStepL()
 	//
 	// Open subsessions few times !
 	//
 	{
 	User::SetJustInTime(EFalse);
 	INFO_PRINTF1(_L("Test Bad Client Case 7a"));
 	
 	RThread t;
 	TInt res=t.Create(_L("OpenSubsessionsFewTimes"),OpenSubsessionsFewTimes,KDefaultStackSize,KDefaultHeapSize,KDefaultHeapSize,NULL);
 	TESTCHECK(res, KErrNone);
 
 	TRequestStatus stat;
 	t.Logon(stat);
 	t.Resume();
 	User::WaitForRequest(stat);
 
 	TESTCHECK(t.ExitType(), EExitKill);
 	//TESTCHECKSTR(t.ExitCategory(), _L("Etel Client Faul"));
 	//TESTCHECK(t.ExitReason(), 11);	// EEtelPanicHandleNotClosed is 11
 	//TESTCHECK(t.ExitType(), EExitPanic);
 	t.Close();
 
 	User::SetJustInTime(ETrue);
 	User::After(KETelThreadShutdownGuardPeriod);
 
 	return TestStepResult();
 	}
 

TInt CTestMemBadClient7a::OpenSubsessionsFewTimes(TAny * /* aArg */)
 	{
 	//
 	// Thread which deliberately attempts to do silly stuff and should panic
 	//
 	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
 
    RTelServer server;
 	TInt ret = server.Connect();
    ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
 	RCall call;
 	ret = call.OpenNewCall(server,DSTD_CALL_FULLNAME);
 	RFax fax;
 	ret = fax.Open(call);
 
 	RPhone phone;
 	phone.Open(server,DSTD_PHONE_NAME);
 	RLine line;
    ret = line.Open(phone,DSTD_LINE_NAME);
 
 	RPhone phoneFc;
 	phoneFc.Open(server,DSTDFC_PHONE_NAME);
 	RLine lineFc;
    ret = lineFc.Open(phoneFc,DSTDFC_LINE_NAME);
 
 	RPhone phonePm;
 	phonePm.Open(server,DSTDPM_PHONE_NAME);
 	RLine linePm;
    ret = linePm.Open(phonePm,DSTDPM_LINE_NAME);
 
    server.Close(); // this is where the thread should be killed
 
 	User::After(KETelThreadShutdownGuardPeriod);
 	delete cleanup;
 	return KErrNone; // (execution should never reach here...)
 	}

CTestMemBadClient7b::CTestMemBadClient7b()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMemBadClient7b"));
	}


enum TVerdict CTestMemBadClient7b::doTestStepL()
//
// Open two phone/line from the same instances of RPhone/RLine
//
	{
	User::SetJustInTime(EFalse);	// switches off the WINS threadbreakpoints
	INFO_PRINTF1(_L("Main thread : Test Bad Client Case 7b"));

	RThread t;
	TInt res=t.Create(_L("DoubleOpen"),DoubleOpenThread,KDefaultStackSize,KDefaultHeapSize,KDefaultHeapSize,NULL);
	TESTCHECK(res, KErrNone);

	TRequestStatus stat;
	t.Logon(stat);
	t.Resume();
	User::WaitForRequest(stat);
	
	TExitCategoryName  exitCategoryName = t.ExitCategory();
	
	TESTCHECKSTR(exitCategoryName, _L("Etel Client Faul"));
	TESTCHECK(t.ExitReason(), 11);	// EEtelPanicHandleNotClosed is 11
	TESTCHECK(t.ExitType(), EExitPanic);
	t.Close();
	User::SetJustInTime(ETrue);

	User::After(KETelThreadShutdownGuardPeriod);

	return TestStepResult();
	}

TInt CTestMemBadClient7b::DoubleOpenThread(TAny * /* anArg */)
//
// Thread which attempts to read twice and should panic
//
	{
	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack

    RTelServer server;
	TInt ret = server.Connect();
   	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);

	RPhone phone;
	RLine line;

	phone.Open(server,DSTD_PHONE_NAME);

    ret = line.Open(phone,DSTD_LINE_NAME);

	phone.Open(server,DSTDFC_PHONE_NAME);

    ret = line.Open(phone,DSTDFC_LINE_NAME);

	server.Close();
	User::After(KETelThreadShutdownGuardPeriod);
	delete cleanup;
	return KErrNone;
	}
