// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "QueueTests.h"
#include "NetConLog.h"
#include "CStubbedAgent.h"

//
//  Test Case NC-1-1
//

CNetConTest0101* CNetConTest0101::NewLC()
	{
	CNetConTest0101* self = new(ELeave) CNetConTest0101();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

const TDesC& CNetConTest0101::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-1");
	return KTestName();
	}

void CNetConTest0101::StartTestL()
	{
	
	iNetCon->SelectAgent(this, NULL, EConnStartExplicit, 100, KErrNone);
	}

void CNetConTest0101::SelectComplete(const TDesC& aName)
	{

	_LIT(KAgentName, "An Agent Name");

	if(aName == KAgentName())
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0101::SelectComplete(TInt)
	{

	CompleteTest(MNetConTest::EFailed);
	}


//
//  Test Case NC-1-2
//

CNetConTest0102* CNetConTest0102::NewLC()
	{
	CNetConTest0102* self = new(ELeave) CNetConTest0102();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

const TDesC& CNetConTest0102::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-2");
	return KTestName();
	}

void CNetConTest0102::StartTestL()
	{
	
	iNetCon->SelectAgent(this, NULL, EConnStartExplicit, 200, KErrNone);
	}

void CNetConTest0102::SelectComplete(const TDesC&)
	{

	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0102::SelectComplete(TInt aError)
	{

	if(aError == KErrBadPower)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}


//
//  Test Case NC-1-3
//

CNetConTest0103* CNetConTest0103::NewLC()
	{
	CNetConTest0103* self = new(ELeave) CNetConTest0103();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

const TDesC& CNetConTest0103::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-3");
	return KTestName();
	}

void CNetConTest0103::StartTestL()
	{
	
	iNetCon->Reconnect(this, REINTERPRET_CAST(CNifAgentBase*, 300));
	}

void CNetConTest0103::ReconnectComplete(TInt aError)
	{

	if(aError == KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}


//
//  Test Case NC-1-4
//

CNetConTest0104* CNetConTest0104::NewLC()
	{
	CNetConTest0104* self = new(ELeave) CNetConTest0104();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

const TDesC& CNetConTest0104::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-4");
	return KTestName();
	}

void CNetConTest0104::StartTestL()
	{
	
	iNetCon->Reconnect(this, REINTERPRET_CAST(CNifAgentBase*, 400));
	}

void CNetConTest0104::ReconnectComplete(TInt aError)
	{

	if(aError == KErrDiskFull)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}


//
//  Test Case NC-1-5
//

CNetConTest0105* CNetConTest0105::NewLC()
	{
	CNetConTest0105* self = new(ELeave) CNetConTest0105();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

const TDesC& CNetConTest0105::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-5");
	return KTestName();
	}

void CNetConTest0105::StartTestL()
	{
	
	iNetCon->SelectAgent(this, NULL, EConnStartExplicit, 500, KErrNone);
	TInt err = iNetCon->CancelRequest(this);

	if(err==KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0105::SelectComplete(const TDesC&)
	{

	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0105::SelectComplete(TInt)
	{

	CompleteTest(MNetConTest::EFailed);
	}


//
//  Test Case NC-1-6
//

CNetConTest0106* CNetConTest0106::NewLC()
	{
	CNetConTest0106* self = new(ELeave) CNetConTest0106();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0106::CNetConTest0106()
	{
	}

void CNetConTest0106::ConstructL()
	{

	CQueueTestBase::ConstructL();

	TCallBack callback(CancelCb, this);
	iCancelCb = new(ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);
	}

CNetConTest0106::~CNetConTest0106()
	{

	if(iCancelCb)
		delete iCancelCb;
	}

const TDesC& CNetConTest0106::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-6");
	return KTestName();
	}

void CNetConTest0106::StartTestL()
	{
	
	iNetCon->SelectAgent(this, NULL, EConnStartExplicit, 600, KErrNone);
	iCancelCb->CallBack();
	}

TInt CNetConTest0106::CancelCb(TAny* aThisPtr)
	{

	CNetConTest0106* self = (CNetConTest0106*)aThisPtr;
	self->CancelCbComplete();
	return KErrNone;
	}

void CNetConTest0106::CancelCbComplete()
	{

	TInt err = iNetCon->CancelRequest(this);

	if(err==KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0106::SelectComplete(const TDesC&)
	{

	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0106::SelectComplete(TInt)
	{

	CompleteTest(MNetConTest::EFailed);
	}


//
//  Test Case NC-1-7
//

CNetConTest0107* CNetConTest0107::NewLC()
	{
	CNetConTest0107* self = new(ELeave) CNetConTest0107();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

const TDesC& CNetConTest0107::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-7");
	return KTestName();
	}

void CNetConTest0107::StartTestL()
	{
	
	iNetCon->Reconnect(this, REINTERPRET_CAST(CNifAgentBase*, 700));
	TInt err = iNetCon->CancelRequest(this);

	if(err==KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0107::ReconnectComplete(TInt)
	{

	CompleteTest(MNetConTest::EFailed);
	}


//
//  Test Case NC-1-8
//

CNetConTest0108* CNetConTest0108::NewLC()
	{
	CNetConTest0108* self = new(ELeave) CNetConTest0108();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0108::CNetConTest0108()
	{
	}

void CNetConTest0108::ConstructL()
	{

	CQueueTestBase::ConstructL();

	TCallBack callback(CancelCb, this);
	iCancelCb = new(ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);
	}

CNetConTest0108::~CNetConTest0108()
	{

	if(iCancelCb)
		delete iCancelCb;
	}

const TDesC& CNetConTest0108::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-8");
	return KTestName();
	}

void CNetConTest0108::StartTestL()
	{
	
	iNetCon->Reconnect(this, REINTERPRET_CAST(CNifAgentBase*, 800));
	iCancelCb->CallBack();
	}

TInt CNetConTest0108::CancelCb(TAny* aThisPtr)
	{

	CNetConTest0108* self = (CNetConTest0108*)aThisPtr;
	self->CancelCbComplete();
	return KErrNone;
	}

void CNetConTest0108::CancelCbComplete()
	{

	TInt err = iNetCon->CancelRequest(this);

	if(err==KErrNone)
		CompleteTest(MNetConTest::EPassed);
	else
		CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0108::ReconnectComplete(TInt)
	{

	CompleteTest(MNetConTest::EFailed);
	}


//
//  Test Case NC-1-9
//

CNetConTest0109* CNetConTest0109::NewLC()
	{
	CNetConTest0109* self = new(ELeave) CNetConTest0109();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0109::CNetConTest0109()
	{ }

void CNetConTest0109::ConstructL()
	{

	CQueueTestBase::ConstructL();

	for(TInt i=0; i< KNumRequests; ++i)
		{
		CTestControllerObserver* test = new(ELeave) CTestControllerObserver(this);
		iObservers.Append(test);
		}
	}

CNetConTest0109::~CNetConTest0109()
	{

	iObservers.ResetAndDestroy();
	}

const TDesC& CNetConTest0109::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-9");
	return KTestName();
	}

void CNetConTest0109::StartTestL()
	{
	
	iNetCon->Reconnect(iObservers[iNumRequestsOutstanding], REINTERPRET_CAST(CNifAgentBase*, 900));
	iExpectedRequest[iNumRequestsOutstanding++] = EReconnect;

	iNetCon->SelectAgent(iObservers[iNumRequestsOutstanding], NULL, EConnStartExplicit, 900, KErrNone);
	iExpectedRequest[iNumRequestsOutstanding++] = ESelection;

	iNetCon->Reconnect(iObservers[iNumRequestsOutstanding], REINTERPRET_CAST(CNifAgentBase*, 900));
	iExpectedRequest[iNumRequestsOutstanding++] = EReconnect;

	iNetCon->SelectAgent(iObservers[iNumRequestsOutstanding], NULL, EConnStartExplicit, 900, KErrNone);
	iExpectedRequest[iNumRequestsOutstanding++] = ESelection;

	iNetCon->SelectAgent(iObservers[iNumRequestsOutstanding], NULL, EConnStartExplicit, 900, KErrNone);
	iExpectedRequest[iNumRequestsOutstanding++] = ESelection;
	
	ASSERT(iNumRequestsOutstanding == KNumRequests);
	}


void CNetConTest0109::SelectComplete(const TDesC& aName)
	{

	_LIT(KAgentName, "An Agent Name");

	if(iExpectedRequest[KNumRequests - iNumRequestsOutstanding] != ESelection || aName != KAgentName())
		{
		CompleteTest(MNetConTest::EFailed);
		}
	else if (iNumRequestsOutstanding == 1)
		{
		CompleteTest(MNetConTest::EPassed);
		}

	--iNumRequestsOutstanding;
	}

void CNetConTest0109::SelectComplete(TInt)
	{

	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0109::ReconnectComplete(TInt aError)
	{

	if(iExpectedRequest[KNumRequests - iNumRequestsOutstanding] != EReconnect || aError != KErrNone)
		{
		CompleteTest(MNetConTest::EFailed);
		}
	else if (iNumRequestsOutstanding == 1)
		{
		CompleteTest(MNetConTest::EPassed);
		}

	--iNumRequestsOutstanding;
	}


//
//  Test Case NC-1-10
//

CTestControllerObserver::CTestControllerObserver(CQueueTestBase* aTestCase)
: iTestCase(aTestCase)
	{}

CTestControllerObserver::~CTestControllerObserver()
	{}

void CTestControllerObserver::SelectComplete(const TDesC& aName)
	{
	iTestCase->SelectComplete(aName);
	}

void CTestControllerObserver::SelectComplete(TInt aError)
	{
	iTestCase->SelectComplete(aError);
	}

void CTestControllerObserver::ReconnectComplete(TInt aError)
	{
	iTestCase->ReconnectComplete(aError);
	}

CNetConTest0110* CNetConTest0110::NewLC()
	{
	CNetConTest0110* self = new(ELeave) CNetConTest0110();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0110::CNetConTest0110()
	{ }

void CNetConTest0110::ConstructL()
	{

	CQueueTestBase::ConstructL();

	for(TInt i=0; i< KNumRequests+3; ++i)
		{
		CTestControllerObserver* test = new(ELeave) CTestControllerObserver(this);
		iObservers.Append(test);
		}
	}

CNetConTest0110::~CNetConTest0110()
	{

	iObservers.ResetAndDestroy();
	}

const TDesC& CNetConTest0110::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-10");
	return KTestName();
	}

void CNetConTest0110::StartTestL()
	{
	
	TInt observerNum(0);

	// request 1
	iNetCon->Reconnect(iObservers[observerNum++], REINTERPRET_CAST(CNifAgentBase*, 1000));
	iExpectedRequest[iNumRequestsOutstanding++] = EReconnect;

	// request 2
	iNetCon->SelectAgent(iObservers[observerNum++], NULL, EConnStartExplicit, 1000, KErrNone);

	// request 3
	iNetCon->Reconnect(iObservers[observerNum++], REINTERPRET_CAST(CNifAgentBase*, 1000));
	iExpectedRequest[iNumRequestsOutstanding++] = EReconnect;

	// request 4
	iNetCon->SelectAgent(iObservers[observerNum++], NULL, EConnStartExplicit, 1000, KErrNone);
	iExpectedRequest[iNumRequestsOutstanding++] = ESelection;

	// request 5
	iNetCon->Reconnect(iObservers[observerNum++], REINTERPRET_CAST(CNifAgentBase*, 1000));

	// request 6
	iNetCon->SelectAgent(iObservers[observerNum++], NULL, EConnStartExplicit, 1000, KErrNone);

	// request 7
	iNetCon->Reconnect(iObservers[observerNum++], REINTERPRET_CAST(CNifAgentBase*, 1000));
	iExpectedRequest[iNumRequestsOutstanding++] = EReconnect;

	// request 8
	iNetCon->SelectAgent(iObservers[observerNum++], NULL, EConnStartExplicit, 1000, KErrNone);
	iExpectedRequest[iNumRequestsOutstanding++] = ESelection;
	
	// Cancel requests 2, 5 and 6
	iNetCon->CancelRequest(iObservers[1]);
	iNetCon->CancelRequest(iObservers[4]);
	iNetCon->CancelRequest(iObservers[5]);

	ASSERT(iNumRequestsOutstanding == KNumRequests);
	ASSERT(observerNum == KNumRequests+3);
	}

void CNetConTest0110::SelectComplete(const TDesC& aName)
	{

	_LIT(KAgentName, "An Agent Name");

	if(iExpectedRequest[KNumRequests - iNumRequestsOutstanding] != ESelection || aName != KAgentName())
		{
		CompleteTest(MNetConTest::EFailed);
		}
	else if (iNumRequestsOutstanding == 1)
		{
		CompleteTest(MNetConTest::EPassed);
		}

	--iNumRequestsOutstanding;
	}

void CNetConTest0110::SelectComplete(TInt)
	{

	CompleteTest(MNetConTest::EFailed);
	}

void CNetConTest0110::ReconnectComplete(TInt aError)
	{

	if(iExpectedRequest[KNumRequests - iNumRequestsOutstanding] != EReconnect || aError != KErrNone)
		{
		CompleteTest(MNetConTest::EFailed);
		}
	else if (iNumRequestsOutstanding == 1)
		{
		CompleteTest(MNetConTest::EPassed);
		}

	--iNumRequestsOutstanding;
	}


//
//  Test Case NC-1-11
//

CNetConTest0111* CNetConTest0111::NewLC()
	{
	CNetConTest0111* self = new(ELeave) CNetConTest0111();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0111::CNetConTest0111()
	{ }

void CNetConTest0111::ConstructL()
	{

	CQueueTestBase::ConstructL();
	iAgent = new(ELeave) CStubbedAgent();
	}

CNetConTest0111::~CNetConTest0111()
	{

	if (iAgent)
		delete iAgent;
	}

const TDesC& CNetConTest0111::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-11");
	return KTestName();
	}

void CNetConTest0111::StartTestL()
	{
	TBuf8<40> agentName;
	iNetCon->AddAgentToNetworkL(iAgent, 11);
	TInt err(KErrNone);
	err = iAgent->Control(0, 0, agentName);
	if(err != KErrNotSupported)
		User::Leave(err);
	CompleteTest(MNetConTest::EPassed);
	}


//
//  Test Case NC-1-12
//

CNetConTest0112* CNetConTest0112::NewLC()
	{
	CNetConTest0112* self = new(ELeave) CNetConTest0112();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0112::CNetConTest0112()
	{ }

void CNetConTest0112::ConstructL()
	{

	CQueueTestBase::ConstructL();
	iAgent = new(ELeave) CStubbedAgent();
	iAnotherAgent = new(ELeave) CStubbedAgent();
	}

CNetConTest0112::~CNetConTest0112()
	{

	if (iAnotherAgent)
		delete iAnotherAgent;

	if (iAgent)
		delete iAgent;
	}

const TDesC& CNetConTest0112::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-12");
	return KTestName();
	}

void CNetConTest0112::StartTestL()
	{
	
	iNetCon->AddAgentToNetworkL(iAgent, 12);
	iNetCon->AddAgentToNetworkL(iAnotherAgent, 12);
	CompleteTest(MNetConTest::EPassed);
	}


//
//  Test Case NC-1-13
//

CNetConTest0113* CNetConTest0113::NewLC()
	{
	CNetConTest0113* self = new(ELeave) CNetConTest0113();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CNetConTest0113::CNetConTest0113()
	{ }

void CNetConTest0113::ConstructL()
	{

	CQueueTestBase::ConstructL();
	iAgent = new(ELeave) CStubbedAgent();
	}

CNetConTest0113::~CNetConTest0113()
	{

	if (iAgent)
		delete iAgent;
	}

const TDesC& CNetConTest0113::Name() const
	{

	_LIT(KTestName, "Test Case NC-1-13");
	return KTestName();
	}

void CNetConTest0113::StartTestL()
	{
	
	iNetCon->AddAgentToNetworkL(iAgent, 13);
	iNetCon->AddAgentToNetworkL(iAgent, 13);
	CompleteTest(MNetConTest::EPassed);
	}

