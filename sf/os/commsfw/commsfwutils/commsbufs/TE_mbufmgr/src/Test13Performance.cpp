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
//#ifdef SYMBIAN_OLD_EXPORT_LOCATION
//#include "networking/log.h"
//#include "networking/teststep.h"
//#else
//#include <networking/log.h>
//#include <networking/teststep.h>
//#endif
#include "TestStepCTMbufmgr.h"

#include "Test13Performance.h"

#include <comms-infras/commsbufpond.h>
// constructor
CTest13Performance::CTest13Performance()
	{
	SetTestStepName(_L("MBufMgrTest13"));// Store the name of this test case
	}

// destructor
CTest13Performance::~CTest13Performance() 
	{
	}


// 
enum TVerdict CTest13Performance::doTestStepL(void)
	{
	SetTestStepResult(EFail);
	TInt aMBufNo; //Taken from the script;
	TInt aLoopNo; //Taken from the script;
	TTime aTimeStart, aTimeStop;
	//-------------- substep 0 -------------------- 
	INFO_PRINTF1(_L("  00 Read parameters from the script:"));
	TInt bRet = GetIntFromConfig(_L("MBufMgrTest13"), _L("MBufNo"), aMBufNo);

	if (!bRet)
		return TestStepResult();

	bRet = GetIntFromConfig(_L("MBufMgrTest13"), _L("LoopNo"), aLoopNo);

	if (!bRet)
		return TestStepResult();


	INFO_PRINTF3(_L("     Info: MBUFNO = %d, LOOPNO= %d"),aMBufNo, aLoopNo);

	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);

	CreateInstanceMBufMgrL((TUint)(3*KMBufSmallSize*aMBufNo));
	CleanupClosePushL(iBufPond);

	TInt i,j,k; //Counters
	TInt64 us;
	RMBufQ aBufList1, aBufList2; //Will contain all small buffers;
	RMBuf* aBuf=0;

	for (i = 0; i<aLoopNo; i++)
		{
		INFO_PRINTF2(_L(" Loop #%d"), i + 1);

		//-------------- substep 2 -------------------- 
		INFO_PRINTF1(_L("  02 Allocate Chain that contains RMBUFNO buffers. Free Chain."));
		INFO_PRINTF1(_L("     ...Repeat the same 1000 times.Display the time spent:"));
		RMBufChain aChain;
		aTimeStart.UniversalTime();
		for (j = 0;j < 1000; j++)
		{
			TRAPD(ret,aChain.AllocL(KMBufSmallSize*aMBufNo));
			if (ret != KErrNone)
				{
				INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
				return TestStepResult();
				}

			aChain.Free();
		}
		aTimeStop.UniversalTime();
		us = aTimeStop.MicroSecondsFrom(aTimeStart).Int64()/1000;
		INFO_PRINTF2(_L("     ...Info: The time spent on 1000 alloc and free is %d mS"),us);

		//-------------- substep 3 -------------------- 
		INFO_PRINTF2(_L("  03 Create %d chains, each containing one RMBuf."), 2 * aMBufNo);
		INFO_PRINTF1(_L("     ...Then, free all even chains:"));

		aTimeStart.UniversalTime();
		TInt attemptCount = 0;
		for (j = 0;j<2*aMBufNo;j++)
			{
			aBuf = static_cast<RMBuf*>(iBufPond.Alloc(KMBufSmallSize, 0, KMaxTInt));
			if (aBuf == NULL)
				{
				++attemptCount;
				if(attemptCount == 3)
				    {
					INFO_PRINTF2(_L("Error: Couldn't allocate RMBuf number %d:"),j);
					User::Leave(EFail);
				    }
				else
				    {
				    User::After(100000);
				    }
				}
			if(aBuf)
			    {
			    aBufList1.Append(aBuf);
			    }
			}

		k=0;
		while (!aBufList1.IsEmpty())
			{
			k++;
			aBuf = aBufList1.Remove();
			if (k%2)
				{
				aBuf->Free();
				}
			else
				{
				aBufList2.Append(aBuf);
				}
			}
		aTimeStop.UniversalTime();
		us = aTimeStop.MicroSecondsFrom(aTimeStart).Int64()/1000;
		INFO_PRINTF2(_L("     ...Info: The time spent is %d mS"), us);


		//-------------- substep 4 -------------------- 
		INFO_PRINTF1(_L("  04 Allocate Chain that contains RMBUFNO buffers. Free Chain."));
		INFO_PRINTF1(_L("     ...Repeat the same 1000 times.Display the time spent:"));
		aTimeStart.UniversalTime();
		for (j = 0;j < 1000; j++)
		{
			TRAPD(ret,aChain.AllocL(KMBufSmallSize*aMBufNo));
			if (ret != KErrNone)
				{
				INFO_PRINTF2(_L("Error: Couldn't allocate RMBuf number %d:"),j);
				User::Leave(EFail);
				}

			aChain.Free();
		} 
		aTimeStop.UniversalTime();
		us = aTimeStop.MicroSecondsFrom(aTimeStart).Int64()/1000;
		INFO_PRINTF2(_L("     ...Info: The time spent on 1000 alloc and free is %d mS"),us);
		
		//-------------- substep 5 -------------------- 
		INFO_PRINTF1(_L("  05 Free all remainig chains:"));
		while (!aBufList2.IsEmpty())
			{
			aBuf = aBufList2.Remove();
			aBuf->Free();
			}
	}


	//-------------- substep 9 -------------------- 
	INFO_PRINTF1(_L("  09 Clean up stack:"));
    CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

	SetTestStepResult(EPass);
	return TestStepResult();
	}
