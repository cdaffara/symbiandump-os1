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
// TestStepVidRecorderOpen.cpp
// 
//

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

#include "CmmfVideoTestControllerUIDs.hrh"

#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>

#include <mda/common/video.h>
#include <mmf/common/mmfcontroller.h>
#include <videoplayer.h>
#include <videorecorder.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"
#include "TS_Codes.h"

// Specific includes for these test steps
#include "TestStepVidRecorder.h"

_LIT(KTestFileName,"C:\\mm\\mmf\\testfiles\\vclnt\\test.dummy");
//_LIT(KClockFileName,"C:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy");	// EABI warning removal

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAllocFailVidRecorderOpenFile.
 *
 *
 * @return	"CTestStepAllocFailVidRecorderOpenFile*"
 *			The constructed CTestStepAllocFailVidRecorderOpenFile
 *
 * @xxxx
 * 
 */
CTestStepAllocFailVidRecorderOpenFile* CTestStepAllocFailVidRecorderOpenFile::NewL(TBool aUseFileHandle)
	{
	CTestStepAllocFailVidRecorderOpenFile* self = new(ELeave) CTestStepAllocFailVidRecorderOpenFile(aUseFileHandle);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepAllocFailVidRecorderOpenFile::CTestStepAllocFailVidRecorderOpenFile(TBool aUseFileHandle) 
	: CTestStepVidRecorderOpenFile(aUseFileHandle)
	{
	iTestStepResult = EPass;
	if (aUseFileHandle)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0311-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0301-CP");
		}
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
TVerdict CTestStepAllocFailVidRecorderOpenFile::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this alloc failure test is openning a file with a CVideoRecorderUtility::OpenFileL()"));

	TInt err = KErrNone;
	iError = KErrNone; //KErrTimedOut;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging

	// Don't record over clock.dummy
	iFileName.Set(KTestFileName);

	for(;;)
		{
		InitWservL();

		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP(err,iRecorder = CVideoRecorderUtility::NewL(*this));

		if (iRecorder != NULL && !RecorderStateStopped() )
			return EFail;
		
		if(iRecorder != NULL)
			TRAP(err, RecOpenAndStartSchedulerL() );

		completed = EFalse;
		if ((err == KErrNotFound) || ( (err == KErrNotSupported || err == KErrNone) &&
					  (iError == KErrNotSupported || iError == KErrNone) ) )
		{ //possible pass - check memory alloc state
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				User::Free(testAlloc);
			// see if valid result and break if wrong - might be premature result
			
			if (iRecorder == NULL)
				badResult = ETrue;

			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;


		delete iRecorder;
		iRecorder = NULL;
		
		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		DeInitWserv();

		if (completed)
			break; // exit loop

		failCount++;
		}



	if (err != KErrNone || badResult)
		{
		if (badResult)
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
		else
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}	

	
	INFO_PRINTF1(_L("finished with this test step"));
	__MM_HEAP_MARKEND;

	// test steps return a result
	return iTestStepResult;
	}

//
//------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAllocFailVidRecorderOpenFileCtrlUid.
 *
 *
 * @return	"CTestStepAllocFailVidRecorderOpenFileCtrlUid*"
 *			The constructed CTestStepAllocFailVidRecorderOpenFileCtrlUid
 *
 * @xxxx
 * 
 */
CTestStepAllocFailVidRecorderOpenFileCtrlUid* CTestStepAllocFailVidRecorderOpenFileCtrlUid::NewL(TBool aUseFileHandle)
	{
	CTestStepAllocFailVidRecorderOpenFileCtrlUid* self = new(ELeave) CTestStepAllocFailVidRecorderOpenFileCtrlUid(aUseFileHandle);
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepAllocFailVidRecorderOpenFileCtrlUid::CTestStepAllocFailVidRecorderOpenFileCtrlUid(TBool aUseFileHandle) 
	: CTestStepVidRecorderOpenFileCtrlUid(aUseFileHandle)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	if (aUseFileHandle)
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0312-CP");
		}
	else
		{
		iTestStepName = _L("MM-MMF-VCLNT-U-0304-CP");
		}
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
TVerdict CTestStepAllocFailVidRecorderOpenFileCtrlUid::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF2(_L("%S"), &iTestStepName);
	INFO_PRINTF1(_L("this alloc failure test is openning a file with a specific Controller Uid: CVideoRecorderUtility::OpenFileL()"));

	TInt err = KErrNone;
	iError = KErrNone; //KErrTimedOut;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging

	// Don't record over clock.dummy
	iFileName.Set(KTestFileName);

	for(;;)
		{
		InitWservL();

		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP(err,iRecorder = CVideoRecorderUtility::NewL(*this));


		if (iRecorder != NULL && !RecorderStateStopped() )
			return EFail;

		if(iRecorder != NULL)
			TRAP(err, RecOpenAndStartSchedulerL(iUseFileHandle) );


		completed = EFalse;
		if (err == KErrNotFound || err == KErrNone) 
		{ //possible pass - check memory alloc state
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				User::Free(testAlloc);
			// see if valid result and break if wrong - might be premature result
			
			if (iRecorder == NULL)
				badResult = ETrue;

			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		delete iRecorder;
		iRecorder = NULL;

		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		DeInitWserv();

		if (completed)
			break; // exit loop

		failCount++;
		}


	if (err != KErrNone || badResult)
		{
		if (badResult)
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
		else
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}	

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAllocFailVidRecorderOpenDesc.
 *
 *
 * @return	"CTestStepAllocFailVidRecorderOpenDesc*"
 *			The constructed CTestStepAllocFailVidRecorderOpenDesc
 *
 * @xxxx
 * 
 */
CTestStepAllocFailVidRecorderOpenDesc* CTestStepAllocFailVidRecorderOpenDesc::NewL()
	{
	CTestStepAllocFailVidRecorderOpenDesc* self = new(ELeave) CTestStepAllocFailVidRecorderOpenDesc;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepAllocFailVidRecorderOpenDesc::CTestStepAllocFailVidRecorderOpenDesc() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0302-CP");
	iHeapSize = 128000;
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
TVerdict CTestStepAllocFailVidRecorderOpenDesc::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0302-CP"));
	INFO_PRINTF1(_L("this alloc failure test is openning a descriptor with a CVideoRecorderUtility::OpenDesL()"));

	TInt err = KErrNone;
	iError = KErrNone; //KErrTimedOut;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging

	for(;;)
		{
		InitWservL();

		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP(err,iRecorder = CVideoRecorderUtility::NewL(*this));

		if (iRecorder != NULL && !RecorderStateStopped() )
			return EFail;

		if(iRecorder != NULL)
			TRAP(err, OpenDescAndStartSchedulerL() );

		completed = EFalse;
		if ((err == KErrNotFound) || ( (err == KErrNotSupported || err == KErrNone) &&
					  (iError == KErrNotSupported || iError == KErrNone) ) )
		{ //possible pass - check memory alloc state
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				User::Free(testAlloc);
			// see if valid result and break if wrong - might be premature result
			
			if (iRecorder == NULL)
				badResult = ETrue;

			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		delete iRecorder;
		iRecorder = NULL;


		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		DeInitWserv();

		if (completed)
			break; // exit loop

		failCount++;
		}



	if (err != KErrNone || badResult)
		{
		if (badResult)
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
		else
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}	


	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepAllocFailVidRecorderOpenUrl.
 *
 *
 * @return	"CTestStepAllocFailVidRecorderOpenUrl*"
 *			The constructed CTestStepAllocFailVidRecorderOpenUrl
 *
 * @xxxx
 * 
 */
CTestStepAllocFailVidRecorderOpenUrl* CTestStepAllocFailVidRecorderOpenUrl::NewL()
	{
	CTestStepAllocFailVidRecorderOpenUrl* self = new(ELeave) CTestStepAllocFailVidRecorderOpenUrl;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepAllocFailVidRecorderOpenUrl::CTestStepAllocFailVidRecorderOpenUrl() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0303-CP");
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
TVerdict CTestStepAllocFailVidRecorderOpenUrl::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0303-CP"));
	INFO_PRINTF1(_L("this alloc failure test is openning a file with a CVideoRecorderUtility::OpenUrlL()"));


	TInt err = KErrNone;
	iError = KErrNone; //KErrTimedOut;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool badResult = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging

	for(;;)
		{
		InitWservL();

		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP(err,iRecorder = CVideoRecorderUtility::NewL(*this));

		if (iRecorder != NULL && !RecorderStateStopped() )
			return EFail;

		if(iRecorder != NULL)
			TRAP(err, OpenUrlAndStartSchedulerL() );

		completed = EFalse;
		if (err == KErrNotFound || err == KErrNone) 
		{ //possible pass - check memory alloc state
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				User::Free(testAlloc);
			// see if valid result and break if wrong - might be premature result
			
			if (iRecorder == NULL)
				badResult = ETrue;

			completed = reachedEnd || badResult;
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		delete iRecorder;
		iRecorder = NULL;


		__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		DeInitWserv();

		if (completed)
			break; // exit loop

		failCount++;
		}



	if (err != KErrNone || badResult)
		{
		if (badResult)
			INFO_PRINTF2(_L("  Bad result with %d memory allocations tested"), failCount);
		else
			INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}	




	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

