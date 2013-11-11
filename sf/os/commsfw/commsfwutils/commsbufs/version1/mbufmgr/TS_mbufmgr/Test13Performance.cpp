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
// Contains MBufMgr Test Step 13 that measures the time spent for alloc/free methods
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test13Performance.h"

// constructor
CTest13Performance::CTest13Performance()
	{
	iTestStepName = _L("MBufMgrTest13");// Store the name of this test case
	}

// destructor
CTest13Performance::~CTest13Performance()
	{
	}


//
enum TVerdict CTest13Performance::doTestStepL(void)
	{
	__UHEAP_MARK;
	
	TInt aMBufNo; //Taken from the script;
	TInt aLoopNo; //Taken from the script;
	TTime aTimeStart, aTimeStop;
	//-------------- substep 0 --------------------
	Log(_L("  00 Read parameters from the script:"));
	TInt bRet = GetIntFromConfig(_L("MBufMgrTest13"), _L("MBufNo"), aMBufNo);
	if (!bRet) return EFail;
	bRet = GetIntFromConfig(_L("MBufMgrTest13"), _L("LoopNo"), aLoopNo);
	if (!bRet) return EFail;
	Log(_L("     Info: MBUFNO = %d, LOOPNO= %d"),aMBufNo, aLoopNo);

#ifdef __CFLOG_ACTIVE
	__CFLOG_CREATEL;
	__CFLOG_OPEN;
#endif
	
	//-------------- substep 1 --------------------
	Log(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);

	CleanupStack::PushL(CreateInstanceMBufMgrL((TUint)(3*KMBufSmallSize*aMBufNo)));

	TInt i,j,k; //Counters
	TInt64 us;
	RMBufQ aBufList1, aBufList2; //Will contain all small buffers;
	RMBuf* aBuf=0;

	for (i = 0; i<aLoopNo; i++)
		{
		Log(_L(" Loop #%d"), i + 1);

		//-------------- substep 2 --------------------
		Log(_L("  02 Allocate Chain that contains RMBUFNO buffers. Free Chain."));
		Log(_L("     ...Repeat the same 1000 times.Display the time spent:"));
		RMBufChain aChain;
		aTimeStart.UniversalTime();
		for (j = 0;j < 1000; j++)
		{
			TRAPD(ret,aChain.AllocL(KMBufSmallSize*aMBufNo));
			if (ret != KErrNone)
				{
				Log(_L("Error: Couldn't allocate RMBuf:"));

#ifdef __CFLOG_ACTIVE
				__CFLOG_CLOSE;
				__CFLOG_DELETE;
#endif
				User::Leave(EFail);
				}

			aChain.Free();
		}
		aTimeStop.UniversalTime();
		us = aTimeStop.MicroSecondsFrom(aTimeStart).Int64()/1000;
		Log(_L("     ...Info: The time spent on 1000 alloc and free is %d mS"),us);

		//-------------- substep 3 --------------------
		Log(_L("  03 Create %d chains, each containing one RMBuf."), 2 * aMBufNo);
		Log(_L("     ...Then, free all even chains:"));
		aTimeStart.UniversalTime();
		for (j = 0;j<2*aMBufNo;j++)
			{
			TRAPD(ret,aBuf = iMBMngr->AllocL(KMBufSmallSize));
			if (ret != KErrNone)
				{
				Log(_L("Error: Couldn't allocate RMBuf number %d:"),j);

#ifdef __CFLOG_ACTIVE
				__CFLOG_CLOSE;
				__CFLOG_DELETE;
#endif
				User::Leave(EFail);
				}
			aBufList1.Append(aBuf);
			}

		k=0;
		while (!aBufList1.IsEmpty())
			{
			k++;
			aBuf = aBufList1.Remove();
			if (k%2)
				iMBMngr->Free(aBuf);
			else
				aBufList2.Append(aBuf);
			}
		aTimeStop.UniversalTime();
		us = aTimeStop.MicroSecondsFrom(aTimeStart).Int64()/1000;
		Log(_L("     ...Info: The time spent is %d mS"), us);

		//-------------- substep 4 --------------------
		Log(_L("  04 Allocate Chain that contains RMBUFNO buffers. Free Chain."));
		Log(_L("     ...Repeat the same 1000 times.Display the time spent:"));
		aTimeStart.UniversalTime();
		for (j = 0;j < 1000; j++)
		{
			TRAPD(ret,aChain.AllocL(KMBufSmallSize*aMBufNo));
			if (ret != KErrNone)
				{
				Log(_L("Error: Couldn't allocate RMBuf number %d:"),j);

#ifdef __CFLOG_ACTIVE
				__CFLOG_CLOSE;
				__CFLOG_DELETE;
#endif
				User::Leave(EFail);
				}

			aChain.Free();
		}
		aTimeStop.UniversalTime();
		us = aTimeStop.MicroSecondsFrom(aTimeStart).Int64()/1000;
		Log(_L("     ...Info: The time spent on 1000 alloc and free is %d mS"),us);

		//-------------- substep 5 --------------------
		Log(_L("  05 Free all remaining chains:"));
		while (!aBufList2.IsEmpty())
			{
			aBuf = aBufList2.Remove();
			iMBMngr->Free(aBuf);
			}
	}


	//-------------- substep 9 --------------------
	Log(_L("  09 Clean up stack:"));
    CleanupStack::PopAndDestroy(iMBMngr);
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
   	__UHEAP_MARKEND;
	return EPass;
	}
