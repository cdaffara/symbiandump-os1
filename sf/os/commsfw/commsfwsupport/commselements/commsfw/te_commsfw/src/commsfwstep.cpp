// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file CommsFWStep.cpp
*/
#include "CommsFWStep.h"
#include <test/testexecutelog.h>
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
#include <comms-infras/commsbufpondop.h>
#include <es_mbuf.h>
#else
#include <es_mbman.h>
#endif

TVerdict CCFTestBase::easyTestStepPreambleL()
	{
	LeaveUnlessPassedL(CEasyTestStep::easyTestStepPreambleL());
	iScheduler = new (ELeave)CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	return EPass;
	}

TVerdict CCFTestBase::easyTestStepPostambleL()
	{
	return CEasyTestStep::easyTestStepPostambleL();
	}

void CCFTestBase::LeaveUnlessPassedL(TVerdict aVerdict)
	{
	if(aVerdict != EPass)
		{
		User::Leave(KErrAbort);
		}
	}

CCFTestBase::~CCFTestBase()
	{
	delete iScheduler; 
	iScheduler = NULL;
	}

//

TVerdict CTestUDSP::easyTestStepPreambleL()
	{
	LeaveUnlessPassedL(CTestSP<RChanUDSP>::easyTestStepPreambleL());
	TUniDirectionalOpenClose<RChanUDSP> createHelper(Logger());
	return createHelper.CreateEndpointsL(iUpper, iLower);
	}

TVerdict CTestUDSP::easyTestStepPostambleL()
	{
	TUniDirectionalOpenClose<RChanUDSP> createHelper(Logger());
	LeaveUnlessPassedL(createHelper.CloseEndpointsL(iUpper, iLower));
	return CTestSP<RChanUDSP>::easyTestStepPostambleL();
	}


//

_LIT(KOOMTestStepName, "OOMCreationAndClose");

TVerdict CTestUDTP::easyTestStepPreambleL()
	{
	LeaveUnlessPassedL(CCFTestCommsChannel<RChanUDTP>::easyTestStepPreambleL());
	// The OOM case does all its own work
	TPtrC testCaseName;
	GetStringFromConfig(ConfigSection(), KTestTag, testCaseName);
	if(testCaseName.CompareF(KOOMTestStepName) != 0)
		{
		TUniDirectionalOpenClose<RChanUDTP> createHelper(Logger());
		return createHelper.CreateEndpointsL(iUpper, iLower);
		}
	return EPass;
	}

TVerdict CTestUDTP::easyTestStepPostambleL()
	{
	// The OOM case does all its own work
	TPtrC testCaseName;
	GetStringFromConfig(ConfigSection(), KTestTag, testCaseName);
	if(testCaseName.CompareF(KOOMTestStepName) != 0)
		{
		TUniDirectionalOpenClose<RChanUDTP> createHelper(Logger());
		LeaveUnlessPassedL(createHelper.CloseEndpointsL(iUpper, iLower));
		}
	return CCFTestCommsChannel<RChanUDTP>::easyTestStepPostambleL();
	}

TVerdict CTestUDTP::easyTestStepL()
	{
	// The INI file section specifies which test case is to be executed.
	TPtrC testCaseName;
	GetStringFromConfig(ConfigSection(), KTestTag, testCaseName);
	if(testCaseName.CompareF(_L("RxTxWithoutNotifications")) == 0)
		{
		return RxTxWithoutNotificationsL();
		}
	else if(testCaseName.CompareF(_L("RxTxWithNotifications")) == 0)
		{
		return RxTxWithNotificationsL();
		}
	else if(testCaseName.CompareF(_L("RxTxCancelNotifications")) == 0)
		{
		return RxTxCancelNotificationsL();
		}
	else if(testCaseName.CompareF(KOOMTestStepName) == 0)
		{
		return OOMCreationAndCloseL();
		}
	return ETestSuiteError;
	}

// TEST 2.2
TVerdict CTestUDTP::RxTxWithoutNotificationsL()
	{			 
	// {.1}
	TMsg txMsg1(1);
	TInt ret = iUpper->Send(txMsg1, 1);
	TESTEQ(ret, KErrNone, "Sending on upper[1] failed with error %d", ret);

	// {.2}
	TMsg txMsg2(2);
	ret = iUpper->Send(txMsg2, 1);
	TESTEQ(ret, KErrOverflow, "Sending on upper[1] returned error %d, not KErrOverflow", ret);

	// {.3}
	TMsg txMsg3(3);
	ret = iUpper->Send(txMsg3, 2);
	TESTEQ(ret, KErrNone, "Sending on upper[2] failed with error %d", ret);

	// {.4}
	TMsg txMsg4(4);
	ret = iUpper->Send(txMsg4, 0);
	TESTEQ(ret, KErrNone, "Sending on upper[0] failed with error %d", ret);

	// {.5}
	TMsg rxMsg;
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg3, "Received msg != third msg sent", 1);

	// {.6}
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg1, "Received msg != first msg sent", 1);

	// {.7}
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg4, "Received msg != fourth msg sent", 1);

	// {.8}
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrUnderflow, "Receving from lower returned error %d, not KErrUnderflow", ret);

	return EPass;
	}

// TEST 2.3
TVerdict CTestUDTP::RxTxWithNotificationsL()
	{
	CBoundedCompletionWait* waitData = CBoundedCompletionWait::NewLC();
	CBoundedCompletionWait* waitSpace1 = CBoundedCompletionWait::NewLC();
	CBoundedCompletionWait* waitSpace2 = CBoundedCompletionWait::NewLC();

	// {.1}
	iLower->NotifyDataAvailable(*waitData);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ETimedOut, 
		"Data available notification before send (status %d)", waitData->Status());

	// {.2}
	iUpper->NotifySpaceAvailable(*waitSpace1, 1);
	waitSpace1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace1->Result(), CBoundedCompletionWait::ECompleted, 
		"No space available [1] notification before send (status %d)", waitSpace1->Status());
	
	// {.3}
	TMsg txMsg1(5);
	TInt ret = iUpper->Send(txMsg1, 1);
	TESTEQ(ret, KErrNone, "Sending on upper [1] failed with error %d", ret);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ECompleted, 
		"Data available notification not completed after send (status %d)", waitData->Status());
	
	// {.4}
	TMsg txMsg2(6);
	ret = iUpper->Send(txMsg2, 0);
	TESTEQ(ret, KErrNone, "Sending on upper [1] failed with error %d", ret);
	
	// {.5}
	TMsg txMsg3(7);
	ret = iUpper->Send(txMsg3, 2);
	TESTEQ(ret, KErrNone, "Sending on upper [1] failed with error %d", ret);

	// {.6}
	iUpper->NotifySpaceAvailable(*waitSpace1, 0);
	waitSpace1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace1->Result(), CBoundedCompletionWait::ETimedOut,
		"Space available [1] notification after send (status %d)", waitSpace1->Status());
	
	// {.7}
	iUpper->NotifySpaceAvailable(*waitSpace2, 1);
	waitSpace2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace2->Result(), CBoundedCompletionWait::ETimedOut,
		"Space available [0] notification after send (status %d)", waitSpace2->Status());
	
	// {.8}
	TMsg rxMsg;
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg3, "Received msg != highest priority msg sent", 1);
	
	// {.9}
	iLower->NotifyDataAvailable(*waitData);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ECompleted, 
		"Data available notification not completed with two sends outstanding (status %d)", waitData->Status());

	// {.10}
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg1, "Received msg != middle priority msg sent", 1);
	waitSpace2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace2->Result(), CBoundedCompletionWait::ECompleted,
		"Space available [1] notification not completed after 2nd receive (status %d)", waitSpace2->Status());

	// {.11}
	iLower->NotifyDataAvailable(*waitData);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ECompleted, 
		"Data available notification not completed with one send outstanding (status %d)", waitData->Status());

	// {.12}
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg2, "Received msg != lowest priority msg sent", 1);
	waitSpace1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace1->Result(), CBoundedCompletionWait::ECompleted,
		"Space available [1] notification not completed after 2nd receive (status %d)", waitSpace1->Status());

	// {.13}
	iLower->NotifyDataAvailable(*waitData);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ETimedOut, 
		"Data available notification after sends drained (status %d)", waitData->Status());

	// Cleanup
	iLower->CancelDataAvailable();
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ECompleted, 
		"No data available notification after cancel (status %d)", waitData->Status());
	TESTEQ(waitData->Status(), KErrCancel, 
		"Data available status not KErrCancel (status %d)", waitData->Status());

	CleanupStack::PopAndDestroy(waitSpace2);
	CleanupStack::PopAndDestroy(waitSpace1);
	CleanupStack::PopAndDestroy(waitData);
	return EPass;
	}

// TEST 2.4
TVerdict CTestUDTP::RxTxCancelNotificationsL()
	{
	CBoundedCompletionWait* waitData = CBoundedCompletionWait::NewLC();
	CBoundedCompletionWait* waitSpace1 = CBoundedCompletionWait::NewLC();
	CBoundedCompletionWait* waitSpace2 = CBoundedCompletionWait::NewLC();

	// {.1}
	iLower->NotifyDataAvailable(*waitData);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ETimedOut, 
		"Data available notification before send (status %d)", waitData->Status());

	// {.2}
	iLower->CancelDataAvailable();
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ECompleted, 
		"No data available notification after cancel (status %d)", waitData->Status());
	TESTEQ(waitData->Status(), KErrCancel, 
		"Data available status not KErrCancel (status %d)", waitData->Status());

	// {.3}
	waitData->SetStatus(KRequestPending);
	iLower->CancelDataAvailable();
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ETimedOut, 
		"Data available notification cancelled twice (status %d)", waitData->Status());

	// {.4}
	TMsg txMsg1(8);
	TInt ret = iUpper->Send(txMsg1, 0);
	TESTEQ(ret, KErrNone, "Sending on upper [0] failed with error %d", ret);

	// {.5}
	TMsg txMsg2(9);
	ret = iUpper->Send(txMsg2, 1);
	TESTEQ(ret, KErrNone, "Sending on upper [1] failed with error %d", ret);

	// {.6}
	TMsg txMsg3(10);
	ret = iUpper->Send(txMsg3, 2);
	TESTEQ(ret, KErrNone, "Sending on upper [2] failed with error %d", ret);

	// {.7}
	iUpper->NotifySpaceAvailable(*waitSpace1, 0);
	waitSpace1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace1->Result(), CBoundedCompletionWait::ETimedOut,
		"Space available notification after send (status %d)", waitSpace1->Status());

	// {.8}
	iUpper->NotifySpaceAvailable(*waitSpace2, 2);
	waitSpace2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace2->Result(), CBoundedCompletionWait::ETimedOut,
		"Space available notification after send (status %d)", waitSpace2->Status());

	// {.9}
	iUpper->CancelSpaceAvailable(0);
	waitSpace1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace1->Result(), CBoundedCompletionWait::ECompleted, 
		"No space available notification after cancel (status %d)", waitSpace1->Status());
	TESTEQ(waitSpace1->Status(), KErrCancel, 
		"Space available status not KErrCancel (status %d)", waitSpace1->Status());

	// {.10}
	waitSpace1->SetStatus(KRequestPending);
	iLower->CancelSpaceAvailable(0);
	waitSpace1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace1->Result(), CBoundedCompletionWait::ETimedOut, 
		"Space available notification cancelled twice (status %d)", waitSpace1->Status());
	TESTEQ(waitSpace2->Result(), CBoundedCompletionWait::ETimedOut, 
		"Space available notification cancelled for other priority (status %d)", waitSpace2->Status());
	TESTEQ(waitSpace2->Status(), KRequestPending, 
		"Space available notification cancelled for other priority(status %d)", waitSpace1->Status());

	// {.11}
	TMsg rxMsg;
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg3, "Received msg != highest priority msg sent", 1);
	waitSpace2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace2->Result(), CBoundedCompletionWait::ECompleted, 
		"Space available notification [2] not completed after receive (status %d)", waitSpace2->Status());
	TESTEQ(waitSpace2->Status(), KErrNone, 
		"Space available notification [2] not KErrNone (status %d)", waitSpace2->Status());

	// {.12}
	waitSpace2->SetStatus(KRequestPending);
	iLower->CancelSpaceAvailable(2);
	waitSpace2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace2->Result(), CBoundedCompletionWait::ETimedOut, 
		"Space available notification cancelled twice (status %d)", waitSpace2->Status());

	// {.13}
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg2, "Received msg != middle priority msg sent", 1);

	// {.14}
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg1, "Received msg != lowest priority msg sent", 1);

	CleanupStack::PopAndDestroy(waitSpace2);
	CleanupStack::PopAndDestroy(waitSpace1);
	CleanupStack::PopAndDestroy(waitData);
	return EPass;
	}

// TEST 2.6

void CTestUDTP::DoOOMCreationL()
	{
	// Create & open
	iUpper = new(ELeave) RChanUDTP;
	TESTTRUE(!iUpper->IsOpen(), "Upper endpoint reported as already open");
	
	TInt ret = iUpper->Create(1);
	TESTEQ(ret, KErrNone, "Upper endpoint Create failed: err %d", ret);

	TESTTRUE(iUpper->IsOpen(), "Upper endpoint reported as closed after successful creation");
	
	iLower = new(ELeave) RChanUDTP;
	TESTTRUE(!iLower->IsOpen(), "Lower endpoint reported as already open");
	
	RChanUDTP::TMsgQueues msgQueues;
	iUpper->GetMsgQueues(msgQueues);
	
	ret = iLower->Create(msgQueues);
	TESTEQ(ret, KErrNone, "Lower endpoint Create failed: err %d", ret);
	
	TESTTRUE(iLower->IsOpen(), "Lower endpoint reported as closed after successful creation");
	}

void CTestUDTP::DoOOMClose()
	{
	// Close & delete - need to tolerate half-constructed cases as may be cleaning up after OOM in creation
	if(iLower)
		{
		iLower->Close();
		delete iLower;
		iLower = NULL;
		}
	if(iUpper)
		{
		iUpper->Close();
		delete iUpper;
		iUpper = NULL;
		}
	}

TVerdict CTestUDTP::OOMCreationAndCloseL()
	{
#ifdef __FLOG_ACTIVE
	// Before marking the heap ensure that CFlog has already created all
	// the RFileLoggers it might need
	__CFLOG(KLogCommsFw, KLogMsg, _L("padding: message"));
	__CFLOG(KLogCommsFw, KLogCode, _L("padding: code"));
	__CFLOG(KLogCommsFw, KLogEvent, _L("padding: event"));
	__CFLOG(KLogCommsFw, KLogWarning, _L("padding: warning"));
	__CFLOG(KLogCommsFw, KCFLogOom, _L("padding: oom"));
#endif

	__UHEAP_MARK;
	TInt oomCount = 1;
	FOREVER
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, oomCount);

		TRAPD(ret, DoOOMCreationL());
		DoOOMClose();
		if(ret == KErrNone)
			{
			break;
			}
		oomCount++;
		}

	__UHEAP_SETFAIL(RHeap::ENone, 0);
	__UHEAP_CHECK(0);
	return EPass;
	}


//

TVerdict CTestBDSP::easyTestStepL()
	{
	// Parent may implement this one
	TVerdict verdict = CTestSP<RChanBDSP>::easyTestStepL();
	if(verdict != ETestSuiteError)
		{
		// Now close gracefully
		return GracefulCloseWithoutTimeoutL();
		}

	// The INI file section specifies which test case is to be executed.
	TPtrC testCaseName;
	GetStringFromConfig(ConfigSection(), KTestTag, testCaseName);
	if(testCaseName.CompareF(_L("NonFullQueueGCWithTimeout")) == 0)
		{
		return NonFullQueueGracefulCloseWithTimeoutL();
		}
	else if(testCaseName.CompareF(_L("FullQueueGCWithTimeout")) == 0)
		{
		return FullQueueGracefulCloseWithTimeoutL();
		}
	else if(testCaseName.CompareF(_L("FullQueueGCWithZeroTimeout")) == 0)
		{
		return FullQueueGracefulCloseWithZeroLengthTimeoutL();
		}
	else if(testCaseName.CompareF(_L("SimultaneousGC")) == 0)
		{
		return SimultaneousGracefulCloseL();
		}
	else if(testCaseName.CompareF(_L("OOMNonFullQueueGCWithTimeout")) == 0)
		{
		return OOMNonFullQueueGracefulCloseWithTimeoutL();
		}
	return ETestSuiteError;
	}

// TEST 3.1
TVerdict CTestBDSP::CreateEndpointsL()
	{
	iUpper = new(ELeave) RChanBDSP(CActive::EPriorityStandard);
	iLower = new(ELeave) RChanBDSP(CActive::EPriorityStandard);

	// {1}
	TInt ret = iUpper->CreateRecv(1);
	TESTEQ(ret, KErrNone, "Upper endpoint CreateRecv failed: err %d", ret);
	// {2}
	RChanBDSP::TMsgQueues msgQueues;
	iUpper->GetRecvMsgQueues(msgQueues);
	// {3}
	ret = iLower->CreateSend(msgQueues);
	TESTEQ(ret, KErrNone, "Lower endpoint CreateSend failed: err %d", ret);
	// {4}
	ret = iLower->CreateRecv(1);
	TESTEQ(ret, KErrNone, "Lower endpoint CreateRecv failed: err %d", ret);
	// {5}
	iLower->GetRecvMsgQueues(msgQueues);
	// {6}
	ret = iUpper->CreateSend(msgQueues);
	TESTEQ(ret, KErrNone, "Upper endpoint CreateSend failed: err %d", ret);

	return EPass;
	}

// TEST 3.5
TVerdict CTestBDSP::GracefulCloseWithoutTimeoutL()
	{
	CBoundedCompletionWait* waitData1 = CBoundedCompletionWait::NewLC();
	CBoundedCompletionWait* waitData2 = CBoundedCompletionWait::NewLC();

	// {.1}
	iUpper->NotifyDataAvailable(*waitData1);
	waitData1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData1->Result(), CBoundedCompletionWait::ETimedOut, 
		"Upper data available notification at outset (status %d)", waitData1->Status());

	// {.2}
	iLower->NotifyDataAvailable(*waitData2);
	waitData2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData2->Result(), CBoundedCompletionWait::ETimedOut, 
		"Lower data available notification at outset (status %d)", waitData2->Status());

	// {.3}
	TMsg txMsg1(5);
	TInt ret = iUpper->Send(txMsg1, 0);
	TESTEQ(ret, KErrNone, "Sending on upper [0] failed with error %d", ret);
	waitData2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData2->Result(), CBoundedCompletionWait::ECompleted, 
		"Lower data available notification not completed after send (status %d)", waitData2->Status());
	TESTEQ(waitData2->Status(), KErrNone, "Lower data notification completed with error %d", waitData2->Status());

	// {.4}
	ret = iUpper->CloseGracefully(KGracefulCloseTimeout);
	TESTEQ(ret, KErrChannelHalfClosed, "Graceful close of upper got %d not KErrChannelHalfClosed", ret);

	// {.5}
	TMsg rxMsg;
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving on lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg1, "Received msg != sent msg", 1);

	// {.6}
	iLower->NotifyDataAvailable(*waitData2);
	waitData2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData2->Result(), CBoundedCompletionWait::ECompleted, 
		"Lower data available not completed after graceful close (status %d)", waitData2->Status());

	// {.7}
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrChannelHalfClosed, "Receving on lower got %d not KErrChannelHalfClosed", ret);

	// {.8}
	TMsg txMsg2(6);
	ret = iLower->Send(txMsg2, 0);
	TESTEQ(ret, KErrNone, "Sending on lower [0] failed with error %d", ret);
	waitData1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData1->Result(), CBoundedCompletionWait::ECompleted, 
		"Upper data available notification not completed after lower send (status %d)", waitData1->Status());
	TESTEQ(waitData1->Status(), KErrNone, "Upper data notification completed with error %d", waitData1->Status());

	// {.9}
	ret = iUpper->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving on upper failed with error %d", ret);
	TESTEQ(rxMsg, txMsg2, "Received msg != sent msg", 1);

	// {.10}
	ret = iLower->CloseGracefully(KGracefulCloseTimeout);
	TESTEQ(ret, KErrChannelFullyClosed, "Graceful close of lower got %d not KErrChannelFullyClosed", ret);

	// {.11}
	delete iLower;
	iLower = NULL;

	// {.12}
	iUpper->NotifyDataAvailable(*waitData1);
	waitData1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData1->Result(), CBoundedCompletionWait::ECompleted, 
		"Upper data available not completed after graceful close (status %d)", waitData1->Status());

	// {.13}
	ret = iUpper->Receive(rxMsg);
	TESTEQ(ret, KErrChannelFullyClosed, "Receving on upper got %d not KErrChannelFullyClosed", ret);

	// {.14}
	delete iUpper;
	iUpper = NULL;

	CleanupStack::PopAndDestroy(waitData2);
	CleanupStack::PopAndDestroy(waitData1);
	return EPass;
	}


TVerdict CTestBDSP::GracefulCloseCoreL(TBool aFullQueue, TInt aTimeout, TBool aOOMLoop)
	{
	CBoundedCompletionWait* waitData = CBoundedCompletionWait::NewLC();

	iUpper->NotifyDataAvailable(*waitData);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ETimedOut, 
		"Upper data available notification at outset (status %d)", waitData->Status());

	TInt ret;
	if(aFullQueue)
		{
		TMsg txMsg(5);
		ret = iUpper->Send(txMsg, 0);
		TESTEQ(ret, KErrNone, "Sending on upper [0] failed with error %d", ret);
		}

	TInt oomCount = 1;
	do
		{
		if(aOOMLoop)
			{
			__UHEAP_SETFAIL(RHeap::EFailNext, oomCount++);
			}
		ret = iUpper->CloseGracefully(aTimeout);
		if(aOOMLoop)
			{
			__UHEAP_SETFAIL(RHeap::ENone, 0);
			INFO_PRINTF3(_L("With FailNext==%d CloseGracefully() returned %d"), oomCount, ret);
			}
		} while(ret == KErrNoMemory);

	TESTEQ(ret, KErrChannelHalfClosed, "Graceful close of upper got %d not KErrChannelHalfClosed", ret);

	waitData->WaitForRequestL(KCompletionPause + aTimeout);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ECompleted, 
		"No upper data available notification after CloseGracefully() timeout (status %d)", waitData->Status());
	TESTEQ(waitData->Status(), KErrCancel, 
		"Upper data available notification %d not KErrCancel", waitData->Status());

	delete iUpper;
	iLower = NULL;

	delete iLower;
	iLower = NULL;

	CleanupStack::PopAndDestroy(waitData);
	return EPass;
	}

// TEST 3.6
TVerdict CTestBDSP::NonFullQueueGracefulCloseWithTimeoutL()
	{
	// Non-full queue, long timeout, no OOM loop
	return GracefulCloseCoreL(EFalse, KGracefulCloseTimeout, EFalse);
	}

// TEST 3.7
TVerdict CTestBDSP::FullQueueGracefulCloseWithTimeoutL()
	{
	// Full queue, long timeout, no OOM loop
	return GracefulCloseCoreL(ETrue, KGracefulCloseTimeout, EFalse);
	}

// TEST 3.8
TVerdict CTestBDSP::FullQueueGracefulCloseWithZeroLengthTimeoutL()
	{
	// Full queue, zero timeout, no OOM loop
	return GracefulCloseCoreL(ETrue, 0, EFalse);
	}

// TEST 3.10
TVerdict CTestBDSP::OOMNonFullQueueGracefulCloseWithTimeoutL()
	{
	// Full queue, zero timeout, OOM loop
	return GracefulCloseCoreL(ETrue, 0, ETrue);
	}

// TEST 3.9
TVerdict CTestBDSP::SimultaneousGracefulCloseL()
	{
	CBoundedCompletionWait* waitData1 = CBoundedCompletionWait::NewLC();
	CBoundedCompletionWait* waitData2 = CBoundedCompletionWait::NewLC();

	// {.1}
	iUpper->NotifyDataAvailable(*waitData1);
	waitData1->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData1->Result(), CBoundedCompletionWait::ETimedOut, 
		"Upper data available notification at outset (status %d)", waitData1->Status());

	// {.2}
	iLower->NotifyDataAvailable(*waitData2);
	waitData2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData2->Result(), CBoundedCompletionWait::ETimedOut, 
		"Lower data available notification at outset (status %d)", waitData2->Status());

	// {.3}
	TInt ret = iUpper->CloseGracefully(KGracefulCloseTimeout);
	TESTEQ(ret, KErrChannelHalfClosed, "Graceful close of upper got %d not KErrChannelHalfClosed", ret);

	// {.4}
	ret = iLower->CloseGracefully(KGracefulCloseTimeout);
	TESTEQ(ret, KErrChannelHalfClosed, "Graceful close of lower got %d not KErrChannelHalfClosed", ret);

	waitData1->WaitForRequestL(KCompletionPause);
	waitData2->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData1->Result(), CBoundedCompletionWait::ECompleted, 
		"Upper data available not completed after graceful close (status %d)", waitData1->Status());
	TESTEQ(waitData2->Result(), CBoundedCompletionWait::ECompleted, 
		"Lower data available not completed after graceful close (status %d)", waitData2->Status());

	// {.5}
	TMsg rxMsg;
	ret = iLower->Receive(rxMsg);
	TESTEQ(ret, KErrChannelFullyClosed, "Receving on lower got %d not KErrChannelFullyClosed", ret);

	// {.6}
	delete iLower;
	iLower = NULL;

	// {.7}
	ret = iUpper->Receive(rxMsg);
	TESTEQ(ret, KErrChannelFullyClosed, "Receving on upper got %d not KErrChannelFullyClosed", ret);

	// {.8}
	delete iUpper;
	iUpper = NULL;

	CleanupStack::PopAndDestroy(waitData2);
	CleanupStack::PopAndDestroy(waitData1);
	return EPass;
	}

//

TVerdict CTestPanic::easyTestStepL()
	{
	TPtrC testCaseName;
	GetStringFromConfig(ConfigSection(), KTestTag, testCaseName);

	if(testCaseName.CompareF(_L("DupCreationUpper")) == 0)
		{
		return DuplicateCreationAsUpperL();
		}
	else if(testCaseName.CompareF(_L("DupCreationLower")) == 0)
		{
		return DuplicateCreationAsLowerL();
		}
	else if(testCaseName.CompareF(_L("BothSenderAndReceiver")) == 0)
		{
		return CreationAsSenderAndReceiverL();
		}
	else if(testCaseName.CompareF(_L("DestroyOpenSendEndpoint")) == 0)
		{
		return DestructionOfOpenSendEndpointL();
		}
	else if(testCaseName.CompareF(_L("DestroyOpenRecvEndpoint")) == 0)
		{
		return DestructionOfOpenRecvEndpointL();
		}
	else if(testCaseName.CompareF(_L("SendAboveUpperPriorityBound")) == 0)
		{
		return SendAboveUpperPriorityBoundL();
		}
	else if(testCaseName.CompareF(_L("NotifySpaceNegativePriority")) == 0)
		{
		return NotifySpaceBelowLowerPriorityBoundL();
		}
	else if(testCaseName.CompareF(_L("UDSPNestedNotifyDataAvailable")) == 0)
		{
		return UDSPNestedNotifyDataAvailableL();
		}
	else if(testCaseName.CompareF(_L("UDTPNestedNotifyDataAvailable")) == 0)
		{
		return UDTPNestedNotifyDataAvailableL();
		}
	else if(testCaseName.CompareF(_L("UDSPNestedNotifySpaceAvailable")) == 0)
		{
		return UDSPNestedNotifySpaceAvailableL();
		}
	return ETestSuiteError;
	}

// TEST 4.1
TVerdict CTestPanic::DuplicateCreationAsUpperL()
	{
	RChanUDSP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating upper failed with error %d", ret);

	// {.2}
	upper.Create(1);
	
	return EFail;	// should never get here
	}

// TEST 4.2
TVerdict CTestPanic::DuplicateCreationAsLowerL()
	{
	RChanUDSP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating upper failed with error %d", ret);

	// {.2}
	RChanUDSP::TMsgQueues msgQueues;
	upper.GetMsgQueues(msgQueues);

	RChanUDSP lower;
	ret = lower.Create(msgQueues);
	TESTEQ(ret, KErrNone, "Creating lower failed with error %d", ret);

	ret = lower.Create(msgQueues);

	return EFail;	// should never get here
	}

// TEST 4.3
TVerdict CTestPanic::CreationAsSenderAndReceiverL()
	{
	RChanUDSP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating as receiver failed with error %d", ret);

	// {.2}
	RChanUDSP::TMsgQueues msgQueues;
	upper.GetMsgQueues(msgQueues);

	// {.3}
	ret = upper.Create(msgQueues);

	return EFail;	// should never get here
	}

// TEST 4.4
TVerdict CTestPanic::DestructionOfOpenRecvEndpointL()
	{
	// {.1}
	RChanUDSP* upper = new(ELeave) RChanUDSP;
	TInt ret = upper->Create(1);
	TESTEQ(ret, KErrNone, "Creating as receiver failed with error %d", ret);

	// {.2}
	delete upper;

	return EFail;	// should never get here
	}

// TEST 4.5
TVerdict CTestPanic::DestructionOfOpenSendEndpointL()
	{
	RChanUDSP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating upper failed with error %d", ret);

	// {.2}
	RChanUDSP::TMsgQueues msgQueues;
	upper.GetMsgQueues(msgQueues);

	// {.3}
	RChanUDSP* lower = new(ELeave) RChanUDSP;
	ret = lower->Create(msgQueues);
	TESTEQ(ret, KErrNone, "Creating lower failed with error %d", ret);

	// {.4}
	delete lower;

	return EFail;	// should never get here
	}

// TEST 4.6
TVerdict CTestPanic::SendAboveUpperPriorityBoundL()
	{
	RChanUDTP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating as receiver failed with error %d", ret);

	// {.2}
	TMsg msg;
	upper.Send(msg, 3);

	return EFail;	// should never get here
	}

// TEST 4.7
TVerdict CTestPanic::NotifySpaceBelowLowerPriorityBoundL()
	{
	RChanUDSP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating upper failed with error %d", ret);

	// {.2}
	TRequestStatus status;
	upper.NotifySpaceAvailable(status, -1);

	return EFail;	// should never get here
	}

// TEST 4.8
TVerdict CTestPanic::UDTPNestedNotifyDataAvailableL()
	{
	RChanUDTP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating upper failed with error %d", ret);

	// {.2}
	RChanUDTP::TMsgQueues msgQueues;
	upper.GetMsgQueues(msgQueues);

	// {.3}
	RChanUDTP lower;
	ret = lower.Create(msgQueues);
	TESTEQ(ret, KErrNone, "Creating lower failed with error %d", ret);

	// {.4}
	TRequestStatus status1;
	lower.NotifyDataAvailable(status1);

	// {.5}
	TRequestStatus status2;
	lower.NotifyDataAvailable(status2);

	return EFail;	// should never get here
	}

TVerdict CTestPanic::UDSPNestedNotifyDataAvailableL()
	{
	RChanUDSP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating upper failed with error %d", ret);

	// {.2}
	RChanUDSP::TMsgQueues msgQueues;
	upper.GetMsgQueues(msgQueues);

	RChanUDSP lower;
	// {.3}
	ret = lower.Create(msgQueues);
	TESTEQ(ret, KErrNone, "Creating lower failed with error %d", ret);

	// {.4}
	TRequestStatus status1;
	lower.NotifyDataAvailable(status1);

	// {.5}
	TRequestStatus status2;
	lower.NotifyDataAvailable(status2);

	return EFail;	// should never get here
	}

TVerdict CTestPanic::UDSPNestedNotifySpaceAvailableL()
	{
	RChanUDSP upper;
	// {.1}
	TInt ret = upper.Create(1);
	TESTEQ(ret, KErrNone, "Creating upper failed with error %d", ret);

	// {.2}
	TMsg msg(1);
	upper.Send(msg, 0);

	// {.3}
	TRequestStatus status1;
	upper.NotifySpaceAvailable(status1, 0);

	// {.4}
	TRequestStatus status2;
	upper.NotifySpaceAvailable(status2, 0);

	return EFail;	// should never get here
	}


//
static const TInt KMBuf_MBufSize = 128;
static const TInt KMBuf_MinGrowth = 64;
static const TInt KMBuf_GrowthThreshold = 40;

TVerdict CTestRCFThread::easyTestStepL()
	{
	// {.1}
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	const TUint KMBufDefaultHeapSize	= 256 * 1024;
	RArray<TCommsBufPoolCreateInfo> poolCreateInfos;
	// Create a pool of mbufs big enough to fill the available heap space
	// Filling the heap at startup avoids having to grow the pools so the growth threshold is ignored.
	TCommsBufPoolCreateInfo createInfo;
	createInfo.iBufSize = KMBuf_MBufSize;
	createInfo.iInitialBufs = (KMBufDefaultHeapSize /(KMBuf_MBufSize+sizeof(RMBuf)))-1;
	createInfo.iGrowByBufs = KMBuf_MinGrowth;
	createInfo.iMinFreeBufs = KMBuf_GrowthThreshold;
	createInfo.iCeiling = KMBufDefaultHeapSize;
	poolCreateInfos.AppendL(createInfo);
	RCommsBufPondOp pond;
	User::LeaveIfError(pond.Open(poolCreateInfos));
	CleanupClosePushL(pond);
	poolCreateInfos.Close();
#else
	MMBufSizeAllocator* mbufSizeAllocator;
	CMBufManager* mbufMgr = CMBufManager::NewL(KMBufDefaultHeapSize, mbufSizeAllocator);
	CleanupStack::PushL(mbufMgr);
 	if (mbufMgr && mbufSizeAllocator)
		{
		// Create a pool of mbufs big enough to fill the available heap space
		// Filling the heap at startup avoids having to grow the pools so the growth threshold is ignored.
		mbufSizeAllocator->AddL(KMBuf_MBufSize, (KMBufDefaultHeapSize /(KMBuf_MBufSize+sizeof(RMBuf)))-1, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
		}
#endif // SYMBIAN_ZERO_COPY_NETWORKING	

	// {.2}
#ifdef __FLOG_ACTIVE
	CCFLogIf* cflog = CCFLogIf::NewL();
	CleanupStack::PushL(cflog);
#endif
	// {.3}
	RCFThread thread;
	TInt ret = thread.Create(_L("RCFThreadTest"), CTestRCFThread::TestEntrypoint, KDefaultStackSize, NULL, NULL);
	TESTEQ(ret, KErrNone, "Creating thread RCFThreadTest failed with error %d", ret);

	// {.4}
	TRequestStatus status;
	thread.Rendezvous(status);
	thread.Resume();
	User::WaitForRequest(status);
	TESTEQ(status, KErrNone, "Thread failed to rendezvous; error %d", ret);

	// And give thread time to exit
	User::After(1000 * 1000);

	// Cleanup
#ifdef __FLOG_ACTIVE
	CleanupStack::PopAndDestroy(cflog);
#endif
	CleanupStack::PopAndDestroy(); // pond or mbufMgr
	return EPass;
	}

TInt CTestRCFThread::TestEntrypoint(TAny*)
	{
	RThread().Rendezvous(KErrNone);
	return 0;
	}

//

TVerdict CTestTCFSubModuleAddress::easyTestStepL()
	{
	_LIT8(KMod1, "Mod1");
	_LIT8(KSub1, "Sub1");
	_LIT8(KSub2, "Sub2");
	// {.1}
	TCFSubModuleAddress mod1(KMod1(), KSub1());
	// {.2}
	TESTEQ(mod1.Module(), KMod1, "Module name != ctor value", 1);
	// {.3}
	TESTEQ(mod1.SubModule(), KSub1(), "Submodule name != ctor value", 1);
	// {.4
	TCFSubModuleAddress mod2(mod1);
	// {.5}
	TESTEQ(mod1, mod2, "Copied SubmoduleAddr != original", 1);
	// {.6}
	mod2.SetSubModule(KSub2());
	// {.7}
	TESTTRUE(mod1 != mod2, "Copied SubmoduleAddr still == original");
	// {.8}
	mod1 = mod2;
	// {.9}
	TESTEQ(mod1, mod2, "Assigned SubmoduleAddr != original", 1);

	// Check case (in)sensitivity
	// {.10}
    TCFModuleName mn(_L8("mod1"));
	mod1.SetModule(mn);
    TCFSubModuleName sn(_L8("sub2"));
	mod1.SetSubModule(sn);
	TESTTRUE(mod1 == mod2, "TCFSubModuleAddress failed - case sensitive");
	// {.11}
	TESTTRUE(mod1.Module() == mod2.Module(), "TCFSubModuleAddress::ModuleName() failed - case sensitive");
	// {.12}
	TESTTRUE(mod1.SubModule() == mod2.SubModule(), "TCFSubModuleAddress::SubModuleName() failed - case sensitive");
	// {.13}
	mn = mod1.Module();
	TESTTRUE(mn != mod2.Module(), "TCFModuleName== failed - case insensitive");
	// {.14}
	sn = mod1.SubModule();
	TESTTRUE(sn != mod2.SubModule(), "TCFSubModuleName== failed - case insensitive");

	return EPass;
	}
