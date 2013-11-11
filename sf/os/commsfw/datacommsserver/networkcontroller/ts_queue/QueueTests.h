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

#if !(defined __QUEUETESTS_H__)
#define __QUEUETESTS_H__

#include <e32base.h>
#include "TestBases.h"

// forward declaration
class CNifAgentBase;

//
//  Test Case NC-1-1
//
class CNetConTest0101 : public CQueueTestBase
	{
public:
	static CNetConTest0101* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);

protected:
	void StartTestL();
	};


//
//  Test Case NC-1-2
//
class CNetConTest0102 : public CQueueTestBase
	{
public:
	static CNetConTest0102* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);

protected:
	void StartTestL();
	};


//
//  Test Case NC-1-3
//
class CNetConTest0103 : public CQueueTestBase
	{
public:
	static CNetConTest0103* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void ReconnectComplete(TInt aError);

protected:
	void StartTestL();
	};


//
//  Test Case NC-1-4
//
class CNetConTest0104 : public CQueueTestBase
	{
public:
	static CNetConTest0104* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void ReconnectComplete(TInt aError);

protected:
	void StartTestL();
	};


//
//  Test Case NC-1-5
//
class CNetConTest0105 : public CQueueTestBase
	{
public:
	static CNetConTest0105* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);

protected:
	void StartTestL();
	};


//
//  Test Case NC-1-6
//
class CNetConTest0106 : public CQueueTestBase
	{
public:
	virtual ~CNetConTest0106();
	static CNetConTest0106* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);

protected:
	CNetConTest0106();
	void ConstructL();
	
	void StartTestL();

private:
	static TInt CancelCb(TAny* aThisPtr);
	void CancelCbComplete();

private:
	CAsyncCallBack* iCancelCb;
	};


//
//  Test Case NC-1-7
//
class CNetConTest0107 : public CQueueTestBase
	{
public:
	static CNetConTest0107* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementation from CQueueTestBase
	virtual void ReconnectComplete(TInt aError);

protected:
	void StartTestL();
	};


//
//  Test Case NC-1-8
//
class CNetConTest0108 : public CQueueTestBase
	{
public:
	virtual ~CNetConTest0108();
	static CNetConTest0108* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementation from CQueueTestBase
	virtual void ReconnectComplete(TInt aError);

protected:
	CNetConTest0108();
	void ConstructL();

	void StartTestL();

private:
	static TInt CancelCb(TAny* aThisPtr);
	void CancelCbComplete();

private:
	CAsyncCallBack* iCancelCb;
	};


//
//  Test Case NC-1-9
//

class CTestControllerObserver : public CBase, public MNetworkControllerObserver
//
//  Utility class used by NC-1-9 and NC-1-10
//
	{
public:
	CTestControllerObserver(CQueueTestBase* aTestCase);
	~CTestControllerObserver();
	
	// from MNetworkControllerObserver
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);
	virtual void ReconnectComplete(TInt aError);

private:
	CQueueTestBase* iTestCase;
	};

const TInt KNumRequests = 5;

class CNetConTest0109 : public CQueueTestBase
	{
public:
	virtual ~CNetConTest0109();
	static CNetConTest0109* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);
	virtual void ReconnectComplete(TInt aError);

protected:
	void StartTestL();

private:
	CNetConTest0109();
	void ConstructL();

private:
	enum TRequestType
		{
		ESelection,
		EReconnect
		};

	RPointerArray<CTestControllerObserver> iObservers;
	TFixedArray<TInt, KNumRequests> iExpectedRequest;
	TInt iNumRequestsOutstanding;
	};


//
//  Test Case NC-1-10
//
class CNetConTest0110 : public CQueueTestBase
	{
public:
	virtual ~CNetConTest0110();
	static CNetConTest0110* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);
	virtual void ReconnectComplete(TInt aError);

protected:
	void StartTestL();

private:
	CNetConTest0110();
	void ConstructL();

private:
	enum TRequestType
		{
		ESelection,
		EReconnect
		};

	RPointerArray<CTestControllerObserver> iObservers;
	TFixedArray<TInt, KNumRequests> iExpectedRequest;
	TInt iNumRequestsOutstanding;
	};

//
//  Test Case NC-1-11
//
class CNetConTest0111 : public CQueueTestBase
	{
public:
	virtual ~CNetConTest0111();
	static CNetConTest0111* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

protected:
	void StartTestL();

private:
	CNetConTest0111();
	void ConstructL();

private:
	CNifAgentBase* iAgent;
	};


//
//  Test Case NC-1-12
//
class CNetConTest0112 : public CQueueTestBase
	{
public:
	virtual ~CNetConTest0112();
	static CNetConTest0112* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

protected:
	void StartTestL();

private:
	CNetConTest0112();
	void ConstructL();

private:
	CNifAgentBase* iAgent;
	CNifAgentBase* iAnotherAgent;
	};


//
//  Test Case NC-1-13
//
class CNetConTest0113 : public CQueueTestBase
	{
public:
	virtual ~CNetConTest0113();
	static CNetConTest0113* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

protected:
	void StartTestL();

private:
	CNetConTest0113();
	void ConstructL();

private:
	CNifAgentBase* iAgent;
	};


//
//  Test Case NC-1-14
//
class CNetConTest0114 : public CQueueTestBase
	{
public:
	static CNetConTest0114* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);

protected:
	void StartTestL();

protected:
	TCommDbMultiConnPref iMultiPref;
	TInt iAnswerNo;
	};


//
//  Test Case NC-1-15
//
class CNetConTest0115 : public CQueueTestBase
	{
public:
	static CNetConTest0115* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);

protected:
	void StartTestL();

protected:
	TCommDbMultiConnPref iMultiPref;
	TInt iAnswerNo;
	};


//
//  Test Case NC-1-16
//
class CNetConTest0116 : public CQueueTestBase
	{
public:
	static CNetConTest0116* NewLC();

	// implement pure virtual from MNetConTest
	const TDesC& Name() const;

	// override implementations from CQueueTestBase
	virtual void SelectComplete(const TDesC& aName);
	virtual void SelectComplete(TInt aError);

protected:
	void StartTestL();

protected:
	TCommDbMultiConnPref iMultiPref;
	TInt iAnswerNo;
	};


#endif // __QUEUETESTS_H__

