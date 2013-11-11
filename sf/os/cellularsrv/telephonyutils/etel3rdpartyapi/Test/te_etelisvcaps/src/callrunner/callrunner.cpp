// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Imlementation of test purpose utility application. 
// It establishes mobile orginated calls on the system. 
// The command line parameter accepts an integer parameter, 
// which specifies the number of calls to be created.  
// 
//

/**
 @file 
 @internalTechnology
*/

#include <bacline.h>
#include "callrunner.h"
#include "callrunnercommon.h"
#include "te_capscallao.h"

_LIT(KCallRunnerStr, "TestCallRunner");

/**
Constructs the CCallRunner instance/
*/
CCallRunner* CCallRunner::NewL()
	{
	CCallRunner* me = new (ELeave) CCallRunner;
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

/**
Second phase construction
*/
void CCallRunner::ConstructL()
	{
	iTelephony = CTelephony::NewL();
	// rename process
	RProcess process;
	process.RenameMe(KCallRunnerStr);
	
	// read the calls number
	CCommandLineArguments* clArg = CCommandLineArguments::NewLC();
	TPtrC buff = clArg->Arg(1);
	TBuf<255> number(buff);
	number.TrimAll();
	if (number.Length() != 1)
		{
		User::Leave(KErrArgument);
		}
	
	iNumberOfCalls = number[0] - '0';
	if (iNumberOfCalls < 0 || iNumberOfCalls > 2)
		{
		User::Leave(KErrArgument);
		}
	
	CleanupStack::PopAndDestroy(clArg);
	
	// set up the property
	static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
	User::LeaveIfError( iCallRunnerProperty.Define(KCallRunnerPropertyKey,
							RProperty::EInt,KAllowAllPolicy,KAllowAllPolicy) );
	User::LeaveIfError( iCallRunnerProperty.Attach(KCallRunnerPropertyUid, KCallRunnerPropertyKey) );
	
	// make calls
	_LIT(KGoodNumber,"1789");
	CTelephony::TCallStatusV1 status;
	CTelephony::TCallStatusV1Pckg statusPckg(status);
	
	//Set the call parameters for call 1
	CTelephony::TCallParamsV1 callParams1;
	callParams1.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg1(callParams1);	

	TTestCallParams testCallParams1;
	testCallParams1.iLine = CTelephony::EVoiceLine;
	testCallParams1.iCallParamsV1Pckg = &callParamsPckg1;
	testCallParams1.iTelAddress.iTelNumber = KGoodNumber;
	testCallParams1.iExpStat = KErrNone;

	// Create active DialNewCallAct objects
	CTestDialNewCallAct* testDialNewCallAct = CTestDialNewCallAct::NewLC(NULL, iTelephony);
	testDialNewCallAct->TestDialNewCall(testCallParams1);
	CActiveScheduler::Start();
	User::LeaveIfError(iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg));
	if (iNumberOfCalls == 2)
		{
		//Set the call parameters for call 2
		CTelephony::TCallParamsV1 callParams2;
		callParams2.iIdRestrict = CTelephony::ESendMyId;
		CTelephony::TCallParamsV1Pckg callParamsPckg2(callParams2);	

		TTestCallParams testCallParams2;
		testCallParams2.iLine = CTelephony::EVoiceLine;
		testCallParams2.iCallParamsV1Pckg = &callParamsPckg2;
		testCallParams2.iTelAddress.iTelNumber = KGoodNumber;
		testCallParams2.iExpStat = KErrNone;
		
		// Put on hold first
		CTestHoldAct* TestHoldAct = CTestHoldAct::NewLC(NULL,iTelephony);	
		TestHoldAct->TestHold(testCallParams1);
		CActiveScheduler::Start();	

		User::LeaveIfError(iTelephony->GetCallStatus(testCallParams1.iCallId,  statusPckg));
		
		// Create active DialNewCallAct objects
		testDialNewCallAct->TestDialNewCall(testCallParams2);
		CActiveScheduler::Start();
		
		User::LeaveIfError(iTelephony->GetCallStatus(testCallParams2.iCallId,  statusPckg));
		
		CleanupStack::PopAndDestroy(TestHoldAct);
		}
 	CleanupStack::PopAndDestroy(testDialNewCallAct);
 	
 	// Inform the client calls are established
 	User::LeaveIfError( iCallRunnerProperty.Set(KCallRunnerPropertyUid, KCallRunnerPropertyKey, ECallRunnerStarted) );
 	}
	
/**
Object descrutor. Performs required cleanup. 
*/
CCallRunner::~CCallRunner()
	{
	iCallRunnerProperty.Close();
	iCallRunnerProperty.Delete(KCallRunnerPropertyKey);
	delete iTelephony;
	}
	
/**
Waits until the message on the property is published.
*/	
void CCallRunner::WaitToClose()
	{
	TRequestStatus s;
	TInt value;
	do 
		{
		iCallRunnerProperty.Subscribe(s);
		User::WaitForRequest(s);
		iCallRunnerProperty.Get(value);
		} while (value != ECallRunnerCloseDown);
	}
	
LOCAL_C void MainL()
/**
Secure variant
*/
	{
	// Leave the hooks in for platform security
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CCallRunner* callRunner = NULL;
	
	// Create the CTestServer derived server
	callRunner = CCallRunner::NewL();
		
		// waits on client notification to hangup and close
	callRunner->WaitToClose();
	delete callRunner;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
@return - Standard Epoc error code on process exit
Secure variant only
Process entry point. Called by client using RProcess API
*/
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
	}


