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
// Contains MBufMgr Test Step 12 General Test
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
#include "Test12General.h"
#include <comms-infras/commsbufpond.h>

// constructor
CTest12General::CTest12General()
	{
	SetTestStepName(_L("MBufMgrTest12"));// Store the name of this test case
	}

// destructor
CTest12General::~CTest12General() 
	{
	}

void CTest12General::LeaveIfErrorL(TInt aErr, RMBufChain& aChain, const TDesC& aMsg)
	{
	if(aErr != KErrNone)
		{
		TBuf<129> logLine;
		logLine.Format(_L("Error %d from %S"), aErr, &aMsg);
		INFO_PRINTF2(_L("%S"), &logLine);
		aChain.Free();
		User::Leave(EFail);
		}
	}

void CTest12General::LeaveIfDifferentL(TInt aLHS, TInt aRHS, RMBufChain& aChain, const TDesC& aMsg)
	{
	if(aLHS != aRHS)
		{
		TBuf<129> logLine;
		logLine.Format(_L("%d != %d in %S"), aLHS, aRHS, &aMsg);
		INFO_PRINTF2(_L("%S"), &logLine);
		aChain.Free();
		User::Leave(EFail);
		}
	}
//
enum TVerdict CTest12General::doTestStepL(void)
	{
	SetTestStepResult(EFail);
	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 - Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CreateInstanceMBufMgrL(KMBufDefaultHeapSize);
	CleanupClosePushL(iBufPond);

	//-------------- substep 2 -------------------- 
	INFO_PRINTF1(_L("  02 - Allocate two desc. (Des1 & Des2) of length 12345."));
	INFO_PRINTF1(_L("     - Fill Des1 with a pattern, and Des2 with zeros:"));
	TBuf8<12345> *aDes1, *aDes2;
    CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<12345> );
    CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<12345> );
	aDes1->SetLength(12345);
	aDes2->SetLength(12345);
	StripeDes(*aDes1, 0, 12345, '@', 'Z');
	StripeDes(*aDes2, 0, 12345, 0, 0);

	//-------------- substep 3 -------------------- 
	INFO_PRINTF1(_L("  03 - Copy in Des1 into Chain:"));
	RMBufChain aChain;
	TRAPD(ret,aChain.AllocL(12345));
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBufChain:"));
		User::Leave(EFail);
		}
	aChain.CopyIn(*aDes1);

	//-------------- substep 4 -------------------- 
	INFO_PRINTF1(_L("  04 - Split Chain into  packets of 5000 bytes. Prepend Header1 to each:"));
	INFO_PRINTF1(_L("     - Place them into Outq1 packet queue. Print the content of Outq1:"));
	int len = aChain.Length();
	RMBufChain aTempChain;
	Test_str aHeader1;
	StripeMem((TUint8 *)&aHeader1, 0, sizeof(aHeader1), 'a', 'm');
	RMBufPktQ aOutq1;

	while (len>5000)
		{
		TRAP(ret,aChain.SplitL(5000, aTempChain));
		if (ret != KErrNone)
			{
			INFO_PRINTF1(_L("Error: Couldn't Split chain:"));
			aChain.Free();
			User::Leave(EFail);
			}
		TRAP(ret,aChain.PrependL(sizeof(aHeader1)));
		if (ret != KErrNone)
			{
			INFO_PRINTF1(_L("Error: Couldn't prepend header to the chain:"));
			aChain.Free();
			User::Leave(EFail);
			}
		aChain.CopyIn(TPtrC8((TUint8 *)&aHeader1, sizeof(aHeader1)));
		aOutq1.Append(aChain);
		aChain.Append(aTempChain);
		len -= 5000;
		}
	TRAP(ret,aChain.PrependL(sizeof(aHeader1)));
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't prepend header to the chain:"));
		aChain.Free();
		User::Leave(EFail);
		}
	aChain.CopyIn(TPtrC8((TUint8 *)&aHeader1, sizeof(aHeader1)));
	aOutq1.Append(aChain);

	Test_QueueStats(aOutq1, _S("OutQ1"));

	//-------------- substep 5 -------------------- 
	INFO_PRINTF1(_L("  05 - Split packets of Outq1 into 556-byte long packets and prepend another Header2 onto each. Place them all into packet queue Outq2:"));
	INFO_PRINTF1(_L("     - Place them all into packet queue Outq2. Print the contents of Outq1 & Outq2"));
	Test_str aHeader2;
	StripeMem((TUint8 *)&aHeader2, 0, sizeof(aHeader2), 'n', 'z');
	RMBufPktQ aOutq2;
	while (!aOutq1.IsEmpty())
		{
		(void)aOutq1.Remove(aChain);
		len = aChain.Length();
		while (len>556)
			{
			TRAP(ret,aChain.SplitL(556, aTempChain));
			if (ret != KErrNone)
				{
				INFO_PRINTF1(_L("Error: Couldn't Split chain:"));
				aChain.Free();
				User::Leave(EFail);
				}
			TRAP(ret,aChain.PrependL(sizeof(aHeader2)));
			if (ret != KErrNone)
				{
				INFO_PRINTF1(_L("Error: Couldn't prepend header to the chain:"));
				aChain.Free();
				User::Leave(EFail);
				}
			aChain.CopyIn(TPtrC8((TUint8 *)&aHeader2, sizeof(aHeader2)));
			aOutq2.Append(aChain);
			aChain.Append(aTempChain);
			len -= 556;
			}
		TRAP(ret,aChain.PrependL(sizeof(aHeader2)));
		if (ret != KErrNone)
			{
			INFO_PRINTF1(_L("Error: Couldn't prepend header to the chain:"));
			aChain.Free();
			User::Leave(EFail);
			}
		aChain.CopyIn(TPtrC8((TUint8 *)&aHeader2, sizeof(aHeader2)));
		aOutq2.Append(aChain);
		}
	Test_QueueStats(aOutq1, _S("OutQ1"));
	Test_QueueStats(aOutq2, _S("OutQ2"));

	//-------------- substep 6 -------------------- 
	INFO_PRINTF1(_L("  06 - Copy the chains from Outq2 into another packet queue Inq2."));
	INFO_PRINTF1(_L("     - Print the contents of Outq2 & Inq1:"));
	RMBufPktQ aInq2;

	while (!aOutq2.IsEmpty())
		{
		(void)aOutq2.Remove(aChain);
		TRAP(ret,aChain.CopyL(aTempChain));
		if (ret != KErrNone)
			{
			INFO_PRINTF1(_L("Error: Couldn't copy chain"));
			aChain.Free();
			User::Leave(EFail);
			}
		aChain.Free();
		aChain.Assign(aTempChain);
		aInq2.Append(aChain);
		}

	Test_QueueStats(aOutq2, _S("OutQ2"));
	Test_QueueStats(aInq2, _S("InQ2"));

	//-------------- substep 7 -------------------- 
	INFO_PRINTF1(_L("  07 - Group the chains back to 5000+Header1 long packets (Including removal of Header2)."));
	INFO_PRINTF1(_L("     - Check the content of each Header2 that is removed."));
	INFO_PRINTF1(_L("     - Place them in packet queue Inq1. Print the contents of Inq2 & Inq1:"));
	RMBufPktQ aInq1;
	Test_str aTempHeader;
	aChain.Init();
	len = 0;
	while (!aInq2.IsEmpty())
		{
		while (len<5020 && !aInq2.IsEmpty())
			{
			(void)aInq2.Remove(aTempChain);
			TPtr8 dest((TUint8 *)&aTempHeader, sizeof(aTempHeader),sizeof(aTempHeader));
			aTempChain.CopyOut(dest);
			aTempChain.TrimStart(sizeof(aTempHeader));
			len += aTempChain.Length();
			aChain.Append(aTempChain);
			if(Mem::Compare((TUint8 *)&aHeader2, sizeof(aHeader2), 
				            (TUint8 *)&aTempHeader, sizeof(aTempHeader))!=0)
				{
				INFO_PRINTF1(_L("Error: The content of header is not the same"));
				aChain.Free();
				User::Leave(EFail);
				}
			}
		aInq1.Append(aChain);
		aChain.Init();
		len = 0;
		}
	Test_QueueStats(aInq2, _S("InQ2"));
	Test_QueueStats(aInq1, _S("InQ1"));

	//-------------- substep 8 -------------------- 
	INFO_PRINTF1(_L("  08 - Group the packets into a single packet (Removing Header1, as well)."));
	INFO_PRINTF1(_L("     - Check the content of each Header1 that is removed."));
	INFO_PRINTF1(_L("     - Place it in Chain. Print the contents of Inq1:"));

	while (!aInq1.IsEmpty())
		{
		(void)aInq1.Remove(aTempChain);
		TPtr8 dest1((TUint8 *)&aTempHeader, sizeof(aTempHeader), sizeof(aTempHeader));
		aTempChain.CopyOut(dest1);
		aTempChain.TrimStart(sizeof(aTempHeader));
		len = aTempChain.Length();
		len = len;
		aChain.Append(aTempChain);
		if(Mem::Compare((TUint8 *)&aHeader1, sizeof(aHeader1), 
				  (TUint8 *)&aTempHeader, sizeof(aTempHeader))!=0)
			{
			INFO_PRINTF1(_L("Error: The content of header is not the same"));
			aChain.Free();
			User::Leave(EFail);
			}
		}
	len = aChain.Length();
	len = len;
	Test_QueueStats(aInq1, _S("InQ1"));

	//-------------- substep 9 -------------------- 
	INFO_PRINTF1(_L("  09 - Copy out Chain into Des2.:"));
	aChain.CopyOut(*aDes2);

	//-------------- substep 10 -------------------- 
	INFO_PRINTF1(_L("  10 - Compare the content of Des1 & Des2:"));
	if(aDes1->Compare(*aDes2))
		{
		aChain.Free();
		INFO_PRINTF1(_L("Error: The content is not the same"));
		User::Leave(EFail);
		}
	aChain.Free();

	//-------------- substep 11 --------------------
	INFO_PRINTF1(_L("  11 - RMBufChain::ReallocL():"));
	TRAP(ret, aChain.AllocL(32));
	// ...32 bytes at start
	LeaveIfErrorL(ret, aChain, _L("AllocL(32)") );
	TRAP(ret, aChain.ReAllocL(24));
	// ...24 bytes at start
	LeaveIfErrorL(ret, aChain, _L("ReAllocL(24)") );	
	LeaveIfDifferentL(24, aChain.Length(), aChain, _L("length check") );
	aChain.TrimStart(8);
	// ...16 bytes starting at offset 8
	LeaveIfDifferentL(16, aChain.Length(), aChain, _L("length check") );
	TRAP(ret, aChain.ReAllocL(32));
	// ...32 bytes starting at offset 8
	LeaveIfErrorL(ret, aChain, _L("ReAllocL(32)") );
	LeaveIfDifferentL(32, aChain.Length(), aChain, _L("length check") );
	TRAP(ret, aChain.ReAllocL(120));
	// ...120 bytes starting at offset 8, only one buffer
	LeaveIfErrorL(ret, aChain, _L("ReAllocL(120)") );
	LeaveIfDifferentL(120, aChain.Length(), aChain, _L("length check") );
	LeaveIfDifferentL((TInt) aChain.First(), (TInt) aChain.Last(), aChain, _L("single buff check") );
	TRAP(ret, aChain.ReAllocL(121));
	// ...121 bytes starting at offset 8, two buffers
	LeaveIfErrorL(ret, aChain, _L("ReAllocL(121)") );
	LeaveIfDifferentL(121, aChain.Length(), aChain, _L("length check") );
	TBool sameBuff = aChain.First() == aChain.Last();
	LeaveIfDifferentL(sameBuff, 0, aChain, _L("two buffs check") );
	aChain.Free();

	//-------------- substep 12 -------------------- 
	INFO_PRINTF1(_L("  12 - Clean up stack:"));
    CleanupStack::PopAndDestroy(aDes2);
    CleanupStack::PopAndDestroy(aDes1);
    CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
        SetTestStepResult(EPass);

	return TestStepResult();
	}

void CTest12General::Test_QueueStats(RMBufPktQ &aQueue, const TText *aName)
	{
	RMBufChain chain = (RMBufChain)aQueue.First();

	INFO_PRINTF2(_L("    Queue stats %5s    : "), aName);
	
	if (chain.IsEmpty()) 
		INFO_PRINTF1(_L("        Empty"));
	else
		{
		TInt n = 0;
		while (!chain.IsEmpty())
			{
			n++;
			INFO_PRINTF3(_L("        chain #%2d:%d "), n, chain.Length());
			chain = chain.Next();
			}
		}
	}
