/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef __TEST20POOLCEILINGFULL_H__
#define __TEST20POOLCEILINGFULL_H__

#include <cflog.h>
#include <es_mbuf.h>

NONSHARABLE_CLASS(CTest20PoolCeilingFull) : public CTestStepCTMbufmgr
	{
	public:
	CTest20PoolCeilingFull();
	~CTest20PoolCeilingFull();
	virtual enum TVerdict doTestStepL( void );
	RCommsBufPond& BufPond(); 

	RMBufChain iChain;
	};

NONSHARABLE_CLASS(CMBufAsyncPoolCeilingFull) : public CActive
	{
	public:
	CMBufAsyncPoolCeilingFull(CTest20PoolCeilingFull& aTestObject);

	~CMBufAsyncPoolCeilingFull();

	void RunL();

	void DoCancel();

	TInt DoStartTest();
	
	private:
	void CompleteSelf();
	static TInt fThread1(TAny*); //High priority thread in this test
	static TInt fThread2(TAny*); //High priority thread in this test
	static TInt fThread3(TAny*); //High priority thread in this test

	CTest20PoolCeilingFull& iTestObject;

	RThread iThread1;
	RThread iThread2;
	RThread iThread3;
	TInt iIteration;
	TInt iError;
	
	RMBufAsyncRequest 	iAsyncRequest;
	RMBufChain			iAsyncChain;
	};

#endif // __TEST20POOLCEILINGFULL_H__
