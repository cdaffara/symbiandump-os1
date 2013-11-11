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
// Contains MBufMgr Test Step 04 for CopyIn() & CopyOut() methods
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

#include "Test04CopyInOut.h"
#include <comms-infras/commsbufpond.h>
// constructor
CTest04CopyInOut::CTest04CopyInOut()
	{
	SetTestStepName(_L("MBufMgrTest04"));// Store the name of this test case
	}

// destructor
CTest04CopyInOut::~CTest04CopyInOut() 
	{
	}

// 
enum TVerdict CTest04CopyInOut::doTestStepL(void)
	{
	SetTestStepResult(EFail);
	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CreateInstanceMBufMgrL(KMBufDefaultHeapSize);
	CleanupClosePushL(iBufPond);

	//-------------- substep 2 -------------------- 
	INFO_PRINTF1(_L("  02 Allocate two 5000-bytes long descriptors (Des1 & Des2):"));
	TBuf8<5000> *aDes1, *aDes2;
    CleanupStack::PushL( aDes1 = new(ELeave) TBuf8<5000> );
    CleanupStack::PushL( aDes2 = new(ELeave) TBuf8<5000> );
	aDes1->SetLength(5000);
	aDes2->SetLength(5000);

	//-------------- substep 3 -------------------- 
	INFO_PRINTF1(_L("  03 Fill in the Des1 with a pattern:"));
	StripeDes(*aDes1, 0, 5000, '@', 'Z');
	
	//-------------- substep 4 -------------------- 
	INFO_PRINTF1(_L("  04 Allocate 5000-bytes long RMBufChain:"));
	RMBufChain aChain;
	TRAPD(ret,aChain.AllocL(5000));
	if (ret != KErrNone)
		{
		INFO_PRINTF1(_L("Error: Couldn't allocate RMBuf:"));
		User::Leave(EFail);
		}

	//-------------- substep 5 -------------------- 
	INFO_PRINTF1(_L("  05 Copy in Des1 into Chain:"));
    aChain.CopyIn(*aDes1);

	//-------------- substep 6 -------------------- 
	INFO_PRINTF1(_L("  06 Fill in Des2 with zeros:"));
	StripeDes(*aDes2, 0, 5000, 0, 0);

	//-------------- substep 7 -------------------- 
	INFO_PRINTF1(_L("  07 Copy out Chain into Des2:"));
    aChain.CopyOut(*aDes2);

	//-------------- substep 8 -------------------- 
	INFO_PRINTF1(_L("  08 Compare the contents of Des1 & Des2:"));
	if(aDes1->Compare(*aDes2))
		{
		INFO_PRINTF1(_L("Error: The content is not the same"));
		aChain.Free();
		User::Leave(EFail);
		}

	//-------------- substep 9 -------------------- 
	INFO_PRINTF1(_L("  09 Free chain. Clean up stack:"));
	aChain.Free();
    CleanupStack::PopAndDestroy(aDes2);
    CleanupStack::PopAndDestroy(aDes1);
    CleanupStack::PopAndDestroy();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
        SetTestStepResult(EPass);

	return TestStepResult();
	}
