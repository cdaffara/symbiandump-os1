// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
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

// Test system includes
#include <testframework.h>

#include "TSU_MMFMIDICLNT.h"
#include "TS_MMFMIDICLNTsuite.h"

#include "TestStepMidiClntAllocFailNew.h"



// --------------------------------------------

/**
 *
 * Static constructor for CTestStepMidiClntAllocFailNew.
 *
 *
 * @return	"CTestStepMidiClntAllocFailNew*"
 *			The constructed CTestStepMidiClntAllocFailNew
 *
 * @xxxx
 * 
 */
CTestStepMidiClntAllocFailNew* CTestStepMidiClntAllocFailNew::NewL()
	{
	CTestStepMidiClntAllocFailNew* self = new(ELeave) CTestStepMidiClntAllocFailNew;
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
CTestStepMidiClntAllocFailNew::CTestStepMidiClntAllocFailNew()
	{
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-MIDICLNT-U-0100-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepMidiClntAllocFailNew::~CTestStepMidiClntAllocFailNew()
	{
	}

	// from MMidiClientUtilityObserver
void CTestStepMidiClntAllocFailNew::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/,const TTimeIntervalMicroSeconds& /*aTime*/,TInt /*aError*/)
	{
	}
void CTestStepMidiClntAllocFailNew::MmcuoTempoChanged(TInt /*aMicroBeatsPerMinute*/)
	{
	}
void CTestStepMidiClntAllocFailNew::MmcuoVolumeChanged(TInt /*aChannel*/,TReal32 /*aVolumeInDecibels*/)
	{
	}
void CTestStepMidiClntAllocFailNew::MmcuoMuteChanged(TInt /*aChannel*/,TBool /*aMuted*/)
	{
	}
void CTestStepMidiClntAllocFailNew::MmcuoSyncUpdate(const TTimeIntervalMicroSeconds& /*aMicroSeconds*/,TInt64 /*aMicroBeats*/)
	{
	}
void CTestStepMidiClntAllocFailNew::MmcuoMetaDataEntryFound(const TInt /*aMetaDataEntryId*/,const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	}
void CTestStepMidiClntAllocFailNew::MmcuoMipMessageReceived(const RArray<TMipMessageEntry>& /*aEntry*/)
	{
	}
void CTestStepMidiClntAllocFailNew::MmcuoPolyphonyChanged(TInt /*aNewPolyphony*/)
	{
	}
void CTestStepMidiClntAllocFailNew::MmcuoInstrumentChanged(TInt /*aChannel*/,TInt /*aBankId*/,TInt /*aInstrumentId*/)
	{
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
TVerdict CTestStepMidiClntAllocFailNew::DoTestStepL()
	{
	iTestStepResult = EPass;
    TInt err       = KErrNone;

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-MIDICLNT-U-0100-CP"));
	INFO_PRINTF1(_L("this is Alloc Failure test of initialising a CMidiClientUtility object with NewL()"));

	//__MM_HEAP_MARK;
	TRAP( err, iMidiClnt = CMidiClientUtility::NewL(*this) );
	delete iMidiClnt;
	iMidiClnt = NULL;
	//__MM_HEAP_MARKEND;

	if (err != KErrNone)
		return EFail;

	TInt failCount = 1;
	TBool completed = EFalse;
	TBool reachedEnd = EFalse; // Note: declare outside loop to help with debugging
	for(;;)	
		{
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		//__MM_HEAP_MARK;

		TRAP( err, (iMidiClnt = CMidiClientUtility::NewL(*this)) );

 		completed = EFalse;
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				reachedEnd = ETrue;
				failCount--; // -= 1;
				}
			else
				User::Free(testAlloc);
			// see if valid result and break if wrong - might be premature result
			if (iMidiClnt != NULL)
				{
				delete iMidiClnt;
				iMidiClnt = NULL;
				}
			completed = reachedEnd;
			}
		else if (err != KErrNoMemory) // bad error code
			completed = ETrue;

		//__MM_HEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone ,0);

		if (completed)
			break; // exit loop

		failCount++;
		}

	//failCount -= 1; // Failcount of 1 equates to 0 successful allocs, etc.

	if (err != KErrNone)
		{
		INFO_PRINTF3(_L("  Error(%d) with %d memory allocations tested"), err, failCount);
		iTestStepResult = EFail;
		}
	else 
		{
		INFO_PRINTF2(_L("  Completed OK with %d memory allocations tested"), failCount);
		iTestStepResult = EPass;
		}

	if (iMidiClnt != NULL)
		{
		delete iMidiClnt;
		iMidiClnt = NULL;
		}

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

