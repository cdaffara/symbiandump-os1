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
// Contains MBufMgr Test Step 07 for TrimStart() method
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test07TrimStart.h"

// constructor
CTest07TrimStart::CTest07TrimStart()
	{
	iTestStepName = _L("MBufMgrTest07");// Store the name of this test case
	}

// destructor
CTest07TrimStart::~CTest07TrimStart()
	{
	}

//
enum TVerdict CTest07TrimStart::doTestStepL(void)
	{
	__UHEAP_MARK;
	
	//-------------- substep 0 --------------------
	Log(_L("  00 Read parameters from the script:"));
	TInt aOffset;// Will contain the position in chain where split action will be done
	TInt aLength;// Will contain the length of desc & chains in the test
	TInt bRet = GetIntFromConfig(_L("MBufMgrTest07"), _L("Length"), aLength);
	if (!bRet)
		{
		return EFail;
		}
	bRet = GetIntFromConfig(_L("MBufMgrTest07"), _L("Offset"), aOffset);
	if (!bRet)
		{
		return EFail;
		}
   	Log(_L("     ------- Chain LENGTH              = %d --"), aLength);
 	Log(_L("     ------- OFFSET where to TrimStart = %d --"), aOffset);
	if ( (aLength > 1000)    ||(aLength <0) ||
		 (aOffset > aLength) || (aOffset <0)  )
		{
		Log(_L("ERROR: Wrong input parameter(s)"));
		return EFail;
		}

#ifdef __CFLOG_ACTIVE
	__CFLOG_CREATEL;
	__CFLOG_OPEN;
#endif
	
	//-------------- substep 1 --------------------
	Log(_L("  01 Creating CMBufManager and installing active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CleanupStack::PushL(CreateInstanceMBufMgrL(KMBufDefaultHeapSize));

	//-------------- substep 2 --------------------
	Log(_L("  02 Allocate two desc. of Length:LENGTH(Des1) & (LENGTH-OFFSET)(Des2):"));
	TBuf8<1000> *aDes1, *aDes2;
    CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<1000> );
    CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<1000> );
	aDes1->SetLength(aLength);
	aDes2->SetLength(aLength-aOffset);

	//-------------- substep 3 --------------------
	Log(_L("  03 Fill in Des1 with a pattern:"));
	StripeDes(*aDes1, 0, aLength, '@', 'Z');

	//-------------- substep 4 --------------------
	Log(_L("  04 Fill in Des2 with zeros:"));
	StripeDes(*aDes2, 0, aLength-aOffset, 0, 0);

	//-------------- substep 5 --------------------
	Log(_L("  05 Allocate Chain:"));
	RMBufChain aChain;
	TRAPD(ret,aChain.AllocL(aLength));
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBuf:"));
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 6 --------------------
	Log(_L("  06 Copy in Des1 into Chain1:"));
	aChain.CopyIn(*aDes1);

	//-------------- substep 7 --------------------
	Log(_L("  07 TrimStart Chain1 at OFFSET position (actually, remove the 1st OFFSET bytes:"));
	aChain.TrimStart(aOffset);

	//-------------- substep 8 --------------------
	Log(_L("  08 Check the length of the chain. It should be (LENGTH - OFFSET):"));
	TInt temp;
	if ( (temp = aChain.Length()) != aLength-aOffset)
		{
		Log(_L("Error: Chain length is %d instead of %d"), temp, aLength-aOffset);
		aChain.Free();
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 09 --------------------
	Log(_L("  09 Copy the last (LENGTH-OFFSET) bytes of Des1 into the start of Des1:"));
	(void)Mem::Copy((TUint8 *)aDes1->Ptr(), (TUint8 *)aDes1->Ptr()+aOffset, aLength-aOffset);

	//-------------- substep 10 --------------------
	Log(_L("  10 Set the length of Des1 to value LENGTH-OFFSET"));
	aDes1->SetLength(aLength-aOffset);

	//-------------- substep 11 --------------------
	Log(_L("  11 Copy out Chain into Des2:"));
	aChain.CopyOut(*aDes2);

	//-------------- substep 12 --------------------
	Log(_L("  12 Compare the content of Des1 & Des2:"));
	if(aDes1->Compare(*aDes2))
		{
		aChain.Free();
		Log(_L("Error: The content is not the same"));
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 13 --------------------
	Log(_L("  13 Free the chains. Clean up stack:"));
	aChain.Free();
    CleanupStack::PopAndDestroy(aDes2);
    CleanupStack::PopAndDestroy(aDes1);
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
