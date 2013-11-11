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
// Contains declaration of test 16
// 
//

#if (!defined __TEST16MEMORYFULL_H__)
#define __TEST16MEMORYFULL_H__

#include <cflog.h>
#include <es_mbuf.h>

static const TInt KFactor = 10;
static const TInt KFactor1 = 9;
static const TInt KFactor2 = 8;
static const TInt KFactor3 = 7;

class RCommsBufPond;
class RMBufAsyncRequest;


NONSHARABLE_CLASS(CTest16MemoryFull) : public CTestStepCTMbufmgr
{
public:
	CTest16MemoryFull();
	~CTest16MemoryFull();
	virtual enum TVerdict doTestStepL( void );
	TInt iMainThreadTries;
	TInt iMainThreadFails;
	TInt iMainThreadSizeFails;
	TInt iMainThreadLastFailure;
	TInt iThread1AllocTries, iThread2AllocTries, iThread3Tries;
	TInt iThread1MemoryFull, iThread2MemoryFull, iThread3AllocFails;
	TInt iThread1SizeFails, iThread2SizeFails, iThread3Fails;
	TInt iThread1LastMemoryFull, iThread2LastMemoryFull;
	TInt iThread1MaxLen, iThread2MaxLen, iThread3MaxLen;
	TUint iThread1AllocTime, iThread1FreeTime;
	TUint iThread2AllocTime, iThread2FreeTime;
	TInt iAllocatedMemory;
	TAny* iNotifierAllocator;

	void FillDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar);
	RCommsBufPond& BufPond(); 

};	

NONSHARABLE_CLASS(CMBufAsyncMemFull) : public CActive
{
public:
	CMBufAsyncMemFull(CTest16MemoryFull* aTestObject,
				RMBufAsyncRequest& aMBufAsyncReq, 
				TDes8 *aDes1, 
				TDes8 *aDes2);

	~CMBufAsyncMemFull() { Cancel(); }

	void RunL();

	void DoCancel()  { iMBufAsyncReq.Cancel(); }

	TInt DoStartTest();

private:

static TInt fThread1(TAny*); //High priority thread in this test
static TInt fThread2(TAny*); //High priority thread in this test
static TInt fThread3(TAny*); //High priority thread in this test

	CTest16MemoryFull* iTestObject;
	TInt iRequested_size[KFactor];
	TDes8 *iDes1;
	TDes8 *iDes2;
	RThread iThread1;
	RThread iThread2;
	RThread iThread3;
	
	TInt iRequestloop;
	
	RMBufAsyncRequest& iMBufAsyncReq;
	RMBufChain iChain;
};

#endif //(__TEST16MEMORYFULL_H__)
