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
// Contains declaration of CTestStepESockSSA class that is
// base class for all test step classes in this test.
// 
//

#if !defined(SYMBIAN_NETWORKING_ESOCKTEST_TESTSTEPESOCKSSA_H_)
#define SYMBIAN_NETWORKING_ESOCKTEST_TESTSTEPESOCKSSA_H_

#include <test/testexecutestepbase.h>


// Relies upon no competing use of the thread semaphore, ie no request completions/AOs
class RMultipleWait : public RFastLock
	{
public:
	RMultipleWait(TInt aInitialCount);
	void Close();
	void Signal(TInt aDelta);
	void Wait();

	typedef RFastLock inherited;
private:
	TInt iCount;
	RThread iOwnerThread;
	};


class CTestStepESockSSA : public CTestStep
	{
public:
	enum { KBlockedRequestIssueDelay = 1000000, KBlockedRequestCompletionDelay = 1000000 };

	CTestStepESockSSA();
	~CTestStepESockSSA();
	
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	class TDataThreadControl
		{
	public:
		TDataThreadControl(RMultipleWait& aBlockSemaphore, TInt aExpectedResult)
		: iBlockSemaphore(aBlockSemaphore),
		  iExpectedResult(aExpectedResult)
			{
			}
	public:
		enum TRequestType
			{ 
			ESocketOpen, 
			EHostResolverOpen,
			EHostResolverOpenMulti,
			EServiceResolverOpen, 
			ENetDBOpen, 
			ENumProtocols, 
			EGetProtocolInfo,
			EFindProtocol
			};
		TRequestType iRequest;
		RMultipleWait& iBlockSemaphore;
		TBool iBlocked;
		TInt iResult;
		TInt iExpectedResult;
		RThread iThread;
		RSocketServ iSession;
		};

	typedef TInt (*TDataThreadEntryPt)(TDataThreadControl&);
protected:
	void SetDummyBlocking();
	void ClearDummyBlocking();

	TInt CreateDataThread(TDataThreadControl::TRequestType aRequest, TDataThreadControl& aControl);
	TVerdict WaitForDataThreadsToBlock(TDataThreadControl* aThreads, TInt aNumThreads);
	TVerdict WaitForDataThreadsToComplete(TDataThreadControl* aThreads, TInt aNumThreads);

	static TInt DataThreadEntry(TDataThreadControl& aControl);
private:
	static void DoDataThreadL(TDataThreadControl& aControl);
	void UnloadBlockerL();
	void KillC32Start();
	};
	
#endif /* SYMBIAN_NETWORKING_ESOCKTEST_TESTSTEPESOCKSSA_H_ */

