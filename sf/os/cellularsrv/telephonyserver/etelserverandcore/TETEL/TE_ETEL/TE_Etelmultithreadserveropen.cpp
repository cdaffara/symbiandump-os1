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
#include "TE_Etelmultithreadserveropen.h"

CTestMultiThreadServerOpen::CTestMultiThreadServerOpen()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMThreadServOpen"));
	}

TInt FirstThreadFunction(TAny* /* aThreadData*/)
	{
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;

	// Phone has already been initialised - so just open handles for this thread
	RTelServer server;

	TInt ret = server.Connect();
//    TESTCHECK(ret, KErrNone);

	server.Close();
	delete cleanup;
	return ret;
	}

TInt SecondThreadFunction(TAny* /* aThreadData*/)
	{
	CTrapCleanup* cleanup;
	if ((cleanup=CTrapCleanup::New())==NULL)
		return KErrGeneral;

	// Phone has already been initialised - so just open handles for this thread
	RTelServer server;

	TInt ret = server.Connect();
//    TESTCHECK(ret, KErrNone);

	server.Close();
	delete cleanup;
	return ret;
	}

enum TVerdict CTestMultiThreadServerOpen::doTestStepL()
	{
	INFO_PRINTF1(_L("Two threads, both connect to ETel server simultaneously"));

	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;

	_LIT(KFirstThreadName,"FirstThread");
	_LIT(KSecondThreadName,"SecondThread");

	RThread t1;
	RThread t2;

	TInt res1=t1.Create(KFirstThreadName, FirstThreadFunction,
					    KStackSize,KHeapSize,KMaxHeapSize,NULL);

	TInt res2=t2.Create(KSecondThreadName,SecondThreadFunction,
					    KStackSize,KHeapSize,KMaxHeapSize,NULL);

	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	TBool req1Complete=EFalse;
	TBool req2Complete=EFalse;
	
	t1.Resume();
	t2.Resume();
	t1.Logon(reqStatus1);	//Request 1
	t2.Logon(reqStatus2);	//Request 2
	
	while ( req1Complete==EFalse || req2Complete==EFalse)
		{
		User::WaitForAnyRequest();
		if (reqStatus1 != KRequestPending && req1Complete==EFalse)
			{
			res1=reqStatus1.Int();
			INFO_PRINTF2(_L("First thread opened ETel session and returned %d"), res1);
			req1Complete=ETrue;
			}
		else if (reqStatus2 != KRequestPending && req2Complete==EFalse)
			{
			res2=reqStatus2.Int();
			INFO_PRINTF2(_L("Second thread opened ETel session and returned %d"), res2);
			req2Complete=ETrue;
			}
		}

	t1.Close();
	t2.Close();

	// Delay 1/2 to allow ETEL server to shutdown

	User::After(500000);

	return TestStepResult();
	}

