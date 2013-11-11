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

#include "TestPlayer.h"
#include "OpenFileByHandle0036.h"

#include <caf/caf.h>
#include <ecom/ecom.h>

/**
 * Constructor
 */
CTestMmfVclntOpenFile0036::CTestMmfVclntOpenFile0036(const TDesC& aTestName,const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	:CTestMmfVclntOpenFile0034(aTestName, aSectName, aKeyName, aPlay)
	{
	}

CTestMmfVclntOpenFile0036* CTestMmfVclntOpenFile0036::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMmfVclntOpenFile0036* self = new (ELeave) CTestMmfVclntOpenFile0036(aTestName,aSectName,aKeyName,aPlay);
	return self;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
	}

CTestMmfVclntOpenFile0036* CTestMmfVclntOpenFile0036::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName,const TBool aPlay)
	{
	CTestMmfVclntOpenFile0036* self = CTestMmfVclntOpenFile0036::NewL(aTestName,aSectName,aKeyName,aPlay);
	CleanupStack::PushL(self);
	return self;
	}

/**
 * Load and initialise an audio file.
 */
TVerdict CTestMmfVclntOpenFile0036::DoTestStepL()
	{
	TVerdict iAllocTestStepResult=EPass;
	TInt err = KErrNone;
	TBool result = EFalse;
	
	InitWservL();

	
	//>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
	if( PerformTestStepL() != EPass )
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
	iAllocTestStepResult = EPass; // TODO check?? assume pass
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		__MM_HEAP_MARK;

		//INFO_PRINTF2(_L("CVideoPlayerUtility: Alloc Test Loop: %d"), failCount);
		//>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
		TVerdict verdict = EFail;
		TRAP(err, verdict = PerformTestStepL());
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

		__MM_HEAP_MARKEND;
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
		iAllocTestStepResult = EFail;
		TBuf<80> format;
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
