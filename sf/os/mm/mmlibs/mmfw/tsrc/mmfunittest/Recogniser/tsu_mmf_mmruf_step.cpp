// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <f32file.h>
#include "tsu_mmf_mmruf_step.h"

static const TInt KTestStepNameSize = 22; // MM-MMF-MMRUF-U-xxxx-HP

/*
**
*/
RMMRUFTestStep::RMMRUFTestStep(const TTestSpec* aTestSpec)
 : 	iTestSpec(aTestSpec)
	{
	ASSERT(iTestSpec != NULL);

	_LIT(KTestStep, "MM-MMF-MMRUF-U-%04d-HP");

	TBuf<KTestStepNameSize> temp;
	temp.Zero();
	temp.AppendFormat(KTestStep, iTestSpec->iTestNumber);
	iTestStepName = temp;
	}

/*
**
*/
RMMRUFTestStep::~RMMRUFTestStep()
	{
	Close();
	}

/*
**
*/
RMMRUFTestStep* RMMRUFTestStep::NewL(const TTestSpec* aTestSpec)
	{
	return new(ELeave) RMMRUFTestStep(aTestSpec);
	}

/*
**
*/
TVerdict RMMRUFTestStep::DoTestStepPreambleL()
	{
	User::LeaveIfError(iAppArc.Connect());
	return EPass;
	}
	
/*
**
*/
TVerdict RMMRUFTestStep::DoTestStepPostambleL()
	{
	iAppArc.Close();
	return EPass;
	}

/*
**
*/
TVerdict RMMRUFTestStep::DoTestStepL()
	{
	iTestStepResult = EFail;
	TPtrC filename(iTestSpec->iFileName);
	INFO_PRINTF2(_L("Testing file: %S"), &filename);
	
	RFile file;
	TInt err = file.Open(iAppArc.FileServer(), filename, EFileRead | EFileShareAny);
	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("RFile.Open(\"%S\") failed: %d"), &filename, err);
		User::Leave(err);
		}
	
	CleanupClosePushL(file);
	TDataRecognitionResult dataType;
	err = iAppArc.Recognise(file, dataType);
	if (err != KErrNone)
		{
		iTestStepResult = EInconclusive;
		INFO_PRINTF2(_L("RApaLsSession.RecognizeData() failed: %d"), err);
		User::Leave(err);
		}
	
	CleanupStack::Pop(); // file
		
	// Check the results.
	TPtrC8 resMime(dataType.iDataType.Des8());
	TPtrC8 expectedMime(iTestSpec->iExpectedMimeType);
	
	if (expectedMime.Compare(resMime) == 0)
		{
		if (iTestSpec->iTestNumber >= 600)
			{
			//  Negative testing above 600
			//	These are negative tests. So Mime type must be empty
			if (resMime.Compare(_L8("")) == 0)
				{
				iTestStepResult = EPass;
				return iTestStepResult;
				}
			}
		// DEF129505 - MMRuf incorrectly recognises unicode text files as MP3 files
		else if (iTestSpec->iTestNumber == 500)
			{
			iTestStepResult = EPass;
			return iTestStepResult;
			}
						
		// Set the result to EFail initially and change it to EPass
		// further down in the code after checking the expected confidence.
		iTestStepResult = EFail;
		TBuf<64> confidence;
		switch (dataType.iConfidence)
			{
			case KConfCertain:
				confidence.Copy(_L("KConfCertain"));
				break;
			case KConfProbable:
				confidence.Copy(_L("KConfProbable"));
				break;
			case KConfPossible:
				confidence.Copy(_L("KConfPossible"));
				break;
			case KConfNotRecognised:
				confidence.Copy(_L("KConfNotRecognised"));
				break;
			default:
				// Safe Guard: This will never happen
				break;	
			}
		
		// #############################################################
		// SPECIAL CASES: There are some formats that are never recognised
		// with confidence = ECertain. Deal with these here.
		switch (iTestSpec->iTestNumber)
			{
			case 55:	// RAM.
			case 115:	// SDP
			case 116:	// SDP
			case 117:	// SDP
			case 118:	// SDP
			case 119:	// SDP
			case 120:	// SDP
			case 121:	// SDP
			case 122:	// SDP
				if (dataType.iConfidence != KConfProbable)
					{
					INFO_PRINTF2(_L("Bad confidence level: %S, expected KConfProbable"), &confidence);
					}
				else
					{
					iTestStepResult = EPass;
					}
					
				return iTestStepResult;
				
			default:
				// Deliberate fall-through.
				break;
			}
			

		// Mod of 3 cos there are 3 test cases for each format
		TInt res = iTestSpec->iTestNumber % 3;
		switch (res)
			{
			case 1:
				// 1st test
				// Buffer Data and file extension are correct.
				if (dataType.iConfidence == KConfCertain)
					{
					iTestStepResult = EPass;
					}
				else
					{
					INFO_PRINTF2(_L("Returned Wrong Confidence Value: %S, expected KConfCertain"), &confidence);
					}	
				break;
				
			case 2:
				// 2nd test
				// Buffer data is correct, but the file extension is incorrect.
				if (dataType.iConfidence >= KConfProbable)
					{
					iTestStepResult = EPass;
					}
				else
					{
					INFO_PRINTF2(_L("Returned Wrong Confidence Value: %S, expected KConfProbable"), &confidence);
					}		
				break;
				
			case 0:
				// 3rd test	
				// Buffer data is unknown and file extension exists.
				if (dataType.iConfidence >= KConfPossible)
					{
					iTestStepResult = EPass;
					}
				else
					{
					INFO_PRINTF2(_L("Returned Wrong Confidence Value: %S, expected KConfPossible"), &confidence);
					}		
				break;

			default:
				// Safe Guard: This must never happen
				break;
			}
		}
	else
		{
		TBuf<KMaxDataTypeLength> returned;
		returned.Copy(dataType.iDataType.Des());
		INFO_PRINTF2(_L("- wrong returned MIME-type: %S"), &returned);
		iTestStepResult = EFail;
		}
	return iTestStepResult;
	}


//
// OOM Tests
//

/*
**
*/
RTestStepMMRUFOOM* RTestStepMMRUFOOM::NewL(const TTestSpec* aTestSpec)
	{
	return new(ELeave) RTestStepMMRUFOOM(aTestSpec);
	}

/*
**
*/
RTestStepMMRUFOOM::RTestStepMMRUFOOM(const TTestSpec* aTestSpec)
	:iTestSpec(aTestSpec)
	{
	ASSERT(iTestSpec != NULL);

	_LIT(KAllocTestStep, "MM-MMF-MMRUF-U-%04d-CP");

	TBuf<KTestStepNameSize> temp;
	temp.Zero();
	temp.AppendFormat(KAllocTestStep, iTestSpec->iTestNumber);
	iTestStepName = temp;
	}
	
/*
**
*/
TVerdict RTestStepMMRUFOOM::DoTestStepPreambleL()
	{
	User::LeaveIfError(iAppArc.Connect());
	return EPass;
	}

/*
**
*/
TVerdict RTestStepMMRUFOOM::DoTestStepPostambleL()
	{
	iAppArc.Close();
	return EPass;
	}

/*
**
*/
TVerdict RTestStepMMRUFOOM::DoTestStepL()
	{
    TInt err;
	TDataRecognitionResult dataType;
	TPtrC filename(iTestSpec->iFileName);
	RFile file;
	TInt failCount = 1;
	TBool completed;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	
	
	iTestStepResult = EFail;
	err = file.Open(iAppArc.FileServer(), filename, EFileRead | EFileShareAny);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("RFile.Open() failed: %d"), err);
		User::Leave(err);
		}
	CleanupClosePushL(file);
		
	FOREVER
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);

		iTestStepResult = EFail;
		// We will check 1000 Allocations as this is a non leaving function.
		err = iAppArc.Recognise(file, dataType);
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("RApaLsSession.RecognizeData() failed: %d"), err);
			iTestStepResult = EInconclusive;
			}

		completed = EFalse;
		if (err == KErrNone)
			{
			if (iTestStepResult == EPass)
				{
				TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
				if (testAlloc==NULL)
					{
					reachedEnd = ETrue;
					failCount--; // -= 1;
					}
				else
					{
					User::Free(testAlloc);
					}
				completed = reachedEnd;
				}
			else
				{
				// the test failed but we have already proved above
				// that the required plugins are present
				// therefore we assume that the interface returned
				// a NULL pointer and so we can continue to test
				// until the timeout or the fail count exceeds
				completed = EFalse;
				}
			}
		else if (err == KErrNotSupported)
			{
			// the test has returned not supported but we already
			// know that the interface *is* supported so we assume
			// that the interface creation failed due to lack of
			// memory and so we continue to test until the timeout
			// or we exceed the fail count
			completed = EFalse;
			}
		else if (err != KErrNoMemory) // bad error code
			{
			completed = ETrue;
			}
				
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		// if we have completed successfully or we have exceeded
		// a 1000 failed ALLOC attempts then quit
		if (completed || failCount > 1000)
			{
			break; // exit loop
			}

		failCount++;
		}

	// finished with test
	CleanupStack::Pop(); //file
	
	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		User::LeaveIfError(err);
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}
	
	TPtrC8 resMime(dataType.iDataType.Des8());
	TPtrC8 expectedMime(iTestSpec->iExpectedMimeType);
	
	TBuf<KMaxDataTypeLength> returned;
	returned.Copy(dataType.iDataType.Des());
		
	if (expectedMime.Compare(resMime) == 0)
		{
		INFO_PRINTF2(_L("Matched Expected MIME-type: %S"), &returned);
		iTestStepResult = EPass;
		}
	else 
		{
		INFO_PRINTF2(_L("- wrong returned MIME-type: %S"), &returned);
		iTestStepResult = EFail;
		}
	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

