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
#include "TE_Etelmemrelinquish.h"

CTestMemRelinquish::CTestMemRelinquish()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestMemRelinquish"));
	}

enum TVerdict CTestMemRelinquish::doTestStepL()
   	{
   	INFO_PRINTF1(_L("Test Call Relinquish Case 2"));
 
 	User::SetJustInTime(EFalse);	// switches off the WINS threadbreakpoints
 
 	RThread t;
 	TInt res=t.Create(_L("RelinquishCall"),RelinquishCall,KDefaultStackSize,KDefaultHeapSize,KDefaultHeapSize,NULL);
 	TESTCHECK(res, KErrNone);
 
 	TRequestStatus stat = KRequestPending;
 	t.Logon(stat);
 	t.Resume();
 	User::WaitForRequest(stat);
 
 	TESTCHECK(t.ExitType(), EExitKill);
 	//TESTCHECK(t.ExitCategory()==_L("Etel Client Faul"));
 	//TESTCHECK(t.ExitReason()==11);	// EEtelPanicHandleNotClosed is 11
 	//TESTCHECK(t.ExitType()==EExitPanic);
 	t.Close();
 
 	User::SetJustInTime(ETrue);
 	User::After(KETelThreadShutdownGuardPeriod);
 
 	return TestStepResult();
 	}
 	



 TInt CTestMemRelinquish::RelinquishCall(TAny * /* aArg */)
 	{
 	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack
 	
 	RTelServer serverA;
 	TInt ret = serverA.Connect();
   	ret=serverA.LoadPhoneModule(DSTD_MODULE_NAME);
 
 	RCall callA;
    ret = callA.OpenNewCall(serverA,DSTD_CALL_FULLNAME);
 	RCall::TCallParams callParams;
 	RCall::TCallParamsPckg callParamsPckg(callParams);
 
    ret=callA.Dial(callParamsPckg,DACQ_PHONE_NUMBER_TO_DIAL);
 	serverA.Close();
 
 	User::After(KETelThreadShutdownGuardPeriod);
 
 	delete cleanup;
 	return KErrNone;
 	}
 
