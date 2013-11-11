// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <ssm/ssmcommandlist.h>
#include "ssmdebug.h"
#include "ssmdebug.h"
#include "ssmpanic.h"
#include <ssm/ssmcommand.h>
#include "ssmcommandbase.h"
#include <ssm/ssmcommandfactory.h>
#include "ssmcommandlistinterface.h"

#include "tcle_step_rclesession.h"


//
//

const TInt KWaiterArrayCount = 10;




//
//

class CExecutionWaiter : public CActive
	{
public:
	static CExecutionWaiter* NewL( TInt aId );
	static CExecutionWaiter* NewLC( TInt aId );
	~CExecutionWaiter();
	
protected:
	void RunL();
	void DoCancel();

private:
	CExecutionWaiter( TInt aId );
	void ConstructL();

private:
	RCleSession iSession;
	CSsmCommandList* iCmdList;
	TInt iId;
	};


void CExecutionWaiter::RunL()
	{
	RDebug::Print( _L("CExecutionWaiter: Test list '%d' processing completed\n"), iId );
	
	if( KWaiterArrayCount == (iId + 1) )
		{
		CActiveScheduler::Stop();
		}
	}

CExecutionWaiter* CExecutionWaiter::NewL( TInt aId )
	{
	CExecutionWaiter* self = new(ELeave) CExecutionWaiter( aId );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CExecutionWaiter* CExecutionWaiter::NewLC( TInt aId )
	{
	CExecutionWaiter* self = new(ELeave) CExecutionWaiter( aId );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;	
	}

CExecutionWaiter::CExecutionWaiter( TInt aId )
: CActive( EPriorityStandard ), iId( aId )
	{
	CActiveScheduler::Add( this );
	}

void CExecutionWaiter::ConstructL()
	{
	iCmdList = CSsmCommandList::NewL();
	TInt listCount = iId + 1;
	TCmdErrorSeverity severity;
	for(TInt i = 0; i < listCount; i++ )
		{
		MSsmCommand* cmd = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdLowSeverity);
		iCmdList->AppendL(cmd);
		CleanupStack::Pop(cmd);
		}
		
	iSession.ConnectL(KCleSrver);
	iSession.ExecuteCommandList( *iCmdList, iStatus, severity );
	SetActive();
	}

void CExecutionWaiter::DoCancel()
	{
	}

CExecutionWaiter::~CExecutionWaiter()
	{
	Cancel();
	if( iCmdList )
		{
		delete iCmdList;
		iCmdList = NULL;
		}
	iSession.Close();
	}



//
//

CSsmCommandList* CCleRCleSessionTest::CreateListWithWaitForApparcInitL(TInt aCount)
	{
	INFO_PRINTF2(_L("Constructing a test Command list consisting of '%d' WaitForApparcInit commands..."), aCount);
	CSsmCommandList* cmdList = CSsmCommandList::NewLC();
	
	for(TInt i = 0; i < aCount; i++)
		{
		MSsmCommand* cmd = SsmCommandFactory::ConstructWaitForApparcInitCommandLC(ECmdLowSeverity);
		cmdList->AppendL(cmd);
		CleanupStack::Pop(cmd);
		}
	CleanupStack::Pop(cmdList);
	return cmdList;
	}


/**
Construct a command list and execute it.
*/
void CCleRCleSessionTest::ExecuteCommandListL()
	{
	INFO_PRINTF1(_L("ExecuteCommandListL() test case started..."));
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	CSsmCommandList* cmdList = CreateListWithWaitForApparcInitL(3);

	TRequestStatus trs;
	TCmdErrorSeverity severity;
	iCleCli.ExecuteCommandList(*cmdList, trs, severity);	
	User::WaitForRequest(trs);
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs.Int(), KErrNone);
	TEST(trs == KErrNone);
	
	delete cmdList;
	CleanupStack::PopAndDestroy(sched);
	
	INFO_PRINTF1(_L("ExecuteCommandListL() finished!"));
	}

/**
Construct a command list and execute it with inter-command delays.
*/
void CCleRCleSessionTest::ExecuteCommandListWithDelaysL()
	{
	INFO_PRINTF1(_L("ExecuteCommandListWithDelaysL() test case started..."));
	
	const TInt KInterCommandDelay = 200; // ms
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	
	CSsmCommandList* cmdList = CreateListWithWaitForApparcInitL(3);
	cmdList->SetDelayBetweenCommands( KInterCommandDelay );
	
	TRequestStatus trs;
	TCmdErrorSeverity severity;
	iCleCli.ExecuteCommandList(*cmdList, trs, severity);	
	User::WaitForRequest(trs);
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs.Int(), KErrNone);
	TEST(trs == KErrNone);
	
	delete cmdList;
	CleanupStack::PopAndDestroy(sched);
	
	INFO_PRINTF1(_L("ExecuteCommandListWithDelaysL() finished!"));
	}


void CCleRCleSessionTest::ExecuteEmptyListL()
	{
	INFO_PRINTF1(_L("ExecuteEmptyListL() test case started...") );
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( sched );
	CActiveScheduler::Install( sched );
	TCmdErrorSeverity severity;
	
	CSsmCommandList* cmdList = CSsmCommandList::NewL();
	CleanupStack::PushL(cmdList);
	
	TRequestStatus trs;
	iCleCli.ExecuteCommandList(*cmdList, trs, severity);	
	User::WaitForRequest(trs);
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs.Int(), KErrArgument);
	TEST(trs == KErrArgument);
	
	CleanupStack::PopAndDestroy(cmdList);
	CleanupStack::PopAndDestroy(sched);
	
	INFO_PRINTF1(_L("ExecuteEmptyListL() finished!"));
	}


void CCleRCleSessionTest::ExecuteListAndCancelL()
	{
	INFO_PRINTF1(_L("ExecuteListAndCancelL() test case started...") );
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( sched );
	CActiveScheduler::Install( sched );
	
	CSsmCommandList* cmdList = CreateListWithWaitForApparcInitL(2);
	
	TRequestStatus trs;
	TCmdErrorSeverity severity;
	iCleCli.ExecuteCommandList(*cmdList, trs, severity);
	iCleCli.ExecuteCommandListCancel();
	User::WaitForRequest( trs );
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs.Int(), KErrCancel);
	TEST(trs == KErrCancel);
	
	delete cmdList;
	CleanupStack::PopAndDestroy(sched);
	
	INFO_PRINTF1(_L("ExecuteListAndCancelL() finished!"));
	}


void CCleRCleSessionTest::ExecuteListTocompletionThenCancelL()
	{
	INFO_PRINTF1(_L("ExecuteListTocompletionThenCancelL() test case started...") );
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( sched );
	CActiveScheduler::Install( sched );
	
	CSsmCommandList* cmdList = CreateListWithWaitForApparcInitL(2);
	
	TRequestStatus trs;
	TCmdErrorSeverity severity;
	iCleCli.ExecuteCommandList(*cmdList, trs, severity);
	User::After( 2000000 ); // Allow sufficient time for the list to complete
	iCleCli.ExecuteCommandListCancel();
	User::WaitForRequest( trs );
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs.Int(), KErrNone);
	TEST(trs == KErrNone);
	
	delete cmdList;
	CleanupStack::PopAndDestroy( sched );

	INFO_PRINTF1(_L("ExecuteListTocompletionThenCancelL() finished!"));
	}


void CCleRCleSessionTest::ExecuteTwoListsL()
	{
	INFO_PRINTF1(_L("ExecuteTwoListsL() test case started...") );

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( sched );
	CActiveScheduler::Install( sched );
	
	CSsmCommandList* cmdList1 = CreateListWithWaitForApparcInitL(3);
	CSsmCommandList* cmdList2 = CreateListWithWaitForApparcInitL(3);
	
	
	TRequestStatus trs2;
	TCmdErrorSeverity severity;
	iCleCli2.ExecuteCommandList(*cmdList2, trs2, severity);
	
	TRequestStatus trs1;
	iCleCli.ExecuteCommandList(*cmdList1, trs1, severity);
	
	User::WaitForRequest(trs1);
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs1.Int(), KErrNone);
	TEST(trs1 == KErrNone);
	User::WaitForRequest(trs2);
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs2.Int(), KErrNone);
	TEST(trs2 == KErrNone);
	
	delete cmdList1;
	delete cmdList2;
	
	CleanupStack::PopAndDestroy(sched);

	INFO_PRINTF1(_L("ExecuteTwoListsL() finished!"));
	}


/**
Exercise the registration and deregistration logic within the server. ie check the array-slot re-use.
*/
void CCleRCleSessionTest::TryToBreakSessionRegistrationL()
	{
	INFO_PRINTF1(_L("TryToBreakSessionRegistrationL() test case started...") );

	RArray<RCleSession> sessionArray;
	CleanupClosePushL( sessionArray );
	const TInt KHandleArrayCount = 10;
	sessionArray.Reserve( KHandleArrayCount );

	INFO_PRINTF2(_L("***** Handle array count '%d'."), sessionArray.Count());
	// Open KHandleArrayCount handles on the CleSrv (these are in addition to the test-harness ones)
	for( TInt i = 0; i < KHandleArrayCount; i++ )
		{
		RCleSession session;
		
		sessionArray.AppendL( session );
		sessionArray[ i ].ConnectL(KCleSrver);
		}
	
	// Close two arbitrarily chosen handles
	sessionArray[ 3 ].Close();
	sessionArray[ 7 ].Close();
	// Declare and open two new ones
	RCleSession newSession_1;
	RCleSession newSession_2;
	newSession_1.ConnectL(KCleSrver);
	newSession_2.ConnectL(KCleSrver);
	// Close them 
	newSession_1.Close();
	newSession_2.Close();
	newSession_1.Close(); // Already closed
	newSession_2.Close(); // Already closed
	
	// Close the array of handles (two are already closed)
	for(TInt i = 0; i < sessionArray.Count(); i++ )
		{
		sessionArray[ i ].Close();
		}	

	CleanupStack::PopAndDestroy( &sessionArray );
	INFO_PRINTF1(_L("TryToBreakSessionRegistrationL() finished!"));
	}


/***
 * CExecutionWaiter is an AO which executes a command list.
 * As each one is instantiated, it compiles and executes a command list of i+1 items  
 */
void CCleRCleSessionTest::ExecuteCmdListPluralityL()
	{
	INFO_PRINTF1(_L("ExecuteCmdListPluralityL() test case started...") );
	
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( sched );
	CActiveScheduler::Install( sched );

	RPointerArray<CExecutionWaiter> waiterArray( 4 );
	CleanupClosePushL( waiterArray );
	
	for(TInt i = 0; i < KWaiterArrayCount; i++ )
		{
		CExecutionWaiter* waiter = CExecutionWaiter::NewLC( i );
		waiterArray.AppendL( waiter );
		CleanupStack::Pop( waiter );
		}	
	
	sched->Start();
	
	waiterArray.ResetAndDestroy();	
	CleanupStack::PopAndDestroy( 2, sched );
	INFO_PRINTF1(_L("ExecuteCmdListPluralityL() finished!"));
	}


/**
 Commence list execution and immediately close the handle. Ensure everyone looks after themselves.
*/
void CCleRCleSessionTest::OpenHandleExecuteAndCloseL()
	{
	INFO_PRINTF1(_L("OpenHandleExecuteAndCloseL() test case started...") );

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( sched );
	CActiveScheduler::Install( sched );
	
	RCleSession session;
	session.ConnectL(KCleSrver);
	
	CSsmCommandList* cmdList = CreateListWithWaitForApparcInitL(3);
	
	TRequestStatus trs;
	TCmdErrorSeverity severity;
	session.ExecuteCommandList(*cmdList, trs, severity);
	session.Close();
	User::WaitForRequest(trs);
	//expected should be cancel as ExecuteCommandListCancel() will be called once session is closed
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs.Int(), KErrCancel);
	TEST(trs == KErrCancel);
	
	delete cmdList;
	
	CleanupStack::PopAndDestroy( sched );
	INFO_PRINTF1(_L("OpenHandleExecuteAndCloseL() finished!"));
	}


void CCleRCleSessionTest::VariousHandleBadnessL()
	{
	INFO_PRINTF1(_L("VariousHandleBadnessL() test case started...") );

	INFO_PRINTF1(_L("***** Close non-open handle"));
	// Close non-open handle
	RCleSession session;
	session.Close();
	session.Close();
	
	INFO_PRINTF1(_L("***** Multiple close once open handle"));
	// Multiple close once open handle
	session.ConnectL(KCleSrver);
	session.Close();
	session.Close();
	
	INFO_PRINTF1(_L("***** Cancel list execution on non-open handle"));
	// Cancel list execution on non-open handle
	RCleSession anotherSession;
	anotherSession.ExecuteCommandListCancel();
	anotherSession.ExecuteCommandListCancel();
	
	INFO_PRINTF1(_L("***** Cancel list execution on open, but nonexecuting handle"));
	// Cancel list execution on open, but nonexecuting handle
	anotherSession.ConnectL(KCleSrver);
	anotherSession.ExecuteCommandListCancel();
	anotherSession.ExecuteCommandListCancel();
	anotherSession.Close();	
	INFO_PRINTF1(_L("***** Cancel list execution on closed handle"));	
	anotherSession.ExecuteCommandListCancel();
	anotherSession.ExecuteCommandListCancel();
	
	INFO_PRINTF1(_L("***** Call ConnectL twice"));
	RCleSession yetAnotherSession;
	yetAnotherSession.ConnectL(KCleSrver);
	TRAPD(err, yetAnotherSession.ConnectL(KCleSrver));
	TEST(KErrAlreadyExists == err);
	if (KErrAlreadyExists == err)
		{
		err = KErrNone;
		}
	User::LeaveIfError(err);
	yetAnotherSession.Close();

	INFO_PRINTF1(_L("VariousHandleBadnessL() finished!"));
	}


void CCleRCleSessionTest::MultipleCallsToExecuteL()
	{
	INFO_PRINTF1(_L("MultipleCallsToExecuteL() test case started...") );

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( sched );
	CActiveScheduler::Install( sched );
	
	CSsmCommandList* cmdList = CreateListWithWaitForApparcInitL(3);
	
	TRequestStatus trs1;
	TRequestStatus trs2; 
	//different TRequestStatus is required because if we use same both commandlists then without waiting 
	//for the first commandlist execution request to complete the commandlist gets deleted
	TCmdErrorSeverity severity;
	iCleCli.ExecuteCommandList(*cmdList, trs1, severity);
	iCleCli.ExecuteCommandList(*cmdList, trs2, severity); // The badness
	User::WaitForRequest(trs1);
	User::WaitForRequest(trs2);
	INFO_PRINTF3(_L("WaitForRequest completed with '%d', expected was '%d'."), trs2.Int(), KErrServerBusy);
	TEST(trs2 == KErrServerBusy);
	
	delete cmdList;
	CleanupStack::PopAndDestroy( sched );
	
	INFO_PRINTF1(_L("MultipleCallsToExecuteL() finished!"));
	}


//
//


CCleRCleSessionTest::~CCleRCleSessionTest()
	{
	}

CCleRCleSessionTest::CCleRCleSessionTest()
	{
	SetTestStepName(KTCleRCleSessionStep);
	}

TVerdict CCleRCleSessionTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CCleRCleSessionTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

/**
Old Test CaseID 		APPFWK-CLE-0001
New Test CaseID 		DEVSRVS-SSMA-CLE-0001
 */

TVerdict CCleRCleSessionTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CCleRCleSessionTest started....."));
	__UHEAP_MARK;
	
	TRAPD(err, iCleCli.ConnectL(KCleSrver));
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("Connecting client to server completed with code '%d'."), err);
	TEST(iCleCli.HeapMark() == KErrNone);
	
	INFO_PRINTF1(_L("Checking we still can connect a second client to server."));
	TRAP(err, iCleCli2.ConnectL(KCleSrver) );
	TEST(err == KErrNone);

	// Starting all test cases...
	TRAP(err, ExecuteCommandListL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListL() completed with code '%d'."), err);

	TRAP(err, ExecuteCommandListWithDelaysL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCommandListWithDelaysL() completed with code '%d'."), err);
		
	TRAP(err, ExecuteListAndCancelL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteListAndCancelL() completed with code '%d'."), err);

	TRAP(err, ExecuteListTocompletionThenCancelL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteListTocompletionThenCancelL() completed with code '%d'."), err);
	
	TRAP(err, ExecuteTwoListsL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteTwoListsL() completed with code '%d'."), err);
	
	TRAP(err, ExecuteCmdListPluralityL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteCmdListPluralityL() completed with code '%d'."), err);
	
	TRAP(err, ExecuteEmptyListL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("ExecuteEmptyListL() completed with code '%d'."), err);
	
	TRAP(err, TryToBreakSessionRegistrationL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("TryToBreakSessionRegistrationL() completed with code '%d'."), err);
	
	TRAP(err, OpenHandleExecuteAndCloseL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("OpenHandleExecuteAndCloseL() completed with code '%d'."), err);
	
	TRAP(err, VariousHandleBadnessL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("VariousHandleBadnessL() completed with code '%d'."), err);
	
	TRAP(err, MultipleCallsToExecuteL());
	TEST(err == KErrNone);
	INFO_PRINTF2(_L("MultipleCallsToExecuteL() completed with code '%d'."), err);

	// Clean-up & finish	
	iCleCli2.Close();
	User::After(100000);	// 0.1seconds to wait for cmd list to be deleted.
	TEST(iCleCli.HeapMarkEnd() == KErrNone);
	iCleCli.Close();
	__UHEAP_MARKEND;
	INFO_PRINTF1(_L("....CCleRCleSessionTest finished!!!"));

	return TestStepResult();
	}
	
