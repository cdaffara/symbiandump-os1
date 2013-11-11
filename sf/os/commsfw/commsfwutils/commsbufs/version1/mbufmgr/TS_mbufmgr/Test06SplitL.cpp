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
// Contains MBufMgr Test Step 06 for SplitL method
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test06SplitL.h"

// constructor
CTest06Split::CTest06Split()
	{
	iTestStepName = _L("MBufMgrTest06");// Store the name of this test case
	}

// destructor
CTest06Split::~CTest06Split()
	{
	}

//
enum TVerdict CTest06Split::doTestStepL(void)
	{
	__UHEAP_MARK;
		
	//-------------- substep 0 --------------------
	Log(_L("  00 Read parameters from the script:"));
	TInt aOffset;// Will contain the position in chain where split action will be done
	TInt aLength;// Will contain the length of desc & chains in the test
	TInt bRet = GetIntFromConfig(_L("MBufMgrTest06"), _L("Length"), aLength);
	if (!bRet)
		{
		return EFail;
		}
	bRet = GetIntFromConfig(_L("MBufMgrTest06"), _L("Offset"), aOffset);
	if (!bRet)
		{
		return EFail;
		}
 	Log(_L("     ------- Chain & Desc LENGTH   = %d --"), aLength);
 	Log(_L("     ------- OFFSET where to Split = %d --"), aOffset);
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
	Log(_L("  02 Allocating two descriptors (Des1 & Des2):"));
	TBuf8<1000> *aDes1, *aDes2;
    CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<1000> );
    CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<1000> );
	aDes1->SetLength(aLength);
	aDes2->SetLength(aLength);

	//-------------- substep 3 --------------------
	Log(_L("  03 Fill in Des1 with a pattern:"));
	StripeDes(*aDes1, 0, aLength, '@', 'Z');

	//-------------- substep 4 --------------------
	Log(_L("  04 Fill in Des2 with zeros:"));
	StripeDes(*aDes1, 0, aLength, 0, 0);

	//-------------- substep 5 --------------------
	Log(_L("  05 Allocate Chain1:"));
	RMBufChain aChain1;
	TRAPD(ret,aChain1.AllocL(aLength));
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate Chain1:"));
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 6 --------------------
	Log(_L("  06 Copy in Des1 into Chain1:"));
	aChain1.CopyIn(*aDes1);

	//-------------- substep 7 --------------------
	Log(_L("  07 Split Chain1. The 2nd part goes to Chain2:"));
	RMBufChain aChain2;
	TRAP(ret,aChain1.SplitL(aOffset, aChain2));
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't Split"));
		aChain1.Free();
		aChain2.Free();
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 8 --------------------
	Log(_L("  08 Check the length of the chains. They should be OFFSET and (LENGTH - OFFSET):"));
	TInt temp;
	if ( (temp = aChain1.Length()) != aOffset)
		{
		Log(_L("Error: Chain1 length is %d instead of %d"), temp, aOffset);
		aChain1.Free();
		aChain2.Free();
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	if ( (temp = aChain2.Length()) != aLength - aOffset)
		{
		Log(_L("Error: Chain2 length is %d instead of %d"), temp, aLength - aOffset);
		aChain1.Free();
		aChain2.Free();
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 9 --------------------
	Log(_L("  09 Copy out Chain1 into Des2:"));
	aChain1.CopyOut(*aDes2);

	//-------------- substep 10 --------------------
	Log(_L("  10 Check the length of Des2. It should be OFFSET "));
	if ( (temp = aDes2->Length()) != aOffset)
		{
		Log(_L("Error: Des2 length is %d instead of %d"), temp, aOffset);
		aChain1.Free();
		aChain2.Free();
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 11 --------------------
	Log(_L("  11 Set the length of Des2 to LENGTH value"));
	aDes2->SetLength(aLength);

	//-------------- substep 12 --------------------
	Log(_L("  12 Copy out the whole Chain2 into Des2 at offset = OFFSET"));
	TPtr8 dest((TUint8*)aDes2->Ptr() + aOffset, aLength-aOffset, aLength - aOffset);
	aChain1.CopyOut(dest);

	//-------------- substep 13 --------------------
	Log(_L("  13 Compare the content of Des1 & Des2:"));
	if(aDes1->Compare(*aDes2))
		{
		aChain1.Free();
		aChain2.Free();
		Log(_L("Error: The content is not the same"));
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 14 --------------------
	Log(_L("  14 Free the chains. Clean up stack:"));
	aChain1.Free();
	aChain2.Free();
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
