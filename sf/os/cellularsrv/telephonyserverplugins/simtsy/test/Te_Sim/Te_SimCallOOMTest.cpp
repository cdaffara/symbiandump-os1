// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimCallOOMTest.h"

CSimCallOOMTest::CSimCallOOMTest() 
	{ 
	SetTestStepName(_L("CallOOMTest"));
	}

TVerdict CSimCallOOMTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginCallOOMTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	iTelServer.SetPriorityClient();

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iLine.Open(iPhone,KVoiceLineName)==KErrNone);
	INFO_PRINTF1(_L("Opened Line object"));

	TName callName;
	TESTL(iCall.OpenNewCall(iLine,callName)==KErrNone);
	INFO_PRINTF1(_L("Opened Call object"));

	iLine.Close();
	iCall.Close();

	TESTL(iLine.Open(iPhone,KVoiceLineName)==KErrNone);
	INFO_PRINTF1(_L("Opened Line object"));

	TName callName2;
	TInt error = iCall.OpenNewCall(iLine,callName2);
	TESTL(error==KErrNone);
	INFO_PRINTF1(_L("Opened Call object"));
	
#ifdef _DEBUG
	TRequestStatus stat;
	TBool oomAvailable = EFalse;
	TInt failure(0);
	const TInt KMaxTests = 1000;
	_LIT(KEmergencyNo, "01632960000");
	
	while (!oomAvailable && failure < KMaxTests)
		{
		INFO_PRINTF2(_L("Failing alloc %d. Making Call... "),failure);

		iTelServer.__DbgFailNext(failure);

		// Check that the heap is balanced at the same level before and 
		// after all attempts at emergency call.
		iTelServer.__DbgMarkHeap();
		iCall.DialEmergencyCall(stat, KEmergencyNo);
		User::WaitForRequest(stat);
		INFO_PRINTF2(_L("result=%d... "), stat.Int());

		if (stat.Int()!=KErrNone)
			{
			INFO_PRINTF2(_L("Failing after %d Allocs"),failure);
			iTelServer.__DbgMarkEnd(0);
			}
		else
			{
			INFO_PRINTF2(_L("Passing after %d Allocs"),failure);
			}
			
		iTelServer.__DbgFailNextAvailable(oomAvailable);

		TInt ret = iCall.HangUp();
		INFO_PRINTF2(_L("HangUp result=%d"), ret);

		if (ret!=KErrNone)
			{
			iCall.HangUp();//if hang up unsuceessfull call again without heap fail
			}

		// Test for the end of the test. Not perfect (doesn't demonstrate 
		// that there wasn't an OOM situation that was handled quietly 
		// such that the call/hangup still worked) but I think it's all we 
		// can do.
		
		if ( stat!=KErrNone || ret!=KErrNone)
			{
			INFO_PRINTF3(_L("OOM testing complete. Call status is %d and hang-up return is %d"),stat.Int(),ret);
			break;
			}
			
		
		failure++;
		}

	// Resetting the heap to flush any FailNext there might be hanging around.
	iTelServer.__DbgFailNext(0);

	iCall.DialEmergencyCall(stat, KEmergencyNo);//use this to make the previous memory failure happen
	User::WaitForRequest(stat);
	iCall.HangUp();
	
	// KMaxTests is used to stop the above while statement looping
	// indefinitely. If KMaxTests is reached then we can guess that something
	// has gone wrong and the test should be failed.
	TESTL(failure < KMaxTests);
#else
	INFO_PRINTF1(_L("TestDisabled on release build"));
#endif // _DEBUG	
	INFO_PRINTF1(_L("Closing objects..."));
	iCall.Close();
	iLine.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

