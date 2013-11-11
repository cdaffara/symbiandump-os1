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
// Contains MBufMgr Test Step 10 for CopyL() methods
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include "TestStepCTMbufmgr.h"
#include "TestSuiteCTMbufmgr.h"

#include "Test10CopyL.h"

// constructor
CTest10Copy::CTest10Copy()
	{
	iTestStepName = _L("MBufMgrTest10");// Store the name of this test case
	}

// destructor
CTest10Copy::~CTest10Copy()
	{
	}

//
enum TVerdict CTest10Copy::doTestStepL(void)
	{
	__UHEAP_MARK;
	
	//-------------- substep 0 --------------------
	Log(_L("  00 Read parameters from the script:"));
	CScriptInput* aInput;//It will hold input
	CleanupStack::PushL( aInput= new (ELeave) CScriptInput );
	TInt i;
	TBuf<20> aBuf;
	TInt bRet;
	aInput->aBufs=0;
	aInput->aChainLen=0;
	for (i = 0;i<50;i++)
		{
		aBuf.Format(_L("Length%02d"),i+1);
		bRet = GetIntFromConfig(_L("MBufMgrTest10"), aBuf, aInput->aBuf[i].aLength);
		if (!bRet) 
			{
			return EFail;
			}
		if (aInput->aBuf[i].aLength == -1) break;

		aBuf.Format(_L("Offset%02d"),i+1);
		bRet = GetIntFromConfig(_L("MBufMgrTest10"), aBuf, aInput->aBuf[i].aOffset);
		if (!bRet)
			{
			return EFail;
			}

		aInput->aBufs++;
		aInput->aChainLen += aInput->aBuf[i].aLength;
		}
	bRet = GetIntFromConfig(_L("MBufMgrTest10"),_L("CopyOffset") , aInput->aCopyOffset);
	bRet = GetIntFromConfig(_L("MBufMgrTest10"),_L("CopyLen") , aInput->aCopyLen);

   	Log(_L("     The following has been read from the script:"));
   	Log(_L("     Number of RMBufs in chain = %2d, with overall length CHAINLEN=%d"),aInput->aBufs,aInput->aChainLen);

	for (i = 0; i < aInput->aBufs ; i++ )
		{
   		Log(_L("     RMBuf #%2d: Offset=%3d Length=%3d"),
					         i+1, aInput->aBuf[i].aOffset, aInput->aBuf[i].aLength);
		}
   	Log(_L("     CopyL() parameters: COPYOFFSET=%d COPYLEN=%d"),aInput->aCopyOffset, aInput->aCopyLen);

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
	Log(_L("  02 Allocate two descriptors (Des1 & Des2) of length = Min(COPYLEN, CHAINLEN - COPYOFFSET):"));
	TBuf8<5000> *aDes1, *aDes2;
	TInt aDesLength;
    CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<5000> );
    CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<5000> );
	aDesLength = Min(aInput->aCopyLen, aInput->aChainLen-aInput->aCopyOffset);
	aDes1->SetLength(aDesLength);
	aDes2->SetLength(aDesLength);

	//-------------- substep 3 --------------------
	Log(_L("  03 Fill in Des1 with a pattern:"));
	StripeDes(*aDes1, 0, aDesLength, '@', 'Z');

	//-------------- substep 4 --------------------
	Log(_L("  04 Create Chain1 containing RMBufs as specified in the script:"));
	RMBufChain aChain1;
	RMBuf *buf=0;
	for (i = aInput->aBufs-1; i >= 0 ; i-- )
		{
		TRAPD(ret,buf = iMBMngr->AllocL(KMBufSmallSize));
		if (ret!=KErrNone)
			{
			Log(_L("Error:Could not allocate buffer"));
			aChain1.Free();

#ifdef __CFLOG_ACTIVE
			__CFLOG_CLOSE;
			__CFLOG_DELETE;
#endif
			User::Leave(EFail);
			}
		buf->SetData(aInput->aBuf[i].aOffset,aInput->aBuf[i].aLength);
		aChain1.Prepend(buf);
		}

	//-------------- substep 5 --------------------
	Log(_L("  05 Copy in Des1 into Chain1 at offset=COPYOFFSET:"));
	TBuf8<5000> *aDesTemp;
    CleanupStack::PushL( aDesTemp = new(ELeave) TBuf8<5000> );
	aDesTemp->SetLength(aDesLength+aInput->aCopyOffset);
	Mem::Copy ((TAny*)(aDesTemp->Ptr()+aInput->aCopyOffset), aDes1->Ptr(), aDesLength);
    aChain1.CopyIn(*aDesTemp);
    CleanupStack::PopAndDestroy(aDesTemp);

	//-------------- substep 6 --------------------
	Log(_L("  06 Copy Chain1 into Chain2:"));
    RMBufChain aChain2;
	aChain1.CopyL(aChain2,aInput->aCopyOffset, aInput->aCopyLen);

	//-------------- substep 7 --------------------
	Log(_L("  07 Copy out Chain2 into Des2:"));
    aChain2.CopyOut(*aDes2);

	//-------------- substep 8 --------------------
	Log(_L("  08 Compare the content of Des1 & Des2:"));
	if(aDes1->Compare(*aDes2))
		{
		Log(_L("Error: The content is not the same"));
		aChain1.Free();
		aChain2.Free();

#ifdef __CFLOG_ACTIVE
		__CFLOG_CLOSE;
		__CFLOG_DELETE;
#endif
		User::Leave(EFail);
		}

	//-------------- substep 9 --------------------
	Log(_L("  09 Free chain. Clean up stack:"));
	aChain1.Free();
	aChain2.Free();
    CleanupStack::PopAndDestroy(aDes2);
    CleanupStack::PopAndDestroy(aDes1);
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
