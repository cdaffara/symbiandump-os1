/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file CommsFWStep.inl
*/
#if !defined(__COMMSFWSTEP_INL__)
#define __COMMSFWSTEP_INL__

// INI data sections have to label themselves; a little ungainly but appears to be only way to manage it with TestExecute
_LIT(KTestTag, "Test");


// TEST 1.1
template <class RChanClass>
TVerdict TUniDirectionalOpenClose<RChanClass>::CreateEndpointsL(RChanClass*& aUpper, RChanClass*& aLower)
	{
	aUpper = new(ELeave) RChanClass;
	aLower = new(ELeave) RChanClass;

	// {.1}
	TESTTRUE(!aUpper->IsOpen(), "Upper endpoint reported as already open");
	
	// {.2}
	TInt ret = aUpper->Create(1);
	TESTEQ(ret, KErrNone, "Upper endpoint Create failed: err %d", ret);
	
	// {.3}
	TESTTRUE(aUpper->IsOpen(), "Upper endpoint reported as closed after successful creation");
	
	// {.4}
	TESTTRUE(!aLower->IsOpen(), "Lower endpoint reported as already open");
	
	// {.5}
	typename RChanClass::TMsgQueues msgQueues;
	aUpper->GetMsgQueues(msgQueues);
	
	// {.6}
	ret = aLower->Create(msgQueues);
	TESTEQ(ret, KErrNone, "Lower endpoint Create failed: err %d", ret);
	
	// {.7}
	TESTTRUE(aLower->IsOpen(), "Lower endpoint reported as closed after successful creation");

	return EPass;
	}

// TEST 1.5
template <class RChanClass>
TVerdict TUniDirectionalOpenClose<RChanClass>::CloseEndpointsL(RChanClass*& aUpper, RChanClass*& aLower)
	{
	CBoundedCompletionWait* wait = CBoundedCompletionWait::NewLC();
	
	// {.1}
	aLower->NotifyDataAvailable(*wait);
	wait->WaitForRequestL(KCompletionPause);
	TESTEQ(wait->Result(), CBoundedCompletionWait::ETimedOut, 
		"Data available notification after prior test step", 1);

	// {.2}
	aLower->CancelDataAvailable();
	aLower->Close();
	wait->WaitForRequestL(KCompletionPause);
	TESTEQ(wait->iStatus, KErrCancel, 
		"Data available notification not cancelled after Close()", 1);
	
	// {.3}
	TESTTRUE(!aLower->IsOpen(), "Lower endpoint reported as open after Close()");

	// {.4}
	wait->iStatus = KErrNone;
	aLower->Close();
	User::After(KCompletionPause);
	TESTEQ(wait->iStatus, KErrNone, 
		"Data available notification changed after second Close()", 1);

	// {.5}
	delete aLower;	// should not ASSERT in dtor
	aLower = NULL;

	// {.6}
	TMsg txMsg(1);
	TInt ret = aUpper->Send(txMsg, 0);
	TESTEQ(ret, KErrNone,
		"Sending on upper failed with error %d", ret);

	// {.7}
	aUpper->NotifySpaceAvailable(*wait, 0);
	wait->WaitForRequestL(KCompletionPause);
	TESTEQ(wait->iStatus, KRequestPending, 
		"Space available notification with closed lower", 1);

	// {.8}
	aUpper->Close();
	wait->WaitForRequestL(KCompletionPause);
	TESTEQ(wait->iStatus, KErrCancel,
		"Space available notification not cancelled after Close()", 1);

	// {.9}
	TESTTRUE(!aUpper->IsOpen(), "Upper endpoint reported as open after Close()");

	// {.10}
	wait->iStatus = KErrNone;
	aUpper->Close();
	User::After(KCompletionPause);
	TESTEQ(wait->iStatus, KErrNone,
		"Space available notification changed after second Close()", 1);

	// {.11}
	delete aUpper;	// should not ASSERT in dtor
	aUpper = NULL;

	CleanupStack::PopAndDestroy(wait);
	return EPass;
	}

//////////////////////////////////////////////////////////////////////


template <class RChanClass>
TVerdict CTestSP<RChanClass>::easyTestStepL()
	{
	// The INI file section specifies which test case is to be executed.
	TPtrC testCaseName;
	this->GetStringFromConfig(CCFTestCommsChannel<RChanClass>::ConfigSection(), KTestTag, testCaseName);
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
	return ETestSuiteError;
	}

// TEST 1.2
template <class RChanClass>
TVerdict CTestSP<RChanClass>::RxTxWithoutNotificationsL()
	{			 
	// {.1}
	TMsg txMsg1(1);
	TInt ret = this->iUpper->Send(txMsg1, 0);
	TESTEQ(ret, KErrNone, "Sending on upper failed with error %d", ret);

	// {.2}
	TMsg txMsg2(2);
	ret = this->iUpper->Send(txMsg2, 0);
	TESTEQ(ret, KErrOverflow, "Sending on upper returned error %d, not KErrOverflow", ret);

	// {.3}
	TMsg rxMsg;
	ret = this->iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg1, "Received msg != first msg sent", 1);

	// {.4}
	ret = this->iLower->Receive(rxMsg);
	TESTEQ(ret, KErrUnderflow, "Receving from lower returned error %d, not KErrUnderflow", ret);

	return EPass;
	}

// TEST 1.3
template <class RChanClass>
TVerdict CTestSP<RChanClass>::RxTxWithNotificationsL()
	{
	CBoundedCompletionWait* waitData = CBoundedCompletionWait::NewLC();
	CBoundedCompletionWait* waitSpace = CBoundedCompletionWait::NewLC();

	// {.1}
	this->iLower->NotifyDataAvailable(*waitData);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ETimedOut, 
		"Data available notification before send (status %d)", waitData->Status());

	// {.2}
	this->iUpper->NotifySpaceAvailable(*waitSpace, 0);
	waitSpace->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace->Result(), CBoundedCompletionWait::ECompleted, 
		"No space available notification before send (status %d)", waitSpace->Status());
	
	// {.3}
	TMsg txMsg(3);
	TInt ret = this->iUpper->Send(txMsg, 0);
	TESTEQ(ret, KErrNone, "Sending on upper failed with error %d", ret);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ECompleted, 
		"Data available notification not completed after send (status %d)", waitData->Status());
	
	// {.4}
	this->iUpper->NotifySpaceAvailable(*waitSpace, 0);
	waitSpace->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace->Result(), CBoundedCompletionWait::ETimedOut,
		"Space available notification after send (status %d)", waitSpace->Status());
	
	// {.5}
	TMsg rxMsg;
	ret = this->iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg, "Received msg != msg sent", 1);
	waitSpace->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace->Result(), CBoundedCompletionWait::ECompleted,
		"Space available notification not completed after receive (status %d)", waitSpace->Status());

	CleanupStack::PopAndDestroy(waitSpace);
	CleanupStack::PopAndDestroy(waitData);
	return EPass;
	}

// TEST 1.4
template <class RChanClass>
TVerdict CTestSP<RChanClass>::RxTxCancelNotificationsL()
	{
	CBoundedCompletionWait* waitData = CBoundedCompletionWait::NewLC();
	CBoundedCompletionWait* waitSpace = CBoundedCompletionWait::NewLC();

	// {.1}
	this->iLower->NotifyDataAvailable(*waitData);
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ETimedOut, 
		"Data available notification before send (status %d)", waitData->Status());

	// {.2}
	this->iLower->CancelDataAvailable();
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ECompleted, 
		"No data available notification after cancel (status %d)", waitData->Status());
	TESTEQ(waitData->Status(), KErrCancel, 
		"Data available status not KErrCancel (status %d)", waitData->Status());

	// {.3}
	waitData->SetStatus(KRequestPending);
	this->iLower->CancelDataAvailable();
	waitData->WaitForRequestL(KCompletionPause);
	TESTEQ(waitData->Result(), CBoundedCompletionWait::ETimedOut, 
		"Data available notification cancelled twice (status %d)", waitData->Status());

	// {.4}
	TMsg txMsg(4);
	TInt ret = this->iUpper->Send(txMsg, 0);
	TESTEQ(ret, KErrNone, "Sending on upper failed with error %d", ret);

	// {.5}
	this->iUpper->NotifySpaceAvailable(*waitSpace, 0);
	waitSpace->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace->Result(), CBoundedCompletionWait::ETimedOut,
		"Space available notification after send (status %d)", waitSpace->Status());

	// {.6}
	this->iUpper->CancelSpaceAvailable(0);
	waitSpace->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace->Result(), CBoundedCompletionWait::ECompleted, 
		"No space available notification after cancel (status %d)", waitSpace->Status());
	TESTEQ(waitSpace->Status(), KErrCancel, 
		"Space available status not KErrCancel (status %d)", waitSpace->Status());

	// {.7}
	waitSpace->SetStatus(KRequestPending);
	this->iLower->CancelSpaceAvailable(0);
	waitSpace->WaitForRequestL(KCompletionPause);
	TESTEQ(waitSpace->Result(), CBoundedCompletionWait::ETimedOut, 
		"Space available notification cancelled twice (status %d)", waitSpace->Status());

	// {.8}
	TMsg rxMsg;
	ret = this->iLower->Receive(rxMsg);
	TESTEQ(ret, KErrNone, "Receving from lower failed with error %d", ret);
	TESTEQ(rxMsg, txMsg, "Received msg != msg sent", 1);

	CleanupStack::PopAndDestroy(waitSpace);
	CleanupStack::PopAndDestroy(waitData);
	return EPass;
	}



#endif	// __COMMSFWSTEP_INL__

