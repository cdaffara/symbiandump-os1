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
#include "OpenFileByHandle7908.h"

#include <caf/caf.h>

/**
 * Constructor
 */
CTestMmfAclntOpenFile7908::CTestMmfAclntOpenFile7908(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	: CTestMmfAclntOpenFile7902(aTestName, aSectName, aKeyName, aFormat, aCreateFile), iRecUtil(NULL),iPlayUtil(NULL)
	{
	}

CTestMmfAclntOpenFile7908* CTestMmfAclntOpenFile7908::NewL(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntOpenFile7908* self = new (ELeave) CTestMmfAclntOpenFile7908(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	return self;
	}

CTestMmfAclntOpenFile7908* CTestMmfAclntOpenFile7908::NewLC(const TDesC& aTestName, const TDesC& aSectName,const TDesC& aKeyName, const TTestFormat aFormat, const TBool aCreateFile)
	{
	CTestMmfAclntOpenFile7908* self = CTestMmfAclntOpenFile7908::NewL(aTestName,aSectName,aKeyName,aFormat,aCreateFile);
	CleanupStack::PushL(self);
	return self;
	}

/**
 * Open a file based clip and record, and then playback 
 * Performs alloc tests separately for each step in this test case
 * The 4 steps in this case are : 
 * 1> Create CMdaAudioRecorderUtility 
 * 2> Open CMdaAudioRecorderUtility and record a clip
 * 3> Create CMdaAudioPlayerUtility
 * 4> Open CMdaAudioRecorderUtility and playback the clip
 */
TVerdict CTestMmfAclntOpenFile7908::DoTestStepL()
	{
	INFO_PRINTF1( _L("TestRecorder : Record File"));
	TInt err = KErrNone;

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
		
	TInt failCount = 0;//total fail count for all the steps = sum of fail counts in each step
	TInt stepFailCount; //fail count in each step
	TVerdict iAllocTestStepResult = EPass; // XXX check?? assume pass
	
	err = PerformTestStep(1, stepFailCount);
	failCount += stepFailCount;
	if (err != KErrNone)
		{
		iAllocTestStepResult = EFail;
		TBuf<80> format;
		format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
		Log(format);
		return iAllocTestStepResult;
		}		
	
	TRAP(err, ExecuteStep1L());//Till step1 alloc tests are done, now keep the conditions ready 
							   //for performing alloc tests on step 2 i,e keep the iRecUtil 
							   //created and open for recording in this step
	if (err != KErrNone)
		{
		iAllocTestStepResult = EFail;
		TBuf<80> format;
		format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
		Log(format);
		return iAllocTestStepResult;
		}								  
	err = PerformTestStep(2, stepFailCount);	
	failCount += stepFailCount;
	delete iRecUtil;// Was created in ExecuteStep1L to be used for performing alloc tests in step 2 	
	if (err != KErrNone)
		{
		iAllocTestStepResult = EFail;
		TBuf<80> format;
		format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
		Log(format);
		return iAllocTestStepResult;
		}
		
	err = PerformTestStep(3, stepFailCount);
	failCount += stepFailCount;
	if (err != KErrNone)
		{
		iAllocTestStepResult = EFail;
		TBuf<80> format;
		format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
		Log(format);
		return iAllocTestStepResult;
		}
	
	TRAP(err, ExecuteStep3L());//Till step3 alloc tests are done, now keep the conditions ready 
							   //for performing alloc tests on step 4 i,e keep the iPlayUtil 
							   //created and open for playback in this step
	if(err != KErrNone)
		{
		iAllocTestStepResult = EFail;
		TBuf<80> format;
		format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
		Log(format);
		return iAllocTestStepResult;	
		}			
	err = PerformTestStep(4, stepFailCount);
	failCount += stepFailCount;
	delete iPlayUtil; // Was created in ExecuteStep3L to be used for performing alloc tests in step 4
		
	stepFailCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc
	
	if (err != KErrNone)
		{
		iAllocTestStepResult = EFail;
		TBuf<80> format;
		format.Format(_L("  Error(%d) with %d memory allocations tested\n"), err, failCount);
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
/*
 * TVerdict CTestMmfAclntOpenFile7908::PerformTestStep(TInt aStepNo, TInt& aStepFailCount)
 * Performs alloc tests on the step aStepNo in this test case
 */	
TInt CTestMmfAclntOpenFile7908::PerformTestStep(TInt aStepNo, TInt& aStepFailCount)	
	{
	TBool completed;
	TInt err;
	for(aStepFailCount = 1;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, aStepFailCount);
		__MM_HEAP_MARK;

//		INFO_PRINTF2(_L("CAudioRecorderUtility: Alloc Test Loop: %d"), failCount);
		//>>>>>>>>>>>>>>>>>>>>>>>>Test Method Call<<<<<<<<<<<<<<<<<<<<<<<<<<
		TVerdict verdict = EFail;

		switch( aStepNo )
			{
			case 1:
				TRAP(err, verdict = ExecuteStep1L());	
				if(err == KErrNone)
					{
					delete iRecUtil; //Created in ExecuteStep1L. We dont need it open anymore 						
					}
				break; 
			case 2:
				TRAP(err, verdict = ExecuteStep2L());	
				break; 
			case 3:
				TRAP(err, verdict = ExecuteStep3L());
				if(err == KErrNone)
					{
					delete iPlayUtil; //Created in ExecuteStep3L. We dont need it open anymore 						
					}
				break; 
			case 4:
				TRAP(err, verdict = ExecuteStep4L());	
				break; 				
			}
				
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
				completed = ETrue;
				aStepFailCount -= 1;
				}
			else
				{
				User::Free(testAlloc);	
				}			
			
			}
		else if (err != KErrNoMemory) // bad error code
			{
			completed = ETrue;
			}			
		
		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone, 0);

		if (completed)
			{
			break; // exit loop
			}

		aStepFailCount++;
		}//for
		return err;	
	}
/*
 * TVerdict CTestMmfAclntOpenFile7908::ExecuteStep1L()
 * Executes the statements for step 1 of this test case
 */
TVerdict CTestMmfAclntOpenFile7908::ExecuteStep1L()
	{
	iError = KErrTimedOut;
		
	iRecUtil = CMdaAudioRecorderUtility::NewL(*this);
	iError = KErrNone;
	return EPass;	
	}

/*
 * TVerdict CTestMmfAclntOpenFile7908::ExecuteStep2L()
 * Executes the statements for step 2 of this test case
 */
TVerdict CTestMmfAclntOpenFile7908::ExecuteStep2L()
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;
	
	User::LeaveIfError(iFs.Connect());
	CleanupClosePushL(iFs);
	User::LeaveIfError(iFs.ShareProtected());

	if(!GetStringFromConfig(iSectName, iKeyName, iFilename))
		{
		ret = EInconclusive;
		}

	User::LeaveIfError(iFile.Replace(iFs,iFilename,EFileWrite));
	CleanupClosePushL(iFile);
	iRecUtil->OpenFileL(iFile);
	CleanupClosePushL(*iRecUtil);
	CActiveScheduler::Start();

	if(iError == KErrNone)
		{
		iError = KErrTimedOut;
		iRecUtil->RecordL();
		CActiveScheduler::Start(); // open -> record
		
		User::After(500000);	// 0.5 sec
		iRecUtil->Stop();
		}
	CleanupStack::PopAndDestroy(3, &iFs);	//iFs, iFile, iRecUtil
	if(iError == KErrNone)
		{
		ret = EPass;
		}		
	return ret;	
	}
	
/*
 * TVerdict CTestMmfAclntOpenFile7908::ExecuteStep3L()
 * Executes the statements for step 3 of this test case
 */
 TVerdict CTestMmfAclntOpenFile7908::ExecuteStep3L()
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;
	
	iPlayUtil = CMdaAudioPlayerUtility::NewL(*this);
	iError = KErrNone;
	ret = EPass;
	return ret;
	}
	
/*
 * TVerdict CTestMmfAclntOpenFile7908::ExecuteStep4L()
 * Executes the statements for step 4 of this test case
 */
TVerdict CTestMmfAclntOpenFile7908::ExecuteStep4L()
	{
	TVerdict ret = EFail;
	iError = KErrTimedOut;	
	
	TRAPD(err, iPlayUtil->OpenFileL(iFilename));
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file for playback err = %d"), err);
		ret = EFail;
		}
	CActiveScheduler::Start();
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening file for playback iError = %d"), iError);
		ret = EFail;
		}
	if( iError == KErrNone )
		{
		iPlayUtil->Play();
		CActiveScheduler::Start();
		}
	iPlayUtil->Close();		
	if (iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error during playback of recorded file iError=%d"), iError);
		ret = EFail;
		}	
	if( iError == KErrNone ) 
		{
		RFile file;
		TInt size = 0;
		
		User::LeaveIfError(iFs.Connect());
		CleanupClosePushL(iFs);
		User::LeaveIfError(iFs.ShareProtected());
		User::LeaveIfError(file.Open(iFs,iFilename,EFileRead));
		CleanupClosePushL(file);
		User::LeaveIfError(file.Size(size));

		if(size > 0)
			{
			ret = EPass;
			}
		CleanupStack::PopAndDestroy(2, &iFs); //iFile, iFs
		}

	ERR_PRINTF2( _L("CMdaAudioRecorderUtility completed with error %d"),iError );
	if(iError == KErrNone)
		{
		User::After(500000);	
		}

	return	ret;
	}	
