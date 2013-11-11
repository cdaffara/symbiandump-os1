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
#include "OpenFileByHandle7806.h"

CTestMmfAclntOpenFile7806::CTestMmfAclntOpenFile7806(const TDesC& aTestName, const TDesC& aSectName)
	: CTestMmfAclntOpenFile7804(aTestName, aSectName)
	{}

CTestMmfAclntOpenFile7806* CTestMmfAclntOpenFile7806::NewL(const TDesC& aTestName, const TDesC& aSectName)
	{
	CTestMmfAclntOpenFile7806* self = new (ELeave) CTestMmfAclntOpenFile7806(aTestName, aSectName);
	return self;
	}

/**
 * Open new file.
 */
TVerdict CTestMmfAclntOpenFile7806::DoTestL(CMdaAudioPlayerUtility* aPlayer)
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
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		__MM_HEAP_MARK;

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

CTestMmfAclntRepeatPlayAlloc::CTestMmfAclntRepeatPlayAlloc(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	: CTestMmfAclntOpenFile7806(aTestName, aSectName), iSectName(aSectName),iKeyName(aKeyName)
	{}

CTestMmfAclntRepeatPlayAlloc* CTestMmfAclntRepeatPlayAlloc::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName)
	{
	CTestMmfAclntRepeatPlayAlloc* self = new (ELeave) CTestMmfAclntRepeatPlayAlloc(aTestName, aSectName, aKeyName);
	return self;
	}

TVerdict CTestMmfAclntRepeatPlayAlloc::PerformTestL(CMdaAudioPlayerUtility* aPlayer)
	{
	INFO_PRINTF1( _L("TestPlayerUtils : OpenFileL(RFile&)/Play"));
	TInt repeatCount;
	TVerdict ret = EFail;
	iError = KErrNone;

	RFs fs;

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareProtected());

	TBuf<KSizeBuf>	filename;
	TPtrC			filename1; 
	RFile			file;

	if(!GetStringFromConfig(iSectName, iKeyName, filename1))
		{
		return EInconclusive;
		}

	GetDriveName(filename);
	filename.Append(filename1);

	User::LeaveIfError( file.Open( fs, filename, EFileRead ) );
	CleanupClosePushL(file);
	if(!GetIntFromConfig(_L("SectionRepeatCount"), _L("numOfRepeat"), repeatCount))
		{
		return EInconclusive;
		}
	aPlayer->SetRepeats(repeatCount, TTimeIntervalMicroSeconds(2));
	aPlayer->OpenFileL(file);
	CActiveScheduler::Start();
	
	if (iError == KErrNone)
		{
		aPlayer->Play();
		CActiveScheduler::Start();
		}

	if (iError == KErrNone)
		{
		ret = EPass;
		}
	
	aPlayer->Close();	
	CleanupStack::PopAndDestroy(2, &fs);

	return ret;
	}
