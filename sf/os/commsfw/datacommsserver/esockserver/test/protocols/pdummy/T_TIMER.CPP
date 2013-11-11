// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <es_prot.h>
#include <e32test.h>

RTest test(_L("Esock delta timer tests"));


TInt theResults[10];
TCallBack* theCallBacks;
TTimerHandle theTimerHandles[10];
CDeltaTimer* theTimer;

class CMyActiveScheduler : public CActiveScheduler
	{
public:
	void Error(TInt anError)const;
	static CMyActiveScheduler* NewL();
	};

void CMyActiveScheduler::Error(TInt /*anError*/) const
//
//
//
	{
	User::Panic(_L("MYActiveScheduler"),0);
	}

CMyActiveScheduler* CMyActiveScheduler::NewL()
	{
	return new(ELeave) CMyActiveScheduler;
	}

struct TTimerCallData
	{
	CDeltaTimer* iTimer;
	TTimerHandle *iTimers;
	TInt iIndex;
	};

TInt  print(TAny* anArg)
//
// Null entry. Just prints and increments it's counter.
//
	{
	TInt index=(TInt)anArg;
	theResults[index]++;
	test.Printf(_L("Callback %d\r\n"),index);
	return 0;
	}

TInt cancelNext(TAny * anArg)
//
// Cancels the next entry - assumes it is valid.
//
	{
	TInt index=(TInt)anArg;
	theTimer->Remove(theTimerHandles[index+1]);
	return print(anArg);
	}

TInt requeue(TAny * anArg)
//
// Note that a requeue will run once every 200 ms until stop is called.
//
	{
	TInt index=(TInt)anArg;
	theTimerHandles[index]=theTimer->Queue(200000,theCallBacks[index]);
	return print(anArg);
	}

TInt requeuePrevious(TAny * anArg)
//
// Requeue the previous entry (assumes previous entry is valid handle)
//
	{
	TInt index=(TInt)anArg;
	theTimerHandles[index-1]=theTimer->Queue(200000,theCallBacks[index-1]);
	return print(anArg);
	}

TInt cancel2ndFollowing(TAny * anArg)
//
// Assumes that 2nd following timer handle is valid.
//
	{
	TInt index=(TInt)anArg;
	theTimer->Remove(theTimerHandles[index+2]);
	return print(anArg);
	}

TInt stop(TAny * anArg)
//
// Stops the active schduler
//
	{
	TInt index=(TInt)anArg;
	theResults[index]++;
	test.Printf(_L("Callback %d, stopping\r\n"),index);
	CMyActiveScheduler::Stop();

	return 0;
	}

TInt silentStop(TAny* /*anArg*/)
//
// Stops the active schduler but doesn't make a big fuss about it
//
	{
	CMyActiveScheduler::Stop();
	return 0;
	}

void testTimerPeriods()
//
//
//
	{

	test.Start(_L("Testing timer periods"));
	RTimer tim;
	tim.CreateLocal();
	TRequestStatus stat;
	test.Next(_L("10 second RTimer"));
	test.Printf(_L("Press a key"));
	test.Getch();

	tim.AfterInMicroSeconds(stat,10000000);
	User::WaitForRequest(stat);
	test.Printf(_L("Complete"));

	test.End();
	}

TInt Counter=0;

TInt printTime(TAny* aCount)
	{
	test.Printf(_L("timer %d\n\r"),(*(TInt *)aCount)++);
	return 0;
	}

const TInt KTimerFactorFifths=500000;
const TInt KTimerFactorTenths=100000;
const TInt KTimerFactorSeconds=1000000;

TInt E32Main()
//
//
//
	{
	CMyActiveScheduler* s=NULL;
	TRAPD(ret,s=CMyActiveScheduler::NewL())
	test(ret==KErrNone);

	CActiveScheduler::Install(s);
	test.Title();
	test.Start(_L("Timer"));
	
	TRAP(ret,theTimer=CDeltaTimer::NewL());
	test(ret==KErrNone);

	Mem::FillZ(theResults,10*sizeof(TInt));

/*	theCallBacks[0].TCallBack(print,0);
	theCallBacks[1].TCallBack(cancelNext,1);
	theCallBacks[2].TCallBack(print,2);		// Gets cancelled
	theCallBacks[3].TCallBack(requeue,3);		// Runs 3 times
	theCallBacks[4].TCallBack(requeuePrevious,4);
	theCallBacks[5].TCallBack(cancel2ndFollowing,5);
	theCallBacks[6].TCallBack(print,6);
	theCallBacks[7].TCallBack(cancelNext,7);	// Gets cancelled
	theCallBacks[8].TCallBack(requeue,8);		// Runs twice
	theCallBacks[9].TCallBack(stop,9);
*/
	TCallBack callBacks[10]=
		{
		/* 0 */ TCallBack(print,(TAny*)0),
		/* 1 */ TCallBack(cancelNext,(TAny*)1),
		/* 2 */ TCallBack(print,(TAny*)2),	// Gets cancelled
		/* 3 */ TCallBack(print,(TAny*)3),		// Runs twice
		/* 4 */ TCallBack(requeuePrevious,(TAny*)4),
		/* 5 */ TCallBack(cancel2ndFollowing,(TAny*)5),
		/* 6 */ TCallBack(print,(TAny*)6),
		/* 7 */ TCallBack(cancelNext,(TAny*)7),	// Gets cancelled
		/* 8 */ TCallBack(requeue,(TAny*)8),	// Runs twice, once on the same RunL as the stop
		/* 9 */ TCallBack(stop,(TAny*)9),
		};

	theCallBacks=callBacks;

	theTimerHandles[0]=theTimer->Queue(200000,theCallBacks[0]);
	theTimerHandles[1]=theTimer->Queue(400000,theCallBacks[1]);
	theTimerHandles[2]=theTimer->Queue(600000,theCallBacks[2]);
	theTimerHandles[3]=theTimer->Queue(800000,theCallBacks[3]);
	theTimerHandles[4]=theTimer->Queue(1000000,theCallBacks[4]);
	theTimerHandles[5]=theTimer->Queue(1200000,theCallBacks[5]);
	theTimerHandles[6]=theTimer->Queue(1400000,theCallBacks[6]);
	theTimerHandles[7]=theTimer->Queue(1600000,theCallBacks[7]);
	theTimerHandles[8]=theTimer->Queue(1800000,theCallBacks[8]);
	theTimerHandles[9]=theTimer->Queue(2000000,theCallBacks[9]);

	CActiveScheduler::Start();

	test(theResults[0]==1);
	test(theResults[1]==1);
	test(theResults[2]==0);
	test(theResults[3]==2);
	test(theResults[4]==1);
	test(theResults[5]==1);
	test(theResults[6]==1);
	test(theResults[7]==0);
	test(theResults[8]==2);
	test(theResults[9]==1);

	theTimer->Remove(theTimerHandles[8]);
	// testTimerPeriods();

	TCallBack printer(printTime,(TAny*)&Counter);
	TInt i;
	test.Next(_L("5th of a second timers"));
	for (i=0;i<20;i++)
		theTimer->Queue(i*KTimerFactorFifths,printer);

	TCallBack end(silentStop,(TAny*)11);
	theTimer->Queue((i+1)*KTimerFactorFifths,end);

	Counter=0;
	CActiveScheduler::Start();

	test.Next(_L("10th of a second timers"));
	for (i=0;i<100;i++)
		theTimer->Queue(i*KTimerFactorTenths,printer);

	theTimer->Queue((i+1)*KTimerFactorTenths,end);

	Counter=0;
	CActiveScheduler::Start();

	test.Next(_L("second timers"));
	for (i=0;i<10;i++)
		theTimer->Queue(i*KTimerFactorSeconds,printer);

	theTimer->Queue((i+1)*KTimerFactorSeconds,end);

	Counter=0;
	CActiveScheduler::Start();

	test.End();
		
	return 0;
	}
