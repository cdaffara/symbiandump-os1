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
#include "TE_Etellowmem.h"

CTestLowMem::CTestLowMem()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLowMem"));
	}

enum TVerdict CTestLowMem::doTestStepL()
//
// Test connecting under OOM
//
	{
	INFO_PRINTF1(_L("Test connecting under OOM"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
	TESTCHECK(ret, KErrNone);

	INFO_PRINTF1(_L("Low memory connection"));
	TInt failure=0;
	RTelServer second;
	ret=-1;
	first.__DbgMarkHeap();
	while (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failing after %d Allocs"),failure);
		first.__DbgFailNext(failure);
		ret=second.Connect();
		INFO_PRINTF2(_L("RTelServer::Connect (second) returned %d."),ret);
		if (ret!=KErrNone)
			first.__DbgCheckHeap(0);
		failure++;
		}
	INFO_PRINTF1(_L("Connected OK"));

	second.Close();

	// Flush any FailNext there might be hanging around.
	ret = second.Connect();
	if (ret == KErrNone)
		{
		second.Close();
		}

	first.__DbgMarkEnd(0);
	first.Close();

	return TestStepResult();
	}

CTestLowMemPhoneHCheck::CTestLowMemPhoneHCheck()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLowMemPhHCheck"));
	}

enum TVerdict CTestLowMemPhoneHCheck::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Low Memory Phone Creation With Heap Check"));
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=first.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	INFO_PRINTF1(_L("Low memory phone open with heap check"));
	TInt failure=0;
	RPhone phones[16];

	TInt i;
	for(i=0;i<16;i++)
		{
		phones[i].Open(first,DSTD_PHONE_NAME);
		}
	for(i=0;i<15;i++)
		phones[i].Close();
	failure=0;
	RPhone phone;
	ret=-1;
	first.__DbgMarkHeap();
	while (ret!=KErrNone)
		{
		INFO_PRINTF2(_L("Failing after %d Allocs"),failure);
		first.__DbgFailNext(failure);
		ret=phone.Open(first,DSTD_PHONE_NAME);
		if (ret!=KErrNone)
			first.__DbgCheckHeap(0);
		failure++;
		}
	INFO_PRINTF1(_L("Created Phone OK"));

	phone.Close();
	phones[15].Close();

	// Flush any FailNext there might be hanging around.
	if (phone.Open(first,DSTD_PHONE_NAME)==KErrNone)
		phone.Close();

	first.__DbgMarkEnd(0);
	first.Close();
	User::After(300000);

	return TestStepResult();
	}

CTestLowMemFnFail::CTestLowMemFnFail()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestLowMemFnFail"));
	}

enum TVerdict CTestLowMemFnFail::doTestStepL()
	{
	INFO_PRINTF1(_L("Test Low Memory Function Failure"));
	
// OOM testing only works with debug builds
#ifdef _DEBUG	

	const TInt KMaxTests = 1000;	
	
	RTelServer first;
	TInt ret=first.Connect();
	INFO_PRINTF2(_L("RTelServer::Connect returned %d."),ret);
    TESTCHECK(ret, KErrNone);
   	ret=first.LoadPhoneModule(DSTD_MODULE_NAME);
    TESTCHECK(ret, KErrNone);

	INFO_PRINTF1(_L("Low memory function failure with heap check"));
	TInt failure=0;
	RPhone phone;
	ret=phone.Open(first,DSTD_PHONE_NAME);
	TESTCHECK(ret, KErrNone);

	ret=KErrNotFound; //Initially set to anything other than KErrNone
	RPhone::TStatus status;

	TBool oomAvailable = EFalse;
	
	// Floggerv2 based logging in Etel is designed to not produce an error 
	// when it encounters a problem and hence the simulated heap failure 
	// is considered to have been "eaten" as the function call will pass 
	// unexpectedly. To overcome this we loop through the test until the
	// function call passes and we still have the simulated heap allocation 
	// failure available to us because it has not been consumed by either a
	// genuine heap allocation in Etel or a call to the logger. 
	first.__DbgMarkHeap();
	while (!oomAvailable  && failure < KMaxTests)
		{
		first.__DbgFailNext(failure);
		ret=phone.GetStatus(status);
		if (ret!=KErrNone)
			{
			INFO_PRINTF2(_L("Failing after %d Allocs"),failure);
			first.__DbgCheckHeap(0);
			}
		else
			{
			INFO_PRINTF2(_L("Passing after %d Allocs"),failure);
			}
		
		first.__DbgFailNextAvailable(oomAvailable);
		failure++;
		}

	INFO_PRINTF1(_L("Phone::GetStatus tested OK"));

	phone.Close();

	// Flush any FailNext there might be hanging around.
	if (phone.Open(first,DSTD_PHONE_NAME)==KErrNone)
		phone.Close();

	first.__DbgMarkEnd(0);
	first.Close();
	User::After(300000);
	
	// KMaxTests is used to stop the above while statement looping
	// indefinitely. If KMaxTests is reached then we can guess that something
	// has gone wrong and the test should be failed.
	TESTCHECKCONDITION(failure < KMaxTests);

#else
	INFO_PRINTF1(_L("Test disabled on release builds."));	
#endif // _DEBUG

	return TestStepResult();
	}
