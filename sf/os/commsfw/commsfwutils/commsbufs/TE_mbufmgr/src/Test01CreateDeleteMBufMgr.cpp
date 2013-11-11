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
// Contains MBufMgr Test Step 01 to Create & destroy RMBufManager
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

#include "Test01CreateDeleteMBufMgr.h"
#include <comms-infras/commsbufpond.h>

// constructor
CTest01CreateDeleteMBufMgr::CTest01CreateDeleteMBufMgr()
	{
	SetTestStepName(_L("MBufMgrTest01"));// Store the name of this test case
	}

// destructor
CTest01CreateDeleteMBufMgr::~CTest01CreateDeleteMBufMgr() 
	{
	}

// 
enum TVerdict CTest01CreateDeleteMBufMgr::doTestStepL(void)
	{
	SetTestStepResult(EFail);
	//-------------- substep 1 -------------------- 
	INFO_PRINTF1(_L("  01 Create CMBufManager and install active scheduler: blh blah blha"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CreateInstanceMBufMgrL(KMBufDefaultHeapSize);

	//-------------- substep 2 -------------------- 
	INFO_PRINTF1(_L("  02 Clean up stack:"));
    iBufPond.Close();
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);

        SetTestStepResult(EPass);

	return TestStepResult();
	}

