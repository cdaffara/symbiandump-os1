// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "TestPlayerUtils.h"
#include "OpenFileByHandle7805.h"

#include <caf/caf.h>

CTestMmfAclntOpenFile7805::CTestMmfAclntOpenFile7805(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	: CTestMmfAclntOpenFile7803(aTestName, aSectName, aKeyName)
	{}

CTestMmfAclntOpenFile7805* CTestMmfAclntOpenFile7805::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CTestMmfAclntOpenFile7805* self = new (ELeave) CTestMmfAclntOpenFile7805(aTestName, aSectName, aKeyName);
	return self;
	}

/**
 * Open new file.
 */
TVerdict CTestMmfAclntOpenFile7805::DoTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	TVerdict iAllocTestStepResult=EPass;
	TInt err = KErrNone;
	TBool result = EFalse;
	
	//>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
	if( PerformTestL(aPlayer) != EPass )
		{
		err = iError;
		}

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Test error, returned error code =  %d"), err);
		User::Leave(err);
		}
	else
		{
		//Check the iAllocTestStepResult
		if (iAllocTestStepResult != EPass)
			{
			result = ETrue;
			}
		}	
	
	TInt failCount = 1;
	TBool completed = EFalse;
	iAllocTestStepResult = EPass; // XXX check?? assume pass
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	
	aPlayer->UseSharedHeap(); //-Enable UseSharedHeap to test controller alloc failures
	
	for(;;)	
		{
		//__MM_HEAP_MARK;
		
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		
		//>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
		TVerdict verdict = EFail;
		TRAP(err, verdict = PerformTestL(aPlayer));
		
		if (err == KErrNone && verdict != EPass)
			{
			err = iError;
			}

		completed = EFalse;
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc == NULL)
				{
				reachedEnd = ETrue;
				failCount -= 1;
				}
			else
				{
				User::Free(testAlloc);	
				}			
			
			//Check the iAllocTestStepResult
			if (iAllocTestStepResult != EPass)
				{
				result = ETrue;
				}
			
			completed = reachedEnd || result;
			}
		else if (err != KErrNoMemory) // bad error code
			{
			completed = ETrue;
			result = EFail;
			}			

		//__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		if (completed)
			{
			break; // exit loop
			}

		failCount++;
		}

	failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc

	if (err != KErrNone || result)
		{
		TBuf<80> format;
		iAllocTestStepResult = EFail;
		if (result)
			{
			format.Format(_L("  Bad result with %d memory allocations tested\n"), failCount);
			}
		else
			{
			format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
			}
		Log(format);
		}
	else 
		{
		TBuf<80> format;
		format.Format(_L("  Completed OK with %d memory allocations tested\n"), failCount);
		Log(format);
		}

	return iAllocTestStepResult;
	}
