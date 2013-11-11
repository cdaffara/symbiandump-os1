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
//
// lbsPropPipeTest.cpp
//

#include <e32base.h>
#include <e32std.h>
#include <e32property.h>

#include "lbs_ps.h"
#include "PropPipe.h"

TUint32 KPipeKey = 0x07897651;
TUint32 KFlowKey = 0x07897652;


TUint32 KSecond	= 1000 * 1000;
TUint32 KWriteTimeout = 5 * KSecond;
TUint32 KReadTimeout  = 6 * KSecond;

TUint32 KPauseInterval = 10 * KSecond;
	
//-------------------------------------------------------------------------------------------------

void doSendThreadProcL()
	{
	CWritePropPipe* p = CWritePropPipe::NewL(KPipeKey, KFlowKey);
	CWritePropPipe& r = *p;

	r.ResetL();
	r << 1 << _L8("One");
	r << 2 << _L8("Two");
	r << 3 << _L8("I am a fish, I am a fish, I am a fish, I am a fish, I am a fish, I am a fish");
	r.CommitL(KWriteTimeout);

	User::After(KPauseInterval);

	// Write a bit more data for the active reader to pick up.
	r.ResetL();
	r << 4 << _L8("Four");
	r << 5 << _L8("Five");
	r.CommitL(KWriteTimeout);
	
	User::After(KPauseInterval);
	
	r.ResetL();
	r << 6 << _L8("Six");
	r << 7 << _L8("Seven");
	r.CommitL(KWriteTimeout);
	
	User::After(KPauseInterval);
		
	delete p;
	}

TInt SendThreadProc(void *)
	{
	CTrapCleanup* c = CTrapCleanup::New();
	
	TRAP_IGNORE(doSendThreadProcL());
	
	delete c;
	return 1;
	}


void doReadTestL()
	{
	User::After(1000000);
	
	CReadPropPipe* p = CReadPropPipe::NewL(KPipeKey, KFlowKey);
	CReadPropPipe& r = *p;
	
	TRAPD(e, r.RefreshL(KReadTimeout));
	if(KErrNone == e)
		{
		TInt32 i1, i2, i3;
		TBuf8<128>  r1, r2, r3;
		
		r >> i1 >> r1;
		r >> i2 >> r2;
		r >> i3 >> r3;
		}
	else
		{
		User::Invariant();
		}	
		
	delete p;	
	}

	

	
//-------------------------------------------------------------------------------------------------

class TTestActiveReader : public MReadPropPipeObserver
	{
public:	
	TTestActiveReader() : iCount(0) { ;	};
	
	void OnReadyToRead(CReadPropPipe& r)
		{
		switch(iCount++)
			{
		case 0:  
			{
			TInt32 i4, i5;
			TBuf8<128>  r4, r5;
			r.RefreshL(0);

			r >> i4 >> r4;
			r >> i5 >> r5;
			break;
			} 
		case 1:
			{
			TInt32 i6, i7;
			TBuf8<128>  r6, r7;
			r.RefreshL(0);

			r >> i6 >> r6;
			r >> i7 >> r7;
	
			CActiveScheduler::Stop();
			break;
			} 
		default: User::Invariant();
				 break;
			}
		}
	
private:
	TInt iCount;
	};

void doActiveReadTest()
	{
	TTestActiveReader reader;
	CNotifyReadPropPipe* p = CNotifyReadPropPipe::NewL(KPipeKey, KFlowKey, reader);	
	
	CActiveScheduler::Start();	

	delete p;
	}
	
//-------------------------------------------------------------------------------------------------

LOCAL_C void MainL()
	{
	__UHEAP_MARK;

	RThread thread;
	TInt e1 = thread.Create(_L("SendThread"), SendThreadProc, 10000, 10000, 15000, NULL);

	if(e1 != KErrNone)
		{
		User::Invariant(); // Bail... this is only an experiment so forget clenup
		}


	// Start threads
	thread.Resume();
	doReadTestL();
	doActiveReadTest();
	
	User::After(60 * KSecond);
	
	__UHEAP_MARKEND;
	}


GLDEF_C TInt E32Main()
	{
	CTrapCleanup* c = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler; 
	CActiveScheduler::Install(scheduler);

	TRAPD(r,MainL());

	delete c;
	delete scheduler;
	return KErrNone;
	}

//-------------------------------------------------------------------------------------------------
