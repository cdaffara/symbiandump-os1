// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <testframework.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidPlayer.h"


//
// Literals 
//
//_LIT(KConfigFile, "video.ini");							// config file for this test - TODO use script to load video.ini
_LIT(KSectionDrm, "SectionDRM");						// video.ini section
_LIT(KEntryFileRights, "VideoContent");						// video.ini filename entry
_LIT(KEntryFileNoRights, "VideoContentNoRights");						// video.ini filename entry
_LIT(KDefaultRightsUniqueId, "LongVideoId");	// unique Id for this content

/**
 *
 * Static constructor for CTestStepVidPlayerDrm.
 *
 * @param	The name of this test step.
 *
 * @return	"CTestStepVidPlayerDrm*"
 *			The constructed CTestStepVidPlayerDrm object.
 *
 * @xxxx
 * 
 */
CTestStepVidPlayerDrm* CTestStepVidPlayerDrm::NewL(const TDesC& aTestName, TSource aSource, TTest aTest)
	{
	CTestStepVidPlayerDrm* self = new(ELeave) CTestStepVidPlayerDrm(aTestName, aSource, aTest, EFalse);
	return self;
	}
	
	
CTestStepVidPlayerDrm* CTestStepVidPlayerDrm::NewAllocTestL(const TDesC& aTestName, TSource aSource, TTest aTest)
	{
	CTestStepVidPlayerDrm* self = new(ELeave) CTestStepVidPlayerDrm(aTestName, aSource, aTest, ETrue);
	return self;
	}


/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @param	The name of this test step.
 * 
 */
CTestStepVidPlayerDrm::CTestStepVidPlayerDrm(const TDesC& aTestName, TSource aSource, TTest aTest, TBool aDoAllocTest) 
	{
	// This is the test case name that is used by the script file.
	iTestStepName = aTestName;
	iTest = aTest;
	iSource = aSource;
	iDoAllocTest = aDoAllocTest;
	iError = KErrNone;
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidPlayerDrm::~CTestStepVidPlayerDrm()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidPlayerDrm::MvpuoOpenComplete(TInt aError)
	{
	iError = aError;
	
	INFO_PRINTF2(_L("Open complete, error code: %d"), aError);
	
	
	switch (iTest)
		{
		case EProtectedPlayIntent:
			if (iError == KErrNone)
				{
				iState = EPrepare;
				NextState();
				}
			else
				{
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}
			break;

		case EProtectedUnknownIntent:
			if (iError == KErrCANoRights || iError == KErrCANoPermission)
				{
				iTestStepResult = EPass;
				iState = EStop;
				NextState();
				}
			else
				{
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}
			break;

		case EProtectedPlayNoRights:
			if (iError == KErrCANoRights)
				{
				iState = EStop;
				NextState();
				}
			else
				{
				iTestStepResult = EFail;
				CActiveScheduler::Stop();
				}

			break;
		
		default:
			ASSERT(EFalse);
			break;
		}

	
	}

void CTestStepVidPlayerDrm::MvpuoFrameReady(CFbsBitmap& /*aFrame*/, TInt /*aError*/)
	{
	INFO_PRINTF1(_L("MvpuoFrameReady callback"));
	}

void CTestStepVidPlayerDrm::MvpuoPrepareComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("Prepare complete %d"), aError);

	if (iError == KErrNone)
		{
		iState = EPlay;
		NextState();
		}
	else
		{
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}

	}

void CTestStepVidPlayerDrm::MvpuoPlayComplete(TInt aError)
	{
	iError = aError;
	INFO_PRINTF2(_L("Play complete %d"), aError);

	if (iError == KErrNone)
		{
		iTestStepResult = EPass;
		iState = EStop;
		NextState();
		}
	else
		{
		iTestStepResult = EFail;
		CActiveScheduler::Stop();
		}

	}
	
void CTestStepVidPlayerDrm::MvpuoEvent(const TMMFEvent& /*aEvent*/)
	{
	INFO_PRINTF1(_L("MvpuoEvent callback"));
	}

TVerdict CTestStepVidPlayerDrm::GetConfig()
	{
	TVerdict verdict = EPass;

	switch (iTest)
		{
	case EProtectedPlayNoRights: 
		if (!GetStringFromConfig(KSectionDrm, KEntryFileNoRights, iFileName))
			{
			INFO_PRINTF1(_L("File name not found in INI file."));
			verdict = EInconclusive;
			}
		break;
	default: 
		if (!GetStringFromConfig(KSectionDrm, KEntryFileRights, iFileName))
			{
			INFO_PRINTF1(_L("File name not found in INI file."));
			verdict = EInconclusive;
			}
		break;	
		}

	return verdict;
	}
	
TInt CTestStepVidPlayerDrm::OpenFile()
	{
	TRect rect, clipRect;
	TRAPD(err, iPlayer = CVideoPlayerUtility::NewL(*this, 
												   EMdaPriorityNormal, 
												   EMdaPriorityPreferenceTimeAndQuality,
										           iWs, 
										           *iScreen, 
										           *iWindow, 
										           rect, 
										           clipRect));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Unable to create video player, error %d"), err);
		}
	else
		{
		ContentAccess::TIntent intent;
		switch (iTest)
			{
		case EProtectedPlayIntent:
			intent = ContentAccess::EPlay;
			break;
		case EProtectedUnknownIntent:
			intent = ContentAccess::EUnknown;
			break;
		case EProtectedPlayNoRights:
			intent = ContentAccess::EPlay;
			break;
		default:
			ASSERT(EFalse); // we shouldn't be here
			intent = ContentAccess::EUnknown;
			};
			
		switch (iSource)
			{
		case EFileSource:
				{
				TMMFileSource tfs(iFileName, KDefaultRightsUniqueId, intent);
				TRAP(err, iPlayer->OpenFileL(tfs));
				break;
				}
		case EFileHandleSource:
				{
				TMMFileHandleSource tfhs(iFile, KDefaultRightsUniqueId, intent);
				TRAP(err, iPlayer->OpenFileL(tfhs));
				break;
				}
			}

		if (err != KErrNone)
			{
			INFO_PRINTF3(_L("Unable to open file %S, error %d"), &iFileName, err);
			}
		else
			{
			INFO_PRINTF2(_L("Opened file %S"), &iFileName);
			}
		}
		
	return err;
	}
	

void CTestStepVidPlayerDrm::CheckVerdict(const TDesC& aName)
	{
	if (iTestStepResult != EPass)
		{
		INFO_PRINTF3(_L("%S iError = %d"), &aName, iError);
	
		// Stop the test run immediately on fail.
		// Note: this causes a return from CActiveScheduler::Start()
		//       in DoTestStepL().
		CActiveScheduler::Stop();
		}
	}
	
/**
 *
 * Run the next test step as defined by the state machine.
 *
 * Test steps are usually asynchronous and the result is checked in the callback.
 * In certain cases it may also be necessary to check a synchronous result.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
void CTestStepVidPlayerDrm::RunStepL()
	{
	iError = KErrNone;
	User::LeaveIfError(OpenFile());
	CActiveScheduler::Start();
	delete iPlayer;
	iPlayer = NULL;
	}
	
/**
 *
 * Run the next test step as defined by the state machine.
 *
 * Test steps are usually asynchronous and the result is checked in the callback.
 * In certain cases it may also be necessary to check a synchronous result.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
void CTestStepVidPlayerDrm::NextState()
	{
	switch (iState)
		{
		case EPrepare:
		iPlayer->Prepare();
		break;
		
		case EPlay:
		iPlayer->Play();
		break;
		
		case EStop:
		// Note: this causes a return from CActiveScheduler::Start()
		//       in DoTestStepL().
		CActiveScheduler::Stop();
		break;
		}

	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidPlayerDrm::DoTestStepPreambleL(void)
	{
	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	if (verdict == EPass)
		{
		// Printing to the console and log file
		INFO_PRINTF2(_L("%S"), &iTestStepName);
		INFO_PRINTF1(_L("This test is opening a protected content file with CVideoPlayerUtility::OpenFileSourceL()"));

		TRAPD(err , InitWservL());
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Unable to initialise window server, error %d"), err);
			verdict = EInconclusive;
			}
		}

	if (verdict == EPass)
		{
		verdict = GetConfig();
		}
		
	if (iSource == EFileHandleSource)
		{
		iFs.Connect();
		iFs.ShareProtected();
		User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead));
		}
	
	return verdict;
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidPlayerDrm::DoTestStepL()
	{
	
	if (iDoAllocTest)
		{
		
		return DoTestStepAllocL();
		}
	else
		{
		RunStepL();
		}
	
		
	INFO_PRINTF1(_L("Finished with this test step"));

	// test steps return a result
	return iTestStepResult;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidPlayerDrm::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	
	iFile.Close();
	iFs.Close();

	UnloadConfig();	// TODO use script to load video.ini
	
	// Destroy the scheduler
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}
	
	
TVerdict CTestStepVidPlayerDrm::DoTestStepAllocL()
    {
    INFO_PRINTF1(_L("Alloc test to open a protected video clip"));

	
    iTestStepResult  = EPass;

    TInt failCount   = 1;
    TBool completed  = EFalse;
    TBool badResult  = EFalse;
    TBool reachedEnd = EFalse;
    TInt error = KErrNone;

    while (!completed)
        {
        INFO_PRINTF2(_L("fail count %d"), failCount);
        __UHEAP_SETFAIL(RHeap::EFailNext, failCount);
        __MM_HEAP_MARK;

		       
        TRAP(error, RunStepL());
        ASSERT(error == KErrNone || iError == KErrNone);
        if (error != KErrNone)
        	iError = error;
        	
        completed = EFalse;
        if (iError == KErrNone)
            {
            
            TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
            if (testAlloc == NULL)
                {
                reachedEnd = ETrue;
                failCount--;
                }
            else
                {
                User::Free(testAlloc);
                testAlloc = NULL;
                }
  


            if (iTestStepResult != EPass)
                {
                badResult = ETrue;
                }

            completed = reachedEnd || badResult;
            }
        else if (iError != KErrNoMemory)
            {
            __UHEAP_SETFAIL(RHeap::ENone, 0);
            ERR_PRINTF2(_L("Bad error code %d"), iError);
            completed = ETrue;
            badResult = EFail;
            }

        __MM_HEAP_MARKEND;
        __UHEAP_SETFAIL(RHeap::ENone, 0);

        failCount++;
        }

    if (iError != KErrNone || badResult)
        {
        if (badResult)
            {
            INFO_PRINTF3(_L("Bad result with %d memory allocations tested. The Error returned is %d."), failCount, iError);
            }
        else
            {
            INFO_PRINTF3(_L("Error(%d) with %d memory allocations tested."), failCount, iError);
            }

        iTestStepResult = EFail;
        }
    else
        {
        INFO_PRINTF2(_L("Completed OK with %d memory allocations tested.\n"), failCount);
        iTestStepResult = EPass;
        }

    return iTestStepResult;
    }


