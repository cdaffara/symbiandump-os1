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
// Contains MBufMgr Test Step 09 for Align() methods
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test09Align.h"

// constructor
CTest09Align::CTest09Align()
	{
	iTestStepName = _L("MBufMgrTest09");// Store the name of this test case
	}

// destructor
CTest09Align::~CTest09Align()
	{
	}

//
enum TVerdict CTest09Align::doTestStepL(void)
	{
	__UHEAP_MARK;
	
	//-------------- substep 0 --------------------
	Log(_L("  00 Read parameters from the script:"));
	CScriptInput* aInput;
	CleanupStack::PushL( aInput= new (ELeave) CScriptInput );
	TInt i;
	TBuf<20> aBuf;
	TInt bRet;
	aInput->aBufsBefore=0;
	for (i = 0;i<50;i++)
		{
		aBuf.Format(_L("Length%02d"),i+1);
		bRet = GetIntFromConfig(_L("MBufMgrTest09"), aBuf, aInput->aBuf[i].aLength);
		if (!bRet)
			{
			return EFail;
			}
		if (aInput->aBuf[i].aLength == -1) break;

		aBuf.Format(_L("Offset%02d"),i+1);
		bRet = GetIntFromConfig(_L("MBufMgrTest09"), aBuf, aInput->aBuf[i].aOffset);
		if (!bRet)
			{
			return EFail;
			}
		aInput->aBufsBefore++;
		}
	bRet = GetIntFromConfig(_L("MBufMgrTest09"),_L("BuffsAfter") , aInput->aBufsAfter);

   	Log(_L("     The following has been read from the script:"));
   	Log(_L("     Number of RMBufs in chain before align= %2d"),aInput->aBufsBefore);
   	Log(_L("     Number of RMBufs in chain after align = %2d"),aInput->aBufsAfter);

	for (i = 0; i < aInput->aBufsBefore ; i++ )
		{
   		Log(_L("     Chain #%2d: Offset=%3d Length=%3d"),
					         i+1, aInput->aBuf[i].aOffset, aInput->aBuf[i].aLength);
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
	Log(_L("  02 Create TestStr structure and fill it with data:"));
	CTestStr* aTestStr1;
	CleanupStack::PushL( aTestStr1 = new (ELeave) CTestStr );

	aTestStr1->iSrcAddr = 0x01234567;
	aTestStr1->iDstAddr = 0x89abcdef;
	aTestStr1->iSrcPort = 0x0246;
	aTestStr1->iDstPort = 0x8ace;
	aTestStr1->iLength  = sizeof(CTestStr);
	aTestStr1->iOptions = 0x1359;
	StripeMem(aTestStr1->iData, 0, sizeof(aTestStr1->iData), '@', 'Z');

	//-------------- substep 3 --------------------
	Log(_L("  03 Create a chain containing RMBufs as specified in the script:"));
	RMBufChain aChain;
	RMBuf *buf=0;
	for (i = aInput->aBufsBefore-1; i >= 0 ; i-- )
		{
		TRAPD(ret,buf = iMBMngr->AllocL(KMBufSmallSize));
		if (ret!=KErrNone)
			{
			Log(_L("Error:Could not allocate buffer"));
			aChain.Free();

#ifdef __CFLOG_ACTIVE
			__CFLOG_CLOSE;
			__CFLOG_DELETE;
#endif
			User::Leave(EFail);
			}
		buf->SetData(aInput->aBuf[i].aOffset,aInput->aBuf[i].aLength);
		aChain.Prepend(buf);
		}

	//-------------- substep 4 --------------------
	Log(_L("  04 Copy in TestStr into Chain:"));
	aChain.CopyIn(TPtrC8((TUint8 *)aTestStr1, sizeof(CTestStr)));

	//-------------- substep 5 --------------------
	Log(_L("  05 Allign Chain for the size of TestStr:"));
	aChain.Align(sizeof(CTestStr));

	//-------------- substep 6 --------------------
	Log(_L("  06 Check the number of RMBufs in chain after allign:"));
	if (aChain.NumBufs() != aInput->aBufsAfter)
		{
		Log(_L("ERROR: Number of bufs is %d instead of %d"),aChain.NumBufs(),aInput->aBufsAfter);
		aChain.Free();

#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 7 --------------------
	Log(_L("  07 Create the pointer of TestStr type and point to the 1st RMBuf in chain:"));
	CTestStr* aTestStr2 = (CTestStr*)(aChain.First()->Ptr());

	//-------------- substep 8 --------------------
	Log(_L("  08 Compare TestStr1 and TestData2. They should be the same:"));
	if ( (aTestStr1->iSrcAddr != aTestStr2->iSrcAddr) ||
		 (aTestStr1->iDstAddr != aTestStr2->iDstAddr) ||
	     (aTestStr1->iSrcPort != aTestStr2->iSrcPort) ||
		 (aTestStr1->iDstPort != aTestStr2->iDstPort) ||
	     (aTestStr1->iLength  != aTestStr2->iLength)  ||
	     (aTestStr1->iOptions != aTestStr2->iOptions) ||
	     (Mem::Compare(aTestStr1->iData, sizeof(aTestStr1->iData),
		      aTestStr2->iData, sizeof(aTestStr2->iData))) )
		{
		Log(_L("ERROR: They are not the same"));
		aChain.Free();

#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 9 --------------------
	Log(_L("  09 Free the chain. Clean up stack:"));
	aChain.Free();
    CleanupStack::PopAndDestroy(aTestStr1);
    CleanupStack::PopAndDestroy(iMBMngr);
	CActiveScheduler::Install(NULL);
    CleanupStack::PopAndDestroy(iActSch);
    CleanupStack::PopAndDestroy(aInput);

#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif
 	__UHEAP_MARKEND;
	return EPass;
	}
