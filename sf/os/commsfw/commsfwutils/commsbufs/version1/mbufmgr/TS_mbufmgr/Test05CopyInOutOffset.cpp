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
// Contains MBufMgr Test Step 05 for CopyIn() & CopyOut() methods with offset
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test05CopyInOutOffset.h"

// constructor
CTest05CopyInOutOffset::CTest05CopyInOutOffset()
	{
	iTestStepName = _L("MBufMgrTest05");// Store the name of this test case
	}

// destructor
CTest05CopyInOutOffset::~CTest05CopyInOutOffset()
	{
	}

//
enum TVerdict CTest05CopyInOutOffset::doTestStepL(void)
	{
	__UHEAP_MARK;
		
	//-------------- substep 0 --------------------
	bool aTest;
	Log(_L("  00 Read from script which test to run:"));
	TPtrC aPtrResult;
	TInt bRet = GetStringFromConfig(_L("MBufMgrTest05"), _L("test"), aPtrResult);
	if (!bRet)
		{
		return EFail;
		}

	if (aPtrResult==_L("CopyIn" ))
		{
		aTest = true;
	 	Log(_L("     ------------ Copy In Test starts ---------------:"));
		}
	else if (aPtrResult==_L("CopyOut"))
		{
		aTest = false;
	 	Log(_L("     ------------ Copy Out Test starts --------------:"));
		}
	else
		{
		Log(_L("Failed to read test case - there should be either CopyIn or CopyOut - case sensitive"));
		return EFail;
		}

#ifdef __CFLOG_ACTIVE
	__CFLOG_CREATEL;
	__CFLOG_OPEN;
#endif

	//-------------- substep 1 --------------------
	Log(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CleanupStack::PushL(CreateInstanceMBufMgrL(KMBufDefaultHeapSize));

	//-------------- substep 2 --------------------
	Log(_L("  02 Allocate two 5000-bytes long descriptors (Des1 & Des2):"));
	TBuf8<5000> *aDes1, *aDes2;
    CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<5000> );
    CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<5000> );
	aDes1->SetLength(5000);
	aDes2->SetLength(5000);

	//-------------- substep 3 --------------------
	Log(_L("  03 Allocate 5000-bytes long RMBufChain:"));
	RMBufChain aChain;
	TRAPD(ret,aChain.AllocL(5000));
	if (ret != KErrNone)
		{
		Log(_L("Error: Couldn't allocate RMBuf:"));
#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 4 --------------------
	Log(_L("  04 Fill in Des1 with a pattern:"));
	StripeDes(*aDes1, 0, 5000, '@', 'Z');

	//-------------- substep 5 --------------------
	Log(_L("  05 Fill in Des2 with a patter different then that in Des1:"));
	StripeDes(*aDes2, 0, 5000, 'a', 'z');

	//-------------- substep 6 --------------------
	Log(_L("  06 Fill in Chain with zeros:"));
    aChain.FillZ();

	if (aTest)
		{
	//----------- CopyIn() method check only-----------
		//-------------- substep 7 --------------------
		Log(_L("  07 Copy in the last 4000 bytes of Des1 into the last 4000 of Chain:"));
		aChain.CopyIn(aDes1->Mid(1000),1000);

		//-------------- substep 8 --------------------
		Log(_L("  08 Copy out Chain into Des2:"));
		aChain.CopyOut(*aDes2);

		//-------------- substep 9 --------------------
		Log(_L("  09 Fill in the 1st 1000 bytes of Des1 with zeros:"));
		StripeDes(*aDes1, 0, 1000, '\0', '\0');
		}
	else
		{
	//----------- CopyOut() method check unly-----------
		//-------------- substep 7 --------------------
		Log(_L("  07 Copy in Des1 into Chain:"));
		aChain.CopyIn(*aDes1);

		//-------------- substep 8 --------------------
		Log(_L("  08 Fill in the 1st 1000 bytes of Des1 & Des2 with zeros:"));
		StripeDes(*aDes1, 0, 1000, '\0', '\0');
		StripeDes(*aDes2, 0, 1000, '\0', '\0');

		//-------------- substep 9 --------------------
		Log(_L("  09 Copy out the last 4000 bytes of Chain into last 4000 of Des2:"));
		TPtr8 dest((TUint8*)aDes2->Ptr()+1000,4000,4000);
		aChain.CopyOut(dest, 1000);
		}

	//-------------- substep 10 --------------------
	Log(_L("  10 Compare the content of Des1 & Des2:"));
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

	//-------------- substep 11 --------------------
	Log(_L("  11 Free the chain. Clean up stack:"));
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
