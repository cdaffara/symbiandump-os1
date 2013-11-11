// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test process priority control
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#include "TPROCPRI.H"

TName OtherGroupName;

TInt ProcPriTestOtherProcess(TAny *aScreenNumber)
	{
	CTrapCleanup* CleanUpStack=CTrapCleanup::New();
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = NULL;
	TInt err;
	TRAP(err, screen = new (ELeave) CWsScreenDevice(ws));
	if (err!=KErrNone)
		return err;
	if ((err=screen->Construct((TInt)aScreenNumber))!=KErrNone)
		{
		delete screen;
		return err;
		}
	
	RWindowGroup group(ws);
	group.Construct(888);
	group.SetName(OTHER_PROC_GROUP_NAME);
	RSemaphore sem1;
	RSemaphore sem2;
	sem1.OpenGlobal(PROC_PRI_SEMAPHORE_NAME1);
	sem2.OpenGlobal(PROC_PRI_SEMAPHORE_NAME2);
	sem1.Signal();	// Signal thread fully initialised
	sem2.Wait();		// Wait for command to disable focus
//
	group.EnableReceiptOfFocus(EFalse);
	ws.Flush();
	sem1.Signal();	// Signal focus disabled
	sem2.Wait();		// Wait for command to enable focus

	group.EnableReceiptOfFocus(ETrue);
	ws.Flush();
	sem1.Signal();	// Signal focus enabled
	sem2.Wait();		// Wait until signalled to exit
//
	group.EnableReceiptOfFocus(EFalse);	// To stop shell being tasked into foreground
	delete screen;
	ws.Close();
	sem1.Close();
	sem2.Close();
	delete CleanUpStack;
	return(KErrNone);
	}

CTProcPri::CTProcPri(CTestStep* aStep) : CTWsGraphicsBase(aStep)
	{
	}

CTProcPri::~CTProcPri()
	{
	if (iFlags&ECreatedSem1)
		iSem1.Close();
	if (iFlags&ECreatedSem2)
		iSem2.Close();
	delete iProcess;
	}

void CTProcPri::TestPriChangeL()
	{
	#define BACK_PRI EPriorityForeground
	#define FORE_PRI EPriorityForeground
	RProcess proc;
	TEST(proc.Priority()==BACK_PRI);
	if (proc.Priority()!=BACK_PRI)
		INFO_PRINTF3(_L("proc.Priority() return value - Expected: %d, Actual: %d"), BACK_PRI, proc.Priority());		

	iSem2.Signal();
	iSem1.Wait();

	TEST(proc.Priority()==FORE_PRI);
	if (proc.Priority()!=FORE_PRI)
		INFO_PRINTF3(_L("proc.Priority() return value - Expected: %d, Actual: %d"), FORE_PRI, proc.Priority());		

	iSem2.Signal();
	iSem1.Wait();

	TEST(proc.Priority()==BACK_PRI);
	if (proc.Priority()!=BACK_PRI)
		INFO_PRINTF3(_L("proc.Priority() return value - Expected: %d, Actual: %d"), BACK_PRI, proc.Priority());		

	TInt ident;
	User::LeaveIfError(ident=TheClient->iWs.FindWindowGroupIdentifier(0,OTHER_PROC_GROUP_NAME,0));
	TInt retVal = TheClient->iWs.SetWindowGroupOrdinalPosition(ident,1);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("TheClient->iWs.SetWindowGroupOrdinalPosition(ident,1) return value - Expected: %d, Actual: %d"), KErrNone, retVal);		

	TEST(proc.Priority()==FORE_PRI);
	if (proc.Priority()!=FORE_PRI)
		INFO_PRINTF3(_L("proc.Priority() return value - Expected: %d, Actual: %d"), FORE_PRI, proc.Priority());		

	retVal = TheClient->iWs.SetWindowGroupOrdinalPosition(ident,0);
	TEST(retVal==KErrNone);
	if (retVal!=KErrNone)
		INFO_PRINTF3(_L("TheClient->iWs.SetWindowGroupOrdinalPosition(ident,0) return value - Expected: %d, Actual: %d"), KErrNone, retVal);		

	TEST(proc.Priority()==BACK_PRI);
	if (proc.Priority()!=BACK_PRI)
		INFO_PRINTF3(_L("proc.Priority() return value - Expected: %d, Actual: %d"), BACK_PRI, proc.Priority());		

	TRequestStatus stat;
	iProcess->Logon(stat);		//Must Logon before the last Signal so we can be sure that it is still alive to get round a base issue
	iSem2.Signal();
	User::WaitForRequest(stat);

	TEST(proc.Priority()==FORE_PRI);
		if (proc.Priority()!=FORE_PRI)
		INFO_PRINTF3(_L("proc.Priority() return value - Expected: %d, Actual: %d"), FORE_PRI, proc.Priority());		

	}

void CTProcPri::ConstructL()
	{
	User::LeaveIfError(iSem1.CreateGlobal(PROC_PRI_SEMAPHORE_NAME1,0,KOwnerType));
	iFlags|=ECreatedSem1;
	User::LeaveIfError(iSem2.CreateGlobal(PROC_PRI_SEMAPHORE_NAME2,0,KOwnerType));
	iFlags|=ECreatedSem2;
	iProcess=CProcess::NewL(CProcess::eProcessPriortyTest,iTest->iScreenNumber);
	iSem1.Wait();
	}
	
void CTProcPri::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	_LIT(KTest1,"Priority Change");
	((CTProcPriStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
		
	switch(++iTest->iState)
		{
/**

  @SYMTestCaseID GRAPHICS-WSERV-0315
  
  @SYMDEF             DEF081259
 
  @SYMTestCaseDesc Test that the priority of a process or thread changes depending
  					on how the the process or thread is being used
    
  @SYMTestPriority High
 
  @SYMTestStatus Implemented
 
  @SYMTestActions Create a process or thread and use it. Check the priority of the
  					process or thread changes depending on how it is being used
  
  @SYMTestExpectedResults Prioirty of the process of thread changes depending on
  							how it is being used
 
*/
	case 1:
		((CTProcPriStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0315"));
		iTest->LogSubTest(KTest1);
		TestPriChangeL();
		break;
	case 2:
		((CTProcPriStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTProcPriStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTProcPriStep*)iStep)->RecordTestResultL();
	}
	
__WS_CONSTRUCT_STEP__(ProcPri)
